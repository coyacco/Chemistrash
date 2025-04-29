// ElementPiece.cpp
#include "ElementPiece.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

AElementPiece::AElementPiece()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->SetHorizontalAlignment(EHTA_Center);
	TextComponent->SetVerticalAlignment(EVRTA_TextCenter);
	TextComponent->SetWorldSize(100.0f);
	TextComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
}

void AElementPiece::SetSymbol(const FString& InSymbol)
{
	ElementSymbol = InSymbol;

	// シンボルに応じて見た目変更
	TextComponent->SetText(FText::FromString(ElementSymbol));
}

FString AElementPiece::GetSymbol() const
{
	return ElementSymbol;
}
