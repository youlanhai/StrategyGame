// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "StrategyTypes.h"
#include "StrategyTeamInterface.h"
#include "StrategyChar.generated.h"


class UStrategyAttachment;

// Base class for the minions
UCLASS(Abstract)
class AStrategyChar : public ACharacter, public IStrategyTeamInterface
{
	GENERATED_UCLASS_BODY()

	// 当前角色死亡后，可以获得的资源数量
	/** How many resources this pawn is worth when it dies. */
	UPROPERTY(EditAnywhere, Category=Pawn)
	int32 ResourcesToGather;

	// 表示当前pawn在死亡状态中
	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Health)
	uint32 bIsDying:1;

	/** Current health of this Pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Health)
	float Health;

public:

	/**
	 * Kills pawn.
	 * @param KillingDamage - Damage amount of the killing blow
	 * @param DamageEvent - Damage event of the killing blow
	 * @param Killer - Who killed this pawn
	 * @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	 * @returns true if allowed
	 */
	virtual void Die(float KillingDamage, struct FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	// Notification that we have fallen out of the world.
	virtual void FellOutOfWorld(const UDamageType& DamageType) override;

	// Begin Actor interface
	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** prevent units from basing on each other or buildings */
	virtual bool CanBeBaseForCharacter(APawn* Pawn) const override;

	/** don't export collisions for navigation */
	virtual bool IsComponentRelevantForNavigation(UActorComponent* Component) const override { return false; }

	// 处理受到的伤害
	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** pass hit notifies to AI */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit) override;
	// End Actor interface


	// Begin StrategyTeamInterface interface
	virtual uint8 GetTeamNum() const override;
	// End StrategyTeamInterface interface


	// 播放格斗动画
	/** 
	 * Starts melee attack. 
	 * @return Duration of the attack anim.
	 */
	float PlayMeleeAnim();

	// "格斗动画"触发"作用"事件
	/** Notification triggered from the melee animation to signal impact. */
	void OnMeleeImpactNotify();

	/** set attachment for weapon slot */
	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetWeaponAttachment(UStrategyAttachment* Weapon);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool IsWeaponAttached();

	/** set attachment for armor slot */
	UFUNCTION(BlueprintCallable, Category=Attachment)
	void SetArmorAttachment(UStrategyAttachment* Armor);

	UFUNCTION(BlueprintCallable, Category=Attachment)
	bool IsArmorAttached();

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum); 

	// 添加一个buff
	/** adds active buff to this pawn */
	void ApplyBuff(const struct FBuffData& Buff);

	/** get current pawn's data */
	const struct FPawnData* GetPawnData() const;

	/** get current health */
	UFUNCTION(BlueprintCallable, Category=Health)
	int32 GetHealth() const;

	/** get max health */
	UFUNCTION(BlueprintCallable, Category=Health)
	int32 GetMaxHealth() const;

	/** get all modifiers we have now on pawn */
	const FPawnData& GetModifiedPawnData() { return ModifiedPawnData; }

protected:
	// 格斗动画
	/** melee anim */
	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	UAnimMontage* MeleeAnim;

	// 死亡动画
	/** death anim */
	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	UAnimMontage* DeathAnim;
	
	/** Armor attachment slot */
	UPROPERTY()
	UStrategyAttachment* ArmorSlot;

	/** Weapon attachment slot */
	UPROPERTY()
	UStrategyAttachment* WeaponSlot;

	// 阵营编号
	/** team number */
	uint8 MyTeamNum;

	// 基础属性数据
	/** base pawn data */
	UPROPERTY(EditDefaultsOnly, Category=Pawn)
	FPawnData PawnData;

	// 受buff影响后的数据
	/** pawn data with added buff effects */
	FPawnData ModifiedPawnData;

	// 活跃的buff
	/** List of active buffs */
	TArray<struct FBuffData> ActiveBuffs;

	// 刷新buff所产生的效果
	/** update pawn data after changes in active buffs */
	void UpdatePawnData();

	// 计算持续性的伤害
	/** update pawn's health */
	void UpdateHealth();

	// 死亡动画播放完毕回调。timer回调。
	/** event called after die animation  to hide character and delete it asap */
	void OnDieAnimationEnd();

private:
	// timer句柄。刷新角色数据
	/** Handle for efficient management of UpdatePawnData timer */
	FTimerHandle TimerHandle_UpdatePawnData;

	// timer句柄。刷新角色血量。
	/** Handle for efficient management of UpdateHealth timer */
	FTimerHandle TimerHandle_UpdateHealth;
};

