// Fill out your copyright notice in the Description page of Project Settings.


#include "LFP/ChessBoard/ChessBoardManager.h"

#include "LFP/UI/HexCellInfoWidget.h"

// Sets default values
AChessBoardManager::AChessBoardManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnCreateChessBoardEvent.AddDynamic(this, &AChessBoardManager::CreateChessBoard);
}

// Called when the game starts or when spawned
void AChessBoardManager::BeginPlay()
{
	Super::BeginPlay();
	
	//HexCellPosMap.Empty();
}

// Called every frame
void AChessBoardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessBoardManager::CreateChessBoard()
{
	if (hexCellTable != nullptr)
	{
		hexCellSet.Empty();
		for (auto it : hexCellTable->GetRowMap())
		{
			FHexCellPosition* row = (FHexCellPosition*)it.Value;
			if (row)
			{
				hexCellSet.Add(*row);
			}
		}
	}
	GetIslandOutlineEdges(hexCellSet);

	polygonVerticesTable->EmptyTable();

	TArray<FVector2f> polygonVerticesArray = GetIslandOutlineVertices();
	if (polygonVerticesTable != nullptr)
	{
		polygonVerticesTable->AddRow("d", FPolygonVertices(polygonVerticesArray));
	}
	
	TArray<FVector2f> polygonVerticesArray2 = GetIslandOutlineVertices();
	if (polygonVerticesTable != nullptr)
	{
		polygonVerticesTable->AddRow("f", FPolygonVertices(polygonVerticesArray2));
	}
	return;
	/*if (HexCellMesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No UnitMesh!"));
		return;
	}*/
	for (int32 Row = 0; Row < numRows; Row++)
	{
		for (int32 Col = 0; Col < numColumns; Col++)
		{
			FActorSpawnParameters SpawnParam = FActorSpawnParameters();
			SpawnParam.bAllowDuringConstructionScript = true;

			AHexCell* CellActor = GetWorld()->SpawnActor<AHexCell>(HexCellActor, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParam);
			if (CellActor)
			{
				OnClearChessBoardEvent.AddDynamic(CellActor, &AHexCell::CellDestroy);
				OnShowChessBoardEvent.AddDynamic(CellActor, &AHexCell::ShowDecal);

				CellActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				CellActor->SetActorRelativeLocation(CalculateCellLocation(Row, Col));

				int32 TempQ = (((Row & 1) - Row) / 2) - Col;
				int32 TempR = Row;
				// Store CellPosition data
				CellActor->SelfCellPos = FHexCellPosition(TempQ, TempR);
				HexCells.Add(CellActor);
				HexCellPosMap.Add(CalculateHashValue(TempQ, TempR), CellActor);

				UHexCellInfoWidget* InfoWidget = Cast<UHexCellInfoWidget>(CellActor->WidgetComponent->GetWidget());
				if (InfoWidget)
				{
					InfoWidget->SetCellInfo(TempQ, TempR);
				}
				//TODO: Material
				//CellActor->MeshComponent->SetStaticMesh(HexCellMesh);
			}
		}
	}
}

void AChessBoardManager::ClearChessBoard()
{
	HexCells.Empty();
	OnClearChessBoardEvent.Broadcast();
	OnClearChessBoardEvent.Clear();
}

void AChessBoardManager::ShowChessBoard(bool bShow /*= false*/)
{
	OnShowChessBoardEvent.Broadcast(bShow);
}

TArray<AHexCell*> AChessBoardManager::FindPath(AHexCell* StartCell, AHexCell* TargetCell)
{
	TArray<AHexCell*> ToSearch;
	TArray<AHexCell*> Processed;
	TArray<AHexCell*> Path;
	if (StartCell == nullptr || TargetCell == nullptr)
	{
		return Path;
	}
	StartCell->AStar_H = GetDistanceBetweenTwoHexCell(StartCell, TargetCell);
	StartCell->AStar_F = StartCell->AStar_H;
	ToSearch.Add(StartCell);

	while (!ToSearch.IsEmpty())
	{
		AHexCell* Current = ToSearch[0];
		for (AHexCell* Temp : ToSearch)
		{
			if (Temp->AStar_F < Current->AStar_F || (Temp->AStar_F == Current->AStar_F && Temp->AStar_H < Current->AStar_H))
			{
				Current = Temp;
			}
		}

		Processed.Add(Current);
		ToSearch.Remove(Current);

		if (Current == TargetCell)
		{
			AHexCell* CurrentPathTile = TargetCell;
			while (CurrentPathTile != StartCell)
			{
				Path.Add(CurrentPathTile);
				CurrentPathTile = CurrentPathTile->ConnectedCell;
			}
			Path.Add(StartCell);
		}

		TArray<AHexCell*> Neighbors = GetNeighbors(Current->SelfCellPos);
		for (AHexCell* Neighbor : Neighbors)
		{
			if (Processed.Contains(Neighbor))
			{
				continue;
			}

			bool bInSearch = ToSearch.Contains(Neighbor);
			//TODO: int32 CostToNeighbor = Current->AStar_G + Current->GetDistance(Neighbor);
			int32 CostToNeighbor = Current->AStar_G + 1;

			if (!bInSearch || CostToNeighbor < Neighbor->AStar_G)
			{
				Neighbor->AStar_G = CostToNeighbor;
				Neighbor->ConnectedCell = Current;

				if (!bInSearch)
				{
					Neighbor->AStar_H = GetDistanceBetweenTwoHexCell(Neighbor, TargetCell);
					Neighbor->AStar_F = Neighbor->AStar_G + Neighbor->AStar_H;
					ToSearch.Add(Neighbor);
				}
			}
		}
	}

	return Path;
}

