#include "Inject.h"
/*
	!!!IMPORTANT!!! for this to work correctly please disable Security Check (/GS-)
*/
#define RELOC_FLAG64(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

//keep this 
#pragma runtime_checks( "", off )
#pragma optimize( "", off )
void __stdcall shellcode()
{
	uintptr_t base = 0x15846254168; // random
	uintptr_t pointer_address = 0x24856841253; // random

	memset((void*)pointer_address, 0x69, 1);

	BYTE* pBase = (BYTE*)base;
	auto* pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>((uintptr_t)pBase)->e_lfanew)->OptionalHeader;

	auto _LoadLibraryA = LI_FN(LoadLibraryA).get();
	auto _GetProcAddress = LI_FN(GetProcAddress).get();
	auto _RtlAddFunctionTable = LI_FN(RtlAddFunctionTable).get();

	auto _DllMain = reinterpret_cast<BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved)>(pBase + pOpt->AddressOfEntryPoint);

	BYTE* LocationDelta = pBase - pOpt->ImageBase;
	if (LocationDelta) {
		if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) {
			auto* pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			const auto* pRelocEnd = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<uintptr_t>(pRelocData) + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);
			while (pRelocData < pRelocEnd && pRelocData->SizeOfBlock) {
				UINT AmountOfEntries = (pRelocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
				WORD* pRelativeInfo = reinterpret_cast<WORD*>(pRelocData + 1);

				for (UINT i = 0; i != AmountOfEntries; ++i, ++pRelativeInfo) {
					if (RELOC_FLAG64(*pRelativeInfo)) {
						UINT_PTR* pPatch = reinterpret_cast<UINT_PTR*>(pBase + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
						*pPatch += reinterpret_cast<UINT_PTR>(LocationDelta);
					}
				}
				pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(pRelocData) + pRelocData->SizeOfBlock);
			}
		}
	}

	if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
		auto* pImportDescr = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		while (pImportDescr->Name) {
			char* szMod = reinterpret_cast<char*>(pBase + pImportDescr->Name);
			HINSTANCE hDll = _LoadLibraryA(szMod);

			ULONG_PTR* pThunkRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->OriginalFirstThunk);
			ULONG_PTR* pFuncRef = reinterpret_cast<ULONG_PTR*>(pBase + pImportDescr->FirstThunk);

			if (!pThunkRef)
				pThunkRef = pFuncRef;

			for (; *pThunkRef; ++pThunkRef, ++pFuncRef) {
				if (IMAGE_SNAP_BY_ORDINAL(*pThunkRef)) {
					*pFuncRef = (ULONG_PTR)_GetProcAddress(hDll, reinterpret_cast<char*>(*pThunkRef & 0xFFFF));
				}
				else {
					auto* pImport = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pBase + (*pThunkRef));
					*pFuncRef = (ULONG_PTR)_GetProcAddress(hDll, pImport->Name);
				}
			}
			++pImportDescr;
		}
	}

	if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size) {
		auto* pTLS = reinterpret_cast<IMAGE_TLS_DIRECTORY*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
		auto* pCallback = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(pTLS->AddressOfCallBacks);
		for (; pCallback && *pCallback; ++pCallback)
			(*pCallback)(pBase, DLL_PROCESS_ATTACH, nullptr);
	}


	//SEH SUPPORT
	auto excep = pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
	if (excep.Size) {
		if (!_RtlAddFunctionTable(
			reinterpret_cast<IMAGE_RUNTIME_FUNCTION_ENTRY*>(pBase + excep.VirtualAddress),
			excep.Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY), (DWORD64)pBase)) {
		}
	}

	_DllMain(pBase, DLL_PROCESS_ATTACH, 0);
}

