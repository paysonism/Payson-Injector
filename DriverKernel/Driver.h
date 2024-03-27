#pragma once
#include "../includes.h"

class Driver
{
public:
	Driver(const wchar_t* driver_name, int target_process_id);
	~Driver();
	uintptr_t get_base_address(const wchar_t* process_name);
    uintptr_t allocate_virtual_memory(int size, int allocation_type, int protect_type);
    bool protect_virtual_memory(uintptr_t address, int size, int protect_type);
    bool write_memory(uintptr_t destination, uintptr_t source, int size);
    bool read_memory(uintptr_t source, uintptr_t destination, int size);
    HANDLE driver_handle;
    int target_process_id;
private:

    /*
            Driver Structs
    */
    typedef struct _k_get_base_module_request {
        ULONG pid;
        ULONGLONG handle;
        WCHAR name[260];
    } k_get_base_module_request, * pk_get_base_module_request;

    typedef struct _k_rw_request {
        ULONG pid;
        ULONGLONG src;
        ULONGLONG dst;
        ULONGLONG size;
    } k_rw_request, * pk_rw_request;

    typedef struct _k_alloc_mem_request {
        ULONG pid, allocation_type, protect;
        PVOID addr;
        SIZE_T size;
    } k_alloc_mem_request, * pk_alloc_mem_request;

    typedef struct _k_protect_mem_request {
        ULONG pid, protect;
        ULONGLONG addr;
        SIZE_T size;
    } k_protect_mem_request, * pk_protect_mem_request;

    /*
        Driver IOCTL codes
    */
    #define ioctl_read_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x193286, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
    #define ioctl_write_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1729823, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
    #define ioctl_get_module_base CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1461419, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
    #define ioctl_protect_virutal_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1433146, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
    #define ioctl_allocate_virtual_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1523794, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


};
