# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 언어 규칙

- 기본 언어는 **한국어**. 문서·설명·커밋 메시지·모든 입출력을 한국어로 작성한다.
- 기술 용어(Common UI, Activatable Widget, MVVM, Enhanced Input, GameplayTag, PrimaryGameLayout, GameUIPolicy 등)는 번역하지 말고 **영어 원문**을 그대로 사용한다.

## 이 저장소의 성격

원래 **스펙/프롬프트 저장소**이며, 여기에 실제 UE 프로젝트가 **하위폴더로 containment**되어 함께 산다(2026-07-08, 세션 A부터).

- `common-ui-starter-kit-prompt.md` — Claude Code가 Common UI 학습용 Starter Kit 프로젝트를 **빈 폴더에서부터 자동 생성**하도록 지시하는 프롬프트 문서. 이 저장소의 핵심 스펙 산출물이다.
- `docs/common-ui-coding-reference.md` — **코딩 레퍼런스 정본**(Common UI/CommonGame/MVVM/Input의 API idiom·함정·네이밍 + Epic C++ 표준 요약). 프롬프트가 "무엇을 만들지", 이 문서가 "어떻게 정확히 쓸지"를 담는다. 세션 B(C++)·세션 C(BP 로직)가 코드 작성 시 참조한다.
- **`CommonUIStarterKit/`** — 실제 UE 5.8 C++ 프로젝트(세션 A에서 생성, **containment**). 이 폴더에서 작업할 때의 가이드·세션 진행 상태·머신 quirk는 **`CommonUIStarterKit/CLAUDE.md`**를 본다.

> **Containment 결정 (2026-07-08)**: 사용자 요청으로 대상 UE 프로젝트를 별도 저장소가 아니라 이 저장소의 `CommonUIStarterKit/` 하위폴더에 뒀다. 하나의 git 저장소가 스펙 문서 + UE 프로젝트를 함께 담는다.
> - 프롬프트는 "프로젝트=저장소 루트"를 가정하므로, **루트 앵커 경로 패턴을 하위폴더용으로 적응**시켰다: `.claude/settings.json`의 deny `Read()`에 `/CommonUIStarterKit/...` 경로 추가, `.gitignore`에 중간 슬래시 패턴(`CommonUIStarterKit/Plugins/Monolith/` 등) 추가.
> - 코딩 레퍼런스는 하위폴더에 복사하지 않고 **루트 `docs/`의 원본을 그대로 참조**한다.
> - **세션 A 결과**: 플러그인 컴파일 게이트 빌드 **통과**(툴체인+CommonUI/CommonInput/MVVM/Monolith가 UE 5.8에서 링크). Monolith는 v0.20.3 UE5.8 빌드(BuildId 55116800 == 엔진 CL 일치).
> - **세션 B 결과**: C++ 3-class 뼈대(레이어링·base widgets·ViewModel 스텁)·GameplayTag·치명적 INI(`GameViewportClientClassName`+`LocalPlayerClassName`) 작성 → **빌드 성공**(우리 코드 에러 0). `.mcp.json`(루트, containment 경로) 생성, **에디터 실행 + Monolith MCP 서버 port 9316 확인**. 상세·다음 단계는 **`CommonUIStarterKit/CLAUDE.md`**.
> - **다음은 프롬프트 C**(에셋 Stage 1~6). ⚠️ **B→C는 Claude Code 재시작 필수**(`.mcp.json`이 세션 시작 시에만 로드됨). 에디터는 계속 켜 둘 것.
> - **머신 quirk**: UE 5.8이 `C:\UE\UE_5.8`(비표준)이고 HKLM에 5.8 미등록 → 엔진 도구는 항상 절대경로로 직접 호출. 상세는 `CommonUIStarterKit/CLAUDE.md`.

## 프롬프트 문서가 기술하는 대상 아키텍처 (big picture)

`common-ui-starter-kit-prompt.md`는 **세 세션** + 세션 C의 Stage 1~6 구조의 automation-first(+ 수동 fallback) 워크플로를 정의한다. **필수 재시작은 B→C(MCP 로드)뿐**이고, A→B는 컨텍스트·오류 격리를 위한 권장 체크포인트(재시작 불필요)다.

