# CommonUIStarterKit (UE 5.8)

Common UI 학습용 **Starter Kit**. Lyra의 **CommonGame 패턴을 학습용으로 재구현**(코드 복사가 아님)하고, 진짜 엔진 라이브러리(CommonUI / CommonInput / EnhancedInput / UMG / ModelViewViewModel)는 그대로 재사용한다. 전 과정을 **3-세션 automation-first 워크플로**로 만들었다.

> 기술 용어(Common UI, Activatable Widget, MVVM, GameUIPolicy, PrimaryGameLayout 등)는 영어 원문 그대로 사용한다.

---

## 3-세션 워크플로

| 세션 | 내용 | 결과 |
|------|------|------|
| **A — 프로젝트 + 플러그인** | UE 5.8 C++ 프로젝트 생성, CommonUI/ModelViewViewModel/Monolith 활성화, 플러그인 컴파일 게이트 빌드 | 툴체인+플러그인 링크 검증 |
| **B — C++ 뼈대** | 3-class 레이어링·base widgets·ViewModel 스텁·치명적 INI 작성 → 빌드 → `.mcp.json` | C++ 빌드 성공, Monolith MCP(port 9316) 준비 |
| **C — 에셋 (Stage 0~6)** | Monolith MCP로 레벨·위젯·style·input·MVVM 에셋 생성, 각 Stage PIE 검증 | 아래 참조 |

---

## 세션 C — Stage 0~6 학습 순서

- **Stage 0 — 프레임워크 부트스트랩**: Empty 레벨 `L_StarterKit` + `BP_StarterGameMode`(PlayerControllerClass·DefaultPawnClass=None) + `BP_StarterPlayerController`(bShowMouseCursor) + World Settings GameModeOverride + 기본 맵 INI. → **프로젝트 시작 시 L_StarterKit 자동 오픈**.
- **Stage 1 — Activatable "hello world"**: `WBP_StarterActivatable`(activate/deactivate lifecycle, `GetDesiredInputConfig`=Menu).
- **Stage 2 — 레이아웃/정책**: `WBP_PrimaryGameLayout`(레이어 스택) + `BP_StarterUIPolicy`(LayoutClass) + `DefaultUIPolicyClass` INI. subsystem→policy가 PostLogin에 반응해 레이아웃을 per-LocalPlayer로 생성.
- **Stage 3 — navigation + style**: `CBS_*`/`CTS_*` style, `WBP_ButtonBase`, `WBP_TitleScreen`→`WBP_MainMenu` push, `GetDesiredFocusTarget` override.
- **Stage 4 — input routing (CommonUI DataTable 경로)**: `DT_UIActions`(Back/Accept) + `DA_CommonInputData`(DefaultClick/BackAction) + `CommonInputSettings.InputData` INI + `UCommonBoundActionBar`(플랫폼 glyph 자동 표시).
- **Stage 5 — layers + modal**: `WBP_PrimaryGameLayout`을 4-layer(`Game`/`GameMenu`/`Menu`/`Modal`)로 확장 + `WBP_PauseMenu` + `WBP_ConfirmationModal`(Confirm/Cancel, `bIsBackHandler`, focus 복귀).
- **Stage 6 — MVVM 설정 (캡스톤)**: `WBP_SettingsScreen`(Slider→라벨 갱신) + `UCuSettingsViewModel`(C++, FieldNotify + `UE_MVVM_SET_PROPERTY_VALUE`). ⚠️ **선언적 MVVM View Binding은 수동/Experimental fallback**(아래 참조).

**내비게이션 흐름**: Title → (Start) → MainMenu → { Start→PauseMenu / Settings→SettingsScreen / Quit→종료 }; PauseMenu → { Resume→pop / Quit→ConfirmationModal → Confirm=종료 · Cancel=pop }.

---

## C++ 클래스 (Cu 프리픽스)

3-class 레이어링(반드시 유지):
```
UCuGameUIManagerSubsystem  (GameInstanceSubsystem; config DefaultUIPolicyClass; GameMode PostLogin에 바인딩)
  └─ UCuGameUIPolicy        (LayoutClass 소유; UCuLocalPlayer마다 레이아웃 1개 생성 → AddToPlayerScreen)
       └─ UCuPrimaryGameLayout (GameplayTag별 UCommonActivatableWidgetStack 레이어 소유; RegisterLayer)
```
- `System/`: `CuGameUIManagerSubsystem`, `CuGameUIPolicy`, `CuLocalPlayer`
- `Layout/`: `CuPrimaryGameLayout`, `CuGameplayTags`(`UI.Layer.Game/GameMenu/Menu/Modal`)
- `Widgets/`: `CuActivatableWidget`, `CuButtonBase`
- `Screens/`: `CuTitleScreen`/`CuMainMenu`/`CuSettingsScreen`/`CuPauseMenu`/`CuConfirmationModal`Widget
- `ViewModels/`: `CuSettingsViewModel`

**세션 C에서 추가한 C++ (BP 내비게이션 활성화)** — `UCuPrimaryGameLayout`:
- `UFUNCTION(BlueprintCallable) PushWidgetToLayer(FGameplayTag, TSubclassOf<UCommonActivatableWidget>)` — template push의 BP 래퍼(엔진 template은 BP 미노출).
- `UFUNCTION(BlueprintCallable) RemoveWidgetFromLayer(...)`, `BlueprintPure GetLayerTag_Game/GameMenu/Menu/Modal`, `static GetPrimaryGameLayoutForPlayer(APlayerController*)`.

