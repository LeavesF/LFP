// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LookingForPanduria/ChessBoard/HexCell.h"
#include "ChessBoardManager.generated.h"

#define M_COS30 (0.866f)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateChessBoardEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClearChessBoardEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowChessBoardEvent, bool, bShow);

UCLASS()
class LOOKINGFORPANDURIA_API AChessBoardManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessBoardManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "ChessBoard")
	FCreateChessBoardEvent OnCreateChessBoardEvent;

	UPROPERTY(BlueprintAssignable, Category = "ChessBoard")
	FClearChessBoardEvent OnClearChessBoardEvent;

	UPROPERTY(BlueprintAssignable, Category = "ChessBoard")
	FShowChessBoardEvent OnShowChessBoardEvent;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ChessBoard")
	//UStaticMesh* HexCellMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ChessBoard")
	TSubclassOf<AHexCell> HexCellActor;

	TArray<AHexCell*> HexCells;

	TMap<int, AHexCell*> HexCellPosMap;

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void CreateChessBoard();

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void ClearChessBoard();

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void ShowChessBoard(bool bShow = false);

	// A-Star Algorithm
	//https://www.bilibili.com/video/BV1v44y1h7Dt/?spm_id_from=444.41.top_right_bar_window_custom_collection.content.click&vd_source=b9cf0af5ab89bda1ad834fb699b19e4c
	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	TArray<AHexCell*> FindPath(AHexCell* StartCell, AHexCell* TargetCell);

protected:
	float EdgeLength{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessBoard")
	int32 NumRows{ 5 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessBoard")
	int32 NumColumns{ 5 };

	FVector CalculateCellLocation(int32 Row, int32 Col);

	TArray<AHexCell*> GetNeighbors(FHexCellPosition Position);

	int32 GetDistanceBetweenTwoHexCell(AHexCell* StartCell, AHexCell* TargetCell);

	int CalculateHashValue(int32 Q, int32 R);

// Debug or Test
public:
	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void ChangeAllToWalkable();
	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	TMap<int, AHexCell*> GetNeighborsForGraph(FHexCellPosition Position);
	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	TArray<AHexCell*> GetIslandOutline(const TArray<AHexCell*>& hexes);
};
