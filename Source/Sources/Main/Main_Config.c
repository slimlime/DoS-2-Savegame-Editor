
//<<>-<>>---------------------------------------------------------------------()
/*
	Configuration
									      */
//()-------------------------------------------------------------------<<>-<>>//

// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� Donn�es							  	  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

#include "_Global.h"
#include "Application.h"
#include "Locale.h"
#include "Texts.h"
#include "Menus.h"
#include "Startup.h"
#include "LastFiles.h"
#include "Game.h"
#include "Dialogs.h"
#include "Requests.h"
#include "Utils.h"

extern APPLICATION		App;
extern CUSTOMMENUTEMPLATE	MainMenu;


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� Chargement et sauvegarde					  	  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

// �算� Chargement ��������������������������������������������������������

void Config_Load(CONFIG *pConfig)
{
	NODE		Root;
	FILEHEADER	Header;
	HANDLE		hFile;
	CONFIGENTRY*	pEntry;
	void*		pData;
	WCHAR*		pszErrorMsg;
	UINT		uLen;
	UINT		uIdent;
	UINT		uType;
	DWORD		dwRead;
	BOOL		bCompleted;
	BOOL		bTempPathIsValid;
	BOOL		bLarianPathIsValid;

	if (!PathFileExists(szConfigPath)) return;

	ZeroMemory(&Root,sizeof(NODE));
	bCompleted = FALSE;
	pszErrorMsg = Locale_GetText(TEXT_ERR_CONFIGLOAD);
	if (!pszErrorMsg) pszErrorMsg = szConfigLoadErr;

	hFile = CreateFile(szConfigPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (hFile == INVALID_HANDLE_VALUE) goto Done;

	//--- V�rification de l'en-t�te ---

	if (!ReadFile(hFile,&Header,sizeof(FILEHEADER),&dwRead,NULL)) goto Done;
	if (dwRead != sizeof(FILEHEADER)) goto Done;

	if (Header.head != FILE_HEADER_CONFIG || Header.version == 0 || Header.version > CONFIG_THIS_VERSION)
		{
		SetLastError(ERROR_INVALID_DATA);
		goto Done;
		}

	//--- Chargement de la configuration ---

	while (1)
		{
		if (!ReadFile(hFile,&uIdent,sizeof(UINT),&dwRead,NULL)) goto Done;
		if (dwRead != sizeof(UINT)) break;
		if (!ReadFile(hFile,&uType,sizeof(UINT),&dwRead,NULL)) goto Done;
		if (dwRead != sizeof(UINT)) goto Done;

		pEntry = HeapAlloc(App.hHeap,HEAP_ZERO_MEMORY,sizeof(CONFIGENTRY));
		if (!pEntry)
			{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			goto Done;
			}
		pEntry->node.type = uType;
		pEntry->uIdent = uIdent;
		List_AddEntry((NODE *)pEntry,&Root);

		switch(uType)
			{
			case CONFIG_TYPE_TEXT:
				if (!ReadFile(hFile,&uLen,sizeof(UINT),&dwRead,NULL)) goto Done;
				if (dwRead != sizeof(UINT)) goto Done;
				if (!uLen) break;
				pEntry->pszValue = HeapAlloc(App.hHeap,0,uLen*sizeof(WCHAR)+sizeof(WCHAR));
				if (!pEntry->pszValue) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto Done; }
				if (!ReadFile(hFile,pEntry->pszValue,uLen*sizeof(WCHAR),&dwRead,NULL)) goto Done;
				if (dwRead != uLen*sizeof(WCHAR)) goto Done;
				pEntry->pszValue[uLen] = 0;
				break;
			case CONFIG_TYPE_UINT:
				if (!ReadFile(hFile,&pEntry->uValue,sizeof(UINT),&dwRead,NULL)) goto Done;
				if (dwRead != sizeof(UINT)) goto Done;
				break;
			case CONFIG_TYPE_BOOL:
				if (!ReadFile(hFile,&pEntry->bValue,sizeof(BOOL),&dwRead,NULL)) goto Done;
				if (dwRead != sizeof(BOOL)) goto Done;
				break;
			}
		}

	//--- Applique la configuration en conservant les param�tres par d�faut ---

	for (pEntry = (CONFIGENTRY *)Root.next; pEntry != NULL; pEntry = (CONFIGENTRY *)pEntry->node.next)
		{
		switch(pEntry->uIdent)
			{
			case CONFIG_IDENT_SAVEONEXIT_V1:
				pData = &pConfig->bSaveOnExit;
				break;
			case CONFIG_IDENT_LOCALENAME_V1:
				pData = &pConfig->pszLocaleName;
				break;
			case CONFIG_IDENT_GAME_V1:
				pData = &pConfig->uGame;
				break;
			case CONFIG_IDENT_PROFILE_V1:
				pData = &pConfig->pszProfile;
				break;
			case CONFIG_IDENT_SHOWHIDDENTAGS_V1:
				pData = &pConfig->bShowHiddenTags;
				break;
			case CONFIG_IDENT_RUNESGROUPS_V1:
				pData = &pConfig->bRunesGroups;
				break;
			case CONFIG_IDENT_RUNESVIEW_V1:
				pData = &pConfig->uRunesView;
				break;
			case CONFIG_IDENT_SKILLSGROUPS_V1:
				pData = &pConfig->bSkillsGroups;
				break;
			case CONFIG_IDENT_SKILLSVIEW_V1:
				pData = &pConfig->uSkillsView;
				break;
			case CONFIG_IDENT_BOOSTERSGROUPS_V1:
				pData = &pConfig->bBoostersGroups;
				break;
			case CONFIG_IDENT_CAPOVERRIDE_V1:
				pData = &pConfig->bCapOverride;
				break;
			case CONFIG_IDENT_SAVELOCATION_V1:
				pData = &pConfig->pszLarianPath;
				break;
			case CONFIG_IDENT_TEMPLOCATION_V1:
				pData = &pConfig->pszTempPath;
				break;
			default:pData = NULL;
			}

		if (!pData) continue;

		switch(pEntry->node.type)
			{
			case CONFIG_TYPE_TEXT:
				if (*(WCHAR **)pData) HeapFree(App.hHeap,0,*(WCHAR **)pData);
				*(WCHAR **)pData = pEntry->pszValue;
				pEntry->pszValue = NULL;
				break;
			case CONFIG_TYPE_UINT:
				*(UINT *)pData = pEntry->uValue;
				break;
			case CONFIG_TYPE_BOOL:
				*(BOOL *)pData = pEntry->bValue;
				break;
			}
		}

	bCompleted = TRUE;

	//--- Termin� ---

Done:	if (!bCompleted) Request_PrintError(App.hWnd,pszErrorMsg,NULL,MB_ICONERROR);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	for (pEntry = (CONFIGENTRY *)Root.next; pEntry != NULL; pEntry = (CONFIGENTRY *)pEntry->node.next)
		{
		switch(pEntry->node.type)
			{
			case CONFIG_TYPE_TEXT:
				if (pEntry->pszValue) HeapFree(App.hHeap,0,pEntry->pszValue);
				break;
			}
		}
	List_ReleaseMemory(&Root);

	//--- V�rifie que les chemins sont toujours valides ---

	bLarianPathIsValid = PathFileExists(pConfig->pszLarianPath);
	bTempPathIsValid = PathFileExists(pConfig->pszTempPath);

	if (bTempPathIsValid && !bLarianPathIsValid) // SAVEGAME path is NOT OK, TEMP path is OK
		{
		WCHAR*	pszWarningMsg;
		WCHAR*	pszWarningTitle;

		pszWarningMsg = Locale_GetText(TEXT_ERR_CONFIGCUSTOMSAVELOCATION);
		pszWarningTitle = Locale_GetText(TEXT_TITLE_WARNING);
		if (!pszWarningMsg) pszWarningMsg = szConfigCustSaveLocationErr;
		if (!pszWarningTitle) pszWarningTitle = szWarning;
		MessageBox(App.hWnd,pszWarningMsg,pszWarningTitle,MB_ICONWARNING|MB_OK);
		Config_DefaultSaveLocation(pConfig,FALSE);
		}

	else if (!bTempPathIsValid && bLarianPathIsValid) // SAVEGAME path is OK, TEMP path is NOT OK
		{
		WCHAR*	pszWarningMsg;
		WCHAR*	pszWarningTitle;

		pszWarningMsg = Locale_GetText(TEXT_ERR_CONFIGCUSTOMTEMPLOCATION);
		pszWarningTitle = Locale_GetText(TEXT_TITLE_WARNING);
		if (!pszWarningMsg) pszWarningMsg = szConfigCustTempLocationErr;
		if (!pszWarningTitle) pszWarningTitle = szWarning;
		MessageBox(App.hWnd,pszWarningMsg,pszWarningTitle,MB_ICONWARNING|MB_OK);
		Config_DefaultTempLocation(pConfig,FALSE);
		}

	else if (!bTempPathIsValid && !bLarianPathIsValid) // BOTH paths are NOT OK
		{
		WCHAR*	pszWarningMsg;
		WCHAR*	pszWarningTitle;

		pszWarningMsg = Locale_GetText(TEXT_ERR_CONFIGCUSTOMLOCATION);
		pszWarningTitle = Locale_GetText(TEXT_TITLE_WARNING);
		if (!pszWarningMsg) pszWarningMsg = szConfigCustLocationErr;
		if (!pszWarningTitle) pszWarningTitle = szWarning;
		MessageBox(App.hWnd,pszWarningMsg,pszWarningTitle,MB_ICONWARNING|MB_OK);
		Config_DefaultSaveLocation(pConfig,FALSE);
		Config_DefaultTempLocation(pConfig,FALSE);
		}

	return;
}


