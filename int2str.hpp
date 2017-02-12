#pragma once

#include <cstddef>
#include <limits>
#include <type_traits>

namespace int2str
{
namespace impl
{

typedef unsigned long long number_t;

template<typename T>
constexpr number_t get_max_divider(number_t n = 1)
{
    return (std::numeric_limits<T>::max() / n <= 9 ? n : get_max_divider<T>(n * 10));
}

template<typename T>
struct next_type
{
    typedef unsigned long long type;
};

template<>
struct next_type<unsigned char>
{
    typedef unsigned short type;
};

template<>
struct next_type<unsigned short>
{
    typedef unsigned int type;
};

template<>
struct next_type<unsigned int>
{
    typedef typename std::conditional<(
        sizeof(unsigned long) > sizeof(unsigned int)), 
        unsigned long, 
        unsigned long long>::type type;
};

template<number_t N>
struct detail
{
    template<typename Iter>
    inline static Iter convert(number_t x, Iter iter)
    {
        if (N > x)
            return detail<N / 10u>::convert(x, iter);
        return detail<N>::convert_step(x, iter);
    }
    
    template<typename Iter>
    inline static Iter convert_step(number_t x, Iter iter)
    {
        if (N > x)
        {
            *iter++ = '0';
            return detail<N / 10u>::convert_step(x, iter);
        }
        auto const w = x / N;
        *iter++ = static_cast<char>('0' + w);
        return detail<N / 10u>::convert_step(x - w * N, iter);
    }
};

template<>
struct detail<1u>
{
    template<typename Iter>
    inline static Iter convert(number_t x, Iter iter)
    {
        return convert_step(x, iter);
    }
        
    template<typename Iter>
    inline static Iter convert_step(number_t x, Iter iter)
    {
        *iter++ = static_cast<char>('0' + x);
        return iter;
    }
};

template<typename FromT=unsigned char, typename T, typename Iter>
inline Iter convert_from(T x, Iter iter)
{
    if (x <= std::numeric_limits<FromT>::max())
        return detail<get_max_divider<FromT>()>::convert(x, iter);
    return convert_from<typename next_type<FromT>::type>(x, iter);
}

template<typename T, typename Enable=void>
struct converter
{
    template<typename Iter>
    inline static Iter run(T x, Iter iter)
    {
        return convert_from(x, iter);
    }
};

template<typename T>
struct converter<T, typename std::enable_if<std::is_signed<T>::value>::type>
{
    template<typename Iter>
    inline static Iter run(T x, Iter iter)
    {
        using U = typename std::make_unsigned<T>::type;
        if (x < 0)
        {
            *iter++ = '-';
            return convert_from(static_cast<U>(-x), iter);
        }
        return convert_from(static_cast<U>(x), iter);
    }
};

} // namespace impl

template<typename T, typename Iter>
inline Iter convert(T x, Iter iter)
{    
    static_assert(std::is_integral<T>::value, "T must be integral type");
    return impl::converter<typename std::decay<T>::type>::run(x, iter);
}

template<typename T, typename Iter>
inline Iter convert_with_zero(T x, Iter iter)
{    
    iter = convert(x, iter);
    *iter++ = 0;
    return iter;
}

} // namespace int2str
