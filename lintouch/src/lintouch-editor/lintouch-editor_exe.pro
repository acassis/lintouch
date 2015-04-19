# $Id: lintouch-editor_exe.pro 1564 2006-04-07 13:33:15Z modesto $
#
#   FILE: lintouch-editor_exe.pro --
# AUTHOR: Martin Man <mman@swac.cz>
#   DATE: 12 July 2004
#
# Copyright (c) 2001-2006 S.W.A.C. GmbH, Germany.
# Copyright (c) 2001-2006 S.W.A.C. Bohemia s.r.o., Czech Republic.
#
# THIS SOFTWARE IS DISTRIBUTED AS IS AND WITHOUT ANY WARRANTY.
#
# This application is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This application is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this application; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

include(./config_unix.pri)

TEMPLATE     = app
TARGET = lintouch-editor

unix {
	target.path = $(bindir)
}
win32 {
	target.path = $(bindir)
	target.files = build/lintouch-editor.exe
}

locales.path = $(datadir)/lintouch/locale
locales.files = locale/*.qm
helpx.path = $(datadir)/lintouch/help/editor
helpx.files = help/*.html
helpxi.path = $(datadir)/lintouch/help/editor/images
helpxi.files = help/images/*.png

INSTALLS += target locales helpx helpxi

SOURCES	= \
		  lib/Bindings.cpp \
		  lib/BindingSerializer.cpp \
		  lib/BindingsHelper.cpp \
		  lib/ConnectionSerializer.cpp \
		  lib/CSV.cpp \
		  lib/DummyProjectBuilder.cpp \
		  lib/EditablePanelViewContainer.cpp \
		  lib/EditablePanelView.cpp \
		  lib/EditorProjectBuilder.cpp \
		  lib/EditorProject.cpp \
		  lib/EditorProjectSerializer.cpp \
		  lib/InfoSerializer.cpp \
		  lib/PanelSerializer.cpp \
		  lib/PluginManager.cpp \
		  lib/SelectionHandle.cpp \
		  lib/ServerLoggingBuilder.cpp \
		  lib/ServerLogging.cpp \
		  lib/ServerLoggingManager.cpp \
		  lib/ServerLoggingSerializer.cpp \
		  lib/TemplateSerializer.cpp \
		  lib/VariableSerializer.cpp \
		  lib/ViewSerializer.cpp \
		  src/AboutDialog.cpp \
		  src/AddDelConnectionCommand.cpp \
		  src/AddDelPanelCommand.cpp \
		  src/AddDelSelectionCommand.cpp \
		  src/AddDelServerLoggingCommand.cpp \
		  src/AddDelVariableCommand.cpp \
		  src/AddDelViewCommand.cpp \
		  src/AddResourceCommand.cpp \
		  src/ChangeBindingCommand.cpp \
		  src/Command.cpp \
		  src/ConnectionPropsChangedCommand.cpp \
		  src/CopyPasteBuffer.cpp \
		  src/DeleteResourceCommand.cpp \
		  src/HelpWindow.cpp \
		  src/ListViewToolTip.cpp \
		  src/main.cpp \
		  src/MainWindow.cpp \
		  src/MoveNResizeSelectionCommand.cpp \
		  src/MRU.cpp \
		  src/PanelOrderChangeCommand.cpp \
		  src/PanelZoomSelector.cpp \
		  src/PreferencesDialog.cpp \
		  src/ProjectConnectionPropertiesDialog.cpp \
		  src/ProjectConnectionsDialog.cpp \
		  src/ProjectGridSettingsDialog.cpp \
		  src/ProjectPanelsDialog.cpp \
		  src/ProjectPreviewDialog.cpp \
		  src/ProjectPreviewTable.cpp \
		  src/ProjectPropertiesDialog.cpp \
		  src/ProjectPropsChangeCommand.cpp \
		  src/ProjectResourcesDialog.cpp \
		  src/ProjectServerLoggingDialog.cpp \
		  src/ProjectServerLoggingPropertiesDialog.cpp \
		  src/ProjectTemplatesDialog.cpp \
		  src/ProjectVariablePropertiesDialog.cpp \
		  src/ProjectVariablesDialog.cpp \
		  src/ProjectViewResizeDialog.cpp \
		  src/ProjectViewsDialog.cpp \
		  src/ProjectWindow.cpp \
		  src/RenameConnectionCommand.cpp \
		  src/RenamePanelCommand.cpp \
		  src/RenameResourceCommand.cpp \
		  src/RenameServerLoggingCommand.cpp \
		  src/RenameTemplateCommand.cpp \
		  src/RenameVariableCommand.cpp \
		  src/RenameViewCommand.cpp \
		  src/ReorderViewCommand.cpp \
		  src/ResizeViewCommand.cpp \
		  src/SelectionPropsChangedCommand.cpp \
		  src/ServerLoggingPropsChangedCommand.cpp \
		  src/TemplateLibrariesDialog.cpp \
		  src/TemplateLibrarySelectionCombo.cpp \
		  src/TemplatePalette.cpp \
		  src/TemplatePropertiesControls.cpp \
		  src/TemplatePropertiesDialog.cpp \
		  src/TemplateShortcutChangeCommand.cpp \
		  src/TemplateTabOrderChangeCommand.cpp \
		  src/VariablePropsChangedCommand.cpp \

HEADERS	= \
		  lib/BindingSerializer.hh \
		  lib/BindingsHelper.hh \
		  lib/Bindings.hh \
		  lib/ConnectionSerializer.hh \
		  lib/CSV.hh \
		  lib/DummyProjectBuilder.hh \
		  lib/EditablePanelViewContainer.hh \
		  lib/EditablePanelView.hh \
		  lib/EditorProjectBuilder.hh \
		  lib/EditorProject.hh \
		  lib/EditorProjectSerializer.hh \
		  lib/InfoSerializer.hh \
		  lib/PanelSerializer.hh \
		  lib/PluginManager.hh \
		  lib/SelectionHandle.hh \
		  lib/ServerLoggingBuilder.hh \
		  lib/ServerLogging.hh \
		  lib/ServerLoggingManager.hh \
		  lib/ServerLoggingSerializer.hh \
		  lib/TemplateSerializer.hh \
		  lib/VariableSerializer.hh \
		  lib/ViewSerializer.hh \
		  src/AboutDialog.hh \
		  src/AddDelConnectionCommand.hh \
		  src/AddDelPanelCommand.hh \
		  src/AddDelSelectionCommand.hh \
		  src/AddDelServerLoggingCommand.hh \
		  src/AddDelVariableCommand.hh \
		  src/AddDelViewCommand.hh \
		  src/AddResourceCommand.hh \
		  src/ChangeBindingCommand.hh \
		  src/Command.hh \
		  src/CommandType.hh \
		  src/ConnectionPropsChangedCommand.hh \
		  src/CopyPasteBuffer.hh \
		  src/DeleteResourceCommand.hh \
		  src/HelpWindow.hh \
		  src/ListViewToolTip.hh \
		  src/MainWindow.hh \
		  src/MoveNResizeSelectionCommand.hh \
		  src/MRU.hh \
		  src/PanelOrderChangeCommand.hh \
		  src/PanelZoomSelector.hh \
		  src/PreferencesDialog.hh \
		  src/ProjectConnectionPropertiesControls.hh \
		  src/ProjectConnectionPropertiesDialog.hh \
		  src/ProjectConnectionsDialog.hh \
		  src/ProjectGridSettingsDialog.hh \
		  src/ProjectPanelsDialog.hh \
		  src/ProjectPreviewDialog.hh \
		  src/ProjectPreviewDialogPrivate.hh \
		  src/ProjectPreviewTable.hh \
		  src/ProjectPropertiesDialog.hh \
		  src/ProjectPropsChangeCommand.hh \
		  src/ProjectResourcesDialog.hh \
		  src/ProjectServerLoggingDialog.hh \
		  src/ProjectServerLoggingPropertiesDialog.hh \
		  src/ProjectTemplatesDialog.hh \
		  src/ProjectVariablePropertiesDialog.hh \
		  src/ProjectVariablesDialog.hh \
		  src/ProjectViewResizeDialog.hh \
		  src/ProjectViewsDialog.hh \
		  src/ProjectWindow.hh \
		  src/RenameConnectionCommand.hh \
		  src/RenamePanelCommand.hh \
		  src/RenameResourceCommand.hh \
		  src/RenameServerLoggingCommand.hh \
		  src/RenameTemplateCommand.hh \
		  src/RenameVariableCommand.hh \
		  src/RenameViewCommand.hh \
		  src/ReorderViewCommand.hh \
		  src/ResizeViewCommand.hh \
		  src/SelectionPropsChangedCommand.hh \
		  src/ServerLoggingPropsChangedCommand.hh \
		  src/TemplateBindingsControls.hh \
		  src/TemplateLibrariesDialog.hh \
		  src/TemplateLibrarySelectionCombo.hh \
		  src/TemplatePalette.hh \
		  src/TemplatePropertiesControls.hh \
		  src/TemplatePropertiesDialog.hh \
		  src/TemplateShortcutChangeCommand.hh \
		  src/TemplateTabOrderChangeCommand.hh \
		  src/VariablePropsChangedCommand.hh \

TRANSLATIONS = \
			   locale/editor_cs.ts \
			   locale/editor_de.ts \

FORMS = \
		src/AboutDialogBase.ui \
		src/HelpWindowBase.ui \
		src/MainWindowBase.ui \
		src/PreferencesDialogBase.ui \
		src/ProjectConnectionPropertiesDialogBase.ui \
		src/ProjectConnectionsDialogBase.ui \
		src/ProjectGridSettingsDialogBase.ui \
		src/ProjectPanelsDialogBase.ui \
		src/ProjectPreviewDialogBase.ui \
		src/ProjectPropertiesDialogBase.ui \
		src/ProjectResourcesDialogBase.ui \
		src/ProjectServerLoggingDialogBase.ui \
		src/ProjectServerLoggingPropertiesDialogBase.ui \
		src/ProjectTemplatesDialogBase.ui \
		src/ProjectVariablePropertiesDialogBase.ui \
		src/ProjectVariablesDialogBase.ui \
		src/ProjectViewResizeDialogBase.ui \
		src/ProjectViewsDialogBase.ui \
		src/ProjectWindowBase.ui \
		src/TemplateLibrariesDialogBase.ui \
		src/TemplatePropertiesDialogBase.ui \

IMAGES = \
		 src/images/magnify.xpm \
		 src/images/modify.xpm \
		 src/images/pan.xpm \
		 src/images/preview.xpm \
		 src/images/online.xpm \
  
RC_FILE = src/lintouch-editor.rc
 
QMAKE_LFLAGS += $${EXE_LFLAGS} $${TC_LFLAGS}

# vim: set noet ts=4 sw=4 tw=76 ft=make:
# $Id: lintouch-editor_exe.pro 1564 2006-04-07 13:33:15Z modesto $
