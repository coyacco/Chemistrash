// ChemistrashGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChemistrashGameMode.generated.h"

struct FChemicalReactionRule;
class AElementPiece;

USTRUCT(BlueprintType)
struct FElementData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Symbol;
};

USTRUCT(BlueprintType)
struct FElementPieceGroup
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AElementPiece>> Pieces;
};

UCLASS()
class CHEMISTRASH_API AChemistrashGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AChemistrashGameMode();

	void SpawnCamera() const;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SpawnNextElement();
	bool CheckForReactionMulti(AElementPiece* ElementPiece);
	bool RecursiveSearch(
		AElementPiece* Current,
		TMap<FString, int32> NeededElements,
		TSet<AElementPiece*>& Visited,
		TArray<AElementPiece*>& OutMatchedPieces);

	void MoveLeft();
	void MoveRight();
	void MoveDown();
	void MoveUp();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AElementPiece> ElementClass;

	UPROPERTY()
	AElementPiece* CurrentElement;

	UPROPERTY()
	TArray<TObjectPtr<AElementPiece>> FallingElementGroups;

	UPROPERTY()
	TArray<TObjectPtr<AElementPiece>> UpdatingElementGroups;

	FVector SpawnLocation;
	float FallInterval;
	float TimeSinceLastFall;

	bool bIsGameOver = false;

	UPROPERTY(VisibleAnywhere)
	int32 Score;

	UPROPERTY(VisibleAnywhere)
	TArray<AElementPiece*> PlacedElements;

	float GridSize = 100.0f;

	// UPROPERTY(EditAnywhere, Category = "Reaction")
	static TArray<FChemicalReactionRule> ReactionRules;
};
