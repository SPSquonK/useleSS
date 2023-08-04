#include "stdafx.h"
#include <stdio.h>
#include "model.h"
#include "Bone.h"




////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CBones :: CBones()
{
	m_nID = 0;
	memset( m_szName, 0, sizeof(m_szName) );
	m_nRHandIdx = m_nLHandIdx = m_nRArmIdx = m_nLArmIdx = 0;

	D3DXMatrixIdentity( &m_mLocalRH );
	D3DXMatrixIdentity( &m_mLocalLH );
	D3DXMatrixIdentity( &m_mLocalShield );
	D3DXMatrixIdentity( &m_mLocalKnuckle );

	memset( m_vEvent, 0, sizeof(m_vEvent) );
	memset( m_nEventParentIdx, 0, sizeof(m_nEventParentIdx) );
}

//
//
//
int		CBones :: LoadBone( LPCTSTR szFileName )
{
	CResFile resFp;
	const BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );	
	if( bRet == FALSE )		
		return	FAIL;

	int nVer;
	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer < VER_BONE )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_BONE );
		resFp.Close();
		return FAIL;
	}

	// ID읽기를 넣음.
	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		Error( "%s : file name is too long", szFileName );
		resFp.Close();
		return FAIL;
	}

	// 파일명 카피
	strcpy( m_szName, szFileName );

	int		nNumBone;
	resFp.Read( &nNumBone, 4, 1 );			// 본 개수 읽음
	const int m_nMaxBone = nNumBone;
	m_pBones = sqktd::dynamic_array<BONE>(nNumBone);			// 본 개수 만큼 할당
	memset( m_pBones.get(), 0, sizeof(BONE) * nNumBone);		// zero clear

	for( int i = 0; i < nNumBone; i ++ )
	{
		int nLen;
		resFp.Read( &nLen, 4, 1 );		// 널 포함길이
		resFp.Read( m_pBones[i].m_szName,		nLen, 1 );		// bone node 이름 저장
		resFp.Read( &m_pBones[i].m_mTM,			sizeof(D3DXMATRIX), 1 );			// World(Object) TM
		resFp.Read( &m_pBones[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		resFp.Read( &m_pBones[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		resFp.Read( &m_pBones[i].m_nParentIdx,	4, 1 );								// parent bone index
		if( strcmpi( m_pBones[i].m_szName + 6, "r hand") == 0 )	// 부모이름이 R Hand면...
			m_nRHandIdx = i;
		if( strcmpi( m_pBones[i].m_szName + 6, "l hand") == 0 )	// 부모이름이 L Hand면...
			m_nLHandIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "l forearm") == 0 )	// 부모이름이 L ForeArm면...
			m_nLArmIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "r forearm") == 0 )	// 부모이름이 R ForeArm면...
			m_nRArmIdx = i;
	}

	resFp.Read( &m_bSendVS, 4, 1 );
	
	// 오른손 무기쥐는 위치의 행렬
	resFp.Read( &m_mLocalRH, sizeof(D3DXMATRIX), 1 );
	resFp.Read( &m_mLocalShield, sizeof(D3DXMATRIX), 1 );
	resFp.Read( &m_mLocalKnuckle, sizeof(D3DXMATRIX), 1 );
	
	// m_vEvent가 추가된 버전.
//	int		nParentIdx[4];
	if( nVer == 5 )
	{
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * 4, 1 );
		resFp.Read( m_nEventParentIdx, sizeof(int) * 4, 1 );
	} else
	if( nVer >= 6 )
	{
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * MAX_MDL_EVENT, 1 );
		resFp.Read( m_nEventParentIdx, sizeof(int) * MAX_MDL_EVENT, 1 );
	}

	if( nVer == 7 )
	{
		resFp.Read( &m_mLocalLH, sizeof(D3DXMATRIX), 1 );
	}

	resFp.Close();

	// 부모 포인터를 셋팅
	for( int i = 0; i < nNumBone; i ++ )
	{
		if( m_pBones[i].m_nParentIdx == -1 )			// 부모가 없으면 부모포인터는 널
			m_pBones[i].m_pParent = NULL;
		else
			m_pBones[i].m_pParent = &m_pBones[ m_pBones[i].m_nParentIdx ];
	}

	return SUCCESS;
}







////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////

CBonesMng		g_BonesMng;

