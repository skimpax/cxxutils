#ifndef __CXXU_UTILS_H__
#define __CXXU_UTILS_H__

#include <stdint.h>

#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <chrono>
#include <typeinfo>
#include <type_traits>

#include <boost/mpl/if.hpp>

#include <timevault/config.h>
#include <cxxu/types.hpp>

// Black magic stolen from:
// http://stackoverflow.com/questions/1005476/
//  how-to-detect-whether-there-is-a-specific-member-variable-in-class
//
// Explanation:
// http://cpptalk.wordpress.com/2009/09/12/
//   substitution-failure-is-not-an-error-2/
#define CXXU_MAKE_HAS(WHAT)                                  \
template <typename T>                                      \
struct has_ ## WHAT {                                      \
    struct Fallback { int WHAT; };                         \
    struct Derived : T, Fallback { };                      \
                                                           \
    template<typename C, C> struct ChT;                    \
                                                           \
    template<typename C>                                   \
    static char (&f(ChT<int Fallback::*, &C::WHAT>*))[1];  \
                                                           \
    template<typename C>                                   \
    static char (&f(...))[2];                              \
                                                           \
    static bool const value = sizeof(f<Derived>(0)) == 2;  \
};

#define CXXU_MAKE_HAS_MEMBER(WHAT, PARAMS)                   \
template <typename K>                                      \
struct has_member_ ## WHAT                                 \
{                                                          \
    template <typename T, void(T::*)(PARAMS)>              \
    struct S;                                              \
                                                           \
    template <typename T>                                  \
    static One f(S<T, &T::WHAT>*);                         \
                                                           \
    template <typename T>                                  \
    static Two f(...);                                     \
                                                           \
    enum { value = sizeof(f<K>(0)) == sizeof(One) };       \
};

#define CXXU_AUTO_RET(x) -> decltype(x) { return x; }

namespace cxxu {

TIMEVAULT_API
bool interactive(void);
TIMEVAULT_API
void set_progress(std::size_t x, std::size_t n, std::size_t w = 50);
TIMEVAULT_API
void clear_progress(std::size_t w = 50);

TIMEVAULT_API
std::string home_directory(void);
TIMEVAULT_API
bool exists(const std::string& path);
TIMEVAULT_API
bool directory_exists(const std::string& path);
TIMEVAULT_API
bool file_exists(const std::string& path);
TIMEVAULT_API
bool touch_file(const std::string& path);
TIMEVAULT_API
bool truncate_file(const std::string& path, std::size_t size);
TIMEVAULT_API
bool copy_file(const std::string& from, const std::string& to);
TIMEVAULT_API
uint64_t file_size(const std::string& path);
TIMEVAULT_API
bool mkpath(const std::string& path);
TIMEVAULT_API
unsigned long rmpath(const std::string& path);
TIMEVAULT_API
bool rmfile(const std::string& path);
TIMEVAULT_API
bool mkfilepath(const std::string& path);
TIMEVAULT_API
void rename(const std::string& from_path, const std::string& to_path);
TIMEVAULT_API
std::string fullpath(const std::string& rel);
TIMEVAULT_API
std::string fullpath(const std::string& dir, const std::string& file);
TIMEVAULT_API
std::string basename(const std::string& path);
TIMEVAULT_API
std::string dirname(const std::string& path);
TIMEVAULT_API
unsigned long long human_readable_size(const std::string& expr);

TIMEVAULT_API
void split(const std::string& re, const std::string& expr, cxxu::string_list& list);

TIMEVAULT_API
void chomp(std::string& s);

TIMEVAULT_API
void unquote(std::string& s);

TIMEVAULT_API
cxxu::string_list glob(const std::string& expr);

TIMEVAULT_API
bool match(const std::string& expr, const std::string& re);

TIMEVAULT_API bool
extract_delimited(
    std::string& from,
    std::string& to,
    unsigned char delim = ';',
    unsigned char quote = '\0'
);

TIMEVAULT_API
std::string demangle_type_name(const std::string& mangled);
TIMEVAULT_API
std::string md5sum(const std::string& file);
TIMEVAULT_API
std::string escape(const char c);
TIMEVAULT_API
std::string escape_for_string(const char c);
TIMEVAULT_API
std::string escape(const std::string& s);

template <typename T>
inline
std::string
type_info(const T& t)
{ return demangle_type_name(typeid(t).name()); }

template <typename T>
inline
std::string
type_info(void)
{ return demangle_type_name(typeid(T).name()); }

inline
cxxu::string_list
split(const std::string& re, const std::string& expr)
{
    cxxu::string_list list;
    split(re, expr, list);

    return std::move(list);
}

template <typename S>
inline
std::string
join(const std::string& sep, const std::vector<S>& v)
{
    std::string joined;

    if (v.size() > 0) {
        joined = v[0];
    }

    for (std::size_t i = 1; i < v.size(); i++) {
        joined += sep;
        joined += v[i];
    }

    return std::move(joined);
}

inline
std::string
indent(const std::string& what)
{
    std::string sep = "\n";
    std::string pad = "    ";
    std::string s;

    if (what.size() > 0) {
        s = pad + join(sep + pad, split(sep, what));
    }

    return std::move(s);
}

template <typename T>
struct make_vector {
    typedef std::vector<T> vector_type;

