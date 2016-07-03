// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAISensingComponent.h"

UStrategyAISensingComponent::UStrategyAISensingComponent(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	SensingInterval = 0.2f;
	SightRadius = 2200.0f;
	SetPeripheralVisionAngle(180.0f);
	bOnlySensePlayers = false;
	bHearNoises = false;
	bSeePawns = true;
}

void UStrategyAISensingComponent::InitializeComponent()
{
	Super::InitializeComponent();
	// set custom data from config file
	SightRadius = SightDistance;
}

//检查的条件：AStrategyChar，可见的，存活的，敌对的
bool UStrategyAISensingComponent::ShouldCheckVisibilityOf(APawn *Pawn) const
{
	AStrategyChar* const TestChar = Cast<AStrategyChar>(Pawn);
	return TestChar != nullptr && !TestChar->bHidden && TestChar->GetHealth() > 0 && AStrategyGameMode::OnEnemyTeam(Pawn, GetOwner());
}

bool UStrategyAISensingComponent::CanSenseAnything() const
{
	return SightRadius > 0.0f;
}

void UStrategyAISensingComponent::UpdateAISensing()
{
	const AActor* const Owner = GetOwner();
	if (!IsValid(Owner) || (Owner->GetWorld() == NULL))
	{
		// Cannot sense without a valid owner in the world.
		return;
	}

	//遍历世界中所有的pawn
	for (FConstPawnIterator Iterator = Owner->GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
	{
		AStrategyChar* const TestChar = Cast<AStrategyChar>(*Iterator);
		//不是自己，并且是可见属性的
		if (!IsSensorActor(TestChar) && ShouldCheckVisibilityOf(TestChar))
		{
			//检查距离和角度
			if (CouldSeePawn(TestChar, true))
			{
				KnownTargets.AddUnique(TestChar);
			}
		}
	}

	for (int32 i = KnownTargets.Num() - 1; i >= 0; i--)
	{
		const AStrategyChar* TestChar = Cast<AStrategyChar>(KnownTargets[i].Get());
		if (TestChar == NULL)
		{
			KnownTargets.RemoveAt(i);
		}
	}
}
