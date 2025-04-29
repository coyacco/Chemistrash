// ElementPiece.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElementPiece.generated.h"

class UTextRenderComponent;

UCLASS()
class CHEMISTRASH_API AElementPiece : public AActor
{
	GENERATED_BODY()

public:
	AElementPiece();

	void SetSymbol(const FString& InSymbol);
	FString GetSymbol() const;

protected:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* TextComponent;

public:
	UPROPERTY()
	FString ElementSymbol;

	bool bReacted = false;
};