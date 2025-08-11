#include "MapWidget.h"

#include "Components/Image.h"

void UMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ensure(MapCanvas && MapImage && PlayerIcon)) return;

	MapImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
}

FReply UMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float ScrollDelta = InMouseEvent.GetWheelDelta();
	CurrentZoom = FMath::Clamp(CurrentZoom + ScrollDelta * 0.1f, 0.5f, 3.0f);
	MapImage->SetRenderScale(FVector2D(CurrentZoom, CurrentZoom));
	return FReply::Handled();
}

FReply UMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDragging = true;
		LastMouseScreenPos = InMouseEvent.GetScreenSpacePosition();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UMapWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDragging)
	{
		FVector2D CurrentScreenPos = InMouseEvent.GetScreenSpacePosition();

		// 把屏幕 delta 转换为地图父级坐标空间 delta
		FVector2D LocalDelta = InGeometry.AbsoluteToLocal(CurrentScreenPos) - InGeometry.AbsoluteToLocal(LastMouseScreenPos);

		// 更新地图位置
		FVector2D CurrentTranslation = MapImage->GetRenderTransform().Translation;
		MapImage->SetRenderTranslation(CurrentTranslation + LocalDelta);

		LastMouseScreenPos = CurrentScreenPos;

		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UMapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDragging = false;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			Super::NativeTick(MyGeometry, InDeltaTime);

			// 1. 相对坐标 (0~1)
			FVector2D Relative = ConvertWorldToMap(PlayerPawn->GetActorLocation());

			// 2. 原始地图大小（贴图尺寸）
			FVector2D RawMapSize = MapImage->GetBrush().ImageSize;

			// 3. 缩放后地图大小
			FVector2D ScaledMapSize = RawMapSize * CurrentZoom;

			// 4. MapImage 的偏移量（RenderTranslation）
			FVector2D MapOffset = MapImage->GetRenderTransform().Translation;

			// 5. Pivot (0.5, 0.5) 意味着中心为地图参考点
			// 所以地图的左上角坐标 = 偏移 - 半宽半高
			FVector2D MapTopLeft = MapOffset - (ScaledMapSize * 0.5f);

			// 6. 最终坐标 = 左上角 + 相对坐标 * 缩放后地图尺寸
			FVector2D FinalIconPos = MapTopLeft + (Relative * ScaledMapSize);

			FString DebugInfo = FString::Printf(
				TEXT("[MapDebug]\nWorldPos: %.1f, %.1f\nRelative: %.2f, %.2f\nZoom: %.2f\nRawMapSize: %.1f, %.1f\nScaledMapSize: %.1f, %.1f\nMapOffset: %.1f, %.1f\nTopLeft: %.1f, %.1f\nFinalIconPos: %.1f, %.1f"),
				PlayerPawn->GetActorLocation().X, PlayerPawn->GetActorLocation().Y,
				Relative.X, Relative.Y,
				CurrentZoom,
				RawMapSize.X, RawMapSize.Y,
				ScaledMapSize.X, ScaledMapSize.Y,
				MapOffset.X, MapOffset.Y,
				MapTopLeft.X, MapTopLeft.Y,
				FinalIconPos.X, FinalIconPos.Y
			);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					1,                      // Key
					0.f,                    // Display time (0 = 每帧刷新)
					FColor::Cyan,
					DebugInfo
				);
			}

			PlayerIcon->SetRenderTranslation(FinalIconPos);
			float Yaw = PlayerPawn->GetActorRotation().Yaw;
			PlayerIcon->SetRenderTransformAngle(Yaw);
		}
	}
}

FVector2D UMapWidget::ConvertWorldToMap(FVector WorldPos)
{
	FVector2D WorldOrigin(0.0f, 0.0f);
	FVector2D WorldSize(3500.f, 3000.f); // X=Y方向，Y=X方向

	// 计算相对坐标 这里是因为场景的宽高是YX UMG的宽高是XY
	float RelativeX = (WorldPos.Y - WorldOrigin.Y) / WorldSize.X;
	float RelativeY = (WorldPos.X - WorldOrigin.X) / WorldSize.Y;

	// 反转Y轴（UMG的Y轴向下，世界是向上）
	RelativeY = 1.0f - RelativeY;

	return FVector2D(
		FMath::Clamp(RelativeX, 0.0f, 1.0f),
		FMath::Clamp(RelativeY, 0.0f, 1.0f)
	);
}

