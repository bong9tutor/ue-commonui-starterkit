// CuActivatableWidget.h — 모든 화면의 베이스 Activatable Widget.
//
// 학습 노트 (레퍼런스 §5):
//  - 입력 모드는 GetDesiredInputConfig() override로 지정한다. FUIInputConfig의 모드는
//    ECommonInputMode::Menu / Game / All 뿐이다. ⚠️ GameAndMenu는 없다(=All).
//  - ⚠️ APlayerController::SetInputMode*를 직접 호출하지 말 것 — UCommonUIActionRouterBase가
//    입력을 관리하므로 라우터를 깨뜨린다. 반드시 GetDesiredInputConfig() 경로를 쓴다.
//  - 포커스는 NativeGetDesiredFocusTarget() override로 지정하고, bAutoRestoreFocus로 스택 복귀 시 복원.
#pragma once

#include "CommonActivatableWidget.h"
#include "CommonInputModeTypes.h"
#include "CuActivatableWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	//~ UCommonActivatableWidget: 이 화면이 활성일 때 원하는 입력 설정을 반환.
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End UCommonActivatableWidget

protected:
	// 이 화면의 입력 모드. 대부분의 메뉴는 Menu. (게임 위 HUD 오버레이는 Game/All을 쓸 수 있음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cu|Input")
	ECommonInputMode InputMode = ECommonInputMode::Menu;
};
