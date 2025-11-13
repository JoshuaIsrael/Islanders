// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#include "Features/Core/Game/CorePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Features/Core/Game/CorePlayerState.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"

ACorePlayerController::ACorePlayerController()
{
	
}

void ACorePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Install controller-level mapping context (switch pawn, menus, etc.)
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInput = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				EnhancedInput->AddMappingContext(InputMappingContext, InputMappingContextPriority);
			}
		}
	}

	// Apply pawn mapping for initial pawn (if any)
	ApplyPawnInputMapping(GetPawn());
}

void ACorePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Whenever we possess something new, update its mapping context
	ApplyPawnInputMapping(InPawn);
}

void ACorePlayerController::ApplyPawnInputMapping(APawn* NewPawn) const
{
	if (!IsLocalController())
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInput = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		// Clear everything, then re-add what we want.
		EnhancedInput->ClearAllMappings();

		// Controller-level mapping (switch pawn etc.)
		if (InputMappingContext)
		{
			EnhancedInput->AddMappingContext(InputMappingContext, InputMappingContextPriority);
		}

		// Pawn-specific mapping
		const ACorePlayerState* CorePlayerState = GetPlayerState<ACorePlayerState>();
		if (!CorePlayerState)
		{
			return;
		}

		constexpr int32 PawnInputPriority = 0; // lower than controller's priority
		if (NewPawn == CorePlayerState->Character && CharacterInputMappingContext)
		{
			EnhancedInput->AddMappingContext(CharacterInputMappingContext, PawnInputPriority);
		}
		else if (NewPawn == CorePlayerState->CameraPawn && CameraInputMappingContext)
		{
			EnhancedInput->AddMappingContext(CameraInputMappingContext, PawnInputPriority);
		}
	}
}

void ACorePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (SwitchPawnAction)
		{
			EnhancedInput->BindAction(SwitchPawnAction, ETriggerEvent::Started, this, &ACorePlayerController::OnSwitchPawnPressed);
		}
	}
}

void ACorePlayerController::OnSwitchPawnPressed(const FInputActionValue& Value)
{
	// Predict next pawn on the client so we can set cursor & input mode
	const ACorePlayerState* CorePlayerState = GetPlayerState<ACorePlayerState>();
	APawn* Current = GetPawn();
	APawn* NextPawn = nullptr;

	if (CorePlayerState && Current)
	{
		if (Current == CorePlayerState->Character && CorePlayerState->CameraPawn)
		{
			NextPawn = CorePlayerState->CameraPawn;
		}
		else if (Current == CorePlayerState->CameraPawn && CorePlayerState->Character)
		{
			NextPawn = CorePlayerState->Character;
		}
		else
		{
			NextPawn = CorePlayerState->Character
				? CorePlayerState->Character
				: CorePlayerState->CameraPawn;
		}
	}

	const bool bNextIsCamera =
		(CorePlayerState && NextPawn && NextPawn == CorePlayerState->CameraPawn);

	if (bNextIsCamera)
	{
		// Strategy camera: show mouse and allow UI interaction
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else
	{
		// Character: game-only input, hide mouse
		const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}

	// Tell the server to actually switch the pawn
	Server_SwitchPawn();
}
 
void ACorePlayerController::Server_SwitchPawn_Implementation()
{
	const ACorePlayerState* CorePlayerState = GetPlayerState<ACorePlayerState>();
	if (!CorePlayerState)
	{
		return;
	}

	APawn* Current = GetPawn();
	if (!Current)
	{
		return;
	}

	APawn* Target = nullptr;

	if (Current == CorePlayerState->Character && CorePlayerState->CameraPawn)
	{
		// Character -> Camera
		Target = CorePlayerState->CameraPawn;
	}
	else if (Current == CorePlayerState->CameraPawn && CorePlayerState->Character)
	{
		// Camera -> Character
		Target = CorePlayerState->Character;
	}
	else
	{
		// Fallback: prefer character, then camera
		Target = CorePlayerState->Character ? CorePlayerState->Character : CorePlayerState->CameraPawn;
	}

	if (!Target || Target == Current)
	{
		return;
	}

	// If we are switching TO the character, snap the camera pawn onto the character
	if (Target == CorePlayerState->CameraPawn && CorePlayerState->CameraPawn)
	{
		const APawn* CharacterPawn = CorePlayerState->Character;
		APawn* CameraPawn = CorePlayerState->CameraPawn;

		const FVector CharacterLocation = CharacterPawn->GetActorLocation();

		// Move the camera pawn to sit on the character (no attachment, just snap)
		CameraPawn->TeleportTo(CharacterLocation, FRotator::ZeroRotator);
	}

	Possess(Target);
	Target->SetOwner(this);

	BlendTo(Target, 0.25f, VTBlend_Cubic, 2.f);
}

void ACorePlayerController::BlendTo(AActor* NewViewTarget, const float BlendTime, const EViewTargetBlendFunction BlendFunc, const float BlendExp)
{
	if (IsLocalController() && NewViewTarget)
	{
		SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp);
	}
}
