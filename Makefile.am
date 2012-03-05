##
# \file         Makefile.am
# \author       Bill Hill
# \date         November 2007
# \version      $Id$
# \par
# Address:
#               MRC Human Genetics Unit,
#               MRC Institute of Genetics and Molecular Medicine,
#               University of Edinburgh,
#               Western General Hospital,
#               Edinburgh, EH4 2XU, UK.
# \par
# Copyright (C), [2012],
# The University Court of the University of Edinburgh,
# Old College, Edinburgh, UK.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be
# useful but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, write to the Free
# Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
# \brief        Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS		= \
		  	-I. \
			$(all_includes)

LDADD			= \
			-lXt -lXmu -lXp -lX11 -lXext \
			-lm -lSM -lICE -ldl -lpthread

bin_PROGRAMS		= \
			MA3DView

MA3DView_SOURCES       	= \
			MAColormapUtils.c \
			domain_menu.c \
			file_menu.c \
			image_menu.c \
			main_buttonbar.c \
			main_menubar.c \
			main_work_area.c \
			MA3DView.c \
			MA3DViewCropImage.c \
			MA3DViewUtils.c \
			MAOpenGLUtils.c \
			MAFilterImage.c

include_HEADERS         = \
			MA3DView.h \
			MA3DViewTypes.h \
			MA3DViewProto.h \
			MA3DViewHelp.h

MA3DView_LDADD          = $(LDADD)
MA3DView_LDFLAGS        = $(AM_LFLAGS)

