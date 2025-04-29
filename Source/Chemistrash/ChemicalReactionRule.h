// ChemicalReactionRule.h
#pragma once

#include "CoreMinimal.h"
#include "ChemicalReactionRule.generated.h"

USTRUCT(BlueprintType)
struct FChemicalReactionRule
{
	GENERATED_BODY()

	// 反応に必要な元素とその数（例：2H + 1O）
	UPROPERTY(EditAnywhere)
	TMap<FString, int32> RequiredElements;

	// 生成される化合物の名前（ここでは表示やログ用途）
	UPROPERTY(EditAnywhere)
	FString ResultCompound;

	// 得点
	UPROPERTY(EditAnywhere)
	int32 ScoreValue = 10;
};
