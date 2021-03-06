﻿#include "stdafx.h"
#include "Engineer.h"
#include "ActorManager.h"
#include "ObjectTable.h"
#include "Physics.h"

Engineer::Engineer()
{
	m_ClassName = CharacterClass::ENGINEER;
}


Engineer::~Engineer()
{
}

bool Engineer::UseSkill( ClassSkill skill, int id, const D3DXVECTOR3& direction )
{
	switch ( skill )
	{
	case ClassSkill::PUSH:
		return SkillPush( id, direction );
	case ClassSkill::GATHER:
		return SkillGather( id, direction );
	default:
		break;
	}

	return false;
}

bool Engineer::SkillGather( int id, const D3DXVECTOR3& direction )
{
	if ( m_GlobalCooldown > 0.0f || m_CooldownTable[static_cast<int>(ClassSkill::GATHER)] > 0.0f )
		return false;

	Character* character = GObjectTable->GetActorManager()->GetCharacterList().at( id );
	D3DXVECTOR3 viewDirection = character->GetViewDirection( direction );
	D3DXVECTOR3	startPoint = character->GetTransform()->GetPosition();
	
	D3DXVECTOR3 tmppos = GObjectTable->GetActorManager()->GetResourceDebrisList().at(0)->GetTransform()->GetPosition();
	printf_s( "debirs no 0 pos is : %f %f %f\n", tmppos.x, tmppos.y, tmppos.z );
	// 안맞으면 false 리턴 
	if ( !DebrisInRay( viewDirection, startPoint ) )
	{
		return false;
	}	
	
	// 캐릭터 자원 증가
	character->GetClassComponent()->IncreaseResource( DEBRIS_TO_RESOURCE_AMOUNT );
				
	// 조심해!! 지금은 일단 리스트에서 삭제함. 
	// 혹시 추후에 데브리가 리젠되는 식으로 기획이 변경되면 이부분을 수정할 것

	// 레이에 맞은 데브리 삭제
	//GObjectTable->GetActorManager()->RemoveResourceDebris( GObjectTable->GetActorManager()->GetGatheredDebris() );

	SetCooldown( ClassSkill::GATHER );
	
	// broadcast
	GObjectTable->GetActorManager()->BroadcastCharacterChange( static_cast<int>( id ), ChangeType::RESOURCE_GATHER );
	
	return true;
}

bool Engineer::SkillShelter( int id, const D3DXVECTOR3& direction )
{
	return false;
}

bool Engineer::SkillDispenser( int id, const D3DXVECTOR3& direction )
{
	
	return true;
}

bool Engineer::DebrisInRay( const D3DXVECTOR3 &viewDirection, const D3DXVECTOR3 &startPoint )
{
	float currentDistance = std::numeric_limits<float>::infinity( );
	int	debrisNumber = -1;

	auto& debrisList = GObjectTable->GetActorManager( )->GetResourceDebrisList( );
	for ( int i = 0; i < RESOURCE_DEBRIS_NUMBER; ++i )
	{
		// 조심해!! 지금은 일단 리스트에서 삭제함. 
		// 혹시 추후에 데브리가 리젠되는 식으로 기획이 변경되면 이부분을 수정할 것
		if ( debrisList[i] == nullptr ) continue;

		// intersection 확인
		float tempDistance = std::numeric_limits<float>::infinity( );
		const CollisionBox* box = debrisList[i]->GetCollisionBox( );
		if ( Physics::IntersectionCheckRayBox( nullptr, &tempDistance, nullptr, viewDirection, startPoint, box ) )
		{
			// 스킬 사용 범위가 정해지면 그것과 비교해서 더 먼 애는 제외
			// 더 가까운 애로 교체
			if ( tempDistance < currentDistance && tempDistance < SKILL_RANGE )
			{
				currentDistance = tempDistance;
				debrisNumber = i;
			}
		}
	}

	GObjectTable->GetActorManager( )->SetGatheredDebris( debrisNumber );
	return ( debrisNumber == -1 ) ? false : true;

}

void Engineer::DoPeriodWork( float dTime )
{
	// 스킬이나 캐릭터 상태 변화 필요한 부분 ㄱㄱ
	UNREFERENCED_PARAMETER( dTime );
}
