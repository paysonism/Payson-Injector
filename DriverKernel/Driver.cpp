#include "Driver.h"

Driver::Driver(const wchar_t* driver_name, int target_process_id)
{
	this->driver_handle = CreateFileW(driver_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	this->target_process_id = target_process_id; // yes i am lazy
}
Driver::~Driver()
{
	CloseHandle(driver_handle);
}
uintptr_t Driver::get_base_address(const wchar_t* process_name)
{
	if (!driver_handle) return 0;

	k_get_base_module_request request;
	request.pid = target_process_id;

	memset(request.name, 0, sizeof(WCHAR) * 260);
	wcscpy(request.name, process_name);
	request.handle = 0; // Make Sure that it is 0 so it doesnt return another one

	DeviceIoControl(driver_handle, ioctl_get_module_base, &request, sizeof(k_get_base_module_request), &request, sizeof(k_get_base_module_request), 0, 0);
	return request.handle;
}

uintptr_t Driver::allocate_virtual_memory(int size, int allocation_type, int protect_type)
{
	if (!driver_handle) return 0;
	k_alloc_mem_request request;
	request.pid = this->target_process_id;
	request.addr = (PVOID)(0);
	request.size = size;
	request.allocation_type = allocation_type;
	request.protect = protect_type;
	DeviceIoControl(driver_handle, ioctl_allocate_virtual_memory, &request, sizeof(k_alloc_mem_request), &request, sizeof(k_alloc_mem_request), 0, 0);
	return (uintptr_t)request.addr;
	
}
bool Driver::write_memory(uintptr_t destination, uintptr_t source, int size)
{
	if (driver_handle == INVALID_HANDLE_VALUE) return 0;
	k_rw_request request;
	request.pid = this->target_process_id;
	request.dst = destination;
	request.src = source;
	request.size = size;
	return DeviceIoControl(driver_handle, ioctl_write_memory, &request, sizeof(k_rw_request), 0, 0, 0, 0);
}
bool Driver::read_memory(uintptr_t source, uintptr_t destination, int size)
{
	if (driver_handle == INVALID_HANDLE_VALUE) return 0;
	k_rw_request request;
	request.pid = this->target_process_id;
	request.dst = destination;
	request.src = source;
	request.size = size;
	return DeviceIoControl(driver_handle, ioctl_read_memory, &request, sizeof(k_rw_request), 0, 0, 0, 0);
}

bool Driver::protect_virtual_memory(uintptr_t address, int size, int protect_type)
{
	if (!driver_handle) return 0;
	k_protect_mem_request request;
	request.pid = this->target_process_id;
	request.addr = address;
	request.size = size;
	request.protect = protect_type;
	return DeviceIoControl(driver_handle, ioctl_protect_virutal_memory, &request, sizeof(k_protect_mem_request), &request, sizeof(k_protect_mem_request), 0, 0);

}