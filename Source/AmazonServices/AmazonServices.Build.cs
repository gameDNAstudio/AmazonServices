// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class AmazonServices : ModuleRules
	{
		public AmazonServices(TargetInfo Target)
		{
			Definitions.Add("AMAZONSERVICES_PACKAGE=1");

			PrivateIncludePaths.AddRange(
				new string[] {
					"AmazonServices/Private",
			});

			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"Core",
					"CoreUObject",
					"Engine",
					"Sockets",
					"OnlineSubsystem",
					"Http",
					"AndroidRuntimeSettings",
				}
			);

			// Get Settings from Config Cache
			ConfigCacheIni Ini = ConfigCacheIni.CreateConfigCacheIni(Target.Platform, "Engine", new DirectoryReference(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../"))));
			bool bEnableAmazonSupport = false;
			if (!Ini.GetBool("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings", "bEnableAmazonSupport", out bEnableAmazonSupport))
			{
				bEnableAmazonSupport = false;
			}

			// Additional Frameworks and Libraries for Android
			if (Target.Platform == UnrealTargetPlatform.Android)
			{
				PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
				string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, BuildConfiguration.RelativeEnginePath);
				AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "AmazonServices_APL.xml")));
			}
		}
	}
}
