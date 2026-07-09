# CommonUIStarterKit — 세션 진행 기록

세션 A/B/C의 **지나간 기록**이다. 앞으로 지켜야 할 규칙은 여기가 아니라 [`project-guide.md`](project-guide.md)에 있다.

이 문서는 자동 로드되지 않으며, "그때 왜 그렇게 했는지"를 되짚을 때만 읽으면 된다.

---

## 세션 A 완료 (2026-07-08) — 프로젝트 + 플러그인

**게이트 통과**: 툴체인 + 모든 플러그인 module(CommonUI/CommonInput/MVVM/Monolith)이 UE 5.8에서 링크됨.

- **환경**: UE 5.8 = `C:\UE\UE_5.8` (Build.version 5.8.0, CL 55116800) · Visual Studio 2022 Community (MSVC 14.44) · git 2.45.1
- **플러그인 컴파일 게이트 빌드**: `CommonUIStarterKitEditor Win64 Development` → **Result: Succeeded** (432/432 action, 에러 0, ~106초).
  - 산출물: `Binaries/Win64/UnrealEditor-CommonUIStarterKit.dll` + `CommonUIStarterKitEditor.target`.
  - 경고: Monolith **자체 소스**의 deprecation(C4996) 28건뿐 — 우리 코드는 무경고.
  - 이 빌드는 Monolith를 precompiled DLL이 아니라 **소스에서 재컴파일**했다(from-scratch 프로젝트 빌드 + plugin Source 포함). Monolith 소스까지 5.8에서 컴파일됨을 검증한 셈.
- **Monolith 5.8 호환 확인**: 릴리스 **v0.20.3 "UE 5.8 support"**의 `Monolith-v0.20.3-UE5.8.zip` 설치. precompiled 바이너리 `BuildId 55116800` == 엔진 `Changelist 55116800`(정확히 일치). VersionName 0.20.3, IsBetaVersion.

## 세션 B 완료 (2026-07-08) — C++ 뼈대

**C++ 3-class 뼈대 빌드 성공 + 에디터 실행 + Monolith MCP 서버(port 9316) 준비 완료.**

- **C++ 빌드**: `CommonUIStarterKitEditor Win64 Development` → **Result: Succeeded**, **우리 코드 에러·경고 0**. UHT 리플렉션 생성 통과.
- **생성한 C++ 클래스** (`Source/CommonUIStarterKit/<role>/`):
  - `Layout/`: `CuGameplayTags`(`UI.Layer.Game/GameMenu/Menu/Modal` 네이티브 태그) · `CuPrimaryGameLayout`(4-layer BindWidgetOptional + `RegisterLayer` + `PushWidgetToLayerStack`/`Async` + static `GetPrimaryGameLayout`, `ECuAsyncWidgetLayerState`).
  - `System/`: `CuLocalPlayer`(header-only) · `CuGameUIManagerSubsystem` · `CuGameUIPolicy`(Within=CuGameUIManagerSubsystem, per-player `FCuRootViewportLayoutInfo`).
  - `Widgets/`: `CuActivatableWidget`(`GetDesiredInputConfig()`→`FUIInputConfig(InputMode, NoCapture)`) · `CuButtonBase`(`Text_Label` BindWidgetOptional).
  - `Screens/`: `CuTitleScreenWidget`/`CuMainMenuWidget`/`CuSettingsScreenWidget`/`CuPauseMenuWidget`/`CuConfirmationModalWidget`(header-only).
  - `ViewModels/`: `CuSettingsViewModel`(`UMVVMViewModelBase`, FieldNotify `MasterVolume` + `UE_MVVM_SET_PROPERTY_VALUE`).
  - Build.cs에 `FieldNotification`(MVVM FieldNotify 생성 코드) + `PublicIncludePaths.Add(ModuleDirectory)` 추가.