// �算� Sauvegarde ��������������������������������������������������������

BOOL Config_Save(BOOL bQuiet, CONFIG *pConfig)
{
	HANDLE		hFile;
	DWORD		dwWrite;
	BOOL		bCompleted;

	bCompleted = FALSE;
	hFile = CreateFile(szConfigPath,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE) goto Done;
	if (!WriteFile(hFile,&pConfig->header,sizeof(FILEHEADER),&dwWrite,NULL)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_BOOL,CONFIG_IDENT_SAVEONEXIT_V1,&pConfig->bSaveOnExit)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_TEXT,CONFIG_IDENT_LOCALENAME_V1,&pConfig->pszLocaleName)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_UINT,CONFIG_IDENT_GAME_V1,&pConfig->uGame)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_TEXT,CONFIG_IDENT_PROFILE_V1,&pConfig->pszProfile)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_BOOL,CONFIG_IDENT_SHOWHIDDENTAGS_V1,&pConfig->bShowHiddenTags)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_BOOL,CONFIG_IDENT_RUNESGROUPS_V1,&pConfig->bRunesGroups)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_UINT,CONFIG_IDENT_RUNESVIEW_V1,&pConfig->uRunesView)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_BOOL,CONFIG_IDENT_SKILLSGROUPS_V1,&pConfig->bSkillsGroups)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_UINT,CONFIG_IDENT_SKILLSVIEW_V1,&pConfig->uSkillsView)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_BOOL,CONFIG_IDENT_BOOSTERSGROUPS_V1,&pConfig->bBoostersGroups)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_BOOL,CONFIG_IDENT_CAPOVERRIDE_V1,&pConfig->bCapOverride)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_TEXT,CONFIG_IDENT_SAVELOCATION_V1,&pConfig->pszLarianPath)) goto Done;
	if (!Config_WriteEntry(hFile,CONFIG_TYPE_TEXT,CONFIG_IDENT_TEMPLOCATION_V1,&pConfig->pszTempPath)) goto Done;
	bCompleted = TRUE;

