#include "stdafx.h"
#include "Snapshot.h"

CSnapshot::CSnapshot()
	: cb(this) {
	dpidCache	= dpidUser	= DPID_UNKNOWN;
}

CSnapshot::CSnapshot( DPID idCache, DPID idUser, OBJID objid, DWORD dwHdr )
	: cb(this) {
	SetSnapshot( objid, dwHdr );	
}

void CSnapshot::SetSnapshot( OBJID objid, DWORD dwHdr )
{
	ASSERT( DPID_UNKNOWN != dpidUser );
	cb_	= 0;
	ar.Flush();
	ar << dpidUser << dwHdr << objid << cb_;
}