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
*   File       :   main_work_area.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 14:25:51 2001				*
*   $Revision$   							*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>

#include <MA3DView.h>

/* callbacks for 3D view */
static void ThreeDDrawSceneCb(
  Widget	w,
  XtPointer	clientData,
  XtPointer	callData)
{
  MAOpenGLDrawScene(w);

  return;
}

static void ThreeDResizeCb(
  Widget	w,
  XtPointer	clientData,
  XtPointer	callData)
{
  HGUglwCanvasCallbackStruct *glwCbs;

  glwCbs = (HGUglwCanvasCallbackStruct *)callData;
  glViewport(0, 0, (GLint)(glwCbs->width), (GLint)(glwCbs->height));

  return;
}

void ThreeDResetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGUglwCanvasTbReset( globals.canvas3D );
  MAOpenGLDrawScene( globals.canvas3D );

  return;
}

static void ThreeDInputCb(
  Widget	w,
  XtPointer	clientData,
  XtPointer	callData)
{
  char		buffer[1];
  KeySym	keysym;
  HGUglwCanvasCallbackStruct *glwCbs;

  glwCbs = (HGUglwCanvasCallbackStruct *)callData;
  switch(glwCbs->event->type)
  {
  case KeyRelease:
    if(XLookupString((XKeyEvent *)glwCbs->event, buffer, 1,
		     &keysym, NULL) == 1)
    {
      switch(keysym)
      {
      case XK_Escape:
      case XK_Q:
      case XK_q:
      case XK_R:
      case XK_r:
	ThreeDResetCb(w, clientData, callData);
	break;
      }
    }
    break;
  }

  return;
}

