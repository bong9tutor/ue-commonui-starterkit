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

namespace
{
	// 게임플레이 HUD보다 위, 다른 플레이어 화면 위젯보다 위에 뜨도록 넉넉히 높은 Z-Order.
	// (익명 namespace 안이라 이미 internal linkage. static은 중복이므로 붙이지 않는다.)
	constexpr int32 CuLayoutZOrder = 1000;
}

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
	//  - NotifyPlayerAdded는 HandleGameModePostLogin(재로그인/seamless travel 포함)마다
	//    다시 호출될 수 있다. RemoveAll(this) 없이 AddWeakLambda만 하면 같은 람다가
	//    누적 구독되어, 다음 PC 세팅 시 N번 중복 발화 → 아래 CreateLayoutWidget도 N번
	//    호출되는 잠복 버그가 있었다. 여기서 먼저 이전 구독을 정리해 항상 1개만 유지한다.
	//    (이 델리게이트의 구독자는 policy(this) 하나뿐이라 RemoveAll(this)가 다른 리스너를
	//    건드리지 않는다 — RemoveAll은 UserObject==this로 바인딩된 것만 걸러 제거한다.)
	LocalPlayer->OnPlayerControllerSet.RemoveAll(this);
	LocalPlayer->OnPlayerControllerSet.AddWeakLambda(this, [this](UCuLocalPlayer* InLocalPlayer, APlayerController* /*PlayerController*/)
	{
		// 주의: NotifyPlayerRemoved가 아니라 NotifyPlayerDestroyed를 불러야 한다.
		//  - NotifyPlayerRemoved: 뷰포트에서 위젯만 내리고(bAddedToViewport=false) RootViewportLayouts의
		//    엔트리는 그대로 남긴다(재부착을 대비한 설계).
		//  - NotifyPlayerDestroyed: 위 NotifyPlayerRemoved를 호출한 뒤 배열 엔트리 자체를 제거한다.
		//    PC가 교체되어 레이아웃을 통째로 새로 만드는 이 경로에서 NotifyPlayerRemoved만 쓰면,
		//    뒤따르는 CreateLayoutWidget의 무조건 Emplace가 같은 LocalPlayer 키로 새 엔트리를 추가해
		//    RootViewportLayouts에 stale 엔트리가 중복 누적된다. GetRootLayout은 FindByKey(첫 매치)라
		//    이미 뷰포트에서 내려간 옛 레이아웃을 반환하고, 새 레이아웃은 조회 불가 + 사실상 누수가 된다.
		//    NotifyPlayerDestroyed로 옛 엔트리를 먼저 지워야 새 Emplace 이후 엔트리가 정확히 1개로 유지된다.
		//  - 더 근본적인 해법은 CreateLayoutWidget 자체를 find-or-add(upsert)로 바꾸는 것이지만,
		//    이번 수정은 기존 함수 시맨틱을 건드리지 않는 최소 변경을 택했다.
		NotifyPlayerDestroyed(InLocalPlayer);
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

// LocalPlayer는 현재 미사용(향후 per-player 다른 LayoutClass override 여지를 위해 시그니처 유지).
TSubclassOf<UCuPrimaryGameLayout> UCuGameUIPolicy::GetLayoutWidgetClass(UCuLocalPlayer* /*LocalPlayer*/)
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
	Layout->AddToPlayerScreen(CuLayoutZOrder);
}

// LocalPlayer는 현재 미사용(향후 per-player 제거 로직 override 여지를 위해 시그니처 유지).
void UCuGameUIPolicy::RemoveLayoutFromViewport(UCuLocalPlayer* /*LocalPlayer*/, UCuPrimaryGameLayout* Layout)
{
	if (Layout)
	{
		Layout->RemoveFromParent();
	}
}
