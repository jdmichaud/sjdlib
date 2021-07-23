#pragma once
#include <cfloat>
#include <cstdint>
#include <iostream>
#include <variant>

using namespace std;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

void __assertion_failure(const char *msg, const char *file, unsigned line, const char *pretty_function) {
    std::cerr << "EQUALITY ASSERTION FAILED: " << msg << std::endl;
    std::cerr << file << ":" << line << " in " << pretty_function << std::endl;
    exit(1);
}

#define ASSERT(cond) \
    if (!(cond)) {      \
        __assertion_failure(#cond, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    }

#define ASSERT_THROW(expr) \
    try {                  \
        (expr);            \
        __assertion_failure("Exception for '"#expr"' was not raised", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    } catch (...) {}         \


template <typename T>
class Option;

template <typename T, typename E>
class Result {
public:
    static Result Ok(T& value) noexcept { return Result(value); }
    static Result Ok(T&& value) noexcept { return Result(std::forward<T&>(value)); }
    static Result Err(E& error) noexcept { return Result(error); }
    static Result Err(E&& error) noexcept { return Result(std::forward<E&>(error)); }

public:
    T&& unwrap() {
        if (this->pvalue != nullptr) {
            T value = std::move(*this->pvalue);
            this->pvalue = nullptr;
            return value;
        }
        throw new std::bad_variant_access();
    }

    T unwrap_or(T deflt) noexcept {
        if (this->pvalue != nullptr) {
            return *this->pvalue;
        }
        return deflt;
    }

    bool is_ok() noexcept { return this->pvalue != nullptr; }
    bool is_err() noexcept { return this->perror != nullptr; }

    Result<T, E>& otherwise(Result <T, E>& alternative) noexcept {
        if (this->pvalue != nullptr) {
            return *this;
        }
        return alternative;
    }

//    Option<T> ok() {}

private:
    explicit Result(T& value) {
        this->pvalue = &value;
    }

    explicit Result(E& error) {
        this->perror = &error;
    }


private:
    T *pvalue = { nullptr };
    E *perror = { nullptr };
};

class Foo {
public:
    explicit Foo(const std::string& s): bar(s) {}
    std::string bar;
private:
    Foo(const Foo& f) = delete; // no copy
};

int main() {
    Result ok = Result<Foo, int>::Ok(Foo("bar"));
    ASSERT(ok.is_ok() == true)
    ASSERT(ok.is_err() == false)
    Foo foo = ok.unwrap();
    ASSERT(std::string(foo.bar) == "bar");
    std::cout << foo.bar << std::endl;

    Result err = Result<Foo, int>::Err(23);
    ASSERT(err.is_ok() == false)
    ASSERT(err.is_err() == true)
    ASSERT_THROW(err.unwrap());

//    alignas(Foo) u8 storage[sizeof(Foo)];
//    new (&storage) Foo(std::forward<Foo>(Foo { .bar = "placement new" }));
//    Foo fou = *reinterpret_cast<Foo *>(&storage);
//    std::cout << fou.bar << std::endl;
//  delete storage;
    //     ^^^^^^^ should not be deleted because placement new just associate a
    //     statically allocated structure to a pre-allocated storage and will
    //     storage duration is automatically delete.
    return 0;
}
