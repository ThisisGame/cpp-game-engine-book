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

#ifndef RTTR_PROPERTY_WRAPPER_OBJECT_H_
#define RTTR_PROPERTY_WRAPPER_OBJECT_H_

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global property read write

template<typename C, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<object_ptr, C*, void, Acc_Level, return_as_copy, set_value, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        property_wrapper(string_view name, type declaring_type,
                         C* pointer, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)),
            m_accessor(pointer)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return true; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<C>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            if (arg.is_type<C>())
                return property_accessor<C>::set_value(*m_accessor, arg.get_value<C>());
            else
                return false;
        }

        variant get_value(instance& object) const
        {
            return (variant(*m_accessor));
        }

    private:
        C* m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global property read_only

template<typename C, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<object_ptr, C*, void, Acc_Level, return_as_copy, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        property_wrapper(string_view name, type declaring_type,
                         C* pointer, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_accessor(pointer)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return true; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<C>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            return (variant(*m_accessor));
        }

    private:
        C* m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global property read write

template<typename C, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<object_ptr, C*, void, Acc_Level, return_as_ptr, set_as_ptr, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        property_wrapper(string_view name, type declaring_type,
                         C* pointer, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_accessor(pointer)
        {
            static_assert(!std::is_pointer<C>::value, "The given type is already a pointer type!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<C*>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            if (arg.is_type<C*>())
            {
                *m_accessor = *arg.get_value<C*>();
                return true;
            }
            else
            {
                return false;
            }
        }

        variant get_value(instance& object) const
        {
            return (variant(m_accessor));
        }

    private:
        C* m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global property read_only

template<typename C, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<object_ptr, C*, void, Acc_Level, return_as_ptr, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        property_wrapper(string_view name, type declaring_type,
                         C* pointer, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_accessor(pointer)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return true; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get<typename std::add_const<C>::type*>(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            return (variant(m_accessor));
        }

    private:
        C* m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global property read write

template<typename C, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<object_ptr, C*, void, Acc_Level, get_as_ref_wrapper, set_as_ref_wrapper, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        property_wrapper(string_view name, type declaring_type,
                         C* pointer, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_accessor(pointer)
        {
            static_assert(!std::is_pointer<C>::value, "The given type is already a pointer type!");

            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return false; }
        bool is_static()    const RTTR_NOEXCEPT                 { return false; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get< std::reference_wrapper<C> >(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            if (arg.is_type<std::reference_wrapper<C>>())
                return property_accessor<C>::set_value(*m_accessor, arg.get_value<std::reference_wrapper<C>>().get());
            else
                return false;
        }

        variant get_value(instance& object) const
        {
            return (variant(std::ref(*m_accessor)));
        }

    private:
        C* m_accessor;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// global property read_only

template<typename C, access_levels Acc_Level, std::size_t Metadata_Count>
class property_wrapper<object_ptr, C*, void, Acc_Level, get_as_ref_wrapper, read_only, Metadata_Count>
    : public property_wrapper_base, public metadata_handler<Metadata_Count>
{
    public:
        property_wrapper(string_view name, type declaring_type,
                         C* pointer, std::array<metadata, Metadata_Count> metadata_list) RTTR_NOEXCEPT
        :   property_wrapper_base(name, declaring_type),
            metadata_handler<Metadata_Count>(std::move(metadata_list)), m_accessor(pointer)
        {
            init();
        }

        access_levels get_access_level() const RTTR_NOEXCEPT    { return Acc_Level; }
        bool is_valid()     const RTTR_NOEXCEPT                 { return true;  }
        bool is_readonly()  const RTTR_NOEXCEPT                 { return true; }
        bool is_static()    const RTTR_NOEXCEPT                 { return true; }
        type get_type()     const RTTR_NOEXCEPT                 { return type::get< std::reference_wrapper< add_const_t<C>> >(); }

        variant get_metadata(const variant& key) const { return metadata_handler<Metadata_Count>::get_metadata(key); }

        bool set_value(instance& object, argument& arg) const
        {
            return false;
        }

        variant get_value(instance& object) const
        {
            return (variant(std::cref(*m_accessor)));
        }

    private:
        C* m_accessor;
};

#endif // RTTR_PROPERTY_WRAPPER_OBJECT_H_
