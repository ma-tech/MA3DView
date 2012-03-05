#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MA3DViewUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MA3DViewUtils.c
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

#include <stdio.h>
#include <stdlib.h>

#include <MA3DView.h>
#include <X11/cursorfont.h>

void popup_dialog_cb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
    XtManageChild( (Widget) client_data );
    XtPopup( XtParent((Widget) client_data), XtGrabNone );
    return;
}

void UnmanageChildCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  if( client_data != NULL ){
    XtUnmanageChild( (Widget) client_data );
  }
  else {
    XtUnmanageChild( w );
  }

  return;
}

void destroy_cb(
Widget          w,
XtPointer       client_data,
XtPointer       call_data)
{
    XtDestroyWidget( (Widget) client_data );
    return;
}
 
int InteractHelpCb(
  Display			*dpy,
  Window			win,
  XtPointer		client_data,
  String			help_str)
{
    HGU_XmHelpShowString( globals.topl, (String) client_data, help_str );
    return( 0 );
}

