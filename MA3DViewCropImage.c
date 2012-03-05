#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MA3DViewCropImage_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MA3DViewCropImage.c
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

static 	int 		cropFlg=0;
static 	WlzDVertex3	rtnVtxs[12];
static	int		nVtxs;

int getSelectedVtx(
  int x,
  int y)
{
  return -1;
}

void displayBlob(
  int x,
  int y)
{
  x -= globals.view_object->domain.i->kol1;
  y -= globals.view_object->domain.i->line1;
  x *= globals.magVal;
  y *= globals.magVal;
  XFillRectangle(globals.dpy, globals.win, globals.gc_red, x-3, y-3, 7, 7);
  return;
}

void displayLine(
  int x1,
  int y1,
  int x2,
  int y2)
{
  x1 -= globals.view_object->domain.i->kol1;
  y1 -= globals.view_object->domain.i->line1;
  x2 -= globals.view_object->domain.i->kol1;
  y2 -= globals.view_object->domain.i->line1;
  x1 *= globals.magVal;
  y1 *= globals.magVal;
  x2 *= globals.magVal;
  y2 *= globals.magVal;
  XDrawLine(globals.dpy, globals.win, globals.gc_red, x1, y1, x2, y2);
  return;
}

void displayBlobsAndLines(void)
{
  int 	i;

  for(i=0; i < nVtxs; i++){
    displayBlob((int) rtnVtxs[i].vtX, (int) rtnVtxs[i].vtY);
    displayLine((int) rtnVtxs[i].vtX, (int) rtnVtxs[i].vtY,
		(int) rtnVtxs[(i+1)%nVtxs].vtX,
		(int) rtnVtxs[(i+1)%nVtxs].vtY);
  }
  XFlush(globals.dpy);
  return;
}


void displayCropRectangle(void)
{
  int		i;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( cropFlg ){
    /* find points of intersection of crop box with plane */
    nVtxs = Wlz3DViewGetGivenBBIntersection(globals.wlzViewStr,
					    globals.cropBBMin,
					    globals.cropBBMax,
					    &(rtnVtxs[0]), &errNum);
    if( nVtxs == 0 ){
      return;
    }

    /* convert to screen coordinates */
    for(i=0; i < nVtxs; i++){
      Wlz3DSectionTransformVtx(&(rtnVtxs[i]), globals.wlzViewStr);
    }

    /* display blobs and lines */
    displayBlobsAndLines();
  }

  return;
}

void cropCanvasInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;
  int			quitTrigger;
  static int		selVtx;

  /* switch on event type */
  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
      break;

    case Button2:
      break;

    case Button3:		/* unused */
      quitTrigger = 1;
      break;

    default:
      break;

    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){

    case Button1:
      break;

    case Button2:
      break;

    case Button3:
      if(quitTrigger && 
	 (cbs->event->xbutton.window == globals.win)){
	cropFlg = 0;
	XtRemoveCallback(globals.canvas, XmNinputCallback,
			 cropCanvasInputCb, NULL);
	XtAddCallback(globals.canvas, XmNinputCallback,
		      canvas_input_cb, NULL);
	canvasExposeCb(globals.canvas, NULL, NULL);
      }
      quitTrigger = 0;
      break;

    default:
      break;

    }
    break;

  case MotionNotify: /* always display feedback on position and domain */
    x = cbs->event->xmotion.x;
    y = cbs->event->xmotion.y;
    display_pointer_feedback_information(x, y);
    if( (selVtx = getSelectedVtx(x, y)) > -1 ){
      
    }
    break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      break;

    case XK_Down:
    case XK_n:
      break;

    case XK_w:
      /* get a filename for the view image */
      break;

    }
    break;

  default:
    break;
  }

  return;
}

static Widget cropDialog=NULL;

void setCropBoundsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  /* set the value for each slider in turn */
  if( slider = XtNameToWidget(cropDialog, "*.cropBBMinX") ){
    HGU_XmSetSliderValue(slider, (float) globals.cropBBMin.vtX);
  }
  if( slider = XtNameToWidget(cropDialog, "*.cropBBMinY") ){
    HGU_XmSetSliderValue(slider, (float) globals.cropBBMin.vtY);
  }
  if( slider = XtNameToWidget(cropDialog, "*.cropBBMinZ") ){
    HGU_XmSetSliderValue(slider, (float) globals.cropBBMin.vtZ);
  }
  if( slider = XtNameToWidget(cropDialog, "*.cropBBMaxX") ){
    HGU_XmSetSliderValue(slider, (float) globals.cropBBMax.vtX);
  }
  if( slider = XtNameToWidget(cropDialog, "*.cropBBMaxY") ){
    HGU_XmSetSliderValue(slider, (float) globals.cropBBMax.vtY);
  }
  if( slider = XtNameToWidget(cropDialog, "*.cropBBMaxZ") ){
    HGU_XmSetSliderValue(slider, (float) globals.cropBBMax.vtZ);
  }
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

void applyCropBoundsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzObject	*obj;
  WlzIBox3	clipBB;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  /* crop the image and install new */
  clipBB.xMin = globals.cropBBMin.vtX;
  clipBB.yMin = globals.cropBBMin.vtY;
  clipBB.zMin = globals.cropBBMin.vtZ;
  clipBB.xMax = globals.cropBBMax.vtX;
  clipBB.yMax = globals.cropBBMax.vtY;
  clipBB.zMax = globals.cropBBMax.vtZ;
  obj = WlzAssignObject(WlzClipObjToBox3D(globals.obj, clipBB, &errNum),
			NULL);
  install_reference_object(obj);
  WlzFreeObj(obj);

  cropFlg = 0;

  return;
}

void dismissCropBoundsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }
  cropFlg = 0;

  return;
}

void resetCropBoundsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }
  globals.cropBBMin.vtX = globals.obj->domain.p->kol1;
  globals.cropBBMin.vtY = globals.obj->domain.p->line1;
  globals.cropBBMin.vtZ = globals.obj->domain.p->plane1;
  globals.cropBBMax.vtX = globals.obj->domain.p->lastkl;
  globals.cropBBMax.vtY = globals.obj->domain.p->lastln;
  globals.cropBBMax.vtZ = globals.obj->domain.p->lastpl;
  setCropBoundsCb(cropDialog, NULL, NULL);

  return;
}

void cropBBMinXCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  while( strcmp(XtName(slider), "cropBBMinX") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval > globals.cropBBMax.vtX ){
    newval = globals.cropBBMax.vtX;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.cropBBMin.vtX = newval;
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

void cropBBMinYCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  while( strcmp(XtName(slider), "cropBBMinY") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval > globals.cropBBMax.vtY ){
    newval = globals.cropBBMax.vtY;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.cropBBMin.vtY = newval;
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

void cropBBMinZCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  while( strcmp(XtName(slider), "cropBBMinZ") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval > globals.cropBBMax.vtZ ){
    newval = globals.cropBBMax.vtZ;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.cropBBMin.vtZ = newval;
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

void cropBBMaxXCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  while( strcmp(XtName(slider), "cropBBMaxX") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval < globals.cropBBMin.vtX ){
    newval = globals.cropBBMin.vtX;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.cropBBMax.vtX = newval;
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

void cropBBMaxYCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  while( strcmp(XtName(slider), "cropBBMaxY") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval < globals.cropBBMin.vtY ){
    newval = globals.cropBBMin.vtY;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.cropBBMax.vtY = newval;
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

void cropBBMaxZCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  if((globals.obj == NULL) || (cropDialog == NULL)){
    return;
  }

  while( strcmp(XtName(slider), "cropBBMaxZ") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval < globals.cropBBMin.vtZ ){
    newval = globals.cropBBMin.vtZ;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.cropBBMax.vtZ = newval;
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

static ActionAreaItem   crop_image_dialog_actions[] = {
  {"Apply",	NULL,		NULL},
  {"Reset",	NULL,		NULL},
  {"Dismiss",     NULL,           NULL},
  {"Help",        NULL,           NULL},
};

Widget createCropDialog(
  Widget	topl)
{
  Widget	dialog, control, widget, frame, title, form, slider, scale;
  float		min, max, val;
  Visual	*visual;
  Arg		arg[1];

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

  dialog = HGU_XmCreateStdDialog(topl, "image_controls_dialog",
				 xmFormWidgetClass,
				 crop_image_dialog_actions, 4);

  if( (widget = XtNameToWidget(dialog, "*.Apply")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, applyCropBoundsCb, NULL);
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  if( (widget = XtNameToWidget(dialog, "*.Reset")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, resetCropBoundsCb, NULL);
  }

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, dismissCropBoundsCb, NULL);
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );

  /* frame for the transform parameters */
  frame = XtVaCreateManagedWidget("crop_controls_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNbottomAttachment, XmATTACH_FORM,
				  NULL);
  
  title = XtVaCreateManagedWidget("crop_controls_frame_title",
				  xmLabelWidgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("crop_controls_form", xmFormWidgetClass,
				 frame, NULL);

  /* now the sliders */
  min = globals.obj->domain.p->kol1;
  max = globals.obj->domain.p->lastkl;
  val = globals.cropBBMin.vtX;
  slider = HGU_XmCreateHorizontalSlider("cropBBMinX", form, val, min, max, 0,
					cropBBMinXCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, cropBBMinXCb, NULL);
  widget = slider;

  min = globals.obj->domain.p->line1;
  max = globals.obj->domain.p->lastln;
  val = globals.cropBBMin.vtY;
  slider = HGU_XmCreateHorizontalSlider("cropBBMinY", form, val, min, max, 0,
					cropBBMinYCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widget,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, cropBBMinYCb, NULL);
  widget = slider;

  min = globals.obj->domain.p->plane1;
  max = globals.obj->domain.p->lastpl;
  val = globals.cropBBMin.vtZ;
  slider = HGU_XmCreateHorizontalSlider("cropBBMinZ", form, val, min, max, 0,
					cropBBMinZCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widget,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, cropBBMinZCb, NULL);
  widget = slider;

  min = globals.obj->domain.p->kol1;
  max = globals.obj->domain.p->lastkl;
  val = globals.cropBBMax.vtX;
  slider = HGU_XmCreateHorizontalSlider("cropBBMaxX", form, val, min, max, 0,
					cropBBMaxXCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widget,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, cropBBMaxXCb, NULL);
  widget = slider;

  min = globals.obj->domain.p->line1;
  max = globals.obj->domain.p->lastln;
  val = globals.cropBBMax.vtY;
  slider = HGU_XmCreateHorizontalSlider("cropBBMaxY", form, val, min, max, 0,
					cropBBMaxYCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widget,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, cropBBMaxYCb, NULL);
  widget = slider;

  min = globals.obj->domain.p->plane1;
  max = globals.obj->domain.p->lastpl;
  val = globals.cropBBMax.vtZ;
  slider = HGU_XmCreateHorizontalSlider("cropBBMaxZ", form, val, min, max, 0,
					cropBBMaxZCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, widget,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, cropBBMaxZCb, NULL);
  widget = slider;

  
  return dialog;
}
 
void cropImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  if( globals.obj == NULL ){
    return;
  }

  /* set the initial crop bounding box */
  globals.cropBBMin.vtX = globals.obj->domain.p->kol1;
  globals.cropBBMin.vtY = globals.obj->domain.p->line1;
  globals.cropBBMin.vtZ = globals.obj->domain.p->plane1;
  globals.cropBBMax.vtX = globals.obj->domain.p->lastkl;
  globals.cropBBMax.vtY = globals.obj->domain.p->lastln;
  globals.cropBBMax.vtZ = globals.obj->domain.p->lastpl;
  cropFlg = 1;

  /* put up the dialog */
  if( cropDialog == NULL ){
    cropDialog = createCropDialog(globals.topl);
  }
  setCropBoundsCb(cropDialog, NULL, NULL);
  XtManageChild( cropDialog );
  XtPopup( XtParent(cropDialog), XtGrabNone );

  displayCropRectangle();

  return;
}
