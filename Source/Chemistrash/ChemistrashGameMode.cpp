// ChemistrashGameMode.cpp
#include "ChemistrashGameMode.h"

#include "ChemicalReactionRule.h"
#include "ElementPiece.h"
#include "EngineUtils.h"
#include "FixedCameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

TArray<FChemicalReactionRule> AChemistrashGameMode::ReactionRules = {
	{
		{
			{"H", 2},
			{"O", 1}
		},
		"H2O",
		20
	},
	{
		{
			{"Na", 1},
			{"Cl", 1}
		},
		"NaCl",
		15
	},
	{
		{
			{"C", 1},
			{"O", 2}
		},
		"CO2",
		20
	},
	{
		{
			{"H", 1},
			{"Cl", 1}
		},
		"HCl",
		15
	},
	{
		{
			{"H", 2},
		},
		"H2",
		10
	}

};

AChemistrashGameMode::AChemistrashGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AChemistrashGameMode::SpawnCamera() const
{
	// カメラをスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector CameraLocation(-1500.f, 0.f, 500.f); // 好きな位置に調整
	FRotator CameraRotation(0.f, 0.f, 0.f); // 下向きとかに調整
	AFixedCameraActor* CameraActor = GetWorld()->SpawnActor<AFixedCameraActor>(
		CameraLocation, CameraRotation, SpawnParams);

	if (CameraActor)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			PC->SetViewTarget(CameraActor);
		}
	}
}

void AChemistrashGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = FVector(0, 0, 1000);
	FallInterval = 1.0f;
	TimeSinceLastFall = 0;

	// 例：H₂O = 2H + 1O
	// FChemicalReactionRule Water;
	// Water.RequiredElements.Add("H", 2);
	// Water.RequiredElements.Add("O", 1);
	// Water.ResultCompound = "H2O";
	// Water.ScoreValue = 20;
	//
	// ReactionRules.Add(Water);

	SpawnCamera();

	SpawnNextElement();
}

bool AChemistrashGameMode::CheckForReactionMulti(AElementPiece* ElementPiece)
{
	bool bReaction = false;

	if (!ElementPiece) return false;

	for (const FChemicalReactionRule& Rule : ReactionRules)
	{
		TSet<AElementPiece*> Visited;
		TMap<FString, int32> NeededElements = Rule.RequiredElements;
		TArray<AElementPiece*> MatchedPieces;

		if (RecursiveSearch(ElementPiece, NeededElements, Visited, MatchedPieces))
		{
			// 反応成立：ピース削除＋スコア加算
			for (AElementPiece* Piece : MatchedPieces)
			{
				// PlacedElements.Remove(Piece);

				// 消えるパーツの上のパーツを調べて覚えておく
				{
					AElementPiece* FallingElement = Piece;
					while (true)
					{
						FVector UpStep = FVector(0, 0, 100);
						FVector NewLocation = FallingElement->GetActorLocation() + UpStep;

						FHitResult Hit;
						FCollisionQueryParams Params;
						Params.AddIgnoredActor(FallingElement);

						bool bHit = GetWorld()->SweepSingleByChannel(Hit, FallingElement->GetActorLocation(),
						                                             NewLocation,
						                                             FQuat::Identity, ECC_WorldStatic,
						                                             FCollisionShape::MakeBox(FVector(10, 10, 10)),
						                                             Params);

						if (bHit)
						{
							FallingElement = Cast<AElementPiece>(Hit.GetActor());
							if (IsValid(FallingElement))
							{
								FallingElementGroups.Add(FallingElement);
								continue;
							}
						}
						break;
					}
				}

				bReaction = true;
				Piece->bReacted = true;
				// Piece->Destroy();
			}
			Score += Rule.ScoreValue;
			UE_LOG(LogTemp, Log, TEXT("Reaction occurred: %s"), *Rule.ResultCompound);
			break; // 一度の反応で1ルール適用
		}
	}

	return bReaction;
}

