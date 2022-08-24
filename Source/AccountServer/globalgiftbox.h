#ifndef __GLOBAL_GIFT_BOX_H__
#define	__GLOBAL_GIFT_BOX_H__

#include "ar.h"
#include "buffer.h"

class CDPMng;

class CGlobalGiftbox
{
private:
	std::map<DWORD, int>	m_map;		
/* first: MAKELOG( dwGiftBox, dwItem )
	second: nNum;
*/
	BOOL	m_bLoad;
public:
	CGlobalGiftbox();
	virtual	~CGlobalGiftbox();

	static	CGlobalGiftbox*	GetInstance( void );

	void	OnUpload( CAr & ar );	// recv & load script
	void	OnQuery( CDPMng* pdp, CAr & ar, DPID dpid );	// recv & check & save
	void	OnRestore( CAr & ar );
private:
	BOOL	Save( void );
};

#endif	// __GLOBAL_GIFT_BOX_H__