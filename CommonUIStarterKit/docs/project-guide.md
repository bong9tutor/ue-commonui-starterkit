# CommonUIStarterKit — 프로젝트 가이드

**UE 프로젝트(`CommonUIStarterKit/`)의 코드·Config·빌드를 건드리기 전에 이 문서를 읽는다.** 이 문서는 자동으로 로드되지 않는다.

여기에는 **앞으로도 지켜야 할 사실**만 있다. 지나간 진행 기록은 [`session-log.md`](session-log.md)에 있다.

| 문서 | 담당 |
|---|---|
| [`../../docs/common-ui-coding-reference.md`](../../docs/common-ui-coding-reference.md) | **규칙 정본.** Common UI/MVVM/Input의 API idiom·함정·네이밍 (§1~§10) |
| [`../../CLAUDE.md`](../../CLAUDE.md) | 저장소 가이드. containment 구조, 권한 정책, 세션 재시작 제약 |
| 이 문서 | 머신 환경, 코드 구조, 확정 결정, 함정 |
| [`session-log.md`](session-log.md) | 세션 A/B/C 진행 기록 |

## 프로젝트 개요

Common UI 학습용 Starter Kit. Lyra **CommonGame 패턴을 학습용으로 재구현**한다(코드 복사가 아니다 — CommonGame은 engine library가 아니라 Lyra 샘플 플러그인이므로). 진짜 엔진 라이브러리(CommonUI / CommonInput / EnhancedInput / UMG / ModelViewViewModel)는 그대로 재사용한다.

3-세션 워크플로(A 프로젝트+플러그인 / B C++ 뼈대 / C 에셋 Stage 1~6)로 만들었고, Stage 0~6까지 완료된 상태다.

## 머신 환경 quirk (반드시 지킬 것)

**UE 5.8이 비표준 경로 `C:\UE\UE_5.8`에 설치돼 있고, HKLM 레지스트리에 5.8이 등록돼 있지 않다**(5.5·5.7만 등록). 따라서 `.uproject`의 `EngineAssociation "5.8"`이 버전 문자열로 자동 해석되지 않는다.

**대응**: 엔진 도구는 **항상 절대경로로 직접 호출**한다. 더블클릭이나 우클릭 메뉴에 의존하지 말 것.

```powershell
# 빌드
& "C:\UE\UE_5.8\Engine\Build\BatchFiles\Build.bat" CommonUIStarterKitEditor Win64 Development `
  -project="D:\Projects\Bong9\ClaudeCode\ue-commonui-starterkit\CommonUIStarterKit\CommonUIStarterKit.uproject" -waitmutex

# 에디터 실행
& "C:\UE\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" `
  "D:\Projects\Bong9\ClaudeCode\ue-commonui-starterkit\CommonUIStarterKit\CommonUIStarterKit.uproject"
```

- **에디터가 실행 중이면 DLL이 잠겨 빌드가 실패한다.** 재빌드는 `에디터 종료 → Build → 재실행` 순서. 단 **에디터를 끄면 Monolith MCP 서버(port 9316)도 함께 내려간다.**
- **Live Coding으로는 새 `UFUNCTION`을 추가할 수 없다**(UHT 재생성 필요). 전체 재빌드 + 에디터 재시작이 필요하다.
- **`.mcp.json`은 저장소 루트에 있다.** Claude Code가 워크스페이스 루트의 것만 로드하기 때문이다. `command`에는 containment 접두어가 붙는다: `CommonUIStarterKit/Plugins/Monolith/Binaries/monolith_proxy.exe`.

## 코드 구조 (확정)

```
CommonUIStarterKit/
├─ CommonUIStarterKit.uproject   # EngineAssociation 5.8 / plugin: CommonUI · ModelViewViewModel · Monolith
├─ Source/
│  ├─ CommonUIStarterKit.Target.cs        # Game 타깃
│  ├─ CommonUIStarterKitEditor.Target.cs  # Editor 타깃 (빌드·에디터 실행은 항상 이쪽)
│  └─ CommonUIStarterKit/
│     ├─ CommonUIStarterKit.Build.cs      # dep 목록 + PublicIncludePaths.Add(ModuleDirectory)
│     ├─ CommonUIStarterKit.h / .cpp      # primary game module
│     ├─ System/    CuLocalPlayer · CuGameUIManagerSubsystem · CuGameUIPolicy
│     ├─ Layout/    CuGameplayTags · CuPrimaryGameLayout
│     ├─ Widgets/   CuActivatableWidget · CuButtonBase
│     ├─ Screens/   CuTitleScreen / CuMainMenu / CuSettingsScreen / CuPauseMenu / CuConfirmationModal (header-only)
│     └─ ViewModels/ CuSettingsViewModel
├─ Config/     DefaultEngine.ini · DefaultGame.ini · DefaultInput.ini
├─ Content/    Core · Input · Maps · UI/{Foundation,Menu,Style}
└─ Plugins/Monolith/   v0.20.3 UE5.8 (gitignore됨, 재설치 대상)
```

