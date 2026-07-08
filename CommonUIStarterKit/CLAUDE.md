# CLAUDE.md — CommonUIStarterKit (UE 5.8 프로젝트)

이 파일은 **실제 UE 프로젝트**(`CommonUIStarterKit`)에서 작업할 때의 가이드다.
프로젝트는 스펙/프롬프트 저장소(`ue-commonui-starterkit`)의 **하위폴더로 containment** 되어 있다.
상위(스펙) 저장소 가이드는 저장소 루트의 `../CLAUDE.md`를 함께 참조하라.

## 언어 규칙

- 기본 언어는 **한국어**. 문서·설명·커밋 메시지·모든 입출력을 한국어로 작성.
- 기술 용어(Common UI, Activatable Widget, MVVM, Enhanced Input, GameplayTag, PrimaryGameLayout, GameUIPolicy 등)는 번역하지 말고 **영어 원문** 사용.

## 프로젝트 개요

Common UI 학습용 Starter Kit. Lyra **CommonGame 패턴을 학습용으로 재구현**(코드 복사 아님)하고,
진짜 엔진 라이브러리(CommonUI / CommonInput / EnhancedInput / UMG / ModelViewViewModel)는 그대로 재사용한다.
3-세션 워크플로(A 프로젝트+플러그인 / B C++ 뼈대 / C 에셋 Stage 1~6)로 만든다.

## 세션 A 완료 상태 (2026-07-08)

**게이트 통과**: 툴체인 + 모든 플러그인 module(CommonUI/CommonInput/MVVM/Monolith)이 UE 5.8에서 링크됨.

- **환경**: UE 5.8 = `C:\UE\UE_5.8` (Build.version 5.8.0, CL 55116800) · Visual Studio 2022 Community (MSVC 14.44) · git 2.45.1
- **플러그인 컴파일 게이트 빌드**: `CommonUIStarterKitEditor Win64 Development` → **Result: Succeeded** (432/432 action, 에러 0, ~106초).
  - 산출물: `Binaries/Win64/UnrealEditor-CommonUIStarterKit.dll` + `CommonUIStarterKitEditor.target`.
  - 경고: Monolith **자체 소스**의 deprecation(C4996) 28건뿐 — 우리 코드는 무경고, 빌드 실패 요인 아님.
  - 이 빌드는 Monolith를 precompiled DLL이 아니라 **소스에서 재컴파일**했다(from-scratch 프로젝트 빌드 + plugin Source 포함). → Monolith 소스까지 5.8에서 컴파일됨을 검증한 셈.
- **Monolith 5.8 호환 확인**: 릴리스 **v0.20.3 "UE 5.8 support"**의 `Monolith-v0.20.3-UE5.8.zip` 설치. precompiled 바이너리 `BuildId 55116800` == 엔진 `Changelist 55116800` (정확히 일치). VersionName 0.20.3, IsBetaVersion.

### ⚠️ 이 머신 고유의 quirk (세션 B/C도 반드시 지킬 것)

1. **UE 5.8이 비표준 경로 `C:\UE\UE_5.8`에 설치**돼 있고, **HKLM 레지스트리에 5.8이 등록돼 있지 않다**(5.5·5.7만 등록). 따라서 `.uproject`의 `EngineAssociation "5.8"`이 버전 문자열로 자동 해석되지 않는다.
   - **대응**: 엔진 도구는 **항상 `C:\UE\UE_5.8` 절대경로로 직접 호출**한다. 더블클릭/우클릭 메뉴에 의존하지 말 것.
   - 빌드: `& "C:\UE\UE_5.8\Engine\Build\BatchFiles\Build.bat" CommonUIStarterKitEditor Win64 Development -project="D:\Projects\Bong9\ClaudeCode\ue-commonui-starterkit\CommonUIStarterKit\CommonUIStarterKit.uproject" -waitmutex`
   - 에디터(세션 B-4): `& "C:\UE\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "D:\Projects\Bong9\ClaudeCode\ue-commonui-starterkit\CommonUIStarterKit\CommonUIStarterKit.uproject"`

