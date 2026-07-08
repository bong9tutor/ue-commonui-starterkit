// CuMainMenuWidget.h — 메인 메뉴 화면 베이스.
//
// 학습 노트: Start / Settings / Quit 버튼(세션 C Stage 3, VerticalBox).
//  버튼은 WBP에서 BindWidget으로 잡고, OnClicked → push/quit 로직을 세션 C에서 연결한다.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuMainMenuWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuMainMenuWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
