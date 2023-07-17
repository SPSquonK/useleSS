#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__TRANSFER_ERROR_TEXT
#define		__CRC
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__INFO_SOCKLIB0516			// 네트워크 에러얻기 
#define		__THROUGHPORTAL0810			// 웹에 의한 실행
#define		__TWN_LOGIN0816				// 대만 로그인 프로토콜 변경	// 클라/인증
#define		__FIX_WND_1109				// 창 생성 시 부모창 지정 오류 수정
#define		__EVENT_1101				// 출첵 이벤트
#define		__EVE_NEWYEAR				// 신년 효과

#define		__S_SERVER_UNIFY			// 서버 통합 Neuz, World
#define		__LANG_IME_0327				// 국가별 ime표시 창 출력 구분

// 해외 6차 적용


// 10
#define		__TRADESYS					// 교환 아이템 스크립트(이벤트, 퀘스트, ... ) Neuz, World
#define		__EVE_BALLOON				// 풍선 이벤트 관련
#define		__CSC_GAME_GRADE			// 게임물등급관련 채팅창 알림 및 이미지 변경.

#define		__EVENT_1101_2				// 출첵 이벤트 64비트 변경
#define		__S_RECOMMEND_EVE			// 추천 이벤트 Neuz, Trans, World
#define		__EVE_MINIGAME				// Mini Game 4종 이벤트 추가 관련.
#define		__S_ADD_EXP					// EXP_S 아이템 생성

#define		__S_ADD_RESTATE				// 상용화 아이템 생성( 리스테트 힘, 민첩, 체력, 지능 )

#define		__CSC_ENCHANT_EFFECT_2		// 기존 이펙트 + 향상된 이펙트

//11차

#define		__EVENTLUA_COUPON			// 쿠폰 이벤트
#define		__NOLIMIT_RIDE_ITEM			// 레벨 제한 없는 비행체 아이템
#define		__NPC_BUFF					// NPC를 통해 버프받기

//	#define		__CAPTURE_JPG				// JPG포맷으로 캡쳐

#define		__SYS_TICKET				// 입장권
#define		__HACK_1023					// 리소스 변조 // 날것 속도, 무기 공격 속도
#define		__JEFF_11

#define		__JEFF_9_20					// 비행 시간을 침묵 시간으로 전용
#define		__JEFF_11_4					// 아레나
#define		__JEFF_11_6					// 바보 몬스터
#define		__DST_GIFTBOX				// 착용 아이템에 기프트 박스 기능 추가 

#define		__SFX_OPT					// 이펙트(파티클) 최적화 관련
//#define		__PERF_0229

#define		__HELP_BUG_FIX				// 도움말관련 버그 수정 (도움말 한개만 띄우기 인스턴스 관리)

#define		__CHIPI_DYO					// NPC를 특정국가에서만 출력할 수 있게 character.inc에 설정 

// 12차

//	#define		__PET_0519	// 펫 각성


// 13차
//	#define		__EXT_ENCHANT				// 제련 확장(속성, 일반)
	#define		__HOUSING					// 하우징 시스템

#define		__LAYER_1020				// __LAYER_1015 테스트
#define		__AZRIA_1023				// 입장권 계층 적용
#define		__SYNC_1217					// 비동기 수정(필리핀 보고)
#define		__SYS_ITEMTRANSY			// NPC메뉴 아이템 트렌지 기능

#define		__SPEED_SYNC_0108		// ResetDestParam speed 수정 느려지게 보이는것 수정 타유저의 초기스피드비동기도 수정 예정 
#define		__PARTY_DEBUG_0129		// 극단장 튕기는 현상 디버깅 neuz

#define		__FUNNY_COIN			// 퍼니 코인

#define		__GPAUTH
#define		__GPAUTH_01
#define		__GPAUTH_02
#define		__EUROPE_0514
#define		__WINDOW_INTERFACE_BUG		// 월드 이동 상황에서 아이템 걸린 윈도우 창 띄워져 있으면 클라이언트 크래쉬되는 문제 수정

#define		__MAP_SECURITY				// 맵툴로 변조한 맵 검사

// 14차
	#define		__SMELT_SAFETY				// 14차 안전제련
	#define		__EQUIP_BIND				// 장착아이템 착용시 귀속
	#define		__EXT_ATTRIBUTE				// 속성제련 공식 확장 및 변경
	#define		__NEW_ITEM_LIMIT_LEVEL		// 아이템 착용레벨 규칙 변경(MASTER, HERO도 레벨 적용을 받고 하위 클래스 무기는 무조건 장착 가능)
	#define		__WATER_EXT					// 물 표현 확장 (물 텍스쳐의 종류 및 프레임속도 변경 추가)
	#define		__WND_EDIT_NUMBER_MODE		// 에디트 컨트롤 숫자만 입력받게 하는 모드
