#ifndef MA3DVIEWRESOURCES_H
#define MA3DVIEWRESOURCES_H

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
*   File       :   MA3DViewResources.h					*
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Fri Oct 19 13:33:39 2001				*
*   $Revision$								*
*   $Name$								*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/
/* the following to be put in an include file */
/* fallback resources  */

static String  fallback_resources[] = {

  /* top-level geometry */
  "MA3DView.geometry:			960x600",

  /* fonts and backgrounds */
  "MA3DView*background:			grey85",
  "MA3DView*XmScrollBar.background:		grey95",
  "MA3DView*.fontList:	      -linotype-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*",
  "MA3DView*.XmText.fontList:		8x13",
  "MA3DView*.XmTextField.fontList:		8x13",
  "MA3DView*.XmList.fontList:		8x13",
  "MA3DView*.XmText.background:		grey95",
  "MA3DView*.XmTextField.background:		grey95",
  "MA3DView*.XmList.background:		grey95",
  "MA3DView*menubar.spacing: 		5",
  "MA3DView*menubar.XmCascadeButton.marginWidth: 1",

  /* widget and gadget class resources */
  "*XmToggleButtonGadget.selectColor:		red",
  "*XmToggleButtonGadget.visibleWhenOff:	True",
  "*XmToggleButtonGadget.indicatorSize:		14",
  "*XmToggleButton.selectColor:			red",
  "*XmToggleButton.visibleWhenOff:		True",
  "*XmToggleButton.indicatorSize:		14",
  "*XmFrameWidget.shadowThickness:		3",
  "*XmForm.horizontalSpacing:    		4",
  "*XmForm.verticalSpacing:      		4",
  "*XmForm.borderWidth:  			0",
  "*XmFrame.borderWidth:  			0",

  /* top level widget */
  "*canvas.width:			1024",
  "*canvas.height:			1024",

  /* work area */
  "*work_area*3Dcanvas.height:		300",
  "*work_area*domain_list_title.labelString:	Domain list:",
  "*work_area*view_frame_title.labelString:	Plane view:",
  "*work_area*view_form.b3.labelString:		mag+",
  "*work_area*view_form.b4.labelString:		mag-",
  "*work_area*view_form*view_mode.labelString:	view mode:",
  "*work_area*view_form*x_y.labelString:	x-y",
  "*work_area*view_form*y_z.labelString:	y-z",
  "*work_area*view_form*z_x.labelString:	z-x",
  "*work_area*view_form*any.labelString:	any",
  "*work_area*distance_slider.labelString: Distance:",
  "*work_area*mode_rc.borderWidth: 2",
  "*work_area*mode_rc.warp_paint_mode.labelString:	Mode:",
  "*work_area*mode_rc.warping.labelString:	Warping",
  "*work_area*mode_rc.painting.labelString:	Painting",
  "*work_area*domain_list.listSizePolicy:	XmCONSTANT",

  /* drawing area defaults */
  "MA3DView*HGU_DrawingArea.background:		white",

  /* command buttons */
  "*buttonbar*domain_ops_title.labelString:	Domain operations:",
  "*buttonbar.domain_ops_frame*new_domain.labelString:		New",
  "*buttonbar.domain_ops_frame*delete_domain.labelString:	Delete",
  "*buttonbar.domain_ops_frame*clear_domain.labelString:	Clear",
  "*buttonbar.domain_ops_frame*clear_all_domains.labelString:	Clear all",
  "*buttonbar.domain_ops_frame*smooth_domain.labelString:	Smooth",
  "*buttonbar.domain_ops_frame*smooth_all.labelString:		Smooth all",

  "*buttonbar*tp_ops_title.labelString:	Tie-point operations:",
  "*buttonbar.tp_ops_frame*reset_all*labelString:	Reset all",
  "*buttonbar.tp_ops_frame*delete_last*labelString:	Delete last",
  "*buttonbar.tp_ops_frame*delete_all*labelString:	Delete all",
  "*buttonbar.tp_ops_frame*confirm_warp*labelString:	Confirm warp",

  "*buttonbar*prop_ops_title.labelString:	Propogate operations:",
  "*buttonbar.prop_ops_frame*propogate_up.labelString:	Propogate up",
  "*buttonbar.prop_ops_frame*propogate_down.labelString:Propogate down",
  "*buttonbar.prop_ops_frame*auto_tp_match.labelString:Auto match",

  /* file dialogs */
  "*read_reference_dialog_popup.mappedWhenManaged:	False",
  "*read_reference_dialog.dialogTitle:			Read 2D Image",
  "*read_reference_dialog.pattern:			*.wlz",
  "*read_reference_dialog*file_type.labelString:	Select image type:",
  "*read_reference_dialog*file_type.borderWidth:	0",
  "*read_reference_dialog.directory:			./",
  "*read_domain_dialog_popup.mappedWhenManaged:		False",
  "*read_domain_dialog.dialogTitle:			Read domain",
  "*read_domain_dialog.pattern:				*.wlz",
  "*read_tree_dialog_popup.mappedWhenManaged:		False",
  "*read_tree_dialog.dialogTitle:			Read domain tree",
  "*read_tree_dialog.dirListLabelString:		Traverse directory",
  "*read_tree_dialog.fileListLabelString:		Select directory",
  "*read_tree_dialog.pattern:				[^.]*",
  "*write_domain_dialog_popup.mappedWhenManaged:	False",
  "*write_domain_dialog.dialogTitle:			Write domain",
  "*write_domain_dialog.pattern:			*.wlz",
  "*export_image_dialog_popup.mappedWhenManaged:	False",
  "*export_image_dialog.dialogTitle:			Export image",
  "*export_image_dialog.pattern:			*.wlz",
 
  /* file_menu */
  "*file_menu.labelString:             		File",
  "*file_menu.mnemonic:                		F",
  "*file_menu*read_reference.labelString:	Read reference...",
  "*file_menu*read_reference.mnemonic:		R",
  "*file_menu*write_reference.labelString:	Write reference...",
  "*file_menu*write_reference.mnemonic:		W",

  /* image menu */
  "*image_menu.labelString:             	Image",
  "*image_menu.mnemonic:                	I",
  "*image_menu*grey_mapping.labelString:	Grey mapping...",
  "*image_menu*grey_profile.labelString:	Grey profile...",
  "*image_menu*grey_stats.labelString:		Grey stats...",
  "*image_menu*remap_image.labelString:		Remap Image",
  "*image_menu*crop_image.labelString:		Crop...",

  /* image controls */
  "*image_controls_dialog*histogram_frame_title.labelString:Source histogram",
  "*image_controls_dialog*hist_magp.labelString: +",
  "*image_controls_dialog*hist_magn.labelString: -",
  "*image_controls_dialog*hist_reset.labelString: >o<",
  "*image_controls_dialog*histogram_graph*graphics.height:80",
  "*image_controls_dialog*transform_frame_title.labelString:Transform",
  "*image_controls_dialog*transform_graph*graphics.height:160",
  "*image_controls_dialog*grey_bounds_frame_title.labelString: Grey-level transform bounds",
  "*image_controls_dialog*src_grey_min.labelString: Source min",
  "*image_controls_dialog*src_grey_width.labelString: Source width",
  "*image_controls_dialog*dst_grey_min.labelString: Dest min",
  "*image_controls_dialog*dst_grey_max.labelString: Dest max",
  "*image_controls_dialog*transform_params_frame_title.labelString: Transform Parameters",
  "*image_controls_dialog*transformType.labelString: Transform type:",
  "*image_controls_dialog*gamma.labelString: Gamma",
  "*image_controls_dialog*mean.labelString: Mean",
  "*image_controls_dialog*sigma.labelString: Sigma",

  /* domain_menu */
  "*domain_menu.labelString:             	Domain",
  "*domain_menu.mnemonic:                	D",
  "*domain_menu*domain_controls.labelString:	Domain controls...",
  "*domain_menu*read_domain.labelString:	Read domain...",
  "*domain_menu*read_domain.mnemonic:		R",
  "*domain_menu*read_tree.labelString:		Read domain tree...",
  "*domain_menu*read_tree.mnemonic:		t",
  "*domain_menu*write_domain.labelString:	Write domain...",
  "*domain_menu*write_domain.mnemonic:		W",
  "*domain_menu*write_all_domain.labelString:	Save all domains",
  "*domain_menu*write_all_domain.mnemonic:	S",
  "*domain_menu*remove_domain.labelString:	Remove domain",
  "*domain_menu*remove_domain.mnemonic:		d",

  "*domain_controls_dialog.dialogTitle:		Domain controls dialog",
  "*domain_controls_dialog*smoothing_controls_title.labelString: Smoothing controls",
  "*domain_controls_dialog*smoothItr.labelString:	Smoothing:",

 /* misc menu */
  "*misc_menu.labelString:				Misc",
  "*misc_menu.mnemonic:		        		M",
  "*misc_menu*macro.labelString:			Macro",
  "*misc_menu*macro*start_record.labelString:		Start Record",
  "*misc_menu*macro*start_record.mnemonic:		S",
  "*misc_menu*macro*end_record.labelString:		End record",
  "*misc_menu*macro*end_record.mnemonic:		E",
  "*misc_menu*macro*replay.labelString:			Replay",
  "*misc_menu*macro*replay.mnemonic:			R",
  "*misc_menu*macro*loop.labelString:			Loop",
  "*misc_menu*macro*loop.mnemonic:			L",
  "*misc_menu*macro*quit_loop.labelString:		Quit loop",
  "*misc_menu*macro*quit_loop.mnemonic:			Q",
  "*misc_menu*save_restore.labelString:			Save/Restore",
  "*misc_menu*save_state.labelString:			Save State...",
  "*misc_menu*restore_state.labelString:		Restore...",
  "*misc_menu*save_restore*save_state.labelString:	Save State",
  "*misc_menu*save_restore*restore_state.labelString:	Restore",

  /* help_menu */
  "*help_menu.labelString:            	Help",
  "*help_menu.mnemonic:               	H",
  "*help_menu.borderWidth:            	0",
  "*help_menu*on_program.labelString:	MA3DView",
  "*help_menu*on_context.mnemonic:	P",
  "*help_menu*on_context.labelString:	On context",
  "*help_menu*on_context.mnemonic:	c",
  "*help_menu*on_windows.labelString:	On windows",
  "*help_menu*on_windows.mnemonic:	w",
  "*help_menu*on_keys.labelString:	On keys",
  "*help_menu*on_keys.mnemonic:		k",
  "*help_menu*on_keys.sensitive:	False",
  "*help_menu*index.labelString:	Index",
  "*help_menu*index.mnemonic:		I",
  "*help_menu*on_help.labelString:	On help",
  "*help_menu*on_help.mnemonic:		h",
  "*help_menu*tutorial.labelString:	Tutorial",
  "*help_menu*tutorial.mnemonic:	T",
  "*help_menu*tutorial.sensitive:	False",
  "*help_menu*version.labelString:	Version",
  "*help_menu*version.mnemonic:		V",
 

  NULL,
};


/* do not add anything after this line */
#endif /* MA3DVIEWRESOURCES_H */
