# Common UI Starter Kit — 코딩 레퍼런스 (Coding Reference)

> **이 문서는 코드 작성 시 참조하는 정본(source of truth)입니다.** `common-ui-starter-kit-prompt.md`는 "무엇을 만들지"를, 이 문서는 "어떻게 정확히 쓸지"(API idiom · 함정 · 네이밍)를 담습니다.

## 0. 이 문서 사용법

- **언제 참조하나**: 세션 B(C++ 뼈대 작성) 전반, 세션 C(BP graph·style·MVVM 저작) 전반. 코드를 쓰기 직전에 관련 섹션을 먼저 읽는다.
- **언어 규칙**: 한국어 서술 + 기술 용어는 영어 원문(`Common UI`, `Activatable Widget`, `MVVM` 등).
- **대상 버전**: **UE 5.8**(5.7 fallback). 버전에 민감한 시그니처는 §9 절차로 확인.
- **스코프**: Common UI / CommonGame / MVVM / CommonInput의 **프로젝트 고유 idiom · 함정 · 네이밍**만 다룬다. Epic의 일반 C++ 표준 전체는 재생산하지 않고 §1에서 링크만 한다.
- **빠르게 훑을 때**: §10 함정 체크리스트부터 본다.

---

## 1. C++ 스타일 기본 (Epic 표준 요약)

> 전체는 [Epic C++ Coding Standard for Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine) 원문 참조. 아래는 이 kit이 반드시 지킬 핵심만.

- **접두사**: `U`(UObject 파생), `A`(AActor 파생), `F`(그 외 struct/class), `E`(enum), `T`(template), `S`(SWidget 파생), `I`(interface), `b`(bool 변수 — `bIsActive`).
- **네이밍**: 타입·변수·함수 모두 **PascalCase**(`camelCase` 아님). bool 반환 함수는 질문형(`IsVisible()`, `ShouldClearBuffer()`). 참조로 반환되는 out 파라미터는 `Out` 접두사(`OutResult`).
- **const-correctness**: 파라미터·메서드에 적극 적용. ⚠️ **반환형에는 `const`를 붙이지 않는다**(move semantics 저해).
- **포인터**: `nullptr` 사용(`NULL` 금지). UObject 멤버 `UPROPERTY()`는 **`TObjectPtr<T>`**(raw `T*` 대신). UObject는 **포인터로 전달**(레퍼런스 아님), null 미허용이면 주석으로 명시.
- **override/final**: 적극 사용 권장. 파생 의도 없는 클래스는 `final`.
- **`auto` 지양**: 타입을 명시. (lambda·장황한 iterator·template 예외만.)
- **include 규율(IWYU)**: forward declaration을 우선. 필요한 헤더는 **직접** include(간접 의존 금지). 모든 헤더에 `#pragma once`.
- **포맷**: 탭 들여쓰기(탭=4), 중괄호는 **새 줄**, 단일 문장 블록에도 중괄호.
- **주석**: 자기설명적 코드 우선. public API는 JavaDoc 스타일. 나쁜 코드를 주석으로 덮지 말고 다시 쓴다.

---

## 2. 네이밍 & 에셋 규칙

**C++ 클래스**: 화면 베이스는 `UStarter*`/화면명(`UTitleScreenWidget`, `UMainMenuWidget` …), 레이어링은 `UGameUIManagerSubsystem`/`UGameUIPolicy`/`UPrimaryGameLayout`/`UCommonLocalPlayer`.

**에셋 접두사** (Epic 권장 + 커뮤니티 표준):

| 유형 | 접두사 | 예 |
|------|--------|-----|
| Widget Blueprint | `WBP_` | `WBP_MainMenu`, `WBP_PrimaryGameLayout` |
| Blueprint class(GameMode/PC 등) | `BP_` | `BP_StarterGameMode` |
| CommonButtonStyle | `CBS_` | `CBS_Default`, `CBS_Primary` |
| CommonTextStyle | `CTS_` | `CTS_Header`, `CTS_Body`, `CTS_Button` |
| CommonBorderStyle | `CBRS_` | `CBRS_Panel` |
| InputAction | `IA_` | `IA_UI_Back`, `IA_UI_Confirm` |
| InputMappingContext | `IMC_` | `IMC_UI` |
| DataTable | `DT_` | `DT_UIActions` |
| Data Asset(UCommonUIInputData 등) | `DA_` | `DA_CommonInputData` |
| Level/Map | `L_` | `L_StarterMap` |

