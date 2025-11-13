// Copyright © 2025 Joshua Israel V. Albao. All rights reserved.


#include "Features/Core/Game/CorePlayerState.h"

#include "Net/UnrealNetwork.h"

void ACorePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACorePlayerState, Character);
	DOREPLIFETIME(ACorePlayerState, CameraPawn);
}
