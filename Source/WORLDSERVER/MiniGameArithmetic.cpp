#include "stdafx.h"

#include ".\minigamearithmetic.h"

std::vector<int> InitPrimeNumber(int maximum) {
	std::vector<int> result;

	for (int i = 2; i < maximum; i++) {
		int nCount = 0;

		bool isPrime = true;
		for (int j = 2; j < i; j++) {
			if (i % j == 0) {
				isPrime = false;
				break;
			}
		}

		if (isPrime)
			result.push_back(i);
	}

	return result;
}

const static std::vector<int> PrimeNumbers = InitPrimeNumber(1000);

CMiniGameArithmetic::CMiniGameArithmetic()
	: m_nResult(0), m_nCorrectCount(0)
{
}

BOOL CMiniGameArithmetic::Excute( CUser* pUser, __MINIGAME_PACKET * pMiniGamePacket )
{
	BOOL bReturn = FALSE;
	
	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );	
	if( pMiniGamePacket->nState == MP_OPENWND || pMiniGamePacket->nState == MP_FAIL )
	{
		MP.nState = pMiniGamePacket->nState;
		if( pMiniGamePacket->nState == MP_OPENWND )
			m_nCorrectCount = 0;		// 창을 새로 열면 정답수 초기화..
		MP.nParam1 = m_nCorrectCount;
		MP.vecszData.push_back( MakeQuestion() );
		SendPacket( pUser, MP );
		return bReturn;
	}

	if( pMiniGamePacket->nParam1 == m_nResult )
	{
		m_nCorrectCount++;
		if( m_nCorrectCount == 5 )		// 5문제 정답을 완료!!
		{
			MP.nState = MP_FINISH;
			bReturn = TRUE;
		}
		else
		{
			MP.nState = MP_TRUE;
			MP.vecszData.push_back( MakeQuestion() );
		}
	}
	else
	{
		MP.nState = MP_FALSE;
		MP.vecszData.push_back( MakeQuestion() );
		m_nCorrectCount = 0;
	}

	MP.nParam1 = m_nCorrectCount;

	SendPacket( pUser, MP );
	return bReturn;
}


int	CMiniGameArithmetic::GetDivisor( int nDivided, std::span<const int> primes ) {
	std::vector<int> vDivisors;
	vDivisors.push_back( 1 );

	for (const int prime : primes) {
		if (nDivided > prime) break;

		while (nDivided % prime == 0) {
			nDivided /= prime;
			vDivisors.emplace_back(prime);
		}
	}

	int n	= xRandom( vDivisors.size() - 1 ) + 1;
	int nDivisor	=1;
	
	for (int i = 0; i < n; ++i) {
		const auto itDivisor = vDivisors.begin() + xRandom( vDivisors.size() );
		nDivisor *= *itDivisor;
		vDivisors.erase(itDivisor);
	}

	return nDivisor;
}

std::string CMiniGameArithmetic::MakeQuestion()
{
	int nNum[3] = {0,};
	int nOper[3] = {0,};
	char szQuestion[20] = {0,};

	for( int i=0; i<3; i++ )
	{
		nNum[i] = xRandom( 1, 1000 );
		nOper[i] = xRandom( 4 );
	}

	int nResult = 0;
	if( nOper[2] > 1 )		// 숫자가 3개인 경우다. 
	{
		if( (nOper[0] < 2) && (nOper[1] > 1) ) // 뒤쪽 연산자가 우선인 경우( *, / )
		{
			nResult = Calculate( nNum[1], nNum[2], nOper[1] );
			nResult = Calculate( nNum[0], nResult, nOper[0] );
		}
		else
		{
			nResult = Calculate( nNum[0], nNum[1], nOper[0] );
			nResult = Calculate( nResult, nNum[2], nOper[1] );
		}
		sprintf( szQuestion, "%d %c %d %c %d", nNum[0], GetOperatorCh( nOper[0] ), nNum[1],
														GetOperatorCh( nOper[1] ), nNum[2] );
	}
	else
	{
		nResult = Calculate( nNum[0], nNum[1], nOper[0] );
		sprintf( szQuestion, "%d %c %d", nNum[0], GetOperatorCh( nOper[0] ), nNum[1] );
	}

	m_nResult = nResult;
	return szQuestion;
}

int CMiniGameArithmetic::Calculate( int n1, int& n2, int nOper )
{
	switch( nOper )
	{
		case 0:	return n1 + n2;
		case 1:	return n1 - n2;
		case 2:	return n1 * n2;
		case 3:	n2 = GetDivisor( n1, PrimeNumbers );
				return n1 / n2;
	}

	return -1;
}

char CMiniGameArithmetic::GetOperatorCh( int nOper )
{
	switch( nOper )
	{
		case 0: return '+';
		case 1: return '-';
		case 2: return '*';
		case 3: return '/';
	}

	return '+';
}
