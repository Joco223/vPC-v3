#include <iostream>
#include <fstream>
#include <string>

#include "cpu.h"

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << "Provide the program file.\n";
		return 0;
	}
	std::fstream input_program_file(argv[1], std::ios::in | std::ios::binary);

	if(!input_program_file.is_open()) {
		std::cout << "Couln't open file: " << argv[1] << '\n';
		return 0; 
	}else{
		cpu CPU = cpu();

		std::string input_program((std::istreambuf_iterator<char>(input_program_file)), std::istreambuf_iterator<char>());
		input_program_file.close();
		
		CPU.load_data(input_program);

		while(!CPU.is_halted()) {
			CPU.proces_instruction();
		}

		std::cout << "\nCPU has halted.\n";

		return 0;
	}
}