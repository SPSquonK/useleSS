#pragma once

#include "FlyFFTypes.h"

#include <variant>
#include <tuple>

namespace RunScriptFunc {
	struct Message {
		TCHAR szWord[64];
		TCHAR szKey[64];
		DWORD dwParam;
		QuestId dwParam2;
	};

	struct AddKey    : public Message { };
	struct AddAnswer : public Message { };

	struct RemoveKey {
		char	lpszVal1[1024];
	};

	struct Say {
		char	lpszVal1[1024];
		QuestId   dwVal2;
	};

	struct InitStat { DWORD dwVal1; };
	struct InitStr { DWORD dwVal1; };
	struct InitSta { DWORD dwVal1; };
	struct InitDex { DWORD dwVal1; };
	struct InitInt { DWORD dwVal1; };

	struct Quest {
		bool isNew;
		char	lpszVal1[1024];
		char	lpszVal2[1024];
		QuestId   dwVal2;
	};

	struct RemoveAllKey {};
	struct Exit {};
	struct QuestSetPlayerName {};

	using Variant = std::variant<
		AddKey, AddAnswer,
		RemoveKey, RemoveAllKey, Say,
		InitStat, InitStr, InitSta, InitInt, InitDex,
		Quest,
		Exit, QuestSetPlayerName
	>;
}

