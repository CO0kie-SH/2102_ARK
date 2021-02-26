#pragma once
#include <iostream>
#include <Windows.h>
#include "Resource.h"
#include "..\Header.h"

#include <map>
#include <vector>
using std::map;
using std::vector;

#define SYMBOLICLINE_NAMER3 L"\\\\.\\MyTestDriver"

typedef struct _MyTreeInfo		//结构体[树叶信息]
{								//包含以下结构
	DWORD		uiDeep;			//定义树深度
	DWORD		uiNumb;			//定义树排序
	LPWCH		str;			//定义字符串
	HTREEITEM	hrTree;			//定义根节点
	HTREEITEM	htTree;			//定义上层节点
}MyTreeInfo, * PMyTreeInfo;


#define gnbTreeFunctions	10					// 一级菜单
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

#define gnbTreeFunctions1					5	// 二级菜单
#define gszTreeCtrl遍历进程	gszTreeFunctions1[0]
#define gszTreeCtrl线程操作	gszTreeFunctions1[1]
#define gszTreeCtrl模块操作	gszTreeFunctions1[2]
constexpr PTCHAR gszTreeFunctions1[] = {
	_T("遍历进程"),
	_T("线程操作"),
	_T("模块操作"),
	_T("隐藏进程"),
	_T("结束操作")
};

#define gnbTreeFunctions2					2	// 二级菜单
#define gszTreeCtrl遍历驱动	gszTreeFunctions2[0]
constexpr PTCHAR gszTreeFunctions2[] = {
	_T("遍历驱动"),
	_T("隐藏驱动")
};

#define gnbTreeFunctions3					3	// 二级菜单
#define gszTreeCtrl遍历文件	gszTreeFunctions3[0]
constexpr PTCHAR gszTreeFunctions3[] = {
	_T("遍历系统目录"),
	_T("创建文件"),
	_T("删除文件")
};


#pragma region 进程相关结构
typedef struct _MyMod
{
	ULONG Base;		//模块基质
	CString str;	//路径
}MyMod, * LPMyMod;

typedef struct _MyProcess3
{
	ULONG tPID;		//进程ID
	ULONG pPID;		//父进程
	ULONG ulThs;	//线程数量
	ULONG ulMod;	//模块数量
	ULONG pNextMod;	//下一个模块
	CString szExe;	//Exe名
	CString szPath;	//进程路径
	vector<MyMod> vMDs;		//模块数组
	vector<MyThread> vTHs;	//线程数组
}MyProcess3, * LPMyProcess3;
#pragma endregion

#pragma region 驱动相关结构
typedef struct _MySys
{
	ULONG Base;		//驱动基址
	ULONG pNextLdr;	//下一个Ldr链
	CString szExe;	//Exe名
	CString szPath;	//驱动路径
}MySys, * LPMySys;
#pragma endregion


#pragma region 文件结构
typedef struct _FILE_BOTH_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	CCHAR ShortNameLength;
	WCHAR ShortName[12];
	_Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, * PFILE_BOTH_DIR_INFORMATION;

typedef struct _MyPath
{
	ULONG FileAttributes;		//文件属性
	LARGE_INTEGER CreationTime;	//创建日期
	CString szPath;				//文件路径
}MyPath, * LPMyPath;
#pragma endregion

class CData
{
public:
	DWORD mPID;
	DWORD mTID;
	HWND hMFC;
	HWND hCon;
	vector<MyProcess3> vPIDs;
public:
	CData();
	~CData();
private:

};

extern CData gcData;