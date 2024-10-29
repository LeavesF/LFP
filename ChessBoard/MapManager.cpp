// Fill out your copyright notice in the Description page of Project Settings.


#include "LFP/ChessBoard/MapManager.h"

AMapManager::AMapManager()
{

}

void AMapManager::BeginPlay()
{

}

void AMapManager::Tick(float DeltaTime)
{

}

void AMapManager::AddHexCell(AHexCell* Cell)
{
	FHexCellData tempHexCell;
	tempHexCell.Location = Cell->SelfCellPos;

	MapData.Add(tempHexCell);
}

void AMapManager::SaveMapData(FString Mapdata)
{
}

void AMapManager::LoadMapdata(FString Mapdata)
{
}
