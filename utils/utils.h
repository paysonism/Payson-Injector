#pragma once


namespace utils
{
	inline int get_pid_from_name(const wchar_t* name)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);
		Process32First(snapshot, &entry);
		do
		{
			if (wcscmp(entry.szExeFile, name) == 0)
			{
				return entry.th32ProcessID;
			}

		} while (Process32Next(snapshot, &entry));

		return 0; // if not found
	}
	inline uintptr_t read_file_by_name(const wchar_t* file_path)
	{
		HANDLE h_dll = CreateFileW(file_path ,GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (h_dll == INVALID_HANDLE_VALUE) return 0;
		int file_size = GetFileSize(h_dll, 0);
		PVOID buffer = VirtualAlloc(0, file_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!ReadFile(h_dll, buffer, file_size, 0, FALSE) || *(int*)(buffer) != 9460301) //  
		{
		
			CloseHandle(h_dll);
			VirtualFree(buffer,0, MEM_RELEASE);
			return 0;
		}
		else
		{
			CloseHandle(h_dll);
			return (uintptr_t)buffer;
		}
	}
	inline PIMAGE_NT_HEADERS get_nt_header(uintptr_t base)
	{
		PIMAGE_DOS_HEADER dos_headers = PIMAGE_DOS_HEADER(base);
		return PIMAGE_NT_HEADERS(base + dos_headers->e_lfanew);
	}
	inline bool mask_compare(void* buffer, const char* pattern, const char* mask)
	{
		for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b)
		{
			if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	inline PBYTE find_pattern(const char* pattern, const char* mask)
	{
		MODULEINFO info = {0};
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(0), &info, sizeof(info));
		info.SizeOfImage -= static_cast<DWORD>(strlen(mask));
		for (auto i = 0UL; i < info.SizeOfImage; i++)
		{
			auto addr = reinterpret_cast<PBYTE>(info.lpBaseOfDll) + i;
			if (mask_compare(addr, pattern, mask))
			{
				return addr;
			}
		}
	}
	inline int get_function_length(void* funcaddress)
	{
		int length = 0;
		for (length = 0; *((UINT32*)(&((unsigned char*)funcaddress)[length])) != 0xCCCCCCCC; ++length);
		return length;
	}
	inline HWND hwndout;
	inline BOOL EnumWindowProcMy(HWND input, LPARAM lParam)
	{
	
		DWORD lpdwProcessId;
		GetWindowThreadProcessId(input, &lpdwProcessId);
		if (lpdwProcessId == lParam)
		{
			hwndout = input;
			return FALSE;
		}
		return true;
	}
	inline HWND get_hwnd_of_process_id(int target_process_id)
	{
		EnumWindows(EnumWindowProcMy, target_process_id);
		return hwndout;
	}
	

}
