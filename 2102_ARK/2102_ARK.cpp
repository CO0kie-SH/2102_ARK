// 2102_ARK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CDevice.h"

int main()
{
    std::cout << "Hello World!\n";
    printf("size=%d\n", sizeof(MyInfoSend));
    CDevice cDevice;
    cDevice.Test();
    system("pause");
}