FVector AChessBoardManager::CalculateCellLocation(int32 Row, int32 Col)
{
	FVector CellLocation = FVector::Zero();
	if (Row % 2 == 1)
	{
		CellLocation.X = Row * edgeLength * 1.5f;
		CellLocation.Y = Col * edgeLength * 2 * M_COS30;
	}
	else
	{
		CellLocation.X = (Row * edgeLength * 1.5f);
		CellLocation.Y = Col * edgeLength * 2 * M_COS30 + (edgeLength * M_COS30);
	}
	

	return CellLocation;
}

TArray<AHexCell*> AChessBoardManager::GetNeighbors(FHexCellPosition Position)
{
	TArray<AHexCell*> Neighbors;
	TArray<FHexCellPosition> NeighborPositions;

	NeighborPositions.Add(FHexCellPosition(Position.Q + 1, Position.R));		// 0'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q + 1, Position.R - 1));	// 60'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q, Position.R - 1));		// 120'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q - 1, Position.R));		// 180'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q - 1, Position.R + 1));	// 240'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q, Position.R + 1));		// 300'方向

	int index = 0;
	for (FHexCellPosition TempPos : NeighborPositions)
	{
		//AHexCell* Neighbor = *(HexCellPosMap.Find(CalculateHashValue(TempPos.Q, TempPos.R)));
		AHexCell** NeighborPtr = HexCellPosMap.Find(CalculateHashValue(TempPos.Q, TempPos.R));
		if (NeighborPtr != nullptr)
		{
			AHexCell* Neighbor = *NeighborPtr;
			if (Neighbor && Neighbor->bWalkable)
			{
				Neighbors.Add(Neighbor);
			}
		}
		index++;
	}

	return Neighbors;
}

int32 AChessBoardManager::GetDistanceBetweenTwoHexCell(AHexCell* StartCell, AHexCell* TargetCell)
{
	int32 Distance_Q = FMath::Abs(StartCell->SelfCellPos.Q - TargetCell->SelfCellPos.Q);
	int32 Distance_R = FMath::Abs(StartCell->SelfCellPos.R - TargetCell->SelfCellPos.R);
	return Distance_Q + Distance_R;
}

int AChessBoardManager::CalculateHashValue(int32 Q, int32 R)
{
	int tempQ = Q + 500;
	return (tempQ << 10) + (R + 500);
}

void AChessBoardManager::ChangeAllToWalkable()
{
	for (AHexCell* Cell : HexCells)
	{
		if (!Cell->bWalkable)
		{
			Cell->bWalkable = true;
		}
	}
}

TArray<int32> AChessBoardManager::GetNeighborsForGraph(FHexCellPosition Position)
{
	TArray<int32> Neighbors;
	TArray<FHexCellPosition> NeighborPositions;

	NeighborPositions.Add(FHexCellPosition(Position.Q + 1, Position.R));		// 0'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q + 1, Position.R - 1));	// 60'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q, Position.R - 1));		// 120'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q - 1, Position.R));		// 180'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q - 1, Position.R + 1));	// 240'方向
	NeighborPositions.Add(FHexCellPosition(Position.Q, Position.R + 1));		// 300'方向

	int index = 0;
	for (FHexCellPosition TempPos : NeighborPositions)
	{
		//AHexCell* Neighbor = *(HexCellPosMap.Find(CalculateHashValue(TempPos.Q, TempPos.R)));
		FHexCellPosition* NeighborPtr = hexCellSet.Find(TempPos);
		if (NeighborPtr != nullptr)
		{
			Neighbors.Add(index);
		}
		index++;
	}

	return Neighbors;
}

