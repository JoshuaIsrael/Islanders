// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoreGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDERS_API ACoreGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/* The default camera pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> CameraPawnClass;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};