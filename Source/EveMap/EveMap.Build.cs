// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EveMap : ModuleRules
{
	public EveMap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Slate", "SlateCore", "UMG", "NavigationSystem", "AIModule", "Niagara", "GameplayTasks", "GameplayTags" });
	}
}
