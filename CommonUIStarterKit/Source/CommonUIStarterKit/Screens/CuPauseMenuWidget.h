// CuPauseMenuWidget.h — 일시정지 메뉴 베이스 (GameMenu/Menu 레이어).
//
// 학습 노트: 파생 WBP_PauseMenu의 그래프가 Resume → DeactivateWidget(self, = pop),
//  Quit → ConfirmationModal을 Modal 레이어에 push 한다. bIsBackHandler로 Back 입력을 받는다.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuPauseMenuWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuPauseMenuWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
