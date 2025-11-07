// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryLibrary.generated.h"


/**
 * Enum defining the different types of weapons.
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None        UMETA(DisplayName = "None"),
	Unarmed     UMETA(DisplayName = "Unarmed"),
	Polearm     UMETA(DisplayName = "Polearm"),
};

/**
 * 
 */
UCLASS()
class ISLANDERS_API UInventoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
