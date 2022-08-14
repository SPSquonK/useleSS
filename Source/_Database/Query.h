#pragma once

#include <sql.h>
#include <sqlext.h>
#include <array>
#include <memory>
#include <optional>
#include "sqktd.h"

class CQuery {
public:
	static SQLINTEGER sqlNts;

	struct Credentials {
		char Name[256] = "";
		char Id[256]   = "";
		char Pass[256] = "";
	};

	class ColString {
	private:
		SQLULEN m_size;
		std::unique_ptr<char[]> m_buffer;

	public:
		ColString();

		[[nodiscard]] int AsInt() const;
		[[nodiscard]] std::int64_t AsInt64() const;
		[[nodiscard]] float AsFloat() const;
		[[nodiscard]] char AsChar() const;
		[[nodiscard]] const char * GetRawString() const;
		std::pair<char *, SQLLEN> GetInfoForBind();
	};

public:
	Credentials DBCredentials;

	// 최대 컬럼수, BLOB 입출력 단위, NULL 필드값
	static constexpr SQLSMALLINT MAXCOL = 256;
	
	enum { BLOBBATCH=10000, CQUERYNULL=-100, CQUERYEOF=-101, 
		CQUERYNOCOL=-102, CQUERYERROR=-103 };
private:
	SQLHENV hEnv = nullptr;							// 환경 핸들
	SQLHDBC hDbc = nullptr;							// 연결 핸들
	std::array<ColString, MAXCOL> Col; // 바인딩될 컬럼 정보

	[[nodiscard]] int FindCol(const char *name) const;				// 컬럼의 이름으로부터 번호를 찾아준다.

	std::optional<CString> m_storeBindedParameters = std::nullopt;

public:
	SQLHSTMT hStmt = nullptr;							// 명령 핸들. 직접 사용할 수도 있으므로 public으로 정의
	SQLSMALLINT nCol;						// 컬럼 개수
	SQLCHAR ColName[MAXCOL][50];			// 컬럼의 이름들
	SQLINTEGER lCol[MAXCOL];				// 컬럼의 길이/상태 정보

	CQuery() = default;
	CQuery(const CQuery &) = delete;
	CQuery & operator=(const CQuery &) = delete;
	~CQuery();								// 파괴자:연결 핸들을 해제한다.

	void PrintDiag( LPCTSTR szSQL, SQLSMALLINT type = SQL_HANDLE_DBC );						// 진단 정보 출력
	void PrintQuery(const char * query);
	BOOL Connect(int Type, const char *ConStr, const char *UID=NULL, const char *PWD=NULL);	// 데이터 소스에 연결한다.
	BOOL Connect(int Type, const Credentials & credentials) {
		return Connect(Type, credentials.Name, credentials.Id, credentials.Pass);
	}
	void DisConnect();						// 데이터 소스 연결을 끊는다
	BOOL Exec(LPCTSTR szSQL);				// SQL문을 실행한다.
	BOOL PrepareFetch();
	void StartLogBindedParameters() { m_storeBindedParameters = CString(); }

	BOOL	Execute( LPCTSTR lpszFormat, ... );

	int ExecGetInt(LPCTSTR szSQL);			// SQL문을 실행하고 첫번째 컬럼의 정수 읽음
	void ExecGetStr(LPCTSTR szSQL, char *buf);		// SQL문을 실행하고 첫번째 컬럼의 문자열 읽음
	BOOL Fetch();						// 한 행 가져오기
	void Clear();							// 커서 해제 및 언 바인딩
	int GetInt(int nCol);					// 정수형 컬럼 읽기
	int GetInt(const char *sCol);					// 정수형 컬럼 읽기

	__int64		GetInt64( int nCol );
	__int64		GetInt64( const char* sCol );

	EXPINTEGER	GetExpInteger( int nCol )	{ return GetInt64( nCol );	}
	EXPINTEGER	GetExpInteger(	const char* sCol ) { return GetInt64( sCol ); 	}
	SERIALNUMBER	GetSerialNumber( int nCol )
		{
			return GetInt( nCol );
		}
	SERIALNUMBER	GetSerialNumber( const char* sCol )
		{
			return GetInt( sCol );
		}

	float GetFloat(int nCol);					// 실수형 컬럼 읽기
	float GetFloat(const char *sCol);				// 실수형 컬럼 읽기
	char GetChar(int nCol);					// 실수형 컬럼 읽기
	char GetChar(const char *sCol);				// 실수형 컬럼 읽기
	void GetStr(int nCol, char *buf);		// 문자열형 컬럼 읽기
	void GetStr(const char *sCol, char *buf);		// 문자열형 컬럼 읽기
	const char * GetStrPtr(int nCol) const;
	const char * GetStrPtr(const char * sCol) const;

	template<SQLSMALLINT ParameterType = SQL_VARCHAR>
	requires ((ParameterType == SQL_VARCHAR || ParameterType == SQL_CHAR))
	bool BindParameter(SQLUSMALLINT parameterNumber,
		char * parameterValuePtr,
		SQLUINTEGER columnSize = 0
	) {
		return BindParameterImpl(parameterNumber, SQL_PARAM_INPUT, SQL_C_CHAR, ParameterType, columnSize, 0,
			parameterValuePtr, 0, &sqlNts);
	}

	template<typename IntegerType>
		requires (sqktd::IsOneOf<std::remove_volatile_t<IntegerType>, int, unsigned int, long, unsigned long>)
	bool BindParameter(SQLUSMALLINT parameterNumber, IntegerType * valuePtr) {
		return BindParameterImpl(parameterNumber, SQL_PARAM_INPUT,
			SQL_C_LONG, SQL_INTEGER, 0, 0, valuePtr, 0, 0
		);
	}

	template<typename FloatType>
		requires (sqktd::IsOneOf<std::remove_volatile_t<FloatType>, float>)
	bool BindParameter(SQLUSMALLINT parameterNumber, FloatType * valuePtr) {
		return BindParameterImpl(parameterNumber, SQL_PARAM_INPUT,
			SQL_C_FLOAT, SQL_REAL, 0, 0, valuePtr, 0, 0
		);
	}

	template<typename IntegerType>
		requires (sqktd::IsOneOf<std::remove_volatile_t<IntegerType>, long long>)
	bool BindParameter(SQLUSMALLINT parameterNumber, IntegerType * valuePtr) {
		return BindParameterImpl(parameterNumber, SQL_PARAM_INPUT,
			SQL_C_SBIGINT, SQL_BIGINT, 0, 0, valuePtr, 0, 0
		);
	}

	BOOL MoreResults( void );
	

	void WriteLogFile(const char *strLog,...);
	static BOOL EnableConnectionPooling();

	private:
		
	BOOL BindParameterImpl(SQLUSMALLINT parameterNumber,
                           SQLSMALLINT inputOutputType,
                           SQLSMALLINT valueType,
                           SQLSMALLINT parameterType,
                           SQLUINTEGER columnSize,
                           SQLSMALLINT decimalDigits,
                           SQLPOINTER  parameterValuePtr,
                           SQLINTEGER bufferLength,
                           SQLINTEGER *strLen_or_IndPtr);
};
// TODO: 모듈 분리 
// 암호화된 토큰을 해독해서 패스워드로 얻는다.
// 예: 토큰 - 3A08DB22
extern BOOL GetPWDFromToken( const char* szToken, char* szPWD );
