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
*   File       :   file_menu.c						*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 14:33:21 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <MA3DView.h>

static void image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

static void write_image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data);

/* menu item structure */
static MenuItem file_type_menu_itemsP[] = {   /* file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_WLZ,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ics", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_ICS,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"den", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_DEN,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VFF,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pic", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PIC,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vtk", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_VTK,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"slc", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_SLC,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ipl", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_IPL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_PNM,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"bmp", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_BMP,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"tiff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   image_type_cb, (XtPointer) WLZEFF_FORMAT_TIFF,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem write_file_type_menu_itemsP[] = {/* write file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_WLZ,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"ics", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_ICS,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"den", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_DEN,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_VFF,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pic", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_PIC,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"vtk", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_VTK,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"slc", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_SLC,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_PNM,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"bmp", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_BMP,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"tiff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   write_image_type_cb, (XtPointer) WLZEFF_FORMAT_TIFF,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

static MenuItem file_menu_itemsP[] = {		/* file_menu items */
  {"read_reference", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   read_reference_cb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"write_reference", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   exportImageCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"Quit", &xmPushButtonGadgetClass, 'Q', "Ctrl<Key>Q", "Ctrl-Q", False,
   quit_cb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

MenuItem	*file_menu_items = &(file_menu_itemsP[0]);

static MenuItem HGU_XmMiscMenuItemsP[] = {     /* misc_menu items */
  {"save_state", &xmPushButtonGadgetClass, 0,NULL, NULL, False,
   SaveStateCallback, NULL, HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"restore_state", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   RestoreStateCallback, NULL, HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL, XmTEAR_OFF_DISABLED, False, False, NULL},
  {"macro", &xmCascadeButtonGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_ENABLED, False, False, &(HGU_XmMacroMenuItemsP[0])},
  NULL,
};
MenuItem	*HGU_XmMiscMenuItems = &(HGU_XmMiscMenuItemsP[0]);

static Widget	read_reference_dialog;
static Widget	export_image_dialog;

/* action and callback procedures */
void exportImageFileCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  FILE		*fp;
  String		icsfile;

  if( globals.obj == NULL ){
    return;
  }

  /* get the file pointer or file name if ics format */
  if((image_type == WLZEFF_FORMAT_ICS) ||
     (image_type == WLZEFF_FORMAT_PNM) ||
     (image_type == WLZEFF_FORMAT_BMP) ||
     (image_type == WLZEFF_FORMAT_TIFF))
  {
    if( (icsfile = HGU_XmGetFileStr(globals.topl, cbs->value,
				    cbs->dir)) == NULL )
    {
      return;
    }
    fp = NULL;
  }
  else
  {
    if( (fp = HGU_XmGetFilePointer(globals.topl, cbs->value,
				   cbs->dir, "w")) == NULL )
    {
      return;
    }
    icsfile = NULL;
  }

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* write the reference object */
  if( globals.obj2D ){
    WlzObject *tmpObj;
    tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, globals.obj->domain.p->domains[0],
			 globals.obj->values.vox->values[0], NULL, NULL, NULL);
    WlzEffWriteObj(fp, icsfile, tmpObj, image_type);
    WlzFreeObj(tmpObj);
  }
  else {
    WlzEffWriteObj(fp, icsfile, globals.obj, image_type);
  }

  /* close the file pointer if non NULL */
  if( fp )
  {
    if( fclose( fp ) == EOF ){
      HGU_XmUserError(globals.topl,
		      "Write Reference Object:\n"
		      "    Error closing the reference object file\n"
		      "    Please check disk sapce or quotas.\n"
		      "    Object not saved to disk.",
		      XmDIALOG_FULL_APPLICATION_MODAL);
    }
  }

  /* set hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  return;
}

void exportImageCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  XtManageChild( export_image_dialog );
  XtPopup( XtParent(export_image_dialog), XtGrabNone );
  return;
}

void read_reference_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    XtManageChild( read_reference_dialog );
    XtPopup( XtParent(read_reference_dialog), XtGrabNone );
    return;
}

void resetGreyRange(void)
{
  WlzPixelV	greyMin, greyMax;
  int		min, max;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( globals.obj == NULL ){
    return;
  }

  /* calculate the grey-range & reset the scale */
  errNum = WlzGreyRange(globals.obj, &greyMin, &greyMax);
  switch( greyMin.type ){
  case WLZ_GREY_LONG:
    min = greyMin.v.lnv;
    max = greyMax.v.lnv;
    break;

  case WLZ_GREY_INT:
    min = greyMin.v.inv;
    max = greyMax.v.inv;
    break;

  case WLZ_GREY_SHORT:
    min = greyMin.v.shv;
    max = greyMax.v.shv;
    break;

  case WLZ_GREY_UBYTE:
    min = greyMin.v.ubv;
    max = greyMax.v.ubv;
    break;

  case WLZ_GREY_FLOAT:
    min = greyMin.v.flv;
    max = greyMax.v.flv;
    break;

  case WLZ_GREY_DOUBLE:
    min = greyMin.v.dbv;
    max = greyMax.v.dbv;
    break;

  case WLZ_GREY_BIT:
  case WLZ_GREY_RGBA:
  default:
    return;
  }
  if( min >= 0 ){
    min = 0;
    if( max < 256 ){
      max = 255;
    }
    else if( max < 4096 ){
      max = 4095;
    }
  }

  globals.srcMin = min;
  globals.srcMax = max;
  if( globals.objHistogram ){
    WlzFreeObj( globals.objHistogram );
    globals.objHistogram = NULL;
  }
  if( globals.histogram ){
    Widget graphics = XtNameToWidget(globals.histogram, "*.graphics");
    setHistogramDisplayCb(graphics, NULL, NULL);
  }

  return;
}

