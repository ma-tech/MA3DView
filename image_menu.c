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
*   File       :   image_menu.c						*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Nov  5 10:33:23 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <MA3DView.h>

/* menu item structure */
static MenuItem image_menu_itemsP[] = {		/* file_menu items */
  {"grey_mapping", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   imageControlsCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"grey_profile", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"grey_stats", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   imageGreyStatsCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"remap_image", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   remapImageCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"crop_image", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   cropImageCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"filter_image", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   filterImageCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

MenuItem	*image_menu_items = &(image_menu_itemsP[0]);
Widget		image_controls_dialog=NULL;

/* image procedures */
void setLutTransform(void)
{
  int		i, gi;
  double	g, A, B, fmin, fmax, mu, sig;

  /* calculate the transform */
  switch( globals.transType ){
  default:
  case WLZ_GREYTRANSFORMTYPE_LINEAR:
    for(i=0, gi=globals.srcMin; gi <= globals.srcSliderMin; i++, gi++){
      globals.lut[i] = globals.dstSliderMin;
    }
    A = ((double) (globals.dstSliderMax - globals.dstSliderMin)) /
      globals.srcSliderWidth;
    B = globals.dstSliderMin - A*globals.srcSliderMin;
    for(; gi <= globals.srcSliderMin + globals.srcSliderWidth; i++, gi++){
      globals.lut[i] = (int) (A*gi + B);
    }
    for(; gi < globals.srcMax; i++, gi++){
      globals.lut[i] = globals.dstSliderMax;
    }
    break;

  case WLZ_GREYTRANSFORMTYPE_GAMMA:
    for(i=0, gi=globals.srcMin; gi <= globals.srcSliderMin; i++, gi++){
      globals.lut[i] = globals.dstSliderMin;
    }
    A = (globals.dstSliderMax - globals.dstSliderMin) / 
      pow(globals.srcSliderWidth, globals.gamma);
    B = globals.dstSliderMin;
    for(; gi <= globals.srcSliderMin + globals.srcSliderWidth; i++, gi++){
      g = gi;
      globals.lut[i] = (int) (A * pow(g - globals.srcSliderMin, globals.gamma) +
			      B);
    }
    for(; gi < globals.srcMax; i++, gi++){
      globals.lut[i] = globals.dstSliderMax;
    }
    break;

  case WLZ_GREYTRANSFORMTYPE_SIGMOID:
    for(i=0, gi=globals.srcMin; gi <= globals.srcSliderMin; i++, gi++){
      globals.lut[i] = globals.dstSliderMin;
    }
    mu = globals.mean + globals.srcSliderMin;
    sig = globals.sigma;
    fmin = 1.0/(1.0 + exp(-(globals.srcSliderMin - mu)/sig));
    fmax = 1.0/(1.0 + exp(-(globals.srcSliderMin + globals.srcSliderWidth -
			    mu)/sig));
    A = (globals.dstSliderMax - globals.dstSliderMin) / (fmax - fmin);
    B = globals.dstSliderMin - A * fmin;
    for(; gi <= globals.srcSliderMin + globals.srcSliderWidth; i++, gi++){
      g = gi;
      globals.lut[i] = (int) (A / (1.0 + exp(-(g - mu)/sig)) + B);
    }
    for(; gi < globals.srcMax; i++, gi++){
      globals.lut[i] = globals.dstSliderMax;
    }
    break;

  }

  if( globals.invert ){
    for(; i >0;){
      i--;
      globals.lut[i] = (globals.dstSliderMax + 
			globals.dstSliderMin - globals.lut[i]);
    }
  }

  return;
}

void remapImageCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  WlzPixelV	min, max;
  WlzPixelP	lut;
  WlzObject	*obj;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( globals.obj == NULL ){
    return;
  }

  /* change the grey-values "in-place" */
  min.type = WLZ_GREY_INT;
  min.v.inv = globals.srcMin;
  max.type = WLZ_GREY_INT;
  max.v.inv = globals.srcMax;
  lut.type = WLZ_GREY_UBYTE;
  lut.p.ubp = globals.lut;
  errNum = WlzGreySetRangeLut(globals.obj, min, max, lut);

  /* convert to a UBYTE image and install */
  obj = WlzAssignObject(WlzConvertPix(globals.obj, WLZ_GREY_UBYTE, &errNum),
			NULL);
  install_reference_object(obj);
  WlzFreeObj(obj);

  return;
}

void imageControlsCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  if( image_controls_dialog == NULL ){
    image_controls_dialog = createImageControls(globals.topl);
  }
  XtManageChild( image_controls_dialog );
  XtPopup( XtParent(image_controls_dialog), XtGrabNone );
  
  return;
}

void okImageControlsCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  return;
}

void imageGreyStatsCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  WlzGreyType	greyType;
  double	min, max, sum, sumSq, mean, stdDev;
  WlzErrorNum	errNum;
  char		strBuf[512];

  if( globals.obj == NULL ){
    return;
  }

  /* get & display the stats */
  WlzGreyStats(globals.obj, &greyType, &min, &max, &sum, &sumSq,
	       &mean, &stdDev, &errNum);
  sprintf(strBuf, 
	  "Image:- \n    %s\n"
	  "Grey statistics:-\n"
	  "    Min: %f\n"
	  "    Max: %f\n"
	  "    Mean: %f\n"
	  "    Std. Dev: %f", globals.file, min, max, mean, stdDev);
  HGU_XmUserInfo(globals.topl, strBuf, XmDIALOG_MODELESS);
  return;
}

void histMagPlusCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  float		xl, xu, yl, yu, X, Y;

  if( globals.objHistogram == NULL ){
    return;
  }

  /* get the current graph scale limits and cursor position */
  HGU_XmGetGraphLimits(globals.histogram, &xl, &xu, &yl, &yu);
  HGU_XmGetHairCursor(globals.histogram, &X, &Y);

  /* reset scale */
  xl = (xl + X) / 2.0;
  xu = (xu + X) / 2.0;
  yl = (yl + Y) / 2.0;
  yu = (yu + Y) / 2.0;
  HGU_XmSetGraphLimits(globals.histogram, xl, xu, yl, yu);

  return;
}

void histMagMinusCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  float		xl, xu, yl, yu, X, Y;

  if( globals.objHistogram == NULL ){
    return;
  }

  /* get the current graph scale limits and cursor position */
  HGU_XmGetGraphLimits(globals.histogram, &xl, &xu, &yl, &yu);
  HGU_XmGetHairCursor(globals.histogram, &X, &Y);

  /* reset scale */
  xl = 2.0 * xl - X;
  xu = 2.0 * xu - X;
  yl = 2.0 * yl - Y;
  yu = 2.0 * yu - Y;
  HGU_XmSetGraphLimits(globals.histogram, xl, xu, yl, yu);

  return;
}

void histMagResetCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  float		xl, xu, yl, yu;
  int		i;
  WlzHistogramDomain	*histDmn;

  if( globals.objHistogram == NULL ){
    return;
  }

  /* reset scale */
  xl = globals.srcMin;
  xu = globals.srcMax;
  yl = 0.0;
  yu = 0.0;
  histDmn = globals.objHistogram->domain.hist;
  switch( histDmn->type ){
  case WLZ_HISTOGRAMDOMAIN_INT:
    for(i=0; i < histDmn->nBins; i++){
      if( histDmn->binValues.inp[i] > yu ){
	yu = histDmn->binValues.inp[i];
      }
    }
    break;

  case WLZ_HISTOGRAMDOMAIN_FLOAT:
    for(i=0; i < histDmn->nBins; i++){
      if( histDmn->binValues.flp[i] > yu ){
	yu = histDmn->binValues.inp[i];
      }
    }
    break;
  }
  HGU_XmSetGraphLimits(globals.histogram, xl, xu, yl, yu);

  return;
}

