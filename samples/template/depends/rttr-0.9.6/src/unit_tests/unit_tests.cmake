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

set(HEADER_FILES test_classes.h
                 method/test_method_reflection.h
                 type/test_type.h
                 variant/test_enums.h
                 version.rc.in
                 )

set(SOURCE_FILES main.cpp
                 misc/test_misc.cpp
                 misc/array_range_test.cpp
                 misc/string_view_test.cpp
                 misc/enum_flags_test.cpp
                 misc/library_test.cpp
                 property/property_access_level_test.cpp
                 property/property_misc_test.cpp
                 property/property_class_inheritance.cpp
                 property/property_class_invoke_wrapper.cpp
                 property/property_member_function.cpp
                 property/property_member_object.cpp
                 property/property_global_function.cpp
                 property/property_global_object.cpp
                 type/test_type.cpp
                 type/test_type_names.cpp
                 type/type_prop_meth_invoke.cpp
                 destructor/destructor_invoke_test.cpp
                 destructor/destructor_misc_test.cpp
                 constructor/constructor_access_level_test.cpp
                 constructor/constructor_default_arg_test.cpp
                 constructor/constructor_param_info_test.cpp
                 constructor/constructor_retrieve_test.cpp
                 constructor/constructor_invoke_test.cpp
                 constructor/constructor_misc_test.cpp
                 constructor/constructor_query_test.cpp
                 enumeration/enumeration_conversion.cpp
                 enumeration/enumeration_misc.cpp
                 instance/instance_test.cpp
                 method/method_invoke_defaults_test.cpp
                 method/method_access_level_test.cpp
                 method/test_method_reflection.cpp
                 method/method_default_arg_test.cpp
                 method/method_misc_test.cpp
                 method/method_invoke_test.cpp
                 method/method_param_info_test.cpp
                 method/method_query_test.cpp
                 variant/variant_assign_test.cpp
                 variant/variant_conv_test.cpp
                 variant/variant_ctor_test.cpp
                 variant/variant_cmp_equal_test.cpp
                 variant/variant_cmp_less_test.cpp
                 variant/variant_cmp_greater_test.cpp
                 variant/variant_cmp_less_or_equal.cpp
                 variant/variant_cmp_greater_or_equal.cpp
                 variant/variant_misc_test.cpp
                 variant/variant_cast_test.cpp
                 variant/variant_conv_to_bool.cpp
                 variant/variant_conv_to_int8.cpp
                 variant/variant_conv_to_int16.cpp
                 variant/variant_conv_to_int32.cpp
                 variant/variant_conv_to_int64.cpp
                 variant/variant_conv_to_uint8.cpp
                 variant/variant_conv_to_uint16.cpp
                 variant/variant_conv_to_uint32.cpp
                 variant/variant_conv_to_uint64.cpp
                 variant/variant_conv_to_double.cpp
                 variant/variant_conv_to_float.cpp
                 variant/variant_conv_to_string.cpp
                 variant/variant_conv_to_enum.cpp
                 variant_associative_view/variant_associative_view_test.cpp
                 variant_sequential_view/variant_sequential_view_test.cpp
                 )
