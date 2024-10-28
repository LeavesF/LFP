// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LookingForPanduria : ModuleRules
{
	public LookingForPanduria(ReadOnlyTargetRules Target) : base(Target)
	{
        CppStandard = CppStandardVersion.Cpp20;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "UMG", "ProceduralMeshComponent" });
    }
}
