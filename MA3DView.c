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
*   File       :   MA3DView.c						*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 14:26:39 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <MA3DView.h>
#include <MA3DViewResources.h>

/* global variables */
ThreeDViewGlobals	globals;
char		*initial_image_file;

static String translations_table =
	"<Map>: InitialiseTopl()";

static void InitialiseTopl(
Widget		w,
XEvent		*event,
String		*params,
Cardinal	*num_params)
{
    Widget	widget;
    
    static	initialised=0;

    if( initialised )
	return;
    initialised = 1;

    /* initialise the work area */
    (void) init_main_work_area( w );

    /* initialise the menu bar */
    (void) init_main_menubar( w );

    /* initialise the button bar */
    (void) init_main_buttonbar( w );

    return;
}

int XNonFatalErrorHandler(
Display		*dpy,
XErrorEvent	*err_event)
{
  char *errStr, *str;

  errStr = (char *) AlcMalloc(sizeof(char)*80);
  str = (char *) AlcMalloc(sizeof(char)*512);

  XGetErrorText(dpy, err_event->error_code, errStr, 80);
  sprintf(str,
	  "An X11 error has occurred:\n\n"
	  "X Error of failed request: %s\n"
	  "Value in failed request: 0x%x\n"
	  "Major opcode of failed request: %d (%s)\n"
	  "Minor opcode of failed request: %d\n"
	  "Serial number of request: %d\n\n"
	  "do you want to save objects\n"
	  "before attempting to continue?",
	  errStr, err_event->resourceid,
	  err_event->request_code,
	  HGU_XRequestToString(err_event->request_code),
	  err_event->minor_code,
	  err_event->serial);

  if( HGU_XmUserConfirm(globals.topl, str,
			"Yes", "No", 1) ){
    /* blah blah blah action here */
  }

  AlcFree(str);
  AlcFree(errStr);
  return( 0 );
}

/* Warning - this is not consistent with the Xlib manual but
   as defined in Xlib.h
   */
int	XFatalErrorHandler(
Display		*dpy)
{
    if( HGU_XmUserConfirm(globals.topl,
			  "A fatal X11 error has occurred, do\n"
			  "you want to save domains\n"
			  "before quitting?",
			  "Yes", "No", 1) ){
      /* blah blah blah action here */
    }

    /* kill the help viewer */
/*    HGU_XmHelpDestroyViewer();*/

    XtDestroyWidget( globals.topl );
    exit( 1 );
    return 0;
}

void non_abort_signal_handler(int sig)
{
    signal( sig, SIG_IGN );
    quit_cb(globals.topl, (XtPointer) globals.topl, NULL);
    signal( sig, non_abort_signal_handler );
    return;
}

void abort_signal_handler(int sig)
{
    if( HGU_XmUserConfirm(globals.topl,
			  "Signal SIGSEGV (fatal) received,\n"
			  "do you want to blah blah blah?",
			  "Yes", "No", 1) )
    {
      /* blah blah blah action here */
    }

    if( HGU_XmUserConfirm(globals.topl,
			  "Save a core file?",
			  "Yes", "No", 1) ){
	abort();
    }
	
    exit( 1 );
}

static XtActionsRec actions[] = {
{"InitialiseTopl",	InitialiseTopl},
{NULL,			NULL},
};

