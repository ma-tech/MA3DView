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
*   File       :   main_buttonbar.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 14:24:56 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

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

