#include "ChemistrashPlayerController.h"
#include "ChemistrashGameMode.h"
#include "Kismet/GameplayStatics.h"

void AChemistrashPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("MoveLeft", IE_Pressed, this, &AChemistrashPlayerController::MoveLeft);
	InputComponent->BindAction("MoveRight", IE_Pressed, this, &AChemistrashPlayerController::MoveRight);
	InputComponent->BindAction("MoveDown", IE_Pressed, this, &AChemistrashPlayerController::MoveDown);
	InputComponent->BindAction("MoveUp", IE_Pressed, this, &AChemistrashPlayerController::MoveUp);
}

void AChemistrashPlayerController::MoveLeft()
{
	if (AChemistrashGameMode* GM = Cast<AChemistrashGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->MoveLeft();
	}
}

void AChemistrashPlayerController::MoveRight()
{
	if (AChemistrashGameMode* GM = Cast<AChemistrashGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->MoveRight();
	}
}

void AChemistrashPlayerController::MoveDown()
{
	if (AChemistrashGameMode* GM = Cast<AChemistrashGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->MoveDown();
	}
}

void AChemistrashPlayerController::MoveUp()
{
	if (AChemistrashGameMode* GM = Cast<AChemistrashGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->MoveUp();
	}
}
