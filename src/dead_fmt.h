//
// Created by jedi on 21/08/2021.
//

#ifndef RESULT_DEAD_FMT_H
#define RESULT_DEAD_FMT_H

/**
 * I realized that writring a format! equivalent facility in C++ was not that easy:
 * see https://en.cppreference.com/w/cpp/utility/format/format
 *
 * So let's put aside for now...
 */

template<typename... Args>
void compiletime_fail(Args...);

template<typename ...Parameters>
struct FormatString {
    template <size_t size>
    consteval FormatString(const char (&buffer)[size]): m_buffer(buffer), m_size(size) {
        this->check_format_expression<size, sizeof...(Parameters)>();
    }

    consteval char const& operator[](std::size_t index) const {
        if (index < m_size) {
            return m_buffer[index];
        }
        compiletime_fail("index out of range");
        throw std::out_of_range("index out of range");
    }

    [[nodiscard]] consteval size_t size() const { return m_size; };

    template <size_t size, size_t parameter_count>
    consteval bool check_format_expression() {
        if (balance<size>() < 0)
            compiletime_fail("unbalance format expression, too many closing brackets");
        if (balance<size>() > 0)
            compiletime_fail("unclosed brackets in format expression");
        if (count_brakets<size>() < parameter_count)
            compiletime_fail("not enough placeholders for provided parameters");
        if (count_brakets<size>() > parameter_count)
            compiletime_fail("too many placeholders for provided parameters");
        return true;
    }

    static constexpr int requires_nonnegative(i32 i)
    {
        if (i < 0) {
            compiletime_fail("too much closing brackets");
        }
        return i;
    }

    template <size_t size>
    [[nodiscard]] consteval int balance(size_t i = 0, i32 ans = 0) const
    {
        requires_nonnegative(ans);
        if (i == size) return ans;
        if ((*this)[i] == '{') return balance<size>(i + 1, ans + 1);
        if ((*this)[i] == '}') return balance<size>(i + 1, ans - 1);
        return balance<size>(i + 1, ans);
    }

    template <size_t size>
    [[nodiscard]] consteval int count_brakets(size_t i = 0, i32 ans = 0) const
    {
        if (i == size) return ans;
        if ((*this)[i] == '{') return count_brakets<size>(i + 1, ans + 1);
        return count_brakets<size>(i + 1, ans);
    }

    const char *m_buffer { nullptr };
    const size_t m_size { 0 };
};

//template<typename... Parameters>
//class FormatParams {
//public:
//    explicit VariadicFormatParams(const Parameters&... parameters)
//        : m_data({ TypeErasedParameter { &parameters, TypeErasedParameter::get_type<Parameters>(), __format_value<Parameters> }... })
//    {
//        this->set_parameters(m_data);
//    }
//
//private:
//    std::array<Parameters, sizeof...(Parameters)> m_data;
//};

template <typename T>
class Formatter {
    std::string to_string(T t);
};



template<typename ...Parameters>
constexpr void println(typename std::type_identity_t<FormatString<Parameters...>>&& fmtstr, // no type deduction
                       const Parameters& ...parameters) {
}

// Further reflexions: https://godbolt.org/z/c91We3qTh

#include <concepts>
#include <string>
#include <iostream>
#include <array>
#include <any>

template<typename T>
concept FloatingPoint = std::floating_point<T>;

auto go(FloatingPoint auto x, FloatingPoint auto y) {
    return x + y;
}

template<typename T>
concept Formattable = std::convertible_to<T, std::string>;

std::string format(Formattable auto p) {
    return std::to_string(p);
}

template <typename T = void>
struct Formatter {
    std::string to_string() = 0;
};

template<>
struct Formatter<int> {
    consteval Formatter(int _v): v(_v) {}
    std::string to_string() { return std::to_string(v); }
    int v;
};

template<>
struct Formatter<unsigned int> {
    consteval Formatter(unsigned int _v): v(_v) {}
    std::string to_string() { return std::to_string(v); }
    unsigned int v;
};

struct Foo {
    int x { 0 };
};

template<>
struct Formatter<Foo> {
    consteval Formatter(Foo _f): f(_f) {}

    std::string to_string() {
        return std::string("Foo { x: ") + std::to_string(f.x) + " }";
    }
    Foo f;
};

template <typename T, size_t N>
struct A
{
   template <typename ... Ts>
   A(size_t s, Ts&& ... vals) : arr(vals...) {}

   std::array<T, N> arr;
};


template<typename T>
struct get_type {
    using Type = T;
};

// template<typename ...Parameters>
// constexpr void println(const Parameters& ...parameters) {
//     // std::array<Formatter, sizeof...(Parameters)> arr({ Formatter { parameters }... });

//     ([&] (auto & parameter)
//     {
//         std::cout << Formatter(parameter) << " ";
//     } (parameters), ...);

//     // for (auto p: arr) {
//     //     std::cout << p.to_string();
//     // }
//     std::cout << std::endl;
// }

int main() {
    // println(1, 2, 3);
    std::any a = Formatter<int>(3);
    std::any b = Formatter<Foo>({ x: 23 });
    std::array<std::any, 2> arr = { a, b };
    for (auto x: arr) {
        std::cout << static_cast<Formatter<auto>>(x).to_string() << " ";
    }
}


#endif //RESULT_DEAD_FMT_H
