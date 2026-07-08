# Unreal Engine Common UI Starter Kit 생성 프롬프트 (Monolith MCP · automation-first + 문서화된 수동 fallback)

> **문서 목적** — Claude Code가 Monolith MCP를 이용해 UE5의 Common UI 학습용 Starter Kit을 **빈 폴더에서부터** 자동 생성하도록 지시하는 프롬프트다. "완전 무인 자동화"가 아니라, **자동화를 우선(automation-first)** 하되 Monolith가 처리하지 못하는 항목(특히 MVVM View Binding)은 **문서화된 수동 fallback 체크리스트**로 넘기는 현실적 전략을 취한다.

## 설계 원칙

- **바퀴를 재발명하지 않는다.** 진짜 엔진 라이브러리(Common UI, CommonInput, Enhanced Input, UMG, ModelViewViewModel)는 그대로 재사용한다. Lyra의 **CommonGame은 engine plugin이 아니라 Lyra 샘플 프로젝트에 포함된 orchestration 플러그인**이므로, 이를 그대로 복사하는 대신 **학습용으로 단순화한 3-class 레이어링만 자체 모듈에 재구현**한다. 이것은 "바퀴 재발명"이 아니라 얇은 orchestration 계층을 교육용으로 다시 쓰는 것이다.
- **단계별로 만들고, 단계마다 PIE로 검증한다.** 모든 위젯/에셋을 한 번에 만들지 않는다. 세션 C의 **Stage 1~6**로 나누고 **각 Stage 끝에서 PIE smoke test**를 통과해야 다음 Stage로 넘어간다. Common UI의 학습 가치는 "조합 가능한 계층을 점진적으로 쌓는 것"에 있다.
- **자동화 불가 항목은 건너뛰지 말고 수동 체크리스트로 남긴다.**
- **코드는 레퍼런스를 먼저 본다.** C++/BP 로직 작성 시 반복되는 API idiom·함정·네이밍은 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md)를 **정본(source of truth)**으로 삼아 먼저 참조한다. 프롬프트는 "무엇을 만들지", 레퍼런스는 "어떻게 정확히 쓸지"를 담는다.

## 자동화 범위 안내

사전 준비(프로젝트 생성, 플러그인 설치, MCP 설정, 에디터 실행)도 Claude Code가 직접 수행할 수 있습니다. 단, 다음 두 가지는 구조상 사용자가 해야 합니다.

1. **UE 5.8 엔진과 Visual Studio(C++ 게임 개발 워크로드)가 미리 설치되어 있어야 합니다.** 엔진 설치 자체(Epic Games Launcher)는 자동화 대상이 아닙니다. (UE 5.8은 2026-06-17 릴리스된, 계획상 마지막 UE5 메이저 버전입니다.)
2. **`.mcp.json` 생성 후(세션 B 끝) Claude Code를 재시작해야 합니다.** Claude Code는 MCP 서버 설정을 세션 시작 시점에 읽기 때문에, 실행 중인 세션에서 `.mcp.json`을 만들어도 그 세션에서는 Monolith 도구를 쓸 수 없습니다. (프로젝트 스코프 MCP 서버 신뢰는 **세션 A의 Step A-0에서 만든 `.claude/settings.json`이 `enabledMcpjsonServers: ["monolith"]`로 미리 승인**하므로, 재시작 후 별도 신뢰 프롬프트 없이 로드됩니다.)

작업을 **세 세션**으로 나눕니다. **기술적으로 재시작이 필수인 경계는 B→C(MCP 로드)뿐**이고, A→B는 컨텍스트·오류 격리를 깔끔하게 하기 위한 **권장 체크포인트**입니다(새 세션 권장, 급하면 세션 A 완료 후 한 세션에서 B를 연속 진행 가능).

- **세션 A — 프로젝트 생성 + 플러그인 설정**: 빈 폴더 → 프로젝트 파일 생성, Monolith 설치, **플러그인 컴파일 게이트 빌드**(빈 module로 툴체인+플러그인이 UE 5.8에서 링크되는지 **C++ 코드 작성 전에** 검증). *(→ CLAUDE.md 핸드오프)*
- **세션 B — C++ 코드베이스 뼈대**: 3-class 레이어링·base widgets·ViewModel 스텁·INI 작성 → C++ 빌드 → `.mcp.json` 생성 → 에디터 실행(port 9316). *(→ CLAUDE.md 핸드오프, **여기서 재시작**)*
- **세션 C — 에셋 생성**: Claude Code 재시작 후 → Monolith 역량 probe → **Stage 1~6**를 순차 생성·검증(각 Stage마다 PIE smoke test).

**Stage 1~6**은 세션 C의 에셋 빌드 순서입니다. Stage는 "무엇을 어떤 순서로 만들고 각 단계에서 무엇을 검증하는가"를 정의하는 학습 순서입니다.

> **Monolith / 버전 참고** — Monolith(`github.com/tumourlove/monolith`)는 **UE 5.7 & 5.8을 단일 소스로 지원**하며 `monolith_proxy.exe`, port **9316**, Blueprint graph 편집 / UI namespace / MVVM binding 저작(reflection) / live-PIE introspection·driving / editor 제어 / engine source 인덱싱을 제공합니다. 그래도 build 전 **설치된 Monolith가 현재 엔진 버전용 빌드인지 확인**하는 것을 hard gate로 둡니다. 5.8에서 문제가 있으면 5.7 fallback을 문서화하세요.

사용법: 빈 작업 폴더에서 Claude Code를 실행하고 **프롬프트 A**(프로젝트+플러그인) → **프롬프트 B**(C++ 뼈대)를 차례로 전달 → 세션 B 완료 후 Claude Code를 **재시작**하고 **프롬프트 C**(에셋)를 전달. (A→B는 원하면 한 세션에서 연속 진행 가능; **B→C만 재시작 필수**.)

---

## 프롬프트 A — 세션 A: 프로젝트 생성 + 플러그인 설정

