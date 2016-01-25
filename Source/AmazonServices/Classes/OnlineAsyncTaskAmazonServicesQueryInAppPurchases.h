// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#pragma once

#include "OnlineAsyncTaskManager.h"

class FAmazonServices;

class FOnlineAsyncTaskAmazonServicesQueryInAppPurchases : public FOnlineAsyncTaskBasic<FAmazonServices>
{
public:
	FOnlineAsyncTaskAmazonServicesQueryInAppPurchases(
		FAmazonServices* InSubsystem,
		const TArray<FString> InProductIds,
		const TArray<bool> InIsConsumableFlags);

	// FOnlineAsyncItem
	virtual FString ToString() const override { return TEXT("QueryInAppPurchases"); }
	virtual void Finalize() override;
	virtual void TriggerDelegates() override;

	// FOnlineAsyncTask
	virtual void Tick() override;

	// FOnlineAsyncTaskAmazonServicesQueryInAppPurchases
	void ProcessQueryAvailablePurchasesResults(bool bInSuccessful);

private:

	// The product ids provided for this task
	const TArray<FString> ProductIds;

	// The consume flags provided for this task
	const TArray<bool> IsConsumableFlags;

	/** Flag indicating that the request has been sent */
	bool bWasRequestSent;
};
