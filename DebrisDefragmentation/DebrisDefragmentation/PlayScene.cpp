﻿#include "stdafx.h"
#include "PlayScene.h"
#include "Debris.h"
#include "DDInputSystem.h"
#include "DDApplication.h"
#include "DDLight.h"
#include "DDUI.h"
#include "Player.h"
#include "NetworkManager.h"
#include "PlayerManager.h"
#include "UIManager.h"
#include "ObjectManager.h"
#include "SkyBox.h"
#include "GameData.h"
#include "ObjectISS.h"

PlayScene::PlayScene()
{
	m_SceneName = L"DefaultPlayScene";
}

PlayScene::PlayScene( std::wstring sceneName )
{
	m_SceneName = sceneName;
}

PlayScene::~PlayScene()
{
	delete GObjectManager;
	delete GNetworkManager;
}


void PlayScene::InitModelPool()
{
	m_ModelPool.InitModel( ModelType::DEBRIS, L"debris.x" );
	m_ModelPool.InitModel( ModelType::SKYBOX, L"skybox.x" );
	m_ModelPool.InitModel( ModelType::EARTH, L"earth.x" );
	m_ModelPool.InitModel( ModelType::PLAYER_MODEL, L"spaceMan.x" );
	m_ModelPool.InitModel( ModelType::ISS, L"iss.x" );
	m_ModelPool.InitModel( ModelType::DISPENSER, L"dispenser.x" );
}


void PlayScene::Init()
{	
	if ( !g_GameData->Init() )
	{
		// 무조건 ok 해야 하는 과정
		// 에러 발생시 프로그램 종료
		return;
	}

	// model pool 초기화
	InitModelPool();
	
	// init objects
	m_pDirectonalLight = DDLight::Create();
	
// 	unsigned int cp = GGameLogic->GetCurrentPlayers();
// 	for ( unsigned int i = 0; i < cp; ++i )
// 	{
// 		AddChild( GGameLogic->GetPlayer(i) );
// 	}	
	AddChild( m_pDirectonalLight );
	
	// test skybox
	//SkyBox* sb = SkyBox::Create( L"skybox.x" );
	SkyBox* sb = SkyBox::Create();
	sb->SetModelMesh( m_ModelPool.GetModel( ModelType::SKYBOX ) );
	AddChild( sb );

	// earth :: 바닥에 지구를 깐다!
	//DDModel* earth = DDModel::Create( L"earth.x" );
	DDModel* earth = DDModel::Create();
	earth->SetModelMesh( m_ModelPool.GetModel( ModelType::EARTH ) );
	earth->GetTransform().SetPosition( 0, -800, 0 );
	AddChild( earth );

	// test debris
	// 이거 할당하느라 느리다. 테스트 끝나면 지울 것
	
	unsigned int debrisCount = g_GameData->GetDebrisNumber();
	//Debris* tempDebris = nullptr;
	//tempDebris = Debris::Create( L"debris.x" );	

	for ( unsigned int i = 0; i < debrisCount; ++i )
	{
		//tempDebris = Debris::Create( L"debris.x" );
		Debris* newDebri = Debris::Create();
		newDebri->SetModelMesh( m_ModelPool.GetModel( ModelType::DEBRIS ) );
		newDebri->GetTransform().SetPosition(
			static_cast<float>( ( rand() % 2000 ) - 1000 ) / 20,
			static_cast<float>( ( rand() % 2000 ) - 1000 ) / 20,
			static_cast<float>( ( rand() % 2000 ) - 1000 ) / 20
			);
		newDebri->GetTransform().SetScale( 0.1f, 0.1f, 0.1f );

		AddChild( newDebri );
	}
	
	// 조심해!
	// 내부 구현 아직 제대로 안 된 상태
	GObjectManager = new ObjectManager;

	m_pObjectISS = ObjectISS::Create();
	m_pObjectISS->Init();
	AddChild( m_pObjectISS );

	GObjectManager->RegisterObjectISS( m_pObjectISS );
	
	GNetworkManager = new NetworkManager;
	GNetworkManager->Init();
	GNetworkManager->Connect();

// 	RECT rect;
// 	GetWindowRect( DDApplication::GetInstance()->GetHWND(), &rect );
// 	ClipCursor( &rect );

	// UI를 추가합니다.
	AddUI();
}