#define		__SHIFT_KEY_CORRECTION		// 쉬프트 키 입력 문제 수정
	#define		__STATIC_ALIGN				// 스태틱 컨트롤에 정교한 정렬 기능 추가
	#define		__ITEM_DROP_SOUND			// 아이템 드롭 시 효과음 스크립트로 처리
	#define		__JOB_TEXT					// 마스터, 히어로 관련 아이템에 '필요 직업' 텍스트 출력되도록 수정
	#define		__RESTATE_CONFIRM			// 리스테트 사용 시, 확인 창 출력
	#define		__PREVENTION_TOOLTIP_BUG	// 프리벤션 툴팁에 의미 없는 텍스트 출력되는 문제 수정
	#define		__DROP_CONFIRM_BUG			// 아이템 관련 확인 창이 열린 상태에서 일어나는 각종 오류 수정
	#define		__CLOUD_ANIMATION_BUG		// 구름 애니메이션이 특정 영역에서만 수행되는 문제 수정
	#define		__BUFF_CRASH				// 버프 렌더링 관련하여 클라이언트 크래쉬되는 문제 수정
	#define		__BS_FIX_SHADOW_ONOBJECT	// 오브젝트 그림자 리시버 조건검사 설정 ( 주인공 위주, 인던 천정 그림자 제거 )
	#define		__BS_FIX_HAIR_AMBIENT		// 캐릭터 헤어 ambient 조정 ( 인던에서 머리카락 색깔 까맣게 나오는 문제 수정 )
	#define		__BALLOON_CODE_IMPROVEMENT	// 풍선 관련 코드 개선
	#define		__WING_ITEM					// 날개 아이템
	#define		__USE_SOUND_LIB_FMOD		// use FMod sound lib
//	end 14th

// 15차 ZU TEST ZWECKEN AUSDEKLARIERUNG ENTFERNT!
	#define		__DYNAMIC_MPU				// 변경가능한 MPU !!
	#define		__BOUND_BOX_COLLISION		// 바운드 박스 충돌 루틴 개선
	#define		__BS_CHANGING_ENVIR			// 대륙별 환경 변경 ( light, fog, sky, weather, 24hours light ... ) <<< ONLY CLIENT!!!! >>>
	#define		__IMPROVE_SYSTEM_VER15		// 15차 개선 사항
	#define		__15TH_INSTANCE_DUNGEON		// 15차 인스턴스 던전
	#define		__TELEPORTER				// 15차 텔레포터
	#define		__FIND_OBJ_INSIGHT			// 시야거리 안의 동적오브젝트 출력( 미니맵 오른쪽버튼 클릭 )
	#define		__MUSIC2					// 마일즈 7.2g ver 업데이트 ( OGG 재생 가능 버젼  ) 차후 Lib업데이트시에 같이 풀어줘야함.
#define		__NEW_CONTINENT15			// 추가 대륙 ( 협곡 )
	#define		__BS_BBOX_ABS_EXTENT		// BBOX 길이 절대값으로 수정
	#define		__BS_FIXED_KNOCKBACK		// 넉백시 상태 꼬임현상 수정 
	#define		__BS_FIXED_EQUIPMOTION		// 아이템 장/탈 착중 기존모션이 유지되는현상 수정 
 // end 15th

	#define		__QUIZ						// 퀴즈 이벤트 시스템

	#define		__EVENTLUA_RAIN				// 장마 이벤트 -> 루아로 변경

	#define		__EVENTLUA_SNOW				// 강설 이벤트

	#define		__ADD_RESTATE_LOW			// 상용화 아이템 생성( 리스테트 하급 힘, 민첩, 체력, 지능 )

	#define		__YS_CHATTING_BLOCKING_SYSTEM	// 캐릭터 채팅 차단 시스템
	#define		__BAN_CHATTING_SYSTEM			// 연속 채팅 금지 시스템


	#define __ENCRYPT_PASSWORD	//	mulcom	BEGIN100218	패스워드 암호화

	#define		__DELETE_CHAR_CHANGE_KEY_VALUE	// 버디버디 i_PIN 도입으로 인한 캐릭터삭제 키값 변경(국내, 주민번호 -> 2차비번)
	#define		__BS_ADJUST_SYNC			// mover간 동기화 개선 

	#define		__SHOP_COST_RATE			// 상점 가격 조정

