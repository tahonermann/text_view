#if !defined(GRAMMATICAL_ASSERT_HPP)
#define GRAMMATICAL_ASSERT_HPP


#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/elem.hpp>


/*
 * These macros are used to produce a function concept definition that returns
 * true when given a semantically well-formed expression and false otherwise.
 * The expression to be tested must be a syntactically valid expression that
 * could be well-formed for some substitution of types (N4040 3.1.2.1p2).
 *
 * For example, given the following macro invocation,
 *   GRAMMATICAL_ASSERT_CONCEPT(f(x,y), (((int),x)) (((long),y)))
 * a function concept definition similar to the following will be produced.
 * The function concept will return true if the expression f(x,y) with x
 * having type int and y having type long is well-formed (ie, if an overload
 * of f() that accepts those argument types exists).
 *   template<typename T0 = void, typename T1 = int, typename T2 = long>
 *   concept bool grammatical_assert_NNN() {
 *       return requires(T0, T1 x, T2 y)
 *              {
 *                  f(x,y);
 *              };
 *   }
 * The first template parameter is present to allow testing expressions that
 * do not require local arguments.  The use of template parameters for each
 * of the local argument types ensures that the expression will be dependent.
 */
#define GRAMMATICAL_ASSERT_TEMPLATE_PARMS(r, data, i, elem)                    \
    (typename T ## i = BOOST_PP_TUPLE_ELEM(1,0,BOOST_PP_TUPLE_ELEM(2, 0, elem)))
#define GRAMMATICAL_ASSERT_REQUIRES_PARMS(r, data, i, elem)                    \
    (T ## i BOOST_PP_TUPLE_ELEM(2, 1, elem))
#define GRAMMATICAL_ASSERT_CONCEPT(name, expr, seq)                            \
    template<BOOST_PP_SEQ_ENUM(                                                \
                 BOOST_PP_SEQ_FOR_EACH_I(                                      \
                     GRAMMATICAL_ASSERT_TEMPLATE_PARMS, _, seq))>              \
    concept bool name() {                                                      \
        return requires(BOOST_PP_SEQ_ENUM(                                     \
                            BOOST_PP_SEQ_FOR_EACH_I(                           \
                                GRAMMATICAL_ASSERT_REQUIRES_PARMS,             \
                                _,                                             \
                                seq)))                                         \
               {                                                               \
                   expr;                                                       \
               };                                                              \
    }

/*
 * These macros are helpers for the macros that follow.  They exist to allow a
 * name that can only be generated once to be used multiple times (ie, to allow
 * a name produced using __COUNTER__ to be named more than once).  The optional
 * type sequence is augmented with a prefixed void type to ensure that a
 * non-empty sequence of types is provided to be used as template parameters to
 * the function concept that is generated.
 */
#define ILL_FORMED_GRAMMATICAL_ASSERT(name, expr, ...)                         \
    GRAMMATICAL_ASSERT_CONCEPT(name, expr, (((void),)) __VA_ARGS__)            \
    static_assert(! name<>(), "");
#define WELL_FORMED_GRAMMATICAL_ASSERT(name, expr, ...)                        \
    GRAMMATICAL_ASSERT_CONCEPT(name, expr, (((void),)) __VA_ARGS__)            \
    static_assert(name<>(), "");

/*
 * These macros are used to statically assert that an expression is semantically
 * well-formed or semantically ill-formed.  'expr' must be a syntactically
 * well-formed expression.  An optional argument supplies the types for
 * identifiers referenced by the expression that are not intended to resolve to
 * names of namespace scope declarations (The list of types and identifiers is
 * used to declare local variables for testing the specified expression).  The
 * type associations are expressed as a sequence of tuples where the first tuple
 * element specifies the type for the variable and the second element specifies
 * the name.  The type must be enclosed in its own set of parenthsis so that
 * type names that include commas (,) will not interfere with the preprocess.
 * For example,
 *   (((int),i)) (((std::vector<int>),vi))
 * specifies that, for the provided expression, references to 'i' correspond to
 * a (local) variable of type int and references to 'vi' correspond to a (local)
 * variable of type std::vector<int>.  Such an expression might be:
 *   f(vi,i)
 */
#define ILL_FORMED_EXPRESSION_ASSERT(expr, ...)                                \
    ILL_FORMED_GRAMMATICAL_ASSERT(                                             \
        BOOST_PP_CAT(unexpectedly_well_formed_expression_, __COUNTER__),       \
        expr,                                                                  \
       __VA_ARGS__)
#define WELL_FORMED_EXPRESSION_ASSERT(expr, ...)                               \
    WELL_FORMED_GRAMMATICAL_ASSERT(                                            \
        BOOST_PP_CAT(unexpectedly_ill_formed_expression_, __COUNTER__),        \
        expr,                                                                  \
       __VA_ARGS__)


#endif // GRAMMATICAL_ASSERT_HPP
