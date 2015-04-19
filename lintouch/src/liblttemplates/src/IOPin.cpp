// $Id: IOPin.cpp 1774 2006-05-31 13:17:16Z hynek $
//
//   FILE: IOPin.cpp -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 03 October 2003
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

#include "lt/variables/io.h"

#include "lt/templates/Connection.hh"
#include "lt/templates/IOPin.hh"
#include "lt/templates/Template.hh"
#include "lt/templates/Variable.hh"
using namespace lt;

// macros to save some typing
#define SET_VAL_IF_DIRTY(var,val,flag) \
if( ! d->types.contains( \
           (IOPin::Type)lt_io_value_type_get( val ) ) ) return; \
if( ! lt_io_value_equals((var),(val)) ) { \
    apr_pool_t * pool = lt_io_value_pool_get((var)); \
   (flag) = 1; \
    apr_pool_clear( pool ); \
   (var) = lt_io_value_deepcopy((val), pool );\
}

#define SET_BIT_IF_DIRTY(var,val,flag) \
if( ! d->types.contains( IOPin::BitType ) ) return; \
if( lt_io_value_type_get((var)) != LT_IO_TYPE_BIT || \
       ((val) & 0x1 ) != lt_io_value_bit_get((var))) { \
   (flag) = 1; \
    lt_io_value_bit_set((var),(val)); \
}

#define SET_NUM_IF_DIRTY(var,val,flag) \
if( ! d->types.contains( IOPin::NumberType ) ) return; \
if( lt_io_value_type_get((var)) != LT_IO_TYPE_NUMBER || \
       ((val) ) != lt_io_value_number_get((var))) { \
   (flag) = 1; \
    lt_io_value_number_set((var),(val)); \
}

//this macro uses QString::operator==
#define SET_STR_IF_DIRTY(var,val,flag) \
if( ! d->types.contains( IOPin::StringType ) ) return; \
if( lt_io_value_type_get((var)) != LT_IO_TYPE_STRING || \
       ((val) ) != (const char *)lt_io_value_string_get((var))) { \
    (flag) = 1; \
    QCString tmp =((val).utf8()); \
    lt_io_value_string_set((var), \
            (const uint8_t*)tmp.data(), tmp.length() ); \
}

struct _Types
{
    IOPin::Type type;
    char * representation;
};

static const _Types _types [] = {
    { IOPin::BitType, "bit" },
    { IOPin::NumberType, "number" },
    { IOPin::StringType, "string" },
    { IOPin::SequenceType, "sequence" },
    { IOPin::InvalidType, "" },
};

struct IOPin::IOPinPrivate
{

    IOPinPrivate() :
        boundTo( NULL ),
        parent( NULL )
    {
        real_value = lt_io_value_create( NULL );
        requested_value = lt_io_value_create( NULL );
        real_value_dirty = requested_value_dirty = 0;
    }

    ~IOPinPrivate()
    {
        apr_pool_destroy( lt_io_value_pool_get( real_value ) );
        apr_pool_destroy( lt_io_value_pool_get( requested_value ) );
    }

    void resetValues( IOPin::Type type )
    {
#define d this
        switch( type ) {
            case IOPin::BitType:
                SET_BIT_IF_DIRTY( real_value, 0, real_value_dirty );
                SET_BIT_IF_DIRTY( requested_value, 0,
                        requested_value_dirty );
                break;
            case IOPin::NumberType:
                SET_NUM_IF_DIRTY( real_value, 0, real_value_dirty );
                SET_NUM_IF_DIRTY( requested_value, 0,
                        requested_value_dirty );
                break;
            case IOPin::StringType:
                SET_STR_IF_DIRTY( real_value, QString( "" ), real_value_dirty );
                SET_STR_IF_DIRTY( requested_value, QString( "" ),
                        requested_value_dirty );
                break;
            default:
                break;
        }
#undef d
    }

    // localized label
    QString label;