bool AChemistrashGameMode::RecursiveSearch(
	AElementPiece* Current,
	TMap<FString, int32> NeededElements,
	TSet<AElementPiece*>& Visited,
	TArray<AElementPiece*>& OutMatchedPieces)
{
	if (!Current || Visited.Contains(Current)) return false;

	const FString& Symbol = Current->ElementSymbol;
	if (!NeededElements.Contains(Symbol) || NeededElements[Symbol] <= 0) return false;

	// 状態を保存
	Visited.Add(Current);
	OutMatchedPieces.Add(Current);
	NeededElements[Symbol]--;

	// すべての元素が揃ったか？
	bool bAllMatched = true;
	for (const auto& Pair : NeededElements)
	{
		if (Pair.Value > 0)
		{
			bAllMatched = false;
			break;
		}
	}
	if (bAllMatched)
	{
		return true;
	}

	// 4方向探索
	FVector Pos = Current->GetActorLocation();
	TArray<FVector> Directions = {
		FVector(0, 0, -GridSize),
		FVector(0, 0, GridSize),
		FVector(0, -GridSize, 0),
		FVector(0, GridSize, 0)
	};

	for (const FVector& Dir : Directions)
	{
		FVector NeighborPos = Pos + Dir;
		for (AElementPiece* Other : PlacedElements)
		{
			if (Other && !Visited.Contains(Other) &&
				Other->GetActorLocation().Equals(NeighborPos, 1.0f))
			{
				// 必要元素があるならDFS継続
				TArray<AElementPiece*> BackupMatched = OutMatchedPieces;
				TSet<AElementPiece*> BackupVisited = Visited;
				TMap<FString, int32> BackupNeeded = NeededElements;

				if (RecursiveSearch(Other, NeededElements, Visited, OutMatchedPieces))
				{
					return true;
				}
				else
				{
					// Backtrack
					OutMatchedPieces = BackupMatched;
					Visited = BackupVisited;
					NeededElements = BackupNeeded;
				}
			}
		}
	}

	return false;
}

void AChemistrashGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsGameOver/* || !CurrentElement*/) return;

	TimeSinceLastFall += DeltaSeconds;
	if (TimeSinceLastFall >= FallInterval)
	{
		TimeSinceLastFall = 0;

		for (int32 i = PlacedElements.Num() - 1; i >= 0; --i)
		{
			AElementPiece* Piece = PlacedElements[i];

			if (Piece && Piece->bReacted) // 条件を判定する関数や式
			{
				Piece->Destroy();
				PlacedElements.RemoveAt(i);
			}
		}

		if (!CurrentElement)
		{
			SpawnNextElement();
			return;
		}

		check(FallingElementGroups.Num() == 0 || UpdatingElementGroups.Num() == 0)

		if (FallingElementGroups.Num() > 0)
		{
			for (TObjectPtr<AElementPiece> FallingElement : FallingElementGroups)
			{
				if (IsValid(FallingElement))
				{
					while (true)
					{
						FVector NewLocation = FallingElement->GetActorLocation() + FVector(0.f, 0.f, -100.f);;
						FHitResult Hit;
						FCollisionQueryParams Params;
						Params.AddIgnoredActor(FallingElement);

						bool bHit = GetWorld()->SweepSingleByChannel(Hit, FallingElement->GetActorLocation(),
						                                             NewLocation,
						                                             FQuat::Identity, ECC_WorldStatic,
						                                             FCollisionShape::MakeBox(FVector(10, 10, 10)),
						                                             Params);

						if (!bHit)
						{
							FallingElement->SetActorLocation(NewLocation);
						}
						else
						{
							UpdatingElementGroups.Add(FallingElement);
							break;
						}
					}
				}
			}
			FallingElementGroups.Empty();
			TimeSinceLastFall += FallInterval;
			return;
		}

		if (UpdatingElementGroups.Num() > 0)
		{
			for (TObjectPtr<AElementPiece> UpdatingElement : UpdatingElementGroups)
			{
				if (IsValid(UpdatingElement))
				{
					FVector DownStep = FVector(0, 0, -100);
					FVector NewLocation = UpdatingElement->GetActorLocation() + DownStep;

					FHitResult Hit;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(UpdatingElement);

					bool bHit = GetWorld()->SweepSingleByChannel(Hit, UpdatingElement->GetActorLocation(), NewLocation,
					                                             FQuat::Identity, ECC_WorldStatic,
					                                             FCollisionShape::MakeBox(FVector(10, 10, 10)), Params);

					if (!bHit)
					{
						UpdatingElement->SetActorLocation(NewLocation);
					}
					else
					{
						// 接触で停止
						CheckForReactionMulti(UpdatingElement);
					}
				}
			}
			UpdatingElementGroups.Empty();
			return;
		}

		FVector DownStep = FVector(0, 0, -100);
		FVector NewLocation = CurrentElement->GetActorLocation() + DownStep;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CurrentElement);

		bool bHit = GetWorld()->SweepSingleByChannel(Hit, CurrentElement->GetActorLocation(), NewLocation,
		                                             FQuat::Identity, ECC_WorldStatic,
		                                             FCollisionShape::MakeBox(FVector(10, 10, 10)), Params);

		if (!bHit)
		{
			CurrentElement->SetActorLocation(NewLocation);
		}
		else
		{
			// 接触で停止
			if (CheckForReactionMulti(CurrentElement))
			{
				CurrentElement = nullptr;
			}
			else
			{
				SpawnNextElement();
			}
		}
	}
}

