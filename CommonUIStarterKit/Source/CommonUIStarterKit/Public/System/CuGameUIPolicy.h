// CuGameUIPolicy.h — LayoutClass를 소유하고 UCuLocalPlayer마다 레이아웃을 1개씩 생성.
//
// 학습 노트 (레퍼런스 big picture):
//  - UCuGameUIManagerSubsystem이 이 policy를 소유(Within=CuGameUIManagerSubsystem)하고,
//    policy가 per-LocalPlayer로 UCuPrimaryGameLayout을 생성해 그 플레이어 화면(AddToPlayerScreen)에 붙인다.
//  - per-player 목록(RootViewportLayouts)을 추적해 split-screen에서도 각기 독립 레이아웃이 되게 한다.
//  - LayoutClass(TSoftClassPtr)는 세션 C에서 만든 WBP_PrimaryGameLayout 파생 BP를 가리킨다.
#pragma once

#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "CuGameUIPolicy.generated.h"

class UCuGameUIManagerSubsystem;
class UCuLocalPlayer;
class UCuPrimaryGameLayout;

/** LocalPlayer ↔ 그 플레이어의 루트 레이아웃 매핑 1건. */
USTRUCT()
struct FCuRootViewportLayoutInfo
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UCuLocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCuPrimaryGameLayout> RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;

	FCuRootViewportLayoutInfo() = default;
	FCuRootViewportLayoutInfo(UCuLocalPlayer* InLocalPlayer, UCuPrimaryGameLayout* InRootLayout, bool bIsInViewport)
		: LocalPlayer(InLocalPlayer), RootLayout(InRootLayout), bAddedToViewport(bIsInViewport)
	{
	}

	// TArray::FindByKey(LocalPlayer) 지원.
	bool operator==(const UCuLocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }
};

UCLASS(Abstract, Blueprintable, Within = CuGameUIManagerSubsystem)
class COMMONUISTARTERKIT_API UCuGameUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	/** WorldContext에서 현재 활성 policy를 얻는다. */
	static UCuGameUIPolicy* GetGameUIPolicy(const UObject* WorldContextObject);

	//~ UObject: policy는 UObject라 GetWorld를 직접 제공해야 BP/타이머 등이 동작한다.
	virtual UWorld* GetWorld() const override;
	//~ End UObject

	UCuGameUIManagerSubsystem* GetOwningUIManager() const;

	/** 이 플레이어의 루트 레이아웃(없으면 nullptr). */
	UCuPrimaryGameLayout* GetRootLayout(const UCuLocalPlayer* LocalPlayer) const;

	// --- player lifecycle (subsystem이 호출) ---
	void NotifyPlayerAdded(UCuLocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(UCuLocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(UCuLocalPlayer* LocalPlayer);

protected:
	void AddLayoutToViewport(UCuLocalPlayer* LocalPlayer, UCuPrimaryGameLayout* Layout);
	void RemoveLayoutFromViewport(UCuLocalPlayer* LocalPlayer, UCuPrimaryGameLayout* Layout);

	/** LayoutClass를 로드해 이 플레이어용 레이아웃 위젯을 생성하고 화면에 붙인다. */
	void CreateLayoutWidget(UCuLocalPlayer* LocalPlayer);
	TSubclassOf<UCuPrimaryGameLayout> GetLayoutWidgetClass(UCuLocalPlayer* LocalPlayer);

	// 세션 C에서 만든 WBP_PrimaryGameLayout 파생 BP 경로. (soft class → 필요 시 로드)
	UPROPERTY(EditAnywhere, Category = "UI Policy")
	TSoftClassPtr<UCuPrimaryGameLayout> LayoutClass;

	UPROPERTY(Transient)
	TArray<FCuRootViewportLayoutInfo> RootViewportLayouts;
};
