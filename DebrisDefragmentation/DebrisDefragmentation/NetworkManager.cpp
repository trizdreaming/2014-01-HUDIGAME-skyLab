﻿#include "stdafx.h"
#include "NetworkManager.h"
#include "DDNetwork.h"
#include "PacketType.h"
#include "PlayerManager.h"
#include "ObjectManager.h"
#include "MatrixTransform.h"
#include "SceneManager.h"
#include "PlayScene.h"
#include "GameOption.h"
#include "ClassComponent.h"
#include "DDModel.h"
#include "DDCamera.h"
#include "DebugData.h"
#include "DebrisModel.h"
#include "Environment.h"
#include "EnvironmentManager.h"
#include "SoundManager.h"
#include "DDSceneDirector.h"

NetworkManager* GNetworkManager = nullptr;
int NetworkManager::m_MyPlayerId = -1;

NetworkManager::NetworkManager()
{
}


NetworkManager::~NetworkManager()
{
}

void NetworkManager::Init()
{
	RegisterHandles();
}

void NetworkManager::Connect()
{
	bool connection = false;

	/// config.h
#ifdef USE_LOCAL_SERVER
	connection = DDNetwork::GetInstance()->Connect( "localhost", 9001 );
#else
	connection = DDNetwork::GetInstance()->Connect( "10.73.45.144", 9001 );
#endif	
	
	if ( connection )
	{
		// 로그인 요청 전송
		LoginRequest outPacket;
		DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
	}
	else
	{
		printf_s( "failed to connection\n" );
	}
}

void NetworkManager::Disconnect()
{
	DDNetwork::GetInstance()->Disconnect();
}

