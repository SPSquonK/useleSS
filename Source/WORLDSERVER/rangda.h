#pragma once

// Controls a single Rangda
class CRangda final {
public:
	// A world - position pair
	struct RANGDA_POS final {
		DWORD	dwWorldId;
		D3DXVECTOR3 vPos;
	};

	struct Prop {
		explicit Prop(DWORD dwMonster) : m_dwMonster(dwMonster) {}

		DWORD	m_dwMonster;                // Monster id
		int		m_nInterval = 0;	          // Time before spawning when dead
		int		m_nReplace = 0;		          // Time before moving if alive and undamage
		BOOL	m_bActiveAttack = FALSE;	  // Is red monster?
		std::vector<RANGDA_POS>	m_vvPos; // Random positions where it can appear

		// Randomly return one of the positions and update oldPos. The new position
		// is different that the one stored in oldPos.
		[[nodiscard]] RANGDA_POS GetRandomPos(std::optional<size_t> & oldPos) const;
	};

	explicit CRangda(Prop prop);

	void	OnTimer();

private:
	void SetObj(OBJID objid);
	void CreateMonster();
	[[nodiscard]] CMover * GetMonster();
	void ProcessReplace(CMover * pMonster);
	void ProcessGenerate();

	static bool IsDamaged(CMover * pMonster);
	static bool DecrementCounter(int & tickCounter); // Decrement tickCounter towards 0 and return true if reached 0

private:
	/* const */ Prop prop;

	struct State {
		int  m_nGenerateCountdown = 0;	// Time before spawning, decreased by 1 at each tick
		int  m_nReplaceCountdown = 0;   // Time before re-placing, decreased by 1 at each tick
		bool m_bReplaceable = true;     // If false, prevent re-placing
		std::optional<size_t> m_nOldPos = std::nullopt;
		OBJID	m_objid = NULL_ID;
	} state;
};

class CRangdaController final {
public:
	static CRangdaController * Instance();

	bool LoadScript(const char * szFile);
	void OnTimer();
	
private:
	CRangdaController() = default;
private:
	std::vector<CRangda> m_vRangda;
};
