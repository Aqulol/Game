using UnrealBuildTool;
using System.Collections.Generic;

public class SettlementGameTarget : TargetRules
{
    public SettlementGameTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("SettlementGame");
    }
}