2. **Containment 구조** — UE 프로젝트가 스펙 저장소의 `CommonUIStarterKit/` 하위폴더에 있다. 경로 앵커에 주의:
   - `.claude/settings.json` deny `Read()`는 저장소 루트에 있고 `/CommonUIStarterKit/Binaries/**` 등 하위폴더 경로로 이미 적응됨.
   - `.gitignore`도 루트에 있으며 `CommonUIStarterKit/Plugins/Monolith/` 등 하위폴더 패턴 추가됨(중간 슬래시 패턴은 루트 앵커라 필요).
   - **`.mcp.json` 위치(세션 B-4)**: Claude Code는 **워크스페이스(=스펙 저장소) 루트**의 `.mcp.json`을 로드한다. 따라서 `.mcp.json`은 **저장소 루트**에 두고, `command`는 하위폴더 접두어를 붙여 `CommonUIStarterKit/Plugins/Monolith/Binaries/monolith_proxy.exe`로 지정한다(프롬프트 B-4의 `Plugins/...`가 아님). `enabledMcpjsonServers: ["monolith"]`는 루트 `.claude/settings.json`에 이미 있음.

## 생성된 파일 (세션 A)

```
CommonUIStarterKit/
├─ CommonUIStarterKit.uproject          # EngineAssociation 5.8, plugin: CommonUI/ModelViewViewModel/Monolith
├─ Source/
│  ├─ CommonUIStarterKit.Target.cs       # Game 타깃
│  ├─ CommonUIStarterKitEditor.Target.cs # Editor 타깃(게이트 빌드/에디터 실행용)
│  └─ CommonUIStarterKit/                 # (세션 B에서 역할별 하위폴더로 구성; Public/Private 미사용)
│     ├─ CommonUIStarterKit.Build.cs      # dep 목록 + PublicIncludePaths.Add(ModuleDirectory)
│     ├─ CommonUIStarterKit.h / .cpp      # primary game module (루트)
│     └─ System/ · Layout/ · Widgets/ · Screens/ · ViewModels/   # 역할별 .h+.cpp 동거
├─ Config/
│  ├─ DefaultEngine.ini                   # 골격 + [세션 B에서 채움] 치명적 CommonUI 설정 TODO 마커
│  ├─ DefaultGame.ini                     # 골격 + DefaultUIPolicyClass TODO 마커
│  └─ DefaultInput.ini                    # 골격 (Enhanced Input 전용 파일 안내)
├─ Content/                               # 빈 폴더 (세션 C 에셋)
└─ Plugins/Monolith/                      # v0.20.3 UE5.8 (gitignore됨, 재설치 대상)
```

## 코딩 레퍼런스 (정본)

- **`../docs/common-ui-coding-reference.md`** (스펙 저장소 루트의 `docs/`) — Common UI/CommonGame/MVVM/Input의 API idiom·함정·네이밍 정본.
  - Containment 구조라 프로젝트 안에 별도 복사하지 않고 **상위 저장소의 원본을 그대로 참조**한다.
  - 세션 B는 C++ 작성 전 §3(모듈·Build.cs)·§4(INI)·§5(input)·§6(layer)·§7(MVVM)·§9(deprecated)를 먼저 읽을 것.

## 세션 B 완료 상태 (2026-07-08)

**C++ 3-class 뼈대 빌드 성공 + 에디터 실행 + Monolith MCP 서버(port 9316) 준비 완료.**