void AChemistrashGameMode::SpawnNextElement()
{
	if (bIsGameOver) return;

	FVector CheckLocation = SpawnLocation;
	FHitResult Hit;
	if (GetWorld()->SweepTestByChannel(CheckLocation, CheckLocation + FVector(0, 0, 1), FQuat::Identity,
	                                   ECC_WorldStatic, FCollisionShape::MakeBox(FVector(50, 50, 50))))
	{
		bIsGameOver = true;
		for (TActorIterator<AElementPiece> It(GetWorld()); It; ++It)
		{
			if (const AElementPiece* Element = *It)
			{
				if (UStaticMeshComponent* Mesh = Element->FindComponentByClass<UStaticMeshComponent>())
				{
					Mesh->SetSimulatePhysics(true);
					// ランダムな方向に少しだけバラけるインパルスを加える
					FVector RandomImpulse = FVector(
						FMath::FRandRange(-100.0f, 100.0f),
						FMath::FRandRange(-100.0f, 100.0f),
						FMath::FRandRange(10.0f, 30.0f)  // 上方向にも跳ねさせるならZに強めの値
					);

					Mesh->AddImpulse(RandomImpulse, NAME_None, true);			}
			}
		}

		const FName SearchTag = FName("FloorMesh"); // 検索したいタグ名
		TArray<AActor*> TaggedActors;

		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SearchTag, TaggedActors);

		for (AActor* Actor : TaggedActors)
		{
			// タグにマッチするアクタに対する処理
			Actor->SetActorRotation(FRotator(5.f, 0, 0));
		}

		UE_LOG(LogTemp, Warning, TEXT("Game Over"));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, 9);
	// FString Symbol = (RandomIndex == 0)
	// 	                 ? TEXT("H")
	// 	                 : (RandomIndex == 1)
	// 	                 ? TEXT("O")
	// 	                 : (RandomIndex == 2)
	// 	                 ? TEXT("Na")
	// 	                 : TEXT("Cl");
	FString Symbol = (RandomIndex == 0)
		                 ? TEXT("H")
		                 : (RandomIndex == 1)
		                 ? TEXT("H")
		                 : (RandomIndex == 2)
		                 ? TEXT("O")
		                 : (RandomIndex == 3)
		                 ? TEXT("Na")
		                 : (RandomIndex == 4)
		                 ? TEXT("Cl")
		                 : (RandomIndex == 5)
		                 ? TEXT("C")
		                 : (RandomIndex == 6)
		                 ? TEXT("O")
		                 : (RandomIndex == 7)
		                 ? TEXT("O")
		                 : (RandomIndex == 8)
		                 ? TEXT("H")
		                 : TEXT("H");

	FActorSpawnParameters Params;
	FRotator SpawnRotation = FRotator(0, 180, 0);
	CurrentElement = GetWorld()->SpawnActor<AElementPiece>(ElementClass, SpawnLocation, SpawnRotation, Params);
	if (CurrentElement)
	{
		CurrentElement->SetSymbol(Symbol);
		PlacedElements.Add(CurrentElement);
	}
}

void AChemistrashGameMode::MoveLeft()
{
	if (CurrentElement)
	{
		FVector NewLocation = CurrentElement->GetActorLocation() + FVector(0.f, -100.f, 0.f);;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CurrentElement);

		bool bHit = GetWorld()->SweepSingleByChannel(Hit, CurrentElement->GetActorLocation(), NewLocation,
		                                             FQuat::Identity, ECC_WorldStatic,
		                                             FCollisionShape::MakeBox(FVector(10, 10, 10)), Params);

		if (!bHit)
		{
			CurrentElement->SetActorLocation(NewLocation);
		}
	}
}

void AChemistrashGameMode::MoveRight()
{
	if (CurrentElement)
	{
		FVector NewLocation = CurrentElement->GetActorLocation() + FVector(0.f, +100.f, 0.f);;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CurrentElement);

		bool bHit = GetWorld()->SweepSingleByChannel(Hit, CurrentElement->GetActorLocation(), NewLocation,
		                                             FQuat::Identity, ECC_WorldStatic,
		                                             FCollisionShape::MakeBox(FVector(10, 10, 10)), Params);

		if (!bHit)
		{
			CurrentElement->SetActorLocation(NewLocation);
		}
	}
}

void AChemistrashGameMode::MoveDown()
{
	if (CurrentElement)
	{
		TimeSinceLastFall += FallInterval;
	}
}

void AChemistrashGameMode::MoveUp()
{
	if (CurrentElement)
	{
		while (true)
		{
			FVector NewLocation = CurrentElement->GetActorLocation() + FVector(0.f, 0.f, -100.f);;
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(CurrentElement);

			bool bHit = GetWorld()->SweepSingleByChannel(Hit, CurrentElement->GetActorLocation(), NewLocation,
			                                             FQuat::Identity, ECC_WorldStatic,
			                                             FCollisionShape::MakeBox(FVector(10, 10, 10)), Params);

			if (!bHit)
			{
				CurrentElement->SetActorLocation(NewLocation);
			}
			else
			{
				break;
			}
		}

		TimeSinceLastFall += FallInterval;
	}
}
