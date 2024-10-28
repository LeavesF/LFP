// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LookingForPanduria/ChessBoard/HexCell.h"
#include "MapManager.generated.h"
/**
 * 
 */

enum ETerrain
{
	Plain,
};

enum ELandscape
{
	Empty,
};

USTRUCT()
struct FHexCellData
{
	GENERATED_USTRUCT_BODY()

		FHexCellPosition Location;
	int32 Height;
	float MovementPointCost;
	ETerrain Terrain;
	ELandscape Landscape;
	bool bIsDrawn;
	bool bIsOccupied;

	FHexCellData()
	{
		Location = FHexCellPosition();
		Height = 0;
		MovementPointCost = 0.f;
		Terrain = Plain;
		Landscape = Empty;
		bIsDrawn = false;
		bIsOccupied = false;
	}
};

UCLASS()
class LOOKINGFORPANDURIA_API AMapManager : public AActor
{
	GENERATED_BODY()
public:
	AMapManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<FHexCellData> MapData;


	void AddHexCell(AHexCell* Cell);
	void SaveMapData(FString Mapdata);
	void LoadMapdata(FString Mapdata);
};
