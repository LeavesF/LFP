// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/DecalComponent.h"
#include "HexCell.generated.h"

USTRUCT()
struct FHexCellPosition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Q{ 0 };
	UPROPERTY()
	int32 R{ 0 };
	UPROPERTY()
	int32 S{ 0 };

	FHexCellPosition()
		: Q(0)
		, R(0)
	{ }

	FHexCellPosition(int32 newQ, int32 newR)
		: Q(newQ)
		, R(newR)
		, S(-newQ - newR)
	{ }

	bool operator==(const FHexCellPosition& Other) const
	{
		return (Q == Other.Q && R == Other.R);
	}

	bool operator!=(const FHexCellPosition& Other) const
	{
		return (Q != Other.Q || R != Other.R);
	}
};

DECLARE_DYNAMIC_DELEGATE(FHighlightHexCellEvent);
DECLARE_DYNAMIC_DELEGATE(FHighlightHexCellConstantEvent);

UCLASS()
class LFP_API AHexCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CellDestroy();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDecalComponent* DecalComponent;

	UFUNCTION(BlueprintCallable)
	void ShowDecal(bool bShow = false);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UWidgetComponent* WidgetComponent;

public:
	UPROPERTY()
	FHighlightHexCellEvent OnHighlightHexCellEvent;

	UPROPERTY()
	FHighlightHexCellConstantEvent OnHighlightHexCellConstantEvent;

	UFUNCTION(BlueprintImplementableEvent)
	void HighlightHexCell();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWalkable{ true };

	FHexCellPosition SelfCellPos;
	AHexCell* ConnectedCell;

	int32 AStar_F{ 0 };
	int32 AStar_H{ 0 };
	int32 AStar_G{ 0 };

// Debug or Test
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetHighlight();

	UFUNCTION(BlueprintCallable)
	void ExecSetHighlight();
};
