// CommonUIStarterKit.h — primary game module 헤더.
// 학습 노트:
//  - UE 게임 module은 최소한 IMPLEMENT_PRIMARY_GAME_MODULE 하나면 동작한다.
//  - 여기서는 학습을 위해 커스텀 module 클래스를 두고 StartupModule/ShutdownModule을 오버라이드한다
//    (지금은 비어 있음 — 세션 B/C에서 GameplayTag 등록·초기화 훅으로 확장 가능).
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCommonUIStarterKitModule : public FDefaultGameModuleImpl
{
public:
	//~ IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface
};
