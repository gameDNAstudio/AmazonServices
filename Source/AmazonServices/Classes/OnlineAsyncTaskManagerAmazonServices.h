// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#pragma once

#include "OnlineAsyncTaskManager.h"
#include "AmazonServicesPackage.h"

/**
 *	Google Play version of the async task manager
 */
class FOnlineAsyncTaskManagerAmazonServices : public FOnlineAsyncTaskManager
{
public:

	FOnlineAsyncTaskManagerAmazonServices() {}

	// FOnlineAsyncTaskManager
	virtual void OnlineTick() override {}
};
