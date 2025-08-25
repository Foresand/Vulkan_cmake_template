#include "func.h"

void test01::func01()
{
    std::cout<<"test01:func01!"<<std::endl;
}
void test01::func02()
{
    this->func01();
}