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
	CreateChessBoard();
	//hexCellSet.Empty();
}

// Called every frame
void AChessBoardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessBoardManager::CreateChessBoard()
{
	if (hexCellTable == nullptr)
	{
		return;
	}
	hexCellSet.Empty();
	polygonVerticesTable->EmptyTable();

	for (auto it : hexCellTable->GetRowMap())
	{
		FHexCellInfo* row = (FHexCellInfo*)it.Value;
		if (row)
		{
			hexCellSet.Add(*row);
		}
	}
	int32 index = 0;
	for (int32 height = 0; height < 4; height++)
	{
		GetIslandOutlineEdges(hexCellSetPtr, height);
		if (outlineEdgeSet.IsEmpty())
		{
			continue;
		}
		while (index < 1024)
		{
			FString tempString = FString::Printf(TEXT("Vertices_%d"), index);
			FName rowName(*tempString);
			TArray<FVector2f> polygonVerticesArray = GetIslandOutlineVertices();
			if (polygonVerticesTable != nullptr)
			{
				if (!polygonVerticesArray.IsEmpty())
				{
					polygonVerticesTable->AddRow(rowName, FPolygonVertices(polygonVerticesArray, height));
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
			index++;
		}
	}
}

void AChessBoardManager::ClearChessBoard()
{
	hexCellSet.Empty();
	OnClearChessBoardEvent.Broadcast();
	OnClearChessBoardEvent.Clear();
}

void AChessBoardManager::ShowChessBoard(bool bShow /*= false*/)
{
	OnShowChessBoardEvent.Broadcast(bShow);
}

//TArray<FHexCellInfo> AChessBoardManager::FindPath(FHexCellInfo StartCell, FHexCellInfo TargetCell)
//{
//	TArray<FHexCellInfo> ToSearch;
//	TArray<FHexCellInfo> Processed;
//	TArray<FHexCellInfo> Path;
//	/*if (StartCell == nullptr || TargetCell == nullptr)
//	{
//		return Path;
//	}*/
//	StartCell->AStar_H = GetDistanceBetweenTwoHexCell(StartCell, TargetCell);
//	StartCell->AStar_F = StartCell->AStar_H;
//	ToSearch.Add(StartCell);
//
//	while (!ToSearch.IsEmpty())
//	{
//		AHexCell* Current = ToSearch[0];
//		for (AHexCell* Temp : ToSearch)
//		{
//			if (Temp->AStar_F < Current->AStar_F || (Temp->AStar_F == Current->AStar_F && Temp->AStar_H < Current->AStar_H))
//			{
//				Current = Temp;
//			}
//		}
//
//		Processed.Add(Current);
//		ToSearch.Remove(Current);
//
//		if (Current == TargetCell)
//		{
//			AHexCell* CurrentPathTile = TargetCell;
//			while (CurrentPathTile != StartCell)
//			{
//				Path.Add(CurrentPathTile);
//				CurrentPathTile = CurrentPathTile->ConnectedCell;
//			}
//			Path.Add(StartCell);
//		}
//
//		TArray<AHexCell*> neighbors = GetNeighbors(Current->SelfCellPos);
//		for (AHexCell* Neighbor : neighbors)
//		{
//			if (Processed.Contains(Neighbor))
//			{
//				continue;
//			}
//
//			bool bInSearch = ToSearch.Contains(Neighbor);
//			//TODO: int32 CostToNeighbor = Current->AStar_G + Current->GetDistance(Neighbor);
//			int32 CostToNeighbor = Current->AStar_G + 1;
//
//			if (!bInSearch || CostToNeighbor < Neighbor->AStar_G)
//			{
//				Neighbor->AStar_G = CostToNeighbor;
//				Neighbor->ConnectedCell = Current;
//
//				if (!bInSearch)
//				{
//					Neighbor->AStar_H = GetDistanceBetweenTwoHexCell(Neighbor, TargetCell);
//					Neighbor->AStar_F = Neighbor->AStar_G + Neighbor->AStar_H;
//					ToSearch.Add(Neighbor);
//				}
//			}
//		}
//	}
//
//	return Path;
//}

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

TArray<FHexCellInfo*> AChessBoardManager::GetNeighbors(FHexCellInfo* Position)
{
	TArray<FHexCellInfo*> neighbors;
	TArray<FHexCellInfo> neighborPositions;

	neighborPositions.Add(FHexCellInfo(Position->Q + 1, Position->R));		// 0'方向
	neighborPositions.Add(FHexCellInfo(Position->Q + 1, Position->R - 1));	// 60'方向
	neighborPositions.Add(FHexCellInfo(Position->Q, Position->R - 1));		// 120'方向
	neighborPositions.Add(FHexCellInfo(Position->Q - 1, Position->R));		// 180'方向
	neighborPositions.Add(FHexCellInfo(Position->Q - 1, Position->R + 1));	// 240'方向
	neighborPositions.Add(FHexCellInfo(Position->Q, Position->R + 1));		// 300'方向

	int index = 0;
	for (FHexCellInfo TempPos : neighborPositions)
	{
		FHexCellInfo* neighborPtr = hexCellSet.Find(TempPos);
		if (neighborPtr != nullptr)
		{
			if (neighborPtr && neighborPtr->bWalkable)
			{
				neighbors.Add(neighborPtr);
			}
			index++;
		}
	}

	return neighbors;
}

int32 AChessBoardManager::GetDistanceBetweenTwoHexCell(FHexCellInfo* StartCell, FHexCellInfo* TargetCell)
{
	int32 Distance_Q = FMath::Abs(StartCell->Q - TargetCell->Q);
	int32 Distance_R = FMath::Abs(StartCell->R - TargetCell->R);
	return Distance_Q + Distance_R;
}

int AChessBoardManager::CalculateHashValue(int32 Q, int32 R)
{
	int tempQ = Q + 500;
	return (tempQ << 10) + (R + 500);
}

void AChessBoardManager::ChangeAllToWalkable()
{

}

TArray<int32> AChessBoardManager::GetNeighborsForGraph(FHexCellInfo* Position)
{
	TArray<int32> neighbors;
	TArray<FHexCellInfo> neighborPositions;

	neighborPositions.Add(FHexCellInfo(Position->Q + 1, Position->R));		// 0'方向
	neighborPositions.Add(FHexCellInfo(Position->Q + 1, Position->R - 1));	// 60'方向
	neighborPositions.Add(FHexCellInfo(Position->Q, Position->R - 1));		// 120'方向
	neighborPositions.Add(FHexCellInfo(Position->Q - 1, Position->R));		// 180'方向
	neighborPositions.Add(FHexCellInfo(Position->Q - 1, Position->R + 1));	// 240'方向
	neighborPositions.Add(FHexCellInfo(Position->Q, Position->R + 1));		// 300'方向

	int index = 0;
	for (FHexCellInfo TempPos : neighborPositions)
	{
		FHexCellInfo* NeighborPtr = hexCellSet.Find(TempPos);
		if (NeighborPtr != nullptr and NeighborPtr->Height == Position->Height)
		{
			neighbors.Add(index);
		}
		index++;
	}

	return neighbors;
}

// 获取多边形轮廓的顶点列表  
void AChessBoardManager::GetIslandOutlineEdges(const TSet<FHexCellInfo>* inHexCellSet, int32 height)
{
	outlineEdgeSet.Empty();

	for (auto it : *inHexCellSet) 
	{
		FHexCellInfo* hex = &it;
		if (hex->Height != height)
		{
			continue;
		}

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
					outlineEdge.A.X = 2 * SQU3 * hex->Q + SQU3 * hex->R + SQU3;
					outlineEdge.A.Y = 3 * hex->R - 1;
					outlineEdge.B.X = 2 * SQU3 * hex->Q + SQU3 * hex->R + SQU3;
					outlineEdge.B.Y = 3 * hex->R + 1;
					break;
				case 5:
					outlineEdge.A.X = 2 * SQU3 * hex->Q + SQU3 * hex->R + SQU3;
					outlineEdge.A.Y = 3 * hex->R + 1;
					outlineEdge.B.X = 2 * SQU3 * hex->Q + SQU3 * hex->R;
					outlineEdge.B.Y = 3 * hex->R + 2;
					break;
				case 4:
					outlineEdge.A.X = 2 * SQU3 * hex->Q + SQU3 * hex->R;
					outlineEdge.A.Y = 3 * hex->R + 2;
					outlineEdge.B.X = 2 * SQU3 * hex->Q + SQU3 * hex->R - SQU3;
					outlineEdge.B.Y = 3 * hex->R + 1;
					break;
				case 3:
					outlineEdge.A.X = 2 * SQU3 * hex->Q + SQU3 * hex->R - SQU3;
					outlineEdge.A.Y = 3 * hex->R + 1;
					outlineEdge.B.X = 2 * SQU3 * hex->Q + SQU3 * hex->R - SQU3;
					outlineEdge.B.Y = 3 * hex->R - 1;
					break;
				case 2:
					outlineEdge.A.X = 2 * SQU3 * hex->Q + SQU3 * hex->R - SQU3;
					outlineEdge.A.Y = 3 * hex->R - 1;
					outlineEdge.B.X = 2 * SQU3 * hex->Q + SQU3 * hex->R;
					outlineEdge.B.Y = 3 * hex->R - 2;
					break;
				case 1:
					outlineEdge.A.X = 2 * SQU3 * hex->Q + SQU3 * hex->R;
					outlineEdge.A.Y = 3 * hex->R - 2;
					outlineEdge.B.X = 2 * SQU3 * hex->Q + SQU3 * hex->R + SQU3;
					outlineEdge.B.Y = 3 * hex->R - 1;
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
		if (startPoint == nullptr)
		{
			break;
		}
		endPoint = outlineEdgeSet.Find(*startPoint);
		if (endPoint == nullptr)
		{
			break;
		}
		polygonVerticesArray.Add(*endPoint * edgeLength * 0.5f);
		UE_LOG(LogTemp, Warning, TEXT("Point(%f,%f)"), endPoint->X, endPoint->Y);
		outlineEdgeSet.Remove(*startPoint);

		startPoint = outlineEdgeSet.Find(*endPoint);
		if (startPoint == nullptr)
		{
			break;
		}
		polygonVerticesArray.Add(*startPoint * edgeLength * 0.5f);
		UE_LOG(LogTemp, Warning, TEXT("Point(%f,%f)"), startPoint->X, startPoint->Y);
		outlineEdgeSet.Remove(*endPoint);
	}

	return polygonVerticesArray;
}

FHexCellInfo* AChessBoardManager::GetHexCellUnderXYPoint(FVector position)
{
	float unit_x = edgeLength * SQU3;
	float unit_y = edgeLength * 1.5f;
	float minDistanceLimit = edgeLength * SQU3 * 0.5f;
	TArray<FVector2D> anchors;
	anchors.Add(FVector2D::ZeroVector);
	anchors.Add(FVector2D::ZeroVector);
	anchors.Add(FVector2D::ZeroVector);
	float distance;
	float minDistance = 10000;
	int index = 0;

	int32 index_x = position.X / unit_x - (position.X < 0 ? 1 : 0);
	int32 index_y = position.Y / unit_y - (position.Y < 0 ? 1 : 0);
	anchors[0].X = index_x * unit_x;
	anchors[1].X = (index_x + 0.5) * unit_x;
	anchors[2].X = (index_x + 1) * unit_x;
	if (index_y % 2 == 0)
	{
		anchors[0].Y = anchors[2].Y = index_y * unit_y;
		anchors[1].Y = (index_y + 1) * unit_y;
	}
	else
	{
		anchors[0].Y = anchors[2].Y = (index_y + 1) * unit_y;
		anchors[1].Y = index_y * unit_y;
	}
	// 找出距离坐标最近的一个点
	for (int i = 0; i < 3; i++)
	{
		//求出距离的平方
		UE_LOG(LogTemp, Warning, TEXT("anchor:(%f,%f)"), anchors[i].X, anchors[i].Y);
		distance = FVector2D::Distance(FVector2D(position.X, position.Y), FVector2D(anchors[i].X, anchors[i].Y));
		UE_LOG(LogTemp, Warning, TEXT("distance:(%f)"), distance);
		//如果已经肯定被捕获
		if (distance < minDistanceLimit)
		{
			index = i;
			break;
		}

		//更新最小距离值和索引
		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}
	}

	int32 q = (anchors[index].X / SQU3 - anchors[index].Y / 3.f) / edgeLength;
	int32 r = anchors[index].Y * 2.f / 3.f / edgeLength;
	UE_LOG(LogTemp, Warning, TEXT("index:%d XY:(%f,%f)"), index, anchors[index].X, anchors[index].Y);
	UE_LOG(LogTemp, Warning, TEXT("QRS:(%d,%d)"), q, r);
	if (FHexCellInfo* hexPtr = hexCellSet.Find(FHexCellInfo(q,r)))
	{
		return hexPtr;
	}
	return nullptr;
}

FVector AChessBoardManager::GetXYPointOfHexCell(int32 q, int32 r, FVector position)
{
	position.X = (SQU3 * q + SQU3 / 2.f * r) * edgeLength;
	position.Y = 1.5f * r * edgeLength;
	return position;
}

