// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilityWidget.h"
#include "GameplayTagContainer.h"
#include "AbilityBarWidget.generated.h"

struct FGameplayEventData;
class UHorizontalBox;
class UAbilitySlotWidget;
class UAbilityAsync_WaitGameplayEvent;

/**
 * UAbilityBarWidget
 *
 * Displays one AbilityWidget per activatable ability found on the owning player's
 * ability system component. The list is refreshed when a specific gameplay event fires,
 * or on demand via RefreshAbilityBar().
 */
UCLASS()
class ISLANDERS_API UAbilityBarWidget : public UAbilityWidget
{
	GENERATED_BODY()

protected:
	/** Horizontal container that will hold each spawned ability widget. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UHorizontalBox* Container = nullptr;

	/** The widget subclass to spawn for each ability. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities|UI")
	TSubclassOf<UAbilitySlotWidget> AbilityWidgetClass;

	/** Padding applied to each spawned ability widget when added to the container. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|UI")
	FMargin AbilityWidgetPadding = FMargin(0.5f);

	/**
	 * Gameplay event tag that signals "the owning actor's abilities changed".
	 * When received, this widget rebuilds the bar.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|Events")
	FGameplayTag AbilitiesChangedEventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Abilities.Changed"));

	/** Minimum number of ability slots to always show (empty if no ability). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|UI")
	int32 MinimumAbilitySlots = 4;

	/** Widget class to use for empty ability slots (if MinimumAbilitySlots > current abilities). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|UI")
	TSubclassOf<UUserWidget> EmptyAbilityWidgetClass;

public:
	/** Manually rebuild the bar from the current ability system state. */
	UFUNCTION(BlueprintCallable, Category = "Abilities|UI")
	void RefreshAbilityBar();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	/** Called when the gameplay event is received; forwards to RefreshAbilityBar(). */
	UFUNCTION(BlueprintCallable, Category = "Abilities|UI")
	void OnAbilitiesChanged(const FGameplayEventData& Payload);

	/** Spawn one ability widget for a given spec handle and add it to the container. */
	void AddAbilityEntry(const struct FGameplayAbilitySpec& Spec) const;

	/** Remove all current children from the container. */
	void ClearBar() const;

	/** Add empty slots up to MinimumAbilitySlots. */
	void AddEmptySlots(int32 CurrentCount) const;

private:
	/** Async listener for the gameplay event; kept to ensure lifetime and to cancel on destruct. */
	UPROPERTY(Transient)
	UAbilityAsync_WaitGameplayEvent* AbilitiesChangedListener = nullptr;

	/** If true, sort abilities by InputID for consistent ordering (recommended). */
	UPROPERTY(EditAnywhere, Category = "Abilities|UI")
	bool bSortByInputId = true;
};
