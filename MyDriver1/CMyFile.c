#include "CMyFile.h"

// ʹ��ָ���ķ�ʽ�������һ��ָ�����ļ���Ŀ¼
HANDLE CreateFile(LPCWSTR pFilePath, ACCESS_MASK Access, BOOLEAN IsFile)
{
	HANDLE FileHandle = NULL;
	IO_STATUS_BLOCK IoStatusBlock = { 0 };

	// ��ʼ���ļ���Ŀ¼�����ֱ��浽 UNICODE_STRING ��
	UNICODE_STRING FilePath = { 0 };
	RtlInitUnicodeString(&FilePath, pFilePath);

	// ����һ�����������ʱ��ͨ������Ҫ�ṩ�ṹ�壬�����ļ�������
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	InitializeObjectAttributes(
		&ObjectAttributes,				// ��Ҫ��ʼ���Ķ������Խṹ��
		&FilePath,						// ��ǰ�Ķ�������������һ������
		OBJ_CASE_INSENSITIVE,			// ��ʾ�����Ʋ����ִ�Сд
		NULL, NULL);					// �����Ŀ¼�Ͱ�ȫ�����ֶ�


	// �����û�����Ĳ���ֵ���� CreateOption ��ֵ��Ŀ¼\�ļ�
	ULONG CreateOption = IsFile ? FILE_NON_DIRECTORY_FILE
		: FILE_DIRECTORY_FILE;


	// �ں˲�����ļ��ĺ����� ZwCreateFile�����ʧ�ܷ���ʧ��ԭ��
	NTSTATUS Status = ZwCreateFile(
		&FileHandle,					// �ɹ����ú��������ڱ������ı���
		Access,							// ��ʲô����Ȩ��ȥ�����ļ� GENERIC_ALL
		&ObjectAttributes,				// ��Ҫ���ڱ����ļ���·��
		&IoStatusBlock,					// ������ǵ�ǰ������ִ�н��
		0,								// ����Ǵ����µ��ļ�����ô�ļ�Ĭ�ϵĴ�С�Ƕ���
		FILE_ATTRIBUTE_NORMAL,			// ������򿪵��ļ�Ӧ�þ���ʲô��������
		FILE_SHARE_VALID_FLAGS,			// �ļ�����Ĺ���ʽ���� R3 ����һ��
		FILE_OPEN_IF,					// ��\�����ļ��ķ�ʽ FILE_OPEN_IF �оͿ�
		CreateOption,					// ��ǰ��������ֻ��Ҫ��ע���������ļ�����Ŀ¼
		NULL, 0);						// ������չ����Ĵ�С������ָ��������ָ��


	// �жϴ�·���Ĳ����Ƿ�ɹ������ʧ�ܷ���-1�����򷵻ؾ��
	if (NT_SUCCESS(Status))
		return FileHandle;
	else
		return (HANDLE)-1;
}

// ��ָ���ļ���д��ָ����С��ָ������
NTSTATUS WriteFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	LARGE_INTEGER Offset = { pOffset, 0 };

	// ��ָ�����ļ���д��ָ�����ȵ�����
	NTSTATUS Status = ZwWriteFile(
		FileHandle,					// ��Ҫ�����ĵ������ĸ��ļ�
		NULL, NULL, NULL,			// �첽 IO ʹ�õ�������������������
		&IoStatusBlock,				// �����˲����Ľ���������ֽ������Ƿ�ɹ�
		Buffer, Length,				// ��Ҫд��������Լ�д��ĳ���
		&Offset, 0);				// ��Ҫ�������ļ�ָ���λ��

	// �����Ƿ�����ɹ�����ֱ�ӷ��ز������
	return Status;
}

// ��ȡָ���ļ��Ļ�����Ϣ���ú�����Ҫ��ȡ�ļ��Ĵ�С
LONGLONG GetFileSize(HANDLE FileHandle)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	FILE_STANDARD_INFORMATION FileInfo = { 0 };

	// ���ڲ�ѯ�ļ���Ϣ�ĺ��������Բ�ѯ�ǳ������Ϣ
	NTSTATUS Status = ZwQueryInformationFile(
		FileHandle,					// ��Ҫ��ѯ�ĸ��ļ�����Ϣ
		&IoStatusBlock,				// ��ѯ�ɹ��˻���ʧ����
		&FileInfo,					// ��ѯ������Ϣ���浽����ṹ�岻�ǹ̶���
		sizeof(FileInfo),			// �ṩ�Ļ������Ĵ�С
		FileStandardInformation);	// ��Ҫ��ѯ�����������͵���Ϣ

	// ����ɹ����ز�ѯ�Ľ�������򷵻� 0
	if (NT_SUCCESS(Status))
		return  FileInfo.EndOfFile.QuadPart;
	else
		return 0;
}

