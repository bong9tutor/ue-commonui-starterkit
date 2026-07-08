// CuButtonBase.h — 텍스트 프로퍼티 + 스타일 적용을 갖춘 베이스 버튼.
//
// 학습 노트:
//  - UCommonButtonBase는 텍스트를 기본 제공하지 않으므로(스타일/상태만 관리), 텍스트 블록은 여기서 얹는다.
//  - Text_Label은 BindWidgetOptional — 세션 C에서 WBP_ButtonBase에 CommonTextBlock을 배치하고
//    이름을 'Text_Label'로 정확히 맞추면 자동 바인딩된다(§6 BindWidget 이름 일치).
//  - 스타일(CommonButtonStyle/CommonTextStyle)은 세션 C Stage 3에서 에셋으로 연결한다.
#pragma once

#include "CommonButtonBase.h"
#include "CuButtonBase.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, Blueprintable)
class COMMONUISTARTERKIT_API UCuButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	/** 버튼 라벨 텍스트를 설정(즉시 갱신). */
	UFUNCTION(BlueprintCallable, Category = "Cu|Button")
	void SetButtonText(FText InText);

protected:
	//~ UUserWidget / UCommonButtonBase
	virtual void NativePreConstruct() override;
	virtual void NativeOnCurrentTextStyleChanged() override;
	//~ End

	void RefreshButtonText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cu|Button")
	FText ButtonText;

	// WBP에 CommonTextBlock을 두고 이름을 'Text_Label'로 맞추면 바인딩됨(선택적).
	UPROPERTY(Transient, meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> Text_Label;
};
