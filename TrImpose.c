#include <taihen.h>
#include <vitasdk.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/io/fcntl.h> 
#include <psp2/appmgr.h> 
#include <psp2/types.h> 

#define DEFAULT_TR 0xFF
#define path "ur0:data/trimpose.txt"

void _start() __attribute__((weak, alias("module_start")));

static __inline SceInt32 sceAppMgrGetAppState(SceAppMgrAppState *appState)
{
	return _sceAppMgrGetAppState(appState, sizeof(SceAppMgrAppState), 0x00);
}

int sceAppMgrIsGameBudgetAppPresent(void);
int sceAppMgrSetDisplayMergeConf(int a1, int a2);

int uiWatcher(SceSize argc, void* argv) {

	int trval = DEFAULT_TR;
	int impose = SCE_FALSE;
	SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0777);
	if (fd >= 0) {
		char buf[sizeof(int)];
		sceIoRead(fd, buf, sizeof(int));
		sceIoClose(fd);
		trval = 0;
		for (int i = 0; buf[i] != '\x0A'; i++)
			trval = trval * 10 + buf[i] - '0';
		if (trval > 0xFF)
			trval = DEFAULT_TR;
	}

	SceAppMgrAppState appState;

	while (1) {
		if (sceAppMgrIsGameBudgetAppPresent() == 1) {
			sceAppMgrGetAppState(&appState);
			if (impose != appState.isSystemUiOverlaid && appState.isSystemUiOverlaid == SCE_TRUE)
				sceAppMgrSetDisplayMergeConf(0x100, trval);
			else if (impose != appState.isSystemUiOverlaid)
				sceAppMgrSetDisplayMergeConf(0x00, 0x100);
			impose = appState.isSystemUiOverlaid;
		}
		sceKernelDelayThread(100000);
	}

	return 0;
}


static tai_hook_ref_t app_start_ref;
int hook_app_start(SceSize argc, const void *args) {

	SceUID ui_watcher = sceKernelCreateThread("UI Overlayed Watcher", uiWatcher, 191, 0x1000, 0, 0, NULL);
	sceKernelStartThread(ui_watcher, 0, NULL);

	return TAI_CONTINUE(int, app_start_ref, argc, args);
}

int module_start(SceSize argc, const void *args) {
	taiHookFunctionExport(&app_start_ref, 
		TAI_MAIN_MODULE,      
		TAI_ANY_LIBRARY, 
		0x935CD196,      
		hook_app_start); 
	return SCE_KERNEL_START_SUCCESS;
}