Done:	if (!bCompleted) Request_PrintError(App.hWnd,Locale_GetText(TEXT_ERR_CONFIGWRITE),NULL,MB_ICONERROR);
	if (hFile != INVALID_HANDLE_VALUE)
		{
		CloseHandle(hFile);
		if (!bCompleted) DeleteFile(szConfigPath);
		}
	if (bCompleted && !bQuiet) MessageBox(App.hWnd,Locale_GetText(TEXT_CONFIG_WRITTEN),Locale_GetText(TEXT_TITLE_INFO),MB_ICONINFORMATION|MB_OK);
	return(bCompleted);
}

//--- Ecriture d'une entr�e ---

int Config_WriteEntry(HANDLE hFile, UINT uType, UINT uIdent, void *pData)
{
	DWORD	dwWrite;
	UINT	uLen;

	if (!WriteFile(hFile,&uIdent,sizeof(UINT),&dwWrite,NULL)) return(0);
	if (!WriteFile(hFile,&uType,sizeof(UINT),&dwWrite,NULL)) return(0);

	switch(uType)
		{
		case CONFIG_TYPE_TEXT:
			uLen = 0;
			if (*(WCHAR **)pData) uLen = wcslen(*(WCHAR **)pData);
			if (!WriteFile(hFile,&uLen,sizeof(UINT),&dwWrite,NULL)) return(0);
			if (*(WCHAR **)pData && !WriteFile(hFile,*(WCHAR **)pData,uLen*sizeof(WCHAR),&dwWrite,NULL)) return(0);
			break;
		case CONFIG_TYPE_UINT:
			if (!WriteFile(hFile,pData,sizeof(UINT),&dwWrite,NULL)) return(0);
			break;
		case CONFIG_TYPE_BOOL:
			if (!WriteFile(hFile,pData,sizeof(BOOL),&dwWrite,NULL)) return(0);
			break;
		}

	return(1);
}


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� S�lection de la langue					  	  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

// �算� S�lection de la langue ����������������������������������������

void Config_SelectLanguage()
{
	NODE	Root;
	INT_PTR	iResult;

	if (!Locale_Enum(App.hWnd,szLangPath,&Root)) return;

	iResult = DialogBoxParam(App.hInstance,MAKEINTRESOURCE(1100),App.hWnd,Config_SelectLanguageProc,(LPARAM)&Root);
	if (!iResult || iResult == -1)
		{
		Request_PrintError(App.hWnd,Locale_GetText(TEXT_ERR_LOCALE_SELECT),NULL,MB_ICONERROR);
		Locale_EnumRelease(&Root);
		return;
		}

	Locale_EnumRelease(&Root);
	return;
}

//--- Bo�te de dialogue ---