당신은 Unreal Engine 5 UI 아키텍처 전문가입니다. Common UI 학습용 Starter Kit을 **빈 폴더에서부터** 구축하는 3-세션 워크플로의 **첫 세션(A)**입니다. 목표는 **프로젝트 파일 + 플러그인**을 갖추고, **C++ 코드를 쓰기 전에** 툴체인과 플러그인(CommonUI/CommonInput/MVVM/Monolith)이 UE 5.8에서 컴파일·링크되는지 검증하는 것입니다(C++ 3-class 뼈대는 다음 **세션 B**에서 작성). 에디터 없이 파일 생성과 커맨드라인 빌드만 사용합니다. 모든 파일에 학습용 한국어 주석을 상세히 작성하되, 기술 용어는 영어 원문을 사용하세요.

### Step A-0 — 권한 사전 설정 (allowlist) ⭐ 가장 먼저

세 세션 전체가 UBT 빌드·에디터 실행·git·다운로드 등 **shell 명령을 대량으로** 실행하고, 세션 C는 **Monolith MCP 도구를 대량으로** 호출합니다. 매번 권한 프롬프트가 뜨지 않도록 **작업 폴더에 아래 두 파일을 가장 먼저 생성**하세요.

> **⚠️ 왜 두 파일로 나누는가 (핵심)** — committed `.claude/settings.json`의 `allow` 규칙은 **workspace 신뢰(trust) 대화상자를 수락한 뒤에만** 적용됩니다(그전엔 읽기만 하고 무시 → 계속 프롬프트). 반면 **gitignore된 `.claude/settings.local.json`은 "repo가 제공하지 않은 본인 파일"로 취급되어 신뢰 없이 즉시 적용**됩니다. 그래서 **`allow`(권한 부여)는 `settings.local.json`에**, **`deny`·MCP(정책, 신뢰 무관하게 적용)는 committed `settings.json`에** 둡니다. (`.gitignore`가 `.claude/settings.local.json`을 무시하는 것이 전제 — Step A-2의 `.gitignore`에 포함됨. Claude Code v2.1.200+ 기준.)

**`.claude/settings.json`** — 커밋 대상(정책·안전; deny/MCP는 신뢰 없이도 적용):
```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "permissions": {
    "deny": [
      "Read(/Binaries/**)",
      "Read(/Intermediate/**)",
      "Read(/DerivedDataCache/**)",
      "Read(/Saved/**)",
      "Read(/Plugins/**/Binaries/**)",
      "Read(/Plugins/**/Intermediate/**)",
      "Bash(rm -rf /)",
      "Bash(rm -rf /*)",
      "Bash(rm -rf ~)",
      "Bash(rm -rf ~/*)",
      "Bash(git push --force*)",
      "Bash(git push -f*)",
      "PowerShell(Remove-Item -Recurse -Force /)",
      "PowerShell(Remove-Item -Recurse -Force C:\\*)"
    ]
  },
  "enabledMcpjsonServers": [
    "monolith"
  ]
}
```

**`.claude/settings.local.json`** — gitignore됨(권한 부여; **신뢰 없이 즉시 적용 → 매번 안 물어봄**):
```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "permissions": {
    "allow": [
      "Bash",
      "PowerShell",
      "Read",
      "Glob",
      "Grep",
      "Edit",
      "Write",
      "mcp__monolith"
    ]
  }
}
```

**각 항목의 역할**

| 설정 | 파일 | 목적 |
|------|------|------|
| `allow: ["Bash", "PowerShell"]` | local | 세션 A의 모든 shell 명령(UBT 빌드·git·다운로드·에디터 실행·포트 폴링·`python` 등) 무프롬프트. 이 환경은 **PowerShell이 primary**라 둘 다 열어야 매끄럽습니다. |
| `allow: ["Read", "Glob", "Grep"]` | local | 코드/로그 탐색 (read-only, 보통 프롬프트 없지만 명시) |
| `allow: ["Edit", "Write"]` | local | `.uproject`/`Build.cs`/`.h`/`.cpp`/`.ini`/`CLAUDE.md` 등 대량 파일 생성·수정 무프롬프트 |
| `allow: ["mcp__monolith"]` | local | 세션 C의 **Monolith MCP 도구 전체** 무프롬프트 (server 이름만 지정하면 그 서버의 모든 action 허용) |
| `enabledMcpjsonServers: ["monolith"]` | committed | `.mcp.json`의 monolith 서버를 **미리 신뢰(승인)** → 재시작 후 서버 신뢰 프롬프트 없이 로드 |
| `deny: ["Read(...)"]` | committed | **UE 생성물 노이즈 차단 (`.claudeignore` 대체).** `Binaries`·`Intermediate`·`DerivedDataCache`·`Saved`·플러그인 `Binaries`/`Intermediate`를 Read 대상에서 하드 차단 → 컨텍스트 낭비 방지(단순화를 위해 `Saved`는 통째로). `Read()` deny는 Read·Grep/Glob(best-effort)·`@file`·Bash의 `cat`/`head`/`tail`/`sed`까지 적용됨(deny > allow). |
| `deny: ["Bash(...)", "PowerShell(...)"]` | committed | 안전망. `rm -rf /`·홈 삭제·git force-push 등 **파괴적/비가역 명령만** 하드 차단(프롬프트 아님). 빌드 정리·일반 명령(`python` 등)은 막지 않음. |

> **왜 `deny`는 committed에 둬도 되는가** — `deny`/`ask` 규칙은 workspace 신뢰와 무관하게 **모든 스코프에서 즉시 적용**됩니다(신뢰 게이트는 `allow`·`additionalDirectories`에만 걸림). 그래서 안전·노이즈 정책은 committed에 두어 공유하고, 권한 부여만 local로 분리합니다.

> **왜 `.claudeignore`가 아니라 `deny Read()`인가** — `.claudeignore`는 Claude Code의 정식 기능이 아니라 그 자체로는 강제되지 않습니다(내장 존중 대상은 `.gitignore`뿐). 노이즈를 실제로 차단하려면 `permissions.deny`의 `Read()` 규칙이 유일하게 신뢰할 수 있는 방법입니다. 단 **완전 차단은 아님**: 임의 스크립트(python/node)나 PowerShell `Get-Content`은 우회할 수 있고, OS 레벨 전면 차단은 sandbox가 필요합니다(이 kit엔 비밀 파일이 없어 실질 위험 낮음).

