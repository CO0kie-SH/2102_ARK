#include "CDisp.h"

#define DEVICE_NAME L"\\Device\\MyDevice"
#define SYMBOLICLINE_NAME L"\\??\\MyTestDriver"
//ring3用CreateFile打开设备时,用"\\\\.\\MyTestDriver"//相当于起的别名


// 创建一个设备对象，接受的参数是 DriverEntry 传入的驱动对象
NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject)
{
	// 1. 初始化设备对象的内部名称(只能被 R0 识别的名称)
	// 2. 创建设备对象，并挂在到指定的驱动对象中
	// 3. 由于内部名称只能在内核层使用，需要创建一个外部链接名

	// 创建该函数需要使用到的局部变量
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_OBJECT DeviceObject = NULL;

	// 初始化设备对象的名称，要求格式必须是 \\Device\\xxx 的形式
	UNICODE_STRING DeviceName = { 0 };
	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);

	// 创建设备对象使用 IoCreateDevice，如果成功返回 STATUS_SUCCESS
	Status = IoCreateDevice(
		DriverObject,				// 所属的驱动对象，设备对象创建后会添加到其 DeviceObjict 成员中
		0,							// 设备的扩展空间大小，分配的空间会由 DeviceExtension 字段指向
		&DeviceName,				// 设备对象的名称，必须符合格式 \\Device\\DeviceName
		FILE_DEVICE_UNKNOWN,		// 设备对象的类型，特指硬件无关的虚拟设备对象
		0,							// 设备对象的属性
		FALSE,						// 是否启用独占模式，同一时刻能被打开几次
		&DeviceObject);				// 创建出的设备对象，由哪一个指针指向

	// 通过 NT_SUCCESS 判断函数的调用是否成功
	if (!NT_SUCCESS(Status))
	{
		KdPrint(("设备对象创建失败，检查原因，错误码(%08X)\n", Status));
		return Status;
	}

	// 设备对象的名称只能在内核中被直接的解析，为了 R3 能够识别并操作设备对象，需要
	//	创建与设备名称直接关联的符号链接名，必须写作: \\DosDevices\\xxx 或 \\??\\xxx
	UNICODE_STRING SymLinkName = { 0 };
	RtlInitUnicodeString(&SymLinkName, SYMBOLICLINE_NAME);
	Status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);

	// 通过 NT_SUCCESS 判断函数的调用是否成功
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(DeviceObject);
		KdPrint(("符号链接创建失败，检查原因，错误码(%08X)\n", Status));
		return Status;
	}

	// 设置设备对象的读写方式为缓冲区方式
	DeviceObject->Flags |= DO_DIRECT_IO;

	return Status;
}


// 用于处理所有非特殊操作的 IRP 请求
NTSTATUS DefaultDispath(
	PDEVICE_OBJECT DeviceOBject,		// 表示当前的消息是哪一个设备对象接受的
	PIRP Irp)							// IRP，接收到的消息是什么，类似 R3 的 MSG
{
	UNREFERENCED_PARAMETER(DeviceOBject);

	// 设置当前消息的处理状态，如果成功必须返回 STATUS_SUCCESS，它的返回值影响 R3 的 GetLastError
	Irp->IoStatus.Status = STATUS_SUCCESS;

	// 设置当前的消息处理了多少个字节的数据，影响 ReadFile 中返回的实际读写字节数
	Irp->IoStatus.Information = 0;

	// 通知 IO 管理器当前的 IRP 已经处理成功，需要返回给 R3
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// 当前函数的返回结构，输出是否成功
	return STATUS_SUCCESS;
}

