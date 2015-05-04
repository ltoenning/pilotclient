/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INHERITANCE_TRAITS_H
#define BLACKMISC_INHERITANCE_TRAITS_H

#include <QMetaType>
#include <type_traits>

namespace BlackMisc
{
    class CEmpty;

    /*!
     * If T has a member typedef base_type, this trait will obtain it, otherwise void.
     */
    template <class T>
    class BaseOf
    {
        //template <typename U> static typename U::base_type *test(int);
        template <typename U> static typename U::base_type *test(typename std::enable_if<! std::is_same<typename U::base_type, CEmpty>::value, int>::type);
        template <typename U> static void *test(...);

    public:
        //! The declared base_type of T, or void if there is none.
        typedef typename std::remove_pointer<decltype(test<T>(0))>::type type;
    };

    /*!
     * It T has a member typedef base_type which is a registered metatype, this trait will obtain it, otherwise void.
     */
    template <class T>
    class MetaBaseOf
    {
    public:
        //! Type of T::base_type, or void if not declared.
        typedef typename std::conditional<QMetaTypeId<typename BaseOf<T>::type>::Defined, typename BaseOf<T>::type, void>::type type;
    };

    /*!
     * Alias for typename BaseOf<T>::type.
     */
    template <class T>
    using BaseOfT = typename BaseOf<T>::type;

    /*!
     * Alias for typename MetaBaseOf<T>::type.
     */
    template <class T>
    using MetaBaseOfT = typename MetaBaseOf<T>::type;
}

#endif
