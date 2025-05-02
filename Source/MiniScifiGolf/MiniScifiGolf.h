// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// IS THIS GOOD?
#if !UE_BUILD_SHIPPING
	#define CALL_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
	#define CUSTOMLOG(fmt, ...) UE_LOG(LogTemp, Warning, TEXT("%s %s"), *CALL_INFO, *FString::Printf(fmt, ##__VA_ARGS__))
#else
	#define CUSTOMLOG(fmt, ...) // disabled in shipping
#endif