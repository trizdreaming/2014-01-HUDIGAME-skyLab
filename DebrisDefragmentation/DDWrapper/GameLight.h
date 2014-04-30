﻿#pragma once
#include "GameObject.h"
#include "DDLight.h"

namespace DDWrapper
{

	public ref class GameLight : public GameObject
	{
	public:
		GameLight();
		~GameLight();

	protected:
		DDLight* Create() { return DDLight::Create(); };
	};
}