> **⚠️ `Saved/`는 통째로 Read 차단 — 로그 진단은 PowerShell로** — 설정 단순화를 위해 `Saved/`를 하위폴더 구분 없이 막습니다. `Read()` deny는 **Read 도구·Grep/Glob·`cat`/`head`/`tail`/`sed`만** 막고 **PowerShell `Get-Content`/`Select-String`은 막지 않으므로**, 빌드·에디터 로그가 필요하면 `Get-Content Saved/Logs/*.log | Select-String LogMonolith`로 읽습니다(이 환경은 PowerShell이 primary라 자연스러움). 그래도 `Read` 도구로 로그를 열어야 하면 `Read(/Saved/**)`만 잠시 제거하세요.
> `Plugins/Monolith/Monolith.uplugin`(Step A-3 버전 게이트가 읽음)은 deny 대상이 아니므로 그대로 읽힙니다.

**추가로 검토해야 할 권한 (상황에 따라)**

- **노이즈 차단을 더 넓히고 싶을 때**: `Read(/Plugins/Monolith/Source/**)`(제3자 C++ 소스)를 deny에 추가할 수 있습니다. 단 Monolith 빌드 오류를 소스로 triage할 경우를 대비해 기본값에는 넣지 않았습니다.
- **엔진 헤더를 Read tool로 직접 열어야 할 때**: 엔진은 프로젝트 밖(`C:\Program Files\Epic Games\UE_5.8`)에 있어 기본 권한 범위를 벗어납니다. C++ API 확인은 세션 C에서 `source` 네임스페이스(Monolith)로 처리하므로 보통 불필요하지만, 필요하면 `permissions.additionalDirectories`에 엔진 경로를 추가하세요.
- **더 강한 자동화가 필요할 때**: 위 allowlist로도 남는 프롬프트가 거슬리면 `permissions.defaultMode: "acceptEdits"`(파일 편집 자동 수락)를 추가할 수 있습니다. `bypassPermissions`(전면 우회)는 안전상 권장하지 않습니다.
- **커밋 자동화가 필요 없으면** git 관련 deny는 그대로 두세요. 반대로 학습 중 커밋을 자주 한다면 `Bash(git push*)`를 allow에 명시해도 됩니다(force-push deny는 유지).
- **버전 노트**: `"PowerShell"`은 이 Windows 환경의 shell tool 이름입니다. 다른 환경(Bash 전용)이라면 `"PowerShell"` 항목은 무시되고 `"Bash"`만 적용됩니다. 만약 `"Bash"`/`"PowerShell"` tool-only 형식이 allow-all로 동작하지 않는 Claude Code 버전이면 `"Bash(*)"`/`"PowerShell(*)"`로 바꾸세요.

### Step A-1 — 환경 확인

1. UE 5.8 설치 경로를 탐색하세요 (기본: `C:\Program Files\Epic Games\UE_5.8`. 없으면 다른 드라이브의 `Epic Games\UE_5.8`도 확인). 찾지 못하면 사용자에게 경로를 질문하세요.
2. Visual Studio C++ 툴체인 존재를 확인하세요 (`vswhere` 활용).
3. git 사용 가능 여부를 확인하세요.

### Step A-2 — UE C++ 프로젝트 생성 (에디터 위저드 없이)

프로젝트명 `CommonUIStarterKit`으로 다음 파일을 직접 생성하세요. UE 프로젝트는 파일 집합일 뿐이므로 위저드 없이 만들 수 있습니다.

- `CommonUIStarterKit.uproject` — EngineAssociation `5.8`, 모듈 정의, **플러그인 활성화 목록**:
  - `CommonUI`, `ModelViewViewModel`, `Monolith`
  - ⚠️ **`CommonInput`은 여기 넣지 마세요.** CommonInput은 별도 plugin이 아니라 **CommonUI 플러그인 안의 module**입니다. `.uproject`에 plugin으로 넣으면 "plugin not found" 경고가 납니다. (Build.cs에서 module로만 참조합니다.)
  - ⚠️ **`EnhancedInput`도 넣지 마세요.** 엔진 기본 활성화 플러그인입니다.
- `Source/CommonUIStarterKit/CommonUIStarterKit.Build.cs` — 의존 module: `Core`, `CoreUObject`, `Engine`, `InputCore`, `CommonUI`, **`CommonInput`**, `EnhancedInput`, `UMG`, `Slate`, `SlateCore`, `ModelViewViewModel`, `GameplayTags`
- `Source/CommonUIStarterKit.Target.cs`, `Source/CommonUIStarterKitEditor.Target.cs`
- **최소 기본 module 소스** (`.h`/`.cpp` — `IMPLEMENT_PRIMARY_GAME_MODULE` + 빈 `StartupModule`/`ShutdownModule`만; 실제 UI 클래스는 **세션 B**에서 작성), `Config/DefaultEngine.ini`·`DefaultGame.ini`·`DefaultInput.ini`(기본값 골격만; CommonUI 관련 실제 값은 **세션 B**에서 채움)
- `Content/`, `Plugins/` 빈 디렉터리
- **`.gitignore`** — UE5 표준(빌드/캐시/저장 폴더·컴파일 산출물·IDE 파일 무시). `Plugins/Monolith/`는 통째로 무시(프롬프트로 재설치, 학습 콘텐츠 아님), `.claude/settings.local.json`은 무시하되 `.claude/settings.json`·`.mcp.json`·`Config/*.ini`·`Content/**`·`Source/**`·`*.uproject`는 커밋. (이 repo의 `.gitignore`를 그대로 복사해 쓰세요.)
- **`.claudeignore`는 만들지 않습니다.** Claude Code가 자동으로 존중하지 않는 파일이라 무의미합니다. Claude 탐색에서 노이즈(바이너리/캐시)를 제외하는 일은 **Step A-0 `.claude/settings.json`의 `permissions.deny` `Read()` 규칙**이 담당합니다(그쪽이 유일하게 강제되는 방법). 파일 *발견* 단계 제외는 `.gitignore`가 이미 처리합니다.
- **`docs/common-ui-coding-reference.md`** — 이 repo의 **코딩 레퍼런스 정본**(Common UI/CommonGame/MVVM/Input의 idiom·함정·네이밍)을 대상 프로젝트로 **그대로 복사**하세요. 세션 B(C++)·세션 C(BP 로직)가 코드 작성 시 참조합니다.

