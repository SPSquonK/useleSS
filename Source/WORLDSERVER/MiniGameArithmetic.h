#pragma once

#include "minigamebase.h"

class CMiniGameArithmetic :	public CMiniGameBase
{
public:
	CMiniGameArithmetic(void);
	CMiniGameArithmetic( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameArithmetic(void);

	BOOL Excute(CUser * pUser, __MINIGAME_PACKET * pMiniGamePacket) override;

private:
	void	InitPrimeNumber();
	int		GetDivisor( int nDivided );	// 나눗셈일때 결과가 정수로 떨어지는 피연산자를 구한다.
	std::string	MakeQuestion();
	int		Calculate( int n1, int& n2, int nOper );
	char	GetOperatorCh( int nOper );

	int m_nResult;
	int m_nCorrectCount;
	std::vector<int>	m_vecnPrimeNumber;		// operator가 나눗셈 인 경우 피연산자를 조합할 소수 목록
};

