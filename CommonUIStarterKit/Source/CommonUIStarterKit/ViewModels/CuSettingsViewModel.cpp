// CuSettingsViewModel.cpp
#include "ViewModels/CuSettingsViewModel.h"

void UCuSettingsViewModel::SetMasterVolume(float NewValue)
{
	// ⚠️ load-bearing 매크로: 대입 + (변경 시에만) FieldNotify broadcast.
	//    이 매크로를 빼고 MasterVolume = NewValue로 직접 대입하면 바인딩이 무음으로 안 갱신된다.
	UE_MVVM_SET_PROPERTY_VALUE(MasterVolume, NewValue);

	// TODO(세션 C Stage 6): 실제 볼륨 적용은 Sound Class/Sound Mix(SetSoundMixClassOverride)로.
	//   UGameUserSettings에는 master volume 필드가 없다(레퍼런스 §8).
}
