#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChemistrashPlayerController.generated.h"

UCLASS()
class CHEMISTRASH_API AChemistrashPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

private:
	void MoveLeft();
	void MoveRight();
	void MoveDown();
	void MoveUp();
};
