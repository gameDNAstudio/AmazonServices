// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#include "AmazonServicesPCH.h"
#include "TaskGraphInterfaces.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include <android_native_app_glue.h>
#endif

////////////////////////////////////////////////////////////////////
/// FOnlineStoreAmazonServices implementation


FOnlineStoreAmazonServices::FOnlineStoreAmazonServices(FAmazonServices* InSubsystem)
	: Subsystem(InSubsystem)
	, CurrentQueryTask(nullptr)
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::FOnlineStoreAmazonServices"));

#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IapSetupServiceAmazon", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
#endif
}


FOnlineStoreAmazonServices::~FOnlineStoreAmazonServices()
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::~FOnlineStoreAmazonServices"));
}


bool FOnlineStoreAmazonServices::IsAllowedToMakePurchases()
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::IsAllowedToMakePurchases"));

#if PLATFORM_ANDROID
	bool bResult = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IapIsAllowedToMakePurchasesAmazon", "()Z", false);
		bResult = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return bResult;
#else
	return false;
#endif
}


bool FOnlineStoreAmazonServices::QueryForAvailablePurchases(const TArray<FString>& ProductIds, FOnlineProductInformationReadRef& InReadObject)
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::QueryForAvailablePurchases"));

	ReadObject = InReadObject;
	ReadObject->ReadState = EOnlineAsyncTaskState::InProgress;

	TArray<bool> ConsumableFlags;
	ConsumableFlags.AddZeroed(ProductIds.Num());

	CurrentQueryTask = new FOnlineAsyncTaskAmazonServicesQueryInAppPurchases(
		Subsystem,
		ProductIds,
		ConsumableFlags);
	Subsystem->QueueAsyncTask(CurrentQueryTask);

	return true;
}

void FOnlineStoreAmazonServices::ProcessQueryAvailablePurchasesResults(bool bInSuccessful, const TArray<FInAppPurchaseProductInfo>& AvailablePurchases)
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::ProcessQueryAvailablePurchasesResults"));

	if (ReadObject.IsValid())
	{
		ReadObject->ReadState = bInSuccessful ? EOnlineAsyncTaskState::Done : EOnlineAsyncTaskState::Failed;
		ReadObject->ProvidedProductInformation.Insert(AvailablePurchases, 0);
	}

	CurrentQueryTask->ProcessQueryAvailablePurchasesResults(bInSuccessful);
}


bool FOnlineStoreAmazonServices::BeginPurchase(const FInAppPurchaseProductRequest& ProductRequest, FOnlineInAppPurchaseTransactionRef& InPurchaseStateObject)
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::BeginPurchase"));

	bool bCreatedNewTransaction = false;

	if (IsAllowedToMakePurchases())
	{
		CachedPurchaseStateObject = InPurchaseStateObject;

#if PLATFORM_ANDROID
		if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
		{
			jstring ProductIdentifierFinal = Env->NewStringUTF(TCHAR_TO_UTF8(*ProductRequest.ProductIdentifier));
			static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IapBeginPurchaseAmazon", "(Ljava/lang/String;Z)Z", false);
			bCreatedNewTransaction = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, ProductIdentifierFinal, ProductRequest.bIsConsumable);
			Env->DeleteLocalRef(ProductIdentifierFinal);
		}
#endif

		UE_LOG(LogOnline, Display, TEXT("Created Transaction? - %s"),
			bCreatedNewTransaction ? TEXT("Created a transaction.") : TEXT("Failed to create a transaction."));

		if (!bCreatedNewTransaction)
		{
			UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::BeginPurchase - Could not create a new transaction."));
			CachedPurchaseStateObject->ReadState = EOnlineAsyncTaskState::Failed;
			TriggerOnInAppPurchaseCompleteDelegates(EInAppPurchaseState::Invalid);
		}
		else
		{
			CachedPurchaseStateObject->ReadState = EOnlineAsyncTaskState::InProgress;
		}
	}
	else
	{
		UE_LOG(LogOnline, Display, TEXT("This device is not able to make purchases."));

		InPurchaseStateObject->ReadState = EOnlineAsyncTaskState::Failed;
		TriggerOnInAppPurchaseCompleteDelegates(EInAppPurchaseState::NotAllowed);
	}

	return bCreatedNewTransaction;
}

