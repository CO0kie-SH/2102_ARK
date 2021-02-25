#pragma once
#include <iostream>
#include <Windows.h>
#include "Resource.h"

#include <map>
using std::map;

typedef struct _MyTreeInfo		//结构体[树叶信息]
{								//包含以下结构
	DWORD		uiDeep;			//定义树深度
	DWORD		uiNumb;			//定义树排序
	LPWCH		str;			//定义字符串
	HTREEITEM	hrTree;			//定义根节点
	HTREEITEM	htTree;			//定义上层节点
}MyTreeInfo, * PMyTreeInfo;


#define gnbTreeFunctions	10		// 树控件初始化信息
constexpr PTCHAR gszTreeFunctions[] = {
	_T("进程功能区"),
	_T("驱动功能区"),
	_T("文件功能区"),
	_T("注册表功能区"),
	_T("IDT功能区"),
	_T("GDT功能区"),
	_T("SSDT功能区"),
	_T("HOOK功能区"),
	_T("内联钩子检查"),
	_T("PDB功能区")
};

#define gnbTreeFunctions1	3		// 树控件初始化信息
constexpr PTCHAR gszTreeFunctions1[] = {
	_T("遍历进程"),
	_T("线程操作"),
	_T("模块操作"),
};


class CData
{
public:
	DWORD mPID;
	DWORD mTID;
	HWND hMFC;
	HWND hCon;
public:
	CData();
	~CData();
private:

};

extern CData gcData;