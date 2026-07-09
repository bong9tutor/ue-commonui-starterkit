// CommonUIStarterKit.Target.cs — Game(런타임/쿠킹) 타깃 규칙.
// 학습 노트:
//  - Target.cs는 "무엇을(모듈) 어떤 타입으로(Game/Editor) 빌드할지"를 정의한다.
//  - 실제 코드 의존성(어떤 module에 링크할지)은 Build.cs가 담당한다.
//  - DefaultBuildSettings/IncludeOrderVersion을 Latest로 두면 UE 5.8 최신 규칙(IWYU 등)을 따른다.
using UnrealBuildTool;

public class CommonUIStarterKitTarget : TargetRules
{
	public CommonUIStarterKitTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		// 이 타깃에 포함할 primary game module.
		ExtraModuleNames.Add("CommonUIStarterKit");
	}
}