> `CBS_`/`CTS_`/`CBRS_`는 이 kit이 채택한 프로젝트 관례다(Common UI style 에셋에 대한 공식 표준 접두사는 없음). 일관되게만 쓰면 된다.

**GameplayTag**: `UI.Layer.Game` / `UI.Layer.GameMenu` / `UI.Layer.Menu` / `UI.Layer.Modal`. 네이티브 선언은 `UE_DECLARE_GAMEPLAY_TAG_EXTERN`(헤더) + `UE_DEFINE_GAMEPLAY_TAG`(cpp).

**Content 경로**: `/Game/UI/Foundation`, `/Game/UI/Menu`, `/Game/UI/HUD`, `/Game/UI/Style`, `/Game/Input`.

> ⚠️ **BindWidget 이름 일치**(핵심): `UPROPERTY(meta=(BindWidget))` C++ 멤버 이름 == WBP 자식 위젯 이름을 **엄격히** 일치시킨다. 상세는 §6.

---

## 3. 모듈 · 플러그인 · Build.cs

- ⚠️ **`CommonInput`은 별도 plugin이 아니라 `CommonUI` 플러그인 내부 module**이다. `.uproject`에 plugin으로 넣지 말 것(“plugin not found” 경고). `CommonUI` plugin만 활성화하고, `CommonInput`은 Build.cs의 module dep으로만 참조.
- ⚠️ **`EnhancedInput`은 엔진 기본 활성화 plugin**이다. `.uproject`에 넣지 말고 Build.cs dep으로만.
- `.uproject` 활성화 plugin: `CommonUI`, `ModelViewViewModel`, `Monolith`.
- **Build.cs 의존 module**(kit 실제 사용분): `Core`, `CoreUObject`, `Engine`, `InputCore`, `CommonUI`, `CommonInput`, `EnhancedInput`, `UMG`, `Slate`, `SlateCore`, `ModelViewViewModel`, `GameplayTags`.
  > 이 kit은 Lyra `CommonGame`을 **복사하지 않고 패턴만 재구현**하므로 `CommonGame`/`CommonUser`/`GameFeatures`/`ModularGameplay` dep은 넣지 않는다.
- ⚠️ **헤더를 include하면 대응 module을 Build.cs에 반드시 추가**한다. 누락 시 unresolved external / missing include 빌드 에러(런타임 아님). 예: `UMVVMViewModelBase.h` → `ModelViewViewModel`.

---

## 4. Config INI 배치

⚠️ **CommonUI/CommonInput/viewport 키는 `DefaultInput.ini`가 아니다.** 잘못된 파일에 넣으면 **무시되어 무음 무효과**.

| 키 | 파일 |
|----|------|
| `GameViewportClientClassName` | `DefaultEngine.ini` |
| `[/Script/CommonInput.CommonInputSettings]` (`InputData` 등) | `DefaultEngine.ini` |
| `UGameUIManagerSubsystem`의 `DefaultUIPolicyClass`(config) | `DefaultGame.ini` |
| Local Player Class | `DefaultEngine.ini` |
| legacy Action/Axis, Enhanced Input 기본값 | `DefaultInput.ini` |

**[치명적, 절대 누락 금지]** — 없으면 `UCommonUIActionRouter`가 입력을 못 받아 gamepad 내비게이션·focus·Back이 **무음으로 전부 죽는다**(마우스만 동작해 버그가 숨음). CommonUI 최다 셋업 실패 원인.

```ini
; DefaultEngine.ini  (NOT DefaultInput.ini)
[/Script/Engine.Engine]
GameViewportClientClassName=/Script/CommonUI.CommonGameViewportClient
```

> 다중 config layer(Default*/per-platform/`Saved/Config`)가 서로 오버라이드하면 "설정이 안 먹는" 것처럼 보인다. 정본 키는 `Default*.ini`(소스 관리)에 두고 병합값을 `Saved/Config`에서 검증.

---

## 5. Input routing · FUIInputConfig · ECommonInputMode