- **INI**: `DefaultEngine.ini`에 치명적 두 줄(`GameViewportClientClassName` + `LocalPlayerClassName`) 설정 완료. `DefaultUIPolicyClass`와 `CommonInputSettings.InputData`는 세션 C에서 에셋 생성 후 값 확정(당시엔 주석 — 존재하지 않는 경로 로드 경고 방지).
- **`.mcp.json`**: 저장소 루트에 생성. `command="CommonUIStarterKit/Plugins/Monolith/Binaries/monolith_proxy.exe"`, `args=[]`.
- **에디터 실행**: port 9316 LISTEN(UnrealEditor PID 확인), HTTP 응답 OK, Monolith 로그 "Project indexing complete". (에셋 0개 인덱싱 경고는 당시 `/Game` 에셋이 없어 정상.)

## 세션 C (2026-07-09) — 에셋 Stage 0~6

### 사전 Probe

Monolith v0.20.3 연결 확인(port 9316, 에디터 PID 35224, engine CL 55116800). `monolith_discover()` 기준 역량 매트릭스:

| 역량 | 자동화 액션 | 판정 |
|------|-------------|------|
| (a) BindWidget child 이름 WBP 트리 | `ui.create_widget_blueprint`/`add_widget`/`rename_widget`/`set_widget_is_variable` | ✅ 자동 |
| (b) Blueprint graph 편집 | `blueprint.add_node`/`connect_pins`/`add_nodes_bulk` + `ui.push_to_activatable_stack` | ✅ 자동 |
| (c) style nested struct/sub-object | `ui.create_common_button_style`/`create_common_text_style`/`set_brush`/`set_font` + `blueprint.set_cdo_properties`/`set_property_at_path` | ✅ 자동 |
| (d) **MVVM View Binding** | 전용 액션 불확실(`ui.bind_widget_to_attribute`는 GAS attribute용) | ⚠️ **Stage 6에서 수동 fallback 확정** |
| (e) Empty 레벨 + World Settings GameMode override | `editor.create_empty_map` + `editor.author_map_settings(game_mode_override)` | ✅ 자동 |

### Stage 0 — 프레임워크 부트스트랩

- `/Game/Maps/L_StarterKit`(Empty Level) + PlayerStart 1개.
- `/Game/Core/BP_StarterGameMode`(AGameModeBase): `PlayerControllerClass=BP_StarterPlayerController_C`, `DefaultPawnClass=None`(빈 문자열로 클리어 — `"None"` 문자열은 hard-ref 실패).
- `/Game/Core/BP_StarterPlayerController`(APlayerController): `bShowMouseCursor=true`.
- World Settings `GameModeOverride` = `BP_StarterGameMode`(`editor.author_map_settings`, `.umap` 저장).
- `DefaultEngine.ini` `[GameMapsSettings]`: `EditorStartupMap`=`GameDefaultMap`=`/Game/Maps/L_StarterKit.L_StarterKit`.
- **PIE 검증 통과**: `run_pie_smoke` `ok:true`, 크래시/에러 0. 런타임 probe로 `GM=BP_StarterGameMode_C PC=BP_StarterPlayerController_C` 확인.

### Stage 1 — 첫 Activatable Widget

- `/Game/UI/Foundation/WBP_StarterActivatable`(`UCuActivatableWidget` 파생) + 중앙 TextBlock `Txt_Hello`.
- 컴파일 클린, PIE-safe, `audit_commonui_widget` 통과. 경고 1건(DesiredFocusTargetName 없음)은 텍스트 전용이라 정상.

### Stage 2 — 레이아웃/정책 배선 검증

