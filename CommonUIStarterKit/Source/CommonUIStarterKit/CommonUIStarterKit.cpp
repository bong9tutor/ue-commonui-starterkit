// CommonUIStarterKit.cpp — primary game module 구현.
#include "CommonUIStarterKit.h"

void FCommonUIStarterKitModule::StartupModule()
{
	// 이 module은 별도의 module-level 초기화가 필요 없다(레이어링·서브시스템·config 로딩만으로 충분).
	// 의도적으로 비어 있음.
}

void FCommonUIStarterKitModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE(FCommonUIStarterKitModule, CommonUIStarterKit, "CommonUIStarterKit");
