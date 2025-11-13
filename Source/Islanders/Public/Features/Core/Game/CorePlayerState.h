// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CorePlayerState.generated.h"

UCLASS()
class ISLANDERS_API ACorePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	/** Third-person character pawn for this player */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<APawn> Character = nullptr;

	/** Floating camera pawn for this player */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<APawn> CameraPawn = nullptr;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};