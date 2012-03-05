#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _main_menubar_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         main_menubar.c
* \author	Richard Baldock
* \date		October 2001
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \brief
* \ingroup	MA3DView
*/

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
    MenuItem	item;
    int		i;

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