INT_PTR CALLBACK Config_SelectLanguageProc(HWND hDlg, UINT uMsgId, WPARAM wParam, LPARAM lParam)
{
	NODE*		pRoot;

	if (uMsgId == WM_INITDIALOG)
		{
		LOCALE_ENUM*	pEnum;
		WCHAR*		pszText;
		LRESULT		lResult;
		DWORD_PTR	vl[1];

		SetWindowLongPtr(hDlg,GWLP_USERDATA,(LONG_PTR)lParam);
		pRoot = (NODE *)lParam;

		for (pEnum = (LOCALE_ENUM *)pRoot->next; pEnum != NULL; pEnum = (LOCALE_ENUM *)pEnum->node.next)
			{
			//--- Add string
			lResult = SendDlgItemMessage(hDlg,200,LB_ADDSTRING,0,(LPARAM)pEnum);
			if (lResult == LB_ERR || lResult == LB_ERRSPACE)
				{
				EndDialog(hDlg,-1);
				return(FALSE);
				}
			//--- Load icon
			vl[0] = (DWORD_PTR)pEnum->szLang;
			if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,szLangIcoPath,0,0,(WCHAR *)&pszText,1,(va_list *)&vl))
				{
				pEnum->hIcon = LoadImage(NULL,pszText,IMAGE_ICON,16,16,LR_LOADFROMFILE|LR_DEFAULTCOLOR);
				LocalFree(pszText);
				}
			//--- Select current language
			if (!wcscmp(pEnum->szLang,App.Config.pszLocaleName))
				{
				SendDlgItemMessage(hDlg,200,LB_SETCURSEL,(WPARAM)lResult,0);
				EnableWindow(GetDlgItem(hDlg,IDOK),TRUE);
				}
			}

		SendDlgItemMessage(hDlg,IDOK,WM_SETTEXT,0,(LPARAM)Locale_GetText(TEXT_OK));
		SendDlgItemMessage(hDlg,IDCANCEL,WM_SETTEXT,0,(LPARAM)Locale_GetText(TEXT_CANCEL));
		SetWindowText(hDlg,Locale_GetText(IDM_CONFIGLANGUAGE));
		Dialog_CenterWindow(hDlg,App.hWnd);
		return(FALSE);
		}

	pRoot = (NODE *)GetWindowLongPtr(hDlg,GWLP_USERDATA);

	switch(uMsgId)
		{
		case WM_MEASUREITEM:
			switch(wParam)
				{
				case 200:
					((MEASUREITEMSTRUCT *)lParam)->itemWidth = 0;
					((MEASUREITEMSTRUCT *)lParam)->itemHeight = App.Font.uFontHeight+4;
					if (((MEASUREITEMSTRUCT *)lParam)->itemHeight < 24) ((MEASUREITEMSTRUCT *)lParam)->itemHeight = 24;
					return(TRUE);
				}
			break;

		case WM_DRAWITEM:
			switch(wParam)
				{
				case 200:
					Config_SelectLanguageDrawItem((DRAWITEMSTRUCT *)lParam);
					return(TRUE);
				}
			break;

		case WM_COMMAND:
			switch(HIWORD(wParam))
				{
				case LBN_DBLCLK:
					switch(LOWORD(wParam))
						{
						case 200:
							if (!Config_SelectLanguageSet(hDlg)) return(TRUE);
							EndDialog(hDlg,IDOK);
							return(TRUE);
						}
					break;

				case BN_CLICKED:
					switch(LOWORD(wParam))
						{
						case IDOK:
							if (!Config_SelectLanguageSet(hDlg)) return(TRUE);
							EndDialog(hDlg,IDOK);
							return(TRUE);
						case IDCANCEL:
							EndDialog(hDlg,IDCANCEL);
							return(TRUE);
						}
					break;
				}
			break;

		case WM_CLOSE:
			EndDialog(hDlg,IDCANCEL);
			return(TRUE);
		}

	return(FALSE);
}

//--- Affichage ---

