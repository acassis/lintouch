// $Id: LTDict.hh 1168 2005-12-12 14:48:03Z modesto $
//
//   FILE: LTDict.hh -- 
// AUTHOR: Martin Man <Martin.Man@seznam.cz>
//   DATE: 19 August 2004
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

#ifndef _LTDICT_HH
#define _LTDICT_HH

#include <qmap.h>
#include <qvaluevector.h>

namespace lt {

    /**
     * Lintouch Dictionary.
     *
     * LTDict is an associative memory for pointer based classes. The
     * elements stored in the dictionary can be accessed either by their
     * keys (dictionary behavior) or by their indices (linear list).
     *
     * Additionally, there is always one 'current' or 'selected' item when
     * the dictionary is not empty.
     *
     * The NULL items can not be inserted into the dictionary.
     *
     * The dictionary can optionally own all inserted items and delete them
     * automatically when cleared or destroyed.
     *
     * @see QDict
     *
     * @author <a href="mailto:mman@swac.cz">Martin Man</a>
     */
    template <class T> class LTDict
    {
        public:

            /**
             * Constructs a dictionary.
             */
            LTDict() :
                m_current( -1 ), m_count( 0 ), m_autoDelete( false )
            {
            }

            /**
             * Copy constructor.
             *
             * Creates copy of the dictionary by copying all its entries and
             * drops the autodelete option for the resulting dictionary.
             */
            LTDict( const LTDict & orig ) :
                m_current( orig.m_current ),
                m_count( orig.m_count ),
                m_autoDelete( false ),
                m_keys( orig.m_keys ),
                m_values( orig.m_values ),
                m_mapping( orig.m_mapping )
            {
            }

            LTDict & operator=( const LTDict & orig )
            {
                if( m_autoDelete ) deleteItems();

                m_current = orig.m_current;
                m_count = orig.m_count;
                m_autoDelete = false;
                m_keys = orig.m_keys;
                m_values = orig.m_values;
                m_mapping = orig.m_mapping;

                return *this;
            }

            /**
             * Destroys the dictionary.
             *
             * When autoDelete is set also deletes all items.
             *
             * @see setAutoDelete
             */
            virtual ~LTDict()
            {
                if( m_autoDelete ) deleteItems();
            }

            /**
             * Returns the number of items in this dictionary.
             */
            unsigned count() const
            {
                return m_mapping.count();
            }

            /**
             * Returns true is the dictionary is empty, false otherwise.
             */
            bool isEmpty() const
            {
                return m_mapping.isEmpty();
            }

            /**
             * Inserts the given item to the dictionary.
             *
             * If the key is already present in the dictionary, the item can
             * be optionally overwritten. The linear order is not altered in
             * such case. The overwitten item is automatically deleted when
             * autoDelete is enabled, lost in space otherwise.
             *
             * @param k The key under which the item is accessible.
             * @param d The item. Must not be NULL.
             * @param index Defines the position of the item in the linear
             * list. When negative, the item is appended to the end of the
             * linear list.
             * @param overwrite Shall we overwrite the old entry?
             */
            void insert( const QString & k, const T * d, int index = -1,
                    bool overwrite = true )
            {
                // we don't allow nulls to be inserted to the dictionary
                if( d == NULL ) return;

                // check that we already have the given key
                bool already_existed = m_mapping.contains( k );

                // do not overwrite existing items
                if( already_existed && ! overwrite ) return;

                // overwrite value only when enabled
                if( already_existed ) {
                    if( m_autoDelete ) deleteItem( m_mapping[ k ] );
                    m_values[ m_mapping[ k ] ] = ( T* )d;
                    return;
                }

                // we append item at the end
                if( index < 0 || (unsigned)index > m_count ) {
                    if( m_keys.size() > m_count ) {
                        m_keys[ m_count ] = k;
                        m_values[ m_count ] = ( T* )d;
                    } else {
                        m_keys.append( k );
                        m_values.append( ( T* )d );
                    }
                    m_mapping[ k ] = m_count;
                    m_count ++;
                } else {
                    Keys::iterator i = m_keys.begin();
                    typename Values::iterator iv = m_values.begin();
                    // find the position where to insert the key
                    for( int j = 0; j < index; j++, ++i, ++iv );
                    m_keys.insert( i, k );
                    m_values.insert( iv, ( T* )d );
                    m_count ++;

                    // update mapping for items that have been shifted to
                    // the right by insert operation
                    for( unsigned l = index; l < m_count; l ++ ) {
                        m_mapping[ m_keys[ l ] ] = l;
                    }
                }

                // when the dict was empty, select its first item as the
                // current one
                if( m_count == 1 ) {
                    m_current = 0;
                }
            }

            /**
             * Removes the item at the given position from the dictionary.
             *
             * The item is deleted when autoDelete is set.
             *
             * The current selection is left untouched when the current item
             * has been deleted thus moving the selection to the item which
             * was next to the deleted one in the linear list.
             *
             * When the current and at the same time last (in the linear
             * list) item is deleted, the current selection is moved to the
             * item that was before the removed one in the linear list.
             *
             * @see setAutoDelete
             */
            void remove( unsigned index )
            {
                if( index < m_count ) {
                    remove( m_keys[ index ] );
                }
            }

            /**
             * Removes the item under the given key from the dictionary.
             *
             * The item is deleted when autoDelete is set.
             *
             * The current selection is left untouched when the current item
             * has been deleted thus moving the selection to the item which
             * was next to the deleted one in the linear list.
             *
             * When the current and at the same time last (in the linear
             * list) item is deleted, the current selection is moved to the
             * item that was before the removed one in the linear list.
             *
             * @see setAutoDelete
             */
            void remove( const QString & k )
            {
                if( m_mapping.contains( k ) ) {
                    unsigned index = m_mapping[ k ];
                    m_mapping.remove( k );
                    if( m_autoDelete ) deleteItem( index );

                    // update keys & values & mapping
                    for( unsigned jj = index; jj < m_count - 1; jj++ ) {
                        m_keys[ jj ] = m_keys[ jj + 1 ];
                        m_values[ jj ] = m_values[ jj + 1 ];
                        m_mapping[ m_keys[ jj ] ] = jj;
                    }
                    m_count --;

                    // what if we removed current item?

                    // if there is a space, we leave the index
                    if( index == (unsigned)m_current && index < m_count ) {
                        return;
                    }

                    // otherwise we shift back, should go to -1 when no
                    // items are in dictionary
                    if( ( index == (unsigned)m_current && index == m_count )
                            || ( (unsigned)m_current >= m_count ) ) {
                        m_current --;
                    }
                }
            }

            /**
             * Moves item at the given postion to the new position.
             *
             * The current selection stays at the same position in the
             * linear list.
             *
             * @param index Which item to move.
             * @param newidx Where to morve the item. When negative, the
             * item is appended to the end of the linear list.
             */
            void move( unsigned index, int newidx )
            {
                unsigned where = (unsigned)newidx;
                if( newidx < 0 || where > m_count ) where = m_count - 1;

                //the item we are moving
                QString what = m_keys[ index ];
                T * what_val = m_values[ index ];

                if( where > index ) {
                    for( unsigned i = index; i < where; i ++ ) {
                        m_keys[ i ] = m_keys[ i + 1 ];
                        m_values[ i ] = m_values[ i + 1 ];
                        m_mapping[ m_keys[ i ] ] = i;
                    }
                } else {
                    for( unsigned i = index; i > where; i -- ) {
                        m_keys[ i ] = m_keys[ i - 1 ];
                        m_values[ i ] = m_values[ i - 1 ];
                        m_mapping[ m_keys[ i ] ] = i;
                    }
                }

                // place the moved element
                m_keys[ where ] = what;
                m_values[ where ] = what_val;
                m_mapping[ what ] = where;
            }

            /**
             * Moves item under the given key to the new position.
             *
             * The current selection stays at the same position in the
             * linear list.
             *
             * @param k Which item to move.
             * @param newidx Where to morve the item. When negative, the
             * item is appended to the end of the linear list.
             */
            void move( const QString & k, int newidx )
            {
                if( m_mapping.contains( k ) ) {
                    move( indexOf( k ), newidx );
                }
            }

            /**
             * Returns the position of the given key or -1 when the key
             * does not exist in the dictionary.
             */
            int indexOf( const QString & k ) const
            {
                // we have the given key
                if( m_mapping.contains( k ) ) {
                    return m_mapping[ k ];
                }
                return -1;
            }

            /**
             * Returns the key at the given position or QString::null for
             * invalid position.
             */
            QString keyAt( unsigned index ) const
            {
                if( index < m_count ) {
                    return m_keys[ index ];
                }
                return QString::null;
            }

            /**
             * Returns true when the dictionary contains the given key,
             * false otherwise.
             */
            bool contains( const QString & k ) const
            {
                return m_mapping.contains( k );
            }

            /**
             * Returns the item at the given position or NULL when the
             * position is invalid.
             */
            const T * operator[]( unsigned index ) const
            {
                if( index < m_count ) {
                    return m_values[ index ];
                }
                qDebug( QString( "LTDict::operator[ unsigned ]:"
                        " nonexistent index '%1' requested" )
                        .arg( index ) );
                return NULL;
            }

            /**
             * Returns the item under the given key or NULL when the
             * key does not exist in the dictionary.
             */
            const T * operator[]( const QString & k ) const
            {
                if( m_mapping.contains( k ) ) {
                    return m_values[ m_mapping[ k ] ];
                }
                qDebug( QString( "LTDict::operator[ const QString & ]:"
                        " nonexistent entry '%1' requested" )
                        .arg( k ) );
                return NULL;
            }

            /**
             * Returns the item at the given position or NULL when the
             * position is invalid.
             */
            T * operator[]( unsigned index )
            {
                if( index < m_count ) {
                    return m_values[ index ];
                }
                qDebug( QString( "LTDict::operator[ unsigned ]:"
                        " nonexistent index '%1' requested" )
                        .arg( index ) );
                return NULL;
            }

            /**
             * Returns the item under the given key or NULL when the
             * key does not exist in the dictionary.
             */
            T * operator[]( const QString & k )
            {
                if( m_mapping.contains( k ) ) {
                    return m_values[ m_mapping[ k ] ];
                }
                qDebug( QString( "LTDict::operator[ const QString & ]:"
                        " nonexistent entry '%1' requested" )
                        .arg( k ) );
                return NULL;
            }

            /**
             * Removes all items from the dictionary.
             *
             * The items are deleted when autoDelete is set.
             *
             * @see setAutoDelete.
             */
            void clear()
            {
                m_mapping.clear();
                m_count = 0;
                m_current = -1;

                if( m_autoDelete ) deleteItems();
            }

        private:

            /**
             * Delete given entry within the m_values storage.
             */
            void deleteItem( unsigned i )
            {
                if( m_values[ i ] ) {
                    delete m_values[ i ];
                    m_values[ i ] = NULL;
                }
            }

            /**
             * Delete all entries within the m_values storage.
             */
            void deleteItems()
            {
                for( unsigned i = 0; i < m_count; i ++ ) {
                    if( m_values[ i ] ) {
                        delete m_values[ i ];
                        m_values[ i ] = NULL;
                    }
                }
            }

        public:

            /**
             * The type defining the storage for keys.
             */
            typedef QValueVector<QString> Keys;

            /**
             * The type defining the storage for values.
             */
            typedef QValueVector<T*> Values;

            /**
             * The type defining the mapping between key and index.
             */
            typedef QMap <QString, int> Mapping;

            /**
             * Returns the keys in the dictionary.
             */
            const Keys & keys() const
            {
                return m_keys;
            }

            /**
             * Returns the values in the dictionary.
             */
            const Values & values() const
            {
                return m_values;
            }

            /**
             * Returns the mapping between the keys and their positions in
             * the linear list.
             */
            const Mapping & mapping() const
            {
                return m_mapping;
            }

        public:

            /**
             * Changes the currently selected item.
             *
             * The current selection remains unaltered when the invalid index
             * is specified.
             */
            void setCurrentIndex( unsigned index )
            {
                // we have the given index
                if( index < m_count ) {
                    m_current = index;
                    return;
                }
            }

            /**
             * Changes the currently selected item.
             *
             * The current selection remains unaltered when the nonexistent
             * key is specified.
             */
            void setCurrentKey( const QString & k )
            {
                // we have the given key
                if( m_mapping.contains( k ) ) {
                    m_current = m_mapping[ k ];
                }
            }

            /**
             * Returns the position of currently selected item or -1 when
             * the dictionary is empty.
             */
            int currentIndex() const
            {
                return m_current;
            }

            /**
             * Returns the key of currently selected item or QString::null
             * when the dictionary is empty.
             */
            QString currentKey() const
            {
                if( m_current != -1 ) {
                    return m_keys[ m_current ];
                }
                return QString::null;
            }

            /**
             * Returns the currently selected item.
             */
            const T * current() const
            {
                if( m_current != -1 ) {
                    return m_values[ m_current ];
                }
                return NULL;
            }

            /**
             * Returns the currently selected item.
             */
            T * current()
            {
                if( m_current != -1 ) {
                    return m_values[ m_current ];
                }
                return NULL;
            }

        public:

            /**
             * Returns the setting of the autoDelete option of this
             * dictionary. False by default.
             *
             * @see setAutoDelete.
             */
            bool autoDelete() const
            {
                return m_autoDelete;
            }

            /**
             * Sets the collection to automatically delete items when they
             * are being removed.
             *
             * When the collection is copied by the operator= or using copy
             * constructor the newly created collection has this option set
             * to false always.
             */
            void setAutoDelete( bool autodelete )
            {
                m_autoDelete = autodelete;
            }

        private:

            /**
             * Currently selected item. -1 when the dictionary is empty.
             */
            int m_current;

            /**
             * Number of the items in the dictionary.
             */
            unsigned m_count;

            /**
             * Should we autodelete items?
             */
            bool m_autoDelete;

            /**
             * Ordered keys.
             */
            Keys m_keys;

            /**
             * Ordered keys.
             */
            Values m_values;

            /**
             * Mapping between keys and indices.
             */
            Mapping m_mapping;
    };

} // namespace lt

#endif /* _LTDICT_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: LTDict.hh 1168 2005-12-12 14:48:03Z modesto $