- ⚠️ **`APlayerController::SetInputMode*`(`GameOnly`/`GameAndUI`/`UIOnly`)·`FlushPressedKeys`를 직접 호출하지 말 것.** `UCommonUIActionRouterBase::ApplyInputConfig`와 충돌해 비결정적 입력 상태가 된다. 대신 **`UCommonActivatableWidget::GetDesiredInputConfig()` override**로 `FUIInputConfig`를 반환한다(빈/null config = 마지막 유효 config 상속).
- ⚠️ **`ECommonInputMode`의 유효값은 `Menu`/`Game`/`All`뿐.** `GameAndMenu`·`GameAndUI`는 **존재하지 않아 컴파일 에러**(둘 다 원하면 `All`).
- cursor/capture는 mode가 암시하지 않는 **별도 축**(`EMouseCaptureMode` + hide-cursor 필드)이므로 `FUIInputConfig`에 명시 설정.
- `CommonUI.Debug.CheckGameViewportClientValid=0`으로 경고를 억지로 끄지 말 것 — 그 경고는 §4 viewport client 누락의 증상이다.

```cpp
// UStarterActivatableWidget.h
virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

// UStarterActivatableWidget.cpp
TOptional<FUIInputConfig> UStarterActivatableWidget::GetDesiredInputConfig() const
{
    return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}
```

---

## 6. Activatable widget · Layer stack

- ⚠️ **`PushWidgetToLayer*`로 push한 위젯에 `ActivateWidget`을 다시 호출하지 말 것.** push가 이미 activate하므로 이중 activation(→ `OnActivated` 중복·focus 다툼·input config 중복 적용).
- **Visibility 토글로 스택 메뉴를 show/hide 하지 말 것.** Activatable stack이 activation/visibility/focus를 함께 관리한다 — push/pop 또는 Activate/Deactivate로.
- ⚠️ **`RegisterLayer(Tag, Container)`는 `BindWidget`만으로 tag→container 맵이 안 채워진다.** `NativeOnInitialized`에서 각 스택을 **명시적으로** 등록한다.
- push API: `PushWidgetToLayerStack<T>(Tag, TSubclassOf<T>)`(동기, hard class) / `PushWidgetToLayerStackAsync<T>(Tag, TSoftClassPtr<T>, ...)`(비동기, soft class + streaming handle — 실제 Lyra가 메뉴/모달을 여는 방식). 정적 접근자 `GetPrimaryGameLayout(APlayerController*)`.
- 포커스: `NativeGetDesiredFocusTarget()` / `BP_GetDesiredFocusTarget` override + `bAutoRestoreFocus`(스택 복귀 시 복원).
- ⚠️ **BindWidget 이름 일치**: `UPROPERTY(meta=(BindWidget))`는 변수 이름으로 **엄격 매칭**한다. 불일치 시 WBP 컴파일 실패(hard bind) 또는 런타임 null(`meta=(BindWidgetOptional)`). 디자이너에서 위젯 rename 시 C++ 프로퍼티도 **동시** 변경. 흔한 증상: "rename 후 버튼이 아무 반응 없음".

```cpp
// UPrimaryGameLayout.h — BindWidget 이름 == WBP 자식 위젯 이름
UPROPERTY(meta = (BindWidget))
TObjectPtr<UCommonActivatableWidgetStack> Layer_Menu;

// UPrimaryGameLayout.cpp
void UPrimaryGameLayout::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    RegisterLayer(TAG_UI_Layer_Menu, Layer_Menu);   // BindWidget만으로는 부족
}
```

---

## 7. MVVM ViewModel · View Binding

- ⚠️ **FieldNotify setter는 반드시 `UE_MVVM_SET_PROPERTY_VALUE`를 쓴다.** 멤버를 직접 대입하면 FieldNotify delegate가 broadcast되지 않아 **바인딩된 위젯이 갱신되지 않는 완전 무음 실패**.
- 파생/computed FieldNotify는 소스 setter에서 `UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(DerivedField)`로 추가 통지.
- ⚠️ **View ↔ ViewModel 바인딩은 C++-only로 저작할 수 없다.** 바인딩은 WBP의 View Bindings 패널(에디터 extension)에 저장된다. C++에서는 프로퍼티를 `FieldNotify`로 표시하고, **바인딩은 WBP 패널**에서 만든다(프로그램 경로는 Experimental Python뿐 → **세션 C의 수동 fallback 항목**).
- viewmodel 인스턴스를 위젯 slot에 **실제 할당**해야 바인딩이 해석된다(creation mode 또는 `SetViewModel`).

```cpp
// USettingsViewModel.cpp — load-bearing 매크로
void USettingsViewModel::SetMasterVolume(float NewValue)
{
    if (UE_MVVM_SET_PROPERTY_VALUE(MasterVolume, NewValue))   // 대입 + broadcast(변경 시에만)
    {
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MasterVolumeText);  // 파생 필드 통지
    }
}
```

---

## 8. Settings · Audio

