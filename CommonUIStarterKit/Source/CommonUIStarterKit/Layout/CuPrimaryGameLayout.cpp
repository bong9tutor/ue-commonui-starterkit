// CuPrimaryGameLayout.cpp
#include "Layout/CuPrimaryGameLayout.h"

#include "CommonActivatableWidget.h"
#include "Layout/CuGameplayTags.h"
#include "System/CuLocalPlayer.h"
#include "System/CuGameUIManagerSubsystem.h"
#include "System/CuGameUIPolicy.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

UCuPrimaryGameLayout* UCuPrimaryGameLayout::GetPrimaryGameLayout(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		return GetPrimaryGameLayout(PlayerController->GetLocalPlayer());
	}
	return nullptr;
}

UCuPrimaryGameLayout* UCuPrimaryGameLayout::GetPrimaryGameLayout(ULocalPlayer* LocalPlayer)
{
	// subsystem → 현재 policy → 이 플레이어의 루트 레이아웃, 순으로 해석한다.
	if (LocalPlayer)
	{
		if (const UGameInstance* GameInstance = LocalPlayer->GetGameInstance())
		{
			if (UCuGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UCuGameUIManagerSubsystem>())
			{
				if (const UCuGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy())
				{
					return Policy->GetRootLayout(Cast<UCuLocalPlayer>(LocalPlayer));
				}
			}
		}
	}
	return nullptr;
}

UCuPrimaryGameLayout* UCuPrimaryGameLayout::GetPrimaryGameLayoutForPlayer(APlayerController* PlayerController)
{
	return GetPrimaryGameLayout(PlayerController);
}

void UCuPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// ⚠️ BindWidget만으로는 tag→container 맵이 비어 있다 → 여기서 명시적으로 등록.
	//    Stage 2는 Menu만 존재하고 나머지는 null일 수 있으므로 null 가드.
	if (Layer_Game)
	{
		RegisterLayer(TAG_UI_Layer_Game, Layer_Game);
	}
	if (Layer_GameMenu)
	{
		RegisterLayer(TAG_UI_Layer_GameMenu, Layer_GameMenu);
	}
	if (Layer_Menu)
	{
		RegisterLayer(TAG_UI_Layer_Menu, Layer_Menu);
	}
	if (Layer_Modal)
	{
		RegisterLayer(TAG_UI_Layer_Modal, Layer_Modal);
	}
}

void UCuPrimaryGameLayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (LayerWidget)
	{
		Layers.Add(LayerTag, LayerWidget);
	}
}

UCommonActivatableWidgetContainerBase* UCuPrimaryGameLayout::GetLayerWidget(FGameplayTag LayerTag) const
{
	if (const TObjectPtr<UCommonActivatableWidgetContainerBase>* Found = Layers.Find(LayerTag))
	{
		return *Found;
	}
	return nullptr;
}

void UCuPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	if (!ActivatableWidget)
	{
		return;
	}
	// 어느 레이어에 있는지 모르므로 전체 레이어에서 제거 시도.
	for (const TPair<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>>& Pair : Layers)
	{
		if (Pair.Value)
		{
			Pair.Value->RemoveWidget(*ActivatableWidget);
		}
	}
}

// --- [세션 C Stage 2+] BP 저작용 래퍼 구현 ---

UCommonActivatableWidget* UCuPrimaryGameLayout::PushWidgetToLayer(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!WidgetClass)
	{
		return nullptr;
	}
	// template push의 BP 래퍼(base 타입으로 push). GetLayerWidget이 null이면 template이 nullptr 반환.
	return PushWidgetToLayerStack<UCommonActivatableWidget>(LayerTag, WidgetClass);
}

void UCuPrimaryGameLayout::RemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	FindAndRemoveWidgetFromLayer(ActivatableWidget);
}

FGameplayTag UCuPrimaryGameLayout::GetLayerTag_Game()     { return TAG_UI_Layer_Game; }
FGameplayTag UCuPrimaryGameLayout::GetLayerTag_GameMenu() { return TAG_UI_Layer_GameMenu; }
FGameplayTag UCuPrimaryGameLayout::GetLayerTag_Menu()     { return TAG_UI_Layer_Menu; }
FGameplayTag UCuPrimaryGameLayout::GetLayerTag_Modal()    { return TAG_UI_Layer_Modal; }
