// $Id: SharedPtr.hh,v 1.4 2004/02/19 14:18:58 hynek Exp $
//
//   FILE: SharedPtr.hh -- 
// AUTHOR: Hynek Petrak <hynek@swac.cz>
//   DATE: 10 September 2003
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

#ifndef _SHAREDPTR_HH
#define _SHAREDPTR_HH

#include "RefCounter.hh"
#include <assert.h>
#include <stddef.h>
#include <algorithm>

namespace lt {

        /**
         * Template class for reference-counted pointers.
         *
         * The SharedPtr&lt;T&gt; class template stores a pointer to a
         * dynamically allocated object, 
         * typically with a C++ <i>new-expression</i>.
         * The object pointed to is guaranteed to be deleted when last
         * SharedPtr&lt;T&gt; pointing to it is destroyed or reset.
         *
         * @author <a href="mailto:hynek@swac.cz">Hynek Petrak</a>
         */
        template<typename T> 
        class SharedPtr {
            private:
                typedef RefCounter count_type;
            public:

                typedef T element_type;
                typedef T value_type;
                /**
                 * Default constructor.
                 * internal pointer as well as reference counter
                 * are set to NULL.
                 */
                SharedPtr():_p(NULL), _c(NULL) {
                }
                /**
                 * Constructor.
                 * Only way to create SharedPtr out of ordinary 
                 * pointer is to explicitly call this constructor.
                 * Reference counter is set to 1. SharedPtr takes ownership
                 * of the pointer.
                 * @param p pointer to T.
                 */
                explicit SharedPtr(T * p): _p(p) {
                    _c = p?new count_type(1):NULL;
                }
                /**
                 * Destructor.
                 * When this is the last refering SharedPtr, the destructor
                 * calls delete on the internal pointer. It also deletes the 
                 * reference counter.
                 */
                ~SharedPtr() {
                    if ((_c) && (--*_c == 0)) {
                        delete _p;
                        delete _c;
                    }
                }
                /**
                 * Generalized "copy" constructor.
                 * This copy contructor is used for SharedPtr<U>s, where U 
                 * differs from T, but conversion T* = U* is possible. 
                 * Copies the pointer and increases the reference counter.
                 * If the conversion is not possible compiler should print 
                 * an error. 
                 */
                template<typename U>
                SharedPtr(SharedPtr<U> const & r): _p(r._getPointer()),
                        _c(r._getCounter()) {
                    if (_c) {
                        ++*_c;
                    }
                }
                /**
                 * Copy constructor.
                 * Copies the pointer and increases the reference counter.
                 */
                SharedPtr(SharedPtr const & r):_p(r._p),
                        _c(r._c) {        
                    if (_c) {
                        ++*_c;
                    }
                }
                /**
                 * Generalized assignment operator.
                 * It is used for SharedPtr<U>s, where U 
                 * differs from T, but conversion T* = U* is possible.
                 * Copies the pointer and increases the reference counter.
                 * If the conversion is not possible compiler should print 
                 * an error.
                 */
                template <typename U>
                SharedPtr & operator=(SharedPtr<U> const & r) {
                    if (_p == r._getPointer())
                        return *this;
                    if ((_c) && (--*_c == 0)) {
                        delete _p;
                        delete _c;
                    }
                    _p = r._getPointer();
                    _c = r._getCounter();
                    if (_c) {
                        ++*_c;
                    }
                    return *this;
                }
                /**
                 * Assignment operator.
                 * Copies the pointer and increases the reference counter.
                 */
                SharedPtr & operator=(SharedPtr const & r) {
                    if (_p == r._p)
                        return *this;
                    if ((_c) && (--*_c == 0)) {
                        delete _p;
                        delete _c;
                    }
                    _p = r._p;
                    _c = r._c;
                    if (_c) {
                        ++*_c;
                    }
                    return *this;
                }
                /**
                 * Resets the pointer to the new pointer p.
                 */
                void reset(T * p = 0) {
                    assert(p == 0 || p != _p);
                    if ((_c) && (--*_c == 0)) {
                        delete _p;
                        delete _c;
                    }
                    _p = p;
                    _c = p?new count_type(1):NULL;
                }
                /**
                 * Dereferencing operator
                 */
                inline T & operator*() const {
                    assert(_p != 0);
                    return *_p;
                }
                /**
                 * Dereferencing operator.
                 * Allows access to members of
                 * the referenced object. Use carefuly! 
                 * The internal pointer must refer to valid object!!!
                 */
                inline T * operator->() const {
                    assert(_p != 0);
                    return _p;
                }
                /**
                 * Returns the internal pointer (doesn't release
                 * the ownership).
                 */
                inline T * get() const {
                    return _p;
                }
                /**
                 * Returns value of internal reference counter.
                 * 0 if the pointer is NULL.
                 */
                inline long useCount() const {
                    return (_c)?*_c:0;
                }
                /**
                 * @return true if the pointer is NULL or reference 
                 * counter is equal to 1
                 */
                inline bool unique() const  {
                    return ((!_c) || (*_c == 1));
                }

                void swap(SharedPtr<T> & other) {
                    std::swap(_p, other._p);
                    std::swap(_c, other._c);
                }

                /** 
                 * @return true if pointer is not NULL
                 * @see isNull()
                 */
                inline operator bool() const { 
                    return (_c != NULL);
                }
                /** 
                 * @return true if pointer is not NULL
                 * @see operator bool()
                 */
                inline bool isNull() const { 
                    return (!_c);
                }
#ifdef ENABLE_DETACH
                /**
                 * Detaches internal pointer from the SharePtr<>.
                 * Pointer is detached only if unique() == true. 
                 * @return internal pointer value or NULL if !unique()
                 */
                inline T* detach() { 
                    if ((_c) &&
                            ((*_c) == 1)) {
                        T* r = _p;
                        delete _c;
                        _c = NULL;
                        _p = NULL;
                        return r;
                    } else
                        return NULL;
                }
#endif
                inline T* _getPointer() const { 
                    return _p; 
                }
                inline count_type* _getCounter() const { 
                    return _c; 
                }
            private:
                T * _p;            // contained pointer
                count_type * _c;   // ptr to reference counter
        };

        template<class T, class U> inline bool operator==(
                SharedPtr<T> const& a, SharedPtr<U> const& b)
        {
            return a.get() == b.get();
        }

        template<class T, class U> inline bool operator!=(
                SharedPtr<T> const& a, SharedPtr<U> const& b)
        {
            return a.get() != b.get();
        }

} // namespace lt

#endif /* _SHAREDPTR_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: SharedPtr.hh,v 1.4 2004/02/19 14:18:58 hynek Exp $
