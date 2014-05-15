﻿#pragma once

// packet에 이런 애들을 인클루드해도 상관이 없을까요...
#include <d3dx9.h>
#include "GameOption.h"

#define MAX_CHAT_LEN	1024

#define MAX_NAME_LEN	30
#define MAX_COMMENT_LEN	40

// POD type
// http://en.wikipedia.org/wiki/Plain_Old_Data_Structures
struct Float3D
{
	Float3D()
		: m_X( 0.0f ), m_Y( 0.0f ), m_Z( 0.0f )
	{}

	Float3D( const D3DXVECTOR3& src ) 
		: m_X( src.x ), m_Y( src.y ), m_Z( src.z )
	{}

	Float3D& operator=( const Float3D& rhs )
	{
		m_X = rhs.m_X;
		m_Y = rhs.m_Y;
		m_Z = rhs.m_Z;

		return *this;
	}

	D3DXVECTOR3 GetD3DVEC()
	{
		return D3DXVECTOR3( m_X, m_Y, m_Z );
	}

	float m_X = 0.0f;
	float m_Y = 0.0f;
	float m_Z = 0.0f;
};


// 조심해!!
// 패킷 종류 - 스킬인지, 게임 진행 상태인지에 따라서 숫자 구간 나눠서 사용하자
enum PacketTypes
{
	PKT_NONE = 0,

	// 로그인 및 기타 초반 접속에 관련된 패킷
	PKT_CS_LOGIN = 1,
	PKT_SC_LOGIN = 2,

	PKT_CS_NEW = 3, // 새 플레이어나 오브젝트 생성
	PKT_SC_NEW = 4,


	// 게임 내 상태 전달에 관련된 패킷
	PKT_CS_GAME_STATE = 101,
	PKT_SC_GAME_STATE = 102,

	PKT_CS_ISS_STATE = 103,
	PKT_SC_ISS_STATE = 104,

	PKT_CS_ISS_MODULE_STATE = 105,
	PKT_SC_ISS_MODULE_STATE = 106,

	PKT_CS_GAME_RESULT = 151,
	PKT_SC_GAME_RESULT = 152,


	// 플레이어 상태에 관련된 패킷
	PKT_CS_DEAD = 201,
	PKT_SC_DEAD = 202,

	PKT_CS_RESPAWN = 203,
	PKT_SC_RESPAWN = 204,

	PKT_CS_COLLISION = 205,
	PKT_SC_COLLISION = 206,

	PKT_CS_KINETIC_STATE = 207,
	PKT_SC_KINETIC_STATE = 208, // 운동 상태

	PKT_CS_CHARACTER_STATE = 209,
	PKT_SC_CHARACTER_STATE = 210, // 연료, 산소 등 

	// 플레이어 스킬에 관련된 패킷
	PKT_CS_ACCELERATION = 301,
	PKT_SC_ACCELERATION = 302,

	PKT_CS_STOP = 303,
	PKT_SC_STOP = 304,

	PKT_CS_ROTATION = 305,
	PKT_SC_ROTATION = 306,

	PKT_CS_USING_SKILL = 307,
	PKT_SC_USING_SKILL = 308,

	// 기타 패킷
	PKT_CS_SYNC = 901,
	PKT_SC_SYNC = 902,

	PKT_MAX = 1024
};

#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize( 0 ), mType( PKT_NONE ) 	{}
	unsigned short mSize;
	short mType;
};


// 로그인 요청
struct LoginRequest : public PacketHeader
{
	LoginRequest()
	{
		mSize = sizeof( LoginRequest );
		mType = PKT_CS_LOGIN;
	}
};

// 아이디 할당해서 전송
struct LoginResult : public PacketHeader
{
	LoginResult()
	{
		mSize = sizeof( LoginResult );
		mType = PKT_SC_LOGIN;
		mPlayerId = -1;

		mTeamColor = -1;
	}

	int		mPlayerId;
	int		mTeamColor;
};

// 가속 좀 하겠습니다.
struct AccelerarionRequest : public PacketHeader
{
	AccelerarionRequest()
	{
		mSize = sizeof( AccelerarionRequest );
		mType = PKT_CS_ACCELERATION;
		mPlayerId = -1;
	}

	int	mPlayerId;
};

// 가속 - id, 위치, 속도, 방향(현재 회전 각도 - 나중에는 행렬자체를 넘겨야 할 수도 있어)
struct AccelerarionResult : public PacketHeader
{
	AccelerarionResult()
	{
		mSize = sizeof( AccelerarionResult );
		mType = PKT_SC_ACCELERATION;
		mPlayerId = -1;
	}

	int	mPlayerId;

