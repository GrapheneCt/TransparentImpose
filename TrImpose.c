#include <taihen.h>
#include <vitasdk.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h> 
#include <psp2/types.h> 

void _start() __attribute__((weak, alias("module_start")));

static __inline SceInt32 sceAppMgrGetAppState(SceAppMgrAppState *appState)
{
	return _sceAppMgrGetAppState(appState, sizeof(SceAppMgrAppState), 0x00);
}

//TODO: Make a2 user-adjustable, fix lockscreen issue
int sceAppMgrSetDisplayMergeConf(int a1, int a2);

int uiWatcher(SceSize argc, void* argv) {

	sceKernelDelayThread(1000000);
	SceAppMgrAppState appState;
	int impose = SCE_FALSE;

	while (1) {
		sceAppMgrGetAppState(&appState);
		if (impose != appState.isSystemUiOverlaid && appState.isSystemUiOverlaid == SCE_TRUE)
			sceAppMgrSetDisplayMergeConf(0x100, 0x80);
		else if (impose != appState.isSystemUiOverlaid)
			sceAppMgrSetDisplayMergeConf(0x00, 0x100);
		impose = appState.isSystemUiOverlaid;
		sceKernelDelayThread(10000);
	}

	return 0;
}


static tai_hook_ref_t app_start_ref;
int hook_app_start(SceSize argc, const void *args) {

	SceUID ui_watcher = sceKernelCreateThread("UI Overlayed Watcher", uiWatcher, 191, 1, 0, 0, NULL);
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