//	#define		__NEW_WEB_BOX				// 새로운 웹 표현 창
	#define		__PROTECT_AWAKE				//각성보호
	#define     __BS_SAFE_WORLD_DELETE		// CWorld::Process에서 safe delete obj( sfx 류로 확인댐 )
//	#define		__GAME_GRADE_SYSTEM			// 게임물 등급 표시 시스템
	#define		__BS_FIX_ARRIVEPOS_ALGO		// 마우스 이동시 도착 검사루틴 개선 ( 정뱡향 축으로 한곳만 도착했을때 멈추는 문제가있었음 )
	#define		__BS_ITEM_UNLIMITEDTIME		// 아이템 유지시간 ( 999999999 >= 일경우 무제한 표기 )
	

#define __BS_EFFECT_LUA /* Behemoth's SFX */

// 8.5차
#if	  defined(__INTERNALSERVER)	// 내부 사무실 테스트서버 

	#define		NO_GAMEGUARD
	#undef		__TRANSFER_ERROR_TEXT

	#define		__CPU_UTILDOWN_060502		// CPU 사용률 감소작업 
	#define		__SLIDE_060502				// 땅으로 꺼지는 문제 수정 
	#define		__GUILDVOTE					// 길드 투표 
	#define		__IAOBJ0622					// 상대 목표 좌표 전송	// 월드, 뉴즈
	#define		__YNOTICE_UNI1026			// 공지사항 유니코드 지원
//	#define		__YENV
//	#define		__YENV_WITHOUT_BUMP
	#define		__SKILL0517					// 스킬 레벨 파라미터
	#define		__YAIMONSTER_EX				// AI기능 추가 - Neuz, World
//	#define		__Y_INTERFACE_VER3			// 인터페이스 버전 3.0 - Neuz
	#define		__VERIFY_MEMPOOL
//	#define		__S_NEW_SKILL_2				// 스킬 개선 패킷 전송 Neuz, World, Trans
	#define		__Y_ROTXZ					// 회전 추가 Neuz, World
	#define		__Y_PATROL					// 정찰...Neuz. World
	#define		__Y_CHARACTER_TEXT			// 캐릭터 정보창 프로그램 텍스트 출력..Neuz
	#define		__Y_RENDER_NAME_OPTION		// 이름 출력 선택 UI.Neuz
	#define		__V060721_TEXTDRAG
	#define		__Y_MAPVIEW_EX
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
//	#define		__EVENT_FALL				// 단풍 이벤트 - 모델교체 Neuz, World
//	#define		__KWCSC_UPDATE				// 스킬 발동 관련 문제 수정 Neuz
//	#define		__CSC_VER8_1				// 8차 'T'키 사용, 상대방 레벨 안보이기, 화면 모든 창 On/Off Neuz
//	#define		__CSC_VER8_2				// 8차 파티창 관련 적용 Neuz
//	#define		__JEFF_VER_8				// 8차 작업
//	#define		__Y_FLAG_SKILL_BUFF			// 팻, 변신 아이템 토글버프 적용... Neuz, World
//	#define		__Y_GAMMA_CONTROL_8			// 밝기, 감마, 명암 조절 Neuz
//	#define		__Y_CHAT_SYSTEM_8			// 시스템 메세지 윈도우화... Neuz
//	#define		__Y_EYE_FLASH_8				// 눈깜박거리기... Neuz
//	#define		__Y_CAMERA_SLOW_8			// 8차 키/마우스 X축 회전시 보간으로 회전하기..Neuz
//	#define		__Y_MAX_LEVEL_8				// 만랩 120으로 조정... Neuz, World, Trans
//	#define		__CSC_VER8_3				// 8차 버프창 관련 적용, Key Down시 스킬발동 1.5초간 딜레이로 지속되게 변경 Neuz
//	#define		__CSC_VER8_4				// 8차 헤어샵관련 수정 및 성형수술 기능 추가.
//	#define		__JHMA_VER_8_1				// 8차 게임내돈드롭금지	Neuz, World
//	#define		__JHMA_VER_8_2				// 8차 게임내아이템판매가격제한풀기	Neuz, World
	#define		__FIX_WND_1109				// 자식 창 초기화 오류 수정
