#pragma once
#include <iostream>
#include <windows.h>


class CDevice
{
public:
	CDevice();
	~CDevice();

private:
	HANDLE DeviceHandle;
};

