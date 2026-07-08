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
- **[Claude Code](https://claude.com/claude-code)**
- **[Monolith MCP](https://github.com/tumourlove/monolith)** (UE 5.7 & 5.8 지원, MIT) — 세션 A에서 설치
- git

### 실행

```text
1. 빈 작업 폴더에서 Claude Code 실행
2. 프롬프트 A 전달  → 프로젝트 + 플러그인 (프로젝트+플러그인 컴파일 게이트)
3. 프롬프트 B 전달  → C++ 뼈대 + .mcp.json + 에디터 실행
4. Claude Code 재시작 (B→C 필수)
5. 프롬프트 C 전달  → Monolith MCP로 Stage 1~6 에셋 생성·검증
```

> 프롬프트 A / B / C의 전문은 **[`common-ui-starter-kit-prompt.md`](common-ui-starter-kit-prompt.md)** 에 있습니다. 권한 프롬프트를 줄이려면 실행 **전에** `.claude/settings.json`(+ `settings.local.json`)을 대상 폴더에 미리 두는 것을 권장합니다.

## 📁 저장소 구성

```text
.
├── common-ui-starter-kit-prompt.md   # ⭐ 3-세션 생성 프롬프트 (핵심 산출물)
├── docs/
│   └── common-ui-coding-reference.md # 코딩 레퍼런스 정본 (API idiom·함정·네이밍)
├── CLAUDE.md                         # Claude Code용 저장소 가이드
├── .claude/settings.json             # 권한 allowlist (deny·MCP 신뢰; committed)
└── .gitignore                        # UE5 프로젝트 표준
```

## 🏛️ 설계 원칙

- **엔진 라이브러리 재사용** — `CommonGame`은 engine plugin이 아니라 Lyra 샘플 플러그인이므로, 코드 복사 대신 얇은 orchestration 계층만 교육용으로 재구현
- **automation-first + 수동 fallback** — Monolith가 못 하는 항목(특히 MVVM View Binding)은 건너뛰지 않고 수동 체크리스트로
- **단계별 + PIE 검증** — Stage 1~6, 각 Stage는 이전 Stage의 PIE 통과를 전제
- **코드는 레퍼런스 정본을 먼저** — 반복 idiom·함정·네이밍은 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md) 참조

## 📚 문서

- **[생성 프롬프트](common-ui-starter-kit-prompt.md)** — 세션 A/B/C 전문
- **[코딩 레퍼런스](docs/common-ui-coding-reference.md)** — Common UI/CommonGame/MVVM/Input의 API idiom·함정(§10 체크리스트)·네이밍 + Epic C++ 표준 요약
- **[CLAUDE.md](CLAUDE.md)** — 아키텍처·세션 재시작 제약·권한 설정 요약

## ⚠️ 알아둘 점

- **Monolith MCP는 제3자 플러그인**입니다([tumourlove/monolith](https://github.com/tumourlove/monolith), MIT). 별도 설치·신뢰가 필요하고, 설치본이 대상 엔진 버전(5.8)을 지원하는지 세션 A에서 확인합니다(안 되면 5.7 fallback).
- **`.mcp.json`은 세션 시작 시에만 로드**되므로 B→C 사이 Claude Code 재시작이 필요합니다.
- MVVM **View Binding** 등 일부 저작은 에디터 패널이 정식 경로라 **수동 fallback** 대상입니다.
- 기본 언어는 **한국어**, 기술 용어는 **영어 원문**을 사용합니다.

## 🙏 참고 / 크레딧

- Epic Games — [Common UI](https://dev.epicgames.com/documentation/en-us/unreal-engine/common-ui-quickstart-guide-for-unreal-engine) · [Lyra `CommonGame`](https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine) · [UMG ViewModel(MVVM)](https://dev.epicgames.com/documentation/en-us/unreal-engine/umg-viewmodel-for-unreal-engine)
- [Monolith](https://github.com/tumourlove/monolith) — UE editor automation MCP
- 커뮤니티: [benui.ca](https://benui.ca/) · [x157 UE5 노트](https://x157.github.io/UE5/)