**네이밍·구조 규약** (레퍼런스 §2·§3):

- 프로젝트 C++ 클래스는 **`Cu` 프리픽스**(`UCu*`/`FCu*`/`ECu*`).
- 소스는 **`Public`/`Private`를 쓰지 않고** 역할별 하위폴더에 `.h`/`.cpp`를 함께 둔다. 이건 의도된 구조다.
- ⚠️ 그래서 **Build.cs에 `PublicIncludePaths.Add(ModuleDirectory);`가 필수**다. 빠지면 하위폴더 헤더의 루트 기준 include(`#include "System/CuGameUIPolicy.h"`)가 해석되지 않아 `fatal error C1083`으로 빌드가 죽는다.

## 확정 결정

**플러그인 배치** — `.uproject`에는 CommonUI / ModelViewViewModel / Monolith 셋만 넣는다. `CommonInput`은 별도 plugin이 아니라 CommonUI plugin **내부의 module**이고, `EnhancedInput`은 엔진 기본 활성화 plugin이다. 둘 다 **Build.cs dep으로만** 참조한다.

**치명적 INI 2줄** — `DefaultEngine.ini`의 `[/Script/Engine.Engine]` 아래 **두 줄 모두** 필수다. 하나라도 빠지면 UI가 에러 없이 **무음으로 죽는다**.

```ini
[/Script/Engine.Engine]
GameViewportClientClassName=/Script/CommonUI.CommonGameViewportClient
LocalPlayerClassName=/Script/CommonUIStarterKit.CuLocalPlayer
```

- 앞줄이 없으면 `UCommonUIActionRouterBase`가 입력을 못 받아 gamepad 내비게이션·focus·Back이 전부 죽는다(마우스만 살아 있어 버그가 은폐된다). CommonUI 최다 셋업 실패 원인.
- 뒷줄이 없으면 엔진이 평범한 `ULocalPlayer`를 만든다. `UCuGameUIPolicy::NotifyPlayerAdded(UCuLocalPlayer*)`가 `UCuLocalPlayer`로 하드 타이핑돼 있어 **레이아웃 위젯이 아예 생성되지 않는다**. 앞줄 누락과 같은 등급이다.

**레이아웃 생성 트리거** (재구현 선택) — subsystem이 `FGameModeEvents::OnGameModePostLoginEvent`(PC 로그인 = PC 확보 보장) + `UGameInstance::OnLocalPlayerRemovedEvent`에 바인딩해 policy에 위임한다. policy는 `UCuLocalPlayer::OnPlayerControllerSet`에도 구독한다(PC 교체·지연 대응).

**BP에 노출된 push 헬퍼** — `UCuPrimaryGameLayout::PushWidgetToLayerStack`은 **template(비-UFUNCTION)**이라 BP에 노출되지 않고, 네이티브 GameplayTag도 BP에 직접 보이지 않는다. `UCommonActivatableWidgetStack`에도 BP 노출 add/push 메서드가 없다. 그래서 BP 저작용으로 다음을 C++에 추가했다(Lyra의 `UCommonUIExtensions::PushContentToLayer`에 대응).

- `UFUNCTION(BlueprintCallable) PushWidgetToLayer(FGameplayTag, TSubclassOf<UCommonActivatableWidget>)` · `RemoveWidgetFromLayer`
- `UFUNCTION(BlueprintPure) GetLayerTag_Game/GameMenu/Menu/Modal`
- `static UFUNCTION GetPrimaryGameLayoutForPlayer(APlayerController*)` — 화면 위젯의 진입점

⚠️ **BP 그래프가 이 `UFUNCTION`들을 문자열로 참조한다. 이름이나 시그니처를 바꾸면 에셋이 조용히 깨진다.**

