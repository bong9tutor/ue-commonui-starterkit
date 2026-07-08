// CuGameUIPolicy.cpp
#include "System/CuGameUIPolicy.h"

#include "System/CuLocalPlayer.h"
#include "System/CuGameUIManagerSubsystem.h"
#include "Layout/CuPrimaryGameLayout.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UCuGameUIPolicy* UCuGameUIPolicy::GetGameUIPolicy(const UObject* WorldContextObject)
{
	if (GEngine && WorldContextObject)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (UCuGameUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UCuGameUIManagerSubsystem>())
				{
					return UIManager->GetCurrentUIPolicy();
				}
			}
		}
	}
	return nullptr;
}

UWorld* UCuGameUIPolicy::GetWorld() const
{
	// CDO에서 호출되면 outer가 subsystem이 아니므로 안전하게 null.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}
	if (const UCuGameUIManagerSubsystem* Manager = GetOwningUIManager())
	{
		if (const UGameInstance* GameInstance = Manager->GetGameInstance())
		{
			return GameInstance->GetWorld();
		}
	}
	return nullptr;
}

UCuGameUIManagerSubsystem* UCuGameUIPolicy::GetOwningUIManager() const
{
	// Within=CuGameUIManagerSubsystem이 생성해 주는 typed-outer 접근자.
	return GetOuterUCuGameUIManagerSubsystem();
}

UCuPrimaryGameLayout* UCuGameUIPolicy::GetRootLayout(const UCuLocalPlayer* LocalPlayer) const
{
	const FCuRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	return LayoutInfo ? ToRawPtr(LayoutInfo->RootLayout) : nullptr;
}

void UCuGameUIPolicy::NotifyPlayerAdded(UCuLocalPlayer* LocalPlayer)
{
	if (!ensure(LocalPlayer))
	{
		return;
	}

	// PlayerController가 (재)설정되면 레이아웃을 (재)생성하도록 구독.
	//  - 세션 C의 PIE에서 타이밍을 검증한다. GameMode PostLogin 경로(subsystem)와 함께
	//    이 델리게이트 경로가 PC 교체/지연 생성 케이스를 커버한다.
	LocalPlayer->OnPlayerControllerSet.AddWeakLambda(this, [this](UCuLocalPlayer* InLocalPlayer, APlayerController* /*PlayerController*/)
	{
		NotifyPlayerRemoved(InLocalPlayer);
		CreateLayoutWidget(InLocalPlayer);
	});

	if (FCuRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		if (!LayoutInfo->bAddedToViewport)
		{
			AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
			LayoutInfo->bAddedToViewport = true;
		}
	}
	else
	{
		CreateLayoutWidget(LocalPlayer);
	}
}

void UCuGameUIPolicy::NotifyPlayerRemoved(UCuLocalPlayer* LocalPlayer)
{
	if (!LocalPlayer)
	{
		return;
	}
	if (FCuRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = false;
	}
}

void UCuGameUIPolicy::NotifyPlayerDestroyed(UCuLocalPlayer* LocalPlayer)
{
	if (!LocalPlayer)
	{
		return;
	}
	NotifyPlayerRemoved(LocalPlayer);
	const int32 Index = RootViewportLayouts.IndexOfByKey(LocalPlayer);
	if (Index != INDEX_NONE)
	{
		RootViewportLayouts.RemoveAt(Index);
	}
}

void UCuGameUIPolicy::CreateLayoutWidget(UCuLocalPlayer* LocalPlayer)
{
	if (!LocalPlayer)
	{
		return;
	}
	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld()))
	{
		TSubclassOf<UCuPrimaryGameLayout> LayoutWidgetClass = GetLayoutWidgetClass(LocalPlayer);
		if (ensure(LayoutWidgetClass && !LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UCuPrimaryGameLayout* NewLayout = CreateWidget<UCuPrimaryGameLayout>(PlayerController, LayoutWidgetClass);
			RootViewportLayouts.Emplace(LocalPlayer, NewLayout, /*bIsInViewport*/ true);
			AddLayoutToViewport(LocalPlayer, NewLayout);
		}
	}
}

TSubclassOf<UCuPrimaryGameLayout> UCuGameUIPolicy::GetLayoutWidgetClass(UCuLocalPlayer* LocalPlayer)
{
	// soft class를 동기 로드. (LayoutClass는 세션 C의 WBP_PrimaryGameLayout 파생 BP를 가리킴)
	return LayoutClass.LoadSynchronous();
}

void UCuGameUIPolicy::AddLayoutToViewport(UCuLocalPlayer* LocalPlayer, UCuPrimaryGameLayout* Layout)
{
	if (!Layout)
	{
		return;
	}
	// per-player 화면에 붙인다(AddToViewport 아님 → split-screen에서 플레이어별 독립).
	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));
	Layout->AddToPlayerScreen(1000);
}

void UCuGameUIPolicy::RemoveLayoutFromViewport(UCuLocalPlayer* LocalPlayer, UCuPrimaryGameLayout* Layout)
{
	if (Layout)
	{
		Layout->RemoveFromParent();
	}
}
