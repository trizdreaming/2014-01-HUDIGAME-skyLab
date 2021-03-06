﻿#pragma once
#include "GameOption.h"
#include "GameMacro.h"
#include "Transform.h"
#include "Rigidbody.h"

class ClassComponent
{
public:
	ClassComponent();
	virtual ~ClassComponent();

	static std::shared_ptr<ClassComponent> Create( CharacterClass className );

	// 사용자들이 사용하는 스킬들!
	bool	SkillGoForward( int id, D3DXVECTOR3 viewDirection ); // 현재 바라보는 방향으로 가속도 부여
	bool	SkillStop( int id );	// 가속도 및 속도 0으로 변경	

	void	ResetStatus();

	// 바라보는 방향으로 몸을 회전 turn body to viewing direction 
	bool	SkillTurnBody( int id, D3DXVECTOR3 direction );

	// 적절한 스킬 사용 함수를 호출한다.
	virtual bool UseSkill( ClassSkill skill, int id, const D3DXVECTOR3& direction ) = 0;

	bool	SkillPush( int id, const D3DXVECTOR3& direction );
	
	float	GetFuel() const { return m_Fuel; }
	void	SetFuel( float val ) { m_Fuel = val < 0.0f ? 0.0f : val; }

	float	GetOxygen() const { return m_Oxygen; }
	void	SetOxygen( float val ){ m_Oxygen = val < 0.0f ? 0.0f : val; }
	void	IncreaseOxygen( float val ) { m_Oxygen = ( m_Oxygen + val < 0 ) ? 0.0f : ( m_Oxygen + val ); }
	void	IncreaseFuel( float val ) { m_Fuel = ( m_Fuel + val < 0 ) ? 0.0f : ( m_Fuel + val ); }

	float	GetHP() const { return m_HP; }
	void	SetHP( float val ) { m_HP = val; }

	bool	IsAlive() { return ( m_HP > 0.0f ) ? true : false; }
	bool	UseFuel( float fuelUse );
	bool	UseOxygen( float oxygenUse );

	void	SetGlobalCooldown( float time ) { m_GlobalCooldown = ( m_GlobalCooldown < time ) ? time : m_GlobalCooldown; }
	float	GetGlobalCooldown() { return m_GlobalCooldown; }

	void	SetMovementControlCooldown( float time ) { m_MovementControlCooldown = time; }

	// 주기적으로 처리해야 하는 일들
	// 예를 들어 산소량 감소와 같은 일을 처리 
	void	Update( float dt );

	CharacterClass GetCharacterClassName() { return m_ClassName; }

	float	GetResource() const { return m_Resource; }
	void	SetResource( float val ) { m_Resource = val; }
	void	IncreaseResource( float val ) { m_Resource += val; }

protected:	
	void		SetCooldown( ClassSkill skillType );
	
	// 산소 및 추진체 관련
	float		m_Oxygen = DEFAULT_OXYGEN;
	float		m_Fuel = DEFAULT_FUEL;
	float		m_HP = DEFAULT_HP;

	// 엔지니어에 있어야하나..
	float		m_Resource = .0f;
	CharacterClass m_ClassName = CharacterClass::NO_CLASS;

	float		m_GlobalCooldown = 0.0f;
	std::array<float, TOTAL_SKILL_NUMBER> m_CooldownTable;
	float		m_MovementControlCooldown = 0.0f;

	virtual void DoPeriodWork( float dTime ) { UNREFERENCED_PARAMETER( dTime ); }
};

