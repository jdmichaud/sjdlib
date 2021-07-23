#include "../src/sjdlib.h"

//#include <utility>

using namespace std;

class Foo {
public:
    explicit Foo(std::string s): bar(std::move(s)) { std::cout << "Foo constructor" << std::endl; }
    Foo(Foo&& foo) noexcept: bar(std::move(foo.bar)) { std::cout << "Move constructor" << std::endl; }

    std::string bar;
private:
    Foo(const Foo& f) = delete; // no copy
};

Result<Foo, int> produce_Foo() {
    Foo foo = Foo("bar");
    return Result<Foo, int>::Ok(std::move(foo));
}

int main() {
    Result ok = produce_Foo();
    ASSERT(ok.is_ok() == true)
    ASSERT(ok.is_err() == false)
    Foo foo = ok.unwrap();
    ASSERT(std::string(foo.bar) == "bar");
    std::cout << foo.bar << std::endl;

    Result err = Result<Foo, int>::Err(23);
    ASSERT(err.is_ok() == false)
    ASSERT(err.is_err() == true)
    ASSERT_THROW(err.unwrap());

    Result ok2 = Result<Foo, int>::Ok(Foo("bar"));
    ASSERT(std::string("bar") == (!ok2).bar);
    Result err2 = Result<Foo, std::string>::Err("error!");
    ASSERT_THROW(!err2);

    Result ok3 = Result<int, string>::Ok(42);

    Option<int> o1(2);
    Option<int> o2;
    ASSERT(o1 != 42);
    ASSERT(o1 == 2);

    return 0;
}
