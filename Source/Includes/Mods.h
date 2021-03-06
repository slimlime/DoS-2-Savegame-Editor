
//<<>-<>>---------------------------------------------------------------------()
/*
	Mods
									      */
//()-------------------------------------------------------------------<<>-<>>//

#ifndef _MODS_INCLUDE
#define _MODS_INCLUDE


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� D�finitions							  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

#define MOD_CRITICAL	0
#define MOD_LARIAN	1
#define MOD_USER	2


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� Structures							  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

typedef struct MODSTRUCT {
	NODE		node;
	BOOL		bIsActive;
	WCHAR*		pszName;
	WCHAR*		pszVersion;
	XML_NODE*	pxnMeta;
	XML_NODE*	pxnGlobals;
} MODSTRUCT;

typedef struct MODSCONTEXT {
	NODE		mods;
	BOOL		bInitialized;
} MODSCONTEXT;


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� Prototypes							  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

void			Mods_Dialog(void);
void			Mods_Release(NODE *);

INT_PTR CALLBACK	Mods_Proc(HWND,UINT,WPARAM,LPARAM);
void			Mods_Check(HWND,int,BOOL,MODSCONTEXT *);

#endif
