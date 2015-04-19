// $Id: TemplatePropertiesControls.hh 1169 2005-12-12 15:22:15Z modesto $
//
//   FILE: TemplatePropertiesControls.hh -- 
// AUTHOR: patrik <patrik@modesto.cz>
//   DATE: 08 September 2004
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

#ifndef _LTEDITORTEMPLATEPROPERTIESCONTROLS_HH
#define _LTEDITORTEMPLATEPROPERTIESCONTROLS_HH

#include <qbrush.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qpen.h>
#include <qspinbox.h>
#include <qvalidator.h>

#include <lt/templates/Property.hh>
#include <lt/project/Panel.hh>

using namespace lt;

/**
 * Private QLineEdit derived control. Only difference is new signal that
 * emits also 'this' pointer along with changed value.
 */
class TemplateLineEdit : public QTextEdit
{
    Q_OBJECT
public:
    /*
     * ctor
     */
    TemplateLineEdit( const QString& k, const PropertyDict& pd,
            QWidget* parent, const char* name = 0 ) :
        QTextEdit( parent, name ), m_key(k), m_pd(pd)
    {
        setText(pd[k]->asString());

        connect( this, SIGNAL( textChanged() ),
                SLOT(textChangedSlot() ) );
        connect( this, SIGNAL( returnPressed() ),
                this, SLOT( textEditFinishedSlot() ) );
        setSizePolicy(
                QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    }

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const {
        return m_key;
    }

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& propKey ) {

        const Property* p = m_pd[m_key];
        if( propKey == m_key ) {
            if( p->asString() != text() ) {
                setText(p->asString());
            }
        }

        // enable/disable property
        setEnabled(p->isEnabled());
    }

private slots:
    /*
     * Helper slot to emit textChangedfrom textChanged with one more
     * parameter.
     */
    void textChangedSlot() {
        emit( textChanged( this, text() ) );
    }

    /*
     * Helper slot to emit lostFocus or returnPressed with one parameter.
     */
    void textEditFinishedSlot() {
        emit( textEditFinished(this) );
    }

private:
    /*
     * Property key
     */
    QString m_key;

    /**
     * PropertyDict
     */
    const PropertyDict& m_pd;

signals:
    /*
     * emited exactly the same as textChanged but adds new parameter 'this'.
     */
    void textChanged( TemplateLineEdit* , const QString& );

    /*
     * emited when returnPressed of lostFocus signal is catched. Not
     * additional parameter this.
     */
    void textEditFinished( TemplateLineEdit* );
};


/**
 * Private QLineEdit derived control for double/float values. Only
 * difference is new signal that emits also 'this' pointer along with
 * changed value.
 */
class TemplateDoubleEdit : public QLineEdit
{
    Q_OBJECT
public:
    /*
     * ctor for double validator
     */
    TemplateDoubleEdit( const QString& k, const PropertyDict& pd,
            QWidget* parent, const char* name = 0 ) :
        QLineEdit( parent, name ), m_key(k), m_pd(pd)
    {
        setValidator( new QDoubleValidator(this) );

        setText( QString::number(pd[k]->asFloatingPoint()) );

        connect( this, SIGNAL( returnPressed() ),
                this, SLOT( doubleEditFinishedSlot() ) );
        connect( this, SIGNAL( lostFocus() ),
                this, SLOT( doubleEditFinishedSlot() ) );
        setSizePolicy(
                QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    }

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const {
        return m_key;
    }

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot(const QString& propKey) {
        if( propKey == m_key ) {
            setText( QString::number(m_pd[m_key]->asFloatingPoint()) );
        }

        // enable/disable property
        setEnabled(m_pd[m_key]->isEnabled());
    }

private slots:
    /*
     * Helper slot to emit lostFocus or returnPressed with one parameter.
     */
    void doubleEditFinishedSlot() {
        emit( doubleEditFinished(this, text().toDouble()) );
    }

signals:
    /*
     * emited when returnPressed of lostFocus signal is catched. Not
     * additional parameter this.
     */
    void doubleEditFinished( TemplateDoubleEdit*, double );

private:
    /*
     * Property key
     */
    QString m_key;

    /**
     * PropertyDict
     */
    const PropertyDict& m_pd;
};


/**
 * Private QSpinBox derived control. Only difference is new signal that
 * emits also 'this' pointer along with changed value.
 */
class TemplateSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    /**
     * ctor
     */
    TemplateSpinBox( const QString& k, const PropertyDict& pd,
            int min, int max, QWidget* parent ) :
        QSpinBox( min, max, 1, parent ), m_key(k), m_pd(pd)
    {
        setValue(pd[k]->asInteger());
        connect( this, SIGNAL(valueChanged(int)),
                SLOT(valueChangedSlot(int)) );
        setSizePolicy(
                QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    }

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const {
        return m_key;
    }

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& propKey ) {

        const Property* p = m_pd[m_key];
        if( propKey == m_key ) {
            if( p->asString() != text() ) {
                setValue(p->asInteger());
            }
        }

        // enable/disable property
        setEnabled(p->isEnabled());
    }

private slots:
    /**
     * Helper slot to emit valueChanged( TemplateSpinBox*, int )
     */
    void valueChangedSlot( int v ) {
        emit( valueChanged( this, v ) );
    }

signals:
    /**
     * emited exactly the same as valueChanged(int) but with 'this'
     * parameter.
     */
    void valueChanged( TemplateSpinBox*, int );

private:
    /*
     * Property key
     */
    QString m_key;

