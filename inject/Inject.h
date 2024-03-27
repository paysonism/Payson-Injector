#pragma once
#include "../includes.h"

class Inject
{
public:
	Inject() {};
	~Inject() {};
	bool inject_module_from_path_to_process_by_name(const wchar_t* module_path, const wchar_t* process_name);

private:

};

