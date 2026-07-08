// CuSettingsScreenWidget.h — 설정 화면 베이스 (Stage 6 MVVM 캡스톤에서 사용).
//
// 학습 노트: UCommonTabListWidgetBase 탭 + 옵션 항목 + UCuSettingsViewModel 바인딩(세션 C Stage 6).
//  ⚠️ View Binding은 WBP의 View Bindings 패널에 저장되므로 수동/Experimental fallback 대상.
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuSettingsScreenWidget.generated.h"

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuSettingsScreenWidget : public UCuActivatableWidget
{
	GENERATED_BODY()
};
