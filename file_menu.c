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
*   $Revision$							*
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
#include <string.h>
#include <time.h>

#include <MA3DView.h>

/* general procedures and structures for managing a file list
   and file-list menu items as in "recent"
*/

#define WLZXMFILELIST_MAXNUMITEMS 15
void WlzXmFileListItemFree(
  void	*entry)
{
  WlzXmFileListCallbackStruct	*cbs;

  if( entry ){
    cbs = (WlzXmFileListCallbackStruct *) entry;
    if( cbs->file ){
      AlcFree(cbs->file);
    }
    AlcFree(cbs);
  }
  return;
}

WlzErrorNum WlzXmFileListClearList(
  AlcDLPList	*fileList)
{
  AlcDLPItem	*item;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( fileList ){
    item = fileList->head;
    while( item ){
      item = AlcDLPItemUnlink(fileList, item, 1, NULL);
    }
  }
  else {
    errNum = WLZ_ERR_PARAM_NULL;
  }

  return errNum;
}

AlcDLPList *WlzXmFileListCreateList(
  String	resourceFile,
  WlzErrorNum	*dstErr)
{
  AlcDLPList	*list=NULL;
  FILE		*fp;
  WlzXmFileListCallbackStruct	*cbs;
  int		index;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcErrno	alcErrno;

  /* check input parameters */
  if( resourceFile == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* create list */
  list = AlcDLPListNew(&alcErrno);

  /* read file to get menu items */
  if( errNum == WLZ_ERR_NONE ){
    if( fp = fopen(resourceFile, "r") ){
      BibFileRecord	*bibfileRecord;
      BibFileError	bibFileErr;

      /* search for file list entry */
      bibFileErr = BibFileRecordRead(&bibfileRecord, NULL, fp);
      while( bibFileErr == BIBFILE_ER_NONE ){

	/* create items and add to list */
	if( !strncmp(bibfileRecord->name, "WlzXmFileListFileRecord", 23) ){
	  cbs = (WlzXmFileListCallbackStruct *)
	    AlcMalloc(sizeof(WlzXmFileListCallbackStruct));
	  errNum = WlzEffBibParseFileRecord(bibfileRecord, &index,
					    &(cbs->file), &(cbs->format));
	  AlcDLPListEntryAppend(list, NULL, (void *) cbs,
				WlzXmFileListItemFree);
	}

	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, NULL, fp);
      }
    }
    else {
      errNum = WLZ_ERR_FILE_OPEN;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return list;
}

MenuItem *WlzXmFileListCreateMenuItems(
  AlcDLPList	*fileList,
  XtCallbackProc	callbackProc,
  WlzErrorNum	*dstErr)
{
  MenuItem	*items;
  int		i, p, numItems;
  AlcDLPItem	*listItem;
  
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcErrno	alcErrno;
  WlzXmFileListCallbackStruct	*cbs;
  /* check inputs */
  if( fileList == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* create menu */
  if( errNum == WLZ_ERR_NONE ){
    /* create menu items */
    numItems = AlcDLPListCount(fileList, &alcErrno);
    items = (MenuItem *) AlcMalloc(sizeof(MenuItem) * (numItems+4));

    /* set menu items */
    i = 0;
    listItem = fileList->head;
    while( listItem ){
      cbs = (WlzXmFileListCallbackStruct *) listItem->entry;

      /* copy in the required fields and generate short name
	 for the menu item */
      for(p=strlen(cbs->file)-1; p >= 0; p--){
	if( cbs->file[p] == '/' ){
	  break;
	}
      }
      items[i].name = (String) AlcMalloc(strlen((&(cbs->file[p+1]))) + 8);
      sprintf(items[i].name, "%d.  %s  ", i, &(cbs->file[p+1]));
      items[i].wclass = &xmPushButtonWidgetClass;
      items[i].mnemonic = '\0';
      items[i].accelerator = NULL;
      items[i].accel_text = NULL;
      items[i].item_set = False;
      items[i].callback = callbackProc;
      items[i].callback_data = (XtPointer) cbs;
      items[i].help_callback = NULL;
      items[i].help_callback_data = NULL;
      items[i].tear_off_model = XmTEAR_OFF_DISABLED;
      items[i].radio_behavior = False;
      items[i].always_one = False;
      items[i].subitems = NULL;

      i++;
      listItem = listItem->next;
      if( listItem == fileList->head ){
	listItem = NULL;
      }
    }

    /* add a separator */
    items[i].name = AlcStrDup("separator");
    items[i].wclass = &xmSeparatorWidgetClass;
    items[i].mnemonic = '\0';
    items[i].accelerator = NULL;
    items[i].accel_text = NULL;
    items[i].item_set = False;
    items[i].callback = NULL;
    items[i].callback_data = NULL;
    items[i].help_callback = NULL;
    items[i].help_callback_data = NULL;
    items[i].tear_off_model = XmTEAR_OFF_DISABLED;
    items[i].radio_behavior = False;
    items[i].always_one = False;
    items[i].subitems = NULL;
    i++;

    /* add button to clear the list */
    items[i].name = AlcStrDup("Clear list");
    items[i].wclass = &xmPushButtonWidgetClass;
    items[i].mnemonic = '\0';
    items[i].accelerator = NULL;
    items[i].accel_text = NULL;
    items[i].item_set = False;
    items[i].callback = callbackProc;
    items[i].callback_data = NULL;
    items[i].help_callback = NULL;
    items[i].help_callback_data = NULL;
    items[i].tear_off_model = XmTEAR_OFF_DISABLED;
    items[i].radio_behavior = False;
    items[i].always_one = False;
    items[i].subitems = NULL;
    i++;

    items[i].name = NULL;
  }

  /* assign error */
  if( dstErr ){
    *dstErr = errNum;
  }
  return items;
}

WlzErrorNum WlzXmFileListDestroyMenuItems(
  MenuItem	*items)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* check inputs */
  if( items == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else {
    for(i=0; items[i].name != NULL; i++){
      AlcFree(items[i].name);
    }
    AlcFree(items);
  }

  return errNum;
}

WlzErrorNum WlzXmFileListResetMenu(
  AlcDLPList	*fileList,
  Widget	cascade,
  XtCallbackProc	callbackProc)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	menu=NULL;
  MenuItem	*items;

  if( cascade ){
    XtVaGetValues(cascade, XmNsubMenuId, &menu, NULL);
    if( menu ){
      XtDestroyWidget(menu);
    }
    items = WlzXmFileListCreateMenuItems(globals.fileList,
					 callbackProc, NULL);
    menu = HGU_XmBuildPulldownMenu(cascade, XmTEAR_OFF_DISABLED,
				   False, False, items);
    WlzXmFileListDestroyMenuItems(items);
  }

  return errNum;
}

WlzErrorNum WlzXmFileListAddFile(
  AlcDLPList	*fileList,
  String	file,
  WlzEffFormat	format)
{
  WlzXmFileListCallbackStruct	*cbs;
  AlcDLPItem	*item;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcErrno	alcErr;

  /* check inputs */
  if((fileList == NULL) || (file == NULL)){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* create new item and add to head of the list */
  if( errNum == WLZ_ERR_NONE ){
    /* check if already in the list, in which case bring
       it to the top */
    item = fileList->head;
    while( item ){
      cbs = (WlzXmFileListCallbackStruct *) item->entry;
      if( !strcmp(file, cbs->file) ){
	break;
      }

      if( item->next == fileList->head ){
	item = NULL;
      }
      else {
	item = item->next;
      }
    }

    /* move or create new list item */
    if( item ){
      AlcDLPItemUnlink(fileList, item, 0, &alcErr);
      AlcDLPItemInsert(fileList, NULL, item);
    }
    else {
      cbs = (WlzXmFileListCallbackStruct *)
	AlcMalloc(sizeof(WlzXmFileListCallbackStruct));
      cbs->file = AlcStrDup(file);
      cbs->format = format;
      AlcDLPListEntryInsert(fileList, NULL, (void *) cbs,
			    WlzXmFileListItemFree);
    }
  }

  while( AlcDLPListCount(fileList, &alcErr) > WLZXMFILELIST_MAXNUMITEMS ){
    AlcDLPItemUnlink( fileList, fileList->head->prev, 1, &alcErr);
  }

  return errNum;
}

void WlzXmFileListWriteHeader(
  FILE	*fp)
{
  BibFileRecord		*bibfileRecord;
  time_t		tmpTime;
  char			*tmpS, tmpBuf[256];
  char  		*idxS = NULL;
  char  		*dateS = NULL;
  char  		*hostS = NULL;
  char  		*userS = NULL;
  static char		unknownS[] = "unknown";

  /* write some sort of identifier */
  bibfileRecord = 
    BibFileRecordMake("Ident", "0",
		      BibFileFieldMakeVa("Text",
					 "WlzXm Image File List",
					 "Version",	"1.0",
					 NULL));
  BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);

  /* now a comment with user, machine, date etc. */
  tmpS = getenv("USER");
  (void )sprintf(tmpBuf, "User: %s", tmpS?tmpS:unknownS);
  userS = AlcStrDup(tmpBuf);

  tmpTime = time(NULL);
  tmpS = ctime(&tmpTime);
  *(tmpS + strlen(tmpS) - 1) = '\0';
  (void )sprintf(tmpBuf, "Date: %s", tmpS?tmpS:unknownS);
  dateS = AlcStrDup(tmpBuf);

  tmpS = getenv("HOST");
  (void )sprintf(tmpBuf, "Host: %s", tmpS?tmpS:unknownS);
  hostS = AlcStrDup(tmpBuf);

  bibfileRecord = 
    BibFileRecordMake("Comment", "0",
		      BibFileFieldMakeVa("Text", userS,
					 "Text", dateS,
					 "Text", hostS,
					 NULL));
  BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);
  AlcFree(userS);
  AlcFree(dateS);
  AlcFree(hostS);

  return;
}

WlzErrorNum WlzXmFileListWriteResourceFile(
  AlcDLPList	*fileList,
  String	resourceFile)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcDLPItem	*item;
  WlzXmFileListCallbackStruct	*cbs;
  FILE		*fp;

  /* check inputs */
  if( (fileList == NULL) || (resourceFile == NULL) ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* open the file */
  if( errNum == WLZ_ERR_NONE ){

    /* currently assume only these entries are written to the
       resource file */
    if( fp = fopen(resourceFile, "w") ){
      /* write a file header */
      WlzXmFileListWriteHeader(fp);
      item = fileList->head;
      while( item ){
	cbs = (WlzXmFileListCallbackStruct *) item->entry;
	WlzEffBibWriteFileRecord(fp, "WlzXmFileListFileRecord",
				 cbs->file, cbs->format);
	item = item->next;
	if( item == fileList->head ){
	  item = NULL;
	}
      }
      fclose( fp );
    }
    else {
      errNum = WLZ_ERR_FILE_OPEN;
    }
  }

  return errNum;
}

WlzObject *WlzXmFileListReadObject(
  Widget			w,
  WlzXmFileListCallbackStruct	*cbs,
  WlzErrorNum			*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((w != NULL) && (cbs != NULL)){
    rtnObj = WlzEffReadObj(NULL, cbs->file, cbs->format, 0, &errNum);
  }
  else {
    errNum = WLZ_ERR_PARAM_NULL;
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}


/* menu item structure */
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
  {"Recent", &xmCascadeButtonWidgetClass, 0, NULL, NULL, False,
   NULL, NULL, HGU_XmHelpStandardCb, NULL,
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
  WlzEffFormat	image_type;
  FILE		*fp;
  String	icsfile;

  if( globals.obj == NULL ){
    return;
  }

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* write the reference object */
  if( globals.obj2D ){
    WlzObject *tmpObj;
    tmpObj = WlzMakeMain(WLZ_2D_DOMAINOBJ, globals.obj->domain.p->domains[0],
			 globals.obj->values.vox->values[0], NULL, NULL, NULL);
    WlzXmWriteExtFFObject(tmpObj, w, cbs, &image_type);
    WlzFreeObj(tmpObj);
  }
  else {
    WlzXmWriteExtFFObject(globals.obj, w, cbs, &image_type);
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
    if( globals.obj2D ){
      WlzFreeObj(globals.obj2D);
      globals.obj2D = NULL;
    }
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

WlzErrorNum checkReadReferenceObject(
  Widget	w,
  WlzObject	*obj)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* can do much better than this */

  /* check the object */
  if( obj == NULL ){
    HGU_XmUserError(w,
		    "Read Reference Object:\n"
		    "    No reference object read - either the\n"
		    "    selected file is empty or it is not a\n"
		    "    woolz object file - please check the file\n"
		    "    or make a new selection",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else {
    /* check object type */
    switch( obj->type ){
    case WLZ_2D_DOMAINOBJ:
    case WLZ_3D_DOMAINOBJ:
      /* check domain */
      if( obj->domain.core == NULL ){
	HGU_XmUserError(globals.topl,
			"Read Reference Object:\n"
			"    NULL domain in the reference object - some-\n"
			"    thing wrong. Please select an alternative\n"
			"    object",
			XmDIALOG_FULL_APPLICATION_MODAL);
	errNum = WLZ_ERR_VALUES_NULL;
      }
      /* check values */
      else if( obj->values.core == NULL ){
	HGU_XmUserError(globals.topl,
			"Read Reference Object:\n"
			"    The reference object must have a grey-\n"
			"    value table. Please select an alternative\n"
			"    object",
			XmDIALOG_FULL_APPLICATION_MODAL);
	errNum = WLZ_ERR_VALUES_NULL;
      }
      break;

    case WLZ_COMPOUND_ARR_1:
    case WLZ_COMPOUND_ARR_2:
      /* check sub-objects to see if they are plausibly
	 an rgb object */
      break;

    default:
      errNum = WLZ_ERR_OBJECT_TYPE;
      break;
    }
  } 

  return errNum;
}

void referenceFileListCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  WlzXmFileListCallbackStruct	*cbs=
    (WlzXmFileListCallbackStruct *) client_data;
  WlzObject	*obj;
  Widget	cascade;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  HGU_XmSetHourGlassCursor(globals.topl);
  if( cbs == NULL ){
    /* clear list selection */
    WlzXmFileListClearList(globals.fileList);
    WlzXmFileListWriteResourceFile(globals.fileList,
				   globals.resourceFile);
    if( cascade = XtNameToWidget(globals.topl,
				 "*file_menu*_pulldown*Recent") ){
      WlzXmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
    }
  }
  else if( obj = WlzXmFileListReadObject(w, cbs, &errNum) ){
    obj = WlzAssignObject(obj, NULL);

    /* install the new reference object */
    if( checkReadReferenceObject(globals.topl, obj) == WLZ_ERR_NONE ){
      install_reference_object( obj );

      /* set the title of the top-level dialog */
      set_topl_title(cbs->file);
      globals.file = cbs->file;

      /* add to the file list and write file */
      WlzXmFileListAddFile(globals.fileList, cbs->file, cbs->format);
      WlzXmFileListWriteResourceFile(globals.fileList,
				     globals.resourceFile);
      if( cascade = XtNameToWidget(globals.topl,
				   "*file_menu*_pulldown*Recent") ){
	WlzXmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
      }
    }
    WlzFreeObj(obj);
  }
  else {
    HGU_XmUserError(globals.topl,
		    "Read Reference Object:\n"
		    "    No reference object read - either the\n"
		    "    selected file is empty or it is not the\n"
		    "    correct object type - please check the\n"
		    "    file or make a new selection",
		    XmDIALOG_FULL_APPLICATION_MODAL);
  }
  HGU_XmUnsetHourGlassCursor(globals.topl);

  return;
}

void read_reference_object_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct *cbs =
    (XmFileSelectionBoxCallbackStruct *) call_data;
  WlzEffFormat	image_type;
  WlzObject	*obj;
  FILE		*fp;
  String	icsfile;
  Widget	cascade;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* set hour glass cursor */
  HGU_XmSetHourGlassCursor(globals.topl);

  /* read the new reference object */
  obj = WlzAssignObject(WlzXmReadExtFFObject(read_reference_dialog, cbs,
					     &image_type, &errNum),
			NULL);

  /* install the new reference object */
  if( checkReadReferenceObject(globals.topl, obj) == WLZ_ERR_NONE ){
    install_reference_object( obj );

    /* set the title of the top-level dialog */
    if( XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &icsfile) ){
      set_topl_title(icsfile);
      globals.file = icsfile;

      /* add to the file list and write file */
      WlzXmFileListAddFile(globals.fileList, icsfile, image_type);
      WlzXmFileListWriteResourceFile(globals.fileList,
				     globals.resourceFile);
      if( cascade = XtNameToWidget(globals.topl,
				   "*file_menu*_pulldown*Recent") ){
	WlzXmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
      }
    }
  }

  if( obj ){
    WlzFreeObj( obj );
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


/* file_menu initialisation procedure */
#define set_att_offset(field)   XtOffsetOf(ThreeDViewGlobals, field)

void file_menu_init(
  Widget	topl)
{
  Widget	form, toggle, cascade;
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
  read_reference_dialog = 
    WlzXmCreateExtFFObjectFSB(topl, "read_reference_dialog",
			      read_reference_object_cb,
			      NULL);
  WlzXmExtFFObjectFSBSetType(read_reference_dialog, WLZEFF_FORMAT_WLZ);
  XtManageChild( read_reference_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( read_reference_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the write image dialog */
  export_image_dialog =
    WlzXmCreateExtFFObjectFSB(topl, "export_image_dialog",
			      exportImageFileCb, NULL);
  WlzXmExtFFObjectFSBSetType(export_image_dialog, WLZEFF_FORMAT_WLZ);
  XtManageChild( export_image_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( export_image_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* add pulldown for the recent file-list */
  if( cascade = XtNameToWidget(globals.topl, "*file_menu*_pulldown*Recent") ){
    globals.resourceFile = (String) 
      AlcMalloc(sizeof(char) * (strlen(getenv("HOME")) + 16));
    sprintf(globals.resourceFile, "%s/%s", getenv("HOME"), ".ma3dviewrc");
    globals.fileList = WlzXmFileListCreateList(globals.resourceFile, NULL);
    WlzXmFileListResetMenu(globals.fileList, cascade, referenceFileListCb);
  }

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