    /**
     * PropertyDict
     */
    const PropertyDict& m_pd;
};


/**
 * TODO
 */
class TemplatePropertyPen : public QWidget
{
    Q_OBJECT
public:
    /**
     * ctor
     */
    TemplatePropertyPen( const QString& k, const PropertyDict& pd,
            QWidget* parent = 0, const char* name = 0 );

    virtual ~TemplatePropertyPen();

    /**
     * Setup the controls acording the given QPen
     */
    void updateFromPen( const QPen& p );

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const;

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& propKey );

private slots:
    virtual void widthValueChangedSlot( int val );
    void styleValueChanged( int );
    void colorButtonClicked();

signals:
    /**
     * TODO
     */
  void penPropertyChanged( TemplatePropertyPen*, const QPen& );
private:
    static int penStyleToIndex( Qt::PenStyle s );
    struct TemplatePropertyPenPrivate;
    TemplatePropertyPenPrivate* d;

};

/**
 * TODO
 */
class TemplatePropertyBrush : public QWidget
{
    Q_OBJECT

public:
    TemplatePropertyBrush( const QString& k, const PropertyDict& pd,
            QWidget* parent, const char* name = NULL );
    virtual ~TemplatePropertyBrush();

    /**
     * Setup the controls acording the given QBrush
     */
    void updateFromBrush( const QBrush& b );

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const;

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& propKey );

private slots:
    void colorButtonClicked();
    void styleValueChanged( int );

signals:
    /**
     * TODO
     */
    void brushPropertyChanged( TemplatePropertyBrush*,
            const QBrush& b );

private:
    static int brushStyleToIndex( Qt::BrushStyle s );

    struct TemplatePropertyBrushPrivate;
    TemplatePropertyBrushPrivate* d;

};

/**
 * TODO
 */
class TemplatePropertyFont : public QWidget
{
    Q_OBJECT

public:
    TemplatePropertyFont( const QString& k, const PropertyDict& pd,
            QWidget* parent, const char* name = NULL );
    virtual ~TemplatePropertyFont();

    /**
     * Setup the controls acording the given QFont
     */
    void updateFromFont( const QFont& f );

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const;

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& propKey );

private slots:
    void changeFont();

signals:
    /**
     * TODO
     */
    void fontPropertyChanged( TemplatePropertyFont*,
            const QFont& f );

private:

    struct TemplatePropertyFontPrivate;
    TemplatePropertyFontPrivate* d;
};

/**
 * Commont control based on QComboBox. Can be used for Choice, Link or
 * Resource Property type.
 */
