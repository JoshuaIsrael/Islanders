// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Features/Combat/Animations/CombatAnimInstance.h"
#include "Features/Combat/Libraries/CombatLibrary.h"
#include "PawnCombatComponent.generated.h"


struct FInputActionValue;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ISLANDERS_API UPawnCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UPawnCombatComponent();

	UFUNCTION(BlueprintPure, Category="Combat")
	bool InCombat() const
	{
		return bInCombat;
	}
	
	void OnToggleCombat(const FInputActionValue& Value);
	
protected:
	/** Whether the pawn is currently in combat mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	bool bInCombat = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Combat")
	ECombatStance CombatStance = ECombatStance::Unarmed;
	
protected:
	/** Get the combat animation instance from the owning pawn */
	UFUNCTION(BlueprintCallable, Category="Combat")
	UCombatAnimInstance* GetCombatAnimInstance() const;
	
};
