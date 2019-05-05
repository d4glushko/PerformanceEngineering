#include <iostream>
#include <thread>


void hello()
{
    std::cout<<"Hello Concurrent World 1\n";
}

void hello2()
{
    std::cout<<"Hello Concurrent World 2\n";
}

int main()
{
    std::thread t(hello);
    std::thread t1(hello2);
    t.join();
    t1.join();
}