//	#define		__JHMA_VER_8_3				// 8차 ctrl 자동공격기능 삭제	Neuz
//	#define		__JHMA_VER_8_4				// 8차 타겟클릭자동공격 이모티콘	Neuz
//	#define		__JHMA_VER_8_5				// 8차 스킬경험치다운변경	Neuz, World


//	#define		__CSC_VER8_5				// 8차 엔젤 소환 Neuz, World, Trans

	#define		__TRAFIC_1218				// 서버에서 보낸 패킷 정보
	#define		__Y_HAIR_BUG_FIX
	#define		__FOR_PROLOGUE_UPDATE		// 태국,대만,미국,독일 Prologue추가
//	#define		__NEWYEARDAY_EVENT_COUPON	// 설 이벤트 - 헤어샾, 성형 무료이용권 관련.
	#define		__TRAFIC_1215				// 전송량 감소

//	#define     __Y_NEW_ENCHANT				// 제련 아이템 변경, Neuz, World

//	#define		__CSC_VER9_1				// 9차 전승관련 Clienet - Neuz

	#define		__HACK_0516					// 미국 해킹 2차

//	#define		__CSC_VER9_RESOLUTION		// Wide및 고해상도 추가
//	#define		__Y_ADV_ENCHANT_EFFECT		// 제련 이팩트 향상 버전
//	#define		__S_9_ADD					// 9차 추가분
//	#define		__RECOVERY10				// 9차 추가 (타격, 피격 10초 후 자동 회복)

//	#define		__CSC_VER9_2				// 9차 상태창 변경 관련 Neuz, World
//	#define		__CSC_VER9_3				// 9차 채팅상황에서 Function Key 사용 가능
//	#define		__CSC_VER9_4				// 클라이언트 구동 시 공백시간에 이미지 추가.
//	#define		__CSC_VER9_5				// 9차 제련관련 (추가 검광 기능)

//	#define		__Y_DRAGON_FIRE				// 메테오니커 파이어 발사!!!


//	#define		__METEONYKER_0608
//	#define		__CSC_UPDATE_WORLD3D		// World3D Object Culling부분 업데이트
//	#define		__CSC_VER9_REMOVE_PKSETTING	// 옵션 설정 창에서 PK설정 제거 (무조건 Ctrl키 누른 상태에서만 공격)

	#define		__SECURITY_0628				// 리소스 버전 인증 not contained

//	#define		__INSERT_MAP				// 스타트메뉴에 전체지도 메뉴 추가 

	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer
	

//	#define		__AI_0711					// 메테오니커 AI 수정

//	#define		__CSC_ENCHANT_EFFECT_2		// 기존 이펙트 + 향상된 이펙트
	
//	#define		__SYS_TICKET				// 입장권
//	#define		__HACK_1023					// 리소스 변조 // 날것 속도, 무기 공격 속도


//  #define		__LINK_PORTAL				// 외부 포탈 연결 시 무인자 처리

//	#define		__JEFF_11

	#define		__JEFF_11_1

//	#define		__JEFF_11_4					// 아레나
//	#define		__JEFF_11_5					// 메모리 누수

//	#define		__DST_GIFTBOX				// 착용 아이템에 기프트 박스 기능 추가 
	#define		__DISABLE_GAMMA_WND			// 윈도우 모드에서 감마 조절 불가하게 수정

	#define		__ATTACH_MODEL				// 모델에 다른 모델 붙이기 (날개...)
	#define		__NEW_PROFILE

	#define		__VTN_TIMELIMIT				// 베트남 플레이 시간 제한

//	#define		__FLYFF_INITPAGE_EXT

	#define		__BS_CHECKLEAK				// 메모리 누수 탐지및 라인 확인 2009/07/14
	#define		__BS_ADJUST_COLLISION		// 충돌 루틴 개선 ( 2009. 07. 28 )
//	#define		__BS_PUTNAME_QUESTARROW		// 퀘스트 클릭시 보상 NPC 화살표 표현중 이름 추가			( 09_1228 제거 )

	#define		__IMPROVE_MAP_SYSTEM		// 향상된 지도 시스템

//	#define		__BS_NO_CREATION_POST		// 임시 : 우체통 생성 무시



	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형
	#define		__BS_ADDOBJATTR_INVISIBLE	// CObj Invisible 에 관한 속성추가   --> 16차 예정 
	#define		__BS_DEATH_ACTION			// die 상태로 진입시 연출 효과 ( client only )
	#define		__BS_EFFECT_LUA				// 오브젝트 상태별 효과연출 ( Lua base )
	#define		__BS_ADD_CONTINENT_WEATHER	// 대륙 날씨 추가 ( 온난화로 인한 태양날씨, 다른 어떤이유로든 변하지 않음 )
	#define		__BS_CHANGEABLE_WORLD_SEACLOUD	// 변경가능한 월드 바다구름 

	
