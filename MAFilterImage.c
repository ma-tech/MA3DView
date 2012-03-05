#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAFilterImage_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAFilterImage.c
* \author	Richard Baldock
* \date         November 2001
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
