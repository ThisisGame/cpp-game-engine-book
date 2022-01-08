// eventpp library
// Copyright (C) 2018 Wang Qi (wqking)
// Github: https://github.com/wqking/eventpp
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//   http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef EVENTMAKER_H_378278023815
#define EVENTMAKER_H_378278023815

// auto generated macros begin

#define I_EVENTPP_ARG_COUNT(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, ...) p15
#define EVENTPP_ARG_COUNT(...) EVENTPP_EXPAND(I_EVENTPP_ARG_COUNT(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define I_EVENTPP_ITERATE_ARGS_0(EXEC, DELI)
#define I_EVENTPP_ITERATE_ARGS_1(EXEC, DELI, p0) I_EVENTPP_ITERATE_ARGS_0(EXEC, DELI ) EXEC(p0)
#define I_EVENTPP_ITERATE_ARGS_2(EXEC, DELI, p0, p1) I_EVENTPP_ITERATE_ARGS_1(EXEC, DELI, p0) DELI() EXEC(p1)
#define I_EVENTPP_ITERATE_ARGS_3(EXEC, DELI, p0, p1, p2) I_EVENTPP_ITERATE_ARGS_2(EXEC, DELI, p0, p1) DELI() EXEC(p2)
#define I_EVENTPP_ITERATE_ARGS_4(EXEC, DELI, p0, p1, p2, p3) I_EVENTPP_ITERATE_ARGS_3(EXEC, DELI, p0, p1, p2) DELI() EXEC(p3)
#define I_EVENTPP_ITERATE_ARGS_5(EXEC, DELI, p0, p1, p2, p3, p4) I_EVENTPP_ITERATE_ARGS_4(EXEC, DELI, p0, p1, p2, p3) DELI() EXEC(p4)
#define I_EVENTPP_ITERATE_ARGS_6(EXEC, DELI, p0, p1, p2, p3, p4, p5) I_EVENTPP_ITERATE_ARGS_5(EXEC, DELI, p0, p1, p2, p3, p4) DELI() EXEC(p5)
#define I_EVENTPP_ITERATE_ARGS_7(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6) I_EVENTPP_ITERATE_ARGS_6(EXEC, DELI, p0, p1, p2, p3, p4, p5) DELI() EXEC(p6)
#define I_EVENTPP_ITERATE_ARGS_8(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7) I_EVENTPP_ITERATE_ARGS_7(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6) DELI() EXEC(p7)
#define I_EVENTPP_ITERATE_ARGS_9(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8) I_EVENTPP_ITERATE_ARGS_8(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7) DELI() EXEC(p8)
#define I_EVENTPP_ITERATE_ARGS_10(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) I_EVENTPP_ITERATE_ARGS_9(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8) DELI() EXEC(p9)
#define I_EVENTPP_ITERATE_ARGS_11(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) I_EVENTPP_ITERATE_ARGS_10(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) DELI() EXEC(p10)
#define I_EVENTPP_ITERATE_ARGS_12(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) I_EVENTPP_ITERATE_ARGS_11(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) DELI() EXEC(p11)
#define I_EVENTPP_ITERATE_ARGS_13(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) I_EVENTPP_ITERATE_ARGS_12(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) DELI() EXEC(p12)
#define I_EVENTPP_ITERATE_ARGS_14(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) I_EVENTPP_ITERATE_ARGS_13(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) DELI() EXEC(p13)
#define I_EVENTPP_ITERATE_ARGS_15(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) I_EVENTPP_ITERATE_ARGS_14(EXEC, DELI, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) DELI() EXEC(p14)
#define I_EVENTPP_ITERATE_ARGS_N(EXEC, DELI, N, ...) EVENTPP_EXPAND(EVENTPP_CONCAT(I_EVENTPP_ITERATE_ARGS_, N) (EXEC, DELI, __VA_ARGS__))
#define EVENTPP_ITERATE_ARGS(EXEC, DELI, ...) I_EVENTPP_ITERATE_ARGS_N(EXEC, DELI, EVENTPP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

// auto generated macros end

#define I_EVENTPP_STRINGIFY(x) #x
#define EVENTPP_STRINGIFY(x) I_EVENTPP_STRINGIFY(x)

#define I_EVENTPP_CONCAT(a, b) a ## b
#define EVENTPP_CONCAT(a, b) I_EVENTPP_CONCAT(a, b)

// EVENTPP_EXPAND is used to expand __VA_ARGS__ correctly in VC++
#define EVENTPP_EXPAND(...) __VA_ARGS__
#define EVENTPP_BRACKETS_EXPAND(...) EVENTPP_EXPAND(EVENTPP_EXPAND __VA_ARGS__)

