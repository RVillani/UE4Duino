using UnrealBuildTool;

public class UE4Duino : ModuleRules
{
    public UE4Duino(ReadOnlyTargetRules Target) : base(Target)
    {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[] { "UE4Duino/Private" });

        PrivateDependencyModuleNames.AddRange(
            new string[]
			{
                "Engine",
                "Core",
                "CoreUObject"
            }
        );
    }
}