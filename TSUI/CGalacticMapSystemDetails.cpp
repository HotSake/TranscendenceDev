//	CGalacticMapSystemDetails.h
//
//	CGalacticMapSystemDetails class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ID_STATION_LIST                         CONSTLIT("idStationList")

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FADE_EDGE_HEIGHT					CONSTLIT("fadeEdgeHeight")
#define PROP_FILL_TYPE							CONSTLIT("fillType")
#define PROP_FONT								CONSTLIT("font")
#define PROP_LINE_COLOR							CONSTLIT("lineColor")
#define PROP_LINE_PADDING						CONSTLIT("linePadding")
#define PROP_LINE_TYPE							CONSTLIT("lineType")
#define PROP_LINE_WIDTH							CONSTLIT("lineWidth")
#define PROP_LL_RADIUS							CONSTLIT("llRadius")
#define PROP_LR_RADIUS							CONSTLIT("lrRadius")
#define PROP_OPACITY							CONSTLIT("opacity")
#define PROP_PADDING_BOTTOM						CONSTLIT("paddingBottom")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")
#define PROP_UL_RADIUS							CONSTLIT("ulRadius")
#define PROP_UR_RADIUS							CONSTLIT("urRadius")

const int MAJOR_PADDING_TOP =					20;
const int INTER_LINE_SPACING =					20;
const int MAX_ICON_SIZE =                       100;
const int MIN_ICON_SIZE =                       32;
const Metric ICON_SCALE =                       0.25;
const int ICON_SPACING_HORZ =                   16;

const int LIST_PADDING_X =                      20;
const int HEADER_PADDING_X =                    20;
const int HEADER_PADDING_Y =                    20;

CGalacticMapSystemDetails::CGalacticMapSystemDetails (const CVisualPalette &VI, CReanimator &Reanimator, const RECT &rcPane) :
        m_VI(VI),
        m_Reanimator(Reanimator),
        m_rcPane(rcPane)

//  CGalacticMapSystemDetails constructor

    {
    }

bool CGalacticMapSystemDetails::CreateDetailsPane (CTopologyNode *pNode, IAnimatron **retpAni)

//  CreateDetailsPane
//
//  Creates an animation showing the details for a given system. Returns FALSE 
//  if we fail to create the pane.

    {
    int i;

    //  Get the list of objects at this node

    TSortMap<CString, SObjDesc> Objs;
    GetObjList(pNode, Objs);

    //  Compute the height of the header

    int cyHeader = m_VI.GetFont(fontHeader).GetHeight() + m_VI.GetFont(fontMedium).GetHeight() + 2 * HEADER_PADDING_Y;

    //  Create a sequencer which will be the root pane.

    CAniSequencer *pRoot = new CAniSequencer;
	pRoot->SetPropertyVector(PROP_POSITION, CVector(m_rcPane.left, m_rcPane.top));

    //  Create a background for the whole pane

    m_VI.CreateFrame(pRoot, NULL_STR, 0, 0, RectWidth(m_rcPane), RectHeight(m_rcPane), CVisualPalette::OPTION_FRAME_TRANS);
    m_VI.CreateFrameHeader(pRoot, NULL_STR, 0, 0, RectWidth(m_rcPane), cyHeader, 0);

    //  Add system information

    CreateSystemHeader(pRoot, pNode);

    //  Create a listbox which will hold all stations in the system.

    RECT rcList;
    rcList.top = cyHeader;
    rcList.bottom = rcList.top + RectHeight(m_rcPane) - cyHeader;
    rcList.left = LIST_PADDING_X;
    rcList.right = RectWidth(m_rcPane) - LIST_PADDING_X;

    CAniListBox *pList;
    m_VI.CreateListBox(pRoot, ID_STATION_LIST, rcList.left, rcList.top, RectWidth(rcList), RectHeight(rcList), 0, &pList);

    //  Add all the stations in the node

    int y = MAJOR_PADDING_TOP;
    for (i = 0; i < Objs.GetCount(); i++)
        {
		//	Generate a record for the object

		IAnimatron *pEntry;
		int cyHeight;
		CreateObjEntry(Objs[i], y, RectWidth(rcList), &pEntry, &cyHeight);

		pList->AddEntry(strFromInt(Objs[i].ObjData.dwObjID), pEntry);
		y += cyHeight + INTER_LINE_SPACING;
        }

    //  Done

    *retpAni = pRoot;

    return true;
    }

