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
│  └─ CommonUIStarterKit/
│     ├─ CommonUIStarterKit.Build.cs      # dep: Core..CommonUI/CommonInput/EnhancedInput/UMG/Slate/MVVM/GameplayTags
│     ├─ CommonUIStarterKit.h             # 최소 primary game module (빈 Startup/Shutdown)
│     └─ CommonUIStarterKit.cpp           # IMPLEMENT_PRIMARY_GAME_MODULE
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

## 다음 세션 B에서 할 일 (C++ 3-class 뼈대)

프롬프트 B(`../common-ui-starter-kit-prompt.md` 프롬프트 B) 기준:

1. **C++ 3-class 레이어링 작성** (레퍼런스 §6):
   `UGameUIManagerSubsystem`(GameInstanceSubsystem) → `UGameUIPolicy`(UObject) → `UPrimaryGameLayout`(UCommonUserWidget) + `UCommonLocalPlayer`.
2. **Base widgets**: `UStarterActivatableWidget`(`GetDesiredInputConfig()` override, `ECommonInputMode::Menu`), `UStarterButtonBase`, 화면별 베이스(Title/MainMenu/Settings/Pause/ConfirmationModal).
3. **ViewModel 스텁**: `USettingsViewModel : UMVVMViewModelBase` (FieldNotify + `UE_MVVM_SET_PROPERTY_VALUE`; 볼륨은 SoundClass/Mix — `UGameUserSettings` 아님).
4. **GameplayTag** 네이티브 선언: `UI.Layer.Game/GameMenu/Menu/Modal`.
5. **INI 실제 값 채우기** (Step B-2) — DefaultEngine.ini의 **`GameViewportClientClassName=/Script/CommonUI.CommonGameViewportClient`** [치명적, 절대 누락 금지], Local Player Class, DefaultUIPolicyClass.
   - ⚠️ **INI 파일 배치 충돌 주의**: `CommonInputSettings.InputData`는 프롬프트 B-2가 `DefaultGame.ini`를 언급하지만 **레퍼런스 §4는 `DefaultEngine.ini`를 정본**으로 본다. 세션 B는 **§4 기준(DefaultEngine.ini)**으로 확정할 것.
6. **C++ 빌드**(위 게이트 빌드 명령 재사용) → 성공 확인.
7. **`.mcp.json` 생성** (위 containment 경로 규칙 준수) → **에디터 실행(port 9316)**.
8. 끝에서 **Claude Code 재시작**(B→C 경계, MCP 로드) 후 세션 C 진행.

## 함정 빠른참조 (레퍼런스 §10 발췌)

- `GameViewportClientClassName` 미설정 → gamepad/focus/Back 무음 사망.
- `SetInputMode*` 직접 호출 금지 → `GetDesiredInputConfig()` override.
- `ECommonInputMode::GameAndMenu` 없음(=`All`).
- FieldNotify setter 직접 대입 금지 → `UE_MVVM_SET_PROPERTY_VALUE`.
- `CommonInput`/`EnhancedInput`을 `.uproject` plugin으로 넣지 말 것 → Build.cs dep으로만.
- push 후 `ActivateWidget` 재호출 금지 · `RegisterLayer`는 `NativeOnInitialized`에서 명시 · BindWidget 이름 엄격 일치.