### Step A-3 — Monolith 플러그인 설치 및 버전 게이트

1. GitHub 최신 릴리스 zip을 받아 `Plugins/Monolith`에 설치하세요 (릴리스 zip에는 사전 컴파일된 DLL과 `monolith_proxy.exe`가 포함됨). 릴리스 다운로드가 불가능한 환경이면 `git clone https://github.com/tumourlove/monolith.git Plugins/Monolith`로 대체하세요.
2. 설치 후 `Plugins/Monolith/Monolith.uplugin`과 `Plugins/Monolith/Binaries/monolith_proxy.exe` 존재를 확인하세요.
3. **[HARD GATE]** `Monolith.uplugin`/릴리스 노트에서 **현재 엔진 버전(5.8) 지원 여부를 확인**하세요. Monolith는 5.7 & 5.8을 지원하지만, 설치본이 구버전이면 진행하지 마세요. 5.8용 빌드를 구할 수 없으면 사용자에게 보고하고, **UE 5.7 fallback**(EngineAssociation `5.7`) 경로를 제안하세요.

### Step A-4 — 플러그인 컴파일 게이트 빌드 (C++ 작성 전 검증)

아직 실제 UI 클래스는 없습니다(최소 module만). **이 상태로 먼저 빌드**해 툴체인과 플러그인이 UE 5.8에서 정상 링크되는지 검증합니다. C++ 코드를 쓰기 전에 하므로, 오류가 나면 **플러그인/환경 문제로 격리**됩니다.

1. UBT로 프로젝트 파일 생성 후 `Build.bat`(또는 `RunUBT`)로 `CommonUIStarterKitEditor Win64 Development` 타깃을 빌드하세요.
2. 오류를 분석해 수정하고 성공할 때까지 반복하세요. 이 단계의 오류는 대개 (a) 플러그인 활성화/module 이름 오타, (b) Monolith 5.8 비호환(→ Step A-3 버전 게이트 재확인, 필요 시 5.7 fallback), (c) VS 툴체인 문제입니다. **Monolith 플러그인 오류와 환경 오류를 구분해 보고**하세요.
3. 빌드 성공 = "툴체인 + 모든 플러그인 module(CommonUI/CommonInput/MVVM/Monolith)이 5.8에서 링크됨" 게이트 통과. (아직 에디터는 실행하지 않습니다.)

### Step A-5 — 세션 A 종료 보고 + 핸드오프

1. `CLAUDE.md`를 생성/갱신해 기록: 프로젝트 개요, 언어 규칙(한국어 기본·기술용어 영어), **권한 설정(Step A-0의 2-파일 allowlist) 요약**, 생성한 파일 목록, **게이트 빌드 결과**, Monolith 5.8 호환 확인 여부, **코딩 레퍼런스(`docs/common-ui-coding-reference.md`) 존재 — 세션 B/C가 코드 작성 시 참조**, **다음 세션 B에서 할 일(C++ 3-class 뼈대 작성)**.
2. 사용자에게 안내:
   > "세션 A(프로젝트+플러그인) 완료 — 툴체인+플러그인 컴파일 게이트를 통과했습니다. 다음은 **프롬프트 B**(C++ 뼈대)입니다. **A→B는 재시작이 필요 없으므로** 이 세션에서 바로 이어가거나, 컨텍스트를 깔끔히 하려면 새 세션에서 프롬프트 B를 전달하세요."

---

## 프롬프트 B — 세션 B: C++ 코드베이스 뼈대

이 프로젝트는 세션 A에서 프로젝트 파일 + 플러그인이 갖춰지고 **게이트 빌드를 통과**한 상태입니다(`CLAUDE.md` 참조). 이 세션에서 Lyra CommonGame **패턴**을 학습용으로 재구현한 **C++ 3-class 레이어링**과 base widgets·ViewModel 스텁·INI를 작성하고, 빌드로 검증한 뒤 다음 세션 C의 MCP 에셋 작업을 위해 `.mcp.json`과 에디터를 준비합니다. 시작 전에 `CLAUDE.md`로 세션 A 결과를 확인하고, **C++를 작성하기 전에 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md)의 §3(모듈·Build.cs)·§4(INI)·§5(input)·§6(layer)·§7(MVVM)·§9(deprecated)를 먼저 읽으세요.** 모든 파일에 학습용 한국어 주석을 상세히 작성하되, 기술 용어는 영어 원문을 사용하세요.

### Step B-1 — C++ 뼈대 작성 (충실한 3-class 레이어링)

Lyra의 CommonGame **패턴**을 학습용으로 단순화하되, **핵심 3-class 분리는 유지**합니다. (CommonGame 코드를 복사하지 않고 패턴만 재구현합니다.)

> 📖 아래 항목의 ⚠️ 요점은 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md)에 근거·예시와 함께 정리돼 있습니다(§5 input·§6 layer·§7 MVVM·§8 settings). 여기서는 "무엇을 구현할지"를, 상세 idiom·시그니처는 레퍼런스를 본다.

**코딩 규약 (반드시 준수 — 레퍼런스 §2)**

- **프로젝트 클래스 프리픽스 `Cu`**: 모든 프로젝트 고유 C++ 클래스는 UE 타입 프리픽스(`U`/`A`/`F`/`E`) 뒤에 **`Cu`**를 붙인다(예: `UCuGameUIManagerSubsystem`). 엔진/CommonGame 재사용 타입과 구분하고, 특히 `UCuLocalPlayer`처럼 CommonGame 실제 클래스명(`UCommonLocalPlayer`)과의 충돌·혼동을 방지한다. primary game module 클래스만 예외(모듈명 규칙).
- **소스 폴더 구조 (역할별)**: module 루트에 평면 배치하지 말고 **역할별 하위폴더**(`System/`·`Layout/`·`Widgets/`·`Screens/`·`ViewModels/`)로 나눈다. ⚠️ **`Public`/`Private`는 쓰지 않고** .h/.cpp를 같은 역할 폴더에 함께 둔다. ⚠️ 이 경우 **Build.cs에 `PublicIncludePaths.Add(ModuleDirectory);`를 반드시 추가**해야 하위폴더 헤더의 **루트 기준 경로 한정 include**(`#include "System/CuGameUIPolicy.h"`)가 해석된다(Public 폴더 없이는 UBT가 루트를 자동 등록하지 않음 → 누락 시 `C1083`). (전체 트리·규칙은 레퍼런스 §2·§3.)

