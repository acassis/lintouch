// $Id: PluginHelper.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: PluginHelper.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 14 October 2003
//
// Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
// Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
//
// THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
//
// This application is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// This application is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this application; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef _PLUGINHELPER_HH
#define _PLUGINHELPER_HH

#ifdef _WIN32
#   define PLUGIN_EXPORT    __declspec(dllexport)
#else
#   define PLUGIN_EXPORT
#endif

#define LT_TEMPLATES_MAJOR_STR    "0"
#define LT_TEMPLATES_MINOR_STR    "0"
#define LT_TEMPLATES_PATCH_STR    "0"

/**
 * Place this macro on top of your c++ plugin implementation file to have
 * the extern C support functions automatically created. IMPLEMENTATION
 * defines the name of the class you are exporting via plugin. INTERFACESTR
 * is the string representation of the intarface your class implements.
 */
#define EXPORT_LT_PLUGIN(IMPLEMENTATION, INTERFACESTR) \
\
    static const char * version_info = \
    "interface="INTERFACESTR"," \
    "qt_version="QT_VERSION_STR"," \
    "lttemplates_major="LT_TEMPLATES_MAJOR_STR"," \
    "lttemplates_minor="LT_TEMPLATES_MINOR_STR"," \
    "lttemplates_patchlevel="LT_TEMPLATES_PATCH_STR"\0"; \
\
extern "C" { \
\
    PLUGIN_EXPORT IMPLEMENTATION * create() { \
        return new IMPLEMENTATION(); \
    } \
\
    PLUGIN_EXPORT const char * lt_version_info() { \
        return version_info; \
    } \
\
}

/**
 * You can call this function from your plugin loading routine to verify
 * that the plugin exports the class you want and that it has been compiled
 * against correct version of qt library and lttemplates library.
 */
bool verify_lt_plugin( const char * classnamestr,
        const char * versioninfo );

#endif /* _PLUGINHELPER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: PluginHelper.hh 1168 2005-12-12 14:48:03Z modesto $