/* misc routines */
XImage *HGU_XmObjToXImage(
  Widget	w,
  WlzObject	*obj)
{
  XImage		*rtnImage=NULL;
  XWindowAttributes	win_att;
  Dimension		width, height;
  UBYTE			*data, *dst_data;
  WlzGreyValueWSpace	*gVWSp = NULL;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  int			i, j;

  /* get window properties */
  if( XGetWindowAttributes(XtDisplayOfObject(w), XtWindowOfObject(w), &win_att) == 0 ){
    return rtnImage;
  }

  /* check the object */
  if( !obj ){
    return rtnImage;
  }
  if( obj->type != WLZ_2D_DOMAINOBJ ){
    return rtnImage;
  }
  if( !obj->domain.core || !obj->values.core ){
    return rtnImage;
  }
  /* allocate space for the data */
  width = obj->domain.i->lastkl - obj->domain.i->kol1 + 1;
  height = obj->domain.i->lastln - obj->domain.i->line1 + 1;
  if( gVWSp = WlzGreyValueMakeWSp(obj, &errNum) ){
    if( data = (UBYTE *) AlcMalloc(((win_att.depth == 8)?1:4)
				   *width*height*sizeof(char)) ){
      dst_data = data;
      /* fill in the values */
      for(j=0; j < height; j++){
	for(i=0; i < width; i++, data++){
	  WlzGreyValueGet(gVWSp, 0, j + obj->domain.i->line1,
			  i + obj->domain.i->kol1);
	  switch( gVWSp->gType ){
	  default:
	  case WLZ_GREY_INT:
	    *data = globals.lut[(*(gVWSp->gPtr[0].inp))-globals.srcMin];
	    break;
	  case WLZ_GREY_SHORT:
	    *data = globals.lut[(*(gVWSp->gPtr[0].shp))-globals.srcMin];
	    break;
	  case WLZ_GREY_UBYTE:
	    *data = globals.lut[(*(gVWSp->gPtr[0].ubp))-globals.srcMin];
	    break;
	  case WLZ_GREY_FLOAT:
	    *data = *(gVWSp->gPtr[0].flp);
	    break;
	  case WLZ_GREY_DOUBLE:
	    *data = *(gVWSp->gPtr[0].dbp);
	    break;
	  }
	  if( win_att.depth == 24 ){
	    data[1] = data[0];
	    data[2] = data[0];
	    data[3] = data[0];
	    data += 3;
	  }
	}
      }
    }	  
    else {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    WlzGreyValueFreeWSp(gVWSp);
  }

  if( errNum == WLZ_ERR_NONE ){
    rtnImage = XCreateImage(XtDisplay(w),
			    win_att.visual, win_att.depth,
			    ZPixmap, 0, (char *) dst_data,
			    width, height, 8, 0);
  }

  return rtnImage;
}

/* misc functions & callbacks for section view */
void display_pointer_feedback_information(
  int	x,
  int	y)
{
  WlzGreyValueWSpace	*gVWSp = NULL;
  int			kol, line, plane;
  char			str_buf[256];
  WlzThreeDViewStruct	*wlzViewStr= globals.wlzViewStr;
  int			widthp, heightp;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  if( globals.obj == NULL ){
    return;
  }
  
  x /= globals.magVal;
  y /= globals.magVal;
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  if( (x < 0) || (x >= widthp) || (y < 0) || (y >= heightp) )
  {
    return;
  }

  kol = (int) (wlzViewStr->xp_to_x[x] + wlzViewStr->yp_to_x[y]);
  line = (int) (wlzViewStr->xp_to_y[x] + wlzViewStr->yp_to_y[y]);
  plane = (int) (wlzViewStr->xp_to_z[x] + wlzViewStr->yp_to_z[y]);

  /* get the image grey-value */
  str_buf[0] = '\0';
  if( gVWSp = WlzGreyValueMakeWSp(globals.obj, &errNum) ){
    WlzGreyValueGet(gVWSp, (double) plane, (double) line, (double) kol);
    switch(gVWSp->gType){
    case WLZ_GREY_INT:
      sprintf(str_buf, "(%d,%d,%d) %d", kol, line, plane, gVWSp->gVal[0].inv);
      break;
    case WLZ_GREY_SHORT:
      sprintf(str_buf, "(%d,%d,%d) %d", kol, line, plane, gVWSp->gVal[0].shv);
      break;
    case WLZ_GREY_UBYTE:
      sprintf(str_buf, "(%d,%d,%d) %d", kol, line, plane, gVWSp->gVal[0].ubv);
      break;
    case WLZ_GREY_FLOAT:
      sprintf(str_buf, "(%d,%d,%d) %f", kol, line, plane, gVWSp->gVal[0].flv);
      break;
    case WLZ_GREY_DOUBLE:
      sprintf(str_buf, "(%d,%d,%d) %f", kol, line, plane, gVWSp->gVal[0].dbv);
      break;
    }
    WlzGreyValueFreeWSp(gVWSp);
  }

  XtVaSetValues(globals.message,
		XmNvalue, 	str_buf,
		NULL);
  return;
}

void setViewSelection(
  String	selection)
{
  Widget	mode_rc, toggle;
  XmToggleButtonCallbackStruct	cbs;

  /* get the mode row-column */
  if( !(mode_rc = XtNameToWidget(globals.topl, "*work_area*mode_rc")) ){
    return;
  }

  /* unset the currently selected */
  XtVaGetValues(mode_rc, XmNmenuHistory, &toggle, NULL);
  if( toggle ){
    XtVaSetValues(toggle, XmNset, False, NULL);
  }

  /* set selection and row-column history */
  if( strcmp(selection, "y_z") == 0 ){
    toggle = XtNameToWidget(globals.topl, "*work_area*mode_rc.y_z");
  }
  else if( strcmp(selection, "z_x") == 0 ){
    toggle = XtNameToWidget(globals.topl, "*work_area*mode_rc.z_x");
  }
  else if( strcmp(selection, "any") == 0 ){
    toggle = XtNameToWidget(globals.topl, "*work_area*mode_rc.any");
  }
  else {
    toggle = XtNameToWidget(globals.topl, "*work_area*mode_rc.x_y");
  }
  if( toggle ){
    XtVaSetValues(toggle, XmNset, True, NULL);
  }
  XtVaSetValues(mode_rc, XmNmenuHistory, toggle, NULL);


  /* call callbacks */
  cbs.reason = XmCR_VALUE_CHANGED;
  cbs.event = NULL;
  cbs.set = True;
  XtCallCallbacks(toggle, XmNvalueChangedCallback, &cbs);


  return;
}

void setViewCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;
  WlzThreeDViewStruct	*wlzViewStr;
  WlzDVertex3	fixed;
  Dimension	widthp, heightp;
  float		new_dist;

  if( cbs->set == False ){
    return;
  }

  /* re-initialise the view structure */
  if( globals.wlzViewStr == NULL ){
    globals.wlzViewStr = WlzMake3DViewStruct(WLZ_3D_VIEW_STRUCT, NULL);
  }
  wlzViewStr = globals.wlzViewStr;
  fixed.vtX = 0.0;
  fixed.vtY = 0.0;
  fixed.vtZ = 0.0;
  wlzViewStr->initialised = 0;
  wlzViewStr->scale       = 1.0;
  wlzViewStr->fixed       = fixed;
  wlzViewStr->dist        = 0.0;
  wlzViewStr->up.vtX	  = 0.0;
  wlzViewStr->up.vtY	  = 0.0;
  wlzViewStr->up.vtZ	  = -1.0;
  wlzViewStr->view_mode   = WLZ_UP_IS_UP_MODE;
  switch( (int) client_data ){
  default:
  case 0:
    wlzViewStr->theta = 0.0;
    wlzViewStr->phi = 0.0;
    break;
  case 1:
    wlzViewStr->theta = 0.0;
    wlzViewStr->phi = WLZ_M_PI/2;
    break;
  case 2:
    wlzViewStr->theta = WLZ_M_PI/2;
    wlzViewStr->phi = WLZ_M_PI/2;
    break;
  case 3: /* fake for now - should be arbitrary*/
    wlzViewStr->theta = WLZ_M_PI/4;
    wlzViewStr->phi = WLZ_M_PI/4;
    break;
  }
  WlzInit3DViewStruct(wlzViewStr, globals.obj);
  new_dist = (wlzViewStr->minvals.vtZ + wlzViewStr->maxvals.vtZ) / 2.0;
  Wlz3DSectionIncrementDistance(wlzViewStr, new_dist);
  wlzViewStr->dist = new_dist;
  /* set the distance slider */
  HGU_XmSetSliderRange(globals.distSlider, wlzViewStr->minvals.vtZ,
		       wlzViewStr->maxvals.vtZ);
  HGU_XmSetSliderValue(globals.distSlider, wlzViewStr->dist);

  /* re-size the canvas */
  widthp  = wlzViewStr->maxvals.vtX - wlzViewStr->minvals.vtX + 1;
  heightp = wlzViewStr->maxvals.vtY - wlzViewStr->minvals.vtY + 1;
  widthp *= globals.magVal;
  heightp *= globals.magVal;
  XtVaSetValues(globals.canvas,
		XmNwidth, widthp,
		XmNheight, heightp,
		NULL);

  /* reset the ximage */
  if(globals.ximage){
    AlcFree(globals.ximage->data);
    globals.ximage->data = NULL;
    XDestroyImage(globals.ximage);
    globals.ximage = NULL;
  }

  /* clear domain boundaries */
  clearDomainBoundaries();
  
  /* call callbacks */
  distanceCb(globals.distSlider, NULL, NULL);
  
  return;
}
void canvas_input_cb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;

  /* switch on event type */
  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
      break;

    case Button2:
      break;

    case Button3:		/* unused */
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

    default:
      break;

    }
    break;

  case MotionNotify: /* always display feedback on position and domain */
    x = cbs->event->xmotion.x;
    y = cbs->event->xmotion.y;
    display_pointer_feedback_information(x, y);
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
 
void CanvasMotionEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct cbs;

  /* call the canvas input callbacks */
  cbs.reason = XmCR_INPUT;
  cbs.event = event;
  cbs.window = XtWindow(w);
  XtCallCallbacks(w, XmNinputCallback, (XtPointer) &cbs);

  return;
}

void setViewMag(
  double		mag,
  int			fixedX,
  int			fixedY)
{
  Widget		menu, widget, scrolledWindow, xScrollBar, yScrollBar;
  int			oldXSize, oldXMin, oldXMax, oldXVal;
  int			oldYSize, oldYMin, oldYMax, oldYVal;
  int			newSize, newMin, newMax, newVal;
  int			increment, pageIncrement;
  int			xp;
  double		a, b;
  Dimension		width, height, widthp, heightp;

  if( globals.obj == NULL ){
    return;
  }

  /* get original scrollbar values */
  scrolledWindow = XtParent(globals.canvas);
  while( !XtIsSubclass(scrolledWindow, xmScrolledWindowWidgetClass) ){
    scrolledWindow = XtParent(scrolledWindow);
  }
  XtVaGetValues(scrolledWindow,
		XmNhorizontalScrollBar, &xScrollBar,
		XmNverticalScrollBar, &yScrollBar,
		NULL);
  XtVaGetValues(xScrollBar,
		XmNsliderSize, &oldXSize,
		XmNminimum, &oldXMin,
		XmNmaximum, &oldXMax,
		XmNvalue, &oldXVal,
		NULL);
  XtVaGetValues(yScrollBar,
		XmNsliderSize, &oldYSize,
		XmNminimum, &oldYMin,
		XmNmaximum, &oldYMax,
		XmNvalue, &oldYVal,
		NULL);
  XtVaGetValues(globals.canvas,
		XmNwidth, &width,
		XmNheight, &height,
		NULL);

  /* test if the fixed point is to be set */
  if( fixedX < 0 ){
    fixedX = (oldXVal + oldXSize/2) * width / (oldXMax - oldXMin);
  }
  if( fixedY < 0 ){
    fixedY = (oldYVal + oldYSize/2) * height / (oldYMax - oldYMin);
  }

  /* reset the scale */
  widthp = (globals.obj->domain.p->lastkl - globals.obj->domain.p->kol1)
    * globals.magVal + 2 * CANVAS_MARGIN;
  heightp = (globals.obj->domain.p->lastln - globals.obj->domain.p->line1)
    * globals.magVal + 2 * CANVAS_MARGIN;
  XtVaSetValues(globals.canvas,
		XmNwidth, widthp,
		XmNheight, heightp,
		NULL);
  
  /* reset the new sliders */
  XtVaGetValues(xScrollBar,
		XmNsliderSize, &newSize,
		XmNminimum, &newMin,
		XmNmaximum, &newMax,
		XmNincrement, &increment,
		XmNpageIncrement, &pageIncrement,
		NULL);

  a = - ((double) width) / (oldXMax - oldXMin);
  b = -a * oldXMin;
  xp = fixedX + a * oldXVal + b;

  a = - ((double) width) * mag / (newMax - newMin);
  b = -a * newMin;
  newVal = (xp - mag * fixedX - b) / a;

  newVal = WLZ_MIN(newVal, newMax - newSize);
  newVal = WLZ_MAX(newVal, newMin);

  XmScrollBarSetValues(xScrollBar, newVal, newSize, increment,
		       pageIncrement, True);

  XtVaGetValues(yScrollBar,
		XmNsliderSize, &newSize,
		XmNminimum, &newMin,
		XmNmaximum, &newMax,
		XmNincrement, &increment,
		XmNpageIncrement, &pageIncrement,
		NULL);

  a = - ((double) height) / (oldYMax - oldYMin);
  b = -a * oldYMin;
  xp = fixedY + a * oldYVal + b;

  a = - ((double) height) * mag / (newMax - newMin);
  b = -a * newMin;
  newVal = (xp - mag * fixedY - b) / a;

  newVal = WLZ_MIN(newVal, newMax - newSize);
  newVal = WLZ_MAX(newVal, newMin);

  XmScrollBarSetValues(yScrollBar, newVal, newSize, increment,
		       pageIncrement, True);

  return;
}