    static
    void make(vector_type& v)
    {}

    template <typename Item, typename... Items>
    static
    void make(vector_type& v, Item& i, Items... items)
    {
        v.push_back(T(i));
        make(v, items...);
    }
};

template <typename... Args>
std::string
join(const std::string& sep, Args... args)
{
    std::vector<std::string> v;
    make_vector<std::string>::make(v, args...);

    return join(sep, v);
}

template <typename... Args>
std::string
cat_dir(Args... args)
{
    std::vector<std::string> v;
    make_vector<std::string>::make(v, args...);

    return join("/", v);
}

template <typename... Args>
std::string
cat_file(Args... args)
{ return cat_dir(std::forward<Args>(args)...); }

template <typename T>
inline
T to_num(const std::string& s)
{
    return ((T) strtoull(s.c_str(), NULL, 10));
}

// Number of bits used by v
template <typename T>
inline
__attribute__ ((const))
uint32_t
used_bits(const T v)
{
    if (v == 0) {
        return 0;
    }

    T index;

    __asm__("bsr %1, %0;" :"=r"(index) :"r"(v));

    return index + 1;
}

template <>
inline
__attribute__ ((const))
uint32_t
used_bits<int8_t>(const int8_t v)
{ return used_bits((int16_t) v); }

template <>
inline
__attribute__ ((const))
uint32_t
used_bits<uint8_t>(const uint8_t v)
{ return used_bits((uint16_t) v); }

inline
uint32_t
byte_size_of(uint32_t bits, uint32_t count)
{
    uint32_t bit_size = bits * count;

    return (bit_size / 8) + ((bit_size % 8) != 0 ? 1 : 0);
}

inline
uint32_t
byte_size_of(uint32_t bits)
{
    return byte_size_of(bits, 1);
}

template <typename T>
inline
void
inc_ptr(T*& ptr, std::size_t bytes)
{
    ptr = (T*)(
        ((uintptr_t) (ptr)) + bytes
    );
}

template <typename T>
inline
void
dec_ptr(T*& ptr, std::size_t bytes)
{
    ptr = (T*)(
        ((uintptr_t) (ptr)) - bytes
    );
}

template <typename R, typename T>
inline
R*
inc_ptr(T*& ptr, std::size_t bytes)
{
    return (R*)(
        ((uintptr_t) (ptr)) + bytes
    );
}

template <typename T>
inline
__attribute__ ((const))
T*
align_to_128bits(T* ptr)
{
    return (T*)(
        (((uintptr_t) (ptr)) + 15) & ~15
    );
}

template <typename T>
inline
__attribute__ ((const))
const T*
align_to_128bits(const T* ptr)
{
    return (const T*)(
        (((uintptr_t) (ptr)) + 15) & ~15
    );
}

inline
__attribute__ ((const))
std::size_t
align_to_128bits(std::size_t size_in_bytes)
{
    return ((size_in_bytes + 15) & ~15);
}

template <typename T>
inline
__attribute__ ((const))
std::size_t
aligned_sizeof(T& t)
{
    return align_to_128bits(sizeof(T));
}

template <typename T>
inline
__attribute__ ((const))
std::size_t
aligned_sizeof(const T& t)
{
    return align_to_128bits(sizeof(T));
}

inline
__attribute__ ((const))
std::size_t
align_to_128(std::size_t count)
{
    return ((count + 127) & ~127);
}

template <std::size_t Size>
struct make_aligned {
    static const std::size_t value = ((Size + 15) & ~15);
};

template <
    typename T,
    typename = typename std::enable_if<
        std::is_arithmetic<T>::value
    >::type
>
inline
auto
as_printable(T i) -> decltype(+i)
{ return +i; }

template <typename T>
inline
const T&
as_printable(
    const T& s,
    typename std::enable_if<
        !std::is_arithmetic<T>::value
    >::type* = 0
)
{ return s; }

template <typename T>
constexpr
typename std::underlying_type<T>::type
integral(T value)
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

template <typename T>
inline
std::size_t
digit_count(T v)
{
    return
        v == 0
        ? 1
        : ((std::size_t) std::log10(v)) + 1
        ;
}

template <typename C>
struct extract_iterator {
    typedef typename boost::mpl::if_<
        std::is_const<C>,
        typename C::const_iterator,
        typename C::iterator
    >::type type;
};

template <typename C>
struct extract_reference {
    typedef typename extract_iterator<C>::type::reference type;
};

template <typename C>
struct extract_value {
    typedef typename std::remove_reference<
        typename extract_reference<C>::type
    >::type type;
};

template <typename V>
struct make_const_reference {
    typedef const typename std::remove_cv<
        typename std::remove_reference<V>::type
    >::type& type;
};

} // namespace cxxu

#endif // __CXXU_UTILS_H__
