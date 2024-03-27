#include "includes.h"
#include <iostream>
#include "xorstr.hpp"
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <random>
#include <iostream>
#include <tlhelp32.h>
#include <tchar.h>
#include <fstream>
#include <filesystem>
#include "log.h"

/*
* 
* Payson Injector
* github.com/paysonism
* 
* This is a simple open source injector made by Payson and Skar
* 
*/

auto main ( ) -> int
{
    Inject* inject = new Inject();
    system("color E");
    std::cout << "\n Payson Injector - github.com/paysonism\n DLL Name: payson1337.dll\n\n Press [F5] in Lobby\n\n";
    while (true) {
        if (GetAsyncKeyState(VK_F5) & 0x8000) {
            inject->inject_module_from_path_to_process_by_name(xor_w(L"payson1337.dll"), xor_w(L"FortniteClient-Win64-Shipping.exe"));
            Sleep(1000);
        }
    }
}