void setImageControls(void)
{
  Widget	slider, option, toggle, widget;

  /* reset the graph scale */
  HGU_XmSetGraphLimits(globals.graph, (float) globals.srcMin,
		       (float) globals.srcMax + 1,
		       (float) 0, (float) 255);

  /* now reset the sliders */
  if( slider = XtNameToWidget(globals.imageDialog, "*src_grey_min") ){
    HGU_XmSetSliderRange(slider, (float) globals.srcMin,
			 (float) globals.srcMax);
    HGU_XmSetSliderValue(slider, (float) globals.srcSliderMin);
  }
  if( slider = XtNameToWidget(globals.imageDialog, "*src_grey_width") ){
    HGU_XmSetSliderRange(slider, (float) 0,
			 (float) globals.srcMax);
    HGU_XmSetSliderValue(slider, (float) globals.srcSliderWidth);
  }
  if( slider = XtNameToWidget(globals.imageDialog, "*dst_grey_min") ){
    HGU_XmSetSliderValue(slider, (float) globals.dstSliderMin);
  }
  if( slider = XtNameToWidget(globals.imageDialog, "*dst_grey_max") ){
    HGU_XmSetSliderValue(slider, (float) globals.dstSliderMax);
  }
  if( slider = XtNameToWidget(globals.imageDialog, "*gamma") ){
    HGU_XmSetSliderValue(slider, (float) globals.gamma);
  }
  if( slider = XtNameToWidget(globals.imageDialog, "*mean") ){
    HGU_XmSetSliderRange(slider, (float) globals.srcMin,
			 (float) globals.srcMax);
    HGU_XmSetSliderValue(slider, (float) globals.mean);
  }
  if( slider = XtNameToWidget(globals.imageDialog, "*sigma") ){
    HGU_XmSetSliderRange(slider, (float) 0,
			 (float) globals.srcMax);
    HGU_XmSetSliderValue(slider, (float) globals.sigma);
  }

  /* reset the transform type and invert toggle */
  if( option = XtNameToWidget(globals.imageDialog, "*transformType") ){
    switch( globals.transType ){
    default:
    case WLZ_GREYTRANSFORMTYPE_LINEAR:
      widget = XtNameToWidget(option, "*linear");
      break;

    case WLZ_GREYTRANSFORMTYPE_GAMMA:
      widget = XtNameToWidget(option, "*gamma");
      break;

    case WLZ_GREYTRANSFORMTYPE_SIGMOID:
      widget = XtNameToWidget(option, "*sigmoid");
      break;
    }
    if( widget ){
      XtVaSetValues(option, XmNmenuHistory, widget, NULL);
    }
  }
  if( toggle = XtNameToWidget(globals.imageDialog, "*invert") ){
    XtVaSetValues(toggle, XmNset, globals.invert, NULL);
  }
  

  /* re-display the transform graph */
  setTransformDisplay();

  return;
}

void resetImageControlValues(void)
{
  globals.srcSliderMin = WLZ_MAX(0, globals.srcMin);
  globals.srcSliderWidth = WLZ_MIN(globals.srcMax - globals.srcMin, 4095);
  globals.dstSliderMin = 0;
  globals.dstSliderMax = 255;
  globals.transType = WLZ_GREYTRANSFORMTYPE_LINEAR;
  globals.gamma = 1.0;
  globals.mean = globals.srcSliderMin + globals.srcSliderWidth/2;
  globals.sigma = globals.srcSliderWidth/20;
  globals.invert = 0;

  return;
}

void resetImageControlsCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  Widget	slider;

  /* reset all the values
     leave type selection and invert alone
  */
  resetImageControlValues();
  setLutTransform();
  setImageControls();

  /* reset the transform lut and re-display image */
  if( globals.ximage ){
    AlcFree(globals.ximage->data);
    globals.ximage->data = NULL;
    XDestroyImage(globals.ximage);
    globals.ximage = NULL;    
  }
  globals.ximage = HGU_XmObjToXImage(w, globals.view_object);

  /* calculate the expose event and call expose callback */
  canvasExposeCb(globals.canvas, NULL, NULL);

  return;
}

static WlzIVertex2     	hist_vtx[1024];
static WlzPolygonDomain	*hist_polydmn=NULL;
void setHistogramDisplayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		i, min, max, nBins, nMax;
  double	binOrigin, binSize;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	widget;

  if( (globals.obj == NULL) || globals.objHistogram ){
    return;
  }

  /* get max and min from globals */
  min = globals.srcMin;
  max = globals.srcMax;
  nBins = WLZ_MIN((max-min+1), 1024);
  binOrigin = min;
  binSize = ((double)(max - min)) / nBins;

  /* calculate the histogram & reset the display */
  if( hist_polydmn == NULL ){
    hist_polydmn = WlzMakePolygonDomain(WLZ_POLYGON_INT, 1024,
					hist_vtx, 1024, 0, NULL);
  }
  if( globals.objHistogram = WlzHistogramObj(globals.obj, nBins,
					     binOrigin, binSize,
					     &errNum) ){
    globals.objHistogram = WlzAssignObject(globals.objHistogram, NULL);
    switch( globals.objHistogram->domain.hist->type ){
    case WLZ_HISTOGRAMDOMAIN_INT:
      for(i=0, nMax=0; i < nBins; i++){
	hist_vtx[i].vtX = binOrigin + i * binSize;
	hist_vtx[i].vtY = globals.objHistogram->domain.hist->binValues.inp[i];
	nMax = WLZ_MAX(nMax, hist_vtx[i].vtY);
      }
      break;

    case WLZ_HISTOGRAMDOMAIN_FLOAT:
      for(i=0, nMax=0; i < nBins; i++){
	hist_vtx[i].vtX = binOrigin + i * binSize;
	hist_vtx[i].vtY = globals.objHistogram->domain.hist->binValues.flp[i];
	nMax = WLZ_MAX(nMax, hist_vtx[i].vtY);
      }
      break;
    }
  }
  HGU_XmSetGraphLimits(globals.histogram, (float) min, (float) max,
		       (float) 0, (float) nMax);
  HGU_XmSetGraphPolyline(globals.histogram, hist_polydmn, 1);

  return;
}


