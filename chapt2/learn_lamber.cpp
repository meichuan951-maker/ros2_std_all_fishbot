#include<iostream>
#include<algorith>

int main(){
    auto add = [](int a,int b) -> int;
    {return a + b;};
    int sum = add(114000,514);
    auto print_sum = [sum]() -> void{
        std::cout <<sum << std::endl;
    };

    print_sum();
    return 0;
}