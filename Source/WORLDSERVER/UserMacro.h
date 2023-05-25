#ifndef __USERMACRO_H__
#define __USERMACRO_H__

#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );

#define	USERPTR							( _it->second )

#define FOR_VISIBILITYRANGE( pCtrl )	\
										auto _it  = pCtrl->m_2pc.begin(); \
										auto _end = pCtrl->m_2pc.end(); \
										while( _it != _end )	\
										{

#define	NEXT_VISIBILITYRANGE( pCtrl )	\
											++_it;	\
										}

#endif // USERMACRO_H