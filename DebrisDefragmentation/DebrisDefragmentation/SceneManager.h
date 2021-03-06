﻿#pragma once
#include "GameMacro.h"
#include "GameOption.h"

class DDScene;
class PlayScene;
class LoadingScene;
class ResultScene;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	CREATE_FUNC( SceneManager );

	DDScene*	CreateScene( std::wstring sceneName, SceneType scene );
	PlayScene*	GetPlayScene() { return ( PlayScene* )m_PlayScene; }

private :

	void		AddSceneToDirector( DDScene* scene );
	void		ClearAllScene();
	// 조심해!!	
	// scene을 vector같은걸로 추가가능하게 아니면 map으로 name , scene 묶어서 사용해야할듯
	// 최카멕 : 스택을 써!
	// 일단은 그냥 logic그대로 옮깁니다.
	// 이거... Scene Director한테 있는 기능인데요??

	PlayScene*			m_PlayScene = nullptr;
	LoadingScene*		m_LoadingScene = nullptr;
	ResultScene*		m_ResultScene = nullptr;

};

extern std::shared_ptr<SceneManager> GSceneManager;

