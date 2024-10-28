// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HexCellInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOOKINGFORPANDURIA_API UHexCellInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetCellInfo(int32 q,int32 r);

};
