// Fill out your copyright notice in the Description page of Project Settings.


#include "LFP/InteractableItems/InteractableItemBase.h"

#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "LFP/UI/ItemInfoWidget.h"

// Sets default values
AInteractableItemBase::AInteractableItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleTrigger"));
	CapsuleTrigger->InitCapsuleSize(120.f, 240.f);
	CapsuleTrigger->SetCollisionProfileName(TEXT("InteractableItem"));
	CapsuleTrigger->SetGenerateOverlapEvents(true);
	RootComponent = CapsuleTrigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	Mesh->SetupAttachment(CapsuleTrigger);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComp->SetupAttachment(CapsuleTrigger);

	FocusCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FocusCircle"));
	FocusCircle->SetupAttachment(CapsuleTrigger);
	FocusCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AInteractableItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	InteractableDistance = CapsuleTrigger->GetScaledCapsuleRadius() * 0.8f;

	CapsuleTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractableItemBase::OnTriggerBeginOverlap);
	CapsuleTrigger->OnComponentEndOverlap.AddDynamic(this, &AInteractableItemBase::OnTriggerEndOverlap);

	SetInfoWidget();
}

// Called every frame
void AInteractableItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableItemBase::ShowInfo(bool bShow)
{
	WidgetComp->SetHiddenInGame(!bShow);
	if (FocusCircle)
	{
		FocusCircle->SetHiddenInGame(!bShow);
	}
	if (bShow)
	{
		//UE_LOG(LogTemp, Warning, TEXT("I'm showing my name"));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("I just hide my name"));
	}
}

void AInteractableItemBase::SetInfoWidget()
{
	UItemInfoWidget* InfoWidget = Cast<UItemInfoWidget>(WidgetComp->GetWidget());
	if (InfoWidget)
	{
		InfoWidget->SetNameText(ItemName);
	}
}

FVector AInteractableItemBase::SimpleGetWalkableStandPoint(const FVector& PlayerLocation, float NewInteractableDistance /*= 0*/)
{
	if (NewInteractableDistance <= 0)
	{
		NewInteractableDistance = InteractableDistance;
	}
	FVector ItemLocation = GetActorLocation();
	FVector PlayerDirection = PlayerLocation - ItemLocation;
	PlayerDirection.Z = 0;
	PlayerDirection.Normalize();

	FHitResult HitResult;
	FVector StartLocation = ItemLocation + (PlayerDirection * NewInteractableDistance);
	FVector EndLocation = StartLocation - FVector(0, 0, 1000.f); // 向下延伸射线的终点位置

#ifdef UE_BUILD_DEBUG
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 5.f, 0, 5.f);
	DrawDebugLine(GetWorld(), PlayerLocation, ItemLocation, FColor::Green, false, 5.f, 0, 5.f);
#endif

	// 向Item射线检测
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
	{
		if (HitResult.GetActor()->ActorHasTag("Walkable"))
		{
			return HitResult.Location;
		}
	}

	return ComplexGetWalkableStandPoint(PlayerLocation, NewInteractableDistance);	
}

FVector AInteractableItemBase::ComplexGetWalkableStandPoint(const FVector& PlayerLocation, float NewInteractableDistance)
{
	if (NewInteractableDistance <= 0)
	{
		NewInteractableDistance = InteractableDistance;
	}

	FVector ItemLocation = GetActorLocation();
	TArray<FVector> StandPoints;

	// 弧度转换
	float Degrees = 30.0f;
	float Radians = FMath::DegreesToRadians(Degrees);
	float Distance_Cos30 = NewInteractableDistance * FMath::Cos(Radians);
	float Distance_Sin30 = NewInteractableDistance * FMath::Sin(Radians);

	// Add 4 points around Item for player choosing to stand
	StandPoints.Add(ItemLocation + FVector(NewInteractableDistance, 0.f, 0.f));
	StandPoints.Add(ItemLocation + FVector(-NewInteractableDistance, 0.f, 0.f));
	StandPoints.Add(ItemLocation + FVector(0.f, NewInteractableDistance, 0.f));
	StandPoints.Add(ItemLocation + FVector(0.f, -NewInteractableDistance, 0.f));

	StandPoints.Add(ItemLocation + FVector(Distance_Cos30, Distance_Sin30, 0.f));
	StandPoints.Add(ItemLocation + FVector(Distance_Cos30, -Distance_Sin30, 0.f));
	StandPoints.Add(ItemLocation + FVector(-Distance_Cos30, Distance_Sin30, 0.f));
	StandPoints.Add(ItemLocation + FVector(-Distance_Cos30, -Distance_Sin30, 0.f));
	StandPoints.Add(ItemLocation + FVector(Distance_Sin30, Distance_Cos30, 0.f));
	StandPoints.Add(ItemLocation + FVector(Distance_Sin30, -Distance_Cos30, 0.f));
	StandPoints.Add(ItemLocation + FVector(-Distance_Sin30, Distance_Cos30, 0.f));
	StandPoints.Add(ItemLocation + FVector(-Distance_Sin30, -Distance_Cos30, 0.f));

	float MinDistance = TNumericLimits<float>::Max(); // 初始化最小距离为最大值
	FVector NearestPoint = PlayerLocation;

	// 遍历四个点
	for (const FVector& StandPoint : StandPoints)
	{
		float Distance = FVector::Dist(StandPoint, PlayerLocation); // 计算点与玩家位置的距离

		// 当距离小于最小距离时进行射线检测
		if (Distance < MinDistance)
		{
			FHitResult HitResult;
			FVector StartLocation = StandPoint;
			FVector EndLocation = StandPoint - FVector(0, 0, 1000.f); // 向下延伸射线的终点位置

#ifdef UE_BUILD_DEBUG
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.f, 0, 5.f);
#endif

			// 进行向下射线检测
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility))
			{
				if (HitResult.GetActor()->ActorHasTag("Walkable"))
				{
					MinDistance = Distance;
					NearestPoint = HitResult.Location;
				}
			}
		}
	}

	return NearestPoint;
}

void AInteractableItemBase::ExecItemFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Exec Succeed:%s"), *ItemName.ToString());
}

void AInteractableItemBase::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsOverlapWithPlayer = true;

	if (!bIsFocusItem)
	{
		return;
	}

	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ExecItemFunction();
	}
	bIsFocusItem = false;
}

void AInteractableItemBase::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsOverlapWithPlayer = false;
}