void install_reference_object(
  WlzObject	*obj)
{
  WlzIBox2	cutBox;
  Dimension	width, height;
  WlzPixelV	min, max, Min, Max;
  char		*data;
  XWindowAttributes	win_att;
  float		sliderVal;
  WlzDomain	domain;
  WlzValues	values;
  WlzObject	*newObj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  

  /* check object type */
  if( obj == NULL ){
    return;
  }
  if( (obj->type == WLZ_2D_DOMAINOBJ) && (obj->values.core != NULL) ){
    globals.obj2D = WlzAssignObject(obj, NULL);
    domain.p = WlzMakePlaneDomain(WLZ_PLANEDOMAIN_DOMAIN, 0, 0,
				  obj->domain.i->line1,
				  obj->domain.i->lastln,
				  obj->domain.i->kol1,
				  obj->domain.i->lastkl,
				  &errNum);
    domain.p->domains[0] = WlzAssignDomain(obj->domain, NULL);
    values.vox = WlzMakeVoxelValueTb(WLZ_VOXELVALUETABLE_GREY,
				     0, 0, WlzGetBackground(obj, NULL),
				     NULL, &errNum);
    values.vox->values[0] = WlzAssignValues(obj->values, NULL);
    newObj = WlzMakeMain(WLZ_3D_DOMAINOBJ, domain, values,
			 NULL, NULL, &errNum);
    obj = newObj;
  }
    
  if( (obj->type != WLZ_3D_DOMAINOBJ) || (obj->values.core == NULL) ){
    HGU_XmUserError(globals.topl,
		    "Install Reference Object:\n"
		    "    Not a 3D grey-level object please check the\n"
		    "    file and try again",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
  if( globals.obj ){
    WlzFreeObj(globals.obj);
  }
  globals.obj = WlzAssignObject(obj, NULL);

  /* reset the histogram */
  resetGreyRange();

  /* reset the lut transform */
  resetImageControlValues();
  setLutTransform();
  if( globals.imageDialog ){
    setImageControls();
  }

  /* reset view to x-y */
  setViewSelection("x-y");

  return;
}


void set_topl_title(
  String	name)
{
  String	name_return, class_return;
  char		str_buff[64];
  int		i, basename_start;

  /* get the top level dialog and application name */
  XtGetApplicationNameAndClass(XtDisplay(globals.topl),
			       &name_return, &class_return);
  /* get file basename */
  for(i=0, basename_start=0; i < strlen(name); i++){
    if( name[i] == '/' ){
      basename_start = i+1;
    }
  }
  (void) sprintf(str_buff, "%s: %s", name_return, name+basename_start);
  XtVaSetValues(globals.topl, XtNtitle, str_buff, NULL);

  return;
}
  

void read_reference_object_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  WlzObject		*obj;
  FILE		*fp;
  String		icsfile;

  /* get the file pointer or file name if ics format */
  if((image_type == WLZEFF_FORMAT_ICS) ||
     (image_type == WLZEFF_FORMAT_PNM) ||
     (image_type == WLZEFF_FORMAT_BMP) ||
     (image_type == WLZEFF_FORMAT_TIFF))
  {
    if( (icsfile = HGU_XmGetFileStr(globals.topl, cbs->value,
				    cbs->dir)) == NULL )
    {
      return;
    }
    fp = NULL;
  }
  else
  {
    if( (fp = HGU_XmGetFilePointer(globals.topl, cbs->value,
				   cbs->dir, "r")) == NULL )
    {
      return;
    }
    icsfile = NULL;
  }

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* read the new reference object */
  obj = WlzAssignObject(WlzEffReadObj(fp, icsfile, image_type, NULL), NULL);

  /* close the file pointer if non NULL */
  if( fp )
  {
    (void) fclose( fp );
  }

  /* check the object */
  if( obj == NULL ){
    HGU_XmUserError(globals.topl,
		    "Read Reference Object:\n"
		    "    No reference object read - either the\n"
		    "    selected file is empty or it is not a\n"
		    "    woolz object file - please check the file\n"
		    "    or make a new selection",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
  }
  if( obj->values.core == NULL ){
    HGU_XmUserError(globals.topl,
		    "Read Reference Object:\n"
		    "    The reference object must have a grey-\n"
		    "    value table. Please select an alternate\n"
		    "    object",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    WlzFreeObj( obj );
    /* set hour glass cursor */
    HGU_XmUnsetHourGlassCursor(globals.topl);
    return;
  }

  /* install the new reference object */
  install_reference_object( obj );
  WlzFreeObj( obj );

  /* set the title of the top-level car */
  if( XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &icsfile) ){
    set_topl_title(icsfile);
    globals.file = icsfile;
  }

  /* set hour glass cursor */
  HGU_XmUnsetHourGlassCursor(globals.topl);
  return;
}


void quit_cb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    int		i;
    char	str_buf[128];
    String	str;

    /* make sure we are mapped and at the top */
    XtMapWidget( globals.topl );

    /* use a dialog widget to confirm exit */
    if( !HGU_XmUserConfirm(globals.topl, "Really quit?", "Yes", "No", 1 ) )
	return;

    /* blah blah blah actions */

    /* use a dialog widget to double confirm exit  - last possible cop out */
    if( !HGU_XmUserConfirm(globals.topl, "Really really quit?",
			   "Yes", "No", 1 ) )
	return;

    /* kill the help viewer */
    HGU_XmHelpDestroyViewer();

    XtDestroyWidget( globals.topl );
    exit( 0 );
}

static void image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  XmString	pattern_str;

  XtRemoveAllCallbacks(read_reference_dialog, XmNokCallback);

  switch( image_type )
  {
   default:
   case WLZEFF_FORMAT_WLZ:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_WLZ);
     pattern_str = XmStringCreateSimple( "*.wlz" );
     break;
     
   case WLZEFF_FORMAT_ICS:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_ICS);
     pattern_str = XmStringCreateSimple( "*.ics" );
     break;

   case WLZEFF_FORMAT_DEN:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_DEN);
     pattern_str = XmStringCreateSimple( "*.den" );
     break;

   case WLZEFF_FORMAT_VFF:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_VFF);
     pattern_str = XmStringCreateSimple( "*.vff" );
     break;

   case WLZEFF_FORMAT_PIC:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_PIC);
     pattern_str = XmStringCreateSimple( "*.pic" );
     break;

   case WLZEFF_FORMAT_VTK:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_VTK);
     pattern_str = XmStringCreateSimple( "*.vtk" );
     break;

   case WLZEFF_FORMAT_SLC:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_SLC);
     pattern_str = XmStringCreateSimple( "*.slc" );
     break;

   case WLZEFF_FORMAT_IPL:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_IPL);
     pattern_str = XmStringCreateSimple( "*.ipl" );
     break;
     
   case WLZEFF_FORMAT_PNM:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_PNM);
     pattern_str = XmStringCreateSimple( "*.p?m" );
     break;
     
   case WLZEFF_FORMAT_BMP:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_BMP);
     pattern_str = XmStringCreateSimple( "*.bmp" );
     break;

   case WLZEFF_FORMAT_TIFF:
     XtAddCallback(read_reference_dialog, XmNokCallback,
		   read_reference_object_cb, (XtPointer) WLZEFF_FORMAT_TIFF);
     pattern_str = XmStringCreateSimple( "*.tif?" );
     break;
  }

  XtAddCallback(read_reference_dialog, XmNokCallback, PopdownCallback, NULL);

  XtVaSetValues(read_reference_dialog, XmNpattern, pattern_str, NULL);
  XmStringFree( pattern_str );
  XmFileSelectionDoSearch( read_reference_dialog, NULL );

  return;
}