class TemplatePropertyChoice : public QComboBox
{
    Q_OBJECT

public:
    /**
     * ctor for ChoiceType
     */
    TemplatePropertyChoice( const QString& k, const PropertyDict& pd,
            QWidget* parent ) :
        QComboBox( parent ), m_key(k), m_pd(pd)
    {
        setDuplicatesEnabled( false );
        setAutoCompletion( true );
        setSizePolicy(
                QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

        const Property* p = pd[k];
        Q_ASSERT(p);

        if( p->type() == Property::ChoiceType ) {
            setEditable(false);
            setInsertionPolicy( QComboBox::NoInsertion );

        } else if( p->type() == Property::LinkType ) {
            setEditable(true);
            setInsertionPolicy( QComboBox::AtTop );

        } else if( p->type() == Property::ResourceType ) {
            setEditable(true);
            setInsertionPolicy( QComboBox::AtTop );

        } else {
            Q_ASSERT(false);
        }

        connect( this, SIGNAL(activated(const QString&)),
                SLOT(itemActivatedSlot(const QString&)) );
    }

    /**
     * Updates the control acording the given Property
     */
    void updateFromChoiceProperty( const Property* p ) {
        Q_ASSERT(p);
        Q_ASSERT(p->type() == Property::ChoiceType);

        clear();
        insertStringList( p->asChoice() );
        setCurrentText( p->asString() );
    }

    /**
     * Updates the control acording the given Property
     */
    void updateFromLinkProperty( const Property* p, const PanelMap& pm ) {
        Q_ASSERT(p);
        Q_ASSERT(p->type() == Property::LinkType);

        fillFromPanelMap( pm );
        setCurrentText( p->asString() );
    }

    /**
     * Updates the control acording the given Property
     */
    void updateFromResourceProperty( const Property* p, const QStringList& ls )
    {
        Q_ASSERT(p);
        Q_ASSERT(p->type() == Property::ResourceType);

        fillFromResources(ls);
        setCurrentText( p->asString() );
    }

    /**
     * Fill combo from PanelMap
     */
    void fillFromPanelMap( const PanelMap& pm ) {

        QString c = currentText();

        clear();
        insertItem(""); // empty item to allow empty value
        for(unsigned i=0; i < pm.count(); ++i ) {
            insertItem( pm.keyAt(i) );
        }

        // set current text in case of we're called from TPD, ie without the
        // current value change
        setCurrentText(c);
    }

    /**
     * Fill combo from Resources
     */
    void fillFromResources( const QStringList& ls ) {

        QString c = currentText();

        clear();
        insertItem(""); // empty item to allow empty value
        insertStringList( ls );

        // set current text in case of we're called from TPD, ie without the
        // current value change
        setCurrentText(m_pd[m_key]->asResource());
    }

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const {
        return m_key;
    }

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& /*propKey*/ ) {

        const Property* p = m_pd[m_key];
        Q_ASSERT(p);

        if( p->asString() != currentText() ) {
            setCurrentText( p->asString());
        }

        // enable/disable property
        setEnabled(p->isEnabled());
    }

protected slots:
    /**
     * Called on item activated signal.
     */
    void itemActivatedSlot( const QString& s) {
        emit( itemActivated(this, s) );
    }

signals:
    /**
     * Emited on itemActivated signal, adds "this" parameter.
     */
    void itemActivated( TemplatePropertyChoice*, const QString& );

private:
    /*
     * Property key
     */
    QString m_key;

    /**
     * PropertyDict
     */
    const PropertyDict& m_pd;
};


/**
 * TODO
 */
class TemplatePropertyColor : public QWidget
{
    Q_OBJECT

public:
    TemplatePropertyColor( const QString& k, const PropertyDict& pd,
            QWidget* parent, const char* name = NULL );
    virtual ~TemplatePropertyColor();

    /**
     * Setup the controls acording the given QColor
     */
    void updateFromColor( const QColor& f );

    /**
     * Returns key of the Property this control handles.
     */
    const QString key() const;

public slots:
    /**
     * Called when any property from PropertyDict gets changed.
     */
    void updateSlot( const QString& propKey );

private slots:
    void changeColor();

signals:
    /**
     * TODO
     */
    void colorPropertyChanged( TemplatePropertyColor*,
            const QColor& );

private:

    struct TemplatePropertyColorPrivate;
    TemplatePropertyColorPrivate* d;
};

#endif /* _LTEDITORTEMPLATEPROPERTIESCONTROLS_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: TemplatePropertiesControls.hh 1169 2005-12-12 15:22:15Z modesto $