void NetworkManager::SendAcceleration()
{
	if ( m_MyPlayerId == -1 )
		return;

	AccelerarionRequest outPacket;

	outPacket.mPlayerId = m_MyPlayerId;
	

	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

void NetworkManager::SendStop()
{
	if ( m_MyPlayerId == -1 )
		return;

	StopRequest  outPacket;

	outPacket.mPlayerId = m_MyPlayerId;

	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

// lookat 방향으로 몸을 회전
void NetworkManager::SendTurnBody()
{
	if ( m_MyPlayerId == -1 )
		return;

	printf( "send turn body\n" );
	RotationRequest outPacket;

	outPacket.mPlayerId = m_MyPlayerId;

	// 카메라 플레이어 분리 후 getcameraviewingdirection은 사용X

	outPacket.mRotation.m_X = GPlayerManager->GetCamera()->GetTransform().GetRotationX();
	outPacket.mRotation.m_Y = GPlayerManager->GetCamera()->GetTransform().GetRotationY();
	if ( !GPlayerManager->GetPlayer( m_MyPlayerId ) )
		return;
	outPacket.mRotation.m_Z = GPlayerManager->GetPlayer( m_MyPlayerId )->GetTransform().GetRotationZ();

	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

void NetworkManager::SendUsingSkill( ClassSkill skilType )
{
	if ( m_MyPlayerId == -1 )
		return;

	UsingSkillRequest outPacket;

	outPacket.mPlayerId = m_MyPlayerId;
	outPacket.mSkill = skilType;
	outPacket.mDirection = GPlayerManager->GetCamera()->GetTransform().GetRotation(); // 카메라 방향 넣어 줘야 한다.

	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

// 
// // 캐릭터가 죽으면 보냄..
// void NetworkManager::SendDeadRequest()
// {
// 	if ( m_MyPlayerId == -1 )
// 		return;
// 
// 	DeadRequest outPacket;
// 
// 	outPacket.mPlayerId = m_MyPlayerId;
// 	
// 
// 	// 지금은 게임 로직 계산은 서버에서만 하므로 일단 보내지 않음
// 	// DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
// }

// 캐릭터가 리스폰될 때 보냄.
void NetworkManager::SendRespawnRequest( CharacterClass characterClass )
{
	if ( m_MyPlayerId == -1 )
		return;

	RespawnRequest outPacket;

	outPacket.mPlayerId = m_MyPlayerId;
	outPacket.mCharacterClass = static_cast<int>( characterClass );

	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

void NetworkManager::RegisterHandles()
{
	// 여기에서 핸들러를 등록하자
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_LOGIN, HandleLoginResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_ACCELERATION, HandleGoForwardResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_STOP, HandleStopResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_ROTATION, HandleTurnBodyResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_USING_SKILL, HandleUsingSkillResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_SYNC, HandleSyncResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_NEW, HandleNewResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_DEAD, HandleDeadResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_RESPAWN, HandleRespawnResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_COLLISION, HandleCollisionResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_GAME_RESULT, HandleGameResultResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_KINETIC_STATE, HandleKineticStateResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_CHARACTER_STATE, HandleCharacterStateResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_GATHER, HandleGatherResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_DEBUG_SERVER, HandleSyncServerDebugInfoResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_DEBUG_CHARACTER, HandleSyncCharacterDebugInfoResult );
}

void NetworkManager::HandleGatherResult( DDPacketHeader& pktBase )
{
	GatherResult inPacket = reinterpret_cast<GatherResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// 데브리 모델의 visible을 끔(굳이 삭제를 할 필요는 없을듯.. 나중에 한번에 하니까 ㅋ)
	GObjectManager->GetResourceDebris( inPacket.mDebrisIndex )->SetVisible( false );
	GEnvironmentManager->PlayFireworkEffect( EffectType::EXPLOSION, GObjectManager->GetResourceDebris( inPacket.mDebrisIndex )->GetTransform().GetPosition());

	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetClassComponent()->SetResource( inPacket.mCurrentResource );
}

void NetworkManager::HandleLoginResult( DDPacketHeader& pktBase )
{
	// 일단 저기서 얼마만큼 읽어와서 해당 패킷을 구성하고
	LoginResult inPacket = reinterpret_cast<LoginResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// 사용자의 player가 최초 로그인한 경우
	m_MyPlayerId = inPacket.mPlayerId;
	GPlayerManager->SetMyPlayerId( m_MyPlayerId );

	if ( GPlayerManager->AddPlayer( m_MyPlayerId ) )
	{
		// camera 설정
		DDCamera* camera = DDCamera::Create();
		GPlayerManager->SetCamera( camera );
		GSceneManager->GetPlayScene()->AddChild(camera, ORDER_COMPASS_UI);

		if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
			return;

		camera->SetFollowingObject( GPlayerManager->GetPlayer( m_MyPlayerId ) );
		camera->GetTransform().SetRotation( (static_cast<TeamColor>(inPacket.mTeamColor) == TeamColor::BLUE)? BLUE_TEAM_ROTATION : RED_TEAM_ROTATION );

		// 팀 설정
		GPlayerManager->GetPlayer( m_MyPlayerId )->SetTeam( static_cast<TeamColor>(inPacket.mTeamColor) );
		GObjectManager->SetRandomSeed( inPacket.mRandomSeed );
		GSceneManager->GetPlayScene()->InitResourceDebris();
	}

	GameStateRequest outPacket;
	outPacket.mPlayerId = m_MyPlayerId;
	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

void NetworkManager::HandleGoForwardResult( DDPacketHeader& pktBase )
{
	AccelerarionResult inPacket = reinterpret_cast<AccelerarionResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );
		
//	GPlayerManager->AddPlayer( inPacket.mPlayerId );
	Player* player = GPlayerManager->GetPlayer( inPacket.mPlayerId );
	if ( !player )
		return;

	// printf_s( "player %d gofoward\ninputVel   : %f %f %f\ncurrentVel : %f %f %f\n", inPacket.mPlayerId, inPacket.mVelocity.x, inPacket.mVelocity.y, inPacket.mVelocity.z, player->GetVelocity().x, player->GetVelocity().y, player->GetVelocity().z);
	// printf_s( "currentAcc : %f %f %f\n", player->GetAcceleration().x, player->GetAcceleration().y, player->GetAcceleration().z );

	player->GetTransform().SetPosition( inPacket.mPos );
	player->GetTransform().SetRotation( inPacket.mRotation );
	player->SetVelocity( inPacket.mVelocity );
	D3DXVECTOR3 viewDirection = player->GetViewDirection();
	D3DXVec3Normalize( &viewDirection, &viewDirection );
	GEnvironmentManager->PlayFireworkEffect( EffectType::EXPLOSION, player->GetTransform().GetPosition() + viewDirection * 3, -viewDirection - DIRECTION_OFFSET, -viewDirection + DIRECTION_OFFSET );

	player->Move( player->GetViewDirection() );
	
	GSoundManager->PlaySound( SE_ACCEL );
}

void NetworkManager::HandleStopResult( DDPacketHeader& pktBase )
{
	StopResult inPacket = reinterpret_cast<StopResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );
		
//	GPlayerManager->AddPlayer( inPacket.mPlayerId );	
	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->Stop();
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetPosition( inPacket.mPos );
	
	GSoundManager->PlaySound( SE_STOP );
}

void NetworkManager::HandleTurnBodyResult( DDPacketHeader& pktBase )
{
	RotationResult inPacket = reinterpret_cast<RotationResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	GPlayerManager->AddPlayer( inPacket.mPlayerId );

	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetRotation( inPacket.mRotation );
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->StopSpin();
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::IDLE );
}

void NetworkManager::HandleSyncResult( DDPacketHeader& pktBase )
{
	GhostSyncResult inPacket = reinterpret_cast<GhostSyncResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// 서버의 충돌박스 그리기 용 임시..
#ifdef USE_CHARACTER_COLLISION_BOX
	int dummyPlayerID = -1;
	if ( inPacket.mPlayerId != -1 )
	{
		dummyPlayerID = inPacket.mPlayerId + REAL_PLAYER_NUM;

		if ( !GPlayerManager->GetPlayer( dummyPlayerID ) )
			return;

		GPlayerManager->AddPlayer( dummyPlayerID );
		GPlayerManager->GetPlayer( dummyPlayerID )->GetTransform().SetPosition( inPacket.mPos );
		GPlayerManager->GetPlayer( dummyPlayerID )->SetVelocity( inPacket.mVelocity );
	}
#else
	if ( inPacket.mPlayerId != -1 )
	{
		GPlayerManager->AddPlayer( inPacket.mPlayerId );

		if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
			return;

		GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetPosition( inPacket.mPos );
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetVelocity( inPacket.mVelocity );
	}
#endif
}

void NetworkManager::HandleUsingSkillResult( DDPacketHeader& pktBase )
{
	UsingSkillResult inPacket = reinterpret_cast<UsingSkillResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// 지금은 특별히 하는 일 없음
	// 스킬 쓴 캐릭터에 변경이 필요한 작업 일단 여기서 처리
	// 스킬 사용에 따른 행동 포인트 등의 지표가 변화가 생긴다면 그걸 처리하는 함수를 따로 만들어야 할 듯

	// 애니메이션 적용
	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	switch ( inPacket.mSkill )
	{
	case ClassSkill::PUSH:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		GSoundManager->PlaySound( SE_PUSH );
		break;
	case ClassSkill::OCCUPY:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		break;
	case ClassSkill::DESTROY:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::ATTACK );
		break;
	case ClassSkill::SHARE_FUEL:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		break;
	case ClassSkill::PULL:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		break;
	case ClassSkill::SET_MINE:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		break;
	case ClassSkill::SHARE_OXYGEN:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		break;
	case ClassSkill::EMP:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		break;
	case ClassSkill::GATHER:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		GSoundManager->PlaySound( SE_DEBRIS_GET );
		break;
	case ClassSkill::SET_DISPENSER:
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::FIRE );
		GSoundManager->PlaySound( SE_DESPENDER_INSTALL );
		break;
	default:
		break;
	}

	if ( inPacket.mSkill == ClassSkill::SHARE_FUEL )
	{
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetClassComponent()->IncreaseFuel( -DEFAULT_FUEL_SHARE_AMOUNT );
	}
	else if ( inPacket.mSkill == ClassSkill::SHARE_OXYGEN )
	{
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetClassComponent()->IncreaseOxygen( -DEFAULT_OXYGEN_SHARE_AMOUNT );
	}
}

