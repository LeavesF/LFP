// Fill out your copyright notice in the Description page of Project Settings.


#include "LFP_CreateMode_PlayerController.h"

ALFP_CreateMode_PlayerController::ALFP_CreateMode_PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ALFP_CreateMode_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ALFP_CreateMode_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ALFP_CreateMode_PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

AHexCell* ALFP_CreateMode_PlayerController::GetHexCellUnderCursor()
{
	FHitResult Hit;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		if (!Hit.GetActor()->ActorHasTag("Walkable"))
		{
			return nullptr;
		}
		FHitResult Hit2;
		FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), true);

		//DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + FVector::UpVector * 2000.f, FColor::Blue, false, 5.f, 0, 5.f);
		if (GetWorld()->LineTraceSingleByChannel(Hit2, Hit.Location, Hit.Location + FVector::UpVector * 2000.f, ECollisionChannel::ECC_GameTraceChannel1, CollisionQueryParams))
		{
			if (AHexCell* HexCell = Cast<AHexCell>(Hit2.GetActor()))
			{
				HexCell->OnHighlightHexCellEvent.ExecuteIfBound();
				return HexCell;
				//UE_LOG(LogTemp, Warning, TEXT("HitHexCell"));
			}
		}
	}
	return nullptr;
}
