// CommonUIStarterKit.Build.cs — 이 게임 module의 의존성 규칙.
//
// 학습 노트 (레퍼런스 §3 근거):
//  - .uproject에는 plugin으로 CommonUI / ModelViewViewModel / Monolith 만 활성화한다.
//  - ⚠️ CommonInput 은 별도 plugin이 아니라 CommonUI plugin '내부의 module'이다.
//    → .uproject에 넣으면 "plugin not found" 경고. 여기 Build.cs의 module dep으로만 참조한다.
//  - ⚠️ EnhancedInput 은 엔진 기본 활성화 plugin이다. .uproject에 넣지 않고 여기 dep으로만 참조.
//  - 헤더를 include하면 반드시 대응 module을 여기에 추가한다(누락 시 링크/컴파일 에러).
//
// 참고(역사적 경위): 이 dep 목록은 세션 A의 "플러그인 컴파일 게이트 빌드"(빈 module 상태로
// 이 module들이 UE 5.8에서 정상 링크되는지를 C++ 작성 '전에' 먼저 검증한 단계)에서 확정됐다.
// 지금은 System/Layout/Widgets/Screens/ViewModels 하위폴더에 이 dep을 실제로 쓰는
// C++ 클래스 20여 개가 들어 있다. 각 dep이 왜 필요한지는 아래 개별 항목 주석을 본다.
using UnrealBuildTool;

public class CommonUIStarterKit : ModuleRules
{
	public CommonUIStarterKit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 이 module은 Public/Private 폴더를 쓰지 않고 역할별 하위폴더(System/Layout/Widgets/…)로 구성한다.
		// UBT는 module 루트를 include 경로에 '자동으로' 넣지 않으므로(Public 폴더가 있을 때만 그 루트를 추가),
		// 여기서 module 루트를 명시적으로 등록한다. → #include "System/CuGameUIPolicy.h" 처럼
		// 루트 기준 경로 한정으로 하위폴더 간 include가 해석된다.
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			// --- 엔진 코어 ---
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",

			// --- Common UI / CommonInput (CommonInput은 CommonUI plugin 내부 module) ---
			"CommonUI",
			"CommonInput",

			// --- Enhanced Input (엔진 기본 plugin; gameplay 액션 전용) ---
			"EnhancedInput",

			// --- UMG / Slate (위젯 UI) ---
			"UMG",
			"Slate",
			"SlateCore",

			// --- MVVM (Stage 6 설정 화면 캡스톤) ---
			"ModelViewViewModel",
			// FieldNotify UPROPERTY 생성 코드가 참조하는 module (UE::FieldNotification::*)
			"FieldNotification",

			// --- GameplayTag (UI.Layer.* 레이어 식별) ---
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// 현재 없음. 세션 B에서 필요 시 추가.
		});
	}
}
