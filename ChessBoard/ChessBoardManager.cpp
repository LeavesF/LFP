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
	/*if (HexCellMesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No UnitMesh!"));
		return;
	}*/
	for (int32 Row = 0; Row < NumRows; Row++)
	{
		for (int32 Col = 0; Col < NumColumns; Col++)
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
	GetIslandOutline(HexCells);
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
		CellLocation.X = Row * EdgeLength * 1.5f;
		CellLocation.Y = Col * EdgeLength * 2 * M_COS30;
	}
	else
	{
		CellLocation.X = (Row * EdgeLength * 1.5f);
		CellLocation.Y = Col * EdgeLength * 2 * M_COS30 + (EdgeLength * M_COS30);
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

TMap<int, AHexCell*> AChessBoardManager::GetNeighborsForGraph(FHexCellPosition Position)
{
	TMap<int, AHexCell*> Neighbors;
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
			if (Neighbor /*&& Neighbor->height=curHeight*/)
			{
				Neighbors.Add(index, Neighbor);
			}
		}
		index++;
	}

	return Neighbors;
}

// 获取多边形轮廓的顶点列表  
TArray<AHexCell*> AChessBoardManager::GetIslandOutline(const TArray<AHexCell*>& hexArray) {
	TArray<AHexCell*> outline;

	for (const auto& hex : hexArray) {
		TArray<AHexCell*> neighbors = GetNeighbors(hex->SelfCellPos);
		if (neighbors.Num() < 6) {
			outline.Add(hex);
			UE_LOG(LogTemp, Log, TEXT("轮廓坐标 Q: %d, R: %d"), hex->SelfCellPos.Q, hex->SelfCellPos.R);
		}
	}

	//// 对轮廓顶点进行排序，确保顺时针或逆时针顺序  
	//std::sort(outline.begin(), outline.end(), [](const AHexCell& a, const AHexCell& b) {
	//	return std::atan2(a.SelfCellPos.Q - b.SelfCellPos.Q, a.SelfCellPos.R - b.SelfCellPos.R) < 0;
	//	});

	return outline;
}