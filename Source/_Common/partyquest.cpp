#include "stdafx.h"

#ifdef __WORLDSERVER
#include "partyquest.h"
#include "party.h"
#include "user.h"
#include "dpdatabaseclient.h"
#include "worldmng.h"
#include "GroupUtils.h"


void CPartyQuestProcessor::Process()
{
	DWORD dwTickCount	= GetTickCount();
	for( int i = 0; i < m_pElem.size(); i++)
	{
		GroupQuest::QuestElem * pElem = &m_pElem[i];
		if( pElem->nId == i )
		{
			CParty* pParty	= g_PartyMng.GetParty( pElem->idGroup );
//			if( !pGuild )
//				continue;

			PARTYQUESTPROP* pProp	= prj.GetPartyQuestProp( pElem->nId );
			if( !pProp )
				continue;

			// 제한 시간 지났다...
			if( pElem->dwEndTime < dwTickCount )
			{
				switch( pElem->nProcess )
				{
					case GroupQuest::ProcessState::Wormon:
						{
							TRACE( "PQP_WORMON - r\n" );

							CRect rect(pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

							Boom(pElem->nId, rect, pProp->dwWorldId);
						}
						break;
					case GroupQuest::ProcessState::GetItem:
						{
							TRACE( "PQP_GETITEM - r\n" );

							CRect rect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

							Boom(pElem->nId, rect, pProp->dwWorldId);
						}
						break;
					default:
						break;
				}
			}
			else
			// 아직 열시미 잡고있는중이다~
			{
				switch( pElem->nProcess )
				{
					case GroupQuest::ProcessState::Wormon:
						{
							TRACE( "PQP_WORMON - p\n" );
							
							if( ++pElem->nCount < 10 )
								continue;

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bsurvivor	= FALSE;	// is survivor exists?

							if( pParty )
							{
								CUser* pUser;
								
								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{										
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) && pUser->IsLive() )
										{
											bsurvivor	= TRUE;
											break;
										}
										else
										{
											pUser->AddQuestTextTime( 0, GroupQuest::ProcessState::Ready, 0xffffffff );
										}
									}
								}

								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{										
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( !rect.PtInRect( point ) )
										{
											pUser->AddQuestTextTime( 0, GroupQuest::ProcessState::Ready, 0xffffffff );
										}
									}
								}
								
							}

							if( !bsurvivor  )
							{
								Boom(pElem->nId, rect, pProp->dwWorldId);
							}
						}
						break;
					case GroupQuest::ProcessState::GetItem:
						{
							TRACE( "PQP_GETITEM - p\n" );
							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bexist	= FALSE;	// is member here?

							if( pParty )
							{
								CUser* pUser;
								
								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
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

								for( int k = 0 ; k < pParty->GetSizeofMember() ; k++ )
								{
									u_long idPlayer = pParty->GetPlayerId( k );
									pUser = (CUser*)prj.GetUserByID( idPlayer );
									if( IsValidObj( pUser ) )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( !rect.PtInRect( point ) )
										{
											pUser->AddQuestTextTime( 0, GroupQuest::ProcessState::Ready, 0xffffffff );
										}
									}
								}
							}

							if( !bexist )
							{
								Boom(pElem->nId, rect, pProp->dwWorldId);

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

void CPartyQuestProcessor::Boom(int nQuestId, CRect rect, DWORD dwWorldId) {
	CPoint		point;
	D3DXVECTOR3 vPos;
	point = rect.CenterPoint();
	vPos.x = (float)(point.x);
	vPos.z = (float)(point.y);
	vPos.y = 100.0f;
	RemoveAllDynamicObj(dwWorldId, vPos, 500);

	RemoveQuest(nQuestId);
}

void CPartyQuestProcessor::RemoveAllDynamicObj( DWORD dwWorldID, D3DXVECTOR3 vPos, int nRange  )
{
	CWorld* pWorld	= g_WorldMng.GetWorld( dwWorldID );

	if( !pWorld )
		return;

	CObj*		pObj;
	D3DXVECTOR3 vDist;

	FOR_LINKMAP( pWorld, vPos, pObj, nRange, LinkType::Dynamic, nTempLayer )
	{
		if( pObj )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				pObj->Delete();
			}
		}
	}
	END_LINKMAP

	D3DXVECTOR3 vPos2	= D3DXVECTOR3( 6968.0f, 0, 3328.8f );
	FOR_LINKMAP( pWorld, vPos, pObj, nRange, LinkType::Player, nTempLayer )
	{
		if( pObj )
		{
			CUser* pUser = (CUser*)pObj;
			pUser->AddQuestTextTime( 0, GroupQuest::ProcessState::Ready, 0xffffffff );			
			pUser->Replace( WI_WORLD_MADRIGAL, vPos2, REPLACE_NORMAL, nDefaultLayer );
		}
	}
	END_LINKMAP
}


void CPartyQuestProcessor::SetQuestParty_Log(int nQuestId, u_long idParty, OBJID objidWormon) {
	// 유저에게 공략시간을 넘겨준다...
	SendQuestLimitTime(GroupQuest::ProcessState::Wormon, MIN( 60 ), idParty );

	// 파티퀘스트시작시 정보를 파일에 기록한다.
	CString strFileName = "StartPartyQuestInfo.txt";
	CString strQuestInfo;
	CString strMemberName;
	CString strDateTime;
	
	CTime time = CTime::GetCurrentTime();
	strDateTime.Format( "DATE : %d%.2d%.2d\nTIME : %d::%d::%d\n", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond() );
	strQuestInfo.Format( "QUEST ID = %d, PARTY ID = %d, WORMON ID = %d\n", nQuestId, idParty, objidWormon );
	
	CParty* pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		for( int i = 0 ; i < pParty->GetSizeofMember() ; ++i )
		{
			CUser* pUsertmp = prj.GetUserByID( pParty->GetPlayerId( i ) );
			if( IsValidObj( pUsertmp ) )
			{
				strMemberName += pUsertmp->GetName();
				strMemberName += "\n";
			}
		}
	}

	FILEOUT( strFileName, "%s%s%s\n", strDateTime, strQuestInfo, strMemberName );
	////////////////////////////////////////////////////////////////////////////////////////////
}

void CPartyQuestProcessor::SendQuestLimitTime(GroupQuest::ProcessState nState, DWORD dwTime, u_long idParty ) {
	CParty * pParty = g_PartyMng.GetParty(idParty);
	if (!pParty) return;

	for (CUser * pUsertmp : AllMembers(*pParty)) {
		pUsertmp->AddQuestTextTime(1, nState, dwTime);
	}
}

CPartyQuestProcessor* CPartyQuestProcessor::GetInstance( void )
{
	static CPartyQuestProcessor sPartyQuestProcessor;
	return &sPartyQuestProcessor;
}


#endif	// __WORLDSERVER
