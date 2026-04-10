// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ObjectPooling : ModuleRules
{
	public ObjectPooling(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);
	}
}