// When this file load request comes in, the file
// is read and loaded into memory.
// When loading, the duplicate returns a pointer to
// the already loaded bone.
const CBones * CBonesMng::LoadBone(LPCTSTR szFileName) {
	char sFile[MAX_PATH] = { 0, };
	strcpy(sFile, szFileName);
	strlwr(sFile);

	if (const auto i = m_mapBones.find(sFile); i != m_mapBones.end()) {
		return i->second.get();
	}

	CBones * pBones = new CBones;
	if (pBones->LoadBone(szFileName) == FAIL) {
		safe_delete(pBones);
		return nullptr;
	}

	m_mapBones.emplace(sFile, pBones);
	return pBones;
}

////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CMotionMng		g_MotionMng;

CMotion * CMotionMng::LoadMotion(LPCTSTR szFileName) {
	char sFile[MAX_PATH] = { 0, };
	std::strcpy(sFile, szFileName);
	strlwr(sFile);

	if (const auto i = m_mapMotions.find(sFile); i != m_mapMotions.end()) {
		return i->second.get();
	}

	CMotion * pMotion = new CMotion;
	if (pMotion->LoadMotion(szFileName) == FAIL) {
		safe_delete(pMotion);
		return nullptr;
	}

	m_mapMotions.emplace(sFile, pMotion);
	return pMotion;
}


////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////

CMotion :: CMotion()
{
	m_pBoneFrame = NULL;
	m_fPerSlerp = 0.5f;
	m_nMaxEvent = 0;
	m_pBoneInfo = NULL;
	memset(m_vEvent, 0, sizeof(m_vEvent));
	m_pAttr = NULL;
	m_pBoneInfo = NULL;
}

CMotion :: ~CMotion()
{
	SAFE_DELETE_ARRAY(m_pAttr);
	if (m_pBoneFrame) {
		for (int i = 0; i < m_nMaxBone; i++)
			m_pBoneFrame[i].m_pFrame = NULL;
	}
	SAFE_DELETE_ARRAY(m_pBoneFrame);
	SAFE_DELETE_ARRAY(m_pBoneInfo);
}

//
//
//
int		CMotion :: LoadMotion( LPCTSTR szFileName )
{
	int		nNumBone;
	int		nNumFrame;
//	int		nNumSize;
//	int		i, j;
	int		nVer;

	CResFile resFp;
	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
	if( bRet == FALSE )	
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", szFileName );
		//ADDERRORMSG( szStr );
		resFp.Close();
		return FAIL;
	}

	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer != VER_MOTION )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MOTION );
		resFp.Close();
		return FAIL;
	}

	// ID읽기를 넣음.
	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		Error( "%s : file name is too long", szFileName );
		resFp.Close();
		return FAIL;
	}

	// 파일명 카피
	strcpy( m_szName, szFileName );
	
	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );		// 
	resFp.Seek( 32, SEEK_CUR );		// reserved

	resFp.Read( &nNumBone, 4, 1 );			// 뼈대 갯수 읽음
	resFp.Read( &nNumFrame, 4, 1 );		// 애니메이션 프레임 개수 읽음
	m_nMaxFrame = nNumFrame;
	m_nMaxBone = nNumBone;

	if( nNumFrame <= 0 )
	{
		Error( "CMotion::LoadMotion : %s read MaxFrame = %d", szFileName, nNumFrame );
		resFp.Close();
		return FAIL;
	}

	// Path: skip it as it is never used
	int nTemp;
	resFp.Read( &nTemp, 4, 1 );
	if (nTemp) {
		resFp.Seek(sizeof(D3DXVECTOR3) * nNumFrame, SEEK_CUR);
	}

	//
	// TM_Animation 데이타 읽음.
	ReadTM( &resFp, nNumBone, nNumFrame );
	
	// 프레임 속성 읽음.
	resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * nNumFrame, 1 );

	resFp.Read( &m_nMaxEvent, 4, 1 );	// 이벤트 좌표 저장
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );
	

	resFp.Close();

	return SUCCESS;
}

