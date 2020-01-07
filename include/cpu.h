#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>

typedef unsigned char byte;
typedef unsigned short word;

class cpu {
private:
	word instruction_pointer;
	word stack_pointer;

	unsigned int clock;
	bool halted, overflow_flag, underflow_flag;

	std::vector<word> registers;

	std::vector<byte> memory;
	std::vector<word> stack;

	std::vector<byte> instruction_arguments;
	byte decode_instruction();
	void execute_instruction(byte op_code);
	void clear_instruction_arguments();

	byte get_lsn(byte argument) const;
	byte get_msn(byte argument) const;
	byte get_msb(word argument) const;
	byte get_lsb(word argument) const;
	void set_msb(byte register_index, byte argument);
	void set_lsb(byte register_index, byte argument);
	word get_word(byte argument1, byte argument2) const;

	std::unordered_map<byte, byte> instruction_size_map = {{0x01, 4}, {0x02, 4}, {0x03, 4}, {0x04, 4},
																												 {0x05, 4}, {0x06, 4}, {0x07, 4}, {0x08, 4},
																												 {0x09, 4}, {0x0a, 2}, {0x0b, 2}, {0x0c, 2},
																												 {0x0d, 2}, {0x0e, 2}, {0x0f, 2}, {0x10, 2},
																												 {0x11, 2}, {0x12, 2}, {0x13, 2}, {0x14, 2},
																												 {0x15, 2}, {0x16, 2}, {0x17, 2}, {0x18, 2},
																												 {0x19, 2}, {0x1a, 3}, {0x1b, 4}, {0x1c, 4},
																												 {0x1d, 3}, {0x1e, 3}, {0x1f, 2}, {0x20, 2},
																												 {0x21, 2}, {0x22, 2}, {0x23, 2}, {0x24, 2},
																												 {0x25, 2}, {0x26, 2}, {0x27, 3}, {0x28, 4}};
public:
	cpu();

	void set_memory_byte(byte data, word offset);
	void load_data(std::vector<byte>& data);

	void reset_cpu();
	void proces_instruction();

	bool is_halted();
};