// CommonUIStarterKit.Build.cs — 이 게임 module의 의존성 규칙.
//
// 학습 노트 (레퍼런스 §3 근거):
//  - .uproject에는 plugin으로 CommonUI / ModelViewViewModel / Monolith 만 활성화한다.
//  - ⚠️ CommonInput 은 별도 plugin이 아니라 CommonUI plugin '내부의 module'이다.
//    → .uproject에 넣으면 "plugin not found" 경고. 여기 Build.cs의 module dep으로만 참조한다.
//  - ⚠️ EnhancedInput 은 엔진 기본 활성화 plugin이다. .uproject에 넣지 않고 여기 dep으로만 참조.
//  - 헤더를 include하면 반드시 대응 module을 여기에 추가한다(누락 시 링크/컴파일 에러).
//
// 세션 A(현재)는 아직 실제 UI 클래스가 없다(빈 module). 그럼에도 아래 의존 module을
// 모두 나열하는 이유: "플러그인 컴파일 게이트 빌드"에서 이 module들이 UE 5.8에서
// 정상 링크되는지(툴체인 + 플러그인 가용성)를 C++ 작성 '전에' 검증하기 위함.
using UnrealBuildTool;

public class CommonUIStarterKit : ModuleRules
{
	public CommonUIStarterKit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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

			// --- GameplayTag (UI.Layer.* 레이어 식별) ---
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// 현재 없음. 세션 B에서 필요 시 추가.
		});
	}
}
