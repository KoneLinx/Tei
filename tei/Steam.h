#pragma once

#include <string_view>


	// Interface not finalised

	bool SteamInitialize();
	void SteamUpdate();
	void SteamCleanup();

	bool SteamSetAchievement(std::string_view);



