// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Core/Game/CoreGameMode.h"

#include "Features/Core/Game/CorePlayerState.h"

APawn* ACoreGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	// Let Super spawn and possess the default character pawn
	APawn* DefaultPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

	// Pair a floating camera pawn with this player (server-side)
	if (CameraPawnClass && NewPlayer && NewPlayer->PlayerState && StartSpot)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = NewPlayer;
		SpawnParams.Instigator = nullptr;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const FVector SpawnLoc = StartSpot->GetActorLocation();
		const FRotator SpawnRot = StartSpot->GetActorRotation();

		if (APawn* CameraPawn = GetWorld()->SpawnActor<APawn>(CameraPawnClass, SpawnLoc, SpawnRot, SpawnParams))
		{
			if (ACorePlayerState* PlayerState = Cast<ACorePlayerState>(NewPlayer->PlayerState))
			{
				PlayerState->CameraPawn   = CameraPawn;
				PlayerState->Character = DefaultPawn;
			}
		}
	}

	return DefaultPawn;
}