- `/Game/UI/Foundation/WBP_PrimaryGameLayout`(`UCuPrimaryGameLayout` 파생) + `Layer_Menu` 스택(BindWidgetOptional 이름 일치, stretch_fill).
- `/Game/UI/Foundation/BP_StarterUIPolicy`(`UCuGameUIPolicy` 파생) → `LayoutClass=WBP_PrimaryGameLayout_C`.
- `DefaultGame.ini`에 `DefaultUIPolicyClass=/Game/UI/Foundation/BP_StarterUIPolicy.BP_StarterUIPolicy_C` 활성화.
- **PIE 검증 통과**: `DefaultUIPolicyClass`가 `UPROPERTY(config)`라 에디터 시작 시에만 로드 → 당시 세션에선 subsystem **CDO에 값을 live로 set**한 뒤 `start_pie`. `Layer_Menu` 컨테이너가 라이브 트리에 존재(= subsystem→policy→layout 자동 생성 + **`UCuLocalPlayer` 캐스팅 성공**) → `push_to_activatable_stack`로 `stack_depth=1`.
- ⚠️ **여기서 발견**: config 키(`DefaultUIPolicyClass`, `EditorStartupMap`)는 에디터 재시작 후에만 로드된다.
- ⚠️ **여기서 발견**: `PushWidgetToLayerStack`은 template(비-UFUNCTION), 네이티브 태그도 BP 미노출, `UCommonActivatableWidgetStack`에도 BP 노출 push 없음 → BP 내비게이션을 위해 C++ 헬퍼가 필요하다.

### C++ 헬퍼 추가 + 재빌드 — Stage 3~5 내비게이션 언블록

- `UCuPrimaryGameLayout`에 BP 노출 추가: `PushWidgetToLayer` · `RemoveWidgetFromLayer` · `GetLayerTag_*` · `GetPrimaryGameLayoutForPlayer`. `.cpp`에 `#include "CommonActivatableWidget.h"` 추가(template 인스턴스화 시 complete type 필요).
- **재빌드**: 에디터 종료 → `Build.bat`(증분 ~10s, Succeeded, 에러 0) → 재실행. **Live Coding은 새 UFUNCTION(UHT 재생성) 불가**라 전체 재빌드 + 에디터 재시작이 필요했다.
- **재시작 부수 효과 = 검증 2건**: 에디터가 `L_StarterKit`을 자동 오픈, `DefaultUIPolicyClass`가 INI에서 정상 로드(CDO 시밍 불필요).

### Stage 3 — style + 버튼 + 타이틀/메뉴 내비게이션

- **Style**(`/Game/UI/Style`): `CBS_Default`/`CBS_Primary`(CommonButtonStyle, NormalBase/Hovered/Pressed RoundedBox tint), `CTS_Header`/`CTS_Body`/`CTS_Button`(CommonTextStyle, Roboto). Probe (c) 결론 확정 — nested struct/브러시/폰트 **자동화 성공**, 수동 fallback 불필요.
- `WBP_ButtonBase`(`UCuButtonBase`): Overlay + `Text_Label`(CTS_Button), `Style=CBS_Default`.
- `WBP_TitleScreen`: `Txt_Title` + `Btn_Start`. 그래프 `OnButtonBaseClicked → GetOwningPlayer → GetPrimaryGameLayoutForPlayer → PushWidgetToLayer(Menu, WBP_MainMenu)`.
- `WBP_MainMenu`: `MenuBox`(VerticalBox) + `Btn_Start`/`Btn_Settings`/`Btn_Quit`. 마감 시 `Btn_Quit→QuitGame`, `Btn_Start→PauseMenu`, `Btn_Settings→SettingsScreen` 배선. `GetDesiredFocusTarget` override로 gamepad 초기 포커스.
- `WBP_PrimaryGameLayout` 그래프: `Event Construct → PushWidgetToLayer(GetLayerTag_Menu, WBP_TitleScreen)`.
- **PIE 검증**: 타이틀 자동 push(depth 1) → 런타임 push로 MainMenu depth 2. 크래시 0.
- ⚠️ **미검증**: 실제 버튼 클릭→push의 런타임 실행. `find_object`로 라이브 위젯이 안 잡혀 델리게이트 broadcast 테스트 불가. 그래프는 컴파일·배선으로 검증. 에디터 in-viewport PIE는 포커스를 못 얻으므로 실입력은 standalone에서 확인.
- ⚠️ **여기서 발견**: `CommonButtonBase`의 BP 클릭 델리게이트는 `OnButtonBaseClicked`(param Button). `OnClicked`/`BPOnClicked` 아님 → `ui.list_widget_events`로 확인.

