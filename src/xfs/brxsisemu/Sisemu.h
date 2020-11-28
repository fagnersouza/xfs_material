#pragma once
#include<Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SISEMU_SENSOR_SIZE		(32)
#define SISEMU_SENSOR_MAX       (SISEMU_SENSOR_SIZE - 1)

#define SISEMU_DOOR_SIZE		(16)
#define SISEMU_DOOR_MAX			(SISEMU_DOOR_SIZE - 1)

#define SISEMU_INDICATOR_SIZE	(16)
#define SISEMU_INDICATOR_MAX    (SISEMU_INDICATOR_SIZE - 1)

#define SISEMU_AUXILIARY_SIZE	(16)
#define SISEMU_AUXILIARY_MAX    (SISEMU_AUXILIARY_SIZE - 1)

#define SISEMU_GUIDLIGHT_SIZE	(16)
#define SISEMU_GUIDLIGHT_MAX    (SISEMU_GUIDLIGHT_SIZE - 1)

#define SISEMU_OK				(0)
#define SISEMU_ERROR			(1)

#define MAX_CHAR_EXTRA			(256)

typedef struct _status {
	WORD Device;
	WORD Sensors[SISEMU_SENSOR_SIZE];
	WORD Doors[SISEMU_DOOR_SIZE];
	WORD Indicators[SISEMU_INDICATOR_SIZE];
	WORD Auxiliaries[SISEMU_AUXILIARY_SIZE];
	WORD Guidlights[SISEMU_GUIDLIGHT_SIZE];
	char Extra[MAX_CHAR_EXTRA + 1];
}SISEMUSTATUS, *LPSISEMUSTATUS;

typedef struct _cap {
	WORD Device;
	WORD Sensors[SISEMU_SENSOR_SIZE];
	WORD Doors[SISEMU_DOOR_SIZE];
	WORD Indicators[SISEMU_INDICATOR_SIZE];
	WORD Auxiliaries[SISEMU_AUXILIARY_SIZE];
	WORD Guidlights[SISEMU_GUIDLIGHT_SIZE];
	char Extra[MAX_CHAR_EXTRA + 1];
}SISEMUCAP, * LPSISEMUCAP;

typedef struct _set {
	WORD Device;
	WORD Sensors[SISEMU_SENSOR_SIZE];
	WORD Doors[SISEMU_DOOR_SIZE];
	WORD Indicators[SISEMU_INDICATOR_SIZE];
	WORD Auxiliaries[SISEMU_AUXILIARY_SIZE];
	WORD Guidlights[SISEMU_GUIDLIGHT_SIZE];
}SISEMUSET, * LPSISEMUSET;

DWORD extern WINAPI SisOpen(LPSTR lpszClientName);
DWORD extern WINAPI SisClose();
DWORD extern WINAPI SisStatus(LPSISEMUSTATUS status);
DWORD extern WINAPI SisCap(LPSISEMUCAP cap);
DWORD extern WINAPI SisReset();
DWORD extern WINAPI SisSet(LPSISEMUSET set);

#ifdef __cplusplus
}       /*extern "C"*/
#endif