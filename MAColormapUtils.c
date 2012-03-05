#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAColormapUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAColormapUtils.c
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
 
void init_3DView_cmap(
Widget	w)
{
  Display		*dpy = XtDisplay( w );
  Colormap		cmap;
  unsigned long		plane_masks[1], free_pixels[256];
  unsigned int		n_free_pixels;
  unsigned long		min_required_val, max_required_val;
  XColor		colorcells[256];
  int			i, j;

  /* allocate the colours we need */
  n_free_pixels = 0;
  min_required_val = 64;
  max_required_val = 255;
  cmap = globals.cmap;

  for(i=0; i < 256; i++){
    
    if( XAllocColorCells(dpy, cmap, False, plane_masks,
			 0, &(free_pixels[n_free_pixels]), 1) == 0 )
      continue;
    
    /* check if required */
    if(free_pixels[n_free_pixels] >= min_required_val &&
       free_pixels[n_free_pixels] <= max_required_val){
      continue;
    } else {
      n_free_pixels++;
    }
  }

  /* free unwanted colorcells */
  XFreeColors(dpy, cmap, free_pixels, n_free_pixels, 0);


  /* set 64-127 to greys */
  for(i=0, j=64; j < 128; i++, j++){
    colorcells[i].red   = (j - 64) * 4 * 256;
    colorcells[i].green = (j - 64) * 4 * 256;
    colorcells[i].blue  = (j - 64) * 4 * 256;
    colorcells[i].pixel = j;
    colorcells[i].flags = DoRed | DoGreen | DoBlue;
  }

  /* set a load of colors for the remaining values
     so that the drawn polylines are visible */
  for(; j < 255; i++, j++){
    colorcells[i].red   = ((j%3)?0:255) * 256;
    colorcells[i].green = (((j+1)%3)?0:255) * 256;
    colorcells[i].blue  = (((j+2)%3)?0:255) * 256;
    colorcells[i].pixel = j;
    colorcells[i].flags = DoRed | DoGreen | DoBlue;
  }

  XStoreColors(dpy, cmap, colorcells, i);

  /* set a few extra colors */
  (void) HGU_XGetColorPixel(XtDisplay(w), cmap, 1.0, 0.0, 0.0);
  (void) HGU_XGetColorPixel(XtDisplay(w), cmap, 0.0, 1.0, 0.0);
  (void) HGU_XGetColorPixel(XtDisplay(w), cmap, 0.0, 0.0, 1.0);

  return;
}

void HGU_XmCreatePrivateColormap(Widget w)
{
  Display	*dpy;
  Window	win;
  Visual	*visual;
  Colormap	cmap, def_cmap;
  XColor	colorcell;
  unsigned long	plane_masks[1], pixels[256];
  unsigned int	n_pixels;
  int			i;

  /* create a private colormap */
  if( (dpy = XtDisplayOfObject( w )) == NULL )
    return;
  win = RootWindow( dpy, DefaultScreen( dpy ) );
  visual = HGU_XGetVisual(dpy, DefaultScreen( dpy ), PseudoColor, 8);
  if( visual == NULL ){
    visual = DefaultVisual( dpy, DefaultScreen( dpy ) );
  }
  cmap = XCreateColormap( dpy, win, visual, AllocNone );
  def_cmap = DefaultColormap( dpy, DefaultScreen( dpy ) );
    
  /* copy all existing colours */
  n_pixels = 0;
  colorcell.flags = DoRed|DoGreen|DoBlue;
  for(i=0; i < 256; i++){

    if( XAllocColorCells(dpy, cmap, False, plane_masks,
			 0, &(pixels[n_pixels]), 1) == 0 )
      continue;
    if( i >= 32 )	/* only free colours over pixel value 32 */
      n_pixels++;

    colorcell.pixel = i;
    XQueryColor(dpy, def_cmap, &colorcell);
    XStoreColor(dpy, cmap, &colorcell);
  }

  XFreeColors(dpy, cmap, pixels, n_pixels, 0);
    
  /* set the colormap as the widget resource */
  XtVaSetValues(w, XmNcolormap, cmap, NULL);
  globals.cmap = cmap;

  return;
}

void install_cmap_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    Widget			canvas = (Widget) client_data;
    XSetWindowAttributes	set_win_att;

    set_win_att.colormap = globals.cmap;
    XChangeWindowAttributes(XtDisplay(canvas), XtWindow(canvas),
			    CWColormap, &set_win_att);
    XInstallColormap(XtDisplay(canvas), globals.cmap);
    return;
}

unsigned long HGU_XGetColorPixel(
  Display	*dpy,
  Colormap	cmap,
  float		red,
  float		green,
  float		blue)
{
  XColor	colorcell_def;

  colorcell_def.red = (short) (red * 255.0) * 256;
  colorcell_def.green = (short) (green * 255.0) * 256;
  colorcell_def.blue = (short) (blue * 255.0) * 256;
  colorcell_def.flags = DoRed | DoGreen | DoBlue;

  XAllocColor(dpy, cmap, &colorcell_def);
  return( colorcell_def.pixel );
}