static WlzIVertex2	win_vtx[5], lut_vtx[256];
static WlzPolygonDomain	*win_polydmn=NULL, *lut_polydmn=NULL;
void setTransformDisplay(void)
{
  int		i;
  double	g, A, B, fmin, fmax, mu, sig;

  /* display the transform window */
  win_vtx[0].vtX = win_vtx[3].vtX = win_vtx[4].vtX = globals.srcSliderMin;
  win_vtx[0].vtY = win_vtx[1].vtY = win_vtx[4].vtY = globals.dstSliderMin;
  win_vtx[1].vtX = win_vtx[2].vtX = globals.srcSliderMin + globals.srcSliderWidth;
  win_vtx[2].vtY = win_vtx[3].vtY = globals.dstSliderMax;
  if( win_polydmn == NULL ){
    win_polydmn = WlzMakePolygonDomain(WLZ_POLYGON_INT, 5,
				       win_vtx, 5, 0, NULL);
  }
  HGU_XmSetGraphPolyline(globals.graph, win_polydmn, 1);

  /* display the transform */
  if( lut_polydmn == NULL ){
    lut_polydmn = WlzMakePolygonDomain(WLZ_POLYGON_INT, 256,
				       lut_vtx, 256, 0, NULL);
  }

  lut_vtx[0].vtX = globals.srcMin;
  lut_vtx[0].vtY = globals.dstSliderMin;
  lut_vtx[1].vtX = globals.srcSliderMin;
  lut_vtx[1].vtY = globals.dstSliderMin;
  for(i=2; i < 200; i++){
    g = globals.srcSliderMin + (i-2) * globals.srcSliderWidth / 198;
    lut_vtx[i].vtX = (int) g;
    lut_vtx[i].vtY = globals.lut[lut_vtx[i].vtX - globals.srcMin];
  }
  lut_vtx[i].vtX = globals.srcSliderMin + globals.srcSliderWidth;
  lut_vtx[i++].vtY = globals.dstSliderMax;
  lut_vtx[i].vtX = globals.srcMax;
  lut_vtx[i++].vtY = globals.dstSliderMax;
  lut_polydmn->nvertices = i;
  
  HGU_XmSetGraphPolyline(globals.graph, lut_polydmn, 2);

  return;
}

void setTransformDisplayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  setLutTransform();
  setTransformDisplay();

  /* reset the transform lut and re-display image */
  if( globals.view_object ){
    if( globals.ximage ){
      AlcFree(globals.ximage->data);
      globals.ximage->data = NULL;
      XDestroyImage(globals.ximage);
      globals.ximage = NULL;    
    }
    globals.ximage = HGU_XmObjToXImage(globals.canvas, globals.view_object);

    /* calculate the expose event and call expose callback */
    canvasExposeCb(globals.canvas, NULL, NULL);
  }

  return;
}

void srcSliderMinCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "src_grey_min") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( (globals.srcSliderWidth + newval) > globals.srcMax ){
    newval = globals.srcMax - globals.srcSliderWidth;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.srcSliderMin = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

void srcSliderWidthCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "src_grey_width") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( (globals.srcSliderMin + newval) > globals.srcMax ){
    newval = globals.srcMax - globals.srcSliderMin;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.srcSliderWidth = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

void dstSliderMinCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "dst_grey_min") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval > globals.dstSliderMax ){
    newval = globals.dstSliderMax;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.dstSliderMin = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

void dstSliderMaxCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "dst_grey_max") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval < globals.dstSliderMin ){
    newval = globals.dstSliderMin;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  globals.dstSliderMax = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

void invertTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;

  globals.invert = cbs->set;
  setTransformDisplayCb(w, client_data, call_data);
  return;
}

void transformTypeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  globals.transType = (WlzGreyTransformType) client_data;
  setTransformDisplayCb(w, client_data, call_data);
  return;
}

void gammaSetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  double	newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "gamma") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (double) HGU_XmGetSliderValue(slider);
  globals.gamma = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

void meanSetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  double	newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "mean") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (double) HGU_XmGetSliderValue(slider);
  globals.mean = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

void sigmaSetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  double	newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "sigma") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (double) HGU_XmGetSliderValue(slider);
  globals.sigma = newval;

  setTransformDisplayCb(w, client_data, call_data);

  return;
}

/* create the image grey-level mapping controls */
static ActionAreaItem   image_controls_dialog_actions[] = {
  {"Apply",	NULL,		NULL},
  {"Reset",	NULL,		NULL},
  {"Dismiss",     NULL,           NULL},
  {"Help",        NULL,           NULL},
};

static MenuItem transformTypeItems[] = {
  {"linear", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   transformTypeCb, (XtPointer) WLZ_GREYTRANSFORMTYPE_LINEAR,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"gamma", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   transformTypeCb, (XtPointer) WLZ_GREYTRANSFORMTYPE_GAMMA,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"sigmoid", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   transformTypeCb, (XtPointer) WLZ_GREYTRANSFORMTYPE_SIGMOID,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

Widget	createImageControls(
  Widget	topl)
{
  Widget	dialog, control, form, frame, title, title_form;
  Widget	rowcolumn, button, option, toggle, widget, rc;
  Widget	graph, graphics, slider, scale;
  Widget	histogram_frame, transform_frame, grey_bounds_frame;
  Widget	transform_params_frame;
  Visual	*visual;
  Arg		arg[1];

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

  dialog = HGU_XmCreateStdDialog(topl, "image_controls_dialog",
				 xmFormWidgetClass,
				 image_controls_dialog_actions, 4);

  if( (widget = XtNameToWidget(dialog, "*.Apply")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, remapImageCb, NULL);
  }

  if( (widget = XtNameToWidget(dialog, "*.Reset")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, resetImageControlsCb, NULL);
  }

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }
  globals.imageDialog = dialog;

  control = XtNameToWidget( dialog, "*.control" );

  /* frame for the histogram */
  frame = XtVaCreateManagedWidget("histogram_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  histogram_frame = frame;
  
  rc = XtVaCreateManagedWidget("histogram_frame_rc", xmRowColumnWidgetClass,
			       frame,
			       XmNchildType, XmFRAME_TITLE_CHILD,
			       XmNorientation, XmHORIZONTAL,
			       XmNpacking, XmPACK_TIGHT,
			       NULL);
  title = XtVaCreateManagedWidget("histogram_frame_title", xmLabelWidgetClass,
				  rc, NULL);
  button = XtVaCreateManagedWidget("hist_magp", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, histMagPlusCb, NULL);
  button = XtVaCreateManagedWidget("hist_magn", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, histMagMinusCb, NULL);
  button = XtVaCreateManagedWidget("hist_reset", xmPushButtonWidgetClass,
				   rc, NULL);
  XtAddCallback(button, XmNactivateCallback, histMagResetCb, NULL);

  graph = HGU_XmCreateGraphD("histogram_graph", frame, NULL,
			     XtWindow(globals.topl));
  HGU_XmSetGraphLimits(graph, (float) 0, (float) 4095,
		       (float) 0, (float) 255);
  globals.histogram = graph;
  graphics = XtNameToWidget(graph, "*.graphics");
  XtAddCallback(graphics, XmNexposeCallback, setHistogramDisplayCb, dialog);

  /* frame for the transform parameters */
  frame = XtVaCreateManagedWidget("transform_params_frame", xmFrameWidgetClass,
				  control,
				  XmNbottomAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  transform_params_frame = frame;
  
  title = XtVaCreateManagedWidget("transform_params_frame_title",
				  xmLabelWidgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("transform_params_form", xmFormWidgetClass,
				 frame, NULL);

  option = HGU_XmBuildMenu(form, XmMENU_OPTION, "transformType", 0,
			   XmTEAR_OFF_DISABLED, transformTypeItems);
  XtManageChild(option);
  XtVaSetValues(option,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		NULL);

  button = XtVaCreateManagedWidget("invert", xmToggleButtonWidgetClass,
				   form,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	option,
				   XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
				   XmNbottomWidget,	option,
				   NULL);
  XtAddCallback(button, XmNvalueChangedCallback, invertTransCb, NULL);

  slider = HGU_XmCreateHorizontalSlider("gamma", form,
					(float) globals.gamma, (float) 0.1,
					(float) 10.0, 2,
					gammaSetCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		option,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, gammaSetCb, NULL);
  widget = slider;

  slider = HGU_XmCreateHorizontalSlider("mean", form,
					(float) globals.mean,
					(float) globals.srcMin,
					(float) globals.srcMax,
					0,
					meanSetCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, meanSetCb, NULL);

  slider = HGU_XmCreateHorizontalSlider("sigma", form,
					(float) globals.sigma, (float) 0.0,
					(float) globals.srcMax,
					0,
					sigmaSetCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	51,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	99,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, sigmaSetCb, NULL);

  /* frame for the transform grey-level bounds */
  frame = XtVaCreateManagedWidget("grey_bounds_frame", xmFrameWidgetClass,
				  control,
				  XmNbottomAttachment, XmATTACH_WIDGET,
				  XmNbottomWidget, transform_params_frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  grey_bounds_frame = frame;
  
  title = XtVaCreateManagedWidget("grey_bounds_frame_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  form = XtVaCreateManagedWidget("grey_bounds_form", xmFormWidgetClass,
				 frame, NULL);

  slider = HGU_XmCreateHorizontalSlider("src_grey_min", form,
					(float) globals.srcSliderMin,
					(float) globals.srcMin,
					(float) globals.srcMax,
					0,
					srcSliderMinCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, srcSliderMinCb, NULL);

  slider = HGU_XmCreateHorizontalSlider("src_grey_width", form,
					(float) globals.srcSliderWidth,
					(float) 0.0,
					(float) globals.srcMax,
					0,
					srcSliderWidthCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	51,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	99,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, srcSliderWidthCb, NULL);

  slider = HGU_XmCreateHorizontalSlider("dst_grey_min", form,
					(float) globals.dstSliderMin,
					(float) 0.0,
					(float) 255.0, 0,
					dstSliderMinCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, dstSliderMinCb, NULL);
  

  slider = HGU_XmCreateHorizontalSlider("dst_grey_max", form,
					(float) globals.dstSliderMax,
					(float) 0.0,
					(float) 255.0, 0,
					dstSliderMaxCb, NULL);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	51,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	99,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, dstSliderMaxCb, NULL);

  /* frame for the transform view */
  frame = XtVaCreateManagedWidget("transform_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment,	XmATTACH_WIDGET,
				  XmNtopWidget,		histogram_frame,
				  XmNbottomAttachment,	XmATTACH_WIDGET,
				  XmNbottomWidget,	grey_bounds_frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  transform_frame = frame;
  
  title = XtVaCreateManagedWidget("transform_frame_title",
				  xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  NULL);
  graph = HGU_XmCreateGraphD("transform_graph", frame, NULL,
			     XtWindow(globals.topl));
  HGU_XmSetGraphLimits(graph, (float) globals.srcMin, (float) globals.srcMax + 1,
		       (float) 0, (float) 255);
  globals.graph = graph;
  graphics = XtNameToWidget(graph, "*.graphics");
  XtAddCallback(graphics, XmNexposeCallback, setTransformDisplayCb, dialog);

  return dialog;
}


void image_menu_init(
  Widget	topl)
{
  Widget	form, file_type_menu, toggle, widget;
  XmString	xmstr;
  Visual	*visual;
  Arg		arg[1];

  /* initialise the globals */
  if(globals.obj == NULL){
    globals.imageDialog = NULL;
    globals.objHistogram = NULL;
    globals.histogram = NULL;
    globals.graph = NULL;
    globals.srcMin = 0;
    globals.srcMax = 4095;
    resetImageControlValues();
    globals.transType = WLZ_GREYTRANSFORMTYPE_LINEAR;
    globals.invert = 0;
    globals.lut = (UBYTE *) AlcCalloc(0xffff, sizeof(char));
    setLutTransform();
  }

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);


  return;
}