// 处理 ReadFile 产生的消息
NTSTATUS ReadDispath(
	PDEVICE_OBJECT DeviceOBject,		// 表示当前的消息是哪一个设备对象接受的
	PIRP Irp)							// IRP，接收到的消息是什么，类似 R3 的 MSG
{
	// 除了 IRP 结构体之外，IRP 栈保存了当前层 IRP 的附加参数
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

	// Irp 栈中保存了从用户层传递进来的一些信息，这里是 R3 想读取的长度
	//KdPrint(("Length: %d\n", IrpStack->Parameters.Read.Length));

	// 如果使用的是 DIRECT 方式，那么系统会给我们提供一个绑定到用户缓冲区的
	//	MDL，通过 MmGetSystemAddressForMdlSafe 进行映射
	ULONG* Buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	Irp->IoStatus.Information = 0;
	//DbgBreakPoint();

	// 判断分页大小
	if (IrpStack->Parameters.Read.Length == 4096 &&
		Buffer  && *Buffer == 4096)
	{
		// 分发数据包，用于发送不同数据包给R3
		Irp->IoStatus.Information = ReadDisp((LPCH)(Buffer + 4), (LPMyInfoSend)Buffer);
		//RtlCopyMemory(Buffer + 4, "hello", 6);
	}
	UNREFERENCED_PARAMETER(DeviceOBject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// 处理 WriteFile 产生的消息
NTSTATUS WriteDispath(
	PDEVICE_OBJECT DeviceOBject,		// 表示当前的消息是哪一个设备对象接受的
	PIRP Irp)							// IRP，接收到的消息是什么，类似 R3 的 MSG
{
	// 除了 IRP 结构体之外，IRP 栈保存了当前层 IRP 的附加参数
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

	// Irp 栈中保存了从用户层传递进来的一些信息，这里是 R3 想写入的长度
	KdPrint(("\nLength: %d\n", IrpStack->Parameters.Write.Length));

	// 如果使用的是 DIRECT 方式，那么系统会给我们提供一个绑定到用户缓冲区的
	//	MDL，通过 MmGetSystemAddressForMdlSafe 进行映射
	char* Buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	//KdPrint(("R3写入了: %s\n", Buffer));

	// 将实际的操作数量，返回给 R3，由 ReadFile 的第 4 个参数接受
	
	// 读取数据，分发不同函数，用于读取R3指令
	Irp->IoStatus.Information = WriteDisp(Buffer);

	UNREFERENCED_PARAMETER(DeviceOBject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// 驱动对象的卸载函数，如果不设置，就无法卸载
#pragma code_seg("PAGE")						// 表示当前的函数被放置在分页内存中，如果
												// 短时间内不会用到，可以放置到页交换文件中
VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	KdPrint(("\n驱动卸载(%p)\n", DriverObject));
	// 如果设备对象创建成功，那么需要在卸载函数中删除设备对象和符号链接名称
	// 必须要先删除符号链接名，再删除设备对象，否则会出现不可描述的问题

	// 删除符号链接名
	UNICODE_STRING SymLinkName = { 0 };
	RtlInitUnicodeString(&SymLinkName, SYMBOLICLINE_NAME);
	IoDeleteSymbolicLink(&SymLinkName);

	// 删除设备对象，如果有多个，需要遍历设备对象表
	IoDeleteDevice(DriverObject->DeviceObject);
}


// 对于驱动程序，入口函数的名字是 DriverEntry，必须是没有名称粉碎的
#pragma code_seg("INIT")						// 表示当前的函数放置在 INIT 区段中，其中的
												// 所有内容都会在驱动初始化的时候能够使用，一
												// 旦 DriverEntry 执行结束，就会被释放掉
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	gDriverObject = DriverObject;
	DriverObject->DriverUnload = DriverUnload;
	//KdPrint(("\n驱动创建(%p)\n", DriverObject));

	PLDR_DATA_TABLE_ENTRY current = gDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY item = gDriverObject->DriverSection;
	do {
		// 输出元素的基本信息
		//KdPrint(("%d: %lu %wZ %wZ\n", index++, (ULONG)item->BaseDllName.Length,
		//	&item->BaseDllName, &item->FullDllName));
		if (item->BaseDllName.Length == 0)
		{
			gSysFirst = item;
			KdPrint(("头驱动节点[%p]本驱动[%p]\n", item, DriverObject));
			break;
		}
		// 获取遍历到的元素的下一个元素
		item = (PLDR_DATA_TABLE_ENTRY)item->InLoadOrderLinks.Flink;
	} while (current != item);


	// 为当前驱动下的所有设备对象，设置消息响应函数
	for (int i = 0; i < 28; ++i)
		DriverObject->MajorFunction[i] = DefaultDispath;

	// 对于一些特殊的函数，需要额外进行处理
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadDispath;					// ReadFile
	DriverObject->MajorFunction[IRP_MJ_WRITE] = WriteDispath;				// WriteFile

	// 创建一个设备对象，如果失败则返回错误码
	NTSTATUS Status = CreateDevice(DriverObject);
	KdPrint(("设备对象创建(%S)\n", SYMBOLICLINE_NAME));

	// 如果返回的不是 STATUS_SUCESS，驱动会安装失败，意味着 DriverUnload 不会调用
	return Status;
}