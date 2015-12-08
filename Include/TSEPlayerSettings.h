//	TSEPlayerSettings.h
//
//	Defines classes and interfaces for player ship display, etc.
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SArmorSegmentImageDesc
	{
	CString sName;								//	Name of segment

	int xDest;									//	Coordinates of destination
	int yDest;
	int xHP;									//	Coordinates of HP display
	int yHP;
	int yName;									//	Coordinates of armor name
	int cxNameBreak;							//	Line break
	int xNameDestOffset;
	int yNameDestOffset;

	CObjectImageArray Image;					//	Image for armor damage
	};

struct SArmorImageDesc
	{
	enum EStyles
		{
		styleNone,								//	Unknown

		styleImage,								//	Armor represented by images
		styleRingSegments,						//	Show as ring segments
		};

	EStyles iStyle;								//	Style of painting armor
	CObjectImageArray ShipImage;				//	Image for ship (with no armor)

	TSortMap<int, SArmorSegmentImageDesc> Segments;
	};

struct SReactorImageDesc
	{
	CObjectImageArray ReactorImage;

	CObjectImageArray PowerLevelImage;
	int xPowerLevelImage;
	int yPowerLevelImage;

	CObjectImageArray FuelLevelImage;
	int xFuelLevelImage;
	int yFuelLevelImage;

	CObjectImageArray FuelLowLevelImage;

	RECT rcReactorText;
	RECT rcPowerLevelText;
	RECT rcFuelLevelText;
	};

struct SShieldImageDesc
	{
	CEffectCreatorRef pShieldEffect;			//	Effect for display shields HUD

	CObjectImageArray Image;					//	Image for shields
	};

struct SWeaponImageDesc
	{
	CObjectImageArray Image;					//	Background image
	};

class CPlayerSettings
	{
	public:
		CPlayerSettings (void) : 
				m_pArmorDesc(NULL),
				m_pShieldDesc(NULL),
				m_pWeaponDesc(NULL),
				m_fOwnArmorDesc(false),
				m_fOwnShieldDesc(false),
				m_fOwnWeaponDesc(false),
				m_pReactorDescInherited(NULL),
				m_pWeaponDescInherited(NULL)
			{ }

		~CPlayerSettings (void) { CleanUp(); }

		CPlayerSettings &operator= (const CPlayerSettings &Source);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		ALERROR Bind (SDesignLoadCtx &Ctx, CShipClass *pClass);
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		inline CXMLElement *GetArmorDesc (void) const { return m_pArmorDesc; }
		inline const CString &GetDesc (void) const { return m_sDesc; }
		inline const CDockScreenTypeRef &GetDockServicesScreen (void) const { return m_pDockServicesScreen; }
		inline const CCurrencyAndValue &GetHullValue (void) const { return m_HullValue; }
		inline DWORD GetLargeImage (void) const { return m_dwLargeImage; }
		inline const SReactorImageDesc &GetReactorDesc (void) const { return (m_fHasReactorDesc ? m_ReactorDesc : *m_pReactorDescInherited); }
		inline CXMLElement *GetShieldDesc (void) const { return m_pShieldDesc; }
		inline const CDockScreenTypeRef &GetShipConfigScreen (void) const { return m_pShipConfigScreen; }
		inline const CDockScreenTypeRef &GetShipScreen (void) const { return m_pShipScreen; }
		inline int GetSortOrder (void) const { return m_iSortOrder; }
		inline const CCurrencyAndRange &GetStartingCredits (void) const { return m_StartingCredits; }
		inline DWORD GetStartingMap (void) const { return m_dwStartMap; }
		inline const CString &GetStartingNode (void) const { return m_sStartNode; }
		inline const CString &GetStartingPos (void) const { return m_sStartPos; }
		inline CXMLElement *GetWeaponDesc (void) const { return m_pWeaponDesc; }
//		inline const SWeaponImageDesc *GetWeaponDesc (void) const { return (m_fHasWeaponDesc ? &m_WeaponDesc : m_pWeaponDescInherited); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		inline bool IsDebugOnly (void) const { return (m_fDebug ? true : false); }
		inline bool IsIncludedInAllAdventures (void) const { return (m_fIncludeInAllAdventures ? true : false); }
		inline bool IsInitialClass (void) const { return (m_fInitialClass ? true : false); }
		void MergeFrom (const CPlayerSettings &Src);

		CXMLElement *GetArmorImageDescRaw (void) const { return (m_fOwnArmorDesc ? m_pArmorDesc : NULL); }
		CXMLElement *GetShieldImageDescRaw (void) const { return (m_fOwnShieldDesc ? m_pShieldDesc : NULL); }
		CXMLElement *GetWeaponDescRaw (void) const { return (m_fOwnWeaponDesc ? m_pWeaponDesc : NULL); }
		const SReactorImageDesc *GetReactorImageDescRaw (void) const { return (m_fHasReactorDesc ? &m_ReactorDesc : NULL); }


	private:
		void CleanUp (void);
		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError);

		CString m_sDesc;							//	Description
		DWORD m_dwLargeImage;						//	UNID of large image
		int m_iSortOrder;							//	For setting order of appearence

		//	Miscellaneous
		CCurrencyAndRange m_StartingCredits;		//	Starting credits
		DWORD m_dwStartMap;							//	Starting map UNID
		CString m_sStartNode;						//	Starting node (may be blank)
		CString m_sStartPos;						//	Label of starting position (may be blank)
		CDockScreenTypeRef m_pShipScreen;			//	Ship screen
		CDockScreenTypeRef m_pDockServicesScreen;	//	Screen used for ship upgrades (may be NULL)
		CDockScreenTypeRef m_pShipConfigScreen;		//	Screen used to show ship configuration (may be NULL)
		CCurrencyAndValue m_HullValue;				//	Value of hull alone (excluding any devices/armor)

		//	HUD descriptors
		CXMLElement *m_pArmorDesc;
		CXMLElement *m_pShieldDesc;
		CXMLElement *m_pWeaponDesc;

		//	Reactor
		SReactorImageDesc m_ReactorDesc;
		const SReactorImageDesc *m_pReactorDescInherited;

		//	Weapons
		SWeaponImageDesc m_WeaponDesc;
		const SWeaponImageDesc *m_pWeaponDescInherited;

		//	Flags
		DWORD m_fInitialClass:1;					//	Use ship class at game start
		DWORD m_fDebug:1;							//	Debug only
		DWORD m_fHasReactorDesc:1;					//	TRUE if m_ReactorDesc initialized
		DWORD m_fIncludeInAllAdventures:1;			//	TRUE if we should always include this ship
		DWORD m_fOwnArmorDesc:1;					//	TRUE if we own m_pArmorDesc
		DWORD m_fOwnShieldDesc:1;					//	TRUE if we own m_pShieldDesc
		DWORD m_fOwnWeaponDesc:1;					//	TRUE if we own m_pWeaponDesc
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};
