/* unit_test.hpp
Provides lightweight Boost.Test macros
(C) 2014 Niall Douglas http://www.nedprod.com/
File Created: Nov 2014


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_BINDLIB_BOOST_UNIT_TEST_HPP
#define BOOST_BINDLIB_BOOST_UNIT_TEST_HPP
 
#include "../config.hpp"
#include <atomic>
#include <mutex>
#define CATCH_CONFIG_PREFIX_ALL
#define CATCH_CONFIG_RUNNER
#include "../../CATCH/single_include/catch.hpp"

#define BOOST_CATCH_UNIT_TESTING 1

namespace boost { namespace unit_test_as_catch {
  static std::mutex &__global_lock()
  {
    static std::mutex lock;
    return lock;
  }
  static std::mutex &global_lock()
  {
    static std::atomic<int> c;
    while(2!=c.load(std::memory_order_acquire))
    {
      int expected=0;
      if(c.compare_exchange_weak(expected, 1, std::memory_order_acquire, std::memory_order_consume))
      {
        __global_lock();
        c.store(2, std::memory_order_release);
        break;
      }
    }
    return __global_lock();
  }
} } // namespace

#undef INTERNAL_CATCH_TEST
#define INTERNAL_CATCH_TEST( expr, resultDisposition, macroName ) \
    { std::lock_guard<decltype(::boost::unit_test_as_catch::global_lock())> ___g(::boost::unit_test_as_catch::global_lock()); \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        try { \
            ( __catchResult->*expr ).endExpression(); \
        } \
        catch( ... ) { \
            __catchResult.useActiveException( Catch::ResultDisposition::Normal ); \
        } \
        INTERNAL_CATCH_REACT( __catchResult ) \
        } while( Catch::isTrue( false && (expr) ) ); }

#undef INTERNAL_CATCH_NO_THROW
#define INTERNAL_CATCH_NO_THROW( expr, resultDisposition, macroName ) \
    { std::lock_guard<decltype(::boost::unit_test_as_catch::global_lock())> ___g(::boost::unit_test_as_catch::global_lock()); \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        try { \
            expr; \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        } \
        catch( ... ) { \
            __catchResult.useActiveException( resultDisposition ); \
        } \
        INTERNAL_CATCH_REACT( __catchResult ) \
        } while( Catch::alwaysFalse() ); }

#undef INTERNAL_CATCH_THROWS
#define INTERNAL_CATCH_THROWS( expr, resultDisposition, macroName ) \
    { std::lock_guard<decltype(::boost::unit_test_as_catch::global_lock())> ___g(::boost::unit_test_as_catch::global_lock()); \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        if( __catchResult.allowThrows() ) \
            try { \
                expr; \
                __catchResult.captureResult( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( ... ) { \
                __catchResult.captureResult( Catch::ResultWas::Ok ); \
            } \
                else \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( __catchResult ) \
        } while( Catch::alwaysFalse() ); }

#undef INTERNAL_CATCH_THROWS_AS
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, resultDisposition, macroName ) \
    { std::lock_guard<decltype(::boost::unit_test_as_catch::global_lock())> ___g(::boost::unit_test_as_catch::global_lock()); \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        if( __catchResult.allowThrows() ) \
            try { \
                expr; \
                __catchResult.captureResult( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( exceptionType ) { \
                __catchResult.captureResult( Catch::ResultWas::Ok ); \
            } \
            catch( ... ) { \
                __catchResult.useActiveException( resultDisposition ); \
            } \
                else \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( __catchResult ) \
        } while( Catch::alwaysFalse() ); }

#undef INTERNAL_CATCH_INFO
#define INTERNAL_CATCH_INFO( log, macroName ) \
    { std::lock_guard<decltype(::boost::unit_test_as_catch::global_lock())> ___g(::boost::unit_test_as_catch::global_lock()); \
    Catch::ScopedMessage INTERNAL_CATCH_UNIQUE_NAME( scopedMessage ) = Catch::MessageBuilder( macroName, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log; }

#undef INTERNAL_CATCH_MSG
#define INTERNAL_CATCH_MSG( messageType, resultDisposition, macroName, log ) \
    { std::lock_guard<decltype(::boost::unit_test_as_catch::global_lock())> ___g(::boost::unit_test_as_catch::global_lock()); \
        do { \
            Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, "", resultDisposition ); \
            __catchResult << log + ::Catch::StreamEndStop(); \
            __catchResult.captureResult( messageType ); \
            INTERNAL_CATCH_REACT( __catchResult ) \
                } while( Catch::alwaysFalse() ); }


#define BOOST_TEST_MESSAGE(msg) CATCH_INFO(msg)
#define BOOST_FAIL(msg) CATCH_FAIL(msg)
#define BOOST_CHECK_MESSAGE(p, msg) CATCH_INFO(msg)

#define BOOST_CHECK(expr) CATCH_CHECK(expr)
#define BOOST_CHECK_THROWS(expr) CATCH_CHECK_THROWS(expr)
#define BOOST_CHECK_THROW(expr, type) CATCH_CHECK_THROWS_AS(expr, type)
#define BOOST_CHECK_NO_THROW(expr) CATCH_CHECK_NOTHROW(expr)

#define BOOST_REQUIRE(expr) CATCH_REQUIRE(expr)
#define BOOST_REQUIRE_THROWS(expr) CATCH_REQUIRE_THROWS(expr)
#define BOOST_CHECK_REQUIRE(expr, type) CATCH_REQUIRE_THROWS_AS(expr, type)
#define BOOST_REQUIRE_NO_THROW(expr) CATCH_REQUIRE_NOTHROW(expr)

#if defined _MSC_VER
# define BOOST_BINDLIB_ENABLE_MULTIPLE_DEFINITIONS inline
#elif defined __MINGW32__
# define BOOST_BINDLIB_ENABLE_MULTIPLE_DEFINITIONS
#elif defined __GNUC__
# define BOOST_BINDLIB_ENABLE_MULTIPLE_DEFINITIONS __attribute__((weak))
#else
# define BOOST_BINDLIB_ENABLE_MULTIPLE_DEFINITIONS inline
#endif

#ifndef BOOST_CATCH_CUSTOM_MAIN_DEFINED
BOOST_BINDLIB_ENABLE_MULTIPLE_DEFINITIONS int main( int argc, char* const argv[] )
{
  int result = Catch::Session().run( argc, argv );
  return result;
}
#endif

#define BOOST_AUTO_TEST_SUITE3(a, b) a ## b
#define BOOST_AUTO_TEST_SUITE2(a, b) BOOST_AUTO_TEST_SUITE3(a, b)
#define BOOST_AUTO_TEST_SUITE(name) namespace BOOST_AUTO_TEST_SUITE2(boost_catch_auto_test_suite, __COUNTER__) {
#define BOOST_AUTO_TEST_SUITE_END() }
#ifndef BOOST_CATCH_AUTO_TEST_CASE_NAME
#define BOOST_CATCH_AUTO_TEST_CASE_NAME(name) #name
#endif
#define BOOST_AUTO_TEST_CASE(test_name, desc) CATCH_TEST_CASE(BOOST_CATCH_AUTO_TEST_CASE_NAME(test_name), desc)

#endif
