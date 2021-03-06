﻿#include "stdafx.h"
#include "GameData.h"
#include "GameMacro.h"

using namespace rapidjson;

std::shared_ptr<GameData> g_GameData = nullptr;

const std::string GameData::GAME_DATA_PATH = ".\\Resources\\Json\\Config.json";

GameData::GameData()
{
}


GameData::~GameData()
{
}

bool GameData::Init()
{
// 	std::ifstream srcFile( GAME_DATA_PATH.c_str() );
// 	std::string json( std::istreambuf_iterator<char>( srcFile ), ( std::istreambuf_iterator<char>() ) );
// 
// 	m_Document.Parse<0>( json.c_str() );

	// 30줄이면 될 걸 왜 3줄로 짰어요??
	// 깨알같구만 ㅋㅋㅋㅋ

	// init JSON
	FILE* file = nullptr;

	std::string JsonRawData;

	if ( !fopen_s( &file, GAME_DATA_PATH.c_str(), "r" ) )
	{
		char line[100] = { 0, };
		while ( !feof( file ) )
		{
			fgets( line, 100, file );
			JsonRawData += line;
		}

		fclose( file );
	}

	// 설정 파일 로드 실패시 false 리턴
	else
	{
		MessageBox( NULL, L"JSON File Load Fail!", L"Message Box", MB_OK );
		assert( false ); // 디버깅 모드에서 걸리도록
		return false;
	}

	// 방어 코드 : JSON 파일 앞에 { 가 나오기 전까지 이상한 문자들이 들어오면 다 자름
	int startPosition = JsonRawData.find_first_of( '{' );
	JsonRawData.erase( 0, ( startPosition > 0 ? startPosition : 0 ) );

	// Json File Mapping
	m_Document.Parse<0>( JsonRawData.c_str() );
	// 에러 체크
	assert( m_Document.IsObject() );
	return true;
}


int GameData::GetDebrisNumber()
{
	return m_Document[JSON_KEY_MAP.at( JSON_DEBRIS_NUMBER ).c_str()].GetInt();
}


D3DXVECTOR3 GameData::GetISSPosition( ISSModuleName name )
{
	const char* iss = JSON_KEY_MAP.at( JSON_ISS ).c_str();
	const char* module = JSON_ISS_PART_NAME.at( name ).c_str();

	Value& position = m_Document[iss]["Position"][module];
	// 조심해! double 에서 float으로 바꾸고 있음!
	return D3DXVECTOR3(
		static_cast<float>( position[SizeType( 0 )].GetDouble() ),
		static_cast<float>( position[SizeType( 1 )].GetDouble() ),
		static_cast<float>( position[SizeType( 2 )].GetDouble() ) );
}


D3DXVECTOR3 GameData::GetISSModuleAxisLength( ISSModuleName name )
{
	const char* iss = JSON_KEY_MAP.at( JSON_ISS ).c_str();
	const char* module = JSON_ISS_PART_NAME.at( name ).c_str();

	Value& position = m_Document[iss]["AxisLength"][module];

	return D3DXVECTOR3(
		static_cast<float>( position[SizeType( 0 )].GetDouble() ),
		static_cast<float>( position[SizeType( 1 )].GetDouble() ),
		static_cast<float>( position[SizeType( 2 )].GetDouble() ) );
}