**레이어링 아키텍처 (3-class split — 반드시 유지)**

```
UCuGameUIManagerSubsystem  (player lifecycle)
        └─ owns ─▶ UCuGameUIPolicy  (LayoutClass 소유, per-LocalPlayer 레이아웃 생성)
                        └─ creates ─▶ UCuPrimaryGameLayout  (레이어 스택 소유)
```

- `UCuGameUIManagerSubsystem : UGameInstanceSubsystem` — **레이아웃을 직접 생성하지 않습니다.** local player lifecycle(추가/제거)에 반응해 `UCuGameUIPolicy`에 위임만 합니다. `UPROPERTY(config) TSoftClassPtr<UCuGameUIPolicy> DefaultUIPolicyClass`를 `DefaultGame.ini`에서 읽습니다.
- `UCuGameUIPolicy : UObject` — `LayoutClass`(= `UCuPrimaryGameLayout` 서브클래스)를 소유하고, `UCuLocalPlayer`마다 레이아웃을 **하나씩** 생성해 그 플레이어 화면에 붙입니다(`AddToViewport`가 아니라 per-player `AddToPlayerScreen` 계열). per-player 목록을 추적해 split-screen에도 각기 독립 레이아웃이 되도록 합니다.
- `UCuPrimaryGameLayout : UCommonUserWidget` — GameplayTag로 식별되는 `UCommonActivatableWidgetStack` 레이어들을 `BindWidget`으로 보유. 제공 API:
  - `RegisterLayer(FGameplayTag, UCommonActivatableWidgetContainerBase*)` — **BindWidget만으로는 tag→container 맵이 채워지지 않습니다.** `NativeOnInitialized`에서 각 스택을 명시적으로 `RegisterLayer` 하세요.
  - `PushWidgetToLayerStack<T>(FGameplayTag, TSubclassOf<T>)` — **동기**(hard class) push
  - `PushWidgetToLayerStackAsync<T>(FGameplayTag, TSoftClassPtr<T>, ...)` — **비동기**(soft class + streaming handle) push. 실제 Lyra가 메뉴/모달을 여는 방식이므로 학습용으로 반드시 포함.
  - `static UCuPrimaryGameLayout* GetPrimaryGameLayout(APlayerController*)` — 정적 접근자
  - ⚠️ push는 컨테이너가 위젯을 **자동으로 activate** 합니다. Blueprint 그래프에서 push 후 다시 `ActivateWidget`을 호출하지 마세요(이중 activation 버그).
- `UCuLocalPlayer : ULocalPlayer` — 정책이 per-player 레이아웃 맵의 key로 삼는 아이덴티티. `DefaultEngine.ini`에서 **Local Player Class**로 지정. (단일 플레이어 kit이라도 학습 지점으로 포함하고, 어디서 split-screen이 hook되는지 주석으로 표시.)

**Activatable / Button 베이스**

- `UCuActivatableWidget : UCommonActivatableWidget` — 모든 화면의 베이스.
  - 입력 모드는 **`GetDesiredInputConfig()` 오버라이드**로 `TOptional<FUIInputConfig>`를 반환해 지정합니다. `FUIInputConfig`의 모드는 **`ECommonInputMode::Menu` / `Game` / `All`** 입니다. ⚠️ **`GameAndMenu`라는 값은 없습니다**(=`All`). ⚠️ **`APlayerController::SetInputMode*`를 직접 호출하지 마세요** — CommonUI의 `UCommonUIActionRouterBase`가 입력을 관리하므로 SetInputMode는 라우터를 깨뜨립니다.
  - 포커스 대상은 `GetDesiredFocusTarget()`(C++) / `BP_GetDesiredFocusTarget`(BP) 오버라이드로 지정하고, `bAutoRestoreFocus`로 스택 복귀 시 포커스 복원.
  - Back 액션 기본 처리(Stage 4에서 `RegisterUIActionBinding`로 연결).
- `UCuButtonBase : UCommonButtonBase` — 버튼 텍스트 프로퍼티 + 스타일 적용 헬퍼.

**화면별 베이스 (C++)**: `UCuTitleScreenWidget`, `UCuMainMenuWidget`, `UCuSettingsScreenWidget`, `UCuPauseMenuWidget`, `UCuConfirmationModalWidget` — 로직은 C++에 최대한 유지.

**ViewModel (Stage 6용, 이번 세션엔 스텁만)**: `UCuSettingsViewModel : UMVVMViewModelBase`
- `INotifyFieldValueChanged` 기반. FieldNotify `UPROPERTY` (예: master volume `float`).
- ⚠️ setter에서 **`UE_MVVM_SET_PROPERTY_VALUE(...)`를 반드시 호출**해야 바인딩된 위젯이 갱신됩니다(빠뜨리면 무음 실패).
- ⚠️ **`UGameUserSettings`에는 master volume 프로퍼티가 없습니다.** 볼륨은 **Sound Class/Sound Mix**(`SetSoundMixClassOverride`) 또는 커스텀 SaveGame으로 구현하세요. 해상도/vsync 등은 `UGameUserSettings`로 가능.

**GameplayTag**: `UE_DEFINE_GAMEPLAY_TAG`로 네이티브 선언 — `UI.Layer.Game` / `UI.Layer.GameMenu` / `UI.Layer.Menu` / `UI.Layer.Modal`.

### Step B-2 — INI 기본 설정 (치명적 항목 포함)

⚠️ **CommonUI/CommonInput 관련 설정은 `DefaultInput.ini`가 아니라 `DefaultGame.ini` / `DefaultEngine.ini`에 넣습니다.** (`DefaultInput.ini`는 Enhanced Input 클래스 기본값 전용.) 키→파일 매트릭스와 근거는 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md) §4 참조.

