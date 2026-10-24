#include<iostream>
#include<memory>

int main()
{
    auto p1 = std::make_shered<std::string>("this is a string");
    std::cout << "p1的引用计数" <<p1.use_count()<<p1.get()<<std::endl;

    auto p2 = p1;
    std::cout << "p2的引用计数" <<p1.use_count()<<p1.get()<<std::endl;

    p1.reset();

    return 0;
}