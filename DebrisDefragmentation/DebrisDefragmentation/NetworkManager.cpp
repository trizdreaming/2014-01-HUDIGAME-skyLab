#include "NetworkManager.h"
#include "DDNetwork.h"
#include "PacketType.h"
#include "GameLogic.h"

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
	DDNetwork::GetInstance()->Connect( "localhost", 9001 );
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

	outPacket.mRotationX = GGameLogic->GetPlayer( m_MyPlayerId )->GetRotationX(); 
	outPacket.mRotationY = GGameLogic->GetPlayer( m_MyPlayerId )->GetRotationY();
	outPacket.mRotationZ = GGameLogic->GetPlayer( m_MyPlayerId )->GetRotationZ();

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

void NetworkManager::SendRotation( double y, double x )
{
	if ( m_MyPlayerId == -1 )
		return;

	RotationRequest outPacket;

	outPacket.mPlayerId = m_MyPlayerId;

	outPacket.mRotationX = GGameLogic->GetPlayer( m_MyPlayerId )->GetRotationX();
	outPacket.mRotationY = GGameLogic->GetPlayer( m_MyPlayerId )->GetRotationY();
	outPacket.mRotationZ = GGameLogic->GetPlayer( m_MyPlayerId )->GetRotationZ();

	DDNetwork::GetInstance()->Write( (const char*)&outPacket, outPacket.mSize );
}

void NetworkManager::RegisterHandles()
{
	// ���⿡�� �ڵ鷯�� �������
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_LOGIN, HandleLoginResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_ACCELERATION, HandleAccelerationResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_STOP, HandleStopResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_ROTATION, HandleRotationResult );
	DDNetwork::GetInstance()->RegisterHandler( PKT_SC_SYNC, HandleSyncResult );
}

void NetworkManager::HandleLoginResult( DDPacketHeader& pktBase )
{
	// �ϴ� ���⼭ �󸶸�ŭ �о�ͼ� �ش� ��Ŷ�� �����ϰ�
	LoginResult inPacket = reinterpret_cast<LoginResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

	// ������� player�� ���� �α����� ���
	m_MyPlayerId = inPacket.mPlayerId;

	if ( !GGameLogic->AddPlayer( m_MyPlayerId ) )
	{
		// ��� �ұ��?
	}
}

void NetworkManager::HandleAccelerationResult( DDPacketHeader& pktBase )
{
	AccelerarionResult inPacket = reinterpret_cast<AccelerarionResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

}

void NetworkManager::HandleStopResult( DDPacketHeader& pktBase )
{
	StopResult inPacket = reinterpret_cast<StopResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

}

void NetworkManager::HandleRotationResult( DDPacketHeader& pktBase )
{
	RotationResult inPacket = reinterpret_cast<RotationResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

}

void NetworkManager::HandleSyncResult( DDPacketHeader& pktBase )
{
	SyncResult inPacket = reinterpret_cast<SyncResult&>( pktBase );
	DDNetwork::GetInstance()->GetPacketData( (char*)&inPacket, inPacket.mSize );

}