BOOL Config_SelectLanguageDrawItem(DRAWITEMSTRUCT *pDraw)
{
	LOCALE_ENUM*	pEnum;

	pEnum = (LOCALE_ENUM *)pDraw->itemData;
	FillRect(pDraw->hDC,&pDraw->rcItem,GetSysColorBrush(pDraw->itemState&ODS_SELECTED?COLOR_HIGHLIGHT:COLOR_WINDOW));

	if (pEnum)
		{
		RECT		rcClient;
		HFONT		hDefFont;
		COLORREF	crText;
		int		iBkMode;

		CopyRect(&rcClient,&pDraw->rcItem);
		rcClient.left += 2;
		rcClient.top += 2;
		rcClient.right -= 2;
		rcClient.bottom -= 2;
		hDefFont = SelectObject(pDraw->hDC,App.Font.hFont);
		crText = SetTextColor(pDraw->hDC,GetSysColor(pDraw->itemState&ODS_SELECTED?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT));
		iBkMode = SetBkMode(pDraw->hDC,TRANSPARENT);
		if (pEnum->hIcon) DrawIconEx(pDraw->hDC,rcClient.left,rcClient.top+(rcClient.bottom-rcClient.top-16)/2,pEnum->hIcon,16,16,0,NULL,DI_NORMAL);
		rcClient.left += 20;
		DrawText(pDraw->hDC,pEnum->pszName,-1,&rcClient,DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
		SetTextColor(pDraw->hDC,crText);
		SetBkMode(pDraw->hDC,iBkMode);
		SelectObject(pDraw->hDC,hDefFont);
		}

	if (pDraw->itemState&ODS_FOCUS) DrawFocusRect(pDraw->hDC,&pDraw->rcItem);
	return(TRUE);
}

//--- S�lection du language ---

int Config_SelectLanguageSet(HWND hDlg)
{
	LOCALE_ENUM*	pEnum;
	int		iSelected;

	iSelected = SendDlgItemMessage(hDlg,200,LB_GETCURSEL,0,0);
	if (iSelected == LB_ERR) return(0);

	pEnum = (LOCALE_ENUM *)SendDlgItemMessage(hDlg,200,LB_GETITEMDATA,(WPARAM)iSelected,0);
	if (pEnum == (LOCALE_ENUM *)LB_ERR) return(0);

	// Language not changed
	if (!wcscmp(pEnum->szLang,App.Config.pszLocaleName)) return(1);

	// Change language
	return(Config_SetLanguage(hDlg,pEnum->szLang));
}


// �算� Modification de la langue ����������������������������������������

int Config_SetLanguage(HWND hWnd, WCHAR *pszLang)
{
	LVGROUP		lvGroup;
	LOCALE_TEXT*	pLocale;
	WCHAR*		pszLocaleName;
	WCHAR*		pszLangCpy;

	pszLangCpy = HeapAlloc(App.hHeap,0,wcslen(pszLang)*sizeof(WCHAR)+sizeof(WCHAR));
	if (pszLangCpy)
		{
		wcscpy(pszLangCpy,pszLang);
		if (Locale_Load(hWnd,szLangPath,pszLang,LOCALE_TYPE_APPLICATION,(void **)&pLocale,&pszLocaleName))
			{
			//--- Destroy the menu ---
			SetMenu(App.hWnd,NULL);
			DrawMenuBar(App.hWnd);
			Menu_Release(App.hMenu,&MainMenu);
			App.hMenu = NULL;

			//--- Destroy the XML view ---
			Tree_Destroy();

			//--- Apply the new language ---
			Locale_Unload(LOCALE_TYPE_APPLICATION,(void **)&App.pLocaleTexts,&App.pszLocaleName);
			if (App.Config.pszLocaleName) HeapFree(App.hHeap,0,App.Config.pszLocaleName);
			App.Config.pszLocaleName = pszLangCpy;
			App.pLocaleTexts = pLocale;
			App.pszLocaleName = pszLocaleName;

			//--- Recreate the menu ---
			App.hMenu = Menu_Create(&MainMenu);
			if (App.hMenu)
				{
				SetMenu(App.hWnd,App.hMenu);
				DrawMenuBar(App.hWnd);
				}
			if (App.Game.Save.pszSaveName)
				{
				Game_Lock(GAME_LOCK_ENABLED|GAME_LOCK_FILE);
				if (App.Game.pdcCurrent) Game_Lock(GAME_LOCK_ENABLED|GAME_LOCK_TREE);
				}
			Menu_SetFlag(App.hMenu,IDM_CONFIGCAPOVERRIDE,App.Config.bCapOverride);
			Menu_SetFlag(App.hMenu,IDM_CONFIGSAVEONEXIT,App.Config.bSaveOnExit);

			//--- Update the accelerators ---
			if (App.hShortcuts) DestroyAcceleratorTable(App.hShortcuts);
			App.hShortcuts = NULL;
			Initialise_Accelerators();

			//--- Append last files ---
			LastFiles_AppendItems();

			//--- Update buttons texts ---
			SetWindowText(App.Game.Layout.hwndAbilitiesBtn,Locale_GetText(TEXT_BUTTON_ABILITIES));
			SetWindowText(App.Game.Layout.hwndTagsBtn,Locale_GetText(TEXT_BUTTON_TAGS));
			SetWindowText(App.Game.Layout.hwndTalentsBtn,Locale_GetText(TEXT_BUTTON_TALENTS));
			SetWindowText(App.Game.Layout.hwndMenuBtn,Locale_GetText(TEXT_BUTTON_MENU));

			//--- Update inventory list groups names ---
			lvGroup.cbSize = sizeof(LVGROUP);
			lvGroup.mask = LVGF_HEADER;
			lvGroup.pszHeader = Locale_GetText(TEXT_INV_NAV);
			SendMessage(App.Game.Layout.hwndInventory,LVM_SETGROUPINFO,(WPARAM)0,(LPARAM)&lvGroup);
			lvGroup.pszHeader = Locale_GetText(TEXT_INV_BAGS);
			SendMessage(App.Game.Layout.hwndInventory,LVM_SETGROUPINFO,(WPARAM)1,(LPARAM)&lvGroup);
			lvGroup.pszHeader = Locale_GetText(TEXT_INV_EQUIPPED);
			SendMessage(App.Game.Layout.hwndInventory,LVM_SETGROUPINFO,(WPARAM)2,(LPARAM)&lvGroup);
			lvGroup.pszHeader = Locale_GetText(TEXT_INV_ITEMS);
			SendMessage(App.Game.Layout.hwndInventory,LVM_SETGROUPINFO,(WPARAM)3,(LPARAM)&lvGroup);

			//--- Update window ---
			InvalidateRect(App.Game.Layout.hwndList,NULL,FALSE);
			InvalidateRect(App.hWnd,NULL,FALSE);
			UpdateWindow(App.hWnd);
			}
		else
			{
			HeapFree(App.hHeap,0,pszLangCpy);
			return(0);
			}
		}
	else
		{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		Request_PrintError(hWnd,Locale_GetText(TEXT_ERR_LOCALE_SELECT),NULL,MB_ICONERROR);
		return(0);
		}

	return(1);
}


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� S�lection des r�pertoires				  	  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

// �算� Restaurer les r�pertoires temporaires ����������������������������

void Config_ResetLocations()
{
	CONFIG*		pConfig;
	BOOL		bTempIsValid;
	BOOL		bLarianIsValid;
	int		iTempResult;
	int		iLarianResult;

	if (App.Game.Save.pszSaveName)
		{
		MessageBox(App.hWnd,Locale_GetText(TEXT_CONFIG_OPENEDSAVEGAME),Locale_GetText(TEXT_TITLE_INFO),MB_ICONEXCLAMATION|MB_OK);
		return;
		}

	pConfig = HeapAlloc(App.hHeap,HEAP_ZERO_MEMORY,sizeof(CONFIG));
	if (!pConfig) return;

	bTempIsValid = Config_DefaultTempLocation(pConfig,TRUE);
	bLarianIsValid = Config_DefaultSaveLocation(pConfig,TRUE);
	if (!bTempIsValid || !bLarianIsValid) goto Done;

	iTempResult = Config_ResetLocationsRequest(pConfig->pszTempPath,App.Config.pszTempPath,TEXT_CONFIG_LOCATION_TEMP);
	if (iTempResult == IDCANCEL) goto Done;
	iLarianResult = Config_ResetLocationsRequest(pConfig->pszLarianPath,App.Config.pszLarianPath,TEXT_CONFIG_LOCATION_LARIAN);
	if (iLarianResult == IDCANCEL) goto Done;

	if (iTempResult == IDYES)
		{
		if (App.Config.pszTempPath) HeapFree(App.hHeap,0,App.Config.pszTempPath);
		App.Config.pszTempPath = pConfig->pszTempPath;
		pConfig->pszTempPath = NULL;
		}

	if (iLarianResult == IDYES)
		{
		if (App.Config.pszLarianPath) HeapFree(App.hHeap,0,App.Config.pszLarianPath);
		App.Config.pszLarianPath = pConfig->pszLarianPath;
		pConfig->pszLarianPath = NULL;
		}

Done:	Config_Release(pConfig);
	HeapFree(App.hHeap,0,pConfig);
	return;
}

//--- Requ�te de confirmation ---

int Config_ResetLocationsRequest(WCHAR *pszNewPath, WCHAR *pszOldPath, UINT uLocaleId)
{
	WCHAR*		pszText;
	DWORD_PTR	vl[3];
	int		iResult;

	vl[0] = (DWORD_PTR)pszNewPath;
	vl[1] = (DWORD_PTR)Locale_GetText(uLocaleId);
	vl[2] = (DWORD_PTR)pszOldPath;
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,Locale_GetText(TEXT_CONFIG_LOCATION),0,0,(WCHAR *)&pszText,1,(va_list *)&vl)) return(IDCANCEL);
	iResult = MessageBox(App.hWnd,pszText,Locale_GetText(IDM_CONFIGRESETLOCATIONS),MB_ICONQUESTION|MB_YESNOCANCEL);
	LocalFree(pszText);
	return(iResult);
}


