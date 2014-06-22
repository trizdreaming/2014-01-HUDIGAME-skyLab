﻿#include "stdafx.h"
#include "DDLight.h"
#include "DDRenderer.h"


DDLight::DDLight()
{
}


DDLight::~DDLight()
{
}
// 
// DDLight* DDLight::Create()
// {
// 	DDLight* pInstance = new DDLight();
// 	return pInstance;
// }

void DDLight::RenderItSelf()
{
	DDVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof( D3DLIGHT9 ) );
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	// 조심해!
	// 방향 바꾸는 하드 코딩 나중에 삭제할 것
	vecDir = DDVECTOR3( -1.0f, -1.0f, 0.0f );

	D3DXMatrixRotationYawPitchRoll( &m_Matrix, vecDir.y, vecDir.x, vecDir.z );

	D3DXVec3Normalize( (DDVECTOR3*)&light.Direction, &vecDir );
	light.Range = 10000.0f;
	DDRenderer::GetInstance()->GetDevice()->SetLight( 0, &light );
	DDRenderer::GetInstance()->GetDevice()->LightEnable( 0, TRUE );

	

}
