#ifndef __OBJMAP_H__
#define __OBJMAP_H__

#pragma once

#ifdef __CORESERVER
#include "..\..\Objects\Obj.h"
#include "MyMap.h"
#include "Data.h"
#else	// __CORESERVER
#include "Ctrl.h"
#include "MyMap.h"
#include "Data.h"
#endif	// __CORESERVER

#include <assert.h>

#ifdef __CORESERVER
class CIdStack
{
private:
	OBJID	m_id;
	CMclCritSec		m_AccessLock;

public:
	CIdStack()	{	m_id	= 0;	}
	virtual	~CIdStack()	{}
	OBJID	GetIdBlock( u_long uSize )	{	CMclAutoLock Lock( m_AccessLock );	m_id	+= uSize;	return ( m_id - uSize );	}
	OBJID	GetId( void )	{	CMclAutoLock Lock( m_AccessLock );	return m_id++;	}
};
#endif


#ifdef __WORLDSERVER
#include <stack>

class CIdStack final {
private:
	CMclCritSec		m_AccessLock;
	std::stack<OBJID> m_stack;

public:

	void	PushIdBlock(OBJID idBase, u_long uSize) {
		CMclAutoLock Lock(m_AccessLock);
		for (DWORD i = 0; i < uSize; i++)
			m_stack.emplace(idBase + i);
	}
	OBJID GetId(void) {
		CMclAutoLock Lock(m_AccessLock);
		if (m_stack.empty())
			WriteLog("%s, %d", __FILE__, __LINE__);
		assert(!m_stack.empty());
		const OBJID elem = m_stack.top();
		m_stack.pop();
		return elem;
	}

	int GetCount(void) {
		return m_stack.size();
	}
};
#endif

class CObjMap final {
public:
#ifndef __CLIENT
	CIdStack					m_idStack;
#endif
	std::map<DWORD, CCtrl *> m_map;

public:
	bool Add(CCtrl * pCtrl);
	bool RemoveKey(DWORD dwKey);
	CCtrl * Find(DWORD dwKey) const;
};

inline CCtrl * CObjMap::Find(const DWORD dwKey) const {
	const auto it = m_map.find(dwKey);
	return it != m_map.end() ? it->second : nullptr;
}

inline bool CObjMap::RemoveKey(const DWORD dwKey) {
	return m_map.erase(dwKey) > 0;
}

inline bool CObjMap::Add(CCtrl * const pCtrl) {
	OBJID id = pCtrl->GetId();
	if (id == NULL_ID) {
#ifdef __CLIENT
		assert(0);
		return false;
#else
		id = m_idStack.GetId();
		pCtrl->SetId(id);
#endif
	}

	if (m_map.emplace(id, pCtrl).second)
		return true;

	assert(0);
	return false;
}


#endif // __OBJMAP_H__