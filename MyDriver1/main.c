#include <ntddk.h>

// ����һ���豸���󣬽��ܵĲ����� DriverEntry �������������
NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject)
{
	// 1. ��ʼ���豸������ڲ�����(ֻ�ܱ� R0 ʶ�������)
	// 2. �����豸���󣬲����ڵ�ָ��������������
	// 3. �����ڲ�����ֻ�����ں˲�ʹ�ã���Ҫ����һ���ⲿ������

	// �����ú�����Ҫʹ�õ��ľֲ�����
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_OBJECT DeviceObject = NULL;

	// ��ʼ���豸��������ƣ�Ҫ���ʽ������ \\Device\\xxx ����ʽ
	UNICODE_STRING DeviceName = { 0 };
	RtlInitUnicodeString(&DeviceName, L"\\Device\\.min");

	// �����豸����ʹ�� IoCreateDevice������ɹ����� STATUS_SUCCESS
	Status = IoCreateDevice(
		DriverObject,				// ���������������豸���󴴽������ӵ��� DeviceObjict ��Ա��
		0,							// �豸����չ�ռ��С������Ŀռ���� DeviceExtension �ֶ�ָ��
		&DeviceName,				// �豸��������ƣ�������ϸ�ʽ \\Device\\DeviceName
		FILE_DEVICE_UNKNOWN,		// �豸��������ͣ���ָӲ���޹ص������豸����
		0,							// �豸���������
		FALSE,						// �Ƿ����ö�ռģʽ��ͬһʱ���ܱ��򿪼���
		&DeviceObject);				// ���������豸��������һ��ָ��ָ��

	// ͨ�� NT_SUCCESS �жϺ����ĵ����Ƿ�ɹ�
	if (!NT_SUCCESS(Status))
	{
		KdPrint(("�豸���󴴽�ʧ�ܣ����ԭ�򣬴�����(%08X)\n", Status));
		return Status;
	}

	// �豸���������ֻ�����ں��б�ֱ�ӵĽ�����Ϊ�� R3 �ܹ�ʶ�𲢲����豸������Ҫ
	//	�������豸����ֱ�ӹ����ķ���������������д��: \\DosDevices\\xxx �� \\??\\xxx
	UNICODE_STRING SymLinkName = { 0 };
	RtlInitUnicodeString(&SymLinkName, L"\\??\\.min");
	Status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);

	// ͨ�� NT_SUCCESS �жϺ����ĵ����Ƿ�ɹ�
	if (!NT_SUCCESS(Status))
	{
		IoDeleteDevice(DeviceObject);
		KdPrint(("�������Ӵ���ʧ�ܣ����ԭ�򣬴�����(%08X)\n", Status));
		return Status;
	}

	// �����豸����Ķ�д��ʽΪ��������ʽ
	DeviceObject->Flags |= DO_DIRECT_IO;

	return Status;
}


// ���ڴ������з���������� IRP ����
NTSTATUS DefaultDispath(
	PDEVICE_OBJECT DeviceOBject,		// ��ʾ��ǰ����Ϣ����һ���豸������ܵ�
	PIRP Irp)							// IRP�����յ�����Ϣ��ʲô������ R3 �� MSG
{
	UNREFERENCED_PARAMETER(DeviceOBject);

	// ���õ�ǰ��Ϣ�Ĵ���״̬������ɹ����뷵�� STATUS_SUCCESS�����ķ���ֵӰ�� R3 �� GetLastError
	Irp->IoStatus.Status = STATUS_SUCCESS;

	// ���õ�ǰ����Ϣ�����˶��ٸ��ֽڵ����ݣ�Ӱ�� ReadFile �з��ص�ʵ�ʶ�д�ֽ���
	Irp->IoStatus.Information = 0;

	// ֪ͨ IO ��������ǰ�� IRP �Ѿ�����ɹ�����Ҫ���ظ� R3
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// ��ǰ�����ķ��ؽṹ������Ƿ�ɹ�
	return STATUS_SUCCESS;
}

