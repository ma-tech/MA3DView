#ifndef MA3DVIEWTYPES_H
#define MA3DVIEWTYPES_H

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
*   File       :   MA3DViewTypes.h					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 13:30:21 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

/* defines */
#define       CANVAS_MARGIN 32

typedef struct _DomainListItem {
  char		*file;
  int		fileDisplayIndx;
  WlzObject	*obj;
  WlzObject	*currObj;
  WlzObject	*currBound;
  WlzObject	*warpedBound;
  int		currPlane;
  int		changedFlg;
  XmString	listStr;
  int		listIndx;
  int		threshVal;
} DomainListItem;

/* global variables */
typedef struct _3DViewGlobals{
  
  Widget		topl;
  XtAppContext		app_con;
  GC			gc_greys, gc_red, gc_blue, gc_green;
  GC			gc_set;
  Visual		*toplVisual;
  int			toplDepth;
  Colormap		cmap;

  /* file_menu globals */
  char			*file;
  WlzObject		*obj;
  WlzObject		*obj2D;
  
  /* image controls globals */
  WlzObject		*objHistogram;
  Widget		imageDialog;
  Widget		histogram;
  Widget		graph;
  int			srcMin;
  int			srcMax;
  int			srcSliderMin;
  int			srcSliderWidth;
  int			dstSliderMin;
  int			dstSliderMax;
  WlzGreyTransformType	transType;
  double		gamma;
  double		mean;
  double		sigma;
  int			invert;
  unsigned char		*lut;

  /* 3D view globals */
  GLuint		ref_display_list;
  WlzDVertex3		bbox_vtx, bbox_size;  
  Widget		canvas3D;

  /* domain list globals */
  HGUDlpList		*dmnList;
  DomainListItem	*currDmnListItem;
  Widget		dmnListWidget;

  /* domain menu globals */
  int			smoothItr;

  /* main_work_area globals */
  Widget		canvas;
  Display		*dpy;
  Window		win;
  Widget		distSlider;
  float			magVal;
  WlzThreeDViewStruct	*wlzViewStr;
  WlzObject		*view_object;
  XImage		*ximage;

  /* crop image globals */
  WlzDVertex3		cropBBMin;
  WlzDVertex3		cropBBMax;

  /* main_button_bar globals */

  /* message area */
  Widget		message;

} ThreeDViewGlobals;

extern ThreeDViewGlobals globals;


/* do not add anything after this line */
#endif /* MA3DVIEWTYPES_H */
