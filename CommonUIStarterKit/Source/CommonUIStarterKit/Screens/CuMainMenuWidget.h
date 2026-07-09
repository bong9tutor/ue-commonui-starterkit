// CuMainMenuWidget.h — 메인 메뉴 화면 베이스.
//
// 학습 노트: Start / Settings / Quit 버튼(VerticalBox)은 C++ BindWidget 멤버가 아니라
//  WBP(WBP_MainMenu) 내부 위젯이다. 클릭 배선은 WBP 그래프에서 각 버튼의
//  OnButtonBaseClicked(CommonButtonBase의 BP 클릭 델리게이트, param Button)에
//  push/quit 로직을 연결하는 방식으로 세션 C에서 완료됐다.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuMainMenuWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuMainMenuWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