// ���� ReadFile ��������Ϣ
NTSTATUS ReadDispath(
	PDEVICE_OBJECT DeviceOBject,		// ��ʾ��ǰ����Ϣ����һ���豸������ܵ�
	PIRP Irp)							// IRP�����յ�����Ϣ��ʲô������ R3 �� MSG
{
	// ���� IRP �ṹ��֮�⣬IRP ջ�����˵�ǰ�� IRP �ĸ��Ӳ���
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

	// Irp ջ�б����˴��û��㴫�ݽ�����һЩ��Ϣ�������� R3 ���ȡ�ĳ���
	KdPrint(("Length: %d\n", IrpStack->Parameters.Read.Length));

	// ���ʹ�õ��� DIRECT ��ʽ����ôϵͳ��������ṩһ���󶨵��û���������
	//	MDL��ͨ�� MmGetSystemAddressForMdlSafe ����ӳ��
	PVOID Buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	RtlCopyMemory(Buffer, "hello", 6);

	// ��ʵ�ʵĲ������������ظ� R3���� ReadFile �ĵ� 4 ����������
	Irp->IoStatus.Information = 20;

	UNREFERENCED_PARAMETER(DeviceOBject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// ���� WriteFile ��������Ϣ
NTSTATUS WriteDispath(
	PDEVICE_OBJECT DeviceOBject,		// ��ʾ��ǰ����Ϣ����һ���豸������ܵ�
	PIRP Irp)							// IRP�����յ�����Ϣ��ʲô������ R3 �� MSG
{
	// ���� IRP �ṹ��֮�⣬IRP ջ�����˵�ǰ�� IRP �ĸ��Ӳ���
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

	// Irp ջ�б����˴��û��㴫�ݽ�����һЩ��Ϣ�������� R3 ��д��ĳ���
	KdPrint(("Length: %d\n", IrpStack->Parameters.Write.Length));

	// ���ʹ�õ��� DIRECT ��ʽ����ôϵͳ��������ṩһ���󶨵��û���������
	//	MDL��ͨ�� MmGetSystemAddressForMdlSafe ����ӳ��
	PVOID Buffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
	KdPrint(("R3д����: %s\n", (char*)Buffer));

	// ��ʵ�ʵĲ������������ظ� R3���� ReadFile �ĵ� 4 ����������
	Irp->IoStatus.Information = 66;

	UNREFERENCED_PARAMETER(DeviceOBject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// ���������ж�غ�������������ã����޷�ж��
#pragma code_seg("PAGE")						// ��ʾ��ǰ�ĺ����������ڷ�ҳ�ڴ��У����
												// ��ʱ���ڲ����õ������Է��õ�ҳ�����ļ���
VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	// ����豸���󴴽��ɹ�����ô��Ҫ��ж�غ�����ɾ���豸����ͷ�����������
	// ����Ҫ��ɾ����������������ɾ���豸���󣬷������ֲ�������������

	// ɾ������������
	UNICODE_STRING SymLinkName = { 0 };
	RtlInitUnicodeString(&SymLinkName, L"\\??\\.min");
	IoDeleteSymbolicLink(&SymLinkName);

	// ɾ���豸��������ж������Ҫ�����豸�����
	IoDeleteDevice(DriverObject->DeviceObject);
}


// ��������������ں����������� DriverEntry��������û�����Ʒ����
#pragma code_seg("INIT")						// ��ʾ��ǰ�ĺ��������� INIT �����У����е�
												// �������ݶ�����������ʼ����ʱ���ܹ�ʹ�ã�һ
												// �� DriverEntry ִ�н������ͻᱻ�ͷŵ�
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = DriverUnload;

	// Ϊ��ǰ�����µ������豸����������Ϣ��Ӧ����
	for (int i = 0; i < 28; ++i)
		DriverObject->MajorFunction[i] = DefaultDispath;

	// ����һЩ����ĺ�������Ҫ������д���
	DriverObject->MajorFunction[IRP_MJ_READ] = ReadDispath;					// ReadFile
	DriverObject->MajorFunction[IRP_MJ_WRITE] = WriteDispath;				// WriteFile

	// ����һ���豸�������ʧ���򷵻ش�����
	NTSTATUS Status = CreateDevice(DriverObject);

	// ������صĲ��� STATUS_SUCESS�������ᰲװʧ�ܣ���ζ�� DriverUnload �������
	return Status;
}