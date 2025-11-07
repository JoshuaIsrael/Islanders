// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Abilities/Widgets/AbilityBarWidget.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Features/Abilities/Widgets/AbilitySlotWidget.h"
#include "GameFramework/PlayerController.h"
#include "Algo/Sort.h"

void UAbilityBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!Container)
	{
		return; // let designer catch missing binding at runtime ensure too
	}

	Container->ClearChildren();

	// Show design-time placeholders
	if (EmptyAbilityWidgetClass && MinimumAbilitySlots > 0)
	{
		AddEmptySlots(/*CurrentCount=*/0);
	}
}

void UAbilityBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Validate bindings early to catch Blueprint wiring issues in logs
	ensureMsgf(Container, TEXT("AbilityBarWidget: Container is not bound"));
	ensureMsgf(AbilityWidgetClass, TEXT("AbilityBarWidget: AbilityWidgetClass is not set"));

	// Initial build if the component is already available
	RefreshAbilityBar();
}

void UAbilityBarWidget::OnAbilitiesChanged(const FGameplayEventData& /*Payload*/)
{
	RefreshAbilityBar();
}

void UAbilityBarWidget::RefreshAbilityBar()
{
	if (!Container || !AbilityWidgetClass)
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		// If ASC not yet set (e.g., early construct), clear to avoid stale UI
		ClearBar();
		AddEmptySlots(/*CurrentCount=*/0);
		return;
	}

	// We avoid copying FGameplayAbilitySpec entries by sorting pointers to them.
	const TArray<FGameplayAbilitySpec>& SpecsArray = AbilitySystemComponent->GetActivatableAbilities();

	TArray<const FGameplayAbilitySpec*> Ordered;
	Ordered.Reserve(SpecsArray.Num());
	for (const FGameplayAbilitySpec& Spec : SpecsArray)
	{
		Ordered.Add(&Spec);
	}

	if (bSortByInputId)
	{
		Algo::Sort(Ordered, [](const FGameplayAbilitySpec* A, const FGameplayAbilitySpec* B)
		{
			// Stable ordering by InputID, then by ability class name as a tie-breaker
			if (A->InputID != B->InputID) { return A->InputID < B->InputID; }
			const UGameplayAbility* AbilA = A->Ability;
			const UGameplayAbility* AbilB = B->Ability;
			const FName NameA = AbilA ? AbilA->GetClass()->GetFName() : NAME_None;
			const FName NameB = AbilB ? AbilB->GetClass()->GetFName() : NAME_None;
			return NameA.LexicalLess(NameB);
		});
	}

	// Rebuild the bar
	ClearBar();

	for (const FGameplayAbilitySpec* SpecPtr : Ordered)
	{
		if (ensure(SpecPtr))
		{
			AddAbilityEntry(*SpecPtr);
		}
	}

	AddEmptySlots(/*CurrentCount=*/Ordered.Num());
}

void UAbilityBarWidget::AddAbilityEntry(const FGameplayAbilitySpec& Spec) const
{
	if (!Container || !AbilityWidgetClass)
	{
		return;
	}

	if (UAbilitySlotWidget* Entry = CreateWidget<UAbilitySlotWidget>(GetOwningPlayer(), AbilityWidgetClass))
	{
		Entry->SetAbility(Spec.Handle);

		if (UHorizontalBoxSlot* HorizontalBoxSlot = Container->AddChildToHorizontalBox(Entry))
		{
			HorizontalBoxSlot->SetPadding(AbilityWidgetPadding);
		}
	}
}

void UAbilityBarWidget::AddEmptySlots(int32 CurrentCount) const
{
	if (!Container || !EmptyAbilityWidgetClass || MinimumAbilitySlots <= 0)
	{
		return;
	}

	for (int32 Index = CurrentCount; Index < MinimumAbilitySlots; ++Index)
	{
		if (UUserWidget* EmptyEntry = CreateWidget<UUserWidget>(GetOwningPlayer(), EmptyAbilityWidgetClass))
		{
			if (UHorizontalBoxSlot* HorizontalBoxSlot = Container->AddChildToHorizontalBox(EmptyEntry))
			{
				HorizontalBoxSlot->SetPadding(AbilityWidgetPadding);
			}
		}
	}
}

void UAbilityBarWidget::ClearBar() const
{
	if (Container)
	{
		Container->ClearChildren();
	}
}
