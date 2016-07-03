// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyAnimInstance.generated.h"

UCLASS()
class UStrategyAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

	/** 
	 *  Notify pawn of the impact.
	 *
	 * @param Notify		The AnimNotify instance that triggered this notification.
	 */
	UFUNCTION()
	void AnimNotify_Melee(UAnimNotify* Notify);
};
