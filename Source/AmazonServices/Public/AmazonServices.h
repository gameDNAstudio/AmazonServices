// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#pragma once

#include "OnlineSubsystem.h"
#include "OnlineSubsystemImpl.h"
#include "AmazonServicesModule.h"
#include "AmazonServicesPackage.h"
#include "OnlineAsyncTaskManagerAmazonServices.h"

/** Forward declarations of all interface classes */
typedef TSharedPtr<class FOnlineStoreAmazonServices, ESPMode::ThreadSafe> FOnlineStoreAmazonServicesPtr;
class FRunnableThread;

/**
 * Amazon subsystem
 */
class AMAZONSERVICES_API  FAmazonServices :
	public FOnlineSubsystemImpl

{
	class FOnlineFactoryAmazonServices* AmazonServicesFactory;

	/** Interface to the In-App Purchases */
	FOnlineStoreAmazonServicesPtr StoreInterface;

	/** Used to toggle between 1 and 0 */
	int TickToggle;

PACKAGE_SCOPE:

	/** Only the factory makes instances */
	FAmazonServices();

	/** Online async task runnable */
	TUniquePtr<class FOnlineAsyncTaskManagerAmazonServices> OnlineAsyncTaskThreadRunnable;

	/** Online async task thread */
	TUniquePtr<FRunnableThread> OnlineAsyncTaskThread;

public:
	// IOnlineSubsystem

	virtual IOnlineSessionPtr GetSessionInterface() const override
	{
		return NULL;
	}
	virtual IOnlineFriendsPtr GetFriendsInterface() const override
	{
		return NULL;
	}
	virtual IOnlinePartyPtr GetPartyInterface() const override
	{
		return NULL;
	}
	virtual IOnlineGroupsPtr GetGroupsInterface() const override
	{
		return nullptr;
	}
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override
	{
		return NULL;
	}
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override
	{
		return NULL;
	}
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override
	{
		return NULL;
	}
	virtual IOnlineVoicePtr GetVoiceInterface() const override
	{
		return NULL;
	}
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override
	{
		return NULL;
	}
	virtual IOnlineTimePtr GetTimeInterface() const override
	{
		return NULL;
	}
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override
	{
		return NULL;
	}

	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override
	{
		return NULL;
	}

	virtual IOnlineIdentityPtr GetIdentityInterface() const override
	{
		return NULL;
	}

	virtual IOnlineStorePtr GetStoreInterface() const override;

	virtual IOnlineEventsPtr GetEventsInterface() const override
	{
		return NULL;
	}

	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override
	{
		return NULL;
	}

	virtual IOnlineSharingPtr GetSharingInterface() const override
	{
		return NULL;
	}

	virtual IOnlineUserPtr GetUserInterface() const override
	{
		return NULL;
	}

	virtual IOnlineMessagePtr GetMessageInterface() const override
	{
		return NULL;
	}

	virtual IOnlinePresencePtr GetPresenceInterface() const override
	{
		return NULL;
	}

	virtual IOnlineChatPtr GetChatInterface() const override
	{
		return NULL;
	}

	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override
	{
		return NULL;
	}

	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override
	{
		return NULL;
	}

	virtual IOnlinePurchasePtr GetPurchaseInterface() const override
	{
		return NULL;
	}

	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	// FTickerBaseObject

	virtual bool Tick(float DeltaTime) override;

	// FAmazonServices

	/**
	 * Destructor
	 */
	virtual ~FAmazonServices();

	/**
	 * @return whether this subsystem is enabled or not
	 */
	bool IsEnabled();

	/** Return the async task manager owned by this subsystem */
	class FOnlineAsyncTaskManagerAmazonServices* GetAsyncTaskManager() { return OnlineAsyncTaskThreadRunnable.Get(); }

	/**
	* Add an async task onto the task queue for processing
	* @param AsyncTask - new heap allocated task to process on the async task thread
	*/
	void QueueAsyncTask(class FOnlineAsyncTask* AsyncTask);
};

typedef TSharedPtr<FAmazonServices, ESPMode::ThreadSafe> FAmazonServicesPtr;
