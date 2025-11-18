// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/Abilities/Characters/AbilitiesCharacterBase.h"
#include "CoreCharacter.generated.h"

class UPawnCombatComponent;

UCLASS()
class ISLANDERS_API ACoreCharacter : public AAbilitiesCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACoreCharacter();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	UPawnCombatComponent* CombatComponent = nullptr;
	
};
