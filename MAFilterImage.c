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
*   File       :   MAFilterImage.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Thu Nov 22 14:53:39 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MA3DView.h>

static Widget	filter_controls_dialog=NULL;

void filterImageCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  Widget	scroller;

  if( filter_controls_dialog == NULL ){
    filter_controls_dialog = createFilterControls(globals.topl);
  }
  XtManageChild( filter_controls_dialog );
  if( scroller = XtNameToWidget(filter_controls_dialog,
				"*control*PageScroller") ){
    XtUnmanageChild(scroller);
  }
  XtPopup( XtParent(filter_controls_dialog), XtGrabNone );
  
  return;
}

Widget createGaussianPage(
  Widget	notebook,
  int		page)
{
  Widget	page_frame, page_content, page_tab;
  Widget	spin, widget;

  page_frame = XtVaCreateManagedWidget("gaussianFrame", xmFrameWidgetClass,
				       notebook,
				       XmNnotebookChildType, XmPAGE,
				       XmNpageNumber, page,
				       NULL);
  page_content = XtVaCreateManagedWidget("gaussianContent", xmFormWidgetClass,
					 page_frame, NULL);
  page_tab = XtVaCreateManagedWidget("gaussian", xmPushButtonWidgetClass,
				     notebook,
				     XmNnotebookChildType, XmMAJOR_TAB,
				     XmNpageNumber, page,
				     NULL);

  /* now the controls */
  spin = XtVaCreateManagedWidget("dummy", xmLabelWidgetClass, page_content,
				 NULL);

  return page_frame;
}

Widget createMedianPage(
  Widget	notebook,
  int		page)
{
  Widget	page_frame, page_content, page_tab;

  page_frame = XtVaCreateManagedWidget("medianFrame", xmFrameWidgetClass,
				       notebook,
				       XmNnotebookChildType, XmPAGE,
				       XmNpageNumber, page,
				       NULL);
  page_content = XtVaCreateManagedWidget("medianContent", xmLabelWidgetClass,
					 page_frame, NULL);
  page_tab = XtVaCreateManagedWidget("median", xmPushButtonWidgetClass,
				     notebook,
				     XmNnotebookChildType, XmMAJOR_TAB,
				     XmNpageNumber, page,
				     NULL);

  return page_frame;
}

Widget createExtremumPage(
  Widget	notebook,
  int		page)
{
  Widget	page_frame, page_content, page_tab;

  page_frame = XtVaCreateManagedWidget("extremumFrame", xmFrameWidgetClass,
				       notebook,
				       XmNnotebookChildType, XmPAGE,
				       XmNpageNumber, page,
				       NULL);
  page_content = XtVaCreateManagedWidget("extremumContent", xmLabelWidgetClass,
					 page_frame, NULL);
  page_tab = XtVaCreateManagedWidget("extremum", xmPushButtonWidgetClass,
				     notebook,
				     XmNnotebookChildType, XmMAJOR_TAB,
				     XmNpageNumber, page,
				     NULL);

  return page_frame;
}

static ActionAreaItem   filter_dialog_actions[] = {
  {"Test",	NULL,		NULL},
  {"Apply",	NULL,		NULL},
  {"Reset",	NULL,		NULL},
  {"Dismiss",     NULL,           NULL},
  {"Help",        NULL,           NULL},
};

Widget createFilterControls(
  Widget topl)
{
  Widget	dialog, control, widget, frame, title, form, scroller;
  float		min, max, val;
  Visual	*visual;
  Arg		arg[1];

  dialog = HGU_XmCreateStdDialog(topl, "image_controls_dialog",
				 xmNotebookWidgetClass,
				 filter_dialog_actions, 5);

  if( (widget = XtNameToWidget(dialog, "*.Apply")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );
  XtVaSetValues(control,
		XmNbindingType, XmNONE,
		XmNbackPagePlacement, XmTOP_RIGHT,
		XmNorientation, XmVERTICAL,
		XmNbackPageSize, 4,
		NULL);

  /* now add a few pages */
  frame = createGaussianPage(control, 1);
  frame = createMedianPage(control, 2);
  frame = createExtremumPage(control, 3);

  return dialog;
}
