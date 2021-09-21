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

#ifndef RTTR_PROPERTY_WRAPPER_MEMBER_FUNC_H_
#define RTTR_PROPERTY_WRAPPER_MEMBER_FUNC_H_

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Getter/Setter - pointer to member function

template<typename Getter, typename Setter, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_func_ptr, Getter, Setter, Acc_Level, return_as_copy, set_value, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using return_type   = typename function_traits<Getter>::return_type;
    using arg_type      = typename param_types<Setter, 0>::type;
    using class_type    = typename function_traits<Getter>::class_type;

    public:
        property_wrapper(string_view name, type declaring_type,
                         Getter get, Setter set,
                         std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(function_traits<Getter>::arg_count == 0, "Invalid number of argument, please provide a getter-member-function without arguments.");
            static_assert(function_traits<Setter>::arg_count == 1, "Invalid number of argument, please provide a setter-member-function with exactly one argument.");
            static_assert(std::is_same<return_type, arg_type>::value, "Please provide the same signature for getter and setter!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<return_type>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            class_type* ptr = object.try_convert<class_type>();
            if (ptr && arg.is_type<arg_type>() )
            {
                (ptr->*m_setter)(arg.get_value<arg_type>());
                return true;
            }
            return false;
        }

        variant get_value(instance& object) const
        {
            if (class_type* ptr = object.try_convert<class_type>())
                return variant((ptr->*m_getter)());
            else
                return variant();
        }

    private:
        Getter  m_getter;
        Setter  m_setter;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Getter - pointer to member function

template<typename Getter, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_func_ptr, Getter, void, Acc_Level, return_as_copy, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using return_type   = typename function_traits<Getter>::return_type;
    using class_type    = typename function_traits<Getter>::class_type;

    public:
        property_wrapper(string_view name, type declaring_type,
                         Getter get, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_getter(get)
        {
            static_assert(function_traits<Getter>::arg_count == 0, "Invalid number of argument, please provide a getter-member-function without arguments.");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true;  }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<return_type>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            if (class_type* ptr = object.try_convert<class_type>())
                return variant((ptr->*m_getter)());
            else
                return variant();
        }

    private:
        Getter  m_getter;
};

/////////////////////////////////////////////////////////////////////////////////////////
// Policy return_as_ptr
/////////////////////////////////////////////////////////////////////////////////////////

// Getter/Setter pointer to member function
template<typename Getter, typename Setter, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_func_ptr, Getter, Setter, Acc_Level, return_as_ptr, set_as_ptr, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using return_type   = typename function_traits<Getter>::return_type;
    using arg_type      = typename param_types<Setter, 0>::type;
    using class_type    = typename function_traits<Getter>::class_type;

    public:
        property_wrapper(string_view name, type declaring_type,
                         Getter get, Setter set,
                         std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(function_traits<Getter>::arg_count == 0, "Invalid number of argument, please provide a getter-member-function without arguments.");
            static_assert(function_traits<Setter>::arg_count == 1, "Invalid number of argument, please provide a setter-member-function with exactly one argument.");
            static_assert(std::is_same<return_type, arg_type>::value, "Please provide the same signature for getter and setter!");

            static_assert(std::is_reference<return_type>::value, "Please provide a getter-member-function with a reference as return value!");
            static_assert(std::is_reference<arg_type>::value, "Please provide a setter-member-function with a reference as return value!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<typename std::remove_reference<return_type>::type*>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            using arg_type_t = remove_reference_t<arg_type>;
            class_type* ptr = object.try_convert<class_type>();
            if (ptr && arg.is_type<arg_type_t*>())
            {
                (ptr->*m_setter)(*arg.get_value<arg_type_t*>());
                return true;
            }
            return false;
        }

        variant get_value(instance& object) const
        {
            if (class_type* ptr = object.try_convert<class_type>())
                return variant(&(ptr->*m_getter)());
            else
                return variant();
        }

    private:
        Getter  m_getter;
        Setter  m_setter;

};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Getter - pointer to member function

