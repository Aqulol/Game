using UnrealBuildTool;
using System.Collections.Generic;

public class SettlementGameEditorTarget : TargetRules
{
    public SettlementGameEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("SettlementGame");
    }
}
