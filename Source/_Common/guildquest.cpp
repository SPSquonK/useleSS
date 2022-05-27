#include "stdafx.h"

#include "guildquest.h"
#include "guild.h"
#include "user.h"
#include "dpdatabaseclient.h"

void CGuildQuestProcessor::Process()
{
	DWORD dwTickCount	= GetTickCount();

	for( int i = 0; i < m_pElem.size(); i++)
	{
		GroupQuest::QuestElem * pElem = &m_pElem[i];
		if( pElem->nId == i )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pElem->idGroup );
//			if( !pGuild )
//				continue;

			GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( pElem->nId );
			if( !pProp )
				continue;

			if( pElem->dwEndTime < dwTickCount )
			{
				switch( pElem->nProcess )
				{
					case GroupQuest::ProcessState::Wormon:
						{
							TRACE( "GQP_WORMON - r\n" );

							CRect rect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

							if( pGuild )
							{
								pGuild->SetQuest( pElem->nId, pElem->nf );
								g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, pElem->nId, pElem->nf );
								pGuild->ReplaceLodestar( rect );
							}

							CMover* pWormon	= prj.GetMover( pElem->objidWormon );
							if( pWormon )
								pWormon->Delete();
							RemoveQuest( pElem->nId );
						}
						break;
					case GroupQuest::ProcessState::GetItem:
						{
							TRACE( "GQP_GETITEM - r\n" );

							CRect rect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

							if( pGuild )
							{
								pGuild->ReplaceLodestar( rect );
							}

							RemoveQuest( pElem->nId );
						}
						break;
					default:
						break;
				}
			}
			else
			{
				switch( pElem->nProcess )
				{
					case GroupQuest::ProcessState::Wormon:
						{
							TRACE( "GQP_WORMON - p\n" );
							
							if( ++pElem->nCount < 10 )
								continue;

							CRect rect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bsurvivor	= FALSE;	// is survivor exists?

							if( pGuild )
							{
								CUser* pUser;
								CGuildMember* pMember;
								for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
								{
									pMember	= i->second;
									pUser	= prj.GetUserByID( pMember->m_idPlayer );
									if( IsValidObj( pUser ) )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) && pUser->IsLive() )
										{
											bsurvivor	= TRUE;
											break;
										}
									}
								}
							}

							if( !bsurvivor  )
							{
								if( pGuild )
								{
									pGuild->SetQuest( pElem->nId, pElem->nf );
									g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, pElem->nId, pElem->nf );
									pGuild->ReplaceLodestar( rect );
								}

								CMover* pWormon	= prj.GetMover( pElem->objidWormon );
								if( pWormon )
									pWormon->Delete();
								RemoveQuest( pElem->nId );
							}
						}
						break;
					case GroupQuest::ProcessState::GetItem:
						{
							TRACE( "GQP_GETITEM - p\n" );
							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bexist	= FALSE;	// is member here?

							if( pGuild )
							{
								CUser* pUser;
								CGuildMember* pMember;
								for( auto i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
								{
									pMember	= i->second;
									pUser	= prj.GetUserByID( pMember->m_idPlayer );
									if( IsValidObj( pUser ) )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) )
										{
											bexist	= TRUE;
											break;
										}
									}
								}
							}

							if( !bexist )
							{
								if( pGuild )
								{
									pGuild->ReplaceLodestar( rect );
								}
								RemoveQuest( pElem->nId );
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

CGuildQuestProcessor* CGuildQuestProcessor::GetInstance( void )
{
	static CGuildQuestProcessor sGuildQuestProcessor;
	return &sGuildQuestProcessor;
}

