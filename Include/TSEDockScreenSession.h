//	TSEDockScreenSession.h
//
//	Classes and functions for dock screen types.
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SDockFrame
	{
	CSpaceObject *pLocation = NULL;			//	Current location
	CDesignType *pRoot = NULL;				//	Either a screen or a type with screens
	CString sScreen;						//	Screen name (UNID or name)
	CString sPane;							//	Current pane
	ICCItemPtr pInitialData;				//	Data for the screen
	ICCItemPtr pStoredData;					//	Read-write data
	ICCItemPtr pReturnData;					//	Data returns from a previous screen

	CDesignType *pResolvedRoot = NULL;
	CString sResolvedScreen;

	TSortMap<CString, CString> DisplayData;	//	Opaque data used by displays
	};

class CDockScreenStack
	{
	public:
		void DeleteAll (void);
		ICCItem *GetData (const CString &sAttrib);
		const CString &GetDisplayData (const CString &sID);
		inline int GetCount (void) const { return m_Stack.GetCount(); }
		const SDockFrame &GetCurrent (void) const;
		ICCItem *GetReturnData (const CString &sAttrib);
		void IncData (const CString &sAttrib, ICCItem *pData, ICCItem **retpResult = NULL);
		inline bool IsEmpty (void) const { return (m_Stack.GetCount() == 0); }
		void Push (const SDockFrame &Frame);
		void Pop (void);
		void ResolveCurrent (const SDockFrame &ResolvedFrame);
		void SetCurrent (const SDockFrame &NewFrame, SDockFrame *retPrevFrame = NULL);
		void SetCurrentPane (const CString &sPane);
		void SetData (const CString &sAttrib, ICCItem *pData);
		void SetDisplayData (const CString &sID, const CString &sData);
		void SetLocation (CSpaceObject *pLocation);
		void SetReturnData (const CString &sAttrib, ICCItem *pData);

	private:
		TArray<SDockFrame> m_Stack;

		static const SDockFrame m_NullFrame;
	};

class CDockSession
	{
	public:
		static constexpr DWORD FLAG_FORCE_UNDOCK =	0x00000001;

		//	FLAG_FORCE_UNDOCK
		bool ExitScreen (DWORD dwFlags = 0);
		bool FindScreenRoot (const CString &sScreen, CDesignType **retpRoot, CString *retsScreen, ICCItemPtr *retpData) const;
		CDockScreenStack &GetFrameStack (void) { return m_DockFrames; }
		const CDockScreenStack &GetFrameStack (void) const { return m_DockFrames; }
		CSpaceObject *OnPlayerDocked (CSpaceObject *pObj);
		inline void SetDefaultScreenRoot (CDesignType *pType) { m_pDefaultScreensRoot = pType; }

	private:
		CDesignType *m_pDefaultScreensRoot = NULL;	//	Default root to look for local screens
		CDockScreenStack m_DockFrames;				//	Stack of dock screens
		TArray<CXMLElement *> m_ScreensInited;		//	List of screens that have called OnInit this session
	};
