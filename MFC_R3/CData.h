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

typedef struct _MyTreeInfo		//�ṹ��[��Ҷ��Ϣ]
{								//�������½ṹ
	DWORD		uiDeep;			//���������
	DWORD		uiNumb;			//����������
	LPWCH		str;			//�����ַ���
	HTREEITEM	hrTree;			//������ڵ�
	HTREEITEM	htTree;			//�����ϲ�ڵ�
}MyTreeInfo, * PMyTreeInfo;


#define gnbTreeFunctions	10					// һ���˵�
constexpr PTCHAR gszTreeFunctions[] = {
	_T("���̹�����"),
	_T("����������"),
	_T("�ļ�������"),
	_T("ע�������"),
	_T("IDT������"),
	_T("GDT������"),
	_T("SSDT������"),
	_T("HOOK������"),
	_T("�������Ӽ��"),
	_T("PDB������")
};

#define gnbTreeFunctions1					5	// �����˵�
#define gszTreeCtrl��������	gszTreeFunctions1[0]
#define gszTreeCtrl�̲߳���	gszTreeFunctions1[1]
#define gszTreeCtrlģ�����	gszTreeFunctions1[2]
constexpr PTCHAR gszTreeFunctions1[] = {
	_T("��������"),
	_T("�̲߳���"),
	_T("ģ�����"),
	_T("���ؽ���"),
	_T("��������")
};

#define gnbTreeFunctions2					2	// �����˵�
#define gszTreeCtrl��������	gszTreeFunctions2[0]
constexpr PTCHAR gszTreeFunctions2[] = {
	_T("��������"),
	_T("��������")
};


#pragma region ������ؽṹ
typedef struct _MyMod
{
	ULONG Base;		//ģ�����
	CString str;	//·��
}MyMod, * LPMyMod;

typedef struct _MyProcess3
{
	ULONG tPID;		//����ID
	ULONG pPID;		//������
	ULONG ulThs;	//�߳�����
	ULONG ulMod;	//ģ������
	ULONG pNextMod;	//��һ��ģ��
	CString szExe;	//Exe��
	CString szPath;	//����·��
	vector<MyMod> vMDs;		//ģ������
	vector<MyThread> vTHs;	//�߳�����
}MyProcess3, * LPMyProcess3;
#pragma endregion

#pragma region ������ؽṹ
typedef struct _MySys
{
	ULONG Base;		//������ַ
	ULONG pNextLdr;	//��һ��Ldr��
	CString szExe;	//Exe��
	CString szPath;	//����·��
}MySys, * LPMySys;
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