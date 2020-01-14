#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "compiler.h"

int main(int argc, char** argv) {

	if (argc < 3) {
		std::cout << "Provide input and output files.\n";
		return 0;
	}

	std::fstream input_program_file(argv[1], std::ios::in);

	if(!input_program_file.is_open()) {
		std::cout << "Couldn't open file: " << argv[1] << '\n';
		return 0;
	}

	std::vector<std::string> lines;
	std::string line_buffer;

	while(getline(input_program_file, line_buffer)) {
		lines.push_back(line_buffer);
	}

	compile(lines, argv[2]);

	return 0;
}