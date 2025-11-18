// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnCombatComponent.generated.h"


struct FInputActionValue;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ISLANDERS_API UPawnCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPawnCombatComponent();
	
	FORCEINLINE bool IsInCombat() const { return bIsInCombat; }
	
	void OnToggleCombat(const FInputActionValue& Value);
	
protected:

	/** Whether the pawn is currently in combat mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	bool bIsInCombat = false;
};
