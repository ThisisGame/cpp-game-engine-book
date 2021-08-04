/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_PROPERTY_WRAPPER_MEMBER_OBJECT_H_
#define RTTR_PROPERTY_WRAPPER_MEMBER_OBJECT_H_

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read write

template<typename C, typename A, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_object_ptr, A(C::*), void, Acc_Level, return_as_copy, set_value, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using accessor = A (C::*);
    public:
        property_wrapper(string_view name, type declaring_type,
                         accessor acc, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_acc(acc)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<A>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            C* ptr = object.try_convert<C>();
            if (ptr && arg.is_type<A>())
                return property_accessor<A>::set_value((ptr->*m_acc), arg.get_value<A>());
            else
                return false;
        }

        variant get_value(instance& object) const
        {
            if (C* ptr = object.try_convert<C>())
                return variant((ptr->*m_acc));
            else
                return variant();
        }

    private:
        accessor m_acc;
};


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read only (because of std::false_type)

template<typename C, typename A, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_object_ptr, A(C::*), void, Acc_Level, return_as_copy, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using accessor = A (C::*);
    public:
        property_wrapper(string_view name, type declaring_type,
                         accessor acc, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_acc(acc)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<A>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            if (C* ptr = object.try_convert<C>())
                return variant(ptr->*m_acc);
            else
                return variant();
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read write

template<typename C, typename A, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_object_ptr, A(C::*), void, Acc_Level, return_as_ptr, set_as_ptr, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using accessor = A (C::*);
    public:
        property_wrapper(string_view name, type declaring_type,
                         accessor acc, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_acc(acc)
        {
            static_assert(!std::is_pointer<A>::value, "The data type of the property is already a pointer type! The given policy cannot be used for this property.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<A*>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            C* ptr = object.try_convert<C>();
            if (ptr && arg.is_type<A*>())
            {
                return property_accessor<A>::set_value((ptr->*m_acc), *arg.get_value<A*>());
            }
            else
            {
                return false;
            }
        }

        variant get_value(instance& object) const
        {
            if (C* ptr = object.try_convert<C>())
                return variant(&(ptr->*m_acc));
            else
                return variant();
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read only

template<typename C, typename A, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_object_ptr, A(C::*), void, Acc_Level, return_as_ptr, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using accessor = A (C::*);
    public:
        property_wrapper(string_view name, type declaring_type,
                         accessor acc, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_acc(acc)
        {
            static_assert(!std::is_pointer<A>::value, "The data type of the property is already a pointer type! The given policy cannot be used for this property.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<typename std::add_const<A>::type*>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            if (C* ptr = object.try_convert<C>())
                return variant(const_cast<const A*>(&(ptr->*m_acc)));
            else
                return variant();
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read write

template<typename C, typename A, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_object_ptr, A(C::*), void, Acc_Level, get_as_ref_wrapper, set_as_ref_wrapper, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using accessor = A (C::*);
    public:
        property_wrapper(string_view name, type declaring_type,
                         accessor acc, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_acc(acc)
        {
            static_assert(!std::is_pointer<A>::value, "The data type of the property is already a pointer type! The given policy cannot be used for this property.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<std::reference_wrapper<A>>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            C* ptr = object.try_convert<C>();
            if (ptr && arg.is_type<std::reference_wrapper<A>>())
                return property_accessor<A>::set_value((ptr->*m_acc), arg.get_value<std::reference_wrapper<A>>().get());
            else
                return false;
        }

        variant get_value(instance& object) const
        {
            if (C* ptr = object.try_convert<C>())
                return variant(std::ref(ptr->*m_acc));
            else
                return variant();
        }

    private:
        accessor m_acc;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// pointer to member - read only

template<typename C, typename A, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_object_ptr, A(C::*), void, Acc_Level, get_as_ref_wrapper, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using accessor = A (C::*);
    public:
        property_wrapper(string_view name, type declaring_type,
                         accessor acc, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_acc(acc)
        {
            static_assert(!std::is_pointer<A>::value, "The data type of the property is already a pointer type! The given policy cannot be used for this property.");

            init();
        }
        using policy_type = std::reference_wrapper<add_const_t<A>>;
        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get< std::reference_wrapper<add_const_t<A>> >(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            if (C* ptr = object.try_convert<C>())
                return variant(std::cref((ptr->*m_acc)));
            else
                return variant();
        }

    private:
        accessor m_acc;
};

#endif // RTTR_PROPERTY_WRAPPER_MEMBER_OBJECT_H_
