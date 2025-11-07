// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Abilities/Widgets/AbilityWidget.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

void UAbilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshAbilitySystemFromOwningPlayer();
}

void UAbilityWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAbilityWidget::RefreshAbilitySystemFromOwningPlayer()
{
	const APawn* Pawn = GetOwningPlayerPawn();

	if (UAbilitySystemComponent* NewComponent = ResolveFromPawnOrPlayerState(Pawn); NewComponent != AbilitySystemComponent)
	{
		UAbilitySystemComponent* Old = AbilitySystemComponent;
		AbilitySystemComponent = NewComponent;

		// C++ hook first (so implementations can bind before Blueprints react)
		OnAbilitySystemComponentSet(AbilitySystemComponent, Old);

		// Blueprint-friendly notification
		OnAbilitySystemComponentReady.Broadcast(AbilitySystemComponent);
	}
}

void UAbilityWidget::SetAbilitySystemFromActor(AActor* SourceActor)
{
	if (UAbilitySystemComponent* NewComponent = ResolveFromActor(SourceActor); NewComponent != AbilitySystemComponent)
	{
		UAbilitySystemComponent* Old = AbilitySystemComponent;
		AbilitySystemComponent = NewComponent;

		OnAbilitySystemComponentSet(AbilitySystemComponent, Old);
		OnAbilitySystemComponentReady.Broadcast(AbilitySystemComponent);
	}
}

UAbilitySystemComponent* UAbilityWidget::ResolveFromPawnOrPlayerState(const APawn* Pawn) const
{
	// 1) Try the pawn directly
	if (Pawn)
	{
		if (UAbilitySystemComponent* FromPawn = ResolveFromActor(Pawn))
		{
			return FromPawn;
		}

		// 2) Try the pawn's PlayerState (common pattern in multiplayer setups)
		if (const APlayerState* PS = Pawn->GetPlayerState())
		{
			if (UAbilitySystemComponent* FromPS = ResolveFromActor(PS))
			{
				return FromPS;
			}
		}
	}

	return nullptr;
}

UAbilitySystemComponent* UAbilityWidget::ResolveFromActor(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	// Check whether the actor implements the interface and retrieve the component if so.
	if (const IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Actor))
	{
		return AbilityInterface->GetAbilitySystemComponent();
	}

	return nullptr;
}
