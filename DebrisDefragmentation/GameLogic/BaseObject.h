﻿/*
	게임 내부에서 인터랙션이 발생하는 모든 것들의 최상위 클래스
	위치와 크기, 회전 등의 정보를 가지고 있다. (DDFramework의 Object와 비슷) 
	움직임은 하위 클래스에서 구현할 것
	2014. 4. 21 최경욱
*/

#pragma once

#include "GameConfig.h"
#include "CollisionBox.h"

class BaseObject
{
public:
	BaseObject();
	virtual ~BaseObject();

	void Update( float dTime );

	void SetPosition( float x, float y, float z ) { m_Position = D3DXVECTOR3( x, y, z ); }
	void SetScale( float scaleX, float scaleY, float scaleZ ) { m_Scale = D3DXVECTOR3( scaleX, scaleY, scaleZ ); }
	void SetRotation( float rotationX, float rotationY, float rotationZ ) { m_Rotation = D3DXVECTOR3( rotationX, rotationY, rotationZ ); }

	D3DXVECTOR3 GetPosition() const { return m_Position; }
	D3DXVECTOR3 GetScale() const { return m_Scale; }
	D3DXVECTOR3 GetRotation() const { return m_Rotation; }
	void		IncreaseRotation(float x, float y, float z)	{ m_Rotation += D3DXVECTOR3( x, y, z ); }
	
	D3DXVECTOR3 GetViewDirection();
	CollisionBox GetCollisionBox() const { return m_CollisionBox; }

protected:
	D3DXMATRIXA16	m_Matrix;
	D3DXVECTOR3		m_Position{ 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3		m_Rotation{ 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3		m_Scale{ 1.0f, 1.0f, 1.0f };
	CollisionBox	m_CollisionBox;

private:
	virtual void UpdateItSelf( float dTime ) { UNREFERENCED_PARAMETER( dTime ); }
};
