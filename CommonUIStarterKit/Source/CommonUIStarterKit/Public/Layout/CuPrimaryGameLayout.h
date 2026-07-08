// CuPrimaryGameLayout.h — GameplayTag별 UCommonActivatableWidgetStack 레이어를 소유하는 루트 레이아웃.
//
// 학습 노트 (레퍼런스 §6):
//  - BindWidget으로 스택을 잡되, tag→container 맵은 BindWidget만으로는 채워지지 않는다.
//    → NativeOnInitialized에서 각 스택을 RegisterLayer로 '명시적으로' 등록한다.
//  - push는 PushWidgetToLayerStack(동기, hard class) / PushWidgetToLayerStackAsync(비동기, soft class).
//    push가 위젯을 자동 activate하므로, 이후 ActivateWidget을 다시 호출하지 말 것(이중 activation).
//  - 정적 접근자 GetPrimaryGameLayout(PlayerController*)로 어디서든 이 플레이어의 레이아웃을 얻는다.
#pragma once

#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "CuPrimaryGameLayout.generated.h"

class UCommonActivatableWidget;
class ULocalPlayer;
class APlayerController;

/** 비동기 push 진행 상태 (StateFunc 콜백에 전달). */
UENUM(BlueprintType)
enum class ECuAsyncWidgetLayerState : uint8
{
	Canceled,     // 로딩 실패/취소
	Initialize,   // 위젯 인스턴스 생성 직후(스택에 추가되기 전 초기화 훅)
	AfterPush     // 스택에 push 완료
};

UCLASS(Abstract, meta = (DisableNativeTick))
class COMMONUISTARTERKIT_API UCuPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/** PlayerController → 이 플레이어의 루트 레이아웃. (subsystem→policy→layout 체인으로 해석) */
	static UCuPrimaryGameLayout* GetPrimaryGameLayout(APlayerController* PlayerController);
	static UCuPrimaryGameLayout* GetPrimaryGameLayout(ULocalPlayer* LocalPlayer);

	/** tag→container 맵에 스택을 명시적으로 등록. NativeOnInitialized에서 호출. */
	void RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	/** 등록된 레이어(스택) 컨테이너를 tag로 조회. 없으면 nullptr. */
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerTag) const;

	/** [동기] hard class 위젯을 레이어 스택에 push (자동 activate). */
	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerTag, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	/** [동기] push + 생성 직후 초기화 콜백. */
	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		// (파생 검사는 컨테이너의 AddWidget<T>가 자체 static_assert로 수행한다.)
		if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerTag))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}
		return nullptr;
	}

	/**
	 * [비동기] soft class를 async load 후 push. streaming handle을 반환.
	 * 실제 Lyra가 메뉴/모달을 여는 방식이라 학습용으로 포함한다.
	 * ⚠️ 학습 단순화: Lyra는 로딩 동안 입력을 suspend(UCommonUIActionRouterBase)하지만,
	 *    이 kit은 그 부분을 생략하고 load→push 흐름만 구현한다. (bSuspendInputUntilComplete는 향후 확장 지점)
	 */
	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerTag, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(ECuAsyncWidgetLayerState, ActivatableWidgetT*)> StateFunc = [](ECuAsyncWidgetLayerState, ActivatableWidgetT*) {})
	{
		(void)bSuspendInputUntilComplete; // 향후 입력 suspend 훅 자리

		TSharedPtr<FStreamableHandle> StreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
			ActivatableWidgetClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(this, [this, LayerTag, ActivatableWidgetClass, StateFunc]()
			{
				TSubclassOf<UCommonActivatableWidget> LoadedClass = ActivatableWidgetClass.Get();
				ActivatableWidgetT* Widget = PushWidgetToLayerStack<ActivatableWidgetT>(LayerTag, LoadedClass,
					[&StateFunc](ActivatableWidgetT& WidgetInstance) { StateFunc(ECuAsyncWidgetLayerState::Initialize, &WidgetInstance); });
				StateFunc(ECuAsyncWidgetLayerState::AfterPush, Widget);
			}));

		if (!StreamingHandle.IsValid())
		{
			StateFunc(ECuAsyncWidgetLayerState::Canceled, nullptr);
		}
		return StreamingHandle;
	}

	/** 어느 레이어에 있든 해당 위젯을 찾아 제거(pop). */
	void FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget);

protected:
	virtual void NativeOnInitialized() override;

	// --- 4개 레이어 스택 (UI.Layer.Game/GameMenu/Menu/Modal) ---
	// ⚠️ BindWidget 이름은 WBP 자식 위젯 이름과 '정확히' 일치해야 한다(§6).
	//    Stage 2는 Menu 하나만, Stage 5에서 4개로 확장하므로 BindWidgetOptional로 둔다.
	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> Layer_Game;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> Layer_GameMenu;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> Layer_Menu;

	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UCommonActivatableWidgetStack> Layer_Modal;

private:
	// tag → 등록된 스택 컨테이너.
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};