void CGalacticMapSystemDetails::CreateObjEntry (const SObjDesc &Obj, int yPos, int cxWidth, IAnimatron **retpAni, int *retcyHeight)

//  CreateObjEntry
//
//  Creates an entry for the station.

    {
	const CG16bitFont &HeaderFont = m_VI.GetFont(fontHeader);
	const CG16bitFont &TextFont = m_VI.GetFont(fontMedium);

	//	Start with a sequencer

	CAniSequencer *pRoot = new CAniSequencer;
	pRoot->SetPropertyVector(PROP_POSITION, CVector(0, yPos));
    int x = 0;
    int y = 0;

    //  Create the icon for the object

    CG32bitImage *pIcon;
    if (CreateObjIcon(Obj.ObjData, &pIcon))
        {
		int xOffset = (MAX_ICON_SIZE - pIcon->GetWidth()) / 2;
		IAnimatron *pIconAni = new CAniRect;
		pIconAni->SetPropertyVector(PROP_POSITION, CVector(x + xOffset, 0));
		pIconAni->SetPropertyVector(PROP_SCALE, CVector(pIcon->GetWidth(), pIcon->GetHeight()));
		pIconAni->SetFillMethod(new CAniImageFill(pIcon, true));

		pRoot->AddTrack(pIconAni, 0);
        }

    //  Add the object name

    CString sHeading = ::ComposeNounPhrase(Obj.ObjData.sName, Obj.iCount, NULL_STR, Obj.ObjData.dwNameFlags, nounCountOnly | nounTitleCapitalize);
	int xText = x + MAX_ICON_SIZE + ICON_SPACING_HORZ;
    int cxText = cxWidth - (MAX_ICON_SIZE + ICON_SPACING_HORZ);

	IAnimatron *pName = new CAniText;
	pName->SetPropertyVector(PROP_POSITION, CVector(xText, y));
	pName->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
	pName->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextHighlight));
	pName->SetPropertyFont(PROP_FONT, &HeaderFont);
	pName->SetPropertyString(PROP_TEXT, sHeading);

	pRoot->AddTrack(pName, 0);
	y += HeaderFont.GetHeight();

    //  Add description

    CString sDesc = CONSTLIT("This is a cool station that will do some stuff and maybe it will be good or maybe it will not be. Either way, you should visit it. Maybe.");

	IAnimatron *pDesc = new CAniText;
	pDesc->SetPropertyVector(PROP_POSITION, CVector(xText, y));
	pDesc->SetPropertyVector(PROP_SCALE, CVector(cxText, 1000));
	pDesc->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextNormal));
	pDesc->SetPropertyFont(PROP_FONT, &TextFont);
	pDesc->SetPropertyString(PROP_TEXT, sDesc);

	RECT rcLine;
	pDesc->GetSpacingRect(&rcLine);

	pRoot->AddTrack(pDesc, 0);
	y += RectHeight(rcLine);

    //  Done

    *retpAni = pRoot;

	if (retcyHeight)
		*retcyHeight = Max((pIcon ? pIcon->GetHeight() : 0), y);
    }

bool CGalacticMapSystemDetails::CreateObjIcon (const CObjectTracker::SObjEntry &Obj, CG32bitImage **retpIcon)

//  CreateObjIcon
//
//  Creates an icon for the object. Returns FALSE if we could not create the 
//  icon.

    {
	CCompositeImageModifiers Modifiers;
    if (Obj.fShowDestroyed)
        Modifiers.SetStationDamage(true);

    int iRotation;
    const CObjectImageArray &FullImage = Obj.pType->GetTypeImage().GetImage(Obj.ImageSel, Modifiers, &iRotation);
    if (!FullImage.IsLoaded())
        return false;

	CG32bitImage *pBmpImage = &FullImage.GetImage(strFromInt(Obj.pType->GetUNID()));
    if (pBmpImage == NULL)
        return false;

	RECT rcBmpImage = FullImage.GetImageRect(0, iRotation);
    int iSize = Max(RectWidth(rcBmpImage), RectHeight(rcBmpImage));
    if (iSize <= 0)
        return false;

    //  Compute scale

    int iScaleSize = Max(MIN_ICON_SIZE, Min((int)(ICON_SCALE * iSize), MAX_ICON_SIZE));
    Metric rScale = (Metric)iScaleSize / (Metric)iSize;

    //  Create a small version

    CG32bitImage *pIcon = new CG32bitImage;
	if (!pIcon->CreateFromImageTransformed(*pBmpImage,
			rcBmpImage.left,
			rcBmpImage.top,
			RectWidth(rcBmpImage),
			RectHeight(rcBmpImage),
			rScale,
			rScale,
			0.0))
        {
        delete pIcon;
        return false;
        }

    //  Done

    *retpIcon = pIcon;
    return true;
    }

