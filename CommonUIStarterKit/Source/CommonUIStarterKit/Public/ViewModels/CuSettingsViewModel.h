// CuSettingsViewModel.h — 설정 화면용 MVVM ViewModel (Stage 6 캡스톤, 이번 세션엔 스텁).
//
// 학습 노트 (레퍼런스 §7 · §8):
//  - UMVVMViewModelBase(INotifyFieldValueChanged) 파생. FieldNotify UPROPERTY를 관찰 가능하게 노출.
//  - ⚠️ setter에서 반드시 UE_MVVM_SET_PROPERTY_VALUE를 써야 바인딩된 위젯이 갱신된다
//    (멤버 직접 대입 시 broadcast 안 됨 → 완전 무음 실패).
//  - ⚠️ 볼륨은 UGameUserSettings에 필드가 없다. 실제 적용은 Sound Class/Sound Mix로 한다(Stage 6).
#pragma once

#include "MVVMViewModelBase.h"
#include "CuSettingsViewModel.generated.h"

UCLASS(BlueprintType)
class COMMONUISTARTERKIT_API UCuSettingsViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// Getter/Setter는 UPROPERTY의 Getter/Setter 지정자가 참조한다(BP 바인딩 대상).
	float GetMasterVolume() const { return MasterVolume; }
	void SetMasterVolume(float NewValue);

private:
	// FieldNotify: 이 값이 바뀌면 바인딩된 View가 자동 갱신된다.
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = true))
	float MasterVolume = 1.0f;
};
