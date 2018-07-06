/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#include <rttr/registration>
#include <catch/catch.hpp>
#include <string>

using namespace rttr;


struct method_order_test_base
{
    method_order_test_base() = default ;
    virtual ~method_order_test_base() = default ;

    std::string whoami() { return "I am base" ; }

    RTTR_ENABLE()

};

struct method_order_test_derived : method_order_test_base
{
    method_order_test_derived() = default ;
    ~method_order_test_derived() override = default ;

    std::string whaomi() { return "I am derived" ; }

    RTTR_ENABLE(method_order_test_base)
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_order_test_base>("method_order_test_base")
        .constructor<>()
        .method("whoami", &method_order_test_base::whoami)
        ;
    registration::class_<method_order_test_derived>("method_order_test_derived")
        .constructor<>()
        .method("whoami", &method_order_test_derived::whoami)
        ;
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - get_methods()", "[method]")
{
    // derived method via C++
    method_order_test_derived derived ;
    const auto& cpp_iam = method_order_test_derived.whoami();

    // derived method via rttr
    const auto& rderived_t = rttr::type.get_by_name("method_order_test_derived")
    const auto& rderived = rderived.create({});
    const auto& rviam = rderived.invoke("whoami");
    const auto& rttr_iam = rviam.get_value<std::string>() ;

    // rttr result must match cpp result
    REQUIRE(rttr_iam == cpp_iam);
}
