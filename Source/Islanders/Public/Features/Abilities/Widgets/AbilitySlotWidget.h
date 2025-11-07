// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilityWidget.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "AbilitySlotWidget.generated.h"

class UProgressBar;
// Forward declarations to reduce header coupling/compile times
class USizeBox;
class UOverlay;
class UImage;
class UTextBlock;
class UGameplayAbility;

// Row used by DT_AbilityMetaData (matches your BP “Out Row Image” pins)
USTRUCT(BlueprintType)
struct FAbilityMetaDataRow : public FTableRowBase
{
	GENERATED_BODY()

	// Main icon brush
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush Image;
};

/**
 * UAbilityWidget
 *
 * A small, self-contained UI widget that displays:
 *  - The bound ability's display name.
 *  - A cooldown overlay with a numeric countdown.
 *
 * Usage:
 *  1) Call SetAbility() with a valid FGameplayAbilitySpecHandle.
 *  2) Ensure AbilitySystemComponent (inherited from UAbilitySystemWidget) is set/available.
 *  3) Bind HandleTagCountChanged() to your ASC tag delegate(s) that signal cooldown changes.
 *
 * Notes:
 *  - Cooldown UI updates are timer-driven (1s tick by default) and auto-stop at 0.
 */
UCLASS()
class ISLANDERS_API UAbilitySlotWidget : public UAbilityWidget
{
	GENERATED_BODY()
	
protected:
	/** Root container. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	USizeBox* Container = nullptr;

	/** Background icon/image for the ability. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UImage* AbilityImage = nullptr;

	/** Overlay that appears while active. */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UImage* ActiveOverlayImage = nullptr;

	/** Overlay that appears while on cooldown (acts as a simple mask). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UProgressBar* CooldownProgress = nullptr;

	/** Overlay that appears while on cooldown (acts as a simple mask). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UOverlay* CooldownOverlay = nullptr;

	/** Background icon/image for the cooldown (e.g., dim/greyscale image). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UImage* CooldownBackground = nullptr;

	/** Numeric countdown (seconds remaining). */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Abilities|UI")
	UTextBlock* CooldownTimeText = nullptr;

	/** Spec handle for the ability this widget represents. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities|State")
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	/** Cached resolved ability (from the spec handle + ASC). */
	UPROPERTY(Transient)
	const UGameplayAbility* Ability = nullptr;

	/** Internal timer for updating the numeric cooldown text. */
	FTimerHandle CooldownTimerHandle;

	/** Tick interval for updating the cooldown label (seconds). 1s gives a classic integer countdown feel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities|Config", meta = (ClampMin = "0.05", UIMin = "0.05"))
	float CooldownTickInterval = 1.0f;

	// Soft reference to avoid hard-loading in every widget instance.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities|UI", meta=(AllowedClasses="DataTable"))
	TSoftObjectPtr<UDataTable> AbilityMetaDataTable;

	// Cached strong ptr after first load (do not expose)
	UPROPERTY(Transient)
	UDataTable* AbilityMetaDataTable_Cached = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities|UI")
	bool bShowCooldownTimeText = false;

public:
	/**
	 * Bind the widget to a specific ability spec.
	 * Call this once you know which spec handle this slot should display.
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities|UI")
	void SetAbility(const FGameplayAbilitySpecHandle NewAbilitySpecHandle);

	void SetAbilityImage();

	UFUNCTION(BlueprintCallable, Category = "Abilities|UI")
	void SetShowCooldownTimeText(bool bShowText);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Initialize the visual state and resolve the spec handle into a UGameplayAbility.
	 * Safe to call multiple times; it will early-out when prerequisites are missing.
	 */
	void InitializeWidget();

	UFUNCTION(BlueprintCallable, Category = "Abilities|UI")
	void CheckCooldownProgress();

	UFUNCTION(BlueprintCallable, Category = "Abilities|UI")
	void CheckIsActive();

	/** Polls the ability for remaining cooldown and updates the overlay/text. Stops the timer at 0. */
	void UpdateCooldownProgress();

	/** Hides cooldown overlay and clears the text. Idempotent. */
	void ClearCooldownUI() const;

	// Loads the soft table once (if needed) and caches it.
	UDataTable* GetAbilityMetaDataTable();

private:
	float TotalCooldownDuration = 0.f;

};