//
//  Motion의 core부분을 읽음. 외부에서 단독 호출 가능.
//
void	CMotion :: ReadTM( CResFile *file, int nNumBone, int nNumFrame )
{

	m_nMaxBone = nNumBone;		// LoadMotion()에서 불려졌다면 이부분은 필요없으나 ReadTM만 따로 불릴 일이 있으면 이게 필요하다.
	m_nMaxFrame = nNumFrame;

	//--- 뼈대구성정보 읽음.
	m_pBoneInfo = new BONE[ nNumBone ];			// 본 개수 만큼 할당
	memset( m_pBoneInfo, 0, sizeof(BONE) * nNumBone );		// zero clear
	
	
	for(int i = 0; i < nNumBone; i ++ )
	{
		int nLen; file->Read( &nLen, 4, 1 );
		if( nLen > 32 )		
			Error("CMotion::ReadTM - %s bonename is too long", m_szName );

		file->Read( m_pBoneInfo[i].m_szName,	nLen, 1 );		// bone node 이름
		file->Read( &m_pBoneInfo[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		file->Read( &m_pBoneInfo[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		file->Read( &m_pBoneInfo[i].m_nParentIdx,	4, 1 );								// parent bone index
	}
	
	// 부모 포인터를 셋팅
	for( int i = 0; i < nNumBone; i ++ )
	{
		if( m_pBoneInfo[i].m_nParentIdx == -1 )			// 부모가 없으면 부모포인터는 널
			m_pBoneInfo[i].m_pParent = NULL;
		else
			m_pBoneInfo[i].m_pParent = &m_pBoneInfo[ m_pBoneInfo[i].m_nParentIdx ];
	}
	
	int		nNumSize;
	file->Read( &nNumSize, 4, 1 );			// 프레임 사이즈 읽음 - 메모리 풀 사이즈
	//--- 모션 읽음.
	m_pMotion		= std::make_unique<TM_ANIMATION[]>(nNumSize);		// 메모리 풀
	m_pBoneFrame	= new BONE_FRAME[ nNumBone ];
	m_pAttr			= new MOTION_ATTR[ nNumFrame ];
	memset( m_pAttr, 0, sizeof(MOTION_ATTR) * nNumFrame );	// nNumSize였는데 nNumFrame이 맞는거 같다.
	
	TM_ANIMATION	*p = m_pMotion.get();

	// 뼈대 수 만큼 루프
	for( int i = 0; i < nNumBone; i ++ )
	{
		int nFrame; file->Read( &nFrame, 4, 1 );
		if( nFrame == 1 )		// 1이면 현재 뼈대에 프레임 있음
		{
			m_pBoneFrame[i].m_pFrame = p;
			file->Read( m_pBoneFrame[i].m_pFrame, sizeof(TM_ANIMATION) * nNumFrame, 1 );		// 한방에 읽어버리기.
			p += nNumFrame;
		} else {
			// 현재 뼈대에 프레임 없음
			file->Read( &(m_pBoneFrame[i].m_mLocalTM), sizeof(D3DXMATRIX), 1 );			// 프레임이 없으면 LocalTM만 읽고
			m_pBoneFrame[i].m_pFrame = NULL;
			// m_mLocalTM에 넣었으므로 메모리 풀에는 넣을필요 없다.
		}
	}
	
	if (m_pMotion.get() + nNumSize != p) {
		Error("%s : frame size error", m_szName);
	}
}

//
// 뼈대의 애니메이션
// 
void	CMotion :: AnimateBone( D3DXMATRIX *pmUpdateBone, const CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight ) const
{

	D3DXQUATERNION		qSlerp;
	D3DXVECTOR3			vSlerp;

	D3DXMATRIX	m1, m2;

	if( pMotionOld == NULL )	// pMotionOld-> 할때 널참조 에러만 안나게함.
		pMotionOld = this;

	// 보간을 위한 Slerp 계산.
	const int nCurrFrame = (int)fFrameCurrent;				// 소숫점 떼내고 정수부만..
	const float fSlp = fFrameCurrent - (float)nCurrFrame;	// 소숫점 부분만 가져옴
	/////////////////////////////

	// 애니메이션 동작이 있는 경우
	int i = m_nMaxBone;

	const BONE_FRAME * pBoneFrame	  = m_pBoneFrame;
	const BONE_FRAME * pBoneFrameOld = pMotionOld->m_pBoneFrame;		// 본의 계층구조 // 이전동작의 뼈대 계층
	const BONE * pBone = m_pBoneInfo;		// 이 모델의 뼈대 시작 포인터.
	// pmUpdateBone

	D3DXMATRIX * pMatBones = pmUpdateBone;			// 업데이트 매트릭스 리스트 시작 포인터

	// 뼈대 갯수만큼 루프 돈다
	while( i-- )
	{
		const TM_ANIMATION * pFrameOld = NULL;				// 이전모션의 프레임
		if( pBoneFrameOld->m_pFrame )
			pFrameOld = &pBoneFrameOld->m_pFrame[nFrameOld];		// 일단은 0으로 한다.  이전모션의 nCurrFrame도 기억하고 있어야 한다.
		else
			pFrameOld = NULL;		// 일단은 0으로 한다.  이전모션의 nCurrFrame도 기억하고 있어야 한다.
		if( pBoneFrame->m_pFrame )		// 이 뼈대에 프레임이 있을때
		{
			const TM_ANIMATION * pFrame = &pBoneFrame->m_pFrame[nCurrFrame];		// 현재 프레임 // 현재 프레임 포인터 받음
			const TM_ANIMATION * pNext  = &pBoneFrame->m_pFrame[nNextFrame];		// 다음 프레임 // 다음 프레임 포인터 받음

			D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp );	// 보간된 쿼터니언 구함
			D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp );		// 보간된 Pos벡터 구함

			if( bMotionTrans )	// 이전 모션과 블렌딩 해야 하는가
			{
				float	fBlendSlp = fBlendWeight;
				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qSlerp, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vSlerp, fBlendSlp );		// 보간된 Pos벡터 구함
				} else
				{
					D3DXQUATERNION	qLocal;
					D3DXVECTOR3		vLocal;
					const D3DXMATRIX		*pmLocalTM = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
					vLocal.x = pmLocalTM->_41;
					vLocal.y = pmLocalTM->_42;
					vLocal.z = pmLocalTM->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal, &qSlerp, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &vLocal, &vSlerp, fBlendSlp );		// 보간된 Pos벡터 구함
				}
			}

			// matAniTM계산
			// 이동행렬 만듦
			D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
		
			// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
			D3DXMatrixRotationQuaternion( &m2, &qSlerp );
			m2 *= m1;		// 이동행렬 X 회전행렬 = 애니행렬
			
			// WorldTM = LocalTM * ParentTM;
			// 최종 변환된 월드매트릭스를 멤버에 넣어서 Render()에서 SetTransfrom함.
		} else
		// 이 뼈대에 프레임이 없을때.
		{
			if( bMotionTrans )	// 이전 모션과 블렌딩 해야 하는가
			{
				float	fBlendSlp = fBlendWeight;
				D3DXQUATERNION	qLocal;
				D3DXVECTOR3		vLocal;
				const D3DXMATRIX		*pmLocalTM = &pBoneFrame->m_mLocalTM;
				D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
				vLocal.x = pmLocalTM->_41;
				vLocal.y = pmLocalTM->_42;
				vLocal.z = pmLocalTM->_43;	// 현재프레임의 LocalTM을 쿼터니온과 벡터로 빼냄.

				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qLocal, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vLocal, fBlendSlp );		// 보간된 Pos벡터 구함
				} else
				{
					D3DXQUATERNION	qLocal2;
					D3DXVECTOR3		vLocal2;
					const D3DXMATRIX		*pmLocalTM2 = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal2, pmLocalTM2 );
					vLocal2.x = pmLocalTM2->_41;
					vLocal2.y = pmLocalTM2->_42;
					vLocal2.z = pmLocalTM2->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal2, &qLocal, fBlendSlp );	// 이전모션과 현재 프레임을 블렌딩
					D3DXVec3Lerp( &vSlerp, &vLocal2, &vLocal, fBlendSlp );		// 보간된 Pos벡터 구함
				}
				D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
			
				// 쿼터니온 보간된 회전키값을 회전행렬로 만듦
				D3DXMatrixRotationQuaternion( &m2, &qSlerp );
				m2 *= m1;			// 이동행렬 X 회전행렬 = 애니행렬
			} else
				m2 = pBoneFrame->m_mLocalTM;

		}
		
		if( pBone->m_pParent )
			m2 *= pMatBones[ pBone->m_nParentIdx ];		// 부모가 없다면 루트인데 항등행렬과 곱할 필요 없어서 생략했다.



		*pmUpdateBone = m2;		// 이 뼈대의 최종 월드 변환 매트릭스

		pBoneFrame ++;
		pBoneFrameOld ++;
		pBone ++;					// 본 ++
		pmUpdateBone ++;			// 본 트랜스폼 매트릭스 ++
	}
}

