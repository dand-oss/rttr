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

    std::string whoami() { return "I am a base method" ; }
    virtual std::string vwhoami() { return "I am a virtual base method" ; }

    RTTR_ENABLE()

};


// RTTR_ENABLE incompatible with clang
#define RTTR_ENABLE_OVERRIDE(...) \
public:\
    RTTR_INLINE rttr::type get_type() const override { return rttr::detail::get_type_from_instance(this); }  \
    RTTR_INLINE void* get_ptr() override { return reinterpret_cast<void*>(this); } \
    RTTR_INLINE rttr::detail::derived_info get_derived_info() override { return {reinterpret_cast<void*>(this), rttr::detail::get_type_from_instance(this)}; } \
    using base_class_list = TYPE_LIST(__VA_ARGS__);


struct method_order_test_derived : method_order_test_base
{
    method_order_test_derived() = default ;
    ~method_order_test_derived() override = default ;

    std::string whoami() { return "I am a non-virtual derived method with same name" ; }
    std::string vwhoami() override { return "I am a virtual derived method with same name" ; }

    RTTR_ENABLE_OVERRIDE(method_order_test_base)
};

/////////////////////////////////////////////////////////////////////////////////////////

RTTR_REGISTRATION
{
    registration::class_<method_order_test_base>("method_order_test_base")
        .method("whoami", &method_order_test_base::whoami)
        .method("vwhoami", &method_order_test_base::vwhoami)
        ;
    registration::class_<method_order_test_derived>("method_order_test_derived")
        .method("whoami", &method_order_test_derived::whoami)
        .method("vwhoami", &method_order_test_derived::vwhoami)
        ;
}

// approach 1 looks up the "method instance" without considering argument type and invoke the registered function()
template<class TT>
std::string rttr_invoke_approach1(
        TT& inst,
        const std::string& meth_name) {

    const auto& inst_t = type::get<TT>();

    // find method
    const auto& meth = inst_t.get_method(meth_name);
    REQUIRE(meth.is_valid() == true);

    // invoke via method
    variant&& iam_var = meth.invoke(inst);
    REQUIRE(iam_var.is_valid() == true);
    REQUIRE(iam_var.is_type<std::string>() == true) ;
    return iam_var.get_value<std::string&>();
}

// approach 2 looks invokes the method via rttr::type
// different code - range based for loop
template<class TT>
std::string rttr_invoke_approach2(
        TT& inst,
        const std::string& meth_name) {

    const auto& inst_t = type::get<TT>();

    // invoke via type
    variant&& iam_var = inst_t.invoke(meth_name, inst, {});
    REQUIRE(iam_var.is_valid() == true);
    REQUIRE(iam_var.is_type<std::string>() == true) ;

    return iam_var.get_value<std::string&>();
}

template<class TT>
void check_nonvirt_meth_order1() {
    TT inst;
    REQUIRE(inst.whoami() == rttr_invoke_approach1<TT>(inst, "whoami"));
}

template<class TT>
void check_virt_meth_order1() {
    TT inst;
    REQUIRE(inst.vwhoami() == rttr_invoke_approach1<TT>(inst, "vwhoami"));
}

template<class TT>
void check_nonvirt_meth_order2() {
    TT inst;
    REQUIRE(inst.whoami() == rttr_invoke_approach2<TT>(inst, "whoami"));
}

template<class TT>
void check_virt_meth_order2() {
    TT inst;
    REQUIRE(inst.vwhoami() == rttr_invoke_approach2<TT>(inst, "vwhoami"));
}

/////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("method - approach1_order_nonvirt_methods_base()", "[method]")
{
    check_nonvirt_meth_order1<method_order_test_base>();
}

TEST_CASE("method - approach1_order_nonvirt_methods_derived()", "[method]")
{
    check_nonvirt_meth_order1<method_order_test_derived>();
}

TEST_CASE("method - approach1_order_virt_methods_base()", "[method]")
{
    check_virt_meth_order1<method_order_test_base>();
}

TEST_CASE("method - approach1_order_nvirt_methods_derived()", "[method]")
{
    check_virt_meth_order1<method_order_test_derived>();
}

TEST_CASE("method - approach2_order_nonvirt_methods_base()", "[method]")
{
    check_nonvirt_meth_order2<method_order_test_base>();
}

TEST_CASE("method - approach2_order_nonvirt_methods_derived()", "[method]")
{
    check_nonvirt_meth_order2<method_order_test_derived>();
}

TEST_CASE("method - approach2_order_virt_methods_base()", "[method]")
{
    check_virt_meth_order2<method_order_test_base>();
}

TEST_CASE("method - approach2_order_nvirt_methods_derived()", "[method]")
{
    check_virt_meth_order2<method_order_test_derived>();
}