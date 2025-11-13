// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CorePlayerController.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UCLASS()
class ISLANDERS_API ACorePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACorePlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	int32 InputMappingContextPriority = 1;

	/** Shared/controller-level inputs (e.g. switch pawn) */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	/** Character controls IMC */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> CharacterInputMappingContext = nullptr;

	/** Camera controls IMC */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> CameraInputMappingContext = nullptr;

	/** Toggle action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> SwitchPawnAction = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	// Input handlers
	void OnSwitchPawnPressed(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_SwitchPawn();

	/** Re-applies mapping contexts based on which pawn we control */
	void ApplyPawnInputMapping(APawn* NewPawn) const;

	void BlendTo(AActor* NewViewTarget, float BlendTime, EViewTargetBlendFunction BlendFunc, float BlendExp);
};

