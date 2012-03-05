#ifndef MA3DVIEWTYPES_H
#define MA3DVIEWTYPES_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MA3DViewTypes_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MA3DViewTypes.h
* \author	Richard Baldock
* \date         October 2001
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
  String		resourceFile;
  AlcDLPList		*fileList;
  
  /* image controls globals */
  WlzObject		*objHistogram[3];
  int			objIndx;
  Widget		imageDialog;
  Widget		histogram[3];
  Widget		graph;
  WlzGreyType		gType;
  int			srcMin[3];
  int			srcMax[3];
  int			srcSliderMin[3];
  int			srcSliderWidth[3];
  int			dstSliderMin[3];
  int			dstSliderMax[3];
  WlzGreyTransformType	transType[3];
  double		gamma[3];
  double		mean[3];
  double		sigma[3];
  int			invert[3];
  unsigned char		*lut[3];

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
