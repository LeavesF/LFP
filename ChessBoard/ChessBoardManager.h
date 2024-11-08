// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LFP/ChessBoard/HexCell.h"
#include "Engine/Datatable.h"
#include "ChessBoardManager.generated.h"

#define M_COS30 (0.866f)
#define SQU3 1.73205

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateChessBoardEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClearChessBoardEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowChessBoardEvent, bool, bShow);

UCLASS()
class LFP_API AChessBoardManager : public AActor
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

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void CreateChessBoard();

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void ClearChessBoard();

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void ShowChessBoard(bool bShow = false);

	// A-Star Algorithm
	//https://www.bilibili.com/video/BV1v44y1h7Dt/?spm_id_from=444.41.top_right_bar_window_custom_collection.content.click&vd_source=b9cf0af5ab89bda1ad834fb699b19e4c
	TArray<FHexCellInfo*> FindPath(FHexCellInfo* StartCell, FHexCellInfo* TargetCell);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessBoard")
	float edgeLength{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessBoard")
	UDataTable* hexCellTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChessBoard")
	UDataTable* polygonVerticesTable;

	TSet<FHexCellInfo> hexCellSet;
	TSet<FHexCellInfo>* hexCellSetPtr = &hexCellSet;

	TMap<FVector2f, FVector2f> outlineEdgeSet;

	FVector CalculateCellLocation(int32 Row, int32 Col);

	TArray<FHexCellInfo*> GetNeighbors(FHexCellInfo* Position);

	int32 GetDistanceBetweenTwoHexCell(FHexCellInfo* StartCell, FHexCellInfo* TargetCell);

	int CalculateHashValue(int32 Q, int32 R);

// Debug or Test
public:
	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	void ChangeAllToWalkable();

	TArray<int32> GetNeighborsForGraph(FHexCellInfo* Position);
	
	void GetIslandOutlineEdges(const TSet<FHexCellInfo>* hexCellset, int32 height);

	UFUNCTION(BlueprintCallable, Category = "ChessBoard")
	TArray<FVector2f> GetIslandOutlineVertices();

	FHexCellInfo* GetHexCellUnderXYPoint(FVector position);

	FVector GetXYPointOfHexCell(int32 q, int32 r, FVector position);
};
