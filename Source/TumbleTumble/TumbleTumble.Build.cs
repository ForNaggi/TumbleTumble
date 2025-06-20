// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TumbleTumble : ModuleRules
{
	public TumbleTumble(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
            "Sockets",
            "Networking",
			"UMG",
            "Slate",
            "SlateCore"
        });
	}
}
