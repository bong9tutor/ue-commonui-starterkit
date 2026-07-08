// CommonUIStarterKitEditor.Target.cs — Editor 타깃 규칙.
// 학습 노트:
//  - 세션 A의 "플러그인 컴파일 게이트 빌드"와 세션 B/C의 에디터 실행은 모두 이 Editor 타깃을 사용한다.
//    (빌드 커맨드: CommonUIStarterKitEditor Win64 Development)
//  - Game 타깃과 달리 에디터 전용 module(UnrealEd 등)까지 링크되는 타입.
using UnrealBuildTool;
using System.Collections.Generic;

public class CommonUIStarterKitEditorTarget : TargetRules
{
	public CommonUIStarterKitEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.Add("CommonUIStarterKit");
	}
}
