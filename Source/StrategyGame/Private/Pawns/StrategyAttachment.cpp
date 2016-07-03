// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAttachment.h"

UStrategyAttachment::UStrategyAttachment(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Effect.bInfiniteDuration = true;
}
