#include <iostream>
#include <tuple>

template<typename T, typename... Args>
auto& operator<<(std::basic_ostream<char>& os, const std::tuple<T, Args...>& tuple) {
    auto remove_first = [](T first, Args... args) { return std::make_tuple(args...); };

    return os << std::get<0>(tuple) << "," << std::apply(remove_first, tuple);
}

template<typename T>
auto& operator<<(std::basic_ostream<char>& os, const std::tuple<T>& tuple) {
    return os << std::get<0>(tuple);
}

int main() {
    std::tuple<char, int, short, const std::string> t{ 'c', 234567890, 1, "asdasdasd" };
    std::cout << t;
}
