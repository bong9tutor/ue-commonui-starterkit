// CuTitleScreenWidget.h — 타이틀 화면 베이스.
//
// 학습 노트: Btn_Start("Press to Start") → 메인 메뉴를 Menu 레이어에 push 하는 화면.
//  ⚠️ 내비게이션 로직은 이 C++ 베이스가 아니라 파생 WBP_TitleScreen의 그래프에 있다:
//     OnButtonBaseClicked → GetOwningPlayer → GetPrimaryGameLayoutForPlayer → PushWidgetToLayer.
//  이 클래스는 BP가 파생할 네이티브 베이스(= 타입 anchor) 역할만 한다.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuTitleScreenWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuTitleScreenWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
