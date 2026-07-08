// CuLocalPlayer.h — per-player UI 아이덴티티.
//
// 학습 노트 (Lyra CommonGame 재구현):
//  - UCuGameUIPolicy가 per-LocalPlayer로 레이아웃을 1개씩 생성할 때, 이 클래스 인스턴스를 맵의 key로 삼는다.
//  - split-screen에서는 LocalPlayer가 여러 개가 되고 각기 독립 레이아웃을 갖는데, 그 분기 지점이 이 아이덴티티다.
//  - DefaultEngine.ini의 [/Script/Engine.Engine] LocalPlayerClassName으로 지정한다(세션 B Step B-2).
//  - 이름을 Cu 프리픽스로 두어 CommonGame 실제 클래스 UCommonLocalPlayer와의 혼동을 없앤다.
#pragma once

#include "Engine/LocalPlayer.h"
#include "CuLocalPlayer.generated.h"

class APlayerController;

UCLASS()
class COMMONUISTARTERKIT_API UCuLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	/**
	 * PlayerController가 이 LocalPlayer에 연결되었을 때 브로드캐스트.
	 * UCuGameUIPolicy가 여기에 바인딩해 "PC가 준비된 뒤" 레이아웃을 생성/재생성할 수 있다.
	 * (세션 C에서 PIE 타이밍을 검증하며, 필요 시 GameMode/PlayerController가 이 델리게이트를 구동한다.)
	 */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UCuLocalPlayer* /*LocalPlayer*/, APlayerController* /*PlayerController*/);
	FPlayerControllerSetDelegate OnPlayerControllerSet;
};