#elif defined(__TESTSERVER)  // 외부 유저 테스트서버 

	#define		NO_GAMEGUARD

	#define		__CPU_UTILDOWN_060502		// CPU 사용률 감소작업 
	#define		__SLIDE_060502				// 땅으로 꺼지는 문제 수정 
	#define     __GUILDVOTE					// 길드 투표
//	#define		__SKILL0517					// 스킬 레벨 파라미터
	#define		__Y_CHARACTER_TEXT			// 캐릭터 정보창 프로그램 텍스트 출력..Neuz
	#define		__V060721_TEXTDRAG			// 글자에 효과주기
	#define		__Y_ROTXZ					// 회전 추가 Neuz, World
	#define		__Y_PATROL					// 정찰...Neuz. World
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
	#define		__TRAFIC_1215				// 전송량 감소

//	#define		__CSC_VER9_1				// 9차 전승관련 Clienet - Neuz

//	#define		__CSC_VER9_RESOLUTION		// Wide및 고해상도 추가
//	#define		__Y_ADV_ENCHANT_EFFECT		// 제련 이팩트 향상 버전
//	#define		__S_9_ADD					// 9차 추가분
//	#define		__RECOVERY10				// 9차 추가 (타격, 피격 10초 후 자동 회복)

//	#define		__CSC_VER9_2				// 9차 상태창 변경 관련 Neuz, World
//	#define		__CSC_VER9_3				// 9차 채팅상황에서 Function Key 사용 가능
//	#define		__CSC_VER9_4				// 클라이언트 구동 시 공백시간에 이미지 추가.
//	#define		__CSC_VER9_5				// 9차 제련관련 (추가 검광 기능)

//	#define		__Y_DRAGON_FIRE				// 메테오니커 파이어 발사!!!
	#define		__HACK_0516					// 미국 해킹 2차

//	#define		__EVENTLUA					// 이벤트 (루아 스크립트 적용) - World, Trans, Neuz
//	#define		__CSC_VER9_REMOVE_PKSETTING	// 옵션 설정 창에서 PK설정 제거 (무조건 Ctrl키 누른 상태에서만 공격)

//	#define		__INSERT_MAP				// 스타트메뉴에 전체지도 메뉴 추가 

//	#define		__METEONYKER_0608
//	#define		__AI_0711					// 메테오니커 AI 수정
	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

//	#undef		__VER
//	#define		__VER	11					// 11차

	#define		__JEFF_11_1
//	#define		__JEFF_11_4					// 아레나
//	#define		__JEFF_11_5					// 메모리 누수



//   VER 16 __TESTSERVER begin
	#define		__GUILD_HOUSE_MIDDLE		// 길드하우스 중형
	#define		__BS_ADDOBJATTR_INVISIBLE	// CObj Invisible 에 관한 속성추가   --> 16차 예정 
	#define		__BS_DEATH_ACTION			// die 상태로 진입시 연출 효과 ( client only )
//	#define		__BS_EFFECT_LUA				// 오브젝트 상태별 효과연출 ( Lua base )
	#define		__BS_ADD_CONTINENT_WEATHER	// 대륙 날씨 추가 ( 온난화로 인한 태양날씨, 다른 어떤이유로든 변하지 않음 )
	#define		__BS_CHANGEABLE_WORLD_SEACLOUD	// 변경가능한 월드 바다구름 
//   VER 16 __TESTSERVER end

	
#elif defined(__MAINSERVER)  // 외부 본섭
//	#define		__TMP_POCKET				// 휴대 가방 일시적으로 막기
	#define		__Y_BEAUTY_SHOP_CHARGE		// 뷰티샵, 개인상점 유료아이템화..Neuz, World
	#define		__TRAFIC_1215
//	#define     __Y_NEW_ENCHANT				// 제련 아이템 변경, Neuz, World
//	#define		__EVENT_FALL				// 단풍

	#define		__JEFF_11_1

#endif	// end - 서버종류별 define 

#ifndef NO_GAMEGUARD 
	#define	__NPROTECT_VER	4	
#endif	

#endif // VERSION_COMMON_H
