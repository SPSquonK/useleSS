#include "stdafx.h"
#include "EventMonster.h"

#ifdef __EVENT_MONSTER

CEventMonster::Instance CEventMonster::instance;

void CEventMonster::Instance::LoadScript()
{
	CLuaBase lua;
	if( lua.RunScript( "EventMonster.lua" ) != 0 )
	{
		Error( "LUA - EventMonster.lua Failed!!!" );
		ASSERT(0);
	}

	lua.GetGloabal( "tEventMonster" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		Prop EventMonster;

		const DWORD	dwId = CScript::GetDefineNum( lua.GetFieldToString( -1, "strMonsterId" ) );
		EventMonster.nLevel = static_cast<int>( lua.GetFieldToNumber( -1, "nLevel" ) );
		EventMonster.dwLootTime = static_cast<DWORD>( lua.GetFieldToNumber( -1, "nLootTime" ) );
		EventMonster.fItemDropRange = static_cast<float>( lua.GetFieldToNumber( -1, "fItemDropRange" ) );
		EventMonster.bPet = lua.GetFieldToBool( -1, "bPet" );
		EventMonster.bGiftBox = lua.GetFieldToBool( -1, "bGiftBox" );
		
		m_mapEventMonster.emplace(dwId, EventMonster);
		lua.Pop( 1 );
	}

	lua.Pop(0);
}

const CEventMonster::Prop * CEventMonster::Instance::GetEventMonster(const DWORD dwId) const
{
	const auto it = m_mapEventMonster.find(dwId);
	return it != m_mapEventMonster.end() ? &it->second : nullptr;
}

#endif // __EVENT_MONSTER