#ifndef __CREATEMONSTER_H__
#define	__CREATEMONSTER_H__


struct CREATE_MONSTER_PROP
{
	DWORD dwKeepTime;
	std::map<DWORD, int> mapMonster;
	
	CREATE_MONSTER_PROP() :	dwKeepTime(0) {}
		
	DWORD GetRandomMonsterId()
	{
		int nRandom = xRandom( 100 );
		auto it=mapMonster.begin();
		for( ; it!=mapMonster.end(); it++ )
		{
			if( nRandom < it->second )
				break;
			nRandom -= it->second;
		}

		return it!=mapMonster.end() ? it->first : NULL_ID;
	}
};

struct CREATE_MONSTER_INFO
{
	DWORD dwOwnerId = NULL_ID;
	DWORD dwEndTick = 0;
	char  chState = 'N';
};

typedef std::map<DWORD, CREATE_MONSTER_PROP*> MAPPROP;
typedef std::map<DWORD, CREATE_MONSTER_INFO> MAPINFO;

class CCreateMonster
{
public:
	//	Constructions
	CCreateMonster();
	~CCreateMonster();

	static CCreateMonster* GetInstance( void );
	void LoadScript();
	void SetState( OBJID objId, const char chState );
	void ProcessRemoveMonster();
	BOOL IsAttackAble( CUser* pUser, CMover* pTarget, BOOL bTextOut = FALSE );
	void CreateMonster( CUser* pUser, DWORD dwObjId, D3DXVECTOR3 vPos );
	void RemoveInfo( CMover* pMover );
	

private:
	CREATE_MONSTER_PROP* GetCreateMonsterProp( CItemElem* pItemElem );

	int m_nMaxCreateNum;
	MAPPROP m_mapCreateMonsterProp;
	MAPINFO m_mapCreateMonsterInfo;

};



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#endif	//	__CREATEMONSTER_H__