// �算� S�lection du r�pertoire temporaire �������������������������������

void Config_SelectTempLocation()
{
	IFileDialog*		pFileDialog;
	IShellItem*		pItem;
	HRESULT			hr;

	if (App.Game.Save.pszSaveName)
		{
		MessageBox(App.hWnd,Locale_GetText(TEXT_CONFIG_OPENEDSAVEGAME),Locale_GetText(TEXT_TITLE_INFO),MB_ICONEXCLAMATION|MB_OK);
		return;
		}

	hr = CoCreateInstance(&CLSID_FileOpenDialog,NULL,CLSCTX_INPROC,&IID_IFileDialog,(void *)&pFileDialog);
	if (SUCCEEDED(hr))
		{
		pFileDialog->lpVtbl->SetTitle(pFileDialog,Locale_GetText(TEXT_CONFIG_TEMPLOCATION));
		pFileDialog->lpVtbl->SetOptions(pFileDialog,FOS_PICKFOLDERS|FOS_FORCEFILESYSTEM);
		if (App.Config.pszTempPath)
			{
			hr = SHCreateItemFromParsingName(App.Config.pszTempPath,NULL,&IID_IShellItem,(void *)&pItem);
			if (SUCCEEDED(hr))
				{
				pFileDialog->lpVtbl->SetFolder(pFileDialog,pItem);
				pItem->lpVtbl->Release(pItem);
				}
			}

		hr = pFileDialog->lpVtbl->Show(pFileDialog,NULL);
		if (SUCCEEDED(hr))
			{
			WCHAR*		pszFilePath;

			hr = pFileDialog->lpVtbl->GetResult(pFileDialog,&pItem);
			if (SUCCEEDED(hr))
				{
				hr = pItem->lpVtbl->GetDisplayName(pItem,SIGDN_FILESYSPATH,&pszFilePath);
				if (SUCCEEDED(hr))
					{
					if (App.Config.pszTempPath) HeapFree(App.hHeap,0,App.Config.pszTempPath);
					App.Config.pszTempPath = Misc_StrCpyAlloc(pszFilePath);
					CoTaskMemFree(pszFilePath);
					}
				pItem->lpVtbl->Release(pItem);
				}
			}
		pFileDialog->lpVtbl->Release(pFileDialog);
		}

	return;
}


