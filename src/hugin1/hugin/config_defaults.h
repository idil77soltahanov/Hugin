// -*- c-basic-offset: 4 -*-
/** @file config_defaults.h
 *
 *  @author Pablo d'Angelo <pablo.dangelo@web.de>
 *
 *  $Id$
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _CONFIG_DEFAULTS_H
#define _CONFIG_DEFAULTS_H

// contains various configuration defaults

// assistant panel
#define HUGIN_ASS_NCONTROLPOINTS             20l
#define HUGIN_ASS_MAX_NORMAL_FOV            100.0
#define HUGIN_ASS_PANO_DOWNSIZE_FACTOR        0.7
#define HUGIN_ASS_AUTO_ALIGN                  1l
#define HUGIN_ASS_OPT_PHOTOMETRIC             1l

// template matching
#define HUGIN_FT_TEMPLATE_SIZE                21l
#define HUGIN_FT_SEARCH_AREA_PERCENT          10l
#define HUGIN_FT_LOCAL_SEARCH_WIDTH           14l
#define HUGIN_FT_CORR_THRESHOLD               0.8
#define HUGIN_FT_CURV_THRESHOLD               0.0

#define HUGIN_FT_ROTATION_SEARCH              0l
#define HUGIN_FT_ROTATION_START_ANGLE         -30.0
#define HUGIN_FT_ROTATION_STOP_ANGLE           30.0
#define HUGIN_FT_ROTATION_STEPS               12l


// Image cache defaults
#define HUGIN_IMGCACHE_UPPERBOUND             78643200
#define HUGIN_IMGCACHE_MAPPING_INTEGER        0l
#define HUGIN_IMGCACHE_MAPPING_FLOAT          1l
#define HUGIN_CP_CURSOR                       1

#define HUGIN_CAPTURE_TIMESPAN                60l

#define HUGIN_PREVIEW_SHOW_DRUID              1l
#define HUGIN_USE_SELECTED_IMAGES             0l
#define HUGIN_CROP_SETS_CENTER                0l

// GUI defaults
#define HUGIN_LANGUAGE                        wxLANGUAGE_DEFAULT
// sort by filename (1), sort by date (2)
#define HUGIN_GUI_SORT_NEW_IMG_ON_ADD         1l

#define HUGIN_LENS_ASSUME_SIMILAR             1l

// Custom Program paths
#define HUGIN_PT_MENDER_EXE_CUSTOM            false
#define HUGIN_ENBLEND_EXE_CUSTOM              false
#define HUGIN_APKOLOR_EXE_CUSTOM              false
#define HUGIN_APSIFT_EXE_CUSTOM               false


// Exiftool
#define HUGIN_EXIFTOOL_COPY_ARGS                   " -XMP:all -exif:all -ICC_Profile:all -Meta:all -GeoTiff:all --XResolution --YResolution --Rotation --Orientation --AutoRotate"

#define HUGIN_EXECDIALOG_ENABLED              1l

// Program defaults
#if defined WIN32

#define HUGIN_PT_SCRIPTFILE                   "PT_script.txt"

#define HUGIN_PT_MENDER_EXE                   "PTmender.exe"
#define HUGIN_PT_BLENDER_EXE                  "PTblender.exe"
#define HUGIN_PT_MASKER_EXE                   "PTmasker.exe"
#define HUGIN_PT_ROLLER_EXE                   "PTroller.exe"

#define HUGIN_SMARTBLEND_EXE                  "smartblend.exe"

#define HUGIN_ENBLEND_EXE                     "enblend.exe"
#define HUGIN_ENFUSE_EXE                     "enfuse.exe"

#define HUGIN_AP_TYPE                         0l
#define HUGIN_APKOLOR_EXE                     "autopano.exe"
#define HUGIN_APKOLOR_ARGS   "/allinone /path:%d /keys:%p /project:oto /name:%o /size:1024 /f %i"

#define HUGIN_APSIFT_EXE                      "autopano-win32.exe"
#define HUGIN_APSIFT_ARGS                     "--output %o --imagelist %namefile"

#define HUGIN_STITCHER_RUN_EDITOR             0l
#define HUGIN_STITCHER_EDITOR                 ""
#define HUGIN_STITCHER_EDITOR_ARGS            "%f"

#define HUGIN_STITCHER_TERMINAL               ""

#elif defined __WXMAC__

#define HUGIN_PT_SCRIPTFILE                   "PT_script.txt"

#define HUGIN_PT_MENDER_EXE                   "PTmender"
#define HUGIN_PT_BLENDER_EXE                  "PTblender"
#define HUGIN_PT_MASKER_EXE                   "PTmasker"
#define HUGIN_PT_ROLLER_EXE                   "PTroller"

#define HUGIN_SMARTBLEND_EXE                  "smartblend.exe"
#define HUGIN_ENBLEND_EXE                     "enblend"
#define HUGIN_ENFUSE_EXE                     "enfuse"

#define HUGIN_AP_TYPE                         1l
#define HUGIN_APKOLOR_EXE                     ""
#define HUGIN_APKOLOR_ARGS                    ""

#define HUGIN_APSIFT_EXE                      "autopano-complete.sh"
#define HUGIN_APSIFT_ARGS                     "-o %o -p %p %i"

#define HUGIN_STITCHER_RUN_EDITOR             0l
#define HUGIN_STITCHER_EDITOR                 ""
#define HUGIN_STITCHER_EDITOR_ARGS            "%f"

#define HUGIN_STITCHER_TERMINAL               ""

#elif defined unix

#define HUGIN_PT_SCRIPTFILE                   "PT_script.txt"

#define HUGIN_PT_MENDER_EXE                   "PTmender"
#define HUGIN_PT_BLENDER_EXE                  "PTblender"
#define HUGIN_PT_MASKER_EXE                   "PTmasker"
#define HUGIN_PT_ROLLER_EXE                   "PTroller"

#define HUGIN_SMARTBLEND_EXE                  "smartblend.exe"

#define HUGIN_ENBLEND_EXE                     "enblend"
#define HUGIN_ENFUSE_EXE                      "enfuse"

#define HUGIN_AP_TYPE                         1l
#define HUGIN_APKOLOR_EXE                     ""
#define HUGIN_APKOLOR_ARGS                    ""

#define HUGIN_APSIFT_EXE                      "autopano-complete.sh"
#define HUGIN_APSIFT_ARGS                     "--output %o --points %p %i"
#define HUGIN_STITCHER_RUN_EDITOR             0l
#define HUGIN_STITCHER_EDITOR                 "gimp-remote"
#define HUGIN_STITCHER_EDITOR_ARGS            "%f"

#define HUGIN_STITCHER_TERMINAL               "xterm -e "

#define HUGIN_EXECDIALOG_ENABLED2              0l

#endif

// enblend args
#define HUGIN_ENBLEND_ARGS                    "-v "
#define HUGIN_ENFUSE_ARGS                     "-v "

// smartblend args
#define HUGIN_SMARTBLEND_ARGS                 ""


#endif // _CONFIG_DEFAULTS_H