main(
  int	argc,
  char	**argv)
{
  Widget		topl, main_w, buttonbar, menubar, work_area;
  Widget		message;
  XtAppContext		app_con;
  XtTranslations	translations;
  Atom			WM_DELETE_WINDOW;
  Display		*dpy;
  Visual		*visual;
  Colormap		colormap;
  Arg			arg[3];

  /* create the top level shell  - long hand but still doesn't work
     for the drag & drop in lists and text widgets */
  XtToolkitInitialize();
  app_con = XtCreateApplicationContext();
  XtAppSetFallbackResources(app_con, fallback_resources);
  dpy = XtOpenDisplay(app_con, NULL, NULL, "MA3Dview", NULL, 0,
		      &argc, argv);

  /* now check what visuals are available since we require 24-bit */
  if( !(visual = HGU_XGetVisual(dpy, DefaultScreen(dpy), TrueColor, 24)) ){
    fprintf(stderr, "%s: can't get required 24-bit visual\n", argv[0]);
    return 1;
  }
  /* also need to create and set the colormap */
  colormap = XCreateColormap(dpy, DefaultRootWindow(dpy), visual, AllocNone);
  XtSetArg(arg[0], XtNdepth, 24);
  XtSetArg(arg[1], XmNvisual, visual);
  XtSetArg(arg[2], XtNcolormap, colormap);

  /* finally create the top-level shell */
  topl = XtAppCreateShell(NULL, "MA3DView", applicationShellWidgetClass,
			  dpy, arg, 3);

  /* check for command line reference image */
  if( argc > 1 ){
    initial_image_file = argv[1];
  }
  else {
    initial_image_file = NULL;
  }

  /* add some additional translations to initialise the process when the
     top-level widget is mapped */
  translations = XtParseTranslationTable( translations_table );
  XtAugmentTranslations( topl, translations );

  globals.topl    = topl;
  globals.app_con = app_con;
  globals.toplVisual = visual;
  globals.cmap = colormap;
  globals.toplDepth = 24;

  /* initialise the save-restore facility */
  HGU_XmSaveRestoreInit( topl, &argc, argv );	
  HGU_XmSaveRestoreHelpCallback(topl, HGU_XmHelpStandardCb,
				"libhguXm/HGU_XmSaveRestore.html");

  /* create the motif main window */
  main_w = XtVaCreateManagedWidget("main_w", xmMainWindowWidgetClass,
				   topl,
				   XmNcommandWindowLocation,
				   XmCOMMAND_BELOW_WORKSPACE,
				   XmNscrollingPolicy, XmAPPLICATION_DEFINED,
				   XmNvisualPolicy, XmVARIABLE,
				   NULL);
  XtAddCallback(main_w, XmNhelpCallback,
		HGU_XmHelpStandardCb, NULL);

  /* create the menu bar */
  menubar = create_main_menubar( main_w );

  /* create the work area */
  work_area = create_main_work_area( main_w );

  /* create the command push buttons */
  buttonbar = create_main_buttonbar( main_w );

  /* create a message window */
  message = XtVaCreateManagedWidget("text", xmTextWidgetClass, main_w,
				 XmNeditable,		False,
				 XmNeditMode,		XmMULTI_LINE_EDIT,
				 XmNrows,		2,
				 XmNcursorPositionVisible,	False,
				 XmNautoShowCursorPosition,	False,
				 NULL);
  globals.message = message;
  XtVaSetValues(main_w,
		XmNmenuBar, menubar,
		XmNcommandWindow, buttonbar,
		XmNworkWindow, work_area,
		XmNshowSeparator, True,
		XmNmessageWindow, message,
		NULL);

  /* add application action procedures */
  XtAppAddActions(app_con, actions, XtNumber(actions));

  /* install signal and error handlers */
  signal(SIGHUP,  non_abort_signal_handler);
  signal(SIGINT,  non_abort_signal_handler);
  signal(SIGQUIT, abort_signal_handler);
  signal(SIGBUS,  abort_signal_handler);
  signal(SIGSEGV, abort_signal_handler);
  signal(SIGSYS,  abort_signal_handler);
  (void) XSetErrorHandler( XNonFatalErrorHandler );
  (void) XSetIOErrorHandler( XFatalErrorHandler );
    
  WM_DELETE_WINDOW = XmInternAtom(XtDisplay(topl),
				  "WM_DELETE_WINDOW", False);
  XmAddWMProtocols(topl, &WM_DELETE_WINDOW, 1);
  XmAddWMProtocolCallback(topl, WM_DELETE_WINDOW, quit_cb, (XtPointer) topl);

  XtRealizeWidget( topl );
  XtAppMainLoop( app_con );

  return( 0 );
}