- **`DefaultEngine.ini` — [치명적, 절대 누락 금지]**
  ```ini
  [/Script/Engine.Engine]
  GameViewportClientClassName=/Script/CommonUI.CommonGameViewportClient
  ; ▲ 이게 없으면 UIActionRouter가 입력을 전달받지 못해
  ;   gamepad 내비게이션·Back이 "무음"으로 전부 죽습니다. CommonUI 최다 셋업 실패 원인.

  [/Script/Engine.GameEngine]  ; (또는 프로젝트 상황에 맞는 위치)
  ; Local Player Class = UCuLocalPlayer
  ```
- **`DefaultGame.ini`**
  - `UCuGameUIManagerSubsystem`의 `DefaultUIPolicyClass`(config) 지정
  - `[/Script/CommonInput.CommonInputSettings]`의 `InputData`(= `UCommonUIInputData` 에셋 경로) — 에셋은 Stage 4에서 생성하고 여기서 연결

### Step B-3 — 빌드 및 검증 (C++ 뼈대)

1. `Build.bat`(또는 `RunUBT`)로 `CommonUIStarterKitEditor Win64 Development` 타깃을 다시 빌드하세요(이번엔 C++ 뼈대 포함).
2. 컴파일 오류는 로그를 분석해 수정하고 성공할 때까지 반복하세요. 플러그인은 세션 A에서 이미 검증됐으므로 **이 단계의 오류는 대부분 프로젝트 C++ 코드 문제**입니다(잘못된 시그니처·deprecated API·헤더 누락 등). 세션 B는 아직 MCP가 없으므로 API 확인은 엔진 헤더/문서로 하세요.

### Step B-4 — MCP 설정 및 에디터 실행

1. 프로젝트 루트에 `.mcp.json` 생성:
   ```json
   {
     "mcpServers": {
       "monolith": {
         "command": "Plugins/Monolith/Binaries/monolith_proxy.exe",
         "args": []
       }
     }
   }
   ```
   > 이 서버(`monolith`)는 Step A-0의 `.claude/settings.json` `enabledMcpjsonServers`에 이미 등록해 두었으므로, 재시작 후 신뢰 프롬프트 없이 로드됩니다. 서버 이름을 `monolith` 외로 바꾸면 settings의 이름도 함께 맞추세요.
2. 에디터를 백그라운드로 실행하세요:
   `"<엔진경로>\Engine\Binaries\Win64\UnrealEditor.exe" "<프로젝트경로>\CommonUIStarterKit.uproject"`
3. 포트 **9316**이 열릴 때까지 폴링하세요 (첫 실행은 셰이더 컴파일 + Monolith 인덱싱 30~60초+ 소요). 실패 시 로그를 확인해 보고하세요. `Saved/`는 Read deny 대상이므로 로그는 PowerShell로 읽으세요: `Get-Content Saved/Logs/*.log | Select-String LogMonolith`.
4. `CLAUDE.md`를 갱신해 기록: C++ 뼈대 컴파일 성공, INI 설정 완료, `.mcp.json` 생성, 에디터 실행(port 9316) 상태, Monolith 사용 규칙(`monolith_discover()`/`monolith_guide()` 우선, 파라미터 추측 금지), **다음 세션 C에서 할 일(역량 probe → Stage 1~6 에셋 생성)**.

### Step B-5 — 세션 종료 보고 + 핸드오프

생성/수정한 파일 목록, C++ 빌드 결과, 에디터/MCP 서버 상태를 요약하고 안내:

> "세션 B(C++ 뼈대) 완료 — 컴파일 성공, `.mcp.json` 생성, 에디터 실행 중입니다. `.mcp.json`은 새 세션에서만 로드되므로 **여기서 Claude Code를 재시작**한 뒤 **프롬프트 C**(에셋)를 전달해 주세요. (monolith 서버 신뢰와 도구 권한은 `.claude/settings.json`·`settings.local.json`에 미리 설정돼 있어 별도 승인 프롬프트는 없습니다. 만약 신뢰 프롬프트가 뜨면 승인하세요.) **에디터는 계속 실행 상태로 두세요.**"

---

## 프롬프트 C — 세션 C: Monolith MCP로 에셋 생성 (Stage 1~6)

이 프로젝트는 세션 A(프로젝트+플러그인)와 세션 B(C++ 뼈대)를 마친 Common UI Starter Kit입니다(`CLAUDE.md` 참조). 에디터가 실행 중이고 Monolith MCP 서버가 연결되어 있습니다. 이제 **Stage별로** Blueprint·style·input 에셋을 만들고 **각 Stage마다 PIE로 검증**합니다.

### 작업 시작 전 필수 절차

1. Monolith MCP 연결을 확인하세요. 도구가 보이지 않으면 사용자에게 `/mcp`로 연결 상태 확인을 요청하고 중단하세요.
2. `monolith_discover()`로 네임스페이스/액션 목록을 파악하고, UI(CommonUI)·Blueprint·editor 네임스페이스에 대해 `monolith_guide()`를 호출하세요.
3. 액션 파라미터는 추측하지 말고 discover/guide 결과 기준으로 사용하세요. 오류 시 `did_you_mean` 힌트를 활용해 재시도하세요.
4. C++ API 확인이 필요하면 `source` 네임스페이스(`get_include_path`, `get_signature`, `verify_symbols`, `check_deprecations` 등)를 활용하세요. **불확실한 시그니처(예: `PushWidgetToLayerStackAsync`)는 반드시 `source.get_signature`로 확인 후 사용.**
5. **BP graph·style·MVVM 저작 시 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md)의 §2(네이밍)·§6(layer)·§7(MVVM)·§10(함정 체크리스트)을 참조하세요.**

### Content 폴더 구조

`UI/Foundation`, `UI/Menu`, `UI/HUD`, `UI/Style`, `Input` (에셋 경로는 `/Game/UI/...` 정규 경로).

---

### 사전 Probe — Monolith 역량 확인 + 수동 fallback 선언 ⭐ 에셋 생성 전 먼저

에셋을 만들기 **전에**, Monolith가 아래 4가지를 실제로 할 수 있는지 `discover`/`guide`(+ 필요 시 소규모 시험 생성)로 **probe**하고, 불가능한 항목은 **수동 체크리스트**로 `CLAUDE.md`에 기록하세요.

