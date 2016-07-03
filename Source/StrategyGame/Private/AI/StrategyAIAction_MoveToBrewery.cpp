// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StrategyGame.h"
#include "StrategyAIAction_MoveToBrewery.h"
#include "StrategyAIController.h"
#include "StrategyBuilding_Brewery.h"
#include "StrategyAIDirector.h"
#include "AI/Navigation/NavigationPathGenerator.h"

#include "VisualLogger/VisualLogger.h"

UStrategyAIAction_MoveToBrewery::UStrategyAIAction_MoveToBrewery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetAcceptanceRadius(150)
	, Destination(FVector::ZeroVector)
	, bIsMoving(false)
	, NotMovingFromTime(0)
{
}

bool UStrategyAIAction_MoveToBrewery::IsSafeToAbort() const
{
	return true; //移动可以随时被打断
}

void UStrategyAIAction_MoveToBrewery::Abort()
{
	check(MyAIController.IsValid());
	Super::Abort();

	bIsMoving = false;
	Destination = FVector::ZeroVector;
	//打断寻路
	if (MyAIController->GetPathFollowingComponent())
	{
		MyAIController->GetPathFollowingComponent()->AbortMove(TEXT("abort brewery"));
	}
	//取消结束事件监听
	MyAIController->UnregisterMovementEventDelegate();
}

void UStrategyAIAction_MoveToBrewery::Activate()
{
	check(MyAIController.IsValid());
	Super::Activate();

	NotMovingFromTime = 0;
	// find brewery base and cache it's destination
	const FPlayerData* TeamData = MyAIController->GetTeamData();
	if (TeamData != NULL && TeamData->Brewery != NULL && TeamData->Brewery->GetAIDirector() != NULL)
	{
		//获取目标酒厂的位置
		const AActor* Actor = TeamData->Brewery->GetAIDirector()->GetEnemyBrewery();
		if (Actor != NULL)
		{
			bIsMoving = true;
			Destination = Actor->GetActorLocation();
			//寻路到目标点
			MyAIController->MoveToLocation(Destination, TargetAcceptanceRadius, true, true, true);
		}
	}

	//监听 移动结束事件
	FOnMovementEvent MovementDelegate;
	MovementDelegate.BindUObject(this, &UStrategyAIAction_MoveToBrewery::OnMoveCompleted);
	MyAIController->RegisterMovementEventDelegate(MovementDelegate);
}

bool UStrategyAIAction_MoveToBrewery::Tick(float DeltaTime)
{
	if (bIsMoving && MyAIController.IsValid())
	{
		const bool bNoMove = (MyAIController->GetMoveStatus() != EPathFollowingStatus::Moving);
		if (!bNoMove)
		{
			NotMovingFromTime = 0;
		}
		else if (NotMovingFromTime == 0)
		{
			NotMovingFromTime = MyAIController->GetWorld()->TimeSeconds;
		}

		//如果停止移动超过2秒后，结束当前Action
		if (bNoMove && (MyAIController->GetWorld()->TimeSeconds - NotMovingFromTime) > 2)
		{
			Abort();
		}
	}
	return bIsExecuted && Destination != FVector::ZeroVector && bIsMoving;
}

bool UStrategyAIAction_MoveToBrewery::ShouldActivate() const
{
	check(MyAIController.IsValid());

	FVector DesiredDestination = FVector::ZeroVector;
	const FPlayerData* TeamData = MyAIController->GetTeamData();
	if (TeamData != NULL && TeamData->Brewery != NULL && TeamData->Brewery->GetAIDirector() != NULL)
	{
		const AActor* Actor = TeamData->Brewery->GetAIDirector()->GetEnemyBrewery();
		if (Actor != NULL)
		{
			DesiredDestination = Actor->GetActorLocation();
		}
	}

	if (DesiredDestination != FVector::ZeroVector)
	{
		//如果距离太近，就无需移动了
		return ((DesiredDestination - MyAIController->GetAdjustLocation()).Size2D() > TargetAcceptanceRadius);
	}
	return false;
}

void UStrategyAIAction_MoveToBrewery::OnMoveCompleted()
{
	bIsMoving = false;
}

void UStrategyAIAction_MoveToBrewery::OnPathUpdated(INavigationPathGenerator* PathGenerator, EPathUpdate::Type inType)
{
	check(MyAIController.IsValid());

	if (inType != EPathUpdate::Update)
	{
		UE_VLOG(MyAIController.Get(), LogStrategyAI, Log, TEXT("WARRNING, OnPathUpdated with error - PathUpdateTyp %d"), int32(inType)); 
		Abort();
	}
}
