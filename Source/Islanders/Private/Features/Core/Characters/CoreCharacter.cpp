// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Core/Characters/CoreCharacter.h"

#include "Features/Combat/Components/PawnCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ACoreCharacter::ACoreCharacter()
{
	CombatComponent = CreateDefaultSubobject<UPawnCombatComponent>(TEXT("CombatComponent"));
}