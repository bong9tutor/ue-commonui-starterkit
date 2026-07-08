// CuGameplayTags.h — UI 레이어 식별용 네이티브 GameplayTag 선언.
//
// 학습 노트 (레퍼런스 §2):
//  - 네이티브 태그는 헤더의 UE_DECLARE_GAMEPLAY_TAG_EXTERN + cpp의 UE_DEFINE_GAMEPLAY_TAG 쌍으로 선언한다.
//  - 4개 레이어: UI.Layer.Game / GameMenu / Menu / Modal — UCuPrimaryGameLayout이 tag→stack 맵의 key로 사용.
#pragma once

#include "NativeGameplayTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Game);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_GameMenu);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Menu);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_Layer_Modal);
