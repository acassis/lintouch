// $Id: TemplatePropertiesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplatePropertiesDialog.hh --
// AUTHOR: patrik <modesto@swac.cz>
//   DATE: 15 July 2004
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

#ifndef _LTEDITORTEMPLATEPROPERTIESDIALOG_HH
#define _LTEDITORTEMPLATEPROPERTIESDIALOG_HH

#include <lt/templates/Property.hh>
#include <lt/templates/Template.hh>

#include "CommandType.hh"
#include "TemplatePropertiesDialogBase.h"

#include <lt/LTMap.hh>

class MainWindow;
class ProjectWindow;

// helper classes of editable controls
class TemplateLineEdit;
class TemplateSpinBox;
class TemplatePropertyPen;
class TemplatePropertyBrush;
class TemplateDoubleEdit;
class TemplatePropertyFont;
class TemplatePropertyChoice;
class TemplatePropertyColor;
class TemplateBindingsCombobox;

class QListViewItem;

/**
 * TODO
 */
class TemplatePropertiesDialog :
    public TemplatePropertiesDialogBase
{

    friend class MainWindow;

    Q_OBJECT

public:
    /**
     * TODO
     */
    TemplatePropertiesDialog( MainWindow* parent,
            const char* name = 0, WFlags fl = Qt::WType_TopLevel );

    /**
     * TODO
     */
    virtual ~TemplatePropertiesDialog();

public slots:
    /**
     * TODO
     */
    void projectChangedSlot( ProjectWindow* pw,
            CommandType t );

protected slots:
    /**
     * Called when a tab is changed/activated.
     */
    void currentSelectionChangedSlot(QListViewItem* w);

protected slots:    // slots for editable controls
    /**
     * Special slot for QLineEdit change.
     */
    void textChangedSlot( TemplateLineEdit* , const QString& );
    void textEditFinishedSlot( TemplateLineEdit* );

    /**
     * Special slot for QSpinBox change.
     */
    void intChangedSlot( TemplateSpinBox*, int );

    /**
     * TemplateDoubleEdit helper slot.
     */
    void doubleEditFinishedSlot( TemplateDoubleEdit*, double );

    /**
     * TODO
     */
    void penPropertyChangedSlot( TemplatePropertyPen* tp,
            const QPen& p );

    /**
     * TODO
     */
    void brushPropertyChangedSlot( TemplatePropertyBrush* tb,
            const QBrush& b ); 

    /**
     * TODO
     */
    void fontPropertyChangedSlot( TemplatePropertyFont* tf,
            const QFont& b );

    /**
     * TODO
     */
    void colorPropertyChangedSlot( TemplatePropertyColor*,
            const QColor& );

    /**
     * Special slot for Choice property.
     */
    void choicePropertyChanged( TemplatePropertyChoice*,
            const QString& );

protected slots:    // bindings specific slots
    /**
     * Called when a connection combobox is changed.
     */
    void connectionComboboxChanged(TemplateBindingsCombobox*,
            const QString& val);

    /**
     * Called when a variable combobox is changed.
     */
    void variableComboboxChanged(TemplateBindingsCombobox*,
            const QString& val);

protected slots:    // General page specific slots
    /**
     * Called when a name field on the general page lost focus of return is
     * presses there.
     */
    void nameEditedSlot();

    /**
     * Called when a X position on the general page lost focus of return is
     * presses there.
     */
    void posXEditedSlot(int v);

    /**
     * Called when a Y position on the general page lost focus of return is
     * presses there.
     */
    void posYEditedSlot(int v);

    /**
     * Called when a width on the general page lost focus of return is
     * presses there.
     */
    void posWEditedSlot(int v);

    /**
     * Called when a height on the general page lost focus of return is
     * presses there.
     */
    void posHEditedSlot(int v);

    /**
     * Called when a layer on the general page lost focus of return is
     * presses there.
     */
    void posZEditedSlot(int v);

protected slots:    // General page specific methods
    /**
     * Create and MoveNResizeSelectionCommand command and exeute it
     *
     * \param rm Map of QRects with new position
     * \param lm Map of new layer values
     */
    void genMoveNResizeCommand(lt::LTMap<QRect> rm, lt::LTMap<double> lm);

protected:
    /**
     * Will refresh the dialog contents according to focused
     * ProjectWindow and its current selection.
     *
     * \return True if the selection contains at least one template
     */
    bool reconstructTabs(const lt::TemplateDict& td);

    /**
     * Create a tab filled with properties from PropertyDict.
     */
    QWidget* createTab( const QString& gr );

    /**
     * Create a tab filled with properties from PropertyDict.
     */
    QWidget* createIOTab( const QString& gr );

    /**
     * Create and execute SelectionPropsChangedCommand from given
     * Property key and new value
     */
    void executeCommand( const QString& key, const QString& value );

    /**
     * Update caption acording currently selected template.
     */
    void updateCaption();

    /**
     * Clear the list and widgetstack.
     */
    void clearAll();

private:
    struct TemplatePropertiesDialogPrivate;
    TemplatePropertiesDialogPrivate * d;

signals:
    /**
     * Emited when a property gets changed by a Undo Command. The key
     * parameter may be null in case of panel or resource
     * add/del/rename/reorder.
     *
     * \param key A key of the property that's changed. May be NULL!
     */
    void propertyChanged( const QString& key );

};

#endif /* _LTEDITORTEMPLATEPROPERTIESDIALOG_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePropertiesDialog.hh 1169 2005-12-12 15:22:15Z modesto $