static void write_image_type_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzEffFormat	image_type = (WlzEffFormat) client_data;
  XmString	pattern_str;

  XtRemoveAllCallbacks(export_image_dialog, XmNokCallback);

  switch( image_type )
  {
   default:
   case WLZEFF_FORMAT_WLZ:
     pattern_str = XmStringCreateSimple( "*.wlz" );
     break;
     
   case WLZEFF_FORMAT_ICS:
     pattern_str = XmStringCreateSimple( "*.ics" );
     break;

   case WLZEFF_FORMAT_DEN:
     pattern_str = XmStringCreateSimple( "*.den" );
     break;
     
   case WLZEFF_FORMAT_VFF:
     pattern_str = XmStringCreateSimple( "*.vff" );
     break;
     
   case WLZEFF_FORMAT_PIC:
     pattern_str = XmStringCreateSimple( "*.pic" );
     break;
     
   case WLZEFF_FORMAT_VTK:
     pattern_str = XmStringCreateSimple( "*.vtk" );
     break;
     
   case WLZEFF_FORMAT_SLC:
     pattern_str = XmStringCreateSimple( "*.slc" );
     break;
     
   case WLZEFF_FORMAT_PNM:
     pattern_str = XmStringCreateSimple( "*.p?m" );
     break;
     
   case WLZEFF_FORMAT_BMP:
     pattern_str = XmStringCreateSimple( "*.bmp" );
     break;
     
   case WLZEFF_FORMAT_TIFF:
     pattern_str = XmStringCreateSimple( "*.tif?" );
     break;
     
  }

  XtAddCallback(export_image_dialog, XmNokCallback,
		exportImageFileCb, client_data);
  XtAddCallback(export_image_dialog, XmNokCallback, PopdownCallback, NULL);

  XtVaSetValues(export_image_dialog, XmNpattern, pattern_str, NULL);
  XmStringFree( pattern_str );
  XmFileSelectionDoSearch( export_image_dialog, NULL );

  return;
}


