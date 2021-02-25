#pragma once
#include <iostream>
#include <Windows.h>
#include "Resource.h"

#include <map>
using std::map;

typedef struct _MyTreeInfo		//�ṹ��[��Ҷ��Ϣ]
{								//�������½ṹ
	DWORD		uiDeep;			//���������
	DWORD		uiNumb;			//����������
	LPWCH		str;			//�����ַ���
	HTREEITEM	hrTree;			//������ڵ�
	HTREEITEM	htTree;			//�����ϲ�ڵ�
}MyTreeInfo, * PMyTreeInfo;


#define gnbTreeFunctions	10		// ���ؼ���ʼ����Ϣ
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

#define gnbTreeFunctions1	3		// ���ؼ���ʼ����Ϣ
constexpr PTCHAR gszTreeFunctions1[] = {
	_T("��������"),
	_T("�̲߳���"),
	_T("ģ�����"),
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