#define EVENTPP_EMPTY()
#define EVENTPP_COMMA() ,
#define EVENTPP_SEMICOLON() ;

#define I_EVENTPP_GET0_2(p0, ...) p0
#define I_EVENTPP_GET0(...) EVENTPP_EXPAND(I_EVENTPP_GET0_2(__VA_ARGS__, ))
#define EVENTPP_GET0(p0) EVENTPP_EXPAND(I_EVENTPP_GET0 p0)

#define I_EVENTPP_GET1_2(p0, p1, ...) p1
#define I_EVENTPP_GET1(...) EVENTPP_EXPAND(I_EVENTPP_GET1_2(__VA_ARGS__, ))
#define EVENTPP_GET1(p0) EVENTPP_EXPAND(I_EVENTPP_GET1 p0)

#define I_EVENTPP_GET2_2(p0, p1, p2, ...) p2
#define I_EVENTPP_GET2(...) EVENTPP_EXPAND(I_EVENTPP_GET2_2(__VA_ARGS__, ))
#define EVENTPP_GET2(p0) EVENTPP_EXPAND(I_EVENTPP_GET2 p0)

#define EVENTPP_MAKE_FIELD_NAME(p0) EVENTPP_CONCAT(v, EVENTPP_GET1(p0))
#define EVENTPP_EXEC_MAKE_FIELD(p0) EVENTPP_GET0(p0) EVENTPP_MAKE_FIELD_NAME(p0)
#define EVENTPP_EXEC_MAKE_GETTER(p0) const EVENTPP_GET0(p0) & EVENTPP_GET1(p0) () const { return EVENTPP_MAKE_FIELD_NAME(p0); }

#define I_EVENTPP_EXEC_MAKE_SETTER_0(p0)
#define I_EVENTPP_EXEC_MAKE_SETTER_1(p0)
#define I_EVENTPP_EXEC_MAKE_SETTER_2(p0)
#define I_EVENTPP_EXEC_MAKE_SETTER_3(p0) void EVENTPP_GET2(p0) (const EVENTPP_GET0(p0) & value) { EVENTPP_MAKE_FIELD_NAME(p0) = value; }
#define I_EVENTPP_EXEC_MAKE_SETTER_4(p0) I_EVENTPP_MAKE_SETTER_HELPER_3(p0)
#define I_EVENTPP_EXEC_MAKE_SETTER_5(p0) I_EVENTPP_MAKE_SETTER_HELPER_3(p0)
#define I_EVENTPP_EXEC_MAKE_SETTER_6(p0) I_EVENTPP_MAKE_SETTER_HELPER_3(p0)
#define I_EVENTPP_EXEC_MAKE_SETTER_N(N, p0) EVENTPP_EXPAND(EVENTPP_CONCAT(I_EVENTPP_EXEC_MAKE_SETTER_, N) (p0))
#define EVENTPP_EXEC_MAKE_SETTER(p0) EVENTPP_EXPAND(I_EVENTPP_EXEC_MAKE_SETTER_N(EVENTPP_ARG_COUNT EVENTPP_EXPAND(p0), p0))

#define EVENTPP_EXEC_MAKE_INITIALIZE(p0) EVENTPP_MAKE_FIELD_NAME(p0)(EVENTPP_MAKE_FIELD_NAME(p0))
#define EVENTPP_EXEC_MAKE_DEFAULT_INITIALIZE(p0) EVENTPP_MAKE_FIELD_NAME(p0)()
#define EVENTPP_EXEC_MAKE_CTOR_FIELD(p0) const EVENTPP_GET0(p0) & EVENTPP_MAKE_FIELD_NAME(p0)

#define I_EVENTPP_CHECK(x, n, ...) n
#define EVENTPP_CHECK(...) EVENTPP_EXPAND(I_EVENTPP_CHECK(__VA_ARGS__, 0, ))

#define I_EVENTPP_IS_ENCLOSED_BY_BRACKETS(...) x, 1
#define EVENTPP_IS_ENCLOSED_BY_BRACKETS(x) EVENTPP_CHECK(I_EVENTPP_IS_ENCLOSED_BY_BRACKETS x)

#define I_EVENTPP_IF_1(t, f) t
#define I_EVENTPP_IF_0(t, f) f
#define EVENTPP_IF(b, t, f) EVENTPP_CONCAT(I_EVENTPP_IF_, b)(t, f)

