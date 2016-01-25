// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#include "AmazonServicesPCH.h"
#include "OnlineAsyncTaskAmazonServicesQueryInAppPurchases.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include <android_native_app_glue.h>
#endif


FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::FOnlineAsyncTaskAmazonServicesQueryInAppPurchases(
	FAmazonServices* InSubsystem,
	const TArray<FString> InProductIds,
	const TArray<bool> InIsConsumableFlags)
	: FOnlineAsyncTaskBasic(InSubsystem)
	, ProductIds(InProductIds)
	, IsConsumableFlags(InIsConsumableFlags)
	, bWasRequestSent(false)
{
}


void FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::ProcessQueryAvailablePurchasesResults(bool bInSuccessful)
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::ProcessQueryAvailablePurchasesResults"));

	bWasSuccessful = bInSuccessful;
	bIsComplete = true;
}


void FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::Finalize()
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::Finalize"));
}


void FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::TriggerDelegates()
{
	UE_LOG(LogOnline, Display, TEXT("FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::TriggerDelegates"));
	Subsystem->GetStoreInterface()->TriggerOnQueryForAvailablePurchasesCompleteDelegates(bWasSuccessful);
}


void FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::Tick()
{
	//	UE_LOG(LogOnline, Display, TEXT("FOnlineAsyncTaskAmazonServicesQueryInAppPurchases::Tick"));

	if (!bWasRequestSent)
	{
#if PLATFORM_ANDROID
		if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
		{
			// Populate some java types with the provided product information
			jobjectArray ProductIDArray = (jobjectArray)Env->NewObjectArray(ProductIds.Num(), FJavaWrapper::JavaStringClass, NULL);
			jbooleanArray ConsumeArray = (jbooleanArray)Env->NewBooleanArray(ProductIds.Num());

			jboolean* ConsumeArrayValues = Env->GetBooleanArrayElements(ConsumeArray, 0);
			for (uint32 Param = 0; Param < ProductIds.Num(); Param++)
			{
				jstring StringValue = Env->NewStringUTF(TCHAR_TO_UTF8(*ProductIds[Param]));
				Env->SetObjectArrayElement(ProductIDArray, Param, StringValue);
				Env->DeleteLocalRef(StringValue);

				ConsumeArrayValues[Param] = IsConsumableFlags[Param];
			}
			Env->ReleaseBooleanArrayElements(ConsumeArray, ConsumeArrayValues, 0);

			static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_IapQueryInAppPurchasesAmazon", "([Ljava/lang/String;[Z)Z", false);
			FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, ProductIDArray, ConsumeArray);

			// clean up references
			Env->DeleteLocalRef(ProductIDArray);
			Env->DeleteLocalRef(ConsumeArray);
		}
#endif

		bWasRequestSent = true;
	}
}
