#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   MA3DView						*
*   File       :   main_menubar.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 14:25:24 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>

#include <MA3DView.h>

extern MenuItem		*HGU_XmMiscMenuItems;

static void HGU_XmPulldownAddHelp(
Widget		w,
XtCallbackProc	callback,
XtPointer	client_data)
{
    /* add the help callback to the button */
    XtAddCallback(w, XmNhelpCallback, callback, client_data);

    /* add the help callback to the menu itself */
    if( (w = XtNameToWidget(w, "*_pulldown")) != NULL )
	XtAddCallback(w, XmNhelpCallback, callback, client_data);

    return;
}

Widget create_main_menubar(
Widget	main_w)
{
    Widget	menubar, menu, widget;

    menubar = XmCreateMenuBar(main_w, "menubar", NULL, 0 );

    /* create pulldown menus */
    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "file_menu",
			     0, XmTEAR_OFF_DISABLED, file_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "NULL");

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "image_menu",
			     0, XmTEAR_OFF_DISABLED, image_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "NULL");

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "domain_menu",
			     0, XmTEAR_OFF_DISABLED, domain_menu_items);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "NULL");

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "misc_menu",
			     0, XmTEAR_OFF_ENABLED,
			     HGU_XmMiscMenuItems);
/*    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "libhguXm/HGU_XmMacroMenu.html");*/

    widget = HGU_XmBuildMenu(menubar, XmMENU_PULLDOWN, "help_menu",
			     0, XmTEAR_OFF_DISABLED, HGU_XmHelpMenuItems);
    XtAddCallback(menubar, XtNdestroyCallback, HGU_XmHelpDestroyCb, NULL);
    HGU_XmPulldownAddHelp(widget, HGU_XmHelpStandardCb,
			  "libhguXm/HGU_XmHelpMenu.html");

    XtVaSetValues(menubar, XmNmenuHelpWidget, widget, NULL);

    XtManageChild( menubar );
    return( menubar );
}

void init_main_menubar(
Widget	topl)
{
    (void) file_menu_init( topl );
    (void) image_menu_init( topl );
    (void) domain_menu_init( topl );
    (void) HGU_XmHelpMenuInit( topl );
}
