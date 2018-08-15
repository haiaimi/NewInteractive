// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class WarSimulate : ModuleRules
{
	public WarSimulate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Landscape"});

		PrivateDependencyModuleNames.AddRange(new string[] {"Slate", "SlateCore" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PrivateIncludePaths.AddRange(
             new string[] {
                   "WarSimulate/Private/UI/Widgets"
             }
        );
	}
}