void NetworkManager::HandleKineticStateResult( DDPacketHeader& pktBase )
{
	KineticStateResult inPacket = reinterpret_cast<KineticStateResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

//	GPlayerManager->AddPlayer( inPacket.mPlayerId );
	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetPosition( inPacket.mPos );
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetVelocity( inPacket.mVelocity );

	if ( inPacket.mIsSpin )
	{
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetSpin( inPacket.mForce, inPacket.mSpinAngularVelocity );
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetSpin( inPacket.mSpinAxis, inPacket.mSpinAngularVelocity );
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetCharacterAnimState( CharacterAnimState::REACT );
		GEnvironmentManager->PlayFireworkEffect( EffectType::EXPLOSION, inPacket.mPos );
	}

	if ( inPacket.mIsAccelerate )
	{
		GPlayerManager->GetPlayer( inPacket.mPlayerId )->Move( inPacket.mForce );
	}
}

void NetworkManager::HandleCharacterStateResult( DDPacketHeader& pktBase )
{
	CharacterStateResult inPacket = reinterpret_cast<CharacterStateResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );
//	PlayerManager* a = GPlayerManager.get();

	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetClassComponent()->SetFuel( inPacket.mFuel );
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetClassComponent()->SetOxygen( inPacket.mOxygen );
}

void NetworkManager::HandleNewResult( DDPacketHeader& pktBase )
{
	NewResult inPacket = reinterpret_cast<NewResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	GPlayerManager->AddPlayer( inPacket.mPlayerId );

	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetPosition( inPacket.mPos );
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetRotation( inPacket.mRotation );
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetVelocity( inPacket.mVelocity );
}


