﻿#include "stdafx.h"
#include "DebrisStormEvent.h"
#include "Physics.h"
#include "ActorManager.h"
#include "ObjectTable.h"

DebrisStormEvent::DebrisStormEvent()
{
	m_Event = GameEvent::DEBRIS_STORM;
}


DebrisStormEvent::~DebrisStormEvent()
{
}



void DebrisStormEvent::Init()
{
	InGameEvent::Init();
	// 데브리 폭풍은 지구궤도를 돈다고 보고 y를 0으로
	m_Direction.y = 0.0f;
}


void DebrisStormEvent::Run()
{
	// 플레이어들에게 영향을 준다.
	// 플레이어를 순회하면서 바운딩 박스의 모든 점이 가려지는지 확인
	// 안 가려지면 죽음으로 판정
	// 안 가려지면 지속 체력 감소

	// iss에서 개별 모듈의 충돌박스를 받아야 되는데
	// iss는 ActorManager가 멤버로 들고 있으므로 요청할 수 밖에 없을 듯

	// ISS module 충돌박스 정보 갱신


	printf_s( "Debris storm\n" );
}