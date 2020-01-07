#include <iostream>

#include "cpu.h"

int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << "Provide the program file.\n";
		return 0;
	}

	cpu CPU = cpu();

	//Small test program that takes input, squares it and outputs it to the screen
	CPU.set_memory_byte(0x27, 0);
	CPU.set_memory_byte(0xaa, 1);
	CPU.set_memory_byte(0xaa, 2);

	CPU.set_memory_byte(0x02, 3);
	CPU.set_memory_byte(0x00, 4);
	CPU.set_memory_byte(0xaa, 5);
	CPU.set_memory_byte(0xaa, 6);

	CPU.set_memory_byte(0x11, 7);
	CPU.set_memory_byte(0x00, 8);

	CPU.set_memory_byte(0x24, 9);
	CPU.set_memory_byte(0x00, 10);

	while(!CPU.is_halted()) {
		CPU.proces_instruction();
	}

	std::cout << "\nCPU has halted.";

	return 0;
}