
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"
#include "XPLMPlugin.h"


#include "checklist_items.h"

#include <string.h>
#include <stdio.h>
#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif

float col_white[] = { 1.0,1.0,1.0 };


int menu_container_id; // Index of menu item in the plugins menu
XPLMMenuID menu_id; // menu container to append menu items to

static XPLMWindowID g_window;



void draw(XPLMWindowID window_id, void * in_refcon);
void menu_handler(void *menu_ref, void *item_ref);

int handle_mouse(XPLMWindowID window_id, int x, int y, int is_down, void * in_refcon) { return 1; }
int dummy_mouse_handler(XPLMWindowID window_id, int x, int y, int is_down, void *in_refcon) { return xplm_CursorDefault; }
XPLMCursorStatus dummy_cursor_status_handler(XPLMWindowID window_id, int x, int y, void * in_refcon) { return 0; }
int dummy_wheel_handler(XPLMWindowID window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void dummy_key_handler(XPLMWindowID window_id, char key, XPLMKeyFlags flags,char virtual_key,void * in_refcon, int losing_focus){}


PLUGIN_API int XPluginStart(
	char * outName,
	char * outSig,
	char * outDesc)
{
	strcpy(outName, "Q4XPChecklistPlugin");
	strcpy(outSig, "blizzard.checklist.q4xp");
	strcpy(outDesc, "A checklist plugin for FlyJsim Q4XP");
	
	menu_container_id = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "Q4XP Checklist", 0, 0);
	menu_id = XPLMCreateMenu("Q4XP Checklist", XPLMFindPluginsMenu(), menu_container_id, menu_handler, NULL);
	XPLMAppendMenuItem(menu_id, "Reload Plugins", (void *)"Menu Item 1", 1);
	XPLMAppendMenuSeparator(menu_id);
	XPLMAppendMenuItem(menu_id, "Toggle Checklist", (void *)"Menu Item 2", 1);
	

	// Left,Bottom,Right,Top
	int desktop_bounds[4];

	// Get desktop bounds
	XPLMGetScreenBoundsGlobal(&desktop_bounds[0], &desktop_bounds[3], &desktop_bounds[2], &desktop_bounds[1]);

	// Window Params
	XPLMCreateWindow_t params;

	params.structSize = sizeof(params);
	params.left = desktop_bounds[0] + 100;
	params.bottom = desktop_bounds[1] + 100;
	params.right = desktop_bounds[0] + 400;
	params.top = desktop_bounds[1] + 300;
	params.visible = 0;
	params.drawWindowFunc = draw;
	params.handleMouseClickFunc = handle_mouse;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	params.decorateAsFloatingWindow = 1;

	g_window = XPLMCreateWindowEx(&params);

	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	// keep size constant as window resives. Left and top edges in same place relative to the window
	XPLMSetWindowGravity(g_window, 0, 1, 0, 1); 

	XPLMSetWindowTitle(g_window, "Q4XP Checklist");

	XPLMSetWindowResizingLimits(g_window, 450, 900, 450, 900);

	return (g_window != NULL);
}

PLUGIN_API void XPluginStop(void)
{
	XPLMDestroyWindow(g_window);
	g_window = NULL;
}

PLUGIN_API void XPluginDisable(void){}
PLUGIN_API int XPluginEnable(void) { return 1; }
PLUGIN_API void XPluginRecieveMessage(XPLMPluginID from,int msg, void *param){}

void draw(XPLMWindowID window_id, void *refcon) 
{
	XPLMSetGraphicsState(
		0,
		0,
		0,
		0,
		1,
		1,
		0
	);

	int window_left_offset, window_top_offset, window_right_offset, window_bottom_offset;

	XPLMGetWindowGeometry(window_id, &window_left_offset, &window_top_offset, &window_right_offset, &window_bottom_offset);
	
	
	int items_left_offset = window_left_offset + 20;

	int items_top_offset = window_top_offset - 20;

	// Space between checklist items
	int items_bottom_margin = 30; 

	XPLMDrawString(col_white, items_left_offset + 140, items_top_offset - 10, (char *)"PRE FLIGHT CHECKLIST", NULL, xplmFont_Proportional);

	int first_item_gap = 0;

	for (int i = 0; i < sizeof(PREFLIGHT_ITEMS) / sizeof(PREFLIGHT_ITEMS[0]); i++) {
		
		// Add extra spacing for first item
		if (i == 0) { first_item_gap = 40; } else { first_item_gap = 0;}

		XPLMDrawString(col_white, items_left_offset, (items_top_offset - 60 ) - (i*(items_bottom_margin+first_item_gap)), (char *)PREFLIGHT_ITEMS[i], NULL, xplmFont_Proportional);
	}
}

void menu_handler(void *menu_ref, void *item_ref)
{
	if(!strcmp((const char *)item_ref, "Menu Item 1"))
	{
		XPLMReloadPlugins();
	}
	else if (!strcmp((const char *)item_ref, "Menu Item 2"))
	{
		// Show plugin
		XPLMSetWindowIsVisible(g_window, 1);
	}
}