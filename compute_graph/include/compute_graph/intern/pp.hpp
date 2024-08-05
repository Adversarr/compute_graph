// The MIT License (MIT)
// Copyright © 2024 Adversarr
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once


#define CG_PP_THIRD_ARG(a,b,c,...) c
#define CG_VA_OPT_SUPPORTED_I(...) CG_PP_THIRD_ARG(__VA_OPT__(,),true,false,)
#define CG_VA_OPT_SUPPORTED CG_VA_OPT_SUPPORTED_I(?)
#if !CG_VA_OPT_SUPPORTED
#  error "VA_OPT is not supported by this compiler"
#endif


#define CG_PP_VAOPT_SIZE(...) CG_PP_VAOPT_SIZE_I(toooo_many, ##__VA_ARGS__, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define CG_PP_VAOPT_SIZE_I(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, N, ...) N

#define CG_PP_VAOPT_FOR_EACH_I_0(macro, ...)
#define CG_PP_VAOPT_FOR_EACH_I_1(macro, _0) macro(_0, 0)
#define CG_PP_VAOPT_FOR_EACH_I_2(macro, _0, _1) CG_PP_VAOPT_FOR_EACH_I_1(macro, _0) macro(_1, 1)
#define CG_PP_VAOPT_FOR_EACH_I_3(macro, _0, _1, _2) CG_PP_VAOPT_FOR_EACH_I_2(macro, _0, _1) macro(_2, 2)
#define CG_PP_VAOPT_FOR_EACH_I_4(macro, _0, _1, _2, _3) CG_PP_VAOPT_FOR_EACH_I_3(macro, _0, _1, _2) macro(_3, 3)
#define CG_PP_VAOPT_FOR_EACH_I_5(macro, _0, _1, _2, _3, _4) CG_PP_VAOPT_FOR_EACH_I_4(macro, _0, _1, _2, _3) macro(_4, 4)
#define CG_PP_VAOPT_FOR_EACH_I_6(macro, _0, _1, _2, _3, _4, _5) CG_PP_VAOPT_FOR_EACH_I_5(macro, _0, _1, _2, _3, _4) macro(_5, 5)
#define CG_PP_VAOPT_FOR_EACH_I_7(macro, _0, _1, _2, _3, _4, _5, _6) CG_PP_VAOPT_FOR_EACH_I_6(macro, _0, _1, _2, _3, _4, _5) macro(_6, 6)
#define CG_PP_VAOPT_FOR_EACH_I_8(macro, _0, _1, _2, _3, _4, _5, _6, _7) CG_PP_VAOPT_FOR_EACH_I_7(macro, _0, _1, _2, _3, _4, _5, _6) macro(_7, 7)
#define CG_PP_VAOPT_FOR_EACH_I_9(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8) CG_PP_VAOPT_FOR_EACH_I_8(macro, _0, _1, _2, _3, _4, _5, _6, _7) macro(_8, 8)
#define CG_PP_VAOPT_FOR_EACH_I_10(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9) CG_PP_VAOPT_FOR_EACH_I_9(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8) macro(_9, 9)
#define CG_PP_VAOPT_FOR_EACH_I_11(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) CG_PP_VAOPT_FOR_EACH_I_10(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9) macro(_10, 10)
#define CG_PP_VAOPT_FOR_EACH_I_12(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) CG_PP_VAOPT_FOR_EACH_I_11(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) macro(_11, 11)
#define CG_PP_VAOPT_FOR_EACH_I_13(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) CG_PP_VAOPT_FOR_EACH_I_12(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) macro(_12, 12)
#define CG_PP_VAOPT_FOR_EACH_I_14(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) CG_PP_VAOPT_FOR_EACH_I_13(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) macro(_13, 13)
#define CG_PP_VAOPT_FOR_EACH_I_15(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) CG_PP_VAOPT_FOR_EACH_I_14(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) macro(_14, 14)
#define CG_PP_VAOPT_FOR_EACH_I_16(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) CG_PP_VAOPT_FOR_EACH_I_15(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) macro(_15, 15)
#define CG_PP_VAOPT_FOR_EACH_I_17(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) CG_PP_VAOPT_FOR_EACH_I_16(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) macro(_16, 16)
#define CG_PP_VAOPT_FOR_EACH_I_18(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) CG_PP_VAOPT_FOR_EACH_I_17(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) macro(_17, 17)
#define CG_PP_VAOPT_FOR_EACH_I_19(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) CG_PP_VAOPT_FOR_EACH_I_18(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) macro(_18, 18)
#define CG_PP_VAOPT_FOR_EACH_I_20(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) CG_PP_VAOPT_FOR_EACH_I_19(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) macro(_19, 19)
#define CG_PP_VAOPT_FOR_EACH_I_21(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) CG_PP_VAOPT_FOR_EACH_I_20(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) macro(_20, 20)
#define CG_PP_VAOPT_FOR_EACH_I_22(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) CG_PP_VAOPT_FOR_EACH_I_21(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) macro(_21, 21)
#define CG_PP_VAOPT_FOR_EACH_I_23(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) CG_PP_VAOPT_FOR_EACH_I_22(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) macro(_22, 22)
#define CG_PP_VAOPT_FOR_EACH_I_24(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) CG_PP_VAOPT_FOR_EACH_I_23(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) macro(_23, 23)
#define CG_PP_VAOPT_FOR_EACH_I_25(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) CG_PP_VAOPT_FOR_EACH_I_24(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) macro(_24, 24)
#define CG_PP_VAOPT_FOR_EACH_I_26(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) CG_PP_VAOPT_FOR_EACH_I_25(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) macro(_25, 25)
#define CG_PP_VAOPT_FOR_EACH_I_27(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) CG_PP_VAOPT_FOR_EACH_I_26(macro, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) macro(_26, 26)