- **C++ 빌드**: `CommonUIStarterKitEditor Win64 Development` → **Result: Succeeded**, **우리 코드 에러·경고 0**(경고는 전부 Monolith 자체 소스 deprecation). UHT 리플렉션 생성 통과.
- **네이밍·구조 규약 (레퍼런스 §2·§3)**: 프로젝트 C++ 클래스는 **`Cu` 프리픽스**(`UCu*`/`FCu*`/`ECu*`), 소스는 **역할별 하위폴더**(System/Layout/Widgets/Screens/ViewModels)로 나누되 **`Public`/`Private`는 쓰지 않고** .h/.cpp를 같은 폴더에 둔다. ⚠️ 그래서 **Build.cs에 `PublicIncludePaths.Add(ModuleDirectory)`**를 넣어야 하위폴더 헤더의 **루트 기준 경로 한정 include**(`#include "System/CuGameUIPolicy.h"`)가 해석된다(누락 시 `C1083`).
- **생성한 C++ 클래스** (`Source/CommonUIStarterKit/<role>/`):
  - `Layout/`: `CuGameplayTags.h/.cpp`(`UI.Layer.Game/GameMenu/Menu/Modal` 네이티브 태그) · `CuPrimaryGameLayout.h/.cpp`(4-layer BindWidgetOptional + `RegisterLayer` + `PushWidgetToLayerStack`/`Async` + static `GetPrimaryGameLayout`, `ECuAsyncWidgetLayerState`).
  - `System/`: `CuLocalPlayer.h`(header-only) · `CuGameUIManagerSubsystem.h/.cpp` · `CuGameUIPolicy.h/.cpp`(Within=CuGameUIManagerSubsystem, per-player `FCuRootViewportLayoutInfo`).
  - `Widgets/`: `CuActivatableWidget.h/.cpp`(`GetDesiredInputConfig()`→`FUIInputConfig(InputMode, NoCapture)`) · `CuButtonBase.h/.cpp`(`Text_Label` BindWidgetOptional).
  - `Screens/`: `CuTitleScreenWidget`/`CuMainMenuWidget`/`CuSettingsScreenWidget`/`CuPauseMenuWidget`/`CuConfirmationModalWidget`(header-only, 세션 C에서 BindWidget·로직 확장).
  - `ViewModels/`: `CuSettingsViewModel.h/.cpp`(`UMVVMViewModelBase`, FieldNotify `MasterVolume` + `UE_MVVM_SET_PROPERTY_VALUE`).
  - module 파일은 루트에 `CommonUIStarterKit.h` + `.cpp`. Build.cs에 `FieldNotification`(MVVM FieldNotify 생성 코드) + `PublicIncludePaths.Add(ModuleDirectory)` 추가.
- **레이아웃 생성 트리거**(재구현 선택): subsystem이 `FGameModeEvents::OnGameModePostLoginEvent`(PC 로그인=PC 확보 보장) + `UGameInstance::OnLocalPlayerRemovedEvent`에 바인딩 → policy에 위임. policy는 `UCuLocalPlayer::OnPlayerControllerSet`에도 구독(PC 교체/지연 대응). ⚠️ **세션 C Stage 2에서 PIE로 실제 타이밍을 검증**하고, 필요 시 트리거를 조정한다.
- **INI 실제 값**: `DefaultEngine.ini`에 **`GameViewportClientClassName=/Script/CommonUI.CommonGameViewportClient`**[치명적] + **`LocalPlayerClassName=/Script/CommonUIStarterKit.CuLocalPlayer`** 설정 완료. `DefaultGame.ini`의 `DefaultUIPolicyClass`(`[/Script/CommonUIStarterKit.CuGameUIManagerSubsystem]`)와 `CommonInputSettings.InputData`는 **세션 C에서 에셋 생성 후 값 확정**(지금은 주석 — 존재하지 않는 경로 로드 경고 방지). INI 배치는 레퍼런스 §4 기준(DefaultEngine.ini).
- **`.mcp.json`**: 저장소 루트에 생성. `command="CommonUIStarterKit/Plugins/Monolith/Binaries/monolith_proxy.exe"`, `args=[]`. (프록시는 args 없이 시작해 `http://localhost:9316/mcp`로 포워딩함을 확인.)
- **에디터 실행**: `UnrealEditor.exe`(위 절대경로)로 실행 중. **port 9316 LISTEN = UnrealEditor(PID 확인), HTTP 응답 OK, Monolith 로그 "Project indexing complete"**. (에셋 0개 인덱싱 경고는 아직 /Game 에셋이 없어서 정상 — 세션 C에서 재인덱싱.)

### ⚠️ B→C 경계: Claude Code 재시작 필수

`.mcp.json`은 **세션 시작 시점에만** 로드된다. 이 세션(A→B 연속)에서는 Monolith MCP 도구가 아직 없다.
**세션 C 전에 반드시 Claude Code를 재시작**하라. 재시작 후 `enabledMcpjsonServers: ["monolith"]`(루트 `.claude/settings.json`) 덕분에 신뢰 프롬프트 없이 monolith 서버가 로드된다.
**에디터는 계속 실행 상태로 둘 것**(끄면 port 9316 서버가 내려가 세션 C에서 MCP가 붙지 못함). 에디터를 재시작하면 위 절대경로 명령으로 다시 띄운다.

## Monolith MCP 사용 규칙 (세션 C)

