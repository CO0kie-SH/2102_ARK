#include "CMyFile.h"

// 使用指定的方式创建或打开一个指定的文件或目录
HANDLE CreateFile(LPCWSTR pFilePath, ACCESS_MASK Access, BOOLEAN IsFile)
{
	HANDLE FileHandle = NULL;
	IO_STATUS_BLOCK IoStatusBlock = { 0 };

	// 初始化文件或目录的名字保存到 UNICODE_STRING 中
	UNICODE_STRING FilePath = { 0 };
	RtlInitUnicodeString(&FilePath, pFilePath);

	// 操作一个具名对象的时候，通常都需要提供结构体，描述文件的名字
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	InitializeObjectAttributes(
		&ObjectAttributes,				// 需要初始化的对象属性结构体
		&FilePath,						// 当前的对象描述的是哪一个名称
		OBJ_CASE_INSENSITIVE,			// 表示对名称不区分大小写
		NULL, NULL);					// 对象根目录和安全属性字段


	// 根据用户传入的布尔值设置 CreateOption 的值是目录\文件
	ULONG CreateOption = IsFile ? FILE_NON_DIRECTORY_FILE
		: FILE_DIRECTORY_FILE;


	// 内核层操作文件的函数是 ZwCreateFile，如果失败返回失败原因
	NTSTATUS Status = ZwCreateFile(
		&FileHandle,					// 成功调用函数后用于保存句柄的变量
		Access,							// 以什么样的权限去操作文件 GENERIC_ALL
		&ObjectAttributes,				// 主要用于保存文件的路径
		&IoStatusBlock,					// 保存的是当前函数的执行结果
		0,								// 如果是创建新的文件，那么文件默认的大小是多少
		FILE_ATTRIBUTE_NORMAL,			// 创建或打开的文件应该具有什么样的属性
		FILE_SHARE_VALID_FLAGS,			// 文件允许的共享方式，和 R3 保持一致
		FILE_OPEN_IF,					// 打开\创建文件的方式 FILE_OPEN_IF 有就开
		CreateOption,					// 当前属性我们只需要关注操作的是文件还是目录
		NULL, 0);						// 描述扩展区域的大小，还有指向此区域的指针


	// 判断打开路径的操作是否成功，如果失败返回-1，否则返回句柄
	if (NT_SUCCESS(Status))
		return FileHandle;
	else
		return (HANDLE)-1;
}

// 向指定文件内写入指定大小的指定数据
NTSTATUS WriteFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	LARGE_INTEGER Offset = { pOffset, 0 };

	// 向指定的文件内写入指定长度的数据
	NTSTATUS Status = ZwWriteFile(
		FileHandle,					// 需要操作的到底是哪个文件
		NULL, NULL, NULL,			// 异步 IO 使用的三个参数，几乎不用
		&IoStatusBlock,				// 保存了操作的结果，例如字节数和是否成功
		Buffer, Length,				// 需要写入的数据以及写入的长度
		&Offset, 0);				// 需要操作的文件指针的位置

	// 无论是否操作成功，都直接返回操作结果
	return Status;
}

// 获取指定文件的基本信息，该函数主要获取文件的大小
LONGLONG GetFileSize(HANDLE FileHandle)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	FILE_STANDARD_INFORMATION FileInfo = { 0 };

	// 用于查询文件信息的函数，可以查询非常多的信息
	NTSTATUS Status = ZwQueryInformationFile(
		FileHandle,					// 需要查询哪个文件的信息
		&IoStatusBlock,				// 查询成功了还是失败了
		&FileInfo,					// 查询到的信息保存到哪里，结构体不是固定的
		sizeof(FileInfo),			// 提供的缓冲区的大小
		FileStandardInformation);	// 需要查询的是哪种类型的信息

	// 如果成功返回查询的结果，否则返回 0
	if (NT_SUCCESS(Status))
		return  FileInfo.EndOfFile.QuadPart;
	else
		return 0;
}

// 从指定文件读取指定大小的指定数据
NTSTATUS ReadFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	LARGE_INTEGER Offset = { pOffset, 0 };

	// 从指定文件读取指定长度的数据
	NTSTATUS Status = ZwReadFile(
		FileHandle,					// 需要操作的到底是哪个文件
		NULL, NULL, NULL,			// 异步 IO 使用的三个参数，几乎不用
		&IoStatusBlock,				// 保存了操作的结果，例如字节数和是否成功
		Buffer, Length,				// 需要读取的数据以及读取的长度
		&Offset, 0);				// 需要操作的文件指针的位置

	// 无论是否操作成功，都直接返回操作结果
	return Status;
}

// 删除文件，不需要指定文件句柄
NTSTATUS DeleteFile(LPCWSTR pFilePath)
{
	// 初始化文件或目录的名字保存到 UNICODE_STRING 中
	UNICODE_STRING FilePath = { 0 };
	RtlInitUnicodeString(&FilePath, pFilePath);

	// 操作一个具名对象的时候，通常都需要提供结构体，描述文件的名字
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	InitializeObjectAttributes(
		&ObjectAttributes,				// 需要初始化的对象属性结构体
		&FilePath,						// 当前的对象描述的是哪一个名称
		OBJ_CASE_INSENSITIVE,			// 表示对名称不区分大小写
		NULL, NULL);					// 对象根目录和安全属性字段

	// 返回删除的结果
	return ZwDeleteFile(&ObjectAttributes);
}