void CGalacticMapSystemDetails::CreateSystemHeader (CAniSequencer *pContainer, CTopologyNode *pTopology) const

//  CreateSystemHeader
//
//  Creates info about the system.

    {
    const CG16bitFont &TitleFont = m_VI.GetFont(fontHeader);
    const CG16bitFont &DescFont = m_VI.GetFont(fontMedium);

    int x = HEADER_PADDING_X;
    int y = HEADER_PADDING_Y;
    int cxWidth = RectWidth(m_rcPane) - (2 * HEADER_PADDING_X);

    //  System name

    int cyText;
    m_VI.CreateTextArea(pContainer, 
            NULL_STR,
            x,
            y,
            cxWidth,
            1000,
            pTopology->GetSystemName(),
            m_VI.GetColor(colorTextHighlight),
            TitleFont,
            NULL,
            &cyText);
    y += cyText;

    //  Figure out when we last visited the system.

    IPlayerController *pPlayer = g_pUniverse->GetPlayer();
    CPlayerGameStats *pStats = (pPlayer ? pPlayer->GetGameStats() : NULL);
    DWORD dwLastVisit = (pStats ? pStats->GetSystemLastVisitedTime(pTopology->GetID()) : 0xffffffff);

    //  Compose a string indicating when we visited.

    CString sVisit;
    if (dwLastVisit == 0xffffffff)
        sVisit = CONSTLIT("You've never visited this system.");
    else if (dwLastVisit == (DWORD)g_pUniverse->GetTicks())
        sVisit = CONSTLIT("You are currently in this system.");
    else
        {
		CTimeSpan Span = g_pUniverse->GetElapsedGameTimeAt(g_pUniverse->GetTicks()) - g_pUniverse->GetElapsedGameTimeAt(dwLastVisit);
        sVisit = strPatternSubst(CONSTLIT("Last visited %s ago."), Span.Format(NULL_STR));
        }

    m_VI.CreateTextArea(pContainer, 
            NULL_STR,
            x,
            y,
            cxWidth,
            1000,
            sVisit,
            m_VI.GetColor(colorTextNormal),
            DescFont,
            NULL,
            &cyText);
    y += cyText;
    }

bool CGalacticMapSystemDetails::GetObjList (CTopologyNode *pNode, TSortMap<CString, SObjDesc> &Results) const

//  GetObjList
//
//  Returns an ordered list of objects to show in the details pane for this 
//  node. We return FALSE if there are no objects in the list.

    {
    int i;

    //  Initialize

    Results.DeleteAll();

    //  Get the list of objects at this node

    TArray<CObjectTracker::SObjEntry> Objs;
    g_pUniverse->GetGlobalObjects().GetGalacticMapObjects(pNode, Objs);
    if (Objs.GetCount() == 0)
        return false;

    //  Now add them to our result sorted and collated

    for (i = 0; i < Objs.GetCount(); i++)
        {
        //  Friendly stations go first, followed by neutral, followed by enemy

        int iDispSort = (Objs[i].fFriendly ? 1 : (Objs[i].fEnemy ? 3 : 2));

        //  Higher level stations go first

        int iLevelSort = (MAX_ITEM_LEVEL + 1 - Objs[i].pType->GetLevel());

        //  Generate a sort string. We want stations with the same type and name
        //  to be collapsed into a single entry.

        CString sSort = strPatternSubst(CONSTLIT("%d-%02d-%08x-%s"), iDispSort, iLevelSort, Objs[i].pType->GetUNID(), Objs[i].sName);

        //  Add to our result list

        SObjDesc *pEntry = Results.SetAt(sSort);
        pEntry->iCount++;
        pEntry->ObjData = Objs[i];
        }

    //  Done

    return true;
    }
