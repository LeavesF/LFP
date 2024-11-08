// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/Datatable.h"
#include "HexCell.generated.h"

USTRUCT(BlueprintType)
struct FHexCellInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	int32 Q{ 0 };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	int32 R{ 0 };

	int32 S{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height{ 0 };

	bool bWalkable{ true };

	int32 AStar_F{ 0 };
	int32 AStar_H{ 0 };
	int32 AStar_G{ 0 };

	FHexCellInfo* ConnectedCell{ nullptr };

	FHexCellInfo()
		: Q(0)
		, R(0)
		, S(0)
	{ }

	FHexCellInfo(int32 newQ, int32 newR)
		: Q(newQ)
		, R(newR)
		, S(-newQ - newR)
	{ }

	bool operator==(const FHexCellInfo& Other) const
	{
		return (Q == Other.Q && R == Other.R);
	}

	bool operator!=(const FHexCellInfo& Other) const
	{
		return (Q != Other.Q || R != Other.R);
	}

	friend uint32 GetTypeHash(const FHexCellInfo& hexCellPosition)
	{
		return HashCombine(GetTypeHash(hexCellPosition.Q), GetTypeHash(hexCellPosition.R));
	}
};

USTRUCT(BlueprintType)
struct FHexEdge : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	FVector2f A{ (0,0) };
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	FVector2f B{ (0,0) };

	FHexEdge()
		: A((0, 0))
		, B((0, 0))
	{ }

	FHexEdge(FVector2f newA, FVector2f newB)
		: A(newA)
		, B(newB)
	{ }

	bool operator==(const FHexEdge& Other) const
	{
		return (A == Other.A && B == Other.B);
	}

	bool operator!=(const FHexEdge& Other) const
	{
		return (A != Other.A || B != Other.B);
	}

	friend uint32 GetTypeHash(const FHexEdge& hexEdge)
	{
		return HashCombine(GetTypeHash(hexEdge.A), GetTypeHash(hexEdge.B));
	}
};

USTRUCT(BlueprintType)
struct FPolygonVertices : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	TArray<FVector2f> vertices{ };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	int32 height{ 0 };

	FPolygonVertices()
		: vertices(), height(0)
	{ }

	FPolygonVertices(TArray<FVector2f> array)
		: vertices(array), height(0)
	{ }

	FPolygonVertices(TArray<FVector2f> array, int32 h)
		: vertices(array), height(h)
	{ }

	bool operator==(const FPolygonVertices& Other) const
	{
		return (vertices == Other.vertices and height == Other.height);
	}

	bool operator!=(const FPolygonVertices& Other) const
	{
		return (vertices != Other.vertices || height != Other.height);
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

	FHexCellInfo SelfCellPos;
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
