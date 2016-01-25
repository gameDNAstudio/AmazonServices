// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#include "AmazonServicesPCH.h"
#include "AmazonServices.h"
#include "AmazonServicesModule.h"

// FAmazonServicesModule
IMPLEMENT_MODULE(FAmazonServicesModule, AmazonServices);

/**
 * Class responsible for creating instance(s) of the subsystem
 */
class FOnlineFactoryAmazonServices : public IOnlineFactory
{
public:

	FOnlineFactoryAmazonServices() {}
	virtual ~FOnlineFactoryAmazonServices() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
		FAmazonServicesPtr OnlineSub = MakeShareable(new FAmazonServices());
		if (OnlineSub->IsEnabled())
		{
			if (!OnlineSub->Init())
			{
				UE_LOG(LogOnline, Warning, TEXT("Amazon API failed to initialize!"));
				OnlineSub->Shutdown();
				OnlineSub = NULL;
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("Amazon API disabled!"));
			OnlineSub->Shutdown();
			OnlineSub = NULL;
		}

		return OnlineSub;
	}
};

void FAmazonServicesModule::StartupModule()
{
	UE_LOG(LogOnline, Log, TEXT("Amazon Startup!"));

	AmazonServicesFactory = new FOnlineFactoryAmazonServices();

	// Create and register our singleton factory with the main online subsystem for easy access
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(AMAZONSERVICES_SUBSYSTEM, AmazonServicesFactory);
}

void FAmazonServicesModule::ShutdownModule()
{
	UE_LOG(LogOnline, Log, TEXT("Amazon Shutdown!"));

	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(AMAZONSERVICES_SUBSYSTEM);

	delete AmazonServicesFactory;
	AmazonServicesFactory = NULL;
}

IOnlineStorePtr FAmazonServices::GetStoreInterface() const
{
	return StoreInterface;
}

bool FAmazonServices::Tick(float DeltaTime)
{
	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}

	if (OnlineAsyncTaskThreadRunnable)
	{
		OnlineAsyncTaskThreadRunnable->GameTick();
	}

	return true;
}

bool FAmazonServices::Init()
{
	StoreInterface = MakeShareable(new FOnlineStoreAmazonServices(this));

	OnlineAsyncTaskThreadRunnable.Reset(new FOnlineAsyncTaskManagerAmazonServices);
	OnlineAsyncTaskThread.Reset(FRunnableThread::Create(OnlineAsyncTaskThreadRunnable.Get(), *FString::Printf(TEXT("OnlineAsyncTaskThread %s"), *InstanceName.ToString())));

	return true;
}

bool FAmazonServices::Shutdown()
{
	UE_LOG(LogOnline, Display, TEXT("FAmazonServices::Shutdown()"));
	StoreInterface = NULL;

	OnlineAsyncTaskThread.Reset();
	OnlineAsyncTaskThreadRunnable.Reset();

	FOnlineSubsystemImpl::Shutdown();
	return true;
}

FString FAmazonServices::GetAppId() const
{
	return TEXT("AmazonServices");
}

bool FAmazonServices::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

bool FAmazonServices::IsEnabled(void)
{
	bool bEnableAmazon = true;
	GConfig->GetBool(TEXT("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"), TEXT("bEnableAmazonSupport"), bEnableAmazon, GEngineIni);
	return bEnableAmazon;
}

FAmazonServices::FAmazonServices() :
	StoreInterface(NULL),
	TickToggle(0)
{

}

FAmazonServices::~FAmazonServices()
{

}

void FAmazonServices::QueueAsyncTask(FOnlineAsyncTask* AsyncTask)
{
	check(OnlineAsyncTaskThreadRunnable);
	OnlineAsyncTaskThreadRunnable->AddToInQueue(AsyncTask);
}
