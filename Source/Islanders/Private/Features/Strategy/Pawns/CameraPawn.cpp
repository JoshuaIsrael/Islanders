// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#include "Features/Strategy/Pawns/CameraPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "TimerManager.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->TargetArmLength = 1400.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw   = true;
	SpringArm->bInheritRoll  = false;
	SpringArm->SetRelativeRotation(FRotator(PitchDegrees, 0.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->MaxSpeed = PanSpeedUnitsPerSec;
	Movement->Acceleration = PanSpeedUnitsPerSec * 4.f;
	Movement->Deceleration = PanSpeedUnitsPerSec * 6.f;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ACameraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyPanningPerFrame();
	ApplyZoomSmoothing(DeltaSeconds);
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Initialize yaw target from current actor yaw
	TargetYaw = GetActorRotation().Yaw;

	// Initialize zoom values
	UserDesiredArmLength = SpringArm->TargetArmLength;

	// Start timer update loop
	UpdateIntervalSeconds = (UpdateRateHz > KINDA_SMALL_NUMBER) ? (1.f / UpdateRateHz) : 0.f;
	if (UpdateIntervalSeconds > 0.f && GetWorld())
	{
		GetWorldTimerManager().SetTimer(UpdateTimerHandle, this, &ACameraPawn::UpdateStep, UpdateIntervalSeconds, /*bLoop*/ true);
	}
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveCamera)
		{
			EnhancedInput->BindAction(MoveCamera, ETriggerEvent::Triggered, this, &ACameraPawn::OnMoveStarted);
			EnhancedInput->BindAction(MoveCamera, ETriggerEvent::Completed, this, &ACameraPawn::OnMoveCompleted);
			EnhancedInput->BindAction(MoveCamera, ETriggerEvent::Canceled, this, &ACameraPawn::OnMoveCompleted);
		}

		if (RotateCamera)
		{
			EnhancedInput->BindAction(RotateCamera, ETriggerEvent::Triggered, this, &ACameraPawn::OnRotateStarted);
			EnhancedInput->BindAction(RotateCamera, ETriggerEvent::Completed, this, &ACameraPawn::OnRotateCompleted);
			EnhancedInput->BindAction(RotateCamera, ETriggerEvent::Canceled, this, &ACameraPawn::OnRotateCompleted);
		}

		if (ZoomCamera)
		{
			EnhancedInput->BindAction(ZoomCamera, ETriggerEvent::Triggered, this, &ACameraPawn::OnZoomStarted);
		}
	}
}

void ACameraPawn::UpdateStep()
{
	// Use the timer interval as dt for deterministic smoothing.
	const float DeltaSeconds = UpdateIntervalSeconds > 0.f
		? UpdateIntervalSeconds
		: (GetWorld() ? GetWorld()->GetDeltaSeconds() : (1.f / 60.f));

	ApplyRotation(DeltaSeconds);
}

void ACameraPawn::ApplyPanningPerFrame()
{
	if (MoveInput.IsNearlyZero())
	{
		return;
	}

	const FVector Forward = bPanRelativeToCamera ? GetCameraForward2D() : FVector::ForwardVector;
	const FVector Right   = bPanRelativeToCamera ? GetCameraRight2D()   : FVector::RightVector;

	const FVector WorldMove = (Forward * MoveInput.Y) + (Right * MoveInput.X);
	AddMovementInput(WorldMove.GetClampedToMaxSize(1.f), 1.f);
}

void ACameraPawn::ApplyPanning(float DeltaSeconds)
{
	if (MoveInput.IsNearlyZero()) return;

	const FVector Forward = bPanRelativeToCamera ? GetCameraForward2D() : FVector::ForwardVector;
	const FVector Right   = bPanRelativeToCamera ? GetCameraRight2D()   : FVector::RightVector;

	const FVector WorldMove = (Forward * MoveInput.Y) + (Right * MoveInput.X);
	AddMovementInput(WorldMove.GetClampedToMaxSize(1.f), 1.f);
}

void ACameraPawn::ApplyRotation(const float DeltaSeconds)
{
	// Ensure RotateInput is within valid range
	RotateInput = FMath::Clamp(RotateInput, -1.0f, 1.0f);

	if (FMath::Abs(RotateInput) > KINDA_SMALL_NUMBER)
	{
		TargetYaw += RotateInput * RotateSpeedDegPerSec * DeltaSeconds;
	}

	FRotator Rotation = GetActorRotation();
	
	// Use FMath::FixedTurn for smooth angle interpolation that handles wrapping correctly
	const float NewYaw = FMath::FixedTurn(Rotation.Yaw, TargetYaw, RotateInterpSpeed * DeltaSeconds * 360.f);
	Rotation.Yaw = NewYaw;
	SetActorRotation(Rotation);

	// Normalize both angles to keep them in reasonable range
	TargetYaw = FRotator::NormalizeAxis(TargetYaw);
	
	// Sync TargetYaw with actual rotation when input stops to prevent drift
	if (FMath::Abs(RotateInput) <= KINDA_SMALL_NUMBER)
	{
		TargetYaw = Rotation.Yaw;
	}

	if (!FMath::IsNearlyEqual(SpringArm->GetRelativeRotation().Pitch, PitchDegrees, 0.01f))
	{
		SpringArm->SetRelativeRotation(FRotator(PitchDegrees, 0.f, 0.f));
	}
}

void ACameraPawn::ApplyZoomSmoothing(const float DeltaSeconds) const
{
	// Smoothly interpolate the arm length for buttery-smooth zooming
	const float CurrentArmLength = SpringArm->TargetArmLength;
	const float NewArmLength = FMath::FInterpTo(CurrentArmLength, UserDesiredArmLength, DeltaSeconds, 8.f);
	SpringArm->TargetArmLength = NewArmLength;
}

void ACameraPawn::OnMoveStarted(const FInputActionValue& Value)
{
	// Vector2D: X=Right (D/A), Y=Forward (W/S)
	MoveInput = Value.Get<FVector2D>();
}

void ACameraPawn::OnMoveCompleted(const FInputActionValue& Value)
{
	MoveInput = FVector2D::ZeroVector;
}

void ACameraPawn::OnRotateStarted(const FInputActionValue& Value)
{
	// Axis1D: -1 (Q), +1 (E). Held = continuous value per step.
	RotateInput = Value.Get<float>();
}

void ACameraPawn::OnRotateCompleted(const FInputActionValue& Value)
{
	RotateInput = 0.f;
}

void ACameraPawn::OnZoomStarted(const FInputActionValue& Value)
{
	const float Wheel = Value.Get<float>();
	float NewLen = UserDesiredArmLength - (Wheel * ZoomStep);
	NewLen = FMath::Clamp(NewLen, ArmLengthMin, ArmLengthMax);
	UserDesiredArmLength = NewLen;

	const float ZoomAlpha = (NewLen - ArmLengthMin) / FMath::Max(1.f, (ArmLengthMax - ArmLengthMin));
	const float ScaledSpeed = PanSpeedUnitsPerSec * (1.f + ZoomAlpha * PanSpeedZoomScalar);
	Movement->MaxSpeed = ScaledSpeed;
	Movement->Acceleration = ScaledSpeed * 4.f;
	Movement->Deceleration = ScaledSpeed * 6.f;
}

FVector ACameraPawn::GetCameraForward2D() const
{
	const FRotator YawRot(0.f, GetActorRotation().Yaw, 0.f);
	return FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
}

FVector ACameraPawn::GetCameraRight2D() const
{
	const FRotator YawRot(0.f, GetActorRotation().Yaw, 0.f);
	return FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
}