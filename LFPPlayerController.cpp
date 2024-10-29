// Copyright Epic Games, Inc. All Rights Reserved.

#include "LFPPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "LFPCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "LFP/InteractableItems/InteractableItemBase.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ALFPPlayerController::ALFPPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ALFPPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	GetItemUnderCursor();
}

void ALFPPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ALFPPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ALFPPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ALFPPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ALFPPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ALFPPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ALFPPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ALFPPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ALFPPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ALFPPlayerController::OnTouchReleased);

		EnhancedInputComponent->BindAction(SetTargetHexCellClickAction, ETriggerEvent::Started, this, &ALFPPlayerController::OnSetTargetHexCellStarted);
	}
}

void ALFPPlayerController::OnInputStarted()
{
	//StopMovement();
}

// Triggered every frame when the input is held down
void ALFPPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
		//if angle of slope > 45 degree, don't SimpleMove
		bSlopeAllowMoveTo = Hit.Normal.Z > 0.7 ? true : false;
		bHitWalkableGround = Hit.GetActor()->ActorHasTag("Walkable");
	}

	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		if (FollowTime >= ShortPressThreshold)
		{
			FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
		}
	}
}

void ALFPPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		//TO DELETE
		//GetHexCellUnderCursor();

		if (bHitWalkableGround && bSlopeAllowMoveTo)
		{
			// Click on Walkable Ground
		}
		else if (LastItem)
		{
			// Click on Interactable Item
			CachedDestination = LastItem->SimpleGetWalkableStandPoint(GetPawn()->GetActorLocation());

			if (LastItem->bIsOverlapWithPlayer)
			{
				// Player has overlapped with Item
				LastItem->ExecItemFunction();
			}
			else
			{
				// Player hasn't overlapped with Item yet
				LastItem->bIsFocusItem = true;
			}
		}
		else
		{
			return;
		}

		// We move to CachedDestination and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);

	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ALFPPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ALFPPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ALFPPlayerController::OnSetTargetHexCellStarted()
{
	AHexCell* HexCell = GetHexCellUnderCursor();
	if (HexCell)
	{
		FHitResult Hit;
		FVector StartLoc = HexCell->GetActorLocation();
		FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), true);

		//DrawDebugLine(GetWorld(), Hit.Location, Hit.Location + FVector::UpVector * 2000.f, FColor::Blue, false, 5.f, 0, 5.f);
		if (GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, StartLoc + FVector::DownVector * 2000.f, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
		{
			CachedDestination = Hit.Location;
			//TO DELETE
			// We move to CachedDestination and spawn some particles
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
	}
}

void ALFPPlayerController::GetItemUnderCursor()
{
	FHitResult Hit;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
	{
		AInteractableItemBase* CurItem = Cast<AInteractableItemBase>(Hit.GetActor());
		if (CurItem)
		{
			if (LastItem == CurItem)
			{
				//If the item was not changed, do nothing
			}
			else
			{
				if (LastItem)
				{
					//LastItem stop highlight
					LastItem->ShowInfo(false);
				}
				//CurItem start highlight
				CurItem->ShowInfo(true);
				LastItem = CurItem;

				//UE_LOG(LogTemp, Warning, TEXT("Change"));
			}
		}
		else
		{
			if (LastItem)
			{
				//LastItem stop highlight
				LastItem->ShowInfo(false);
				LastItem = nullptr;
			}
		}
	}

}

AHexCell* ALFPPlayerController::GetHexCellUnderCursor()
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