// �算� S�lection du r�pertoire des sauvegardes ��������������������������

void Config_SelectSaveLocation()
{
	IFileDialog*		pFileDialog;
	IShellItem*		pItem;
	HRESULT			hr;

	if (App.Game.Save.pszSaveName)
		{
		MessageBox(App.hWnd,Locale_GetText(TEXT_CONFIG_OPENEDSAVEGAME),Locale_GetText(TEXT_TITLE_INFO),MB_ICONEXCLAMATION|MB_OK);
		return;
		}

	hr = CoCreateInstance(&CLSID_FileOpenDialog,NULL,CLSCTX_INPROC,&IID_IFileDialog,(void *)&pFileDialog);
	if (SUCCEEDED(hr))
		{
		pFileDialog->lpVtbl->SetTitle(pFileDialog,Locale_GetText(TEXT_CONFIG_SAVELOCATION));
		pFileDialog->lpVtbl->SetOptions(pFileDialog,FOS_PICKFOLDERS|FOS_FORCEFILESYSTEM);
		if (App.Config.pszLarianPath)
			{
			hr = SHCreateItemFromParsingName(App.Config.pszLarianPath,NULL,&IID_IShellItem,(void *)&pItem);
			if (SUCCEEDED(hr))
				{
				pFileDialog->lpVtbl->SetFolder(pFileDialog,pItem);
				pItem->lpVtbl->Release(pItem);
				}
			}

		hr = pFileDialog->lpVtbl->Show(pFileDialog,NULL);
		if (SUCCEEDED(hr))
			{
			WCHAR*		pszFilePath;

			hr = pFileDialog->lpVtbl->GetResult(pFileDialog,&pItem);
			if (SUCCEEDED(hr))
				{
				hr = pItem->lpVtbl->GetDisplayName(pItem,SIGDN_FILESYSPATH,&pszFilePath);
				if (SUCCEEDED(hr))
					{
					if (App.Config.pszLarianPath) HeapFree(App.hHeap,0,App.Config.pszLarianPath);
					App.Config.pszLarianPath = Misc_StrCpyAlloc(pszFilePath);
					CoTaskMemFree(pszFilePath);
					}
				pItem->lpVtbl->Release(pItem);
				}
			}
		pFileDialog->lpVtbl->Release(pFileDialog);
		}

	return;
}


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� Fonctions						  	  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

// �算� Valeurs par d�faut �����������������������������������������������

int Config_Defaults(CONFIG *pConfig)
{
	WCHAR*			pszError;
	WCHAR*			pszTemp;
	int			iSize;

	pszError = Locale_GetText(TEXT_ERR_CONFIG);
	if (!pszError) pszError = szConfigErr;

	//--- Header ---

	pConfig->header.head = FILE_HEADER_CONFIG;
	pConfig->header.version = CONFIG_THIS_VERSION;
	pConfig->header.size = sizeof(CONFIG);

	//--- Config ---

	pConfig->bSaveOnExit = TRUE;

	//--- Locale database ---

	iSize = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,LOCALE_SISO639LANGNAME2,NULL,0);
	pConfig->pszLocaleName = HeapAlloc(App.hHeap,0,iSize*sizeof(WCHAR));
	if (!pConfig->pszLocaleName)
		{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		Request_PrintError(NULL,pszError,NULL,MB_ICONHAND);
		return(0);
		}
	GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,LOCALE_SISO639LANGNAME2,pConfig->pszLocaleName,iSize);

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,szLangPath,0,0,(WCHAR *)&pszTemp,1,(va_list *)&pConfig->pszLocaleName))
		{
		Request_PrintError(NULL,pszError,NULL,MB_ICONHAND);
		return(0);
		}

	if (!PathFileExists(pszTemp))
		{
		HeapFree(App.hHeap,0,pConfig->pszLocaleName);
		pConfig->pszLocaleName = Misc_StrCpyAlloc(szLangDefault);
		if (!pConfig->pszLocaleName)
			{
			Request_PrintError(NULL,pszError,NULL,MB_ICONHAND);
			LocalFree(pszTemp);
			return(0);
			}
		}

	LocalFree(pszTemp);

	//--- Jeu ---

	Config_DefaultSaveLocation(pConfig,FALSE);
	Config_DefaultTempLocation(pConfig,FALSE);

	//--- Edition ---

	pConfig->bRunesGroups = TRUE;
	pConfig->uRunesView = LV_VIEW_TILE;
	pConfig->bSkillsGroups = TRUE;
	pConfig->uSkillsView = LV_VIEW_ICON;
	pConfig->bBoostersGroups = TRUE;

	//--- Configuration sauvegard�e ---

	Config_Load(pConfig);
	return(1);
}