bool Inject::inject_module_from_path_to_process_by_name(const wchar_t* module_path, const wchar_t* process_name)
{
	int target_process_id = utils::get_pid_from_name(process_name);
	
	if (!target_process_id)
	{
		printf(xor_a(" Fortnite is not loaded, make sure that you are in lobby before loading!\n"));
		return false;
	}

	printf(xor_a(""), target_process_id);

	auto target_process_hwnd = utils::get_hwnd_of_process_id(target_process_id); // HWND needed for hook
	auto nt_dll = LoadLibraryA(xor_a("ntdll.dll"));
	auto thread_id = GetWindowThreadProcessId(target_process_hwnd, 0); // also needed for hook


	uintptr_t target_file = utils::read_file_by_name(module_path);

	if (!target_file)
	{
		printf(xor_a(" Cannot find payson1337.dll in the current folder.\n"));
		return false;
	}

	printf(xor_a(""), target_file);


	PIMAGE_NT_HEADERS nt_header = utils::get_nt_header(target_file);



	Driver* driver = new Driver(xor_w(L"\\\\.\\{zxchigfysordfds-dsyfgsydfzxhcgzxjhgvsdf}"), target_process_id);
	
	uintptr_t target_process_base_address = driver->get_base_address(process_name);

	uintptr_t allocated_base = driver->allocate_virtual_memory(nt_header->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

	driver->protect_virtual_memory((uintptr_t)allocated_base, nt_header->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE);

	printf(xor_a(""), nt_header->OptionalHeader.SizeOfImage, allocated_base);

	if (!driver->write_memory((uintptr_t)allocated_base, (uintptr_t)target_file, 0x1000))
	{
		printf(xor_a("Failed writing memory at %p\n"), allocated_base);
	}
	printf(xor_a(""), allocated_base);

	IMAGE_SECTION_HEADER* section_header = IMAGE_FIRST_SECTION(nt_header);
	for (int i = 0; i != nt_header->FileHeader.NumberOfSections; i++, ++section_header)
	{
		if (section_header->SizeOfRawData)
		{
			if (!driver->write_memory((uintptr_t)allocated_base + section_header->VirtualAddress, (uintptr_t)target_file + section_header->PointerToRawData, section_header->SizeOfRawData)) {
			//	printf(xor_a("Failed writing memory at %p\n"), allocated_base + section_header->VirtualAddress);

				return false;
			}
		}
	}
	printf(xor_a("Successfully wrote sections!\n"));

	uintptr_t allocated_shellcode = driver->allocate_virtual_memory(0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	uintptr_t shellcode_value = driver->allocate_virtual_memory(sizeof(int), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // there we set the value to stop the hook once the shellcode is called!
	printf(xor_a(""), 0x1000, allocated_shellcode);


	uintptr_t allocatedbase_offset = uintptr_t((uintptr_t)utils::find_pattern(xor_a("\x68\x41\x25\x46\x58\x01\x00\x00"), xor_a("xxxxxx??")) - (uintptr_t)&shellcode); //scans the value 0x15846254168 in shellcode
	uintptr_t allocatedvalue_offset = uintptr_t((uintptr_t)utils::find_pattern(xor_a("\x53\x12\x84\x56\x48\x02\x00\x00"), xor_a("xxxxxx??")) - (uintptr_t)&shellcode); // scans the value 0x24856841253 in shellcode
	if (!allocatedbase_offset || !allocatedvalue_offset)
	{
		printf(xor_a(""));
		return false;
	}

	auto shellcodefunction_length = utils::get_function_length(&shellcode);
	uintptr_t localshellcodealloc = (uintptr_t)VirtualAlloc(0, shellcodefunction_length, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	memcpy((PVOID)localshellcodealloc, &shellcode, shellcodefunction_length);

	*(uintptr_t*)(localshellcodealloc + allocatedbase_offset) = allocated_base;
	*(uintptr_t*)(localshellcodealloc + allocatedvalue_offset) = shellcode_value;

	driver->write_memory(allocated_shellcode, localshellcodealloc, 0x1000);
	
	auto win_event_hook = SetWinEventHook(EVENT_MIN, EVENT_MAX, nt_dll, (WINEVENTPROC)allocated_shellcode, target_process_id, thread_id, WINEVENT_INCONTEXT);
	printf(xor_a("Injected"), win_event_hook);
	while (true)
	{
		
		int buffer;
		driver->read_memory(shellcode_value, (uintptr_t)&buffer, sizeof(int));
		if (buffer == 0x69) { // if shellcode called
			UnhookWinEvent(win_event_hook);
			return true;
		}
	}
	return false;
}
