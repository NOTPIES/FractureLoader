#pragma once
#include <cstdint>
#include <iostream>
#include "../../magic_enum/magic_enum.hpp"

enum CClient_PlatformEnum
{
	PC,
	XBONE,
	PS4,
	MOBILE,
	SWITCH
};

enum CClient_DemoType
{
	None,
	E3Demo,
	E32017Demo,
	E32017Shirt
};

namespace Snowdrop
{
	enum class EGameActionState : int32_t
	{
		Startup = 0,
		Login = 1,
		MainMenu = 2,
		SetupGame = 3,
		Loading = 4,
		Normal = 5, // default
		Combat = 7,
		Cinematic = 8,
		MessageBoxUI = 9,
		NewUITest = 10,
		SystemPauseMenu = 11,
		CinematicPauseMenu = 12,
		GameOver = 13,
		SystemInit = 14,
		Minigame = 15,
		SavegameUI = 16,
		GenderSelection = 17,
		Legal3rdParty = 18,
		PressStart = 19,
		AutoSaveWarning = 20,
		Selfie = 21,
		InGameUIPause = 22,
		PauseMenu = 6
	};

	enum ETimeOfDayType
	{
		None,
		Day,
		Night
	};
} // namespace Snowdrop