- `monolith_discover()` / `monolith_guide()`를 우선 호출하고, 액션 파라미터를 **추측하지 않는다**(discover/guide 결과 기준, 오류 시 `did_you_mean` 활용).
- 불확실한 C++ 시그니처는 `source` 네임스페이스(`get_signature`/`verify_symbols`/`check_deprecations`)로 확인.
- 파괴적 액션(에셋 삭제) 금지. 자동화 불가 항목은 건너뛰지 말고 수동 체크리스트로 정리.

## 다음 세션 C에서 할 일 (에셋 Stage 1~6)

프롬프트 C(`../common-ui-starter-kit-prompt.md` 프롬프트 C) 기준:

1. **사전 Probe**: `monolith_discover()`/`guide()`로 역량 확인 + 자동 불가 항목(특히 MVVM View Binding)을 수동 fallback 체크리스트로 `CLAUDE.md`에 기록.
2. **Stage 1~6**을 순차 생성·**각 Stage마다 PIE 검증**:
   - Stage 1 `WBP_StarterActivatable`, Stage 2 `WBP_PrimaryGameLayout`(스택 1개→RegisterLayer) + 정책/subsystem 배선(DefaultUIPolicyClass 값 확정),
   - Stage 3 style 에셋 + `WBP_ButtonBase`/`WBP_TitleScreen`/`WBP_MainMenu`, Stage 4 CommonUI DataTable 입력 경로(+ `CommonInputSettings.InputData` INI 값 확정),
   - Stage 5 4-layer + PauseMenu/ConfirmationModal, Stage 6 MVVM 설정 화면(View Binding은 수동 fallback).
3. C++ base 클래스에서 파생한 WBP를 만들 때 **BindWidget 이름을 C++ 멤버명과 정확히 일치**(§6): 레이어 스택은 `Layer_Game/Layer_GameMenu/Layer_Menu/Layer_Modal`, 버튼 텍스트는 `Text_Label`.

## 함정 빠른참조 (레퍼런스 §10 발췌)

- `GameViewportClientClassName` 미설정 → gamepad/focus/Back 무음 사망.
- `SetInputMode*` 직접 호출 금지 → `GetDesiredInputConfig()` override.
- `ECommonInputMode::GameAndMenu` 없음(=`All`).
- FieldNotify setter 직접 대입 금지 → `UE_MVVM_SET_PROPERTY_VALUE`.
- `CommonInput`/`EnhancedInput`을 `.uproject` plugin으로 넣지 말 것 → Build.cs dep으로만.
- push 후 `ActivateWidget` 재호출 금지 · `RegisterLayer`는 `NativeOnInitialized`에서 명시 · BindWidget 이름 엄격 일치.

## 세션 C — 사전 Probe 결과 (2026-07-09)

Monolith v0.20.3 연결 확인(port 9316, 에디터 PID 35224, engine CL 55116800). `monolith_discover()` 기준 역량 매트릭스:

| 역량 | 자동화 액션 | 판정 |
|------|-------------|------|
| (a) BindWidget child 이름 WBP 트리 | `ui.create_widget_blueprint`/`add_widget`/`rename_widget`/`set_widget_is_variable` | ✅ 자동 (이름 일치 Stage 2/3 검증) |
| (b) Blueprint graph 편집 | `blueprint.add_node`/`connect_pins`/`add_nodes_bulk` + `ui.push_to_activatable_stack` | ✅ 자동 |
| (c) style nested struct/sub-object | `ui.create_common_button_style`/`create_common_text_style`/`set_brush`/`set_font` + `blueprint.set_cdo_properties`/`set_property_at_path` | ✅ 자동 |
| (d) **MVVM View Binding** | 전용 액션 불확실(`ui.bind_widget_to_attribute`는 GAS attribute용) | ⚠️ **Stage 6에서 재확인, 실패 시 수동 fallback** |
| (e) Empty 레벨 + World Settings GameMode override | `editor.create_empty_map` + `editor.author_map_settings(game_mode_override)` | ✅ 자동 (수동 fallback 불필요) |

PIE 검증: `editor.run_pie_smoke`(async, `on_compile_errors:"refuse"` 가드) → `poll_pie_smoke`. 컴파일 오류 사전 확인은 `editor.list_errored_blueprints`.

### Stage 0 완료 (2026-07-09) — 프레임워크 부트스트랩