| 역량 | 왜 위험한가 | 자동 불가 시 fallback |
|------|-------------|----------------------|
| (a) 정확한 `BindWidget` child 이름으로 WBP 위젯 트리 생성 | C++ `BindWidget` 프로퍼티명과 **정확히 일치**해야 바인딩됨 | 위젯 트리/이름만 수동 배치 체크리스트 |
| (b) Blueprint **graph** 편집 (OnClicked→push 등) | data-asset 생성보다 난도 높음 | 그래프 노드 연결을 수동 체크리스트로 |
| (c) style data-only BP의 **nested struct / sub-object** 값 설정 (FSlateBrush, font 등) | 중첩 구조·서브에셋 참조가 까다로움 | 스칼라/컬러만 자동, 브러시·폰트는 수동 |
| (d) **MVVM View Binding** 저작 | 바인딩은 WBP의 `UMVVMWidgetBlueprintExtension_View`에 저장되며, 정식 경로는 View Bindings 에디터 패널. 프로그램적 경로는 Experimental `unreal.MVVMView` Python API뿐 | **Stage 6의 View Binding은 수동/Experimental fallback으로 사전 선언** |

**검증**: 에디터 up + MCP 연결 확인 + capability matrix를 `CLAUDE.md`에 기록.

---

### Stage 1 — CORE "hello world" Activatable

- `WBP_StarterActivatable`(`UCuActivatableWidget` 파생, `UI/Foundation`) 1개를 만들고, 임시로 PlayerController에서 화면에 표시.
- 학습 포인트: activate/deactivate lifecycle, `GetDesiredInputConfig()`(`ECommonInputMode::Menu`), `GetDesiredFocusTarget()`.
- **PIE 검증**: 위젯이 표시되고 gamepad 포커스가 안착하는가.

### Stage 2 — CORE "the layout"

- `WBP_PrimaryGameLayout`(`UCuPrimaryGameLayout` 파생) — 우선 **스택 1개**(`UI.Layer.Menu`)만. Overlay 안에 `UCommonActivatableWidgetStack`을 배치하되 **`BindWidget` 이름과 정확히 일치**.
- `NativeOnInitialized`(또는 BP)에서 `RegisterLayer(Tag, Stack)` 명시 호출.
- `UCuGameUIManagerSubsystem→UCuGameUIPolicy`가 per-LocalPlayer로 `WBP_PrimaryGameLayout`을 생성하고, Stage 1 위젯을 `PushWidgetToLayerStack`으로 push. (push 후 `ActivateWidget` 중복 호출 금지.)
- **PIE 검증**: BeginPlay 시 레이아웃 생성 + Stage 1 위젯이 push되어 표시.

### Stage 3 — CORE "navigation + style"

- **스타일 에셋** (`UI/Style`): `CommonButtonStyle` 파생 `CBS_Default`/`CBS_Primary`, `CommonTextStyle` 파생 `CTS_Header`/`CTS_Body`/`CTS_Button`, (선택) `CommonBorderStyle` `CBRS_Panel`. 색상/폰트/브러시를 실제 값으로 설정(`bulk_fill`/`describe` 리플렉션 활용). ⚠️ **여기서 사전 Probe의 (c) 자동화 성패가 판가름** — 브러시/폰트 서브오브젝트가 자동으로 안 되면 수동 fallback으로 처리하고 보고.
- `WBP_ButtonBase`(`UCuButtonBase` 파생) — 스타일 연결 + 텍스트 블록.
- `WBP_TitleScreen`("Press Any Key" → 메인 메뉴 push), `WBP_MainMenu`(Start / Settings / Quit, VerticalBox).
- **PIE 검증**: D-pad/키보드로 포커스 이동, 클릭 시 MainMenu가 push.

### Stage 4 — CORE "input routing + Back" (CommonUI DataTable 경로)

메뉴 입력은 **CommonUI DataTable action 경로**로 배선합니다(Enhanced Input 브릿지 사용 안 함 — Enhanced Input은 gameplay 액션 전용).

- **입력 액션 DataTable** (`Input`): 행 구조체 **`FCommonInputActionDataBase`** — `Back`, `Confirm`(DefaultClick), 필요 시 `NavigateBack`/`TabLeft`/`TabRight`. **표시 이름 + 플랫폼별 키 바인딩**을 담습니다.
- **`UCommonUIInputData` 에셋** (`Input`): `DefaultClickAction` / `DefaultBackAction`을 위 DataTable의 `FDataTableRowHandle`로 지정. → `DefaultGame.ini`의 `[/Script/CommonInput.CommonInputSettings] InputData`에 연결(INI 골격은 Step B-2, 에셋 경로 연결은 여기서).
- **플랫폼 버튼 아이콘**은 별개입니다: `UCommonInputBaseControllerData` **서브클래스 에셋**(FKey → Slate brush 매핑). 위 action DataTable과 **혼동하지 마세요** — action 테이블은 "무슨 액션인가", controller data는 "그 버튼의 그림은 무엇인가".
- 화면 베이스에서 `RegisterUIActionBinding(FBindUIActionArgs)`로 Back/Confirm 처리.
- `UCommonBoundActionBar`를 레이아웃에 배치 → 현재 가능한 bound action을 플랫폼 glyph와 함께 자동 표시(CommonUI 멀티플랫폼 입력의 핵심 시연).
- **PIE 검증**: B(게임패드)/ESC로 MainMenu가 Title로 pop, action bar에 올바른 아이콘 표시.

### Stage 5 — CORE "layers + modal" (core 커리큘럼 종료)

- `WBP_PrimaryGameLayout`을 **4-layer**로 확장: `UI.Layer.Game` / `GameMenu` / `Menu` / `Modal` 각각 `UCommonActivatableWidgetStack` + `RegisterLayer`.
- `WBP_PauseMenu`(`Menu` 레이어, Resume / Settings / Quit to Title), `WBP_ConfirmationModal`(`Modal` 레이어, 제목/본문 + Confirm/Cancel, **하위 레이어 입력 차단**).
- 모달 종료 시 `bAutoRestoreFocus`로 이전 위젯 포커스 복귀.
- **PIE 검증**: 게임플레이 위에 pause → Quit이 modal 오픈 → Cancel이 포커스 복귀.