    // list of variable types we can be bound to
    IOPin::TypeList types;

    // real and requested value
    lt_io_value_t * real_value;
    lt_io_value_t * requested_value;

    bool real_value_dirty;
    bool requested_value_dirty;

    // the variable we are bound to
    Variable * boundTo;

    //template who owns us
    Template * parent;
};

IOPin::IOPin( Template * parent, const QString & label, IOPin::Type type ) :
    d( new IOPinPrivate() )
{
    d->parent = parent;
    d->label = label;
    d->types.append( type );

    // initialize real + req value to correct default value according to
    // IOPin type
    d->resetValues( type );
}

IOPin::IOPin( Template * parent, const QString & label,
        const QString & types ) :
    d( new IOPinPrivate() )
{
    d->parent = parent;
    d->label = label;
    d->types = IOPin::decodeTypes( types );

    // initialize real + req value to correct default value according to
    // IOPin type
    if( !d->types.isEmpty() ) {
        d->resetValues( d->types.first() );
    }
}

IOPin::~IOPin()
{
    // break any existing connections
    unbind();

    delete d;
}

Template * IOPin::parent() const
{
    return d->parent;
}

IOPin::Type IOPin::decodeType( const QString & type )
{
    QString s = type.stripWhiteSpace().lower();

    // does the piece match one of our types ???
    for( unsigned i = 0; 
            _types[ i ].type != IOPin::InvalidType; i ++ ) {

        if( s.find( _types[ i ].representation ) == 0 ) {
            return _types[ i ].type;
        }
    }
    return IOPin::InvalidType;
}

IOPin::TypeList IOPin::decodeTypes( const QString & types )
{
    // if not specified, anything can connect to us
    if( types.stripWhiteSpace().isEmpty() ) {
        return IOPin::supportedTypesAsTypeList();
    }

    IOPin::Type t;
    IOPin::TypeList r;

    // split into pieces
    QStringList l = QStringList::split( ',', types );

    // try to match each piece
    for( QStringList::const_iterator it =
            const_cast<QStringList&>(l).begin();
            it != const_cast<QStringList&>(l).end();
            ++ it ) {

        t = IOPin::decodeType( *it );
        // when at least one is invalid, we return empty list
        if( t == IOPin::InvalidType ) return IOPin::TypeList();

        r.append( t );
    }

    return r;
}

QString IOPin::encodeTypes( IOPin::TypeList types )
{
    QString r;

    // for each type
    for( IOPin::TypeList::const_iterator it =
            const_cast<IOPin::TypeList&>(types).begin(); 
            it != const_cast<IOPin::TypeList&>(types).end(); ++ it ) {

        // does the type match one of our types ????
        for( unsigned i = 0; 
                _types [ i ].type != IOPin::InvalidType; i ++ ) {

            if(( * it ) == _types [ i ].type ) {
                if( ! r.isEmpty() ) r += ", ";
                r += _types [ i ].representation;
            }
        }
    }

    return r;
}

QStringList IOPin::supportedTypesAsStringList()
{
    static QStringList supportedTypes;
    static bool initialized = 0;

    if( ! initialized ) {
        initialized = 1;
        for( unsigned i = 0; 
                _types [ i ].type != IOPin::InvalidType; i ++ ) {
            supportedTypes.append( _types [ i ].representation );
        }
    }

    return supportedTypes;
}

IOPin::TypeList IOPin::supportedTypesAsTypeList()
{
    static IOPin::TypeList supportedTypes;
    static bool initialized = 0;

    if( ! initialized ) {
        initialized = 1;
        for( unsigned i = 0; 
                _types [ i ].type != IOPin::InvalidType; i ++ ) {
            supportedTypes.append( _types [ i ].type );
        }
    }

    return supportedTypes;
}

bool IOPin::isValid() const
{
    // if at least one type is supported we are valid
    return d->types.count() > 0;
}

QString IOPin::label() const
{
    return d->label;
}

IOPin::TypeList IOPin::types() const
{
    return d->types;
}

