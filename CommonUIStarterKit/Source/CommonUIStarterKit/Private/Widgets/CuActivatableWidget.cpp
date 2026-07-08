// CuActivatableWidget.cpp
#include "Widgets/CuActivatableWidget.h"

#include "Engine/EngineBaseTypes.h" // EMouseCaptureMode

TOptional<FUIInputConfig> UCuActivatableWidget::GetDesiredInputConfig() const
{
	// 메뉴 UI는 마우스 캡처 없이(NoCapture) 커서를 자유롭게 둔다.
	// ⚠️ SetInputMode*를 직접 부르지 말고 이 config 반환으로만 입력 모드를 지정한다(레퍼런스 §5).
	return FUIInputConfig(InputMode, EMouseCaptureMode::NoCapture);
}
