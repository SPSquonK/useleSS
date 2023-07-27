#ifndef __LORD_SKILL_EXECUTABLE_H__
#define	__LORD_SKILL_EXECUTABLE_H__

#include "lordskill.h"

class CLordSkillComponentItemBuf :
	public CLordSkillComponentExecutable
{
public:
	CLordSkillComponentItemBuf(int nId, CScript & script) : CLordSkillComponentExecutable(nId, script)	{}
	virtual	~CLordSkillComponentItemBuf()	{}
	virtual	void	Execute( u_long idPlayer, u_long idTarget, VOID* pParam );
};

class CLordSkillComponentSummon:
	public CLordSkillComponentExecutable
{
public:
	CLordSkillComponentSummon(int nId, CScript & script) : CLordSkillComponentExecutable(nId, script)	{}
	virtual	~CLordSkillComponentSummon()	{}
	virtual	void	Execute( u_long idPlayer, u_long idTarget, VOID* pParam );
	virtual	int		IsExecutable( CUser* pUser, const char* szTarget, u_long &idTarget );
};

class CLordSkillComponentTeleport:
	public CLordSkillComponentExecutable
{
public:
	CLordSkillComponentTeleport(int nId, CScript & script) : CLordSkillComponentExecutable(nId, script)		{}
	virtual	~CLordSkillComponentTeleport()	{}
	virtual	void	Execute( u_long idPlayer, u_long idTarget, VOID* pParam );
	virtual	int		IsExecutable( CUser* pUser, const char* szTarget, u_long &idTarget );
};

namespace	lordskill
{
	int		UseRequirements( CUser* pUser, const char* szTarget, int nSkill, u_long & idTarget );
};

#endif	// __LORD_SKILL_EXECUTABLE_H__