/* file_menu initialisation procedure */
#define set_att_offset(field)   XtOffsetOf(ThreeDViewGlobals, field)

void file_menu_init(
  Widget	topl)
{
  Widget	form, file_type_menu, toggle;
  XmString	xmstr;
  Visual	*visual;
  Arg		arg[1];

  /* initialise the globals including some work-area globals */
  globals.file = NULL;
  globals.obj = NULL;
  globals.obj2D = NULL;
  globals.wlzViewStr = NULL;
  globals.view_object = NULL;
  globals.ximage = NULL;

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

  /* create the read-obj file selection dialog */
  read_reference_dialog = XmCreateFileSelectionDialog( topl,
						"read_reference_dialog", arg, 1);

  /* add a form to include file type and fill-blanks toggle */
  form = XtVaCreateManagedWidget("read_file_form", xmFormWidgetClass,
				 read_reference_dialog,
				 XmNborderWidth,	0,
				 NULL);

  /* add a file type selection menu */
  file_type_menu = HGU_XmBuildMenu(form, XmMENU_OPTION,
				   "file_type", 0, XmTEAR_OFF_DISABLED,
				   file_type_menu_itemsP);
  XtVaSetValues(file_type_menu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		NULL);
  XtManageChild( file_type_menu );

  /* add the callbacks */
  image_type_cb( read_reference_dialog, (XtPointer) WLZEFF_FORMAT_WLZ,
		NULL);
  XtAddCallback( read_reference_dialog, XmNcancelCallback, 
		PopdownCallback, NULL);
  XtManageChild( read_reference_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( read_reference_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the write image dialog */
  export_image_dialog =
    XmCreateFileSelectionDialog(topl,
				"export_image_dialog", arg, 1);

  /* add a file type selection menu */
  file_type_menu = HGU_XmBuildMenu(export_image_dialog, XmMENU_OPTION,
				   "file_type", 0, XmTEAR_OFF_DISABLED,
				   write_file_type_menu_itemsP);
  XtManageChild( file_type_menu );

  /* add the callbacks */
  write_image_type_cb( export_image_dialog, (XtPointer) WLZEFF_FORMAT_WLZ,
		      NULL);
  XtAddCallback( export_image_dialog, XmNcancelCallback, 
		PopdownCallback, NULL);
  XtAddCallback(export_image_dialog, XmNmapCallback,
		FSBPopupCallback, NULL);
  XtManageChild( export_image_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( export_image_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* check for an initial reference file */
  if( initial_image_file != NULL ){
    FILE *fp;
    WlzObject 	*obj;

    /* open the reference object file and install */
    if( (fp = fopen(initial_image_file, "r")) ){
      image_menu_init(topl);
      HGU_XmSetHourGlassCursor(topl);
      if( obj = WlzReadObj( fp, NULL ) ){
	install_reference_object( obj );
      }
      (void) fclose( fp );

      /* set the title of the top-level window */
      set_topl_title(initial_image_file);
      globals.file = initial_image_file;

      HGU_XmUnsetHourGlassCursor(topl);
    }
  }

  return;
}