**MVVM View Binding은 수동 fallback** — Monolith에 뷰모델 바인딩 저작 액션이 없다(probe 확정). `UCuSettingsViewModel`(C++)은 준비돼 있고, View Bindings 에디터에서 수동 연결한다. C++만으로 대체하려 들지 말 것.

## 함정 빠른참조

레퍼런스 §10에 더해, 이 프로젝트에서 실제로 밟은 것들.

**Config 로드 타이밍** — 다음 세 키는 `UPROPERTY(config)` 또는 startup settings라 **에디터 재시작 후에만** INI에서 로드된다. 실행 중인 에디터에 즉시 반영된다고 가정하지 말 것.

| 키 | 위치 |
|---|---|
| `DefaultUIPolicyClass` | `DefaultGame.ini` `[/Script/CommonUIStarterKit.CuGameUIManagerSubsystem]` |
| `EditorStartupMap` / `GameDefaultMap` | `DefaultEngine.ini` `[/Script/EngineSettings.GameMapsSettings]` |
| `InputData` | `DefaultEngine.ini` `[/Script/CommonInput.CommonInputSettings]` |

**C++ ↔ 에셋 이름 계약** — `Content/`는 텍스트 검색이 안 되므로 `Grep`에 안 잡힌다고 안전하다 판단하지 말 것.

- `BindWidget`/`BindWidgetOptional` 멤버명은 WBP 자식 위젯 이름과 **정확히** 일치해야 한다. 레이어 스택은 `Layer_Game`/`Layer_GameMenu`/`Layer_Menu`/`Layer_Modal`, 버튼 라벨은 `Text_Label`.
- `UCLASS` 이름·경로는 INI와 BP 부모 클래스가 문자열로 참조한다. `UPROPERTY(config)` 이름은 INI 키와 짝이다.

**Common UI 일반**

- push가 위젯을 자동 activate한다. push 직후 `ActivateWidget()`을 다시 부르지 말 것(이중 activation).
- `RegisterLayer(Tag, Stack)`는 `NativeOnInitialized`에서 명시적으로 호출한다. `BindWidget`만으로는 tag→container 맵이 채워지지 않는다.
- `CommonButtonBase`의 BP 클릭 델리게이트는 **`OnButtonBaseClicked`**(param Button)다. `OnClicked`/`BPOnClicked`가 아니다.
- `UCommonUIInputData`는 abstract다. `DA_CommonInputData`는 **BP 서브클래스**이고 INI 경로에 `_C` 접미사가 붙는다.
- Monolith 기본 `MonolithDefaultCommonButton_C`는 존재하지 않는다. `UCommonBoundActionBar`에는 자체 버튼 클래스(`WBP_ActionBarButton`)가 필요하다.

**Monolith 자동화**

- `blueprint.set_cdo_properties`(bulk)는 한 필드가 실패하면 atomic 롤백한다. 클래스 필드 클리어는 **단일 `set_cdo_property`에 빈 문자열**로. (`"None"` 문자열은 hard-ref 실패를 낸다.)
- `ui.add_widget`로 만든 non-button 위젯은 `IsVariable=false`다. 그래프에서 참조하기 전에 `ui.set_widget_is_variable`이 필요하다. `WBP_ButtonBase` 인스턴스는 자동으로 변수다.
- `find_object`/`find_first_object`로 라이브 PIE 위젯이 잡히지 않는다. python으로 델리게이트 broadcast를 테스트할 수 없다.

## Monolith MCP 사용 규칙

- `monolith_discover()` / `monolith_guide()`를 우선 호출하고, 액션 파라미터를 **추측하지 않는다**(discover/guide 결과 기준, 오류 시 `did_you_mean` 활용).
- 불확실한 C++ 시그니처는 `source` 네임스페이스(`get_signature`/`verify_symbols`/`check_deprecations`)로 확인한다.
- 파괴적 액션(에셋 삭제) 금지. 자동화 불가 항목은 건너뛰지 말고 수동 체크리스트로 정리한다.
- PIE 검증은 `editor.run_pie_smoke`(async, `on_compile_errors:"refuse"` 가드) → `poll_pie_smoke`. 컴파일 오류 사전 확인은 `editor.list_errored_blueprints`.

## 남은 수동 작업

- **MVVM View Binding** — `WBP_SettingsScreen`의 View Bindings를 에디터에서 `UCuSettingsViewModel`에 수동 연결.