// 조심해!!
// 로직 부분은 나중에 게임 매니저에 구현하는 걸로~!
// 4. 23. UI 업데이트 하는 부분을 추가했습니다. 
void PlayScene::UpdateItSelf( float dTime )
{
	UNREFERENCED_PARAMETER( dTime );
	
	// player가 로그인해있지 않으면 루프는 패스.
	if ( !GNetworkManager->IsPlayerLogon() )
	{
		return;
	}

	// 캐릭터가 죽어있으면.. 아래는 실행이 안되고 종료..
	// 캐릭터 컴포넌트까지 접근이 좀 구구절절하다...ㅠㅠ	
	if ( !GPlayerManager->GetPlayer( GNetworkManager->GetMyPlayerId() )->GetClassComponent()->IsAlive() )
	{
		// space 누르면 respawn request보낸다.
		if ( KEY_DOWN == GetKeyState( VK_SPACE ) )
		{
			// 조심해!!
			// 일단은 striker만 보내보자..
			// 뭘 보낼지 선택하는 부분은 나중에 추가할 것!
			GNetworkManager->SendRespawnRequest(CharacterClass::STRIKER);
		}

		// player 조정이 playscene에 있으므로 여기서 return 함.
		// 동시에 player의 updatable을 꺼줘야 player가 계속 sendDeadrequest하는 것을 막을 수 있음..
		return;
	}


	// 키 고유 넘버는 아래를 참조하자 
	// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	// 현재 w키가 눌렸는지 확인한다
	// 눌렸으면 캐릭터 가속도 세팅하라고 시킴
	// s키가 눌렸다면 정지	
	if ( KEY_DOWN == GetKeyState( VK_W ) )
	{
		// w : 현재 몸체가 바라보는 방향으로 가속
		GNetworkManager->SendAcceleration();
	}
	
	if ( KEY_DOWN == GetKeyState( VK_S ) )
	{
		// s : 정지!
		GNetworkManager->SendStop();
	}

	if ( KEY_DOWN == GetKeyState( VK_1 ) )
	{
		// 1 : 밀기 스킬 시전!
		GNetworkManager->SendUsingSkill( ClassSkill::PUSH );
	}

	if ( KEY_DOWN == GetKeyState( VK_2 ) )
	{
		// 2 : 당기기 스킬 시전!
		GNetworkManager->SendUsingSkill( ClassSkill::PULL );
	}

	if ( KEY_DOWN == GetKeyState( VK_3 ) )
	{
		// 3 : 점령 스킬 시전!
		GNetworkManager->SendUsingSkill( ClassSkill::OCCUPY );
	}

	if ( KEY_DOWN == GetKeyState( VK_4 ) )
	{
		// 4 : 파괴 스킬 시전!
		GNetworkManager->SendUsingSkill( ClassSkill::DESTROY );
	}

	if ( KEY_DOWN == GetKeyState( VK_5 ) )
	{
		// 4 : 파괴 스킬 시전!
		GNetworkManager->SendUsingSkill( ClassSkill::SET_DISPENSER );
	}

	if ( KEY_DOWN == GetKeyState( VK_SPACE ) )
	{
		GNetworkManager->SendTurnBody();
	}
	// 문교수 커피가 든 컵을 들고 오다가 커피가 손에 닿자...
	// 문교수 : 보강 간섭이 일어나지 않게 엇박자로 걸었어야 되는데...라고 말씀하셨다.
	// ㅋㅋㅋㅋ 카멕님이 적으셨습니까

	// 마우스 좌표 변화를 받아온다
	// 변화량을 기준으로 캐릭터한데 회전하라고 시킨다.		
	// 고개 돌리기는 서버로 보낼 필요 없이 클라이언트에서만 적용
	
	DDPoint currentMousePos = GetMousePosition();
	GPlayerManager->GetPlayer( GNetworkManager->GetMyPlayerId() )->LookAt(
		currentMousePos.GetY() - m_PrevMousePosition.GetY(),
		currentMousePos.GetX() - m_PrevMousePosition.GetX(),
		0
		);

	MousePointer( MOUSE_POINTER_ON, currentMousePos );
	UpdateUI();	
	UpdateISS();
}

// Mouse Pointer 가릴지 살려둘지 결정
// true 시키면 커서가 계속 가운데로 이동함
void PlayScene::MousePointer( bool mousePointer, DDPoint currentMousePos )
{
	if ( !mousePointer )
	{
		// 마우스 커서 500, 500에 놓기
		/// config.h
		POINT pt = { DDApplication::GetInstance()->GetScreenWidth() / 2, DDApplication::GetInstance()->GetScreenHeight() / 2 };
		::ClientToScreen( DDApplication::GetInstance()->GetHWND(), &pt );
		::SetCursorPos( pt.x, pt.y );

		// 이전 포지션 위치를 500, 500에 놓기
		m_PrevMousePosition = DDPoint( static_cast<float>( DDApplication::GetInstance()->GetScreenWidth() / 2 ), static_cast<float>( DDApplication::GetInstance()->GetScreenHeight() / 2) );

		// 커서 숨기기
		::ShowCursor( false );
	}
	else
	{
		m_PrevMousePosition = currentMousePos;
	}
}

void PlayScene::AddUI()
{
	// UI 생성 및 추가 부분을 UI Manager가 처리하도록 뺐음
	// 인자로 UI Tag 를 받도록 바꿈
	AddChild( g_UIManager->CreateUI( ClientUITag::UI_OXYGEN_TAG, UI_OXYGEN_POSITION_X, UI_OXYGEN_POSITION_Y ) );
	AddChild( g_UIManager->CreateUI( ClientUITag::UI_FRAME_TAG, UI_FRAME_POSITION_X, UI_FRAME_POSITION_Y ) );
	// frame 크기는 60%로
	g_UIManager->GetUI( ClientUITag::UI_FRAME_TAG )->GetTransform().SetScale( 0.6f );
	AddChild( g_UIManager->CreateUI( ClientUITag::UI_FUEL_TAG, UI_FUEL_POSITION_X, UI_FUEL_POSITION_Y ) );
}

void PlayScene::UpdateUI()
{
	unsigned int myId = GNetworkManager->GetMyPlayerId();
	
	// 초기화 덜 됨
// 	if ( myId >= g_PlayerManager->GetNumberOfCurrentPlayers() )
// 	{
// 		return;
// 	}

	float currentOxygen = GPlayerManager->GetPlayer( myId )->GetOxygen();
	float currentFuel = GPlayerManager->GetPlayer( myId )->GetGas();

	// 현재는 front가 pFuelUI
	g_UIManager->GetUI( ClientUITag::UI_OXYGEN_TAG )->GetTransform().SetScale( currentOxygen /  DEFAULT_OXYGEN , 1, 1 );
	g_UIManager->GetUI( ClientUITag::UI_FUEL_TAG )->GetTransform().SetScale( currentFuel / DEFAULT_FUEL , 1, 1 );
}

void PlayScene::UpdateISS()
{

}
