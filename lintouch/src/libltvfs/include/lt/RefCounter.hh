// $Id: RefCounter.hh,v 1.3 2004/02/19 16:45:13 hynek Exp $
//
//   FILE: RefCounter.hh -- 
// AUTHOR: Hynek Petrak <hynek@swac.cz>
//   DATE: 29 December 2003
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

#ifndef _REFCOUNTER_HH
#define _REFCOUNTER_HH


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
namespace lt {

        class RefCounter {
            public:
                explicit RefCounter(long v) : _value(v) {}

                void operator++() {
                    InterlockedIncrement(&_value);
                }

                long operator--() {
                    return InterlockedDecrement(&_value);
                }

                operator long() const {
                    return _value;
                }

            private:

                RefCounter(RefCounter const &);
                RefCounter & operator=(RefCounter const &);

                long _value;
        };
} // namespace lt

#elif defined(__GLIBCPP__)
#include <bits/atomicity.h>
namespace lt {

        class RefCounter {
            public:
                explicit RefCounter(long v) : _value(v) {}

                void operator++() {
                    __atomic_add(&_value, 1);
                }

                long operator--() {
                    return __exchange_and_add(&_value, -1)-1;
                }

                operator long() const {
                    return __exchange_and_add(&(_Atomic_word)_value, 0);
                }

            private:

                RefCounter(RefCounter const &);
                RefCounter & operator=(RefCounter const &);

                _Atomic_word _value;
        };

} // namespace lt

#elif defined(HAVE_PTHREAD)
#include <pthread.h>
namespace lt {

        class RefCounter {
            private:

                class scoped_lock
                {
                public:

                    scoped_lock(pthread_mutex_t & m): m_(m) {
                        pthread_mutex_lock(&m_);
                    }

                    ~scoped_lock() {
                        pthread_mutex_unlock(&m_);
                    }

                private:

                    pthread_mutex_t & m_;
                };

            public:

                explicit RefCounter(long v): _value(v) {
                    pthread_mutex_init(&mutex_, 0);
                }

                ~RefCounter() {
                    pthread_mutex_destroy(&mutex_);
                }

                void operator++() {
                    scoped_lock lock(mutex_);
                    ++_value;
                }

                long operator--() {
                    scoped_lock lock(mutex_);
                    return --_value;
                }

                operator long() const {
                    scoped_lock lock(mutex_);
                    return _value;
                }

            private:

                RefCounter(RefCounter const &);
                RefCounter & operator=(RefCounter const &);

                mutable pthread_mutex_t mutex_;
                long _value;
        };

} // namespace lt

#else
namespace lt {

//#error "Multi-thread environment not recognized or not available." 
        typedef long RefCounter;

} // namespace lt
#endif

#endif /* _REFCOUNTER_HH */

// vim: set et ts=4 sw=4 tw=76:
// $Id: RefCounter.hh,v 1.3 2004/02/19 16:45:13 hynek Exp $