- **세션 A (프로젝트 + 플러그인)**: 에디터 없이 UE 5.8 C++ 프로젝트 `CommonUIStarterKit` 파일 생성 — plugin(**CommonUI / ModelViewViewModel / Monolith** 활성화; CommonInput은 CommonUI plugin 내 module이라 `.uproject`에 넣지 않고 Build.cs에만, EnhancedInput은 engine-default) + Monolith 설치 + **플러그인 컴파일 게이트 빌드**(빈 module로 툴체인+플러그인이 5.8에서 링크되는지 **C++ 작성 전** 검증).
- **세션 B (C++ 뼈대)**: 3-class 레이어링·base widgets·ViewModel 스텁·INI 작성 → C++ 빌드 → `.mcp.json` 생성 → 에디터 실행(port 9316). 끝에서 재시작.
- **세션 C (에셋)**: Claude Code 재시작 후 Monolith MCP(`monolith_discover()` / `monolith_guide()`)로 역량 probe → Stage 1~6 위젯·style·input 에셋을 **단계별로 생성하고 각 Stage마다 PIE로 검증**.

대상 프로젝트의 UI 아키텍처는 Lyra CommonGame **패턴을 학습용으로 재구현**한다(CommonGame 코드 복사가 아님 — CommonGame은 engine library가 아니라 Lyra 샘플 플러그인이므로). 진짜 엔진 라이브러리(CommonUI / CommonInput / EnhancedInput / UMG / ModelViewViewModel)는 그대로 재사용한다.

**충실한 3-class 레이어링 (반드시 유지):**

```
UCuGameUIManagerSubsystem  (player lifecycle, config DefaultUIPolicyClass)
    └─ UCuGameUIPolicy  (LayoutClass 소유, UCuLocalPlayer마다 레이아웃 1개 생성)
          └─ UCuPrimaryGameLayout  (GameplayTag별 UCommonActivatableWidgetStack 레이어 소유)
```

- 레이어 4개: `UI.Layer.Game` / `GameMenu` / `Menu` / `Modal`. `BindWidget` + `RegisterLayer(Tag, Stack)`로 tag→container 맵을 명시적으로 채운다.
- push는 `PushWidgetToLayerStack`(sync) / `PushWidgetToLayerStackAsync`(soft class). push가 자동 activate하므로 이중 `ActivateWidget` 금지.
- 입력 모드는 `GetDesiredInputConfig()` → `FUIInputConfig`(`ECommonInputMode::Menu`/`Game`/`All`; `GameAndMenu` 없음). `SetInputMode*` 직접 호출 금지.
- 메뉴 입력은 CommonUI **DataTable action 경로**(`FCommonInputActionDataBase` + `UCommonUIInputData` + `CommonInputSettings`). Enhanced Input은 gameplay 액션 전용.
- 설정 화면은 MVVM(`UCuSettingsViewModel : UMVVMViewModelBase`, FieldNotify + `UE_MVVM_SET_PROPERTY_VALUE`). View Binding은 수동/Experimental fallback 대상.
- **프로젝트 C++ 클래스는 `Cu` 프리픽스**(`UCu*`/`FCu*`/`ECu*`)를 쓰고, 소스는 `Public/Private` + 역할별 하위폴더(System/Layout/Widgets/Screens/ViewModels)로 구성한다(레퍼런스 §2).

## 치명적 셋업 (누락 주의)

`DefaultEngine.ini`에 반드시:
```ini
[/Script/Engine.Engine]
GameViewportClientClassName=/Script/CommonUI.CommonGameViewportClient
```
없으면 `UCommonUIActionRouterBase`가 입력을 못 받아 gamepad/Back 내비게이션이 **무음으로 전부 죽는다**(CommonUI 최다 셋업 실패 원인).

## 세션 간 재시작 제약 (중요)