// ��ָ���ļ���ȡָ����С��ָ������
NTSTATUS ReadFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	LARGE_INTEGER Offset = { pOffset, 0 };

	// ��ָ���ļ���ȡָ�����ȵ�����
	NTSTATUS Status = ZwReadFile(
		FileHandle,					// ��Ҫ�����ĵ������ĸ��ļ�
		NULL, NULL, NULL,			// �첽 IO ʹ�õ�������������������
		&IoStatusBlock,				// �����˲����Ľ���������ֽ������Ƿ�ɹ�
		Buffer, Length,				// ��Ҫ��ȡ�������Լ���ȡ�ĳ���
		&Offset, 0);				// ��Ҫ�������ļ�ָ���λ��

	// �����Ƿ�����ɹ�����ֱ�ӷ��ز������
	return Status;
}

// ɾ���ļ�������Ҫָ���ļ����
NTSTATUS DeleteFile(LPCWSTR pFilePath)
{
	// ��ʼ���ļ���Ŀ¼�����ֱ��浽 UNICODE_STRING ��
	UNICODE_STRING FilePath = { 0 };
	RtlInitUnicodeString(&FilePath, pFilePath);

	// ����һ�����������ʱ��ͨ������Ҫ�ṩ�ṹ�壬�����ļ�������
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	InitializeObjectAttributes(
		&ObjectAttributes,				// ��Ҫ��ʼ���Ķ������Խṹ��
		&FilePath,						// ��ǰ�Ķ�������������һ������
		OBJ_CASE_INSENSITIVE,			// ��ʾ�����Ʋ����ִ�Сд
		NULL, NULL);					// �����Ŀ¼�Ͱ�ȫ�����ֶ�

	// ����ɾ���Ľ��
	return ZwDeleteFile(&ObjectAttributes);
}

/*
	�ں˵�ö���ļ�_pipixia233333�Ĳ���-CSDN����
	https://blog.csdn.net/qq_41071646/article/details/86384279

BOOLEAN MyQueryFileAndFileFolder(UNICODE_STRING ustrPath)
{
	HANDLE hFile = NULL;
	OBJECT_ATTRIBUTES objectAttributes = { 0 };
	IO_STATUS_BLOCK iosb = { 0 };
	NTSTATUS status = STATUS_SUCCESS;

	// ��ȡ�ļ����
	InitializeObjectAttributes(&objectAttributes, &ustrPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&hFile, FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_ANY_ACCESS,
		&objectAttributes, &iosb, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
		NULL, 0);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ZwCreateFile[ERROR][%u]", status));
		return FALSE;
	}

	// �����ļ�
	// ע��˴��Ĵ�С!!!һ��Ҫ�����㹻�ڴ棬�������ExFreePool������
	ULONG ulLength = (2 * 4096 + sizeof(FILE_BOTH_DIR_INFORMATION)) * 0x2000;
	PFILE_BOTH_DIR_INFORMATION pDir = ExAllocatePoolZero(PagedPool, ulLength, 'elif');
	// ����pDir���׵�ַ�������ͷ��ڴ�ʹ��!!!
	PFILE_BOTH_DIR_INFORMATION pBeginAddr = pDir;
	// ��ȡ��Ϣ
	status = ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, &iosb, pDir, ulLength,
		FileBothDirectoryInformation, FALSE, NULL, FALSE);
	if (!NT_SUCCESS(status))
	{
		ExFreePoolWithTag(pBeginAddr, 'elif');
		ZwClose(hFile);
		KdPrint(("ZwQueryDirectoryFile[ERROR][%u]", status));
		return FALSE;
	}
	// ����
	UNICODE_STRING ustrTemp;
	UNICODE_STRING ustrOne;
	UNICODE_STRING ustrTwo;
	RtlInitUnicodeString(&ustrOne, L".");
	RtlInitUnicodeString(&ustrTwo, L"..");
	WCHAR wcFileName[1024] = { 0 };
	while (TRUE)
	{
		// �ж��Ƿ����ϼ�Ŀ¼���Ǳ�Ŀ¼
		RtlZeroMemory(wcFileName, 1024);
		RtlCopyMemory(wcFileName, pDir->FileName, pDir->FileNameLength);
		RtlInitUnicodeString(&ustrTemp, wcFileName);
		if ((0 != RtlCompareUnicodeString(&ustrTemp, &ustrOne, TRUE)) &&
			(0 != RtlCompareUnicodeString(&ustrTemp, &ustrTwo, TRUE)))
		{
			if (pDir->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Ŀ¼
				KdPrint(("[DIRECTORY]\t%wZ\n", &ustrTemp));
			}
			else
			{
				// �ļ�
				KdPrint(("[FILE]\t\t%wZ\n", &ustrTemp));
			}
		}
		// �������
		if (0 == pDir->NextEntryOffset)
		{
			KdPrint(("[QUERY OVER]\n\n"));
			break;
		}
		// pDirָ��ĵ�ַ�ı��ˣ���������ExFreePool(pDir)������������ԣ����뱣���׵�ַ
		pDir = (PFILE_BOTH_DIR_INFORMATION)((PUCHAR)pDir + pDir->NextEntryOffset);
	}
	// �ͷ��ڴ�, �ر��ļ����
	ExFreePoolWithTag(pBeginAddr, 'elif');
	ZwClose(hFile);

	return TRUE;
}
*/