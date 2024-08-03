//
// Created by adversarr on 2024/8/2.
//


// example
// class Node: public NodeBase {
// public:
//     CG_NODE_COMMON(Node, "Node Name", "Node desc");
//
//     CG_NODE_INPUTS(
//         CG_SOCKET(float, x, "Input x"),
//         CG_SOCKET(int, y, "Input y")
//     );
//
//     CG_NODE_OUTPUTS(
//         CG_SOCKET(float, z, "Output z")
//     );
// };

#include <cstddef>

// need something like boost pp.

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

#define APPLY_MACRO(type, name, desc, ith) (type, name, desc, ith)

#define CG_PP_EXPAND_FRONT_APPLY(x, i) \
    CG_PP_EVAL(APPLY_MACRO CG_PP_EMPTY() (i, CG_PP_TUPLE_UNPACK x))

#define CG_PP_FOR_EACH_I(...)          \
    CG_PP_VAOPT_FOR_EACH_I(CG_PP_EXPAND_FRONT_APPLY, __VA_ARGS__)

#define CG_NODE_INPUT_SOCKET_IMPL(Type, Name, desc, ith, ...) \
  template <> struct socket_meta<ith> {                       \
    using type = Type;                                        \
    static constexpr size_t index = ith;                      \
    static constexpr const char *name = #Name;                \
    static constexpr const char* description = desc;          \
  };                                                          \
  using Name##_t = socket_meta<ith>;                          \
  static constexpr Name##_t Name{};

#define CG_NODE_PP_ADAPTOR(x, i) \
    CG_PP_EVAL(CG_NODE_INPUT_SOCKET_IMPL CG_PP_EMPTY() (CG_PP_TUPLE_UNPACK x, i))

#define CG_NODE_INPUTS(...) \
    typedef struct intern_input_meta { \
      template<size_t I> struct socket_meta {\
        using type = void;\
      };\
      __VA_OPT__(CG_PP_VAOPT_FOR_EACH_I(CG_NODE_PP_ADAPTOR, __VA_ARGS__))\
    } in

#define CG_NODE_OUTPUTS(...) \
    typedef struct intern_output_meta { \
      template<size_t I> struct socket_meta {\
        using type = void;\
      };\
      __VA_OPT__(CG_PP_VAOPT_FOR_EACH_I(CG_NODE_PP_ADAPTOR, __VA_ARGS__))\
    } out

#define CG_NODE_COMMON(NodeType, Name, Desc)                                      \
    NodeType(NodeDescriptor const *descriptor) noexcept : NodeDerive<NodeType>(descriptor) {} \
    friend class NodeDescriptorBuilder<NodeType>;                                 \
    static constexpr const char* name = Name;                                     \
    static constexpr const char* description = Desc;

