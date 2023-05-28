#pragma once

// GetXXFromStr -> read a value
// "1/2/3" GetIntFromStr x3 -> 1 2 3
// GetXXPaFromStr -> If a / is encountered, it is not passed
// "1/2/3" GetIntPaFromStr x3 -> 1 0 0 (location is blocked on the first /)

// ',', '/', '\0' 을 만날때까지 값을 리턴하고 커서는 다음...
inline void GetStrFromStr(const char *pBuf, char *strReturn, int *pLocation)
{
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!='/' && pBuf[*pLocation]!=0) {
		strReturn[count]=pBuf[*pLocation];
		count++; (*pLocation)++;
	}
	strReturn[count]='\0';(*pLocation)++;
}

inline int GetIntFromStr(const char * pBuf, int * pLocation) {
	char strTemp[50];
	GetStrFromStr(pBuf, strTemp, pLocation);
	return atoi(strTemp);
}


inline void GetStrPaFromStr(const char * pBuf, char * strReturn, int * pLocation) {
	int count = 0;
	while (pBuf[*pLocation] != ',' && pBuf[*pLocation] != 0) {
		if (pBuf[*pLocation] != '/') {
			strReturn[count] = pBuf[*pLocation];
			count++; (*pLocation)++;
		} else {
			if (count == 0) {
				strReturn[0] = '\0';
			} else {
				strReturn[count] = pBuf[*pLocation];
			}
			return;
		}
	}
	strReturn[count] = 0; (*pLocation)++;
}

inline	__int64 GetInt64PaFromStr(char * pBuf, int * pLocation) {
	char strTemp[50];
	GetStrPaFromStr(pBuf, strTemp, pLocation);
	return _atoi64(strTemp);
}

// ',', '\0' 을 만날때까지 값을 리턴하고 커서는 다음...
// '/' 를 만나면 값을 리턴하고 커서는 현재('/')...
inline int GetIntPaFromStr(const char *pBuf, int *pLocation )
{
	char strTemp[50];
	GetStrPaFromStr(pBuf, strTemp, pLocation);
	return atoi(strTemp);
}

inline SERIALNUMBER	GetSerialNumberPaFromStr(char *pBuf, int *pLocation )
{
	char strTemp[50];
	GetStrPaFromStr(pBuf, strTemp, pLocation);
	return atoi(strTemp);
}