	Float3D mPos;
	Float3D mVelocity;
	Float3D mRotation;
};

// 장비를 정지합니다.
struct StopRequest : public PacketHeader
{
	StopRequest()
	{
		mSize = sizeof( StopRequest );
		mType = PKT_CS_STOP;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 장비를 정지해라.
struct StopResult : public PacketHeader
{
	StopResult()
	{
		mSize = sizeof( StopResult );
		mType = PKT_SC_STOP;
		mPlayerId = -1;
	}

	int		mPlayerId;

	Float3D mPos;
};

// 회전 좀 하겠습니다. - 나중에는 각도만이 아니라 회전 변환 행렬 전체를 보내야?
struct RotationRequest : public PacketHeader
{
	RotationRequest()
	{
		mSize = sizeof( RotationRequest );
		mType = PKT_CS_ROTATION;
		mPlayerId = -1;
	}

	int		mPlayerId;

	Float3D mRotation;
};

// 회전 해라.
struct RotationResult : public PacketHeader
{
	RotationResult()
	{
		mSize = sizeof( RotationResult );
		mType = PKT_SC_ROTATION;
		mPlayerId = -1;
	}

	int		mPlayerId;

	Float3D mRotation;
};

// 주기적인 동기화를 위해서 - 오브젝트들은 어떻게 하지... 일단 플레이어만 하자
struct SyncRequest : public PacketHeader
{
	SyncRequest()
	{
		mSize = sizeof( SyncRequest );
		mType = PKT_CS_SYNC;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

struct SyncResult : public PacketHeader
{
	SyncResult()
	{
		mSize = sizeof( SyncResult );
		mType = PKT_SC_SYNC;
		mPlayerId = -1;
	}

	int		mPlayerId;

	Float3D mPos; 
	Float3D mVelocity;
};


// 캐릭터 hp가 0일 때 보냄.
struct DeadRequest : public PacketHeader
{
	DeadRequest()
	{
		mSize = sizeof( DeadRequest );
		mType = PKT_CS_DEAD;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

struct DeadResult : public PacketHeader
{
	DeadResult()
	{
		mSize = sizeof ( DeadResult );
		mType = PKT_SC_DEAD;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

struct RespawnRequest : public PacketHeader
{
	RespawnRequest()
	{
		mSize = sizeof ( RespawnRequest );
		mType = PKT_CS_RESPAWN;
		mPlayerId = -1;
		mCharacterClass = 0; // no_class
	}
	int		mPlayerId;
	int		mCharacterClass;
};

struct RespawnResult : public PacketHeader
{
	RespawnResult()
	{
		mSize = sizeof ( RespawnResult );
		mType = PKT_SC_RESPAWN;
		mPlayerId = -1;
		mCharacterClass = 0; // no_class
	}

	int		mPlayerId;
	int		mCharacterClass;

	Float3D mPos;
	Float3D mRotation;
};

// 일단 안씀.. 충돌체크는 서버에서
struct CollisionRequest : public PacketHeader
{

};

struct CollisionResult : public PacketHeader
{
	CollisionResult()
	{
		mSize = sizeof ( CollisionResult );
		mType = PKT_SC_COLLISION;
		mPlayerId = -1;		
	}

	int		mPlayerId; 
	
	Float3D mPos;
	Float3D mVelocity;
};


// 일단 새 플레이어 생성하는 것에 대해서만
// 나중에는 플레이어 오브젝트 구분해야 함
struct NewResult : public PacketHeader
{
	NewResult()
	{
		mSize = sizeof( NewResult );
		mType = PKT_SC_NEW;
		mPlayerId = -1;
	}

	int		mPlayerId;

	Float3D mPos;
	Float3D mVelocity;
	Float3D mRotation;
};

// 스킬 사용
struct UsingSkillRequest : public PacketHeader
{
	UsingSkillRequest()
	{
		mSize = sizeof( UsingSkillRequest );
		mType = PKT_CS_USING_SKILL;
		mPlayerId = -1;

		mSkill = ClassSkill::NO_SKILL;
	}

	int			mPlayerId;

	ClassSkill	mSkill;
	Float3D		mDirection;
};

// 사용 결과
struct UsingSkillResult : public PacketHeader
{
	UsingSkillResult()
	{
		mSize = sizeof( UsingSkillResult );
		mType = PKT_SC_USING_SKILL;
		mPlayerId = -1;

		mSkill = ClassSkill::NO_SKILL;
	}

	int			mPlayerId;

	ClassSkill	mSkill;
};

// 로그인을 성공하면 지금 게임 상태를 모두 전송 받는다.
struct GameStateRequest : public PacketHeader
{
	GameStateRequest()
	{
		mSize = sizeof( GameStateRequest );
		mType = PKT_CS_GAME_STATE;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 오브젝트나 플레이어는 각각의 sync패킷으로 보내고
// 이 패킷에는 게임 현재 상태(시작하고 지난 시간, 각 팀 점수 등)를 전송
struct GameStateResult : public PacketHeader
{
	GameStateResult()
	{
		mSize = sizeof( GameStateResult );
		mType = PKT_SC_GAME_STATE;
		mPlayerId = -1;
	}

	int		mPlayerId;

	// 추가할 것
};

// 지금 ISS 위치, 속도 알려주세요
struct IssStateRequest : public PacketHeader
{
	IssStateRequest()
	{
		mSize = sizeof( IssStateRequest );
		mType = PKT_CS_ISS_STATE;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 지금 ISS 위치, 속도, 모듈 상태 알려드립니다.
// 하나 하나 다 복사 하다가 뭔가 아닌 것 같아서 배열로 갑니다.
// 배열 복사도 된다고 하셔서...
struct IssStateResult : public PacketHeader
{
	IssStateResult()
	{
		mSize = sizeof( IssStateResult );
		mType = PKT_SC_ISS_STATE;

		mIssPositionZ = 0.0f;
		mIssVelocityZ = 0.0f;

		memset( mModuleOwner, 0, sizeof( mModuleOwner ) );
		memset( mModuleHP, 0, sizeof( mModuleHP ) );
	}
	
	float	mIssPositionZ;
	float	mIssVelocityZ;

	int		mModuleOwner[MODULE_NUMBER];
	float	mModuleHP[MODULE_NUMBER];
};

// 지금 ISS 모듈 소유자 및 체력 알려주세요
struct IssModuleStateRequest : public PacketHeader
{
	IssModuleStateRequest()
	{
		mSize = sizeof( IssModuleStateRequest );
		mType = PKT_CS_ISS_MODULE_STATE;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 지금 ISS 모듈 소유자 및 체력 알려드립니다.
struct IssModuleStateResult : public PacketHeader
{
	IssModuleStateResult()
	{
		mSize = sizeof( IssModuleStateResult );
		mType = PKT_SC_ISS_MODULE_STATE;
		mModuleIdx = -1;

		mOwner = -1;
		mHP = 1.0f;
	}

	int		mModuleIdx;

	int		mOwner;
	float	mHP;
};

// 게임 결과를 알려주세요
struct GameResultRequest : public PacketHeader
{
	GameResultRequest()
	{
		mSize = sizeof( GameResultRequest );
		mType = PKT_CS_GAME_RESULT;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 게임 결과 
struct GameResultResult : public PacketHeader
{
	GameResultResult()
	{
		mSize = sizeof( GameResultResult );
		mType = PKT_SC_GAME_RESULT;
		mWinnerTeam = -1;
	}

	int		mWinnerTeam;

	// 나중에 부가 정보 추가할 것
};

// 운동 상태 좀 알려주세요
struct KineticStateRequest : public PacketHeader
{
	KineticStateRequest( )
	{
		mSize = sizeof( KineticStateRequest );
		mType = PKT_CS_KINETIC_STATE;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 넌 지금 이러고 있단다
struct KineticStateResult : public PacketHeader
{
	KineticStateResult( )
	{
		mSize = sizeof( KineticStateResult );
		mType = PKT_SC_KINETIC_STATE;
		mPlayerId = -1;

		mSpinAngularVelocity = 0.0f;
	}

	int		mPlayerId;

	Float3D mPos;
	Float3D mForce;
	Float3D mVelocity;
	Float3D mSpinAxis;
	// Float3D mForce; // 적용되는 시점이 차이가 날 수 밖에 없으므로 다른 방식으로 동기화 할 것

	float mSpinAngularVelocity;
};

// 운동 상태 좀 알려주세요
struct CharacterStateRequest : public PacketHeader
{
	CharacterStateRequest( )
	{
		mSize = sizeof( CharacterStateRequest );
		mType = PKT_CS_CHARACTER_STATE;
		mPlayerId = -1;
	}

	int		mPlayerId;
};

// 넌 지금 이러고 있단다
struct CharacterStateResult : public PacketHeader
{
	CharacterStateResult( )
	{
		mSize = sizeof( CharacterStateResult );
		mType = PKT_SC_KINETIC_STATE;
		mPlayerId = -1;

		mFuel = 0.0f;
		mOxygen = 0.0f;
	}

	int		mPlayerId;

	float	mFuel;
	float	mOxygen;
};


#pragma pack(pop)