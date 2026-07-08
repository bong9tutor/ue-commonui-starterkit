// CuTitleScreenWidget.h — 타이틀 화면 베이스.
//
// 학습 노트: "Press Any Key" → 메인 메뉴를 push 하는 화면(세션 C Stage 3).
//  로직(입력 감지 → PushWidgetToLayerStack)은 세션 C에서 이 베이스에 추가한다.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuTitleScreenWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuTitleScreenWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
