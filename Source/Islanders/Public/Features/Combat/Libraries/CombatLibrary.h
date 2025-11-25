// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatLibrary.generated.h"

/**
 * Combat stances for pawns
 */
UENUM(BlueprintType)
enum class ECombatStance : uint8
{
	Unarmed			UMETA(DisplayName = "Unarmed"),
	Staff			UMETA(DisplayName = "Staff"),
	OneHandSword	UMETA(DisplayName = "OneHandSword")
};

/**
 * 
 */
UCLASS()
class ISLANDERS_API UCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
