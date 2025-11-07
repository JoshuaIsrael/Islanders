// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDERS_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Player Vitals Widget
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	class UPawnStatusWidget* PlayerVitals;

	// Ability Bar Widget
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	class UAbilityBarWidget* AbilityBar;
};
