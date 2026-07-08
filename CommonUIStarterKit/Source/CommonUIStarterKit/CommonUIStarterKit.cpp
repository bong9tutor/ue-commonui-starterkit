// CommonUIStarterKit.cpp — primary game module 구현.
// 학습 노트:
//  - IMPLEMENT_PRIMARY_GAME_MODULE(모듈클래스, "모듈명", "게임명") 매크로가
//    이 module을 프로젝트의 primary game module로 등록한다(프로젝트당 정확히 1개).
//  - StartupModule/ShutdownModule은 module 로드/언로드 시점 훅. 지금은 비어 있다.
#include "CommonUIStarterKit.h"

void FCommonUIStarterKitModule::StartupModule()
{
	// 세션 A: 의도적으로 비어 있음 (플러그인 컴파일 게이트용 최소 module).
}

void FCommonUIStarterKitModule::ShutdownModule()
{
	// 세션 A: 의도적으로 비어 있음.
}

IMPLEMENT_PRIMARY_GAME_MODULE(FCommonUIStarterKitModule, CommonUIStarterKit, "CommonUIStarterKit");