- **`/Game/Maps/L_StarterKit`**(Empty Level) + PlayerStart 1개.
- **`/Game/Core/BP_StarterGameMode`**(AGameModeBase): `PlayerControllerClass=BP_StarterPlayerController_C`, `DefaultPawnClass=None`(빈 문자열로 클리어 — "None" 문자열은 hard-ref 실패).
- **`/Game/Core/BP_StarterPlayerController`**(APlayerController): `bShowMouseCursor=true`.
- **World Settings GameModeOverride** = `BP_StarterGameMode`(`editor.author_map_settings`, .umap 저장).
- **`DefaultEngine.ini`** `[/Script/EngineSettings.GameMapsSettings]`: `EditorStartupMap`=`GameDefaultMap`=`/Game/Maps/L_StarterKit.L_StarterKit`. ⚠️ 에디터는 시작 시 config를 읽으므로 **다음 실행부터** L_StarterKit 자동 오픈(현 세션 에디터는 재시작 금지 — MCP 유지).
- **PIE 검증 통과**: `run_pie_smoke` `ok:true`, 크래시/에러 0, 런타임 probe로 `GM=BP_StarterGameMode_C PC=BP_StarterPlayerController_C` 확인. LocalPlayer(`UCuLocalPlayer`) 검증은 Stage 2에서 policy 캐스팅으로 확인.
- ⚠️ 소소한 quirk: `blueprint.set_cdo_properties`(bulk)가 한 필드 실패 시 atomic 롤백 → 클래스 클리어는 **단일 `set_cdo_property`에 빈 문자열**로.

### Stage 1 완료 (2026-07-09)

- **`/Game/UI/Foundation/WBP_StarterActivatable`**(UCuActivatableWidget 파생) + 중앙 TextBlock `Txt_Hello`("Stage 1 — Hello Common UI"). 컴파일 클린, PIE-safe, `audit_commonui_widget` 통과(경고 1건: DesiredFocusTargetName 없음 — 텍스트 전용이라 정상, 포커스 타깃은 Stage 3 버튼부터).

### Stage 2 완료 (2026-07-09) — 레이아웃/정책 배선 검증

- **`/Game/UI/Foundation/WBP_PrimaryGameLayout`**(UCuPrimaryGameLayout 파생) + `Layer_Menu` 스택(CommonActivatableWidgetStack, BindWidgetOptional 이름 일치, stretch_fill).
- **`/Game/UI/Foundation/BP_StarterUIPolicy`**(UCuGameUIPolicy 파생) → `LayoutClass=WBP_PrimaryGameLayout_C`.
- **`DefaultGame.ini`** `[/Script/CommonUIStarterKit.CuGameUIManagerSubsystem] DefaultUIPolicyClass=/Game/UI/Foundation/BP_StarterUIPolicy.BP_StarterUIPolicy_C` 활성화.
- **PIE 검증 통과**(런타임): `DefaultUIPolicyClass`가 `UPROPERTY(config)`라 **에디터 시작 시에만 로드** → 현 세션에선 subsystem **CDO에 값을 live로 set(run_python, get_default_object)** 한 뒤 `start_pie`. 결과 `Layer_Menu` 컨테이너가 라이브 트리에 존재(=subsystem→policy→layout 자동 생성 + **`UCuLocalPlayer` 캐스팅 성공** 확인) → `push_to_activatable_stack`로 WBP_StarterActivatable push → `stack_depth=1, active=WBP_StarterActivatable_C`.
- ⚠️ **핵심 quirk 발견**: `DefaultUIPolicyClass`(config)와 `EditorStartupMap`(GameMapsSettings)은 **에디터 재시작 후에만** INI에서 로드된다. 현 세션 검증은 CDO live-set 시밍으로 우회. 배포 kit은 재시작 시 정상 배선.
- ⚠️ **C++ 제약 발견 (Stage 3~5 영향)**: `UCuPrimaryGameLayout::PushWidgetToLayerStack`은 **template(비-UFUNCTION)**, 네이티브 태그도 BP 미노출. `UCommonActivatableWidgetStack`도 **BP/스크립트 노출 add/push 메서드 없음**(python `dir` 확인). → **버튼 내비게이션(OnClicked→push)을 BP로 저작하려면 C++에 BlueprintCallable push 헬퍼 추가 필요**(Lyra의 `UCommonUIExtensions::PushContentToLayer`에 대응). Stage 3 진입 전 결정 필요.

### C++ 헬퍼 추가 + 재빌드 완료 (2026-07-09) — Stage 3~5 내비게이션 언블록

