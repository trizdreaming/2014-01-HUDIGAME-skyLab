﻿#pragma once

#include <map>
#include <WinSock2.h>
#include "Config.h"
#include "..\..\PacketType.h"
#include "CircularBuffer.h"
#include "ObjectPool.h"
#include "Character.h"

#define BUFSIZE	(1024*10)

class ClientSession;
class ClientManager;
class GameManager;
struct DatabaseJobContext;

struct OverlappedIO : public OVERLAPPED
{
	OverlappedIO() : mObject( nullptr )
	{}

	ClientSession* mObject;
};

class ClientSession : public ObjectPool<ClientSession>
{
public:
	ClientSession( SOCKET sock, GameManager* gameManager )
		: mConnected( false ), mLogon( false ), mSocket( sock ), mPlayerId( -1 ), mSendBuffer( BUFSIZE ), mRecvBuffer( BUFSIZE ), mOverlappedRequested( 0 )
		, /* mPosX( 0 ), mPosY( 0 ), mPosZ( 0 ), */ mDbUpdateCount( 0 ), m_GameManager( gameManager )
	{
		memset( &mClientAddr, 0, sizeof( SOCKADDR_IN ) );
		//memset( mPlayerName, 0, sizeof( mPlayerName ) );
	}
	~ClientSession() {}

	void	OnRead( size_t len );
	void	OnWriteComplete( size_t len );

	bool	OnConnect( SOCKADDR_IN* addr );

	bool	PostRecv();

	bool	SendRequest( PacketHeader* pkt );
	bool	Broadcast( PacketHeader* pkt );

	void	Disconnect();

	bool	IsConnected() const { return mConnected; }

	void	DatabaseJobDone( DatabaseJobContext* result );

	/// 현재 Send/Recv 요청 중인 상태인지 검사하기 위함
	void	IncOverlappedRequest()		{ ++mOverlappedRequested; }
	void	DecOverlappedRequest()		{ --mOverlappedRequested; }
	bool	DoingOverlappedOperation() const { return mOverlappedRequested > 0; }

	// 패킷 처리할 핸들을 만들자
	///# 사실 이 부분도 궁극적으로는 콘텐츠 관련 객체를 만들고 던지는게 편하다.
	// 방학과 함께 requestHandler제작 시작하겠습니다.
	void	HandleLoginRequest( LoginRequest& inPacket );
	void	HandleAccelerationRequest( AccelerarionRequest& inPacket );
	void	HandleStopRequest( StopRequest& inPacket );
	void	HandleRotationRequest( RotationRequest& inPacket );
	void	HandleGameStateRequest( GameStateRequest& inPacket );
	void	HandleRespawnRequest( RespawnRequest& inPacket );
	void	HandleUsingSkillRequest( UsingSkillRequest& inPacket );

	int					GetPlayerId() const { return mPlayerId; }
	Character&			GetCharacter() { return m_Character; }
	const SOCKET		GetSock() { return mSocket; }
	
	void	SetGameManager( GameManager* manager ) { m_GameManager = manager; }

	// 현재 내 상태를 나를 포함한 전체 플레이어에게 전달
	void	BroadcastCollisionResult();
	void	BroadcastDeadResult();
	void	BroadcastGatherResult();
	void	BroadcastDispenserEffect( bool flag );
	void	SyncGhostInfo();

	void	BroadcastAcceleration();
	void	BroadcastKineticState( bool accelerationFlag, bool spinFlag );
	void	BroadcastCharacterState();

	// 현재 내 상태를 targetClient에게 전달
	void	SendCurrentStatus( ClientSession* targetClient );
	void	SendWarning();

	// 플레이어 캐릭터 정보 전송
	void	SyncCharacterDebugInfo();

private:

	bool	SendFlush(); ///< Send요청 중인것들 모아서 보냄
	void	OnTick();

	void	LoginDone( int pid );
	void	UpdateDone();

private:
	bool			mConnected;
	bool			mLogon;
	SOCKET			mSocket;

	int				mPlayerId;
	SOCKADDR_IN		mClientAddr;

	CircularBuffer	mSendBuffer;
	CircularBuffer	mRecvBuffer;

	OverlappedIO	mOverlappedSend;
	OverlappedIO	mOverlappedRecv;
	int				mOverlappedRequested;

	int				mDbUpdateCount; ///< DB에 주기적으로 업데이트 하기 위한 변수

	Character		m_Character;
	GameManager*	m_GameManager = nullptr;

	friend class ClientManager;
};

void CALLBACK RecvCompletion( DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags );
void CALLBACK SendCompletion( DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags );
