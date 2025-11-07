// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityWidget.generated.h"

class UAbilitySystemComponent;
class AActor;
class APawn;
class APlayerController;
class APlayerState;
class IAbilitySystemInterface;

/** Broadcast when the widget successfully resolves (or changes) the ability system component. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemComponentReady, UAbilitySystemComponent*, Component);

/**
 * UAbilitySystemWidget
 *
 * Base widget that discovers and caches the owning player's ability system component.
 * Resolution order (by default):
 *  - Owning pawn (if it implements the ability system interface)
 *  - Owning pawn's PlayerState (if it implements the ability system interface)
 *
 * You can also manually set the component from any actor that implements the interface.
 *
 * Derived classes can:
 *  - Bind to OnAbilitySystemComponentReady for Blueprint reactions, or
 *  - Override OnAbilitySystemComponentSet (C++) for immediate code reactions.
 */
UCLASS()
class ISLANDERS_API UAbilityWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Fired whenever the cached component is set or changed. */
	UPROPERTY(BlueprintAssignable, Category = "Abilities|Events")
	FOnAbilitySystemComponentReady OnAbilitySystemComponentReady;

	/** Returns the cached ability system component (may be null if not yet resolved). */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

	/**
	 * Attempts to resolve and cache the component from the owning player:
	 *   Pawn → Pawn's PlayerState (if either implements the interface).
	 * Call this when possession changes or after the widget is added for a newly possessed pawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RefreshAbilitySystemFromOwningPlayer();

	/**
	 * Manually set the component source from any actor that implements the ability system interface.
	 * Passing nullptr clears the cached component.
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SetAbilitySystemFromActor(AActor* SourceActor);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Called whenever the cached component is updated (including first set). Old may be null. */
	virtual void OnAbilitySystemComponentSet(UAbilitySystemComponent* NewComponent, UAbilitySystemComponent* OldComponent) {}

	/** Resolve from the given actor: actor implements interface → return component, otherwise nullptr. */
	static UAbilitySystemComponent* ResolveFromActor(const AActor* Actor);

	/** Resolve using the common gameplay setup: Pawn first, then PlayerState. */
	UAbilitySystemComponent* ResolveFromPawnOrPlayerState(const APawn* Pawn) const;

protected:
	/** Cached pointer used by derived widgets; do not store references to this across maps. */
	UPROPERTY(Transient)
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
};
