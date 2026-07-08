// CommonUIStarterKit.h — primary game module 헤더.
// 학습 노트:
//  - UE 게임 module은 최소한 IMPLEMENT_PRIMARY_GAME_MODULE 하나면 동작한다.
//  - 여기서는 학습을 위해 커스텀 module 클래스를 두고 StartupModule/ShutdownModule을 오버라이드한다.
//  - module 클래스는 모듈명 규칙을 따르므로 Cu 프리픽스 예외(FCommonUIStarterKitModule).
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
