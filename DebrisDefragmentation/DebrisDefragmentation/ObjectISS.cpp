﻿#include "ObjectISS.h"
#include "ISSModel.h"
#include "Physics.h"

ObjectISS::ObjectISS()
{
}

ObjectISS::~ObjectISS()
{
}


void ObjectISS::Init( )
{
	InitCollisionBox( );

	m_CharacterModel = ISSModel::Create( L"iss.x" );
	AddChild( m_CharacterModel );
}

void ObjectISS::UpdateItSelf( float dTime )
{
	Physics::CalcCurrentPosition( &m_Position, &m_RigidBody.m_Velocity, m_RigidBody.m_Acceleration, dTime );
}

void ObjectISS::RenderItSelf( )
{
	// 조심해!!
	// 나중에 ISSModule이 ISS의 m_Matrix를 참조할 수 있도록 변경할 것
	std::for_each( m_ModuleList.begin( ), m_ModuleList.end( ),
		[&]( ISSModule &eachModule )
	{
		eachModule.SetMatrix( m_Matrix );
		DrawModuleCollisionBox( eachModule );
	}
	);
}


void ObjectISS::InitCollisionBox( )
{
	// 각각의 모듈을 초기화
	m_ModuleList[0].Init( ISSModuleName::PART0 );
	m_ModuleList[1].Init( ISSModuleName::PART1 );
	m_ModuleList[2].Init( ISSModuleName::PART2 );
	m_ModuleList[3].Init( ISSModuleName::PART3 );
	m_ModuleList[4].Init( ISSModuleName::PART4 );
	m_ModuleList[5].Init( ISSModuleName::PART5 );
	m_ModuleList[6].Init( ISSModuleName::PART6 );
	m_ModuleList[7].Init( ISSModuleName::PART7 );
	m_ModuleList[8].Init( ISSModuleName::PART8 );
	m_ModuleList[9].Init( ISSModuleName::PART9 );
}

void ObjectISS::DrawModuleCollisionBox( ISSModule & module )
{
	LPDIRECT3DDEVICE9 pD3DDevice = DDRenderer::GetInstance( )->GetDevice( );

	const CollisionBox* currentBox = module.GetCollisionBox();

	D3DXVECTOR4 tempMat;
	D3DXVECTOR3 currentPos;
	D3DXVECTOR3 currentPoint[8];
	D3DXVECTOR3 currentAxis[3];

	// 각 점 좌표
	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[0], &m_Matrix );
	currentPoint[0] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[1], &m_Matrix );
	currentPoint[1] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[2], &m_Matrix );
	currentPoint[2] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[3], &m_Matrix );
	currentPoint[3] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[4], &m_Matrix );
	currentPoint[4] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[5], &m_Matrix );
	currentPoint[5] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[6], &m_Matrix );
	currentPoint[6] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	D3DXVec3Transform( &tempMat, &currentBox->m_PointList[7], &m_Matrix );
	currentPoint[7] = D3DXVECTOR3( tempMat.x, tempMat.y, tempMat.z );

	// 그리기
	D3DXMATRIX projMat, viewMat;
	pD3DDevice->GetTransform( D3DTS_PROJECTION, &projMat );
	pD3DDevice->GetTransform( D3DTS_VIEW, &viewMat );
	viewMat *= projMat;

	ID3DXLine *Line;

	if ( D3DXCreateLine( pD3DDevice, &Line ) != D3D_OK )
		return;
	Line->SetWidth( 0.5 );
	Line->SetAntialias( true );

	D3DXVECTOR3 point[11];
	point[0] = currentPoint[0];
	point[1] = currentPoint[1];
	point[2] = currentPoint[2];
	point[3] = currentPoint[3];
	point[4] = currentPoint[0];
	point[5] = currentPoint[4];
	point[6] = currentPoint[5];
	point[7] = currentPoint[6];
	point[8] = currentPoint[7];
	point[9] = currentPoint[4];
	Line->Begin( );

	Line->DrawTransform( point, 10, &viewMat, D3DXCOLOR( 0.2f, 0.6f, 1.0f, 1.0f ) );

	// 비어 있는 선 그리기
	D3DXVECTOR3 pointT[2];
	pointT[0] = currentPoint[7];
	pointT[1] = currentPoint[3];
	Line->DrawTransform( pointT, 2, &viewMat, D3DXCOLOR( 0.2f, 0.6f, 1.0f, 1.0f ) );

	pointT[0] = currentPoint[6];
	pointT[1] = currentPoint[2];
	Line->DrawTransform( pointT, 2, &viewMat, D3DXCOLOR( 0.2f, 0.6f, 1.0f, 1.0f ) );

	pointT[0] = currentPoint[5];
	pointT[1] = currentPoint[1];
	Line->DrawTransform( pointT, 2, &viewMat, D3DXCOLOR( 0.2f, 0.6f, 1.0f, 1.0f ) );

	Line->End( );
	Line->Release( );
}

void ObjectISS::SetOwner( int moduleIdx, int color )
{
	m_ModuleList[moduleIdx].SetOwner( static_cast<TeamColor>( color ) );
}

void ObjectISS::SetHP( int moduleIdx, float hp )
{
	m_ModuleList[moduleIdx].SetHP( hp );
}