`.mcp.json`은 Claude Code 세션 시작 시점에만 로드된다. 세션 B에서 생성해도 같은 세션에서는 Monolith 도구를 쓸 수 없으므로 **B→C 경계에서 반드시 Claude Code를 재시작**한 뒤 세션 C를 진행한다(A→B는 재시작 불필요). Monolith는 UE 5.7 & 5.8 지원, `monolith_proxy.exe`, port 9316. 서버 신뢰는 `.claude/settings.json`의 `enabledMcpjsonServers: ["monolith"]`로 미리 승인되어 재시작 후 프롬프트 없이 로드된다.

## 권한(allowlist) 설정 — 2파일 분리

프롬프트 실행을 매끄럽게 하려고 권한을 두 파일로 나눈다. **핵심 이유**: committed `settings.json`의 `allow`는 **workspace 신뢰 수락 후에만** 적용(그전엔 계속 프롬프트)되지만, **gitignore된 `settings.local.json`의 `allow`는 신뢰 없이 즉시 적용**된다(본인 파일로 취급). 그래서 권한 부여는 local, 정책은 committed에 둔다.

- **`.claude/settings.local.json`** (gitignore됨) — `allow`: `Bash`/`PowerShell`(shell 전체, `python` 포함 — 이 환경은 PowerShell이 primary), `Read`/`Glob`/`Grep`, `Edit`/`Write`, `mcp__monolith`. **신뢰 없이 즉시 적용 → 매번 안 물어봄.**
- **`.claude/settings.json`** (committed) — 정책·안전, 신뢰 무관하게 적용:
  - `deny`(`Read()`): **UE 생성물 노이즈 차단** — `Binaries`·`Intermediate`·`DerivedDataCache`·`Saved`(통째)·플러그인 `Binaries`/`Intermediate`. `Read()` deny는 Read·Grep·Glob·`cat`/`head`/`tail`/`sed`까지 적용.
  - `deny`(shell): `rm -rf /`·홈 삭제·git force-push 등 파괴적/비가역 명령만 차단(안전망). 일반 명령(python 등)은 막지 않음.
  - `enabledMcpjsonServers: ["monolith"]`: 프로젝트 스코프 MCP 서버 사전 신뢰.

**로그 진단** — `Saved/`가 Read deny 대상이라 빌드·에디터 로그는 PowerShell `Get-Content Saved/Logs/*.log | Select-String LogMonolith`로 읽는다(`Read()` deny는 PowerShell `Get-Content`을 막지 않음). `Read` 도구로 열어야 하면 `Read(/Saved/**)`만 잠시 제거.

**`.claudeignore`는 쓰지 않는다** — Claude Code 정식 기능이 아니라 강제되지 않는다. 노이즈 제외는 위 `deny Read()`(강제됨)와 `.gitignore`(발견 단계 제외)로 처리한다.

동일 템플릿이 `common-ui-starter-kit-prompt.md`의 **Step A-0**에 있으며, 프롬프트 실행 대상 폴더에서도 가장 먼저 이 파일을 만들도록 지시한다. 광범위 allow는 "본인 머신에서 본인 UE 프로젝트를 빌드하는 로컬 자동화"라는 전제에서의 선택이다.

## 빌드 / 테스트

현재 저장소에는 빌드 시스템·테스트가 없다(마크다운 문서뿐). 대상 UE 프로젝트의 빌드·실행 절차는 `common-ui-starter-kit-prompt.md`의 **Step A-4**(플러그인 게이트 빌드)·**Step B-3**(C++ 뼈대 빌드)·**Step B-4**(에디터 실행), 검증은 세션 C 각 Stage의 PIE smoke test에 정의되어 있다.

## Monolith MCP 사용 규칙 (세션 C)

- `monolith_discover()` / `monolith_guide()`를 우선 호출하고, 액션 파라미터를 **추측하지 않는다**(discover/guide 결과 기준, 오류 시 `did_you_mean` 활용).
- 불확실한 C++ 시그니처는 `source` 네임스페이스(`get_signature`/`verify_symbols`/`check_deprecations`)로 확인.
- 파괴적 액션(에셋 삭제) 금지. 자동화 불가 항목은 건너뛰지 말고 수동 체크리스트로 정리.