#define CG_PP_NTH_ARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, N, ...) N

#define CG_PP_VAOPT_FOR_EACH_I(macro, ...)  \
    CG_PP_NTH_ARG(~, ##__VA_ARGS__,  \
        CG_PP_VAOPT_FOR_EACH_I_27,          \
        CG_PP_VAOPT_FOR_EACH_I_26,          \
        CG_PP_VAOPT_FOR_EACH_I_25,          \
        CG_PP_VAOPT_FOR_EACH_I_24,          \
        CG_PP_VAOPT_FOR_EACH_I_23,          \
        CG_PP_VAOPT_FOR_EACH_I_22,          \
        CG_PP_VAOPT_FOR_EACH_I_21,          \
        CG_PP_VAOPT_FOR_EACH_I_20,          \
        CG_PP_VAOPT_FOR_EACH_I_19,          \
        CG_PP_VAOPT_FOR_EACH_I_18,          \
        CG_PP_VAOPT_FOR_EACH_I_17,          \
        CG_PP_VAOPT_FOR_EACH_I_16,          \
        CG_PP_VAOPT_FOR_EACH_I_15,          \
        CG_PP_VAOPT_FOR_EACH_I_14,          \
        CG_PP_VAOPT_FOR_EACH_I_13,          \
        CG_PP_VAOPT_FOR_EACH_I_12,          \
        CG_PP_VAOPT_FOR_EACH_I_11,          \
        CG_PP_VAOPT_FOR_EACH_I_10,          \
        CG_PP_VAOPT_FOR_EACH_I_9,           \
        CG_PP_VAOPT_FOR_EACH_I_8,           \
        CG_PP_VAOPT_FOR_EACH_I_7,           \
        CG_PP_VAOPT_FOR_EACH_I_6,           \
        CG_PP_VAOPT_FOR_EACH_I_5,           \
        CG_PP_VAOPT_FOR_EACH_I_4,           \
        CG_PP_VAOPT_FOR_EACH_I_3,           \
        CG_PP_VAOPT_FOR_EACH_I_2,           \
        CG_PP_VAOPT_FOR_EACH_I_1,           \
        CG_PP_VAOPT_FOR_EACH_I_0) (macro, ##__VA_ARGS__)

#define CG_PP_TUPLE_UNPACK(...) __VA_ARGS__

#define CG_PP_EMPTY()
#define CG_PP_DEFER(id) id CG_PP_EMPTY()
#define CG_PP_EVAL(...) CG_PP_EVAL1(CG_PP_EVAL1(CG_PP_EVAL1(__VA_ARGS__)))
#define CG_PP_EVAL1(...) CG_PP_EVAL2(CG_PP_EVAL2(CG_PP_EVAL2(__VA_ARGS__)))
#define CG_PP_EVAL2(...) CG_PP_EVAL3(CG_PP_EVAL3(CG_PP_EVAL3(__VA_ARGS__)))
#define CG_PP_EVAL3(...) CG_PP_EVAL4(CG_PP_EVAL4(CG_PP_EVAL4(__VA_ARGS__)))
#define CG_PP_EVAL4(...) __VA_ARGS__

#define CG_PP_FOR_EACH_I(...)          \
    CG_PP_VAOPT_FOR_EACH_I(CG_PP_EXPAND_FRONT_APPLY, __VA_ARGS__)
