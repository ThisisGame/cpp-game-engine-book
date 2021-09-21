####################################################################################
#                                                                                  #
#  Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           #
#                                                                                  #
#  This file is part of RTTR (Run Time Type Reflection)                            #
#  License: MIT License                                                            #
#                                                                                  #
#  Permission is hereby granted, free of charge, to any person obtaining           #
#  a copy of this software and associated documentation files (the "Software"),    #
#  to deal in the Software without restriction, including without limitation       #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,        #
#  and/or sell copies of the Software, and to permit persons to whom the           #
#  Software is furnished to do so, subject to the following conditions:            #
#                                                                                  #
#  The above copyright notice and this permission notice shall be included in      #
#  all copies or substantial portions of the Software.                             #
#                                                                                  #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
#  SOFTWARE.                                                                       #
#                                                                                  #
####################################################################################

set(HEADER_FILES access_levels.h
                 argument.h
                 array_range.h
                 associative_mapper.h
                 constructor.h
                 destructor.h
                 enumeration.h
                 enum_flags.h
                 filter_item.h
                 instance.h
                 library.h
                 method.h
                 policy.h
                 property.h
                 parameter_info.h
                 registration
                 registration_friend
                 registration.h
                 registration_friend.h
                 sequential_mapper.h
                 string_view.h
                 rttr_cast.h
                 rttr_enable.h
                 type
                 type.h
                 variant.h
                 variant_associative_view.h
                 variant_sequential_view.h
                 wrapper_mapper.h
                 detail/base/core_prerequisites.h
                 detail/base/version.h.in
                 detail/base/version.rc.in
                 detail/comparison/comparable_types.h
                 detail/comparison/compare_array_less.h
                 detail/comparison/compare_array_less_impl.h
                 detail/comparison/compare_array_equal.h
                 detail/comparison/compare_array_equal_impl.h
                 detail/comparison/compare_equal.h
                 detail/comparison/compare_equal_impl.h
                 detail/comparison/compare_less.h
                 detail/comparison/compare_less_impl.h
                 detail/constructor/constructor_invoker.h
                 detail/constructor/constructor_wrapper.h
                 detail/constructor/constructor_wrapper_defaults.h
                 detail/constructor/constructor_wrapper_base.h
                 detail/conversion/std_conversion_functions.h
                 detail/conversion/number_conversion.h
                 detail/default_arguments/default_arguments.h
                 detail/default_arguments/invoke_with_defaults.h
                 detail/destructor/destructor_wrapper.h
                 detail/destructor/destructor_wrapper_base.h
                 detail/enumeration/enumeration_helper.h
                 detail/enumeration/enumeration_wrapper.h
                 detail/enumeration/enumeration_wrapper_base.h
                 detail/enumeration/enum_data.h
                 detail/filter/filter_item_funcs.h
                 detail/impl/argument_impl.h
                 detail/impl/array_range_impl.h
                 detail/impl/associative_mapper_impl.h
                 detail/impl/enum_flags_impl.h
                 detail/impl/instance_impl.h
                 detail/impl/rttr_cast_impl.h
                 detail/impl/sequential_mapper_impl.h
                 detail/impl/string_view_impl.h
                 detail/impl/wrapper_mapper_impl.h
                 detail/library/library_p.h
                 detail/metadata/metadata.h
                 detail/metadata/metadata_handler.h
                 detail/method/method_accessor.h
                 detail/method/method_invoker.h
                 detail/method/method_wrapper.h
                 detail/method/method_wrapper_base.h
                 detail/misc/argument_wrapper.h
                 detail/misc/argument_extractor.h
                 detail/misc/class_item_mapper.h
                 detail/misc/data_address_container.h
                 detail/misc/flat_map.h
                 detail/misc/flat_multimap.h
                 detail/misc/function_traits.h
                 detail/misc/iterator_wrapper.h
                 detail/misc/misc_type_traits.h
                 detail/misc/register_wrapper_mapper_conversion.h
                 detail/misc/sequential_container_type_traits.h
                 detail/misc/std_type_traits.h
                 detail/misc/template_type_trait.h
                 detail/misc/template_type_trait_impl.h
                 detail/misc/utility.h
                 detail/parameter_info/parameter_infos.h
                 detail/parameter_info/parameter_infos_compare.h
                 detail/parameter_info/parameter_names.h
                 detail/parameter_info/parameter_info_wrapper_base.h
                 detail/parameter_info/parameter_info_wrapper.h
                 detail/policies/prop_policies.h
                 detail/policies/meth_policies.h
                 detail/policies/ctor_policies.h
                 detail/property/property_accessor.h
                 detail/property/property_wrapper.h
                 detail/property/property_wrapper_base.h
                 detail/property/property_wrapper_func.h
                 detail/property/property_wrapper_member_func.h
                 detail/property/property_wrapper_member_object.h
                 detail/property/property_wrapper_object.h
                 detail/registration/bind_types.h
                 detail/registration/bind_impl.h
                 detail/registration/register_base_class_from_accessor.h
                 detail/registration/registration_impl.h
                 detail/registration/registration_executer.h
                 detail/registration/registration_manager.h
                 detail/registration/registration_state_saver.h
                 detail/type/accessor_type.h
                 detail/type/base_classes.h
                 detail/type/get_create_variant_func.h
                 detail/type/get_derived_info_func.h
                 detail/type/type_converter.h
                 detail/type/type_comparator.h
                 detail/type/type_data.h
                 detail/type/type_register.h
                 detail/type/type_impl.h
                 detail/type/type_name.h
                 detail/type/type_register_p.h
                 detail/type/type_string_utils.h
                 detail/variant/variant_compare.h
                 detail/variant/variant_data.h
                 detail/variant/variant_data_converter.h
                 detail/variant/variant_data_policy.h
                 detail/variant/variant_impl.h
                 detail/variant_associative_view/variant_associative_view_private.h
                 detail/variant_associative_view/variant_associative_view_creator.h
                 detail/variant_associative_view/variant_associative_view_creator_impl.h
                 detail/variant_sequential_view/variant_sequential_view_private.h
                 detail/variant_sequential_view/variant_sequential_view_creator_impl.h
                 detail/variant_sequential_view/variant_sequential_view_creator.h
                )

set(SOURCE_FILES constructor.cpp
                 destructor.cpp
                 enumeration.cpp
                 library.cpp
                 method.cpp
                 parameter_info.cpp
                 policy.cpp
                 property.cpp
                 registration.cpp
                 type.cpp
                 variant.cpp
                 variant_associative_view.cpp
                 variant_sequential_view.cpp
                 detail/comparison/compare_equal.cpp
                 detail/comparison/compare_less.cpp
                 detail/misc/standard_types.cpp
                 detail/conversion/std_conversion_functions.cpp
                 detail/constructor/constructor_wrapper_base.cpp
                 detail/destructor/destructor_wrapper_base.cpp
                 detail/enumeration/enumeration_helper.cpp
                 detail/enumeration/enumeration_wrapper_base.cpp
                 detail/library/library_win.cpp
                 detail/library/library_unix.cpp
                 detail/method/method_wrapper_base.cpp
                 detail/parameter_info/parameter_info_wrapper_base.cpp
                 detail/property/property_wrapper_base.cpp
                 detail/registration/registration_executer.cpp
                 detail/registration/registration_state_saver.cpp
                 detail/type/type_data.cpp
                 detail/type/type_register.cpp
                 detail/variant/variant_compare.cpp
                 )