#define EVENTPP_ADD_BACKETS(x) EVENTPP_IF(EVENTPP_IS_ENCLOSED_BY_BRACKETS(x), x, (x))
#define EVENTPP_REMOVE_BACKETS(x) EVENTPP_BRACKETS_EXPAND(EVENTPP_IF(EVENTPP_IS_ENCLOSED_BY_BRACKETS(x), (EVENTPP_EXPAND x), (x)))

#define I_EVENTPP_MAKE_EVENT(className, baseClassName, baseClassArgs, ...) \
	class className : public EVENTPP_REMOVE_BACKETS(baseClassName) { \
		public: \
			className() \
				: EVENTPP_REMOVE_BACKETS(baseClassName) EVENTPP_ADD_BACKETS(baseClassArgs), \
					EVENTPP_ITERATE_ARGS(EVENTPP_EXEC_MAKE_DEFAULT_INITIALIZE, EVENTPP_COMMA, __VA_ARGS__) {} \
			className(EVENTPP_ITERATE_ARGS(EVENTPP_EXEC_MAKE_CTOR_FIELD, EVENTPP_COMMA, __VA_ARGS__)) \
				: EVENTPP_REMOVE_BACKETS(baseClassName) EVENTPP_ADD_BACKETS(baseClassArgs), \
					EVENTPP_ITERATE_ARGS(EVENTPP_EXEC_MAKE_INITIALIZE, EVENTPP_COMMA, __VA_ARGS__) {} \
			EVENTPP_ITERATE_ARGS(EVENTPP_EXEC_MAKE_GETTER, EVENTPP_EMPTY, __VA_ARGS__) \
			EVENTPP_ITERATE_ARGS(EVENTPP_EXEC_MAKE_SETTER, EVENTPP_EMPTY, __VA_ARGS__) \
		private: \
			EVENTPP_ITERATE_ARGS(EVENTPP_EXEC_MAKE_FIELD, EVENTPP_SEMICOLON, __VA_ARGS__); \
	}

#define EVENTPP_MAKE_EVENT(className, baseClassName, baseClassArgs, ...) EVENTPP_EXPAND(I_EVENTPP_MAKE_EVENT(className, baseClassName, baseClassArgs, __VA_ARGS__))

#define EVENTPP_MAKE_EMPTY_EVENT(className, baseClassName, baseClassArgs) \
	class className : public EVENTPP_REMOVE_BACKETS(baseClassName) { \
		public: \
			className() \
				: EVENTPP_REMOVE_BACKETS(baseClassName) EVENTPP_ADD_BACKETS(baseClassArgs) {} \
	}


#endif


/*
Perl script to generate some of the macros

use strict;
use warnings;

my $paramCount = 15;

sub outputParams
{
	my ($count) = @_;

	for(my $i = 0; $i < $count; ++$i) {
		print ', ' if($i > 0);
		print 'p', $i;
	}
}

sub doMain
{
	print '#define I_EVENTPP_ARG_COUNT(';
	&outputParams($paramCount + 1);
	print ', ...) p', $paramCount, "\n";
	print '#define EVENTPP_ARG_COUNT(...) EVENTPP_EXPAND(I_EVENTPP_ARG_COUNT(__VA_ARGS__';
	for(my $i = $paramCount; $i >= 0; --$i) {
		print ', ', $i;
	}
	print "))\n";
	print "\n";

	print "#define I_EVENTPP_ITERATE_ARGS_0(EXEC, DELI)\n";
	for(my $i = 1; $i <= $paramCount; ++$i) {
		print '#define I_EVENTPP_ITERATE_ARGS_', $i, '(EXEC, DELI, ';
		&outputParams($i);
		print ') I_EVENTPP_ITERATE_ARGS_', ($i - 1), '(EXEC, DELI';
		print ($i > 1 ? ', ' : ' ');
		&outputParams($i - 1);
		print ')', ($i > 1 ? ' DELI()' : ''), ' EXEC(p', ($i - 1), ')';
		print "\n";
	}
	print "#define I_EVENTPP_ITERATE_ARGS_N(EXEC, DELI, N, ...) EVENTPP_EXPAND(EVENTPP_CONCAT(I_EVENTPP_ITERATE_ARGS_, N) (EXEC, DELI, __VA_ARGS__))\n";
	print "#define EVENTPP_ITERATE_ARGS(EXEC, DELI, ...) I_EVENTPP_ITERATE_ARGS_N(EXEC, DELI, EVENTPP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)\n";
	print "\n";

}

&doMain;


*/