### Stage 4 — CommonUI DataTable 입력 경로

- `/Game/Input/DT_UIActions`(`FCommonInputActionDataBase`: Back=Esc/Gamepad_FaceButton_Right, Accept=Enter/FaceButton_Bottom).
- `DA_CommonInputData` — **BP 서브클래스**(`UCommonUIInputData`가 abstract). DefaultClickAction=Accept, DefaultBackAction=Back.
- `DefaultEngine.ini` `[CommonInputSettings] InputData=/Game/Input/DA_CommonInputData.DA_CommonInputData_C` 활성화(재시작 후 로드).
- `WBP_ActionBarButton`(자체 `CommonActionWidget`=`InputActionWidget` glyph) + `UCommonBoundActionBar`를 레이아웃에 배치. ⚠️ Monolith 기본 `MonolithDefaultCommonButton_C`는 미존재 → 자체 버튼 클래스 필요.

### Stage 5 — 4-layer + 모달

- 레이아웃 4-layer 확장(`Layer_Game`/`GameMenu`/`Menu`/`Modal`) + z-order(GameMenu 10 < Menu 20 < Modal 30 < ActionBar 100).
- `WBP_PauseMenu`(Resume→pop, Quit→ConfirmationModal push), `WBP_ConfirmationModal`(Cancel→pop, Confirm→QuitGame). 둘 다 `bIsBackHandler` + `GetDesiredFocusTarget`.

### Stage 6 — MVVM 설정 (캡스톤)

- `WBP_SettingsScreen`(`Slider_Volume` + `Txt_Value` + `Btn_Back`): Slider `OnValueChanged`→FormatText→SetText로 라벨 실시간 갱신. Back→pop, `bIsBackHandler`, 포커스 타깃.
- ⚠️ 선언적 **MVVM View Binding은 수동 fallback**(probe (d) 확정). Monolith에 뷰모델 바인딩 저작 액션 없음. `UCuSettingsViewModel`(C++)은 준비됨.
- ⚠️ **여기서 발견**: `add_widget`로 만든 non-button 위젯(`Txt_Value` 등)은 `IsVariable=false` → 그래프 참조 전 `ui.set_widget_is_variable` 필요. `WBP_ButtonBase` 인스턴스는 자동 변수.

### 최종 검증 (2026-07-09)

- 에디터 재시작 → 전체 INI 로드(`DefaultUIPolicyClass`·`EditorStartupMap`·`CommonInputSettings.InputData`). `get_editor_world()=='L_StarterKit'`.
- 통합 PIE: 타이틀 자동 push(Menu depth 1) → 런타임 push로 MainMenu(Menu depth 2)·ConfirmationModal(Modal depth 1) 렌더 확인. **errored blueprint 0, error-level 로그 0, 크래시 0.**
- `README.md` 작성 완료(프로젝트 개요·3세션·Stage 0~6·에셋 목록·Lyra 대응표·수동 fallback·Stage 7).

---

## 코드 리뷰 & 리팩토링 (2026-07-09)

Stage 0~6 완료 후 `ue-code-reviewer` / `ue-code-refactorer` 서브에이전트로 전수 감사를 수행했다. 리뷰어에게는 사전 조사 결과를 주지 않고 독립적으로 찾게 했다.

**기준선**: 문서 재편 커밋(`fa3b9ef`) 시점에서 콜드 빌드 `Result: Succeeded`, 우리 코드 에러·경고 0.

### 고친 것 (커밋 4개)

