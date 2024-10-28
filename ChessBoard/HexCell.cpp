// Fill out your copyright notice in the Description page of Project Settings.


#include "LookingForPanduria/ChessBoard/HexCell.h"

// Sets default values
AHexCell::AHexCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBoom"));
	RootComponent = MeshComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(RootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HexCellInfoWidget"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetCastShadow(false);

	Tags.Add("HexCell");
	Tags.Add("ChessBoard");
}

// Called when the game starts or when spawned
void AHexCell::BeginPlay()
{
	Super::BeginPlay();
	
	OnHighlightHexCellEvent.BindDynamic(this, &AHexCell::HighlightHexCell);
	OnHighlightHexCellConstantEvent.BindDynamic(this, &AHexCell::SetHighlight);
}

// Called every frame
void AHexCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHexCell::CellDestroy()
{
	Destroy();
}

void AHexCell::ShowDecal(bool bShow)
{
	DecalComponent->SetHiddenInGame(!bShow);
}

void AHexCell::ExecSetHighlight()
{
	OnHighlightHexCellConstantEvent.ExecuteIfBound();
}