/* action and callback procedures */
void magCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		magFlg = (int) client_data;
  Dimension	newW, newH;
  double	mag;

  /* reset value then force to prescribed values for max and min limits
     to avoid floating point errors */
  mag = globals.magVal;
  if( magFlg ){
    globals.magVal *= 2;
  }
  else {
    globals.magVal /= 2;
  }
  if( globals.magVal > 15.9 ){
    globals.magVal = 16.0;
  }
  else if( globals.magVal > 7.9 ){
    globals.magVal = 8.0;
  }
  else if( globals.magVal > 3.9 ){
    globals.magVal = 4.0;
  }
  else if( globals.magVal > 1.9 ){
    globals.magVal = 2.0;
  }
  else if( globals.magVal > 0.9 ){
    globals.magVal = 1.0;
  }
  else if( globals.magVal > 0.4 ){
    globals.magVal = 0.5;
  }
  else {
    globals.magVal = 0.25;
  }
  mag = globals.magVal / mag;

  /* resize the canvas, reset the scroll bars and re-display */
  setViewMag(mag, -1, -1);
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}
void canvasExposeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmDrawingAreaCallbackStruct *cbs = 
    (XmDrawingAreaCallbackStruct *) call_data;
  int	xExp, yExp, wExp, hExp;
  int	xImg, yImg, wImg, hImg;
  Dimension	width, height, rW, rH;
  XWindowAttributes	win_att;

  /* check if there is a reference object */
  if((globals.obj == NULL) || (globals.view_object == NULL) ||
     (globals.ximage == NULL)){
    return;
  }

  /* get the exposed region and clear it */
  XtVaGetValues(globals.canvas, XmNwidth, &width,
		XmNheight, &height, NULL);
  if( (cbs == NULL) || (cbs->event == NULL) || (cbs->event->type != Expose) ){
    Widget	scrolled_window, x_bar, y_bar;
    int 	minimum, maximum, value, size;
    
    /* the whole visible region */
    scrolled_window = XtParent(globals.canvas);
    while( !XtIsSubclass(scrolled_window, xmScrolledWindowWidgetClass) ){
      scrolled_window = XtParent(scrolled_window);
    }

    /* get the scrollbars */
    XtVaGetValues(scrolled_window,
		  XmNhorizontalScrollBar, &x_bar,
		  XmNverticalScrollBar, &y_bar,
		  NULL);

    /* set x expose region */
    XtVaGetValues(x_bar,
		  XmNminimum, &minimum,
		  XmNmaximum, &maximum,
		  XmNvalue,   &value,
		  XmNsliderSize, &size, NULL);
    xExp = value * width / (maximum - minimum);
    wExp = size * width / (maximum - minimum);
      
    /* set y expose region */
    XtVaGetValues(y_bar,
		  XmNminimum, &minimum,
		  XmNmaximum, &maximum,
		  XmNvalue,   &value,
		  XmNsliderSize, &size, NULL);
    yExp = value * height / (maximum - minimum);
    hExp = size * height / (maximum - minimum);
  }
  else {
    xExp = cbs->event->xexpose.x;
    yExp = cbs->event->xexpose.y;
    wExp = cbs->event->xexpose.width;
    hExp = cbs->event->xexpose.height;
  }

  /* find matching region in the ximage */
  xImg = (xExp) / globals.magVal;
  yImg = (yExp) / globals.magVal;
  wImg = xImg + wExp / globals.magVal + 2;
  hImg = yImg + hExp / globals.magVal + 2;

  if( wImg < 0 ){
    return;
  }
  else if( xImg > globals.ximage->width ){
    return;
  }
  else if( xImg < 0 ){
    xImg = 0;
  }
  if( wImg > globals.ximage->width ){
    wImg = globals.ximage->width - xImg + 1;
  }
  else {
    wImg = wImg - xImg + 1;
  }

  if( hImg < 0 ){
    return;
  }
  else if( yImg > globals.ximage->height ){
    return;
  }
  else if( yImg < 0 ){
    yImg = 0;
  }
  if( hImg > globals.ximage->height ){
    hImg = globals.ximage->height - yImg + 1;
  }
  else {
    hImg = hImg - yImg + 1;
  }

  xExp = (xImg) * globals.magVal;
  yExp = (yImg) * globals.magVal;

  /* display the image */
  if( globals.magVal > 1.0 ){
    /* create a temporary ximage  - assume 24 bit */
    unsigned int	*data, *orig_data, *toPtr, *fromPtr;
    int		intMag = WLZ_NINT(globals.magVal);
    int		i, j, k;
    XImage	*tmpImage;

    data = (unsigned int *) AlcMalloc(sizeof(unsigned int)
				      * wImg * hImg * intMag * intMag);
    toPtr = data;
    orig_data = (unsigned int *) globals.ximage->data;
    for(j=0; j < hImg; j++){
      fromPtr = orig_data + (j + yImg) * globals.ximage->width + xImg;
      for(i=0; i < wImg; i++, fromPtr++){
	for(k=0; k < intMag; k++, toPtr++){
	  *toPtr = *fromPtr;
	}
      }
      for(k=1; k < intMag; k++){
	memcpy(toPtr, toPtr - wImg*intMag, sizeof(int)*wImg*intMag);
	toPtr += wImg*intMag;
      }
    }
    if( XGetWindowAttributes(globals.dpy, globals.win, &win_att) ){
      tmpImage = XCreateImage(globals.dpy,
			      win_att.visual, win_att.depth,
			      ZPixmap, 0, (char *) data,
			      wImg*intMag, hImg*intMag, 8, 0);
      XPutImage(globals.dpy, globals.win, globals.gc_greys, tmpImage,
		0, 0, xExp, yExp, wImg*intMag, hImg*intMag);
      AlcFree(data);
      tmpImage->data = NULL;
      XDestroyImage(tmpImage);
    }
  }
  else if( globals.magVal < 1.0 ){
    /* create a temporary ximage  - assume 24 bit */
    unsigned int	*data, *orig_data, *toPtr, *fromPtr;
    int		intMag = WLZ_NINT(1.0/globals.magVal);
    int		i, j, k, w, h;
    XImage	*tmpImage;

    w = wImg / intMag;
    h = hImg / intMag;
    data = (unsigned int *) AlcMalloc(sizeof(unsigned int) * w *h);
    toPtr = data;
    orig_data = (unsigned int *) globals.ximage->data;
    for(j=0; j < h; j++){
      fromPtr = orig_data + (j*intMag + yImg) * globals.ximage->width + xImg;
      for(i=0; i < w; i++, toPtr++, fromPtr += intMag){
	*toPtr = *fromPtr;
      }
    }
    if( XGetWindowAttributes(globals.dpy, globals.win, &win_att) ){
      tmpImage = XCreateImage(globals.dpy,
			      win_att.visual, win_att.depth,
			      ZPixmap, 0, (char *) data,
			      w, h, 8, 0);
      XPutImage(globals.dpy, globals.win, globals.gc_greys, tmpImage,
		0, 0, xExp, yExp, w, h);
      AlcFree(data);
      tmpImage->data = NULL;
      XDestroyImage(tmpImage);
    }
  }
  else {
    XPutImage(globals.dpy, globals.win, globals.gc_greys, globals.ximage,
	      xImg, yImg, xExp, yExp, wImg, hImg);
  }

  /* display the domains */
  displayDomains();

  /* display the crop rectangle */
  displayCropRectangle();
  
  return;
}

void distanceCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	slider = w;
  float		new_dist;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  WlzObject	*obj;

  /* check there is a reference object */
  if( globals.obj == NULL ){
    return;
  }

  if( !globals.wlzViewStr->initialised ){
    return;
  }

  while( strcmp(XtName(slider), "distance_slider") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }
  new_dist = HGU_XmGetSliderValue( slider );
  Wlz3DSectionIncrementDistance(globals.wlzViewStr,
				(new_dist - globals.wlzViewStr->dist));
  globals.wlzViewStr->dist = new_dist;

  /* reset the display object */
  if( globals.view_object ){
    WlzFreeObj(globals.view_object);
    globals.view_object = NULL;
  }
  if( globals.obj2D ){
    obj = WlzGetSectionFromObject(globals.obj, globals.wlzViewStr,
				  WLZ_INTERPOLATION_NEAREST, &errNum);
  }
  else {
    obj = WlzGetSectionFromObject(globals.obj, globals.wlzViewStr,
				  WLZ_INTERPOLATION_NEAREST, &errNum);
  }
  if( errNum == WLZ_ERR_NONE ){
    globals.view_object = WlzAssignObject(obj, NULL);
  }
  else {
    fprintf(stderr, "distanceCb: error in section object\n");
    return;
  }

  /* reset the ximage */
  if( globals.ximage ){
    AlcFree(globals.ximage->data);
    globals.ximage->data = NULL;
    XDestroyImage(globals.ximage);
    globals.ximage = NULL;    
  }

  globals.ximage = HGU_XmObjToXImage(globals.canvas, obj);

  /* clear domain boundaries */
  clearDomainBoundaries();
  
  /* calculate the expose event and call expose callback */
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}
    