template<typename Getter, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_func_ptr, Getter, void, Acc_Level, return_as_ptr, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using return_type   = typename function_traits<Getter>::return_type;
    using class_type    = typename function_traits<Getter>::class_type;
    using policy_type   = add_pointer_t<add_const_t<remove_reference_t<return_type>>>;

    public:
        property_wrapper(string_view name, type declaring_type,
                         Getter get,
                         std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_getter(get)
        {
            static_assert(function_traits<Getter>::arg_count == 0, "Invalid number of argument, please provide a getter-member-function without arguments.");
            static_assert(std::is_reference<return_type>::value, "Please provide a getter-member-function with a reference as return value!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<policy_type>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            if (class_type* ptr = object.try_convert<class_type>())
                return variant(const_cast<policy_type>(&(ptr->*m_getter)()));
            else
                return variant();
        }

    private:
        Getter  m_getter;
};

/////////////////////////////////////////////////////////////////////////////////////////
// Policy return_as_ptr
/////////////////////////////////////////////////////////////////////////////////////////

// Getter/Setter pointer to member function
template<typename Getter, typename Setter, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_func_ptr, Getter, Setter, Acc_Level, get_as_ref_wrapper, set_as_ref_wrapper, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using return_type   = typename function_traits<Getter>::return_type;
    using arg_type      = typename param_types<Setter, 0>::type;
    using class_type    = typename function_traits<Getter>::class_type;

    public:
        property_wrapper(string_view name, type declaring_type,
                         Getter get, Setter set,
                         std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_getter(get), m_setter(set)
        {
            static_assert(function_traits<Getter>::arg_count == 0, "Invalid number of argument, please provide a getter-member-function without arguments.");
            static_assert(function_traits<Setter>::arg_count == 1, "Invalid number of argument, please provide a setter-member-function with exactly one argument.");
            static_assert(std::is_same<return_type, arg_type>::value, "Please provide the same signature for getter and setter!");

            static_assert(std::is_reference<return_type>::value, "Please provide a getter-member-function with a reference as return value!");
            static_assert(std::is_reference<arg_type>::value, "Please provide a setter-member-function with a reference as return value!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get< std::reference_wrapper<remove_reference_t<return_type>> >(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            using arg_type_t = remove_reference_t<arg_type>;
            class_type* ptr = object.try_convert<class_type>();
            if (ptr && arg.is_type<std::reference_wrapper<arg_type_t>>())
            {
                (ptr->*m_setter)(arg.get_value<std::reference_wrapper<arg_type_t>>().get());
                return true;
            }
            return false;
        }

        variant get_value(instance& object) const
        {
            if (class_type* ptr = object.try_convert<class_type>())
                return variant(std::ref((ptr->*m_getter)()));
            else
                return variant();
        }

    private:
        Getter  m_getter;
        Setter  m_setter;

};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Getter - pointer to member function

template<typename Getter, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<member_func_ptr, Getter, void, Acc_Level, get_as_ref_wrapper, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    using return_type   = typename function_traits<Getter>::return_type;
    using class_type    = typename function_traits<Getter>::class_type;
    using policy_type   = std::reference_wrapper<add_const_t<remove_reference_t<return_type>>>;

    public:
        property_wrapper(string_view name, type declaring_type,
                         Getter get,
                         std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_getter(get)
        {
            static_assert(function_traits<Getter>::arg_count == 0, "Invalid number of argument, please provide a getter-member-function without arguments.");
            static_assert(std::is_reference<return_type>::value, "Please provide a getter-member-function with a reference as return value!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<policy_type>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            if (class_type* ptr = object.try_convert<class_type>())
                return variant(std::cref((ptr->*m_getter)()));
            else
                return variant();
        }

    private:
        Getter  m_getter;
};

#endif // RTTR_PROPERTY_WRAPPER_MEMBER_FUNC_H_
