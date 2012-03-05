#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _MAOpenGLUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         MAOpenGLUtils.c
* \author	Richard Baldock
* \date		October 2001
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

static HGUglTextFont *threeDFont = NULL;
static GLdouble	initial_rot[4][4];

void MAOpenGLInitCb(
  Widget 	w, 
  XtPointer	clientData,
  XtPointer	callData)
{
  static String xFontStr = "-*-courier-*-*-*-*-14-*-*-*-*-*-*-*";
  Font		xFont;
  WlzDVertex3	tVtx, orgVtx;
  Display	*dpy = XtDisplay( w );
 
  (void )HGUglwCreateCanvasGlxContext(w, NULL);
  HGUglwCanvasGlxMakeCurrent(w, NULL);
  xFont = XLoadFont(XtDisplay(w), xFontStr);
  if(xFont)
  {
    threeDFont = HGUglTextFontCreate(xFont);
  }
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  if( globals.toplDepth == 24 ){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE, GL_LINEAR);
  }
  else {
    glClearIndex( (GLfloat) BlackPixelOfScreen(XtScreen(w)) );
  }

  /* set the initial transform - should be from resources */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotated( (GLdouble) 90.0, (GLdouble) 1.0, (GLdouble) 0.0, (GLdouble) 0.0);
  glRotated((GLdouble) -40.0, (GLdouble) 0.0, (GLdouble) 0.0, (GLdouble) 1.0);
  glRotated( (GLdouble) 20.0, (GLdouble) 0.6, (GLdouble) 0.4, (GLdouble) 0.0);
  glGetDoublev(GL_MODELVIEW_MATRIX, &initial_rot[0][0]);

  /* create the default 3D display  DisplayList */
  globals.ref_display_list = glGenLists( (GLsizei) 1 );
  glNewList( globals.ref_display_list, GL_COMPILE );
  WLZ_VTX_3_SET(tVtx, 10.0, 10.0, 10.0);
  WLZ_VTX_3_SET(orgVtx, 5.0, 5.0, 5.0);
  if( globals.toplDepth == 24 ){
    glColor3d((GLdouble) 1.0, (GLdouble) 1.0, (GLdouble) 1.0);
  }
  else {
    glIndexi( HGU_XGetColorPixel(dpy, globals.cmap, 1.0, 1.0, 1.0) );
  }
  HGUglShapeWireCube(tVtx, orgVtx);

  glEndList();

  WLZ_VTX_3_SET(globals.bbox_vtx, -2.0, -2.0, -2.0);
  WLZ_VTX_3_SET(globals.bbox_size, 14.0, 14.0, 14.0);

  return;
}

void MAOpenGLDrawScene(Widget canvasW)
{
  GLdouble	rot[4][4];
  WlzDVertex3	trans;
  Dimension	width, height;
  GLfloat	aspect, max_radius;
  GLdouble	theta;
  int		i;
  XEvent	event;

  /* check for queued timer events */
/*  if( XtAppPending(globals.app_con) & XtIMTimer )
  {
    fprintf(stderr, "ThreeDDrawScene: queued timer event\n");
    XtAppProcessEvent(globals.app_con, XtIMTimer);
    return;
  }*/

  max_radius = globals.bbox_size.vtX;
  max_radius = (globals.bbox_size.vtY > max_radius) ?
    globals.bbox_size.vtY : max_radius;
  max_radius = (globals.bbox_size.vtZ > max_radius) ?
    globals.bbox_size.vtZ : max_radius;

  XtVaGetValues(canvasW, XmNwidth, &width, XmNheight, &height, NULL);
  if((width > 0) && (height > 0))
  {
    HGUglwCanvasGlxMakeCurrent(canvasW, NULL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspect = (GLfloat) width / (GLfloat) height;
    theta = (GLdouble) height
      / 3.0 				/* pixels/mm */
      / 300.0; 				/* viewer dist. in mm */
    gluPerspective( (GLdouble) (theta * 180 / 3.1415962),
		    (GLdouble) aspect,
		    (GLdouble) max_radius / 10.0,
		    (GLdouble) 2 * max_radius / theta * 10.0);
    glTranslated(0.0,0.0,(GLdouble) -1.2 * max_radius / theta);
    
    /* this commented-out section enables orthographic projection */
    /*if(height > width)
    {
      aspect = (GLfloat )height/(GLfloat )width * max_radius;
      glOrtho(-max_radius, max_radius, -(aspect), aspect,
	      -max_radius, +max_radius);
    }
    else
    {
      aspect = (GLfloat )width/(GLfloat )height * max_radius;
      glOrtho(-(aspect), aspect, -max_radius, max_radius,
	      -max_radius, +max_radius);
    }*/
    /* set the fog parameters */
    glFogf(GL_FOG_START, 0.0);
    glFogf(GL_FOG_END, max_radius / theta / 2.0);

    HGUglwCanvasTbGetTranslate(canvasW, &trans);
    HGUglwCanvasTbGetRotateMatrixGL(canvasW, rot);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(trans.vtX*max_radius, trans.vtY*max_radius,
		 trans.vtZ*max_radius);
    glMultMatrixd(&rot[0][0]);
    glMultMatrixd(&initial_rot[0][0]);
    glTranslated((GLdouble) -(globals.bbox_vtx.vtX + globals.bbox_size.vtX/2),
		 (GLdouble) -(globals.bbox_vtx.vtY + globals.bbox_size.vtY/2),
		 (GLdouble) -(globals.bbox_vtx.vtZ + globals.bbox_size.vtZ/2));

    /* display the reference image sisplay list */
    glCallList( globals.ref_display_list );

    HGUglwCanvasSwapBuffers(canvasW);
  }
  glFinish();

  return;
}