#if PLATFORM_ANDROID
extern "C" void Java_com_amazon_AmazonIapManager_nativeQueryCompleteAmazon(JNIEnv* jenv, jobject thiz, jboolean bSuccess, jobjectArray productIDs, jobjectArray titles, jobjectArray descriptions, jobjectArray prices)
{
	TArray<FInAppPurchaseProductInfo> ProvidedProductInformation;

	if (jenv && bSuccess)
	{
		jsize NumProducts = jenv->GetArrayLength(productIDs);
		jsize NumTitles = jenv->GetArrayLength(titles);
		jsize NumDescriptions = jenv->GetArrayLength(descriptions);
		jsize NumPrices = jenv->GetArrayLength(prices);

		ensure((NumProducts == NumTitles) && (NumProducts == NumDescriptions) && (NumProducts == NumPrices));

		for (jsize Idx = 0; Idx < NumProducts; Idx++)
		{
			// Build the product information strings.

			FInAppPurchaseProductInfo NewProductInfo;

			jstring NextId = (jstring)jenv->GetObjectArrayElement(productIDs, Idx);
			const char* charsId = jenv->GetStringUTFChars(NextId, 0);
			NewProductInfo.Identifier = FString(UTF8_TO_TCHAR(charsId));
			jenv->ReleaseStringUTFChars(NextId, charsId);
			jenv->DeleteLocalRef(NextId);

			jstring NextTitle = (jstring)jenv->GetObjectArrayElement(titles, Idx);
			const char* charsTitle = jenv->GetStringUTFChars(NextTitle, 0);
			NewProductInfo.DisplayName = FString(UTF8_TO_TCHAR(charsTitle));
			jenv->ReleaseStringUTFChars(NextTitle, charsTitle);
			jenv->DeleteLocalRef(NextTitle);

			jstring NextDesc = (jstring)jenv->GetObjectArrayElement(descriptions, Idx);
			const char* charsDesc = jenv->GetStringUTFChars(NextDesc, 0);
			NewProductInfo.DisplayDescription = FString(UTF8_TO_TCHAR(charsDesc));
			jenv->ReleaseStringUTFChars(NextDesc, charsDesc);
			jenv->DeleteLocalRef(NextDesc);

			jstring NextPrice = (jstring)jenv->GetObjectArrayElement(prices, Idx);
			const char* charsPrice = jenv->GetStringUTFChars(NextPrice, 0);
			NewProductInfo.DisplayPrice = FString(UTF8_TO_TCHAR(charsPrice));
			jenv->ReleaseStringUTFChars(NextPrice, charsPrice);
			jenv->DeleteLocalRef(NextPrice);

			ProvidedProductInformation.Add(NewProductInfo);

			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\nProduct Identifier: %s, Name: %s, Description: %s, Price: %s\n"),
				*NewProductInfo.Identifier,
				*NewProductInfo.DisplayName,
				*NewProductInfo.DisplayDescription,
				*NewProductInfo.DisplayPrice);
		}
	}


	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.ProcessQueryIapResultAmazon"), STAT_FSimpleDelegateGraphTask_ProcessQueryIapResultAmazon, STATGROUP_TaskGraphTasks);

	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]()
	{
		if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get("AmazonServices"))
		{
			// call store implementation to process query results.
			if (FOnlineStoreAmazonServices* StoreInterface = (FOnlineStoreAmazonServices*)OnlineSub->GetStoreInterface().Get())
			{
				StoreInterface->ProcessQueryAvailablePurchasesResults(bSuccess, ProvidedProductInformation);
			}
		}
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("In-App Purchase Amazon query was completed  %s\n"), bSuccess ? TEXT("successfully") : TEXT("unsuccessfully"));
	}),
		GET_STATID(STAT_FSimpleDelegateGraphTask_ProcessQueryIapResultAmazon),
		nullptr,
		ENamedThreads::GameThread
		);
}

extern "C" void Java_com_amazon_AmazonIapManager_nativePurchaseCompleteAmazon(JNIEnv* jenv, jobject thiz, jboolean bSuccess, jstring productId, jstring receiptData)
{
	FString ProductId, ReceiptData;
	if (bSuccess)
	{
		const char* charsId = jenv->GetStringUTFChars(productId, 0);
		ProductId = FString(UTF8_TO_TCHAR(charsId));
		jenv->ReleaseStringUTFChars(productId, charsId);

		const char* charsReceipt = jenv->GetStringUTFChars(receiptData, 0);
		ReceiptData = FString(UTF8_TO_TCHAR(charsReceipt));
		jenv->ReleaseStringUTFChars(receiptData, charsReceipt);
	}
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("1... ProductId: %s, ReceiptData: %s\n"), *ProductId, *ReceiptData);

	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.ProcessIapResultAmazon"), STAT_FSimpleDelegateGraphTask_ProcessIapResultAmazon, STATGROUP_TaskGraphTasks);

	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]()
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("In-App Purchase Amazon was completed  %s\n"), bSuccess ? TEXT("successfully") : TEXT("unsuccessfully"));
		if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get("AmazonServices"))
		{
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("2... ProductId: %s, ReceiptData: %s\n"), *ProductId, *ReceiptData);
			// call store implementation to process query results.
			if (FOnlineStoreAmazonServices* StoreInterface = (FOnlineStoreAmazonServices*)OnlineSub->GetStoreInterface().Get())
			{
				StoreInterface->ProcessPurchaseResult(bSuccess, ProductId, ReceiptData);
			}
		}
	}),
		GET_STATID(STAT_FSimpleDelegateGraphTask_ProcessIapResultAmazon),
		nullptr,
		ENamedThreads::GameThread
		);
}

void FOnlineStoreAmazonServices::ProcessPurchaseResult(bool bInSuccessful, const FString& ProductId, const FString& InReceiptData)
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineStoreAmazonServices::ProcessPurchaseResult"));
	UE_LOG(LogOnline, Display, TEXT("3... ProductId: %s, ReceiptData: %s\n"), *ProductId, *InReceiptData);


	if (CachedPurchaseStateObject.IsValid())
	{
		FInAppPurchaseProductInfo& ProductInfo = CachedPurchaseStateObject->ProvidedProductInformation;
		ProductInfo.Identifier = ProductId;
		ProductInfo.DisplayName = TEXT("n/a");
		ProductInfo.DisplayDescription = TEXT("n/a");
		ProductInfo.DisplayPrice = TEXT("n/a");
		ProductInfo.ReceiptData = InReceiptData;

		CachedPurchaseStateObject->ReadState = EOnlineAsyncTaskState::Done;
	}

	TriggerOnInAppPurchaseCompleteDelegates(bInSuccessful ? EInAppPurchaseState::Success : EInAppPurchaseState::Failed);
}
#endif
