// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Abilities/Widgets/AbilitySlotWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UAbilitySlotWidget::SetAbility(const FGameplayAbilitySpecHandle NewAbilitySpecHandle)
{
	AbilitySpecHandle = NewAbilitySpecHandle;
}

void UAbilitySlotWidget::SetAbilityImage()
{
	// Table must be loaded and ability valid.
	const UDataTable* Table = GetAbilityMetaDataTable();
	if (!Table || !IsValid(Ability))
	{
		return;
	}

	if (!AbilityImage && !CooldownBackground)
	{
		return; // nothing to apply
	}

	// Best practice: use a stable key (class FName), fall back to display name for compatibility.
	const FName ClassKey = Ability->GetClass()->GetFName();

	constexpr TCHAR Ctx[] = TEXT("UAbilityWidget::SetAbilityImage");
	const FAbilityMetaDataRow* Row = Table->FindRow<FAbilityMetaDataRow>(ClassKey, Ctx);

	// Fallback: some projects key rows by class display name (localized). Avoid if you can.
	if (!Row)
	{
		const FText DisplayNameText = Ability->GetClass()->GetDisplayNameText();
		const FString DisplayNameString = DisplayNameText.ToString().TrimStartAndEnd();
		if (!DisplayNameString.IsEmpty())
		{
			Row = Table->FindRow<FAbilityMetaDataRow>(FName(*DisplayNameString), Ctx);
		}
	}

	if (!Row)
	{
		// No row found – keep existing brushes. (Optional) Uncomment to clear:
		// if (AbilityImage)       { AbilityImage->SetBrush(FSlateBrush()); }
		// if (CooldownBackground) { CooldownBackground->SetBrush(FSlateBrush()); }
		return;
	}

	if (AbilityImage)
	{
		AbilityImage->SetBrush(Row->Image);
	}
}

void UAbilitySlotWidget::SetShowCooldownTimeText(bool bShowText)
{
	bShowCooldownTimeText = bShowText;
}

void UAbilitySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeWidget();
}

void UAbilitySlotWidget::NativeDestruct()
{
	// Always clear timers that might be active
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}
	CooldownTimerHandle.Invalidate();

	Super::NativeDestruct();
}

void UAbilitySlotWidget::InitializeWidget()
{
	// Validate bound widgets early
	if (!ensureMsgf(CooldownOverlay, TEXT("CooldownOverlay is not bound")))
	{
		return;
	}
	if (!ensureMsgf(CooldownTimeText, TEXT("CooldownTimeText is not bound")))
	{
		return;
	}

	// We need the ASC to resolve the spec handle into an ability
	if (!AbilitySystemComponent)
	{
		ClearCooldownUI();
		return;
	}

	bool bIsInstanced = false;
	Ability = UAbilitySystemBlueprintLibrary::GetGameplayAbilityFromSpecHandle(
		AbilitySystemComponent, AbilitySpecHandle, bIsInstanced);

	if (!IsValid(Ability))
	{
		ClearCooldownUI();
		return;
	}

	SetAbilityImage();

	ClearCooldownUI(); // Start with a clean state

	ActiveOverlayImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UAbilitySlotWidget::CheckCooldownProgress()
{
	if (!IsValid(Ability))
	{
		return;
	}

	TotalCooldownDuration = Ability->GetCooldownTimeRemaining();

	if (TotalCooldownDuration <= 0.f)
	{
		// No cooldown to show
		ClearCooldownUI();
		return;
	}

	// Avoid starting duplicate timers
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FTimerManager& TM = World->GetTimerManager();
	if (TM.IsTimerActive(CooldownTimerHandle))
	{
		return;
	}

	// Ensure the overlay is visible and push an immediate first update
	CooldownOverlay->SetVisibility(ESlateVisibility::Visible);
	UpdateCooldownProgress();

	// Tick every second to keep the text snappy and cheap
	TM.SetTimer(
		CooldownTimerHandle,
		this,
		&UAbilitySlotWidget::UpdateCooldownProgress,
		0.0333f,
		true,
		0.0f
	);
}

void UAbilitySlotWidget::CheckIsActive()
{
	if (!IsValid(Ability))
	{
		return;
	}

	bool bIsActive = Ability->IsActive();

	ActiveOverlayImage->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UAbilitySlotWidget::UpdateCooldownProgress()
{
	if (!IsValid(Ability))
	{
		ClearCooldownUI();
		return;
	}

	if (const float Remaining = Ability->GetCooldownTimeRemaining(); Remaining <= 0.f)
	{
		CooldownTimerHandle.Invalidate();
		
		ClearCooldownUI();

		// Stop ticking
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CooldownTimerHandle);
		}
	}
	else
	{
		// Ceil so it feels like an integer countdown (3,2,1)
		const int32 SecondsLeft = FMath::CeilToInt(Remaining);
		CooldownOverlay->SetVisibility(ESlateVisibility::Visible);
		CooldownProgress->SetPercent(1.0f - (Remaining / TotalCooldownDuration));

		if (bShowCooldownTimeText)
		{
			CooldownTimeText->SetText(FText::AsNumber(SecondsLeft));
		}
	}
}

void UAbilitySlotWidget::ClearCooldownUI() const
{
	if (CooldownTimeText)
	{
		CooldownTimeText->SetText(FText::GetEmpty());
	}
	if (CooldownOverlay)
	{
		CooldownOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (CooldownProgress)
	{
		CooldownProgress->SetPercent(1.0f);	
	}
}

UDataTable* UAbilitySlotWidget::GetAbilityMetaDataTable()
{
	// Already loaded/cached
	if (AbilityMetaDataTable_Cached)
	{
		return AbilityMetaDataTable_Cached;
	}

	// Nothing set in defaults
	if (!AbilityMetaDataTable.IsValid() && !AbilityMetaDataTable.ToSoftObjectPath().IsValid())
	{
		return nullptr;
	}

	// Synchronous lazy load once (widgets are short-lived; async adds complexity here).
	if (!AbilityMetaDataTable.IsValid())
	{
		AbilityMetaDataTable_Cached = AbilityMetaDataTable.LoadSynchronous();
	}
	else
	{
		AbilityMetaDataTable_Cached = AbilityMetaDataTable.Get();
	}

	// Validate row struct early so bad assets fail loudly.
	if (AbilityMetaDataTable_Cached)
	{
		const UScriptStruct* Expected = FAbilityMetaDataRow::StaticStruct();
		if (const UScriptStruct* Actual   = AbilityMetaDataTable_Cached->GetRowStruct(); !ensureAlwaysMsgf(Actual && (Actual == Expected || Actual->IsChildOf(Expected)),
			TEXT("AbilityMetaDataTable row struct mismatch. Expected %s, got %s"),
			*GetNameSafe(Expected), *GetNameSafe(Actual)))
		{
			AbilityMetaDataTable_Cached = nullptr; // prevent use
		}
	}

	return AbilityMetaDataTable_Cached;
}


