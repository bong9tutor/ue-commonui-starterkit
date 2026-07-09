# UE Common UI Starter Kit — Prompt Kit

**Claude Code + Monolith MCP로 Unreal Engine 5.8용 Common UI 학습 Starter Kit을 빈 폴더에서 자동 생성하는 "프롬프트 킷"입니다.**

![Unreal Engine 5.8](https://img.shields.io/badge/Unreal%20Engine-5.8-0E1128?logo=unrealengine&logoColor=white)
![Claude Code](https://img.shields.io/badge/Claude%20Code-agentic-D97757)
![Monolith MCP](https://img.shields.io/badge/Monolith-MCP-333333)
![Common UI](https://img.shields.io/badge/Common%20UI-CommonGame%20%C2%B7%20MVVM-2E7D32)
![Docs](https://img.shields.io/badge/lang-%ED%95%9C%EA%B5%AD%EC%96%B4-blue)

> ⚠️ **이 저장소는 완성된 UE 프로젝트가 아닙니다.** UE5 Common UI 학습 프로젝트를 **처음부터 자동으로 만들어내도록** Claude Code에게 지시하는 프롬프트·스펙·레퍼런스 모음입니다. 실제 `CommonUIStarterKit` 프로젝트는 프롬프트를 실행하면 대상 폴더에 생성됩니다.

---

## ✨ 무엇인가요?

Unreal Engine의 **Common UI**를 제대로 배우려면 Activatable Widget, layer stack, input routing, style asset, MVVM, CommonGame 레이어링을 한꺼번에 이해해야 해서 진입장벽이 높습니다. 이 킷은 그 과정을:

- **"바퀴를 재발명하지 않고"** — 엔진 라이브러리(Common UI / CommonInput / Enhanced Input / UMG / MVVM)를 그대로 재사용하고, Lyra의 `CommonGame`은 **코드 복사 대신 패턴만 학습용으로 재구현**
- **단계별(Stage 1~6)로 쌓으며, 각 단계마다 PIE로 검증**하고
- **automation-first + 문서화된 수동 fallback** 전략으로 (Monolith MCP가 못 하는 것만 수동 체크리스트로)

Claude Code가 대신 진행하도록 설계했습니다. 검증된 최신 API·함정을 문서에 정본으로 담아, 흔한 Common UI 셋업 실패(입력이 "무음"으로 죽는 등)를 사전에 차단합니다.

## 🎯 생성되는 프로젝트가 가르치는 것

- **Common UI 핵심** — `UCommonActivatableWidget` / `UCommonActivatableWidgetStack`, `UCommonButtonBase`, style asset(`CommonButtonStyle`/`TextStyle`/`BorderStyle`), `UCommonBoundActionBar`
- **충실한 3-class 레이어링** (Lyra CommonGame 패턴 재구현)
  ```
  UGameUIManagerSubsystem  →  UGameUIPolicy  →  UPrimaryGameLayout
  (player lifecycle)          (per-player 생성)   (GameplayTag별 layer stack)
  ```
- **GameplayTag 기반 4-layer** (`UI.Layer.Game` / `GameMenu` / `Menu` / `Modal`)와 push/pop 내비게이션
- **CommonInput DataTable 입력 경로** + 멀티플랫폼 버튼 glyph, Back/Confirm action routing
- **MVVM 설정 화면** (`UMVVMViewModelBase` + FieldNotify)
- 결과 흐름: **타이틀 → 메인 메뉴 → 설정 → 일시정지 → 확인 모달**

## 🧩 3-세션 워크플로

`.mcp.json`이 세션 시작 시에만 로드되는 제약 때문에 작업을 세 단계로 나눕니다. **재시작이 필수인 경계는 B→C뿐**입니다.

| 세션 | 하는 일 | 산출 |
|------|---------|------|
| **A — 프로젝트 + 플러그인** | `.uproject`/`Build.cs` 생성, Monolith 설치, **플러그인 컴파일 게이트 빌드**(C++ 작성 전 링크 검증) | 빌드 통과한 빈 프로젝트 |
| **B — C++ 코드베이스 뼈대** | 3-class 레이어링·base widgets·ViewModel·INI 작성 → C++ 빌드 → `.mcp.json` → 에디터 실행 | 컴파일된 C++ 뼈대 |
| **C — 에셋 생성** *(재시작 후)* | Monolith MCP로 Stage 1~6 Blueprint·style·input 에셋 생성 + 각 Stage PIE 검증 | 동작하는 UI 흐름 + README |

## 🚀 사용법

### 사전 요구사항

- **Unreal Engine 5.8** (또는 5.7) + **Visual Studio** (C++ 게임 개발 워크로드)
- **.NET 10 SDK (x64)** — IDE(Rider / Visual Studio)에서 `.uproject`를 열 때 필요 (아래 note 참고)
- **[Claude Code](https://claude.com/claude-code)**
- **[Monolith MCP](https://github.com/tumourlove/monolith)** (UE 5.7 & 5.8 지원, MIT) — 세션 A에서 설치
- git

> **ℹ️ .NET 10 SDK — IDE 프로젝트 로딩에 필요** — UE 5.8의 UnrealBuildTool(UBT)은 **.NET 10**을 타깃합니다. 커맨드라인 빌드(`Build.bat` / `RunUBT`)는 **엔진에 번들된 .NET 10**을 써서 별도 설치 없이 동작하지만, **Rider·Visual Studio는 시스템 `dotnet`으로 UBT를 실행**하므로 시스템에 .NET 10이 없으면 `.uproject` 로딩이 다음 오류로 실패합니다:
> `You must install or update .NET to run this application. … Framework: 'Microsoft.NETCore.App', version '10.0.0'`
> winget으로 설치하세요(설치 중 UAC 승인이 필요할 수 있음). `-e --id`로 패키지를 정확히 지정합니다:
> ```powershell
> winget install -e --id Microsoft.DotNet.SDK.10 --architecture x64
> # 런타임만 필요하면(UBT엔 이걸로도 충분): winget install -e --id Microsoft.DotNet.Runtime.10 --architecture x64
> ```
> (`--scope machine`을 붙이면 패키지가 해당 스코프를 지원하지 않아 설치가 진행되지 않을 수 있으니 위 형태를 사용합니다.)
> 설치 확인 후 IDE 재시작: `dotnet --list-runtimes | Select-String 'NETCore.App 10'` (기존 .NET 8/9와 공존하며 영향 없음).

### 실행

```text
1. 빈 작업 폴더에서 Claude Code 실행
2. 프롬프트 A 전달  → 프로젝트 + 플러그인 (프로젝트+플러그인 컴파일 게이트)
3. 프롬프트 B 전달  → C++ 뼈대 + .mcp.json + 에디터 실행
4. Claude Code 재시작 (B→C 필수)
5. 프롬프트 C 전달  → Monolith MCP로 Stage 1~6 에셋 생성·검증
```

> 프롬프트 A / B / C의 전문은 **[`docs/common-ui-starter-kit-prompt.md`](docs/common-ui-starter-kit-prompt.md)** 에 있습니다. 권한 프롬프트를 줄이려면 실행 **전에** `.claude/settings.json`(+ `settings.local.json`)을 대상 폴더에 미리 두는 것을 권장합니다.

## 📁 저장소 구성

```text
.
├── docs/
│   ├── common-ui-starter-kit-prompt.md   # ⭐ 3-세션 생성 프롬프트 (핵심 산출물)
│   └── common-ui-coding-reference.md     # 코딩 레퍼런스 정본 (API idiom·함정·네이밍)
├── CommonUIStarterKit/                   # 실제 UE 5.8 프로젝트 (containment)
│   └── docs/
│       ├── project-guide.md              # 머신 환경·코드 구조·확정 결정·함정
│       └── session-log.md                # 세션 A/B/C 진행 기록
├── CLAUDE.md                             # Claude Code용 저장소 가이드 (유일한 자동 로드 지시문)
├── .claude/agents/                       # 서브에이전트: 코드 리뷰어 · 리팩토러
├── .claude/settings.json                 # 권한 allowlist (deny·MCP 신뢰; committed)
└── .gitignore                            # UE5 프로젝트 표준
```

## 🏛️ 설계 원칙

- **엔진 라이브러리 재사용** — `CommonGame`은 engine plugin이 아니라 Lyra 샘플 플러그인이므로, 코드 복사 대신 얇은 orchestration 계층만 교육용으로 재구현
- **automation-first + 수동 fallback** — Monolith가 못 하는 항목(특히 MVVM View Binding)은 건너뛰지 않고 수동 체크리스트로
- **단계별 + PIE 검증** — Stage 1~6, 각 Stage는 이전 Stage의 PIE 통과를 전제
- **코드는 레퍼런스 정본을 먼저** — 반복 idiom·함정·네이밍은 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md) 참조

## 📚 문서

- **[생성 프롬프트](docs/common-ui-starter-kit-prompt.md)** — 세션 A/B/C 전문
- **[코딩 레퍼런스](docs/common-ui-coding-reference.md)** — Common UI/CommonGame/MVVM/Input의 API idiom·함정(§10 체크리스트)·네이밍 + Epic C++ 표준 요약
- **[프로젝트 가이드](CommonUIStarterKit/docs/project-guide.md)** — UE 프로젝트를 건드리기 전에 읽을 것: 머신 환경·코드 구조·확정 결정·함정
- **[세션 로그](CommonUIStarterKit/docs/session-log.md)** — 세션 A/B/C와 Stage 0~6 진행 기록
- **[CLAUDE.md](CLAUDE.md)** — containment 구조·세션 재시작 제약·권한 설정·문서 지도

## ⚠️ 알아둘 점

- **Monolith MCP는 제3자 플러그인**입니다([tumourlove/monolith](https://github.com/tumourlove/monolith), MIT). 별도 설치·신뢰가 필요하고, 설치본이 대상 엔진 버전(5.8)을 지원하는지 세션 A에서 확인합니다(안 되면 5.7 fallback).
- **`.mcp.json`은 세션 시작 시에만 로드**되므로 B→C 사이 Claude Code 재시작이 필요합니다.
- MVVM **View Binding** 등 일부 저작은 에디터 패널이 정식 경로라 **수동 fallback** 대상입니다.
- 기본 언어는 **한국어**, 기술 용어는 **영어 원문**을 사용합니다.

## 🙏 참고 / 크레딧

- Epic Games — [Common UI](https://dev.epicgames.com/documentation/en-us/unreal-engine/common-ui-quickstart-guide-for-unreal-engine) · [Lyra `CommonGame`](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine) · [UMG ViewModel(MVVM)](https://dev.epicgames.com/documentation/en-us/unreal-engine/umg-viewmodel-for-unreal-engine)
- [Monolith](https://github.com/tumourlove/monolith) — UE editor automation MCP
- 커뮤니티: [benui.ca](https://benui.ca/) · [x157 UE5 노트](https://x157.github.io/UE5/)