| 커밋 | 내용 |
|---|---|
| `57ff031` | 낡은 주석 7곳 정정 (주석 전용) |
| `c06e95c` | 매직 넘버 `1000` → `CuLayoutZOrder`, 미사용 파라미터 주석 처리, 미사용 `using` 제거 |
| `74476b8` | `OnLocalPlayerRemovedEvent` 언바인딩 누락 → `LocalPlayerRemovedHandle`로 대칭 복원 |
| `686b3d6` | **레이아웃 재생성 경로의 중복 엔트리 + 중복 바인딩** (치명, 잠복) |

**`686b3d6`이 고친 버그** — `NotifyPlayerAdded`가 `OnPlayerControllerSet`에 거는 람다에 결함이 둘 있었다.

- `AddWeakLambda`를 매번 다시 걸면서 `RemoveAll(this)` 가드가 없어, PostLogin 반복 시 구독이 누적된다.
- 람다가 부르던 `NotifyPlayerRemoved`는 배열 엔트리를 남기는데, 뒤따르는 `CreateLayoutWidget`이 무조건 `Emplace`한다. 같은 LocalPlayer 키로 엔트리가 둘이 되고 `GetRootLayout`의 `FindByKey`가 stale 레이아웃을 반환한다.

수정: `RemoveAll(this)` 추가 + 람다 본문을 `NotifyPlayerDestroyed`로 교체.

⚠️ **이 경로는 PIE로 검증할 수 없다.** `OnPlayerControllerSet.Broadcast()` 호출처가 저장소에 0건이라 발화하지 않는다. 정적 검증(콜드 빌드 + 리뷰어 단독 집중 리뷰)만 했다. 지금 고친 이유는 잠복 상태라 회귀 위험이 0이기 때문이다.

### 리뷰어가 독립적으로 찾은 것

선입견 없이 돌렸을 때 위 4건을 모두 짚었고, 특히 `686b3d6`을 최우선(무음 실패)으로 올렸다. Public/Private 미사용이나 플러그인의 Build.cs dep 배치를 "위반"으로 오탐하지 않았다. `686b3d6` 단독 리뷰와 누적 diff 최종 리뷰 모두 **통과**.

리뷰어가 추가로 짚은 것: `RemoveWidgetFromLayer`/`FindAndRemoveWidgetFromLayer`가 BP 노출 API인데 어떤 그래프도 쓰지 않는다(모든 화면이 `DeactivateWidget()` self-pop). 이름 계약이라 손대지 않고 후속 과제로 남겼다.

### 최종 검증

- **콜드 재빌드**(`Rebuild.bat`, 198 액션): `Result: Succeeded`. 에러 0. 경고 22건은 **전부 Monolith 플러그인 자체 소스**(C4996 deprecation), 우리 모듈 0건.
- **PIE 스모크**(`run_pie_smoke` → `poll_pie_smoke`): `ok:true`. Blueprint Runtime Error 0, Accessed None 0, teardown 클린.
- **라이브 위젯 트리**: `ui.get_activatable_stack_state(Layer_Menu)` → `stack_depth=1`, `active=WBP_TitleScreen_C`. subsystem→policy→layout 체인과 `UCuLocalPlayer` 캐스팅, 타이틀 자동 push 모두 회귀 없음.
- `list_errored_blueprints` 0건.

### 도구 사용 중 발견한 함정

- **`Binaries/` 접근이 deny 규칙으로 전면 차단된다.** `ls`·`glob`·`Test-Path` 모두 거부된다. 그래서 `.mcp.json`이 가리키는 `monolith_proxy.exe`의 존재 여부를 확인할 수 없다. 조사 에이전트가 이를 "死경로"로 단정했으나 **위양성**이었다(실제로 MCP 연결은 성공한다).
- **서브에이전트 세션에서는 `Saved/Logs` 접근이 PowerShell로도 막힌다.** 가이드는 메인 에이전트 기준으로 `Get-Content`가 허용된다고 적고 있으나, 리뷰어 세션에서는 거부됐다. 로그 열람이 필요하면 메인 세션에서 해야 한다.
- `editor.run_python`의 파라미터 이름은 `code`가 아니라 **`command`**다. `unreal` python 모듈에 `find_objects_of_class`는 없다.
