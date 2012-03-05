#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _main_buttonbar_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         main_buttonbar.c
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


/* private globals */

/* other procedures */

void newDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzObject	*obj;
  char		*file;

  /* get a file string - must be new or it will be
     rejected (silently) */
  if( file = HGU_XmUserGetFilename(globals.topl,
				   "Please type in a filename\n"
				   "for the new domain which must\n"
				   "differ from others in the list",
				   "OK", "cancel", "newDomain.wlz",
				   NULL, "*.wlz") ){
    obj = WlzAssignObject(WlzMakeEmpty(NULL), NULL);
    installNewDomain(obj, file, 0);
    WlzFreeObj(obj);
    AlcFree(file);
  }

  return;
}

void deleteDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

void clearDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}

void clearAllDomainsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}


/* create control buttons */
Widget create_main_buttonbar(
Widget	main_w)
{
    Widget	buttonbar, button, widget;
    Widget	frame, frame_title, frame_rc;

    buttonbar = XtVaCreateWidget("buttonbar", xmRowColumnWidgetClass,
				 main_w,
				 XmNborderWidth,	0,
				 XmNverticalSpacing,	2,
				 XmNorientation,	XmHORIZONTAL,
				 NULL);

    /* domain operation buttons */
    frame = XtVaCreateManagedWidget("domain_ops_frame", xmFrameWidgetClass,
				    buttonbar, NULL);
    frame_title = XtVaCreateManagedWidget("domain_ops_title",
					  xmLabelWidgetClass, frame,
					  XmNchildType, XmFRAME_TITLE_CHILD,
					  NULL);
    frame_rc = XtVaCreateWidget("domain_ops_rc", xmRowColumnWidgetClass,
				frame,
				XmNchildType, XmFRAME_WORKAREA_CHILD,
				XmNborderWidth,	0,
				XmNverticalSpacing,	2,
				XmNorientation,	XmHORIZONTAL,
				NULL);
    button = XtVaCreateManagedWidget("new_domain", xmPushButtonGadgetClass,
				     frame_rc, NULL);
    XtAddCallback(button, XmNactivateCallback, newDomainCb, NULL);
    button = XtVaCreateManagedWidget("delete_domain", xmPushButtonGadgetClass,
				     frame_rc, NULL);
    XtAddCallback(button, XmNactivateCallback, deleteDomainCb, NULL);
    button = XtVaCreateManagedWidget("clear_domain", xmPushButtonGadgetClass,
				     frame_rc, NULL);
    XtAddCallback(button, XmNactivateCallback, clearDomainCb, NULL);
    button = XtVaCreateManagedWidget("clear_all_domains", xmPushButtonGadgetClass,
				     frame_rc, NULL);
    XtAddCallback(button, XmNactivateCallback, clearAllDomainsCb, NULL);
    XtManageChild(frame_rc);

    XtManageChild( buttonbar );
    return( buttonbar );
}

void init_main_buttonbar(
Widget	topl)
{
  return;
}