### Stage 6 — OPTIONAL "MVVM settings 캡스톤" (별도 시스템)

Common UI가 아닌 **독립 시스템**(MVVM)을 완성된 shell 위에 얹는 단계임을 명시.

- `WBP_SettingsScreen`(`UCuSettingsScreenWidget` 파생) + `UCommonTabListWidgetBase` 탭 + 옵션 항목.
- `UCuSettingsViewModel`(`UMVVMViewModelBase`, FieldNotify + **`UE_MVVM_SET_PROPERTY_VALUE`**). 스칼라 1개(예: master volume via **Sound Class/Mix**, `UGameUserSettings` 아님).
- ViewModel을 activatable stack에 push된 뒤에도 유지하려면 **`Resolver` creation mode**(`UMVVMViewModelContextResolver`, DI 방식) 사용 고려.
- ⚠️ **View Binding은 사전 Probe에서 선언한 수동/Experimental-Python fallback 항목**입니다. Monolith가 MVVM binding 저작을 지원하면 자동화하되, 실패하면 `WBP_SettingsScreen`의 바인딩을 수동 체크리스트로 남기고 절차를 문서화하세요.
- **PIE 검증**: 컨트롤 조작 → viewmodel 값 변경 → 바인딩된 label 갱신.

---

### 레벨/프레임워크 연결

- 테스트 맵 생성, GameMode/PlayerController Blueprint 생성.
- 프로젝트 세팅(Default GameMode, Default Map, **Local Player Class = `UCuLocalPlayer`**) 갱신.
- `UCuGameUIManagerSubsystem`(→ Policy)이 `WBP_PrimaryGameLayout`을 생성하고 타이틀 화면을 push하도록 연결.

### 최종 검증 및 마무리

1. 모든 Blueprint의 컴파일 상태를 MCP로 확인하고 오류 수정.
2. `editor` 네임스페이스의 PIE 스모크 테스트(또는 live-PIE driving)로 **타이틀 → 메인 메뉴 → 설정 → 일시정지 → 모달** 흐름이 크래시 없이 동작하는지 확인. (각 Stage에서 이미 검증했으므로 여기서는 통합 회귀.)
3. 로그 캡처로 Common UI 관련 경고/에러 확인(특히 `CommonGameViewportClient` 미설정 시 입력 무음 실패 여부).
4. `README.md` 작성: 프로젝트 개요, **3-세션 워크플로(A 프로젝트+플러그인 / B C++ 뼈대 / C 에셋)와 Stage 1~6 학습 순서**, 생성된 에셋 목록, **Lyra CommonGame 대응 관계 표**, "다음 단계"(Stage 7).

### Lyra CommonGame 대응표 (README에 포함)

| 이 Starter Kit (재구현) | 실제 Lyra / 엔진 | 성격 |
|---|---|---|
| `UCuGameUIManagerSubsystem` | `UGameUIManagerSubsystem`(CommonGame, Lyra가 `ULyraUIManagerSubsystem`로 파생) | player lifecycle — **재구현** |
| `UCuGameUIPolicy` | `UGameUIPolicy`(CommonGame; Lyra `B_LyraUIPolicy`) | LayoutClass 소유·per-player 생성 — **재구현** |
| `UCuPrimaryGameLayout` | `UPrimaryGameLayout`(CommonGame; `W_OverallUILayout`) | 레이어 스택 소유 — **재구현** |
| `UCuLocalPlayer` | `UCommonLocalPlayer`(CommonGame) | per-player 아이덴티티 — **재구현** |
| `UCommonActivatableWidgetStack` | 동일(CommonUI) | **엔진 재사용** |
| `UCommonActivatableWidget` / `UCommonButtonBase` / `UCommonBoundActionBar` / `UCommonTabListWidgetBase` | 동일(CommonUI) | **엔진 재사용** |
| `FCommonInputActionDataBase` / `UCommonUIInputData` / `UCommonInputBaseControllerData` | 동일(CommonInput) | **엔진 재사용** |
| `UMVVMViewModelBase` | 동일(ModelViewViewModel) | **엔진 재사용** |
| (미구현) | `UUIExtensionSubsystem` / `UGameFeatureAction_AddWidgets`(UIExtension, CommonGame) | Stage 7 "다음 단계" |

### Stage 7 — "다음 단계" (문서만, 이 kit에서는 빌드하지 않음)

- `UUIExtensionSubsystem`(HUD extension point 태그 → 위젯) + `UGameFeatureAction_AddWidgets`로 GameFeature/Experience가 레이아웃을 몰라도 UI를 주입하는 **모듈러 UI** 패턴.
- ⚠️ **4개 navigation layer(스택)와 혼동 금지**: layer stack = "화면 내비게이션", extension point = "모듈러 주입". 서로 다른 개념.

### 제약 및 주의사항

- deprecated API 사용 금지 (불확실하면 `source.check_deprecations`로 확인).
- 파괴적인 액션(에셋 삭제 등) 사용 금지.
- MCP로 자동화가 불가능한 작업은 **건너뛰지 말고** 해당 항목만 **수동 작업 체크리스트**로 별도 정리(사전 Probe 결과와 일관).
- **각 Stage는 이전 Stage의 PIE 검증 통과를 전제로 한다.** 순서를 건너뛰지 말 것.
- 각 Stage 완료 시 생성/수정한 에셋 목록을 요약 보고.
- 입력 모드에 `SetInputMode*` 직접 호출 금지, `ECommonInputMode`에 `GameAndMenu` 없음(=`All`), `.uproject`에 `CommonInput`/`EnhancedInput`을 plugin으로 넣지 말 것 — 이 세 가지는 반복되는 함정. **함정 전체 목록의 정본은 [`docs/common-ui-coding-reference.md`](docs/common-ui-coding-reference.md) §10 체크리스트**다.

작업 시작 전에 `monolith_discover()` 결과와 역량 probe 결과를 먼저 요약하고, 현재 액션으로 구현 불가능한 요구사항은 대안(수동 fallback)과 함께 보고한 뒤 진행하세요.