// �算� R�pertoire temporaire par d�faut ���������������������������������

BOOL Config_DefaultTempLocation(CONFIG *pConfig, BOOL bQuiet)
{
	WCHAR*			pszWarningMsg;
	WCHAR*			pszWarningTitle;
	int			iSize;

	pszWarningMsg = Locale_GetText(TEXT_ERR_CONFIGTEMPLOCATION);
	pszWarningTitle = Locale_GetText(TEXT_TITLE_WARNING);
	if (!pszWarningMsg) pszWarningMsg = szConfigTempLocationErr;
	if (!pszWarningTitle) pszWarningTitle = szWarning;

	iSize = GetTempPath(0,NULL);
	pConfig->pszTempPath = HeapAlloc(App.hHeap,0,iSize*sizeof(WCHAR)+sizeof(WCHAR));
	if (!pConfig->pszTempPath)
		{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		if (!bQuiet) Request_PrintError(NULL,pszWarningMsg,pszWarningTitle,MB_ICONWARNING|MB_OK);
		return(FALSE);
		}
	GetTempPath(iSize,pConfig->pszTempPath);
	return(TRUE);
}


// �算� R�pertoire par d�faut pour les sauvegardes �����������������������

BOOL Config_DefaultSaveLocation(CONFIG *pConfig, BOOL bQuiet)
{
	static const GUID	FOLDERID_Documents = { 0xfdd39ad0, 0x238f, 0x46af, { 0xad, 0xb4, 0x6c, 0x85, 0x48, 0x03, 0x69, 0xc7 } };
	WCHAR*			pszTemp;
	WCHAR*			pszWarningMsg;
	WCHAR*			pszWarningTitle;

	pszWarningMsg = Locale_GetText(TEXT_ERR_CONFIGSAVELOCATION);
	pszWarningTitle = Locale_GetText(TEXT_TITLE_WARNING);
	if (!pszWarningMsg) pszWarningMsg = szConfigSaveLocationErr;
	if (!pszWarningTitle) pszWarningTitle = szWarning;

	SHGetKnownFolderPath(&FOLDERID_Documents,KF_FLAG_DEFAULT,NULL,&pszTemp);
	if (!pszTemp)
		{
		if (!bQuiet) Request_PrintError(NULL,pszWarningMsg,pszWarningTitle,MB_ICONWARNING|MB_OK);
		return(FALSE);
		}
	pConfig->pszLarianPath = HeapAlloc(App.hHeap,0,wcslen(pszTemp)*sizeof(WCHAR)+sizeof(WCHAR)+wcslen(szLarianStudios)*sizeof(WCHAR)+sizeof(WCHAR));
	if (!pConfig->pszLarianPath)
		{
		if (!bQuiet) Request_PrintError(NULL,pszWarningMsg,pszWarningTitle,MB_ICONWARNING|MB_OK);
		CoTaskMemFree(pszTemp);
		return(FALSE);
		}
	wcscpy(pConfig->pszLarianPath,pszTemp);
	PathAppend(pConfig->pszLarianPath,szLarianStudios);
	CoTaskMemFree(pszTemp);
	return(TRUE);
}


// �算� Lib�re les donn�es utilis�es par la configuration ����������������

void Config_Release(CONFIG *pConfig)
{
	if (pConfig->pszLocaleName) HeapFree(App.hHeap,0,pConfig->pszLocaleName);
	if (pConfig->pszProfile) HeapFree(App.hHeap,0,pConfig->pszProfile);
	if (pConfig->pszLarianPath) HeapFree(App.hHeap,0,pConfig->pszLarianPath);
	if (pConfig->pszTempPath) HeapFree(App.hHeap,0,pConfig->pszTempPath);
	pConfig->pszLocaleName = NULL;
	pConfig->pszProfile = NULL;
	pConfig->pszLarianPath = NULL;
	pConfig->pszTempPath = NULL;
	return;
}
