// Copyright 2021 (C) Bruno Xavier Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {
//
public class SimpleInventory : ModuleRules {
    public SimpleInventory(ReadOnlyTargetRules Target) : base(Target) {
        PrivatePCHHeaderFile = "Public/SimpleInventory_Shared.h";
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		//
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"UMG",
				"Core",
				"Slate",
				"Engine",
				"Savior3",
				"SlateCore",
				"CoreUObject"
			}///
		);//
		//
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Classes"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"Private"));
	}///
}///
//
}