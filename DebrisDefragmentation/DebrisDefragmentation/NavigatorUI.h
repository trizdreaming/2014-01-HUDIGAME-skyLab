﻿#pragma once

#include "DDUIModule.h"

class NavigatorUI :
	public DDUIModule
{
public:
	NavigatorUI();
	virtual ~NavigatorUI();

	void Update( float dt );

	bool GenerateUI();
};

