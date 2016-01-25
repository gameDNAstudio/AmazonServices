// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

#pragma once

#include "OnlineSubsystem.h"
#include "AmazonServices.h"

/** FName declaration of Amazon subsystem */
#define AMAZONSERVICES_SUBSYSTEM FName(TEXT("AmazonServices"))

/** pre-pended to all Amazon logging */
#undef ONLINE_LOG_PREFIX
#define ONLINE_LOG_PREFIX TEXT("AmazonServices: ")

#include "CoreUObject.h"
#include "OnlineStoreInterfaceAmazonServices.h"
