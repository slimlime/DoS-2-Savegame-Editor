
//<<>-<>>---------------------------------------------------------------------()
/*
	Structures des sauvegardes
									      */
//()-------------------------------------------------------------------<<>-<>>//

#ifndef _INFOS_DEFINITIONS
#define _INFOS_DEFINITIONS


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� D�finitions							  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

enum {
	INFOS_GROUP_MISC = 1,
	INFOS_GROUP_TEAM,
	INFOS_GROUP_MODS,
};

enum {
	INFOS_MODS_NAME = 0,
	INFOS_MODS_VERSION
};

enum {
	INFOS_ICON_EMPTY = 0,
	INFOS_ICON_SAVEGAME,
	INFOS_ICON_VERSION,
	INFOS_ICON_MODDED,
	INFOS_ICON_NOTMODDED,
	INFOS_ICON_DIFFICULTY,
	INFOS_ICON_DATE,
	INFOS_ICON_CHARACTER,
	INFOS_ICON_NOTCHARACTER,
	INFOS_ICON_LARIANMOD,
	INFOS_ICON_PLAYERMOD,
	INFOS_ICON_LAST /* Used for table size */
};


// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //
// い�									  い� //
// い� Prototypes							  い� //
// い�									  い� //
// いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい //

void			Infos_Show(void);
INT_PTR CALLBACK	Infos_Proc(HWND,UINT,WPARAM,LPARAM);

void			Infos_Tree(void);
int			Infos_LoadMetaDatas(HWND);
int			Infos_Initialise(HWND);
void			Infos_PrepareAndUpdate(HWND,WCHAR *,NODE *);
WCHAR*			Infos_Get(UINT,UINT,WCHAR **,XML_NODE *);
BOOL			Infos_Draw(HWND,UINT,DRAWITEMSTRUCT *,NODE *);
void			Infos_DrawImage(DRAWITEMSTRUCT *,NODE *);

#endif