- **`UCuPrimaryGameLayout`에 BP 노출 추가**(`CuPrimaryGameLayout.h/.cpp`): `UFUNCTION(BlueprintCallable) PushWidgetToLayer(FGameplayTag, TSubclassOf<UCommonActivatableWidget>)`(template push 래퍼) + `RemoveWidgetFromLayer` + `BlueprintPure` 태그 접근자 `GetLayerTag_Game/GameMenu/Menu/Modal`. `.cpp`에 `#include "CommonActivatableWidget.h"` 추가(template 인스턴스화 시 complete type 필요).
- **재빌드**: 에디터 종료 → `Build.bat CommonUIStarterKitEditor`(증분, ~10s, **Result: Succeeded**, 에러 0) → 재실행. Live Coding은 새 UFUNCTION(UHT 재생성) 불가라 **전체 재빌드+에디터 재시작**이 필요했음.
- **재시작 부수 효과 = 요구사항 검증 2건**: (1) **요구사항 2 실증** — 에디터가 `L_StarterKit`을 자동 오픈(`get_editor_world().get_name()=='L_StarterKit'`). (2) `DefaultUIPolicyClass`가 이제 INI에서 정상 로드(CDO 시밍 불필요).
- python 노출 확인: `push_widget_to_layer`/`remove_widget_from_layer`/`get_layer_tag_menu` 등 정상. → BP 그래프에서 push 저작 가능.
- (2차 소규모 재빌드) 화면 위젯이 레이아웃을 얻도록 `static UFUNCTION GetPrimaryGameLayoutForPlayer(APlayerController*)` 추가 → Build 성공. 화면의 `GetOwningPlayer → GetPrimaryGameLayoutForPlayer → PushWidgetToLayer` 내비 패턴 성립.

### Stage 3 진행 (2026-07-09) — style + 버튼 + 타이틀/메뉴 내비게이션

- **Style 에셋**(`/Game/UI/Style`): `CBS_Default`/`CBS_Primary`(CommonButtonStyle, NormalBase/Hovered/Pressed RoundedBox tint — 포커스/hover 피드백), `CTS_Header`/`CTS_Body`/`CTS_Button`(CommonTextStyle, Roboto + Size/Color). ⚠️ 사전 Probe (c) 결론: **nested struct/브러시/폰트 자동화 성공**(`set_cdo_properties` 중첩 JSON, errors:0) — 수동 fallback 불필요.
- **`WBP_ButtonBase`**(UCuButtonBase): Overlay + `Text_Label`(CommonTextBlock, CTS_Button), `Style=CBS_Default`, `ButtonText` 기본값. 컴파일 클린.
- **`WBP_TitleScreen`**(UCuTitleScreenWidget): `Txt_Title`(CTS_Header) + `Btn_Start`("Press to Start"). 그래프: **`OnButtonBaseClicked`(=CommonButtonBase의 BP 바인딩 델리게이트) → GetOwningPlayer → GetPrimaryGameLayoutForPlayer → PushWidgetToLayer(Menu, WBP_MainMenu)**. 컴파일 클린(연결 5/5 OK).
- **`WBP_MainMenu`**(UCuMainMenuWidget): `MenuBox`(VerticalBox) + `Btn_Start`/`Btn_Settings`/`Btn_Quit`(WBP_ButtonBase, per-instance ButtonText). ⚠️ **버튼 클릭 액션 미배선**(다음 작업).
- **`WBP_PrimaryGameLayout` 그래프**: Event Construct → `PushWidgetToLayer(GetLayerTag_Menu, WBP_TitleScreen)` → **재시작만으로 플레이 시 타이틀 자동 표시**.
- **PIE 검증**: `start_pie`(CDO 시밍 없이, INI에서 정책 로드) → `Layer_Menu` depth 1 `active=WBP_TitleScreen_C`(자동 push 확인) → 런타임 push로 `WBP_MainMenu_C` depth 2(렌더 확인). 크래시 0.
- ⚠️ **미검증**: 실제 버튼 '클릭→push'의 런타임 실행(Monolith가 라이브 위젯을 내부 해석하지만 python `find_object`엔 안 잡혀 델리게이트 broadcast 테스트 불가). 그래프는 컴파일·배선으로 검증됨 → **입력 붙는 Stage 4/통합 PIE에서 실입력으로 확인**.
- 델리게이트 이름 quirk: CommonButtonBase의 BP 클릭 델리게이트는 `OnButtonBaseClicked`(param Button). `OnClicked`/`BPOnClicked` 아님 → `ui.list_widget_events`로 확인.

