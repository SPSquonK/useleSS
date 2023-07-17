#ifndef __VERSION_COMMON__
#define __VERSION_COMMON__
#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER					// 클라이언트 전용코드를 빌드하지 않기 위한 define
#define		__ITEM_REMOVE_LIST			// 아이템 제거 

#define		__EVENT_1101				// 출첵 이벤트

// 해외 6차 적용
//	#define		__VERIFY_0201

//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
//#define		__TRANS_0413				// 트랜스 서버 메로리 최적화
//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.

#define		__SKILL_0205				// 스킬 변경 부분만 저장

#define		__EVENT_1101_2				// 출첵 이벤트 64비트 변경
#define		__GETMAILREALTIME			// 실시간 메일 가지고 오기 Trans
#define		__S_RECOMMEND_EVE			// 추천 이벤트 Neuz, Trans, World
#define		__EXP_ANGELEXP_LOG			// 경험치, 엔젤 경험치 로그 관련 CHARACTER_TBL 컬럼 추가

//11th

#define		__EVENTLUA_COUPON			// 쿠폰 이벤트
#define		__LOG_PLAYERCOUNT_CHANNEL	// 채널별 동접 로그

#define		__CONV_0906
#define		__INVALID_LOGIN_0320		// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
#define		__INVALID_LOGIN_0612		// 보완

#define		__JEFF_9_20		// 비행 시간을 침묵 시간으로 전용
#define		__JEFF_FIX_0

#define	__REMOVE_PLAYER_0221		// 캐릭터 삭제 패킷 조작

#define		__SEND_ITEM_ULTIMATE	// 얼터멋 웨폰도 ITEM_SEND_TBL에서 지급가능하게...

#define		__AUTO_NOTICE		// 자동 공지

// 12차

// 13차
//	#define		__HOUSING			// 하우징 시스템


#define		__OCCUPATION_SHOPITEM	// 점령길드 전용 구매 가능 아이템

#define		__LAYER_1015	// 레이어 저장	

#define		__FUNNY_COIN			// 퍼니 코인

#define		__REMOVEITEM_POCKET		// 삭제 예약 테이블에 휴대가방 포함..

#define		__PERIN_BUY_BUG				// 페냐 반복구매 버그 확인용 코드

// 14차

#define		__QUIZ					// 퀴즈 이벤트 시스템

#define		__ERROR_LOG_TO_DB		// 에러 로그 시스템


// 15차

#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 

	#define		__SKILL_0205
	#define		__GUILDVOTE				// 길드 투표 
	#define		__VERIFY_MEMPOOL

//	#define		__S_NEW_SKILL_2			// 스킬 개선 패킷 전송 Neuz, World, Trans
	#define		__Y_MAX_LEVEL_8			// 만랩 120으로 조정... Neuz, World, Trans
//	#define		__INVALID_LOGIN_0320	// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
//	#define		__INVALID_LOGIN_0612	// 보완




//	#define		__CONV_0906




	#define		__JEFF_11_2		// 수신자 없는 메일 로딩 시 반송

	#define		__VTN_TIMELIMIT

	#define		 __GUILD_HOUSE_MIDDLE	// 길드하우스 중형



#elif defined(__TESTSERVER)		// 외부 유저 테스트서버 

//	#define		__NOTIFYSTART0706		// 런쳐에게 시작 알림 방식을 이벤트로  
	#define		__GUILDVOTE				// 길드 투표 
	#define		__Y_MAX_LEVEL_8			// 만랩 120으로 조정... Neuz, World, Trans

//	#define		__CONV_0906
//	#define		__INVALID_LOGIN_0320	// 저장이 완료 되지 않은 사용자의 접속 막기(복사 방지)
//	#define		__INVALID_LOGIN_0612	// 보완

	#define	__JEFF_11_2		// 수신자 없는 메일 로딩 시 반송

	#define __GUILD_HOUSE_MIDDLE	// 길드하우스 중형


#elif defined(__MAINSERVER)		// 외부 본섭 

	#define		__IDC
//	#define		__TOOMANY_PENDINGLOG1115

#endif	// end - 서버종류별 define 

#endif