#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <sstream>
#include <cstdio>
#include <array>
#include <cstring>
#include <typeinfo>
#include <random>
#include "int2str.hpp"
#ifdef BOOST_LEXICAL_CAST
#include <boost/lexical_cast.hpp>
#endif

template<typename Duration>
class Profiler
{
public:

    Profiler(Duration& result)
        : mResult(result),
        mStart(Clock::now())
    {}

    ~Profiler()
    {
        mResult = std::chrono::duration_cast<Duration>(Clock::now() - mStart);
    }

    Profiler(Profiler const&) = delete;
    Profiler(Profiler&&) = delete;
    Profiler& operator = (Profiler const&) = delete;
    Profiler& operator = (Profiler&&) = delete;

private:

    typedef std::chrono::steady_clock Clock;
    Duration& mResult;
    Clock::time_point mStart;
};

#define RANDOM_PROFILING_BEGIN(msg) \
{ \
    typedef std::chrono::duration<double> ResultType; \
    std::cout << (msg) << "... " << std::flush; \
    ResultType _result; \
    { \
        typedef int number_t; \
        auto const _imin = std::numeric_limits<number_t>::min(); \
        auto const _imax = std::numeric_limits<number_t>::max(); \
        std::random_device _rdev; \
        std::uniform_int_distribution<number_t> _dist(_imin, _imax); \
        Profiler<ResultType> _prof(_result); \
        for (auto _i = 0u; _i < 10000000u; ++_i) \
        { \
            auto const i = _dist(_rdev);

//          TEST CODE GOES HERE

#define RANDOM_PROFILING_END() \
        } \
    } \
    std::cout << std::fixed << std::setprecision(3) << _result.count() << "s" << std::endl; \
}

#define PROFILING_BEGIN(type, msg) \
{ \
    typedef std::chrono::duration<double> ResultType; \
    std::cout << "[" << typeid(type).name() << "] " << (msg) << "... " << std::flush; \
    ResultType _result; \
    { \
        auto const _imin = std::numeric_limits<type>::min(); \
        auto const _imax = std::numeric_limits<type>::max(); \
        Profiler<ResultType> _prof(_result); \
        for (auto i = _imin; i < _imax; ++i) \
        { \

//          TEST CODE GOES HERE

#define PROFILING_END() \
        } \
    } \
    std::cout << std::fixed << std::setprecision(3) << _result.count() << "s" << std::endl; \
}

void StdStringStreamWithRewind()
{
    std::stringstream ss;
    ss.str(std::string(64, '0'));
    RANDOM_PROFILING_BEGIN("std::stringstream with rewind")
    ss.seekp(0);
    ss << i;
    RANDOM_PROFILING_END()
}

template<typename T>
void StdStringStreamWithRewind()
{
    std::stringstream ss;
    ss.str(std::string(64, '0'));
    PROFILING_BEGIN(T, "std::stringstream with rewind")
    ss.seekp(0);
    ss << i;
    PROFILING_END()
}

#ifdef BOOST_LEXICAL_CAST

void BoostLexicalCastOnStack()
{
    typedef std::array<char, 64> buf_t;
    RANDOM_PROFILING_BEGIN("boost::lexical_cast on stack")
    buf_t buf = boost::lexical_cast<buf_t>(i);
    RANDOM_PROFILING_END()
}

template<typename T>
void BoostLexicalCastOnStack()
{
    typedef std::array<char, 64> buf_t;
    PROFILING_BEGIN(T, "boost::lexical_cast on stack")
    buf_t buf = boost::lexical_cast<buf_t>(i);
    PROFILING_END()
}

#endif

template<typename T>
char const* PrintfFormat();

template<>
inline char const* PrintfFormat<short>(){ return "%hd"; }

template<>
inline char const* PrintfFormat<unsigned short>(){ return "%hu"; }

template<>
inline char const* PrintfFormat<int>(){ return "%d"; }

template<>
inline char const* PrintfFormat<unsigned int>(){ return "%u"; }

template<>
inline char const* PrintfFormat<long>(){ return "%ld"; }

template<>
inline char const* PrintfFormat<unsigned long>(){ return "%lu"; }

template<>
inline char const* PrintfFormat<long long>(){ return "%lld"; }

template<>
inline char const* PrintfFormat<unsigned long long>(){ return "%llu"; }

void StdSprintf()
{
    char buf[64];
    RANDOM_PROFILING_BEGIN("sprintf")
    sprintf(buf, PrintfFormat<number_t>(), i);
    RANDOM_PROFILING_END()
}