### Stage 3 마감 (2026-07-09)
- MainMenu: Btn_Quit→QuitGame, Btn_Start→PauseMenu(Stage5), Btn_Settings→SettingsScreen(Stage6). `GetDesiredFocusTarget` override(MainMenu/TitleScreen→Btn_Start)로 gamepad 초기 포커스. ⚠️ 실입력 클릭은 standalone/뷰포트 포커스에서 확인(에디터 in-viewport PIE는 포커스 미획득).

### Stage 4 완료 (2026-07-09) — CommonUI DataTable 입력 경로
- `/Game/Input/DT_UIActions`(FCommonInputActionDataBase: Back=Esc/Gamepad_FaceButton_Right, Accept=Enter/FaceButton_Bottom) + `DA_CommonInputData`(**BP 서브클래스** — UCommonUIInputData가 abstract; DefaultClickAction=Accept, DefaultBackAction=Back).
- `DefaultEngine.ini` `[/Script/CommonInput.CommonInputSettings] InputData=/Game/Input/DA_CommonInputData.DA_CommonInputData_C` 활성화(재시작 후 로드).
- `WBP_ActionBarButton`(자체 CommonActionWidget=`InputActionWidget` glyph) + `UCommonBoundActionBar`를 레이아웃에 배치. ⚠️ Monolith 기본 `MonolithDefaultCommonButton_C`는 미존재 → 자체 버튼 클래스 필요.

### Stage 5 완료 (2026-07-09) — 4-layer + 모달
- 레이아웃 4-layer 확장(Layer_Game/GameMenu/Menu/Modal) + z-order(GameMenu 10 < Menu 20 < Modal 30 < ActionBar 100).
- `WBP_PauseMenu`(Resume→pop, Quit→ConfirmationModal push), `WBP_ConfirmationModal`(Cancel→pop, Confirm→QuitGame), 둘 다 `bIsBackHandler` + `GetDesiredFocusTarget`.

### Stage 6 완료 (2026-07-09) — MVVM 설정 (캡스톤)
- `WBP_SettingsScreen`(Slider_Volume + Txt_Value + Btn_Back): Slider `OnValueChanged`→FormatText→SetText로 라벨 실시간 갱신. Back→pop, bIsBackHandler, 포커스 타깃.
- ⚠️ 선언적 **MVVM View Binding은 수동 fallback**(probe (d) 확정): Monolith에 뷰모델 바인딩 저작 액션 없음. `UCuSettingsViewModel`(C++)은 준비됨 — View Bindings 에디터에서 수동 연결.
- quirk: `add_widget`로 만든 non-button 위젯(Txt_Value 등)은 IsVariable=false → 그래프 참조 전 `ui.set_widget_is_variable` 필요. WBP_ButtonBase 인스턴스는 자동 변수.

### 최종 검증 완료 (2026-07-09)
- 에디터 재시작 → 전체 INI 로드(DefaultUIPolicyClass·EditorStartupMap·CommonInputSettings.InputData). `get_editor_world()=='L_StarterKit'`(요구사항 2 재실증).
- 통합 PIE: 타이틀 자동 push(Menu depth 1) → 런타임 push로 MainMenu(Menu depth 2)·ConfirmationModal(Modal depth 1) 렌더 확인. **errored blueprint 0, error-level 로그 0, 크래시 0.**
- `README.md` 작성 완료(프로젝트 개요·3세션·Stage 0~6·에셋 목록·Lyra 대응표·수동 fallback·Stage 7).

### 델리게이트/그래프 quirk 메모 (세션 C에서 확인)
- CommonButtonBase BP 클릭 델리게이트 = **`OnButtonBaseClicked`**(param Button). `OnClicked`/`BPOnClicked` 아님 → `ui.list_widget_events`로 확인.
- 버튼 내비 패턴: `OnButtonBaseClicked`(ComponentBoundEvent) → GetOwningPlayer(pure) → GetPrimaryGameLayoutForPlayer → PushWidgetToLayer(GetLayerTag_X, WidgetClass). pop은 `DeactivateWidget`(self).
- `find_object`/`find_first_object`로 라이브 PIE 위젯이 안 잡힘 → python 델리게이트 broadcast 테스트 불가(그래프는 컴파일·배선으로 검증).
