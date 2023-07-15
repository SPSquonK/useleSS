#ifndef __VERSION_COMMON_H__
#define	__VERSION_COMMON_H__
#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER				// 클라이언트 전용코드를 빌드하지 않기 위한 define

#define		__CRC
#define		__SO1014				// 소켓 예외 처리( 캐쉬, 인증, 로그인 )
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__VERIFYNETLIB
#define		__DOS1101
#define		__TWN_LOGIN0816

#define	__GPAUTH
#define	__GPAUTH_01
#define	__GPAUTH_02		// 독일/프랑스 통합 인증 - 구매
//#define	__GPAUTH_03		// 독일/프랑스 통합 인증 - 권한
#define	__EUROPE_0514

// 14th
//	#define 	__PCBANG		// PC방 혜택

#define __ENCRYPT_PASSWORD	//	mulcom	BEGIN100218	패스워드 암호화


#if	  defined(__INTERNALSERVER)		// 내부 사무실 테스트서버 
	#define	__SECURITY_0628
#elif defined(__TESTSERVER)			// 외부 유저 테스트서버


	
#elif defined(__MAINSERVER)			// 외부 본섭

#endif	// end - 서버종류별 define 

#endif	// __VERSION_COMMON_H__