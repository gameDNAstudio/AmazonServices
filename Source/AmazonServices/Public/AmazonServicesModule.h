// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#pragma once

#include "ModuleInterface.h"
#include "ModuleManager.h"
#include "AmazonServicesPackage.h"

/**
 * Module used for talking with an Amazon service via Http requests
 */
class FAmazonServicesModule :
	public IModuleInterface
{

private:

	/** Class responsible for creating instance(s) of the subsystem */
	class FOnlineFactoryAmazonServices* AmazonServicesFactory;

public:

	/**
	 * Constructor
	 */
	FAmazonServicesModule() :
		AmazonServicesFactory(NULL)
	{
	}

	/**
	 * Destructor
	 */
	virtual ~FAmazonServicesModule()
	{
	}

	// IModuleInterface

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
};

typedef TSharedPtr<FAmazonServicesModule> FAmazonServicesModulePtr;
