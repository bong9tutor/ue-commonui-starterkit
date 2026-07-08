// CuPauseMenuWidget.h — 일시정지 메뉴 베이스 (Menu 레이어, 세션 C Stage 5).
//
// 학습 노트: Resume / Settings / Quit to Title. Quit 선택 시 ConfirmationModal을 Modal 레이어에 push.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuPauseMenuWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuPauseMenuWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