// 获取多边形轮廓的顶点列表  
void AChessBoardManager::GetIslandOutlineEdges(const TSet<FHexCellPosition> inHexCellSet)
{
	for (const auto hex : inHexCellSet) {
		TArray<int32> neighbors = GetNeighborsForGraph(hex);
		TArray<int32> outline = { 1,1,1,1,1,1 };
		for (int neighbor : neighbors)
		{
			outline[neighbor] = 0;
		}
		for (int index = 0; index < 6; index++)
		{
			if (outline[index] == 1)
			{
				FHexEdge outlineEdge = FHexEdge();
				switch (index)
				{
				case 0:
					outlineEdge.A.X = 2 * SQU3 * hex.Q + SQU3 * hex.R + SQU3;
					outlineEdge.A.Y = -3 * hex.R - 1;
					outlineEdge.B.X = 2 * SQU3 * hex.Q + SQU3 * hex.R + SQU3;
					outlineEdge.B.Y = -3 * hex.R + 1;
					break;
				case 1:
					outlineEdge.A.X = 2 * SQU3 * hex.Q + SQU3 * hex.R + SQU3;
					outlineEdge.A.Y = -3 * hex.R + 1;
					outlineEdge.B.X = 2 * SQU3 * hex.Q + SQU3 * hex.R;
					outlineEdge.B.Y = -3 * hex.R + 2;
					break;
				case 2:
					outlineEdge.A.X = 2 * SQU3 * hex.Q + SQU3 * hex.R;
					outlineEdge.A.Y = -3 * hex.R + 2;
					outlineEdge.B.X = 2 * SQU3 * hex.Q + SQU3 * hex.R - SQU3;
					outlineEdge.B.Y = -3 * hex.R + 1;
					break;
				case 3:
					outlineEdge.A.X = 2 * SQU3 * hex.Q + SQU3 * hex.R - SQU3;
					outlineEdge.A.Y = -3 * hex.R + 1;
					outlineEdge.B.X = 2 * SQU3 * hex.Q + SQU3 * hex.R - SQU3;
					outlineEdge.B.Y = -3 * hex.R - 1;
					break;
				case 4:
					outlineEdge.A.X = 2 * SQU3 * hex.Q + SQU3 * hex.R - SQU3;
					outlineEdge.A.Y = -3 * hex.R - 1;
					outlineEdge.B.X = 2 * SQU3 * hex.Q + SQU3 * hex.R;
					outlineEdge.B.Y = -3 * hex.R - 2;
					break;
				case 5:
					outlineEdge.A.X = 2 * SQU3 * hex.Q + SQU3 * hex.R;
					outlineEdge.A.Y = -3 * hex.R - 2;
					outlineEdge.B.X = 2 * SQU3 * hex.Q + SQU3 * hex.R + SQU3;
					outlineEdge.B.Y = -3 * hex.R - 1;
					break;
				default:
					break;
				}
				outlineEdgeSet.Add(outlineEdge.A, outlineEdge.B);
				UE_LOG(LogTemp, Warning, TEXT("start(%f,%f);end(%f,%f)"), outlineEdge.A.X, outlineEdge.A.Y, outlineEdge.B.X, outlineEdge.B.Y);
			}
		}
	}
}

TArray<FVector2f> AChessBoardManager::GetIslandOutlineVertices()
{
	TMap<FVector2f, FVector2f>::TIterator iter = outlineEdgeSet.CreateIterator();
	FVector2f* startPoint;
	FVector2f* endPoint;

	TArray<FVector2f> polygonVerticesArray;
	//polygonVerticesArray.Add(iter->Key);
	startPoint = outlineEdgeSet.Find(iter->Key);
	while (true)
	{
		endPoint = outlineEdgeSet.Find(*startPoint);
		if (endPoint == nullptr)
		{
			break;
		}
		polygonVerticesArray.Add(*endPoint * 100);
		UE_LOG(LogTemp, Warning, TEXT("Point(%f,%f)"), endPoint->X, endPoint->Y);
		outlineEdgeSet.Remove(*startPoint);

		startPoint = outlineEdgeSet.Find(*endPoint);
		if (startPoint == nullptr)
		{
			break;
		}
		polygonVerticesArray.Add(*startPoint * 100);
		UE_LOG(LogTemp, Warning, TEXT("Point(%f,%f)"), startPoint->X, startPoint->Y);
		outlineEdgeSet.Remove(*endPoint);
	}
	/*if (polygonVerticesTable != nullptr)
	{
		polygonVerticesTable->AddRow("d", FPolygonVertices(polygonVerticesArray));
	}*/
	return polygonVerticesArray;
}

