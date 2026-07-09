// CuGameUIManagerSubsystem.cpp
#include "System/CuGameUIManagerSubsystem.h"

#include "System/CuLocalPlayer.h"
#include "System/CuGameUIPolicy.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"

bool UCuGameUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// dedicated server에는 로컬 UI가 없으므로 생성하지 않는다(이 kit은 client 학습용).
	return Outer && !CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance();
}

void UCuGameUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// config(DefaultGame.ini)로 지정된 정책 클래스를 로드해 인스턴스화.
	if (!CurrentPolicy && !DefaultUIPolicyClass.IsNull())
	{
		if (TSubclassOf<UCuGameUIPolicy> PolicyClass = DefaultUIPolicyClass.LoadSynchronous())
		{
			SwitchToPolicy(NewObject<UCuGameUIPolicy>(this, PolicyClass));
		}
	}

	// PC 로그인/LocalPlayer 제거를 트리거로 policy에 위임.
	GameModePostLoginHandle = FGameModeEvents::OnGameModePostLoginEvent().AddUObject(this, &UCuGameUIManagerSubsystem::HandleGameModePostLogin);
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		LocalPlayerRemovedHandle = GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &UCuGameUIManagerSubsystem::HandleLocalPlayerRemoved);
	}
}

void UCuGameUIManagerSubsystem::Deinitialize()
{
	if (GameModePostLoginHandle.IsValid())
	{
		FGameModeEvents::OnGameModePostLoginEvent().Remove(GameModePostLoginHandle);
		GameModePostLoginHandle.Reset();
	}

	// OnLocalPlayerRemovedEvent는 UGameInstance의 멤버라 GameInstance가 이미 파괴된
	// 시점(Deinitialize 순서에 따라)일 수 있으므로 null 가드 후 Remove한다.
	if (LocalPlayerRemovedHandle.IsValid())
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			GameInstance->OnLocalPlayerRemovedEvent.Remove(LocalPlayerRemovedHandle);
		}
		LocalPlayerRemovedHandle.Reset();
	}

	Super::Deinitialize();
}

void UCuGameUIManagerSubsystem::SwitchToPolicy(UCuGameUIPolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}

void UCuGameUIManagerSubsystem::HandleGameModePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	if (!CurrentPolicy || !NewPlayer)
	{
		return;
	}
	// FGameModeEvents는 전역이므로 '내 GameInstance의 world'로 필터링.
	const UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance && NewPlayer->GetWorld() == GameInstance->GetWorld())
	{
		if (UCuLocalPlayer* LocalPlayer = Cast<UCuLocalPlayer>(NewPlayer->GetLocalPlayer()))
		{
			CurrentPolicy->NotifyPlayerAdded(LocalPlayer);
		}
	}
}

void UCuGameUIManagerSubsystem::HandleLocalPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (CurrentPolicy)
	{
		if (UCuLocalPlayer* CommonLocalPlayer = Cast<UCuLocalPlayer>(LocalPlayer))
		{
			CurrentPolicy->NotifyPlayerDestroyed(CommonLocalPlayer);
		}
	}
}
