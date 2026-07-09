# CLAUDE.md

이 저장소에서 작업할 때의 가이드. **이 파일이 유일하게 자동 로드되는 지시문**이다. 나머지 문서는 필요할 때 열어서 읽는다.

## 언어 규칙

- 기본 언어는 **한국어**. 문서·설명·커밋 메시지·모든 입출력을 한국어로 작성한다.
- 기술 용어(Common UI, Activatable Widget, MVVM, Enhanced Input, GameplayTag, PrimaryGameLayout, GameUIPolicy 등)는 번역하지 말고 **영어 원문**을 그대로 사용한다.

## 이 저장소의 성격

원래 **스펙/프롬프트 저장소**이며, 여기에 실제 UE 5.8 프로젝트가 **하위폴더로 containment**되어 함께 산다(2026-07-08 결정). 하나의 git 저장소가 스펙 문서 + UE 프로젝트를 함께 담는다.

**저장소 루트와 UE 프로젝트 루트(`CommonUIStarterKit/`)는 다르다.** 경로를 섞지 말 것.

## 문서 지도

각 주제의 **정본은 한 곳뿐**이다. 사실을 이 파일에 복사하지 말고, 아래에서 찾아 읽는다.

| 문서 | 담당 |
|---|---|
| [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md) | **코딩 규칙 정본.** Common UI/CommonGame/MVVM/Input의 API idiom·함정·네이밍 + Epic C++ 표준 (§1~§10) |
| [`docs/common-ui-starter-kit-prompt.md`](docs/common-ui-starter-kit-prompt.md) | **스펙 산출물.** 빈 폴더에서 Starter Kit을 자동 생성하는 프롬프트(세션 A/B/C) |
| [`CommonUIStarterKit/docs/project-guide.md`](CommonUIStarterKit/docs/project-guide.md) | UE 프로젝트의 머신 환경·코드 구조·확정 결정·함정 |
| [`CommonUIStarterKit/docs/session-log.md`](CommonUIStarterKit/docs/session-log.md) | 세션 A/B/C 진행 기록 (히스토리) |
| [`CommonUIStarterKit/README.md`](CommonUIStarterKit/README.md) | 사람이 읽는 프로젝트 소개 |

프롬프트가 "무엇을 만들지", 코딩 레퍼런스가 "어떻게 정확히 쓸지"를 담는다. 문서 간 충돌이 보이면 **정본을 따르고, 충돌 사실 자체를 보고**한다.

## UE 프로젝트를 건드리기 전에

`CommonUIStarterKit/`의 코드·Config·빌드를 건드리기 전에 **[`CommonUIStarterKit/docs/project-guide.md`](CommonUIStarterKit/docs/project-guide.md)를 먼저 읽는다.**

엔진 경로·빌드 명령·에디터와 Monolith MCP의 관계·치명적 INI 설정을 **추측하지 않는다.** 이 프로젝트에는 추측하면 조용히 실패하는 함정이 여럿 있고, 전부 그 문서에 적혀 있다.

## Containment 구조 (경로 앵커 주의)

프롬프트는 "프로젝트 = 저장소 루트"를 가정하므로, 루트 앵커 경로 패턴을 하위폴더용으로 적응시켰다.

- `.claude/settings.json`의 deny `Read()`에 `/CommonUIStarterKit/...` 경로 추가.
- `.gitignore`에 중간 슬래시 패턴(`CommonUIStarterKit/Plugins/Monolith/` 등) 추가.
- 코딩 레퍼런스는 하위폴더에 복사하지 않고 **루트 `docs/`의 원본을 그대로 참조**한다.
- `.mcp.json`은 **저장소 루트**에 둔다(Claude Code가 워크스페이스 루트의 것만 로드). `command`에는 containment 접두어가 붙는다.

## 세션 간 재시작 제약 (중요)

`.mcp.json`은 Claude Code **세션 시작 시점에만** 로드된다. 세션 B에서 생성해도 같은 세션에서는 Monolith 도구를 쓸 수 없으므로 **B→C 경계에서 반드시 Claude Code를 재시작**한 뒤 세션 C를 진행한다(A→B는 재시작 불필요).

서버 신뢰는 `.claude/settings.json`의 `enabledMcpjsonServers: ["monolith"]`로 미리 승인되어 재시작 후 프롬프트 없이 로드된다. **에디터는 계속 켜 둔다**(끄면 port 9316 서버가 내려간다).

## 권한(allowlist) 설정 — 2파일 분리

프롬프트 실행을 매끄럽게 하려고 권한을 두 파일로 나눈다. **핵심 이유**: committed `settings.json`의 `allow`는 **workspace 신뢰 수락 후에만** 적용(그전엔 계속 프롬프트)되지만, **gitignore된 `settings.local.json`의 `allow`는 신뢰 없이 즉시 적용**된다(본인 파일로 취급). 그래서 권한 부여는 local, 정책은 committed에 둔다.

- **`.claude/settings.local.json`** (gitignore됨) — `allow`: `Bash`/`PowerShell`(shell 전체), `Read`/`Glob`/`Grep`, `Edit`/`Write`, `mcp__monolith`. 신뢰 없이 즉시 적용된다.
- **`.claude/settings.json`** (committed) — 정책·안전, 신뢰 무관하게 적용:
  - `deny`(`Read()`): **UE 생성물 노이즈 차단** — `Binaries`·`Intermediate`·`DerivedDataCache`·`Saved`(통째)·플러그인 `Binaries`/`Intermediate`. `Read()` deny는 Read·Grep·Glob·`cat`/`head`/`tail`/`sed`까지 적용된다.
  - `deny`(shell): `rm -rf /`·홈 삭제·git force-push 등 파괴적/비가역 명령만 차단(안전망).
  - `enabledMcpjsonServers: ["monolith"]`: 프로젝트 스코프 MCP 서버 사전 신뢰.

**로그 진단** — `Saved/`가 Read deny 대상이라 빌드·에디터 로그는 PowerShell `Get-Content Saved/Logs/*.log | Select-String LogMonolith`로 읽는다(`Read()` deny는 PowerShell `Get-Content`을 막지 않는다).

**`.claudeignore`는 쓰지 않는다** — Claude Code 정식 기능이 아니라 강제되지 않는다. 노이즈 제외는 위 `deny Read()`(강제됨)와 `.gitignore`(발견 단계 제외)로 처리한다.

동일 템플릿이 [`docs/common-ui-starter-kit-prompt.md`](docs/common-ui-starter-kit-prompt.md)의 **Step A-0**에 있다. 광범위 allow는 "본인 머신에서 본인 UE 프로젝트를 빌드하는 로컬 자동화"라는 전제에서의 선택이다.

## 서브에이전트

- `ue-code-reviewer` — C++·Common UI·입력·Config·Monolith 자동화 변경 후 리뷰. 파일을 수정하지 않는다.
- `ue-code-refactorer` — 동작 보존 리팩토링. 아키텍처 변경은 사용자 요청이 있을 때만.

두 에이전트는 규칙을 자체 보유하지 않고 위 문서 지도를 따라 읽는다. 정의는 [`.claude/agents/`](.claude/agents/)에 있다.

## 빌드 / 테스트

저장소 루트 자체에는 빌드 시스템·테스트가 없다(마크다운 문서뿐).

UE 프로젝트의 빌드·에디터 실행 명령은 [`CommonUIStarterKit/docs/project-guide.md`](CommonUIStarterKit/docs/project-guide.md)의 "머신 환경 quirk" 절에 있다. **엔진 경로를 추측하지 말고 그 명령을 그대로 쓴다.**