void IOPin::setRealBitValue( bool bit )
{
    SET_BIT_IF_DIRTY(d->real_value, bit, d->real_value_dirty);
}

void IOPin::setRealNumberValue( int num )
{
    SET_NUM_IF_DIRTY(d->real_value, num, d->real_value_dirty);
}

void IOPin::setRealStringValue( const QString & str )
{
    SET_STR_IF_DIRTY(d->real_value, str, d->real_value_dirty);
}

void IOPin::setRealValue( const lt_io_value_t * val )
{
    SET_VAL_IF_DIRTY(d->real_value, val, d->real_value_dirty);
}

IOPin::Type IOPin::realValueType() const
{
    return ( IOPin::Type )lt_io_value_type_get( d->real_value );
}

bool IOPin::realBitValue() const
{
    return lt_io_value_bit_get( d->real_value );
}

int IOPin::realNumberValue() const
{
    return lt_io_value_number_get( d->real_value );
}

QString IOPin::realStringValue() const
{
    return QString::fromUtf8(
           (const char*)lt_io_value_string_get( d->real_value ) );
}

bool IOPin::isRealValueDirty() const
{
    return d->real_value_dirty;
}

void IOPin::syncRealValue()
{
    // we only clear dirty flag
    d->real_value_dirty = 0;
}

void IOPin::setRequestedBitValue( bool bit ) {
    SET_BIT_IF_DIRTY(d->requested_value, bit, d->requested_value_dirty);
}

void IOPin::setRequestedNumberValue( int num ) {
    SET_NUM_IF_DIRTY(d->requested_value, num, d->requested_value_dirty);
}

void IOPin::setRequestedStringValue( const QString & str ) {
    SET_STR_IF_DIRTY(d->requested_value, str, d->requested_value_dirty);
}

void IOPin::setRequestedValue( const lt_io_value_t * val )
{
    SET_VAL_IF_DIRTY(d->requested_value, val, d->requested_value_dirty);
}

IOPin::Type IOPin::requestedValueType() const
{
    return ( IOPin::Type )lt_io_value_type_get( d->requested_value );
}

bool IOPin::requestedBitValue() const
{
    return lt_io_value_bit_get( d->requested_value );
}

int IOPin::requestedNumberValue() const
{
    return lt_io_value_number_get( d->requested_value );
}

QString IOPin::requestedStringValue() const {
    return QString::fromUtf8(
           (const char*)lt_io_value_string_get( d->requested_value ) );
}

bool IOPin::isRequestedValueDirty() const {
    return d->requested_value_dirty;
}

void IOPin::syncRequestedValue()
{
    // we will change directly the requested value of the variable we are
    // bound to

    // check that we are actually bound to someone and that we are dirty
    if( d->requested_value_dirty && d->boundTo != NULL ) {
        lt_var_t * var = d->boundTo->m_var;

        // we transfer our value to the varset directly
        lt_var_requested_value_set( var, d->requested_value );

    }
    d->requested_value_dirty = 0;
}

void IOPin::bindToVariable( Variable * other )
{
    // first break any existing connections
    unbind();

    // and if valid var has been specified, create new binding
    if( other == NULL ) return;

    // check that the variable's type is accepted by this iopin
    if( ! types().contains( other->type() ) ) return;

    // remember this iopin as being bound with the given variable.
    other->m_connectedIOPins.append( this );

    // and remember where we are bound to
    d->boundTo = other;
}

void IOPin::unbind()
{
    // first check that we are bound to some variable
    if( d->boundTo == NULL ) return;

    // remove the iopin from variable
    d->boundTo->m_connectedIOPins.remove( this );

    // cleanup
    d->boundTo = NULL;
}

Variable * IOPin::boundTo() const
{
    return d->boundTo;
}

// vim: set et ts=4 sw=4 tw=76:
// $Id: IOPin.cpp 1774 2006-05-31 13:17:16Z hynek $
