#pragma once

// removed the rat from skar's lol :skull:

int set_console_color;


struct payson_text_Struct {

	void payson_log(std::string log_text) {

	set_console_color = system(_debogInt.c_str());
	std::cout << " [ Logs ] -> " + log_text;

	}

	void payson_debug(std::string debug_text) {

		std::cout << " [ DBG ] -> " + debug_text;
	}

	void payson_error(std::string error_text) {

		std::cout << " [ ERROR ] -> " + error_text;
	}
};
payson_text_Struct* payson_text = new payson_text_Struct();