Widget create_main_work_area(
Widget	main_w)
{
  Widget	frame, scrolled_window, work_area, view_form, label;
  Widget	button, slider, scale, canvas, mode_rc, list;
  Widget	paned;
  XmString	dmnStr;
  Pixel		bg;
  XVisualInfo	*visualInfo, visualTemplate;
  int		nitems;

  /* the work area is a form enclosing some button controls plus a scrolled
     window and the distance slider */
  work_area = XtVaCreateWidget("work_area", xmFormWidgetClass, main_w,
			       NULL);

  /* paned window for the 3D feedback view and domain list */
  paned = XtVaCreateManagedWidget("feedback_domain_paned",
				  xmPanedWindowWidgetClass, work_area,
				  XmNtopAttachment,   XmATTACH_FORM,
				  XmNbottomAttachment,   XmATTACH_FORM,
				  XmNleftAttachment,   XmATTACH_FORM,
				  XmNrightAttachment,	XmATTACH_POSITION,
				  XmNrightPosition,	25,
				  NULL);

  /* frame for a 3D view window */
  frame = XtVaCreateManagedWidget("feedback_frame", xmFrameWidgetClass,
				  paned, NULL);
  label = XtVaCreateManagedWidget("3D view", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  bg = BlackPixelOfScreen(XtScreen(main_w));
  visualTemplate.visualid = globals.toplVisual->visualid;
  visualInfo = XGetVisualInfo(XtDisplay(main_w), VisualIDMask,
			      &visualTemplate, &nitems);
  canvas = XtVaCreateManagedWidget("3Dcanvas",
				   hguGLwCanvasTbWidgetClass,
				   frame,
				   XmNbackground,		bg,
				   HGUglwNtrackballSize,	512,
				   HGUglwNvisualInfo,	visualInfo,
				   XtNvisual,	globals.toplVisual,
				   XtNcolormap,	globals.cmap,
				   XtNdepth,		globals.toplDepth,
				   HGUglwNrgba,	True,
				   NULL);
  XtAddCallback(canvas, HGUglwNinitCallback,     MAOpenGLInitCb, NULL);
  XtAddCallback(canvas, HGUglwNresizeCallback,   ThreeDResizeCb, NULL);
  XtAddCallback(canvas, HGUglwNexposeCallback,   ThreeDDrawSceneCb,
		NULL);
  XtAddCallback(canvas, HGUglwNinputCallback,    ThreeDInputCb, NULL);
  XtAddCallback(canvas, HGUglwNtrackballCallback, ThreeDDrawSceneCb,
		NULL);
  globals.canvas3D = canvas;


  /* now a frame for the domain list */
  frame = XtVaCreateManagedWidget("domain_list_frame", xmFrameWidgetClass,
				  paned, NULL);

  list = XmCreateScrolledList(frame, "domain_list", NULL, 0);
  /*XtVaSetValues(XtParent(list),
		XmNtopAttachment,   	XmATTACH_FORM,
		XmNbottomAttachment,  	XmATTACH_FORM,
		XmNleftAttachment,   	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	25,
		NULL);*/
		
  dmnStr = XmStringCreateLocalized("No selection");
  XmListAddItem(list, dmnStr, 0);
  globals.dmnListWidget = list;
  XmStringFree(dmnStr);
  XtManageChild(list);
  XtAddCallback(list, XmNbrowseSelectionCallback, domainListCb, NULL);

  label = XtVaCreateManagedWidget("domain_list_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);

  /* now a frame for the scrolled window for the canvas plus slider */
  frame = XtVaCreateManagedWidget("view_frame", xmFrameWidgetClass, work_area,
				  XmNtopAttachment,   XmATTACH_FORM,
				  XmNbottomAttachment,   XmATTACH_FORM,
				  XmNleftAttachment,   XmATTACH_POSITION,
				  XmNleftPosition,	26,
				  XmNrightAttachment,   XmATTACH_FORM,
				  NULL);
  view_form = XtVaCreateManagedWidget("view_form", xmFormWidgetClass, frame,
				      XmNchildType, XmFRAME_WORKAREA_CHILD,
				      NULL);
  label = XtVaCreateManagedWidget("view_frame_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);

  /* at the bottom a distance slider */
  slider = HGU_XmCreateHorizontalSlider("distance_slider", view_form,
					0.0, -1000.0, 1000.0, 0,
					distanceCb, NULL);
  XtVaSetValues(slider,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, distanceCb, NULL);

  globals.distSlider = slider;

   /* mag buttons */
  button = XtVaCreateManagedWidget("b3", xmPushButtonGadgetClass, view_form,
				   XmNtopAttachment,  XmATTACH_FORM,
				   XmNleftAttachment,   XmATTACH_FORM,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, magCb, (XtPointer) 1);
  button = XtVaCreateManagedWidget("b4", xmPushButtonGadgetClass, view_form,
				   XmNtopAttachment,  XmATTACH_FORM,
				   XmNleftAttachment,   XmATTACH_WIDGET,
				   XmNleftWidget,	button,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, magCb, (XtPointer) 0);

  /* mode row-column and mode buttons */
  mode_rc = XtVaCreateManagedWidget("mode_rc", xmRowColumnWidgetClass,
				    view_form,
				    XmNtopAttachment,  XmATTACH_FORM,
				    XmNrightAttachment,  XmATTACH_FORM,
				    XmNorientation,	XmHORIZONTAL,
				    XmNradioBehavior,	True,
				    XmNpacking,		XmPACK_TIGHT,
				    XmNborderWidth,	1,
				    NULL);
  button = XtVaCreateManagedWidget("view_mode", xmLabelGadgetClass,
				   mode_rc, NULL);
  button = XtVaCreateManagedWidget("x_y", xmToggleButtonGadgetClass,
				   mode_rc, XmNset, True, NULL);
  XtAddCallback(button, XmNvalueChangedCallback, setViewCb, (XtPointer) 0);
  button = XtVaCreateManagedWidget("y_z", xmToggleButtonGadgetClass,
				   mode_rc, XmNset, False, NULL);
  XtAddCallback(button, XmNvalueChangedCallback, setViewCb, (XtPointer) 1);
  button = XtVaCreateManagedWidget("z_x", xmToggleButtonGadgetClass,
				   mode_rc, XmNset, False, NULL);
  XtAddCallback(button, XmNvalueChangedCallback, setViewCb, (XtPointer) 2);
  button = XtVaCreateManagedWidget("any", xmToggleButtonGadgetClass,
				   mode_rc, XmNset, False, NULL);
  XtAddCallback(button, XmNvalueChangedCallback, setViewCb, (XtPointer) 3);

  /* scrolled window and canvas */
  scrolled_window =
    XtVaCreateManagedWidget("scrolled_window",
			    xmScrolledWindowWidgetClass, view_form,
			    XmNscrollingPolicy, 	XmAUTOMATIC,
			    XmNbottomAttachment,	XmATTACH_WIDGET,
			    XmNbottomWidget,	slider,
			    XmNtopAttachment,	XmATTACH_WIDGET,
			    XmNtopWidget,	mode_rc,
			    XmNleftAttachment,	XmATTACH_FORM,
			    XmNrightAttachment,	XmATTACH_FORM,
			    NULL);

  canvas = XtVaCreateManagedWidget("canvas",
				   xmDrawingAreaWidgetClass,
				   scrolled_window,
				   XmNheight, 512,
				   XmNwidth, 512,
				   NULL);

  globals.canvas = canvas;
  XtAddCallback(canvas, XmNexposeCallback, canvasExposeCb, NULL);
  XtAddCallback(canvas, XmNinputCallback, canvas_input_cb, NULL);
  XtAddEventHandler(canvas, PointerMotionMask,
		    False, CanvasMotionEventHandler, NULL);


  XtManageChild(work_area);
  return work_area;
}


void init_main_work_area(
  Widget	topl)
{
  XGCValues	gc_values;

  /* sort out the display window */
  globals.dpy = XtDisplay( globals.canvas );
  globals.win = XtWindow ( globals.canvas );
  globals.magVal = 1.0;

  /* initialise the display object and ximage */
  /* done in init_file_menu */

  /* create some graphics contexts */
  globals.gc_greys = XCreateGC(globals.dpy, globals.win, 0, &gc_values);

  gc_values.foreground = 0xff;
  gc_values.background = 0;
  globals.gc_red = XCreateGC(globals.dpy, globals.win,
			     GCForeground|GCBackground, &gc_values);
  gc_values.foreground = 0xff00;
  gc_values.background = 0;
  globals.gc_green = XCreateGC(globals.dpy, globals.win,
			     GCForeground|GCBackground, &gc_values);
  gc_values.foreground = 0xff0000;
  gc_values.background = 0;
  globals.gc_blue = XCreateGC(globals.dpy, globals.win,
			     GCForeground|GCBackground, &gc_values);
  gc_values.foreground = 0xffffff;
  gc_values.background = 0;
  globals.gc_set = XCreateGC(globals.dpy, globals.win,
			     GCForeground|GCBackground, &gc_values);

  return;
}
