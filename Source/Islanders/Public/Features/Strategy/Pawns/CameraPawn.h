// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "CameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UInputAction;
class UInputMappingContext;

/**
 * Pawn class for a strategy-style camera with panning, rotation, and zooming.
 */
UCLASS()
class ISLANDERS_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	UFloatingPawnMovement* Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Update")
	float UpdateRateHz = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float ArmLengthMin = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float ArmLengthMax = 2400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float ZoomStep = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
	float PitchDegrees = -55.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Rotation")
	float RotateSpeedDegPerSec = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Rotation")
	float RotateInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float PanSpeedUnitsPerSec = 1600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	float PanSpeedZoomScalar = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
	bool bPanRelativeToCamera = true;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveCamera;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> RotateCamera;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ZoomCamera;

private:
	/* Timer handle for update steps */
	FTimerHandle UpdateTimerHandle;

	/* Update interval in seconds */
	float UpdateIntervalSeconds = 1.f / 60.f;

	/* Movement input vector: X = forward/back, Y = right/left */
	FVector2D MoveInput = FVector2D::ZeroVector;

	/* Rotation input: positive = clockwise, negative = counter-clockwise */
	float RotateInput = 0.f;

	/* Zoom input: positive = zoom in, negative = zoom out */
	float TargetYaw = 0.f;

	/* Desired arm length based on zoom input */
	float UserDesiredArmLength = 1400.f;

	/* Smoothed arm length for interpolation */
	float SmoothedArmLength = 1400.f;

	/* Per-step update function */
	void UpdateStep();

	/* Per-frame application of rotation */
	void ApplyPanningPerFrame();

	/* Apply panning based on input */
	void ApplyPanning(float DeltaSeconds);

	/* Apply rotation and panning based on input */
	void ApplyRotation(float DeltaSeconds);

	/* Apply zoom smoothing */
	void ApplyZoomSmoothing(float DeltaSeconds) const;

	// Input handlers
	void OnMoveStarted(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void OnRotateStarted(const FInputActionValue& Value);
	void OnRotateCompleted(const FInputActionValue& Value);
	void OnZoomStarted(const FInputActionValue& Value);

	FVector GetCameraForward2D() const;
	FVector GetCameraRight2D() const;
};
