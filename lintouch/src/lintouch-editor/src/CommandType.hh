// $Id: CommandType.hh 1564 2006-04-07 13:33:15Z modesto $
//
//   FILE: CommandType.hh -- 
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 26 July 2004
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

#ifndef _LTEDITORCOMMANDTYPE_HH
#define _LTEDITORCOMMANDTYPE_HH

/**
 * TODO
 */
typedef enum CommandType {

    /**
     * NOOP
     */
    LTE_CMD_NOOP = 0,

    /**
     * Project
     */
    LTE_CMD_PROJECT_PROPS_CHANGE = 1,

    /**
     * Views
     */
    LTE_CMD_VIEW_ADD_DEL,       // RTTI only
    LTE_CMD_VIEW_ADD,
    LTE_CMD_VIEW_DELETE,
    LTE_CMD_VIEW_RENAME,
    LTE_CMD_VIEW_SELECT,
    LTE_CMD_VIEW_RESIZE,
    LTE_CMD_VIEW_ORDER_CHANGE,

    /**
     * Panels
     */
    LTE_CMD_PANEL_ADD_DEL,      // RTTI only
    LTE_CMD_PANEL_ADD,
    LTE_CMD_PANEL_DELETE,
    LTE_CMD_PANEL_RENAME,
    LTE_CMD_PANEL_SELECT,
    LTE_CMD_PANEL_ORDER_CHANGE,

    /**
     * Selection
     */
    LTE_CMD_TEMPLATE_TABORDER_CHANGE,
    LTE_CMD_TEMPLATE_SHORTCUT_CHANGE,
    LTE_CMD_TEMPLATE_RENAME,

    /**
     * Selection
     */
    LTE_CMD_SELECTION_CHANGE,
    LTE_CMD_SELECTION_ADD_DEL,  // RTTI only
    LTE_CMD_SELECTION_ADD,
    LTE_CMD_SELECTION_DEL,
    LTE_CMD_SELECTION_MOVE_RESIZE,
    LTE_CMD_SELECTION_PROPS_CHANGE,

    /**
     * Resources
     */
    LTE_CMD_RESOURCE_ADD,
    LTE_CMD_RESOURCE_DELETE,
    LTE_CMD_RESOURCE_RENAME,

    /**
     * Preview
     */
    LTE_CMD_IOPINS_CHANGE,

    /**
     * Grid Settings
     */
    LTE_CMD_GRID_PROPS_CHANGE,

    /**
     * Bindings
     */
    LTE_CMD_BINDING_CHANGE,

    /**
     * Connections
     */
    LTE_CMD_CONNECTION_ADD_DEL,     // RTTI only
    LTE_CMD_CONNECTION_ADD,
    LTE_CMD_CONNECTION_DEL,
    LTE_CMD_CONNECTION_RENAME,
    LTE_CMD_CONNECTION_PROPS_CHANGE,

    /**
     * Variables
     */
    LTE_CMD_VARIABLE_ADD_DEL,     // RTTI only
    LTE_CMD_VARIABLE_ADD,
    LTE_CMD_VARIABLE_DEL,
    LTE_CMD_VARIABLE_RENAME,
    LTE_CMD_VARIABLE_PROPS_CHANGE,

    /**
     * ServerLogging
     */
    LTE_CMD_SERVERLOGGING_ADD_DEL,     // RTTI only
    LTE_CMD_SERVERLOGGING_ADD,
    LTE_CMD_SERVERLOGGING_DEL,
    LTE_CMD_SERVERLOGGING_RENAME,
    LTE_CMD_SERVERLOGGING_PROPS_CHANGE,

} CommandType ;

#endif /* _LTEDITORCOMMANDTYPE_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: CommandType.hh 1564 2006-04-07 13:33:15Z modesto $
