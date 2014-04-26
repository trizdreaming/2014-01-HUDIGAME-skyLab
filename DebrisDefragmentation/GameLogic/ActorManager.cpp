﻿#include "stdafx.h"
#include "ActorManager.h"
#include "Physics.h"

ActorManager::ActorManager()
{
	for ( unsigned int playerId = 0; playerId < MAX_PLAYER_NUM; ++playerId )
	{
		m_ActorList[playerId] = nullptr;
	}
}


ActorManager::~ActorManager()
{
}

void ActorManager::Init( )
{
	m_PrevTime = timeGetTime( );
	// InitializeSRWLock( &m_SRWLock );
}

int ActorManager::RegisterUser( Actor* newActor )
{
	// user id를 manager에 등록된 actor index와 같게 유지하면 객체들 인터랙션을 처리할 때 유리할 것 같은데
	// 어떻게 효율적으로 비어있는 index를 찾고 그걸 세션에게 할당할 지 잘 모르겠네요
	// 일단 이렇게 처음부터 순회하면서 비어있는 자리가 있으면 거기에 포인터 등록하고 idx 반환하도록 하는데...
	for ( unsigned int actorId = 0; actorId < MAX_PLAYER_NUM; ++actorId )
	{
		if ( m_ActorList[actorId] == nullptr )
		{
			m_ActorList[actorId] = newActor;
			return actorId;
		}
	}

	return -1; // 빈 자리가 없습니다.
}

void ActorManager::ChangeActor( Actor* newActor, int actorId )
{
	// 다른 세션 캐릭터를 바꾸는 일은 없도록 만들어야 할 듯
	m_ActorList[actorId] = newActor;
}

void ActorManager::DeleteActor( int actorId )
{
	if ( m_ActorList[actorId] != nullptr )
	{
		// 객체의 삭제는 생성한 clientSession에서 한다
		// delete m_ActorList[actorId];
		m_ActorList[actorId] = nullptr;
	}
}

bool ActorManager::Update( )
{
	DWORD currentTime = timeGetTime( );
	float dt = ( static_cast<float>( currentTime - m_PrevTime ) ) / 1000.f;
	m_PrevTime = currentTime;

	for ( unsigned int actorId = 0; actorId < MAX_PLAYER_NUM; ++actorId )
	{
		if ( m_ActorList[actorId] != nullptr )
		{
			m_ActorList[actorId]->Update( dt );
		}
	}

	// 충돌 체크
	return CheckCollision();
}

bool ActorManager::IsValidId( int actorId )
{
	if ( m_ActorList[actorId] != nullptr )
		return true;

	return false;
}

bool ActorManager::CheckCollision()
{
	bool returnVal = false; // 충돌이 있는지 알려줌 - 클라이언트들에게 업데이트하라고 시켜야 되니까

	// 순회하면서 각 액터들이 충돌하는지 확인
	// 충돌하는 애들 있으면 각각에게 충돌 판정을 전달한다.
	// 전달 방식은 충돌 후 이동 방향(서로 반대 방향)을 전달한다.
	for ( int i = 0; i < MAX_PLAYER_NUM; ++i )
	{
		if ( m_ActorList[i] == nullptr )
			continue;

		for ( int j = i + 1; j < MAX_PLAYER_NUM; ++j )
		{
			if ( m_ActorList[j] == nullptr )
				continue;

			// 두 점의 거리가 가까우면 체크 안 함
			D3DXVECTOR3 collisionDirection = m_ActorList[j]->GetPosition() - m_ActorList[i]->GetPosition();
			// printf_s( "%f / %f\n", D3DXVec3Length( &collisionDirection ), m_ActorList[i]->GetCollisionBox().m_Radius + m_ActorList[j]->GetCollisionBox().m_Radius );
			if ( D3DXVec3Length( &collisionDirection ) > m_ActorList[i]->GetCollisionBox().m_Radius + m_ActorList[j]->GetCollisionBox().m_Radius )
				continue;

			// 충돌체크
			if ( Physics::IsCollide( m_ActorList[i]->GetCollisionBox(), m_ActorList[j]->GetCollisionBox() ) )
			{
				printf_s( "collision!\n" );
				// 두 물체의 중심점을 잇는 단위 벡터 생성
				D3DXVec3Normalize( &collisionDirection, &collisionDirection );

				// 충격량 산출
				float impulse = D3DXVec3Dot( &(m_ActorList[i]->GetVelocity() - m_ActorList[j]->GetVelocity() ), &collisionDirection );

				// 질량에 비례해서 운동 상태 변경
				m_ActorList[i]->IncreaseVelocity( 
					collisionDirection * impulse * m_ActorList[i]->GetMass() 
					/ ( m_ActorList[i]->GetMass() + m_ActorList[j]->GetMass() ) 
					);

				m_ActorList[j]->IncreaseVelocity(
					collisionDirection * impulse * m_ActorList[j]->GetMass()
					/ ( m_ActorList[i]->GetMass() + m_ActorList[j]->GetMass() )
					);

				returnVal = true;
			}
		}
	}

	return returnVal;
}

int ActorManager::DetectTarget( int actorId )
{
	// 충돌 박스의 각 점들을 조합해서 만들 수 있는 면 6개에 대해서
	// 요청한 액터의 뷰( z축 ) 방향과 각각의 면이 교차하는지 확인한다.
	float currentDistance = static_cast<float>( HUGE );
	int targetId = -1;
	
	D3DXVECTOR3 viewDirection = m_ActorList[actorId]->GetViewDirection( );
	D3DXVECTOR3	startPoint = m_ActorList[actorId]->GetPosition();
	
	for ( int i = 0; i < MAX_PLAYER_NUM; ++i )
	{
		if ( i == actorId || m_ActorList[i] == nullptr )
			continue;

		if ( Physics::IntersectionCheckRayBox( viewDirection, startPoint, m_ActorList[i]->GetCollisionBox() ) )
		{
			// 거리 구해서 더 짧으면 인덱스 업데이트
			float tempDistance = D3DXVec3Length( &( startPoint - m_ActorList[i]->GetPosition() ) );
			if ( tempDistance < currentDistance )
			{
				currentDistance = tempDistance;
				targetId = i;
			}
		}
	}

	return targetId;
}
