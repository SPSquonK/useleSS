#pragma once

#include <array>
#include <memory>
#include "WndTListBox.hpp"

class CWndDialog final : public CWndNeuz 
{ 
public:
	static constexpr UINT WIDC_NewQuests = 901;
	static constexpr UINT WIDC_CurrentQuests = 902;

	struct WORDBUTTON {
		BOOL bStatus;
		CRect rect;
		TCHAR szWord[64];
		TCHAR szKey[64];
		DWORD dwParam;
		DWORD dwParam2;
		int nLinkIndex; // Index to concatenate if word is broken by a newline
	};

	class CWndAnswer : public CWndButton {
	public:
		const WORDBUTTON * m_pWordButton;
	};

private:
	std::array<std::unique_ptr<CWndAnswer>, 6> m_apWndAnswer;

public: 
	CTimer m_timer;
	CTexture m_texChar;
	BOOL m_bWordButtonEnable = FALSE;
	int m_nWordButtonNum = 0;
	int m_nKeyButtonNum  = 0;
	int m_nContextButtonNum = 0;
	int m_nSelectKeyButton = - 1;
	CUIntArray m_aContextMark[ 32 ];

	WORDBUTTON m_aWordButton[ 32 ];
	WORDBUTTON m_aKeyButton[ 32 ];
	WORDBUTTON m_aContextButton[ 32 ];
	CEditString m_string;

	DWORD m_dwQuest = 0;// context 버튼에서 사용함 
	BOOL m_bSay;
	size_t m_nCurArray;
	std::vector<std::unique_ptr<CEditString>> m_strArray;
	OBJID m_idMover;
	std::map<CString, CString> m_mapWordToOriginal;

	struct ListedQuest {
		CEditString displayName;
		CString strKey;
		QuestId questId;
	};

private:
	struct NewQuestDisplayer {
		CTexture * m_pExpectedQuestListIconTexture = nullptr;
		CTexture * m_pNewQuestListIconTexture = nullptr;
		int xOffset = 0;

		void Render(
			C2DRender * p2DRender, CRect rect,
			ListedQuest & quest, DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	struct CurrentQuestDisplayer {
		CTexture * m_pCompleteQuestListIconTexture = nullptr;
		CTexture * m_pCurrentQuestListIconTexture = nullptr;
		int xOffset = 0;

		void Render(
			C2DRender * p2DRender, CRect rect,
			ListedQuest & quest, DWORD color, const WndTListBox::DisplayArgs & misc
		) const;
	};

	CWndTListBox<ListedQuest, NewQuestDisplayer> m_newQuestListBox;
	CWndTListBox<ListedQuest, CurrentQuestDisplayer> m_currentQuestListBox;

public:
	 
	CWndDialog() = default;
	~CWndDialog(); 

	void RemoveAllKeyButton();
	void RemoveKeyButton( LPCTSTR lpszKey );
	void AddAnswerButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest );
	void AddKeyButton( LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam, DWORD dwQuest );
	void ParsingString( LPCTSTR lpszString );
	void Say( LPCTSTR lpszString, DWORD dwQuest );
	void EndSay();
	void BeginText();
	void MakeKeyButton();
	void MakeContextButton();
	void MakeAnswerButton();
	void UpdateButtonEnable();
	BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	void RunScript( const char* szKey, DWORD dwParam, DWORD dwQuest );
	void AddQuestInList(const LPCTSTR lpszWord, const LPCTSTR lpszKey, QuestId dwQuest, bool isNewQuest);
	void MakeQuestKeyButton( const CString& rstrKeyButton );

private:
	std::pair<ListedQuest, bool> MakeListedQuest(const LPCTSTR lpszWord, const LPCTSTR lpszKey, QuestId dwQuest);
	[[nodiscard]] const char * GetOriginalOfWord(const CString & strWord) const;

public:

	virtual BOOL Process();
	void OnSetCursor() override;
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseWndSurface( CPoint point);
}; 

