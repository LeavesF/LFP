// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableItemBase.generated.h"

class UCapsuleComponent;
class UWidgetComponent;

UCLASS()
class LOOKINGFORPANDURIA_API AInteractableItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void ShowInfo(bool bShow = true);

	UPROPERTY(EditAnywhere)
	FName ItemName;

// Init Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interact, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> WidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> FocusCircle;

// Info UI
protected:
	UFUNCTION(BlueprintCallable)
	void SetInfoWidget();

// Interact
public:
	FVector SimpleGetWalkableStandPoint(const FVector& PlayerLocation, float NewInteractableDistance = 0);
	FVector ComplexGetWalkableStandPoint(const FVector& PlayerLocation, float NewInteractableDistance = 0);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interact, meta = (AllowPrivateAccess = "true"))
	float InteractableDistance = 90.f;
	// Factor * Radius = InteractableDistance
	float InteractableDistance_Factor = 0.8f;

public:
	bool bIsFocusItem{ false };
	bool bIsOverlapWithPlayer{ false };

	virtual void ExecItemFunction();

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
