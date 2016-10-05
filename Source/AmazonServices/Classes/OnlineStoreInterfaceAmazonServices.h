// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#pragma once

#include "OnlineStoreInterface.h"


/*enum EGooglePlayBillingResponseCode
{
	Ok					= 0,
	Cancelled			= 1,
	BillingUnavailable	= 3,
	ItemUnavailable		= 4,
	DeveloperError		= 5,
	Error				= 6,
	ItemAlreadyOwned	= 7,
	ItemNotOwned		= 8,
};*/


/**
 * The resulting state of an iap transaction
 */
namespace EInAppPurchaseResult
{
	enum Type
	{
		Succeeded = 0,
		RestoredFromServer,
		Failed,
		Cancelled,
	};
}


/**
 * Implementation of the Platform Purchase receipt. For this we provide an identifier and the encrypted data.
 */
 /*class FGooglePlayPurchaseReceipt : public IPlatformPurchaseReceipt
 {
 public:
	 // Product identifier
	 FString Identifier;

	 // The encrypted receipt data
	 FString Data;
 };*/


 /**
  *	FOnlineStoreAmazonServices - Implementation of the online store for AmazonServices
  */
class FOnlineStoreAmazonServices : public IOnlineStore
{
public:
	/** C-tor */
	FOnlineStoreAmazonServices(FAmazonServices* InSubsystem);
	/** Destructor */
	virtual ~FOnlineStoreAmazonServices();

	// Begin IOnlineStore 
	virtual bool QueryForAvailablePurchases(const TArray<FString>& ProductIDs, FOnlineProductInformationReadRef& InReadObject) override;
	virtual bool BeginPurchase(const FInAppPurchaseProductRequest& ProductRequest, FOnlineInAppPurchaseTransactionRef& InReadObject) override;
	virtual bool IsAllowedToMakePurchases() override;
	virtual bool RestorePurchases(const TArray<FInAppPurchaseProductRequest>& ConsumableProductFlags, FOnlineInAppPurchaseRestoreReadRef& InReadObject) override
	{
		// Not Yet Implemented
		return false;
	}
	// End IOnlineStore 

	void ProcessQueryAvailablePurchasesResults(bool bInSuccessful, const TArray<FInAppPurchaseProductInfo>& AvailablePurchases);
	void ProcessPurchaseResult(bool bInSuccessful, const FString& InProductId, const FString& InReceiptData);

private:

	/** Pointer to owning subsystem */
	FAmazonServices* Subsystem;

	/** The current query for iap async task */
	class FOnlineAsyncTaskAmazonServicesQueryInAppPurchases* CurrentQueryTask;

	/** Delegate fired when a query for purchases has completed, whether successful or unsuccessful */
	FOnQueryForAvailablePurchasesComplete OnQueryForAvailablePurchasesCompleteDelegate;

	/** Delegate fired when a purchase transaction has completed, whether successful or unsuccessful */
	FOnInAppPurchaseComplete OnPurchaseCompleteDelegate;

	/** Cached in-app purchase query object, used to provide the user with product information attained from the server */
	FOnlineProductInformationReadPtr ReadObject;

	/** Cached in-app purchase transaction object, used to provide details to the user, of the product that has just been purchased. */
	FOnlineInAppPurchaseTransactionPtr CachedPurchaseStateObject;
};

typedef TSharedPtr<FOnlineStoreAmazonServices, ESPMode::ThreadSafe> FOnlineStoreAmazonServicesPtr;
