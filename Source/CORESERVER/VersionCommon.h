#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER				// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__MAP_SIZE						
#define		__S8_SERVER_PORT		// 2006월 11월 7일 업데이트 - World, Cache, CoreServer
#define		__EVE_NEWYEAR			// 신년 효과

//
//#define		__HACK_0516 // 미국 해킹 2차
#define		__JEFF_9_20		// 비행 시간을 침묵 시간으로 전용

//
#define		__MA_VER11_01			// 극단레벨업포인트변경
#define		__AUTO_NOTICE		// 자동 공지

// 13차
#define		__LAYER_1015		// 동적 객체 층

// 14차
#define		__QUIZ						// 퀴즈 이벤트 시스템

#define		__EVENTLUA_RAIN				// 장마 이벤트 -> 루아로 변경
#define		__EVENTLUA_SNOW				// 강설 이벤트


#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 
	#define		__GUILDVOTE			// 길드 투표 
	#define		__HACK_0516			// 미국 해킹 2차


#elif defined(__TESTSERVER)		// 외부 유저 테스트서버 

//	#define		__NOTIFYSTART0706	// 런쳐에게 시작 알림 방식을 이벤트로  
	#define		__GUILDVOTE			// 길드 투표 
	#define		__HACK_0516			// 미국 해킹 2차

#endif	// end - 서버종류별 define 


#endif