template<typename T>
void StdSprintf()
{
    char buf[64];
    PROFILING_BEGIN(T, "sprintf")
    sprintf(buf, PrintfFormat<T>(), i);
    PROFILING_END()
}

void Int2StrConvert()
{
#ifdef _MSC_VER  
    static char buf[64];
#else
    char buf[64];
#endif
    RANDOM_PROFILING_BEGIN("int2str::convert")
    int2str::convert(i, buf);
    RANDOM_PROFILING_END()
}

template<typename T>
void Int2StrConvert()
{
#ifdef _MSC_VER  
    static char buf[64];
#else
    char buf[64];
#endif
    PROFILING_BEGIN(T, "int2str::convert")
    int2str::convert(i, buf);
    PROFILING_END()
}

template<typename T, typename Enable=void>
struct PerfomanceTester
{
    static void Test()
    {
        std::cerr << "[" << typeid(T).name() << "] skipped" << std::endl;
    }
};

template<typename T>
struct PerfomanceTester<T, typename std::enable_if<sizeof(T) <= 4u>::type>
{
    static void Test()
    {
        Int2StrConvert<T>();
#ifdef BOOST_LEXICAL_CAST
        BoostLexicalCastOnStack<T>();
#endif
        StdSprintf<T>();
        StdStringStreamWithRewind<T>();
    }
};

void TestPerfomance()
{
    std::cout << "Test Perfomance" << std::endl;
    PerfomanceTester<short>::Test();
    PerfomanceTester<unsigned short>::Test();
    PerfomanceTester<int>::Test();
    PerfomanceTester<unsigned int>::Test();
    PerfomanceTester<long>::Test();
    PerfomanceTester<unsigned long>::Test();
    PerfomanceTester<long long>::Test();
    PerfomanceTester<unsigned long long>::Test();
}

inline void StrcmpAndExit(char const *str1, char const *str2)
{
    if (strcmp(str1, str2) != 0)
    {
        std::cerr << "FAILED\n" << str1 << " != " << str2 << std::endl;
        std::exit(1);
    }
}

template<typename T>
struct TestWatcher
{
    TestWatcher()
    {
        std::cout << "[" << typeid(T).name() << "] test... " << std::flush;
    }

    ~TestWatcher()
    {
        std::cout << (std::uncaught_exception() ? "ERROR" : "OK") << std::endl;
    }
};

template<typename T, typename Enable=void>
struct FunctionalTester
{
    static void Test()
    {
        TestWatcher<T> const tw;
        char buf1[64];
        char buf2[64];
        int2str::convert(std::numeric_limits<T>::min(), buf1);
        sprintf(buf2, PrintfFormat<T>(), std::numeric_limits<T>::min());
        StrcmpAndExit(buf1, buf2);
        int2str::convert(std::numeric_limits<T>::max(), buf1);
        sprintf(buf2, PrintfFormat<T>(), std::numeric_limits<T>::max());
        StrcmpAndExit(buf1, buf2);
    }
};

template<typename T>
struct FunctionalTester<T, typename std::enable_if<sizeof(T) <= 4u>::type>
{
    static void Test()
    {
        TestWatcher<T> const tw;
        typedef std::array<char, 64> buf_t;
        buf_t buf1;
        T const imin = std::numeric_limits<T>::min();
        T const imax = std::numeric_limits<T>::max();
        for (T i = imin; i < imax; ++i)
        {
            int2str::convert(i, buf1.data());
#ifdef BOOST_LEXICAL_CAST
            buf_t buf2 = boost::lexical_cast<buf_t>(i);
#else
            buf_t buf2;
            sprintf(buf2.data(), PrintfFormat<T>(), i);
#endif
            StrcmpAndExit(buf1.data(), buf2.data());
        }
    }
};

void TestFunctional()
{
    std::cout << "Test Functional" << std::endl;
    FunctionalTester<short>::Test();
    FunctionalTester<unsigned short>::Test();
    FunctionalTester<int>::Test();
    FunctionalTester<unsigned int>::Test();
    FunctionalTester<long>::Test();
    FunctionalTester<unsigned long>::Test();
    FunctionalTester<long long>::Test();
    FunctionalTester<unsigned long long>::Test();
}

void TestPerfomanceRandom()
{
    std::cout << "Test Perfomance Random" << std::endl;
    Int2StrConvert();
#ifdef BOOST_LEXICAL_CAST
    BoostLexicalCastOnStack();
#endif
    StdSprintf();
    StdStringStreamWithRewind();
}

int main()
{
    TestFunctional();
    TestPerfomance();
    TestPerfomanceRandom();
    return 0;
}
