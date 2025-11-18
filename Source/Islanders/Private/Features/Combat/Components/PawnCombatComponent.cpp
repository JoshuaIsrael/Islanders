// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Combat/Components/PawnCombatComponent.h"


UPawnCombatComponent::UPawnCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPawnCombatComponent::OnToggleCombat(const FInputActionValue& Value)
{
	bIsInCombat = !bIsInCombat;
}