- ⚠️ **`UGameUserSettings`에는 master(또는 채널) volume 프로퍼티가 없다.** 볼륨 슬라이더를 존재하지 않는 필드에 배선하는 무음 dead-end 금지.
- 볼륨은 **`USoundClass` + `USoundMix`**(`SetSoundMixClassOverride` / `PushSoundMixModifier`) 또는 Audio Modulation control bus로 구동.
- 값 영속화: **`UGameUserSettings` 서브클래스**(`UPROPERTY(config)` 필드 추가 + `GameUserSettingsClassName`을 서브클래스로) 또는 커스텀 `SaveGame`. base `UGameUserSettings`에 커스텀 필드를 얹으면 직렬화되지 않는다.
  > 이 kit의 §7 MVVM 데모(master volume)는 **SoundClass/Mix 구동 + `UGameUserSettings` 서브클래스 영속화**를 기본으로 한다.

---

## 9. Deprecated API · 버전 노트

- ⚠️ 오래된 튜토리얼의 **deprecated CommonUI API 복붙 금지**(구 `PushContentToLayer` 오버로드, pre-5.x input-config helper 등은 5.x에서 renamed/deprecated). strict 설정에서 deprecated 경고가 에러가 되고 동작도 다를 수 있다.
- **확인 방법**: 세션 C는 Monolith `source` 네임스페이스(`get_signature` / `verify_symbols` / `check_deprecations`), 세션 B는 엔진 헤더/공식 5.8 문서로 확인.
- **불확실 시그니처(예: `PushWidgetToLayerStackAsync`의 파라미터 순서·핸들 타입)는 반드시 확인 후 사용.** 추측 금지.

---

## 10. 함정 빠른참조 체크리스트 (Pitfalls quick-ref)

> **DON'T → WHY → FIX.** 굵은 4개가 최다·최고위험.

| DON'T | WHY | FIX | 상세 |
|-------|-----|-----|------|
| **`GameViewportClientClassName` 미설정** | UIActionRouter가 입력 못 받음 → gamepad/focus/Back **무음 사망**(마우스만 동작해 은폐) | DefaultEngine.ini에 `=/Script/CommonUI.CommonGameViewportClient` | §4 |
| **`SetInputMode*` 직접 호출** | action router와 충돌 → 비결정적 입력 | `GetDesiredInputConfig()` override | §5 |
| **`ECommonInputMode::GameAndMenu`/`GameAndUI` 사용** | 존재하지 않음 → 컴파일 에러 | `Menu`/`Game`/`All` | §5 |
| **FieldNotify setter 직접 대입** | broadcast 안 됨 → 바인딩 **무음 미갱신** | `UE_MVVM_SET_PROPERTY_VALUE` | §7 |
| `CommonInput`/`EnhancedInput`을 `.uproject` plugin으로 | plugin 아님(module/엔진기본) → 경고/빌드 문제 | Build.cs module dep으로만 | §3 |
| push 후 `ActivateWidget` 재호출 | 이중 activation → focus/OnActivated 중복 | push가 activate; 재호출 금지 | §6 |
| `RegisterLayer` 생략(BindWidget만) | tag→container 맵 비어 push 실패 | `NativeOnInitialized`에서 등록 | §6 |
| BindWidget 이름 불일치 | WBP 컴파일 실패 또는 런타임 null | C++ 멤버명 == WBP 위젯명 | §2·§6 |
| CommonUI 키를 `DefaultInput.ini`에 | 무시됨 → 무음 무효과 | `DefaultEngine.ini`/`DefaultGame.ini` | §4 |
| `UGameUserSettings.MasterVolume` 기대 | 그런 필드 없음 | SoundClass+SoundMix + 서브클래스 저장 | §8 |
| deprecated CommonUI API 복붙 | 5.8에서 renamed/deprecated | source/헤더로 확인 후 사용 | §9 |

---

### 참고 출처
- [Epic C++ Coding Standard](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine)
- [CommonUI Input Technical Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/commonui-input-technical-guide-for-unreal-engine) · [Input Fundamentals for CommonUI](https://dev.epicgames.com/documentation/unreal-engine/input-fundamentals-for-commonui-in-unreal-engine)
- [UMG ViewModel (MVVM)](https://dev.epicgames.com/documentation/en-us/unreal-engine/umg-viewmodel-for-unreal-engine)
- [x157 UE5 CommonUI/CommonGame 노트](https://x157.github.io/UE5/CommonUI/) · [benui.ca](https://benui.ca/)
