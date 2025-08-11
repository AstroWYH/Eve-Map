// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EveMapCharacter.generated.h"

class UMapWidget;

UCLASS(Blueprintable)
class AEveMapCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEveMapCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	UFUNCTION(BlueprintCallable)
	void ToggleMap();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MapWidgetClass;

	UPROPERTY()
	UMapWidget* MapWidgetInstance;

	bool bMapVisible = false;
};

