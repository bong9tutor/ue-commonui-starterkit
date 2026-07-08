// CuConfirmationModalWidget.h — 확인 모달 베이스 (Modal 레이어, 세션 C Stage 5).
//
// 학습 노트:
//  - 제목/본문 + Confirm/Cancel. Modal 레이어의 스택은 하위 레이어 입력을 차단한다.
//  - 모달 종료 시 bAutoRestoreFocus(부모 UCuActivatableWidget/UCommonActivatableWidget)로
//    이전 위젯 포커스가 복원된다.
//  - Confirm/Cancel 결과는 델리게이트로 알린다(호출부가 바인딩).
#pragma once

#include "Widgets/CuActivatableWidget.h"
#include "CuConfirmationModalWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCuConfirmationModalConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCuConfirmationModalCancelled);

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuConfirmationModalWidget : public UCuActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Cu|Modal")
	FOnCuConfirmationModalConfirmed OnConfirmed;

	UPROPERTY(BlueprintAssignable, Category = "Cu|Modal")
	FOnCuConfirmationModalCancelled OnCancelled;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cu|Modal")
	FText TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cu|Modal")
	FText MessageText;
};
