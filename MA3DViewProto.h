#ifndef MA3DVIEWPROTO_H
#define MA3DVIEWPROTO_H

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
*   File       :   MA3DViewProto.h					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 13:42:09 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

/* from all over! */
extern Widget	create_main_buttonbar	(Widget w); /* from main_buttonbar.c */
extern Widget	create_main_menubar	(Widget w); /* from main_menubar.c   */

extern void	init_main_buttonbar	(Widget w); /* from main_menubar.c   */
extern void	init_main_menubar	(Widget w); /* from main_menubar.c   */

/* main_work_area.c */
extern void setViewSelection(String	selection);
extern void canvasExposeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void distanceCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void	init_main_work_area	(Widget w);
extern Widget	create_main_work_area	(Widget w);
extern XImage *HGU_XmObjToXImage(
  Widget	w,
  WlzObject	*obj);
extern void canvas_input_cb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void display_pointer_feedback_information(
  int	x,
  int	y);

/* main_buttonbar.c */

/* domain_menu.c */
extern MenuItem		*domain_menu_items;
extern void	domain_menu_init(Widget	topl);
extern void readDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void writeDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void writeAllDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void removeDomainCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void displayDomains(void);
extern void domainListCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void installNewDomain(
  WlzObject	*obj,
  char		*file,
  int		displayIndx);
extern void clearDomainBoundaries(void);

/* image_menu.c */
extern MenuItem		*image_menu_items;
extern Widget	createImageControls(Widget topl);
extern void image_menu_init(
  Widget	topl);

extern void remapImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void imageControlsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void imageGreyStatsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void setHistogramDisplayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void setTransformDisplayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void setLutTransform(void);
extern void resetImageControlValues(void);
extern void setImageControls(void);
extern void setTransformDisplay(void);

/* file_menu.c */
extern MenuItem		*file_menu_items;

extern void	file_menu_init(Widget	topl);

extern void	read_reference_cb(Widget		w,
				  XtPointer	client_data,
				  XtPointer	call_data);

extern void	write_obj_cb	(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);

extern void	quit_cb		(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);
extern void resetGreyRange(void);

extern void exportImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

extern void exportImageFileCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void install_reference_object(
  WlzObject	*obj);
extern void referenceFileListCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void WlzXmFileListItemFree(
  void	*entry);
extern AlcDLPList *WlzXmFileListCreateList(
  String	resourceFile,
  WlzErrorNum	*dstErr);
extern MenuItem *WlzXmFileListCreateMenuItems(
  AlcDLPList	*fileList,
  XtCallbackProc	callbackProc,
  WlzErrorNum	*dstErr);
extern WlzErrorNum WlzXmFileListAddFile(
  AlcDLPList	*fileList,
  String	file,
  WlzEffFormat	format);
extern void WlzXmFileListWriteHeader(
  FILE	*fp);
extern WlzErrorNum WlzXmFileListWriteResourceFile(
  AlcDLPList	*fileList,
  String	resourceFile);
extern WlzObject *WlzXmFileListReadObject(
  Widget			w,
  WlzXmFileListCallbackStruct	*cbs,
  WlzErrorNum			*dstErr);

/* MAColormapUtils.c */
extern void HGU_XmCreatePrivateColormap(Widget w);
extern void init_warp_paint_cmap(Widget	w);
extern unsigned long HGU_XGetColorPixel(
  Display	*dpy,
  Colormap	cmap,
  float		red,
  float		green,
  float		blue);

/* MAWarpPaint.c */
extern char	*initial_image_file;

/* MAWarpPaintUtils.c */
extern void popup_dialog_cb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);
extern set_backingX(Display	*dpy,
		    Window	win);
extern void UnmanageChildCb(Widget	w,
			    XtPointer	client_data,
			    XtPointer	call_data);
extern void destroy_cb(Widget          w,
		       XtPointer       client_data,
		       XtPointer       call_data);
extern void HGU_XmSetHourGlassCursor(Widget	w);
extern void HGU_XmUnsetHourGlassCursor(Widget	w);
extern int InteractDispatchEvent(Display		*dpy,
				 Window			win,
				 XtPointer		client_data,
				 XEvent			*event);
extern int InteractDispatchWinEvent(Display		*dpy,
				    Window		win,
				    XtPointer		client_data,
				    caddr_t		*cb_struct);
extern char *HGU_XRequestToString(
  char	request_code);

/* MAOpenGLUtils.c */
extern void MAOpenGLInitCb(Widget 	w,
			   XtPointer	clientData,
			   XtPointer	callData);
extern void MAOpenGLDrawScene(Widget canvasW);

/* MA3DViewCropImage.c */
extern void cropImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);
extern void displayCropRectangle(void);

/* MAFilterImage.c */
extern Widget	createFilterControls(Widget topl);
extern void filterImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* do not add anything after this line */
#endif /* MA3DVIEWPROTO_H */