---

## 생성된 에셋 (세션 C)

| 경로 | 에셋 |
|------|------|
| `/Game/Maps` | `L_StarterKit` |
| `/Game/Core` | `BP_StarterGameMode`, `BP_StarterPlayerController` |
| `/Game/UI/Foundation` | `WBP_PrimaryGameLayout`, `BP_StarterUIPolicy`, `WBP_StarterActivatable`, `WBP_ButtonBase`, `WBP_ActionBarButton` |
| `/Game/UI/Menu` | `WBP_TitleScreen`, `WBP_MainMenu`, `WBP_PauseMenu`, `WBP_ConfirmationModal`, `WBP_SettingsScreen` |
| `/Game/UI/Style` | `CBS_Default`, `CBS_Primary`, `CTS_Header`, `CTS_Body`, `CTS_Button` |
| `/Game/Input` | `DT_UIActions`, `DA_CommonInputData` |

---

## Lyra CommonGame 대응표

| 이 Starter Kit (재구현) | 실제 Lyra / 엔진 | 성격 |
|---|---|---|
| `UCuGameUIManagerSubsystem` | `UGameUIManagerSubsystem`(CommonGame) | player lifecycle — **재구현** |
| `UCuGameUIPolicy` | `UGameUIPolicy`(CommonGame) | LayoutClass·per-player 생성 — **재구현** |
| `UCuPrimaryGameLayout` | `UPrimaryGameLayout`(CommonGame) | 레이어 스택 소유 — **재구현** |
| `UCuLocalPlayer` | `UCommonLocalPlayer`(CommonGame) | per-player identity — **재구현** |
| `PushWidgetToLayer`(BP 래퍼) | `UCommonUIExtensions::PushContentToLayer` | BP push 진입점 — **재구현** |
| `UCommonActivatableWidget/Stack` · `UCommonButtonBase` · `UCommonBoundActionBar` | 동일(CommonUI) | **엔진 재사용** |
| `FCommonInputActionDataBase` · `UCommonUIInputData` | 동일(CommonInput) | **엔진 재사용** |
| `UMVVMViewModelBase` | 동일(ModelViewViewModel) | **엔진 재사용** |
| (미구현) | `UUIExtensionSubsystem` / `UGameFeatureAction_AddWidgets` | Stage 7 "다음 단계" |

---

## 실행 방법

머신 quirk: UE 5.8이 비표준 경로(`C:\UE\UE_5.8`)이고 HKLM 미등록 → 엔진 도구는 절대경로로 직접 호출.

- **빌드**: `& "C:\UE\UE_5.8\Engine\Build\BatchFiles\Build.bat" CommonUIStarterKitEditor Win64 Development -project="<경로>\CommonUIStarterKit.uproject" -waitmutex`
- **에디터**: `& "C:\UE\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "<경로>\CommonUIStarterKit.uproject"` → 시작 시 `L_StarterKit` 자동 오픈.
- **플레이**: PIE 실행 → 타이틀 화면 자동 표시. ⚠️ **에디터 in-viewport PIE는 뷰포트를 클릭해야 gamepad/키보드 포커스가 붙는다**(standalone에선 자동). 마우스로도 내비게이션 가능.

---

## 검증 상태

- ✅ 요구사항 1·2(전용 레벨 + GameMode/PC + 기본 맵 자동 오픈) — 에디터 재시작으로 실증.
- ✅ 정책→레이아웃→타이틀 자동 push, 4-layer 스택에 Title/MainMenu/Pause/Modal 렌더(각 레이어·z-order) — PIE 실증, errored blueprint 0, error-level 로그 0.
- ✅ 모든 WBP/BP 컴파일 클린.

### 알려진 수동 fallback (설계상)
- **선언적 MVVM View Binding**(`UCuSettingsViewModel.MasterVolume` ↔ Slider/Label): 프로그램적 저작은 Experimental(`unreal.MVVMView`) 뿐이라 자동화하지 않음. `WBP_SettingsScreen`은 대체로 **Slider `OnValueChanged` → 라벨 갱신**(BP 그래프)으로 구현. 정식 View Binding은 View Bindings 에디터 패널에서 수동 연결(뷰모델 클래스는 C++로 준비됨).
- **버튼 클릭/Back의 실입력 내비게이션**: 그래프는 컴파일·배선으로 검증. 실입력(gamepad/키보드) 라우팅은 **standalone 실행 또는 뷰포트 포커스 상태**에서 확인 권장(CommonInputSettings.InputData는 에디터 재시작으로 로드됨).

---

## Stage 7 — 다음 단계 (이 kit에서는 빌드하지 않음)

- `UUIExtensionSubsystem`(HUD extension point) + `UGameFeatureAction_AddWidgets`로 GameFeature/Experience가 레이아웃을 몰라도 UI를 주입하는 **모듈러 UI** 패턴.
- ⚠️ 4개 navigation layer(스택)와 혼동 금지: layer stack = "화면 내비게이션", extension point = "모듈러 주입".