void NetworkManager::HandleDeadResult( DDPacketHeader& pktBase )
{
	DeadResult inPacket = reinterpret_cast<DeadResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// 죽으면 처리할 부분 추가할 것
	// fps 게임에서 죽으면 다른 캐릭터 보는 view로 바꿔준다거나, scene을 바꿔준다거나...	

	// 죽어라

	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetClassComponent()->SetHP( 0.0f );
}

void NetworkManager::HandleRespawnResult( DDPacketHeader& pktBase )
{
	RespawnResult inPacket = reinterpret_cast<RespawnResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	Player* targetPlayer = GPlayerManager->GetPlayer( inPacket.mPlayerId );
	if ( !targetPlayer )
		return;

	targetPlayer->ChangeClass( static_cast<CharacterClass>( inPacket.mCharacterClass ) );
	targetPlayer->GetTransform().SetPosition( inPacket.mPos );
	targetPlayer->GetTransform().SetRotation( inPacket.mRotation );	
	targetPlayer->InitRigidBody();

	printf_s( "class changed : %d\n", inPacket.mCharacterClass );
	printf_s( "respawn player" );	
}

void NetworkManager::HandleCollisionResult( DDPacketHeader& pktBase )
{
	CollisionResult inPacket = reinterpret_cast<CollisionResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );
	
//	GPlayerManager->AddPlayer( inPacket.mPlayerId );

	if ( !GPlayerManager->GetPlayer( inPacket.mPlayerId ) )
		return;

	GPlayerManager->GetPlayer( inPacket.mPlayerId )->GetTransform().SetPosition( inPacket.mPos );
	GPlayerManager->GetPlayer( inPacket.mPlayerId )->SetVelocity( inPacket.mVelocity );
	GEnvironmentManager->PlayFireworkEffect( EffectType::EXPLOSION, inPacket.mPos );

	GSoundManager->PlaySoundW( SE_COLLISION );
}

void NetworkManager::HandleGameResultResult( DDPacketHeader& pktBase )
{
	GameResultResult inPacket = reinterpret_cast<GameResultResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// ISS의 체력을 바꾼다.
	printf_s( "winner id %d\n", inPacket.mWinnerTeam );

	// 여기서 씬 전환을 한다
	DDSceneDirector::GetInstance()->ChangeScene( GSceneManager->CreateScene(L"ResultScene", SceneType::RESULT_SCENE) );

	// 일단 다른 씬들이 없으므로 게임을 종료시킨다.
	DDNetwork::GetInstance()->Disconnect();
}

void NetworkManager::HandleSyncServerDebugInfoResult( DDPacketHeader& pktBase )
{
	DebugServerInfoResult inPacket = reinterpret_cast<DebugServerInfoResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	for ( int i = 0; i < REAL_PLAYER_NUM; ++i )
	{
		GDebugData->mPlayerTeam[i] = inPacket.mPlayerTeam[i];
		GDebugData->mPlayerClass[i] = inPacket.mPlayerClass[i];
	}

	GDebugData->mIssPos = inPacket.mIssPos;
	GDebugData->mIssVelocity = inPacket.mIssVelocity;

	for ( int i = 0; i < MODULE_NUMBER; ++i )
	{
		GDebugData->mModuleOwner[i] = inPacket.mModuleOwner[i];
		GDebugData->mModuleHP[i] = inPacket.mModuleHP[i];
	}
}

void NetworkManager::HandleSyncCharacterDebugInfoResult( DDPacketHeader& pktBase )
{
	DebugClientInfoResult inPacket = reinterpret_cast<DebugClientInfoResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	GDebugData->mPlayerId = inPacket.mPlayerId;
	GDebugData->mClass = inPacket.mClass;

	GDebugData->mPos = inPacket.mPos;

	GDebugData->mIsSpin = inPacket.mIsSpin;
	GDebugData->mIsAccelerate = inPacket.mIsAccelerate;

	GDebugData->mSpeedConstant = inPacket.mSpeedConstant;
	GDebugData->mAcceleration = inPacket.mAcceleration;
	GDebugData->mVelocity = inPacket.mVelocity;
	GDebugData->mSpinAxis = inPacket.mSpinAxis;
	GDebugData->mSpinAngularVelocity = inPacket.mSpinAngularVelocity;

	GDebugData->mFuel = inPacket.mFuel;
	GDebugData->mOxygen = inPacket.mOxygen;
}

CharacterClass NetworkManager::GetMyClass()
{ 
	if ( !GPlayerManager->GetPlayer( m_MyPlayerId ) )
		return CharacterClass::NO_CLASS;

	return GPlayerManager->GetPlayer( m_MyPlayerId )->GetClassComponent()->GetCharacterClassName(); 
}

