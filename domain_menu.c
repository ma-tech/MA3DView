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
*   File       :   domain_menu.c					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Mon Oct 22 13:49:59 2001				*
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
#include <string.h>

#include <MA3DView.h>

/* menu item structure */
static MenuItem domain_menu_itemsP[] = {		/* file_menu items */
  {"read_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   readDomainCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"write_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   writeDomainCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"write_all_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   writeAllDomainCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"", &xmSeparatorGadgetClass, 0, NULL, NULL, False,
   NULL, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"remove_domain", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   removeDomainCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

MenuItem	*domain_menu_items = &(domain_menu_itemsP[0]);
Widget		domain_controls_dialog=NULL;

static Widget	read_domain_dialog, read_tree_dialog, write_domain_dialog;

/* domain procedures */
#define MAXXPOINTS	128
static XPoint	xpoints[MAXXPOINTS];

void DisplayPoly(
  Display		*dpy,
  Window		win,
  GC			gc,
  WlzPolygonDomain	*poly)
{
  WlzIVertex2	*ivtxs;
  WlzFVertex2	*fvtxs;
  WlzDVertex2	*dvtxs;
  int		i, j;

  /* check the polygon */
  if( poly == NULL ){
    return;
  }

  /* copy to an X line list */
  switch( poly->type ){
  case WLZ_POLYGON_INT:
    ivtxs = poly->vtx;
    for(i=0, j=0; i < poly->nvertices; i++, j++, ivtxs++){
      xpoints[j].x = ivtxs->vtX;
      xpoints[j].y = ivtxs->vtY;
      if( j == (MAXXPOINTS - 1) ){
	XDrawLines(dpy, win, gc, xpoints, j+1, CoordModeOrigin);
	j=0;
	xpoints[j].x = ivtxs->vtX;
	xpoints[j].y = ivtxs->vtY;
      }
    }
    break;

  case WLZ_POLYGON_FLOAT:
    fvtxs = (WlzFVertex2 *) poly->vtx;
    for(i=0, j=0; i < poly->nvertices; i++, j++, fvtxs++){
      xpoints[j].x = WLZ_NINT(fvtxs->vtX);
      xpoints[j].y = WLZ_NINT(fvtxs->vtY);
      if( j == (MAXXPOINTS - 1) ){
	XDrawLines(dpy, win, gc, xpoints, j+1, CoordModeOrigin);
	j=0;
	xpoints[j].x = WLZ_NINT(fvtxs->vtX);
	xpoints[j].y = WLZ_NINT(fvtxs->vtY);
      }
    }
    break;

  case WLZ_POLYGON_DOUBLE:
    dvtxs = (WlzDVertex2 *) poly->vtx;
    for(i=0, j=0; i < poly->nvertices; i++, j++, dvtxs++){
      xpoints[j].x = WLZ_NINT(dvtxs->vtX);
      xpoints[j].y = WLZ_NINT(dvtxs->vtY);
      if( j == (MAXXPOINTS - 1) ){
	XDrawLines(dpy, win, gc, xpoints, j+1, CoordModeOrigin);
	j=0;
	xpoints[j].x = WLZ_NINT(dvtxs->vtX);
	xpoints[j].y = WLZ_NINT(dvtxs->vtY);
      }
    }
    break;

  default:
    return;
  }

  /* display the polyline */
  XDrawLines(dpy, win, gc, xpoints, j, CoordModeOrigin);
  XFlush( dpy );
  return;
}


void DisplayBound(
  Display	*dpy,
  Window	win,
  GC		gc,
  WlzBoundList	*bound)
{
  /* check boundary */
  if( bound == NULL ){
    return;
  }

  /* display the polyline */
  DisplayPoly(dpy, win, gc, bound->poly);

  /* display the next bound elements */
  DisplayBound(dpy, win, gc, bound->next);

  /* display the down bound elements */
  DisplayBound(dpy, win, gc, bound->down);

  return;
}

void DisplayBoundPiece(
  Display	*dpy,
  Window	win,
  GC		gc,
  WlzBoundList	*bound)
{
  /* check boundary */
  if( bound == NULL ){
    return;
  }

  /* display the polyline */
  if( bound->type == WLZ_BOUNDLIST_PIECE ){
    DisplayPoly(dpy, win, gc, bound->poly);
  }

  /* display the next bound elements */
  DisplayBoundPiece(dpy, win, gc, bound->next);

  /* display the down bound elements */
  DisplayBoundPiece(dpy, win, gc, bound->down);

  return;
}

void displayDomains(void)
{
  DomainListItem	*newItem;
  HGUDlpListItem	*item;
  WlzObject		*obj1;
  WlzErrorNum		errNum;

  /* check globals and domain list */
  if((globals.obj == NULL) ||
     (globals.dmnList == NULL) ||
     (HGUDlpListCount(globals.dmnList) <= 0)){
    return;
  }

  /* loop through domains and display the boundlist */
  item = HGUDlpListHead(globals.dmnList);
  while( item ){
    newItem = (DomainListItem *) HGUDlpListEntryGet(globals.dmnList, item);
    if( !newItem->currBound ){
      /* get the section thought the domain */
      if( newItem->obj ){
	if( obj1 = WlzGetSectionFromObject(newItem->obj, globals.wlzViewStr,
					   WLZ_INTERPOLATION_NEAREST,
					   &errNum) ){
	  WlzObject	*tmpBoundary;

	  obj1 = WlzAssignObject(obj1, NULL);
	  tmpBoundary = WlzObjToBoundary(obj1, 1, &errNum);
	  newItem->currBound =
	    WlzAssignObject(tmpBoundary, NULL);
	  WlzFreeObj(obj1);
	}
      }
    }
    /* display it */
    if( newItem->currBound ){
      if( obj1 ){
	if( newItem == globals.currDmnListItem ){
	  DisplayBoundPiece(globals.dpy, globals.win, globals.gc_blue,
			    obj1->domain.b);
	}
	else {
	  DisplayBoundPiece(globals.dpy, globals.win, globals.gc_red,
			    obj1->domain.b);
	}
	WlzFreeObj(obj1);
      }
    }
    item = HGUDlpListNext(globals.dmnList, item);
  }  

  return;
}

void freeDomainListItem(
  void		*item)
{
  DomainListItem	*dmnItem = (DomainListItem *) item;

  if( dmnItem->obj ){
    WlzFreeObj(dmnItem->obj);
  }
  if( dmnItem->currBound ){
    WlzFreeObj(dmnItem->currBound);
  }
  XmStringFree(dmnItem->listStr);
  free(dmnItem->file);
  AlcFree(item);

  return;
}

void clearDomainBoundaries( void )
{
  DomainListItem	*newItem;
  HGUDlpListItem	*item;

  /* loop through domains and display the boundlist */
  item = HGUDlpListHead(globals.dmnList);
  while( item ){
    newItem = (DomainListItem *) HGUDlpListEntryGet(globals.dmnList, item);
    if( newItem->currBound ){
      WlzFreeObj(newItem->currBound);
      newItem->currBound = NULL;
    }
    item = HGUDlpListNext(globals.dmnList, item);
  }  

  return;
}

void installNewDomain(
  WlzObject	*obj,
  char		*file,
  int		displayIndx)
{
  DomainListItem	*newItem, *matchItem=NULL;
  HGUDlpListItem	*item;
  int			i;
  WlzObject		*obj1;
  WlzErrorNum		errNum;

  /* check for repeats - need confirm to continue */
  obj = WlzAssignObject(obj, NULL);
  item = HGUDlpListHead(globals.dmnList);
  while( item ){
    newItem = (DomainListItem *) HGUDlpListEntryGet(globals.dmnList, item);
    if( strcmp(newItem->file, file) ){
      item = HGUDlpListNext(globals.dmnList, item);
    }
    else {
      /* may want to reset this object */
      if( !HGU_XmUserConfirm(globals.topl,
			     "This domain is already on installed.\n"
			     "Do you want to re-read the domain\n"
			     "to extend the existing region?\n"
			     "Note: to reset a domain then delete\n"
			     "it first then re-read.",
			     "Yes", "No", 1) ){
	WlzFreeObj(obj);
	return;
      }
      matchItem = newItem;
      break;
    }
  }

  /* remove overlaps with existing domains  - the new dominates */
  item = HGUDlpListHead(globals.dmnList);
  while( item ){
    newItem = (DomainListItem *) HGUDlpListEntryGet(globals.dmnList, item);
    if( strcmp(newItem->file, file) ){
      obj1 = WlzDiffDomain(newItem->obj, obj, &errNum);
      if( errNum == WLZ_ERR_NONE ){
	WlzFreeObj(newItem->obj);
	if( obj1 ){
	  newItem->obj = WlzAssignObject(obj1, NULL);
	}
	else {
	  newItem->obj = NULL;
	}
      }
    }
    item = HGUDlpListNext(globals.dmnList, item);
  }


  /* OK extend old or create a new list item */
  if( matchItem ){
    obj1 = WlzUnion2(matchItem->obj, obj, &errNum);
    if( errNum == WLZ_ERR_NONE ){
      WlzFreeObj(matchItem->obj);
      if( obj1 ){
	matchItem->obj = WlzAssignObject(obj1, NULL);
      }
      else {
	matchItem->obj = NULL;
      }
    }
  }
  else if( newItem = (DomainListItem *) AlcCalloc(sizeof(DomainListItem), 1) ){
    newItem->file = AlcStrDup(file);
    newItem->fileDisplayIndx = displayIndx;
    newItem->obj = WlzAssignObject(obj, NULL);
    newItem->listStr = XmStringCreateLocalized(newItem->file+displayIndx);

    /* add it to the domain list and domain list widget */
    (void) HGUDlpListInsert(globals.dmnList, NULL, newItem,
			    freeDomainListItem);
    XmListAddItem(globals.dmnListWidget, newItem->listStr, 0);
    XmListSelectItem(globals.dmnListWidget, newItem->listStr, True);
  }

  /* re-display the view */
/*  canvasExposeCb(globals.canvas, NULL, NULL);*/

  WlzFreeObj(obj);
  return;
}

/* action and callback procedures */
void readDomainCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  return;
}


void readDomainFileCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  return;
}


void writeDomainFileCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  return;
}

void writeDomainCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  return;
}

void writeAllDomainCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  return;
}

void removeDomainCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    return;
}

void domainListCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmListCallbackStruct	*cbs=(XmListCallbackStruct *) call_data;
  DomainListItem	*newItem;
  HGUDlpListItem	*item;

  /* find and set the selected domain */
  /* loop through domains and display the boundlist */
  globals.currDmnListItem = NULL;
  item = HGUDlpListHead(globals.dmnList);
  while( item ){
    newItem = (DomainListItem *) HGUDlpListEntryGet(globals.dmnList, item);
    if( XmStringCompare(newItem->listStr, cbs->item) == True ){
      globals.currDmnListItem = newItem;
      break;
    }

    item = HGUDlpListNext(globals.dmnList, item);
  }  

  return;
}


void domain_menu_init(
  Widget	topl)
{
  Widget	form, file_type_menu, toggle, widget;
  XmString	xmstr;
  Visual	*visual;
  Arg		arg[1];

  /* initialise the globals */
  globals.dmnList = HGUDlpListCreate(NULL);
  globals.currDmnListItem = NULL;
  globals.smoothItr = 1;

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(topl);
  XtSetArg(arg[0], XmNvisual, visual);

  /* create the read domain dialog */
  read_domain_dialog =
    XmCreateFileSelectionDialog(topl,
				"read_domain_dialog", arg, 1);

  /* add the callbacks */
  XtAddCallback(read_domain_dialog, XmNcancelCallback, PopdownCallback, NULL);
  XtManageChild(read_domain_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( read_domain_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  /* create the write domain dialog */
  write_domain_dialog =
    XmCreateFileSelectionDialog(topl,
				"write_domain_dialog", arg, 1);

  /* add the callbacks */
  XtAddCallback(write_domain_dialog, XmNokCallback,
		writeDomainFileCb, (XtPointer) WLZEFF_FORMAT_WLZ);
  XtAddCallback(write_domain_dialog, XmNokCallback, PopdownCallback, NULL);
  XtAddCallback(write_domain_dialog, XmNcancelCallback, PopdownCallback, NULL);
  XtManageChild( write_domain_dialog );

  /* add to the save restore list */
  HGU_XmSaveRestoreAddWidget( write_domain_dialog, HGU_XmFSD_SaveFunc,
			     (XtPointer) XtName(topl), NULL, NULL );

  return;
}
