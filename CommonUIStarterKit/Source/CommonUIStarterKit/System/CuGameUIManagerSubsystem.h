// CuGameUIManagerSubsystem.h — player lifecycle에 반응해 UCuGameUIPolicy에 위임하는 최상위 subsystem.
//
// 학습 노트 (레퍼런스 big picture · 3-class split의 최상단):
//  - ⚠️ 이 subsystem은 레이아웃을 '직접' 생성하지 않는다. LocalPlayer 추가/제거에 반응해
//    UCuGameUIPolicy에 위임만 한다(관심사 분리).
//  - DefaultUIPolicyClass는 UPROPERTY(config)로 DefaultGame.ini에서 읽는다(세션 B Step B-2).
//  - 레이아웃 생성 트리거: GameMode PostLogin(=PC 로그인 시점, PC 확보 보장) + LocalPlayer 제거 이벤트.
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
#include "CuGameUIManagerSubsystem.generated.h"

class UCuGameUIPolicy;
class UCuLocalPlayer;
class AGameModeBase;
class APlayerController;
class ULocalPlayer;

UCLASS(config = Game)
class COMMONUISTARTERKIT_API UCuGameUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//~ USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem

	const UCuGameUIPolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }
	UCuGameUIPolicy* GetCurrentUIPolicy() { return CurrentPolicy; }

protected:
	void SwitchToPolicy(UCuGameUIPolicy* InPolicy);

private:
	// GameMode에 PC가 로그인하면 그 PC의 LocalPlayer로 policy에 알린다.
	void HandleGameModePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
	// LocalPlayer 제거 시 policy에 알려 레이아웃을 화면에서 내린다.
	void HandleLocalPlayerRemoved(ULocalPlayer* LocalPlayer);

	UPROPERTY(Transient)
	TObjectPtr<UCuGameUIPolicy> CurrentPolicy = nullptr;

	// 세션 C에서 만든 정책 BP를 DefaultGame.ini로 지정한다.
	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<UCuGameUIPolicy> DefaultUIPolicyClass;

	FDelegateHandle GameModePostLoginHandle;

	// OnLocalPlayerRemovedEvent(UGameInstance 멤버) 구독 핸들.
	// AddUObject는 weak 참조라 dangling 크래시로 이어지진 않지만, Deinitialize에서 짝을 맞춰
	// Remove하지 않으면 subsystem이 GameInstance보다 먼저 재생성/교체되는 확장 시나리오에서
	// 유령 바인딩이 남는다. GameModePostLoginHandle과 동일한 대칭을 유지한다.
	FDelegateHandle LocalPlayerRemovedHandle;
};
