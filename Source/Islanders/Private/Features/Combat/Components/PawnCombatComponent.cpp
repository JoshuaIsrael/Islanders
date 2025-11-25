// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Combat/Components/PawnCombatComponent.h"

#include "Features/Combat/Animations/CombatAnimInstance.h"
#include "GameFramework/Character.h"


UPawnCombatComponent::UPawnCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPawnCombatComponent::OnToggleCombat(const FInputActionValue& Value)
{
	bInCombat = !bInCombat;

	UCombatAnimInstance* CombatAnimInstance = GetCombatAnimInstance();
	if (!CombatAnimInstance)
	{
		return;
	}

	CombatAnimInstance->bInCombat = bInCombat;
}

UCombatAnimInstance* UPawnCombatComponent::GetCombatAnimInstance() const
{
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return nullptr;
	}

	if (!OwnerCharacter->GetMesh())
	{
		return nullptr;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return nullptr;
	}

	UCombatAnimInstance* CombatAnimInstance = Cast<UCombatAnimInstance>(AnimInstance);
	if (!CombatAnimInstance)
	{
		return nullptr;
	}

	return CombatAnimInstance;
}