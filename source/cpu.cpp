#include "cpu.h"

cpu::cpu() : 
	instruction_pointer(0), 
	stack_pointer(0), 
	clock(0),
	halted(false),
	overflow_flag(false),
	underflow_flag(false) {

	registers.resize(8); //8 general purpose registers
	memory.resize(65535); //64KiB memory
	stack.resize(1); //To offser the stack pointer by one so it points to the last item
	instruction_arguments.resize(4); //Biggest instructions are 4 bytes big
}

void cpu::reset_cpu() {
	instruction_pointer = 0;
	stack_pointer = 0;
	clock = 0;
	halted = false;
	overflow_flag = false;
	underflow_flag = false;

	registers.clear();
	registers.resize(8);

	memory.clear();
	memory.resize(65535);

	stack.clear();
	stack.resize(1);

	instruction_arguments.clear();
	instruction_arguments.resize(4);
}

void cpu::set_memory_byte(byte data, word offset) {
	memory[offset] = data;
}

void cpu::load_data(std::string& data) {
	for(word i = 0; i < data.length(); i++) {
		memory[i] = data[i];
	}
}

byte cpu::get_lsn(byte argument) const {
	return argument & 0b00001111;
}

byte cpu::get_msn(byte argument) const {
	return argument & 0b11110000;
}

byte cpu::get_lsb(word argument) const {
	return argument & 0b0000000011111111;
}

byte cpu::get_msb(word argument) const {
	return argument & 0b1111111100000000;
}

void cpu::set_lsb(byte register_index, byte argument) {
	registers[register_index] &= 0x00FF;
	registers[register_index] |= (argument & 0x00FF);
}

void cpu::set_msb(byte register_index, byte argument) {
	registers[register_index] &= 0xFF00;
	registers[register_index] |= ((argument << 8) & 0xFF00);
}

word cpu::get_word(byte argument1, byte argument2) const {
	return (argument1 << 8) | argument2;
}

void cpu::clear_instruction_arguments() {
	for(int i = 0; i < instruction_arguments.size(); i++) {
		instruction_arguments[i] = 0;
	}
}

void cpu::proces_instruction() {
	byte op_code = decode_instruction();
	execute_instruction(op_code);
	instruction_pointer += instruction_size_map[op_code];
	clear_instruction_arguments();
}

byte cpu::decode_instruction() {
	byte op_code = memory[instruction_pointer];

	for(byte i = 0; i < instruction_size_map[op_code]-1; i++) {
		instruction_arguments[i] = memory[instruction_pointer + i + 1];
	}

	return op_code;
}

void cpu::execute_instruction(byte op_code) {
	word old_register_value = 0;
	std::string input;
	switch(op_code) {
		case 0x00: halted = true; break;
		case 0x01: registers[instruction_arguments[0]] = get_word(instruction_arguments[1], instruction_arguments[2]); break;
		case 0x02: set_lsb(instruction_arguments[0], memory[get_word(instruction_arguments[1], instruction_arguments[2])]); break;
		case 0x03: set_msb(instruction_arguments[0], memory[get_word(instruction_arguments[1], instruction_arguments[2])]); break;
		case 0x04: memory[get_word(instruction_arguments[1], instruction_arguments[2])] = get_lsb(instruction_arguments[0]); break;
		case 0x05: memory[get_word(instruction_arguments[1], instruction_arguments[2])] = get_msb(instruction_arguments[0]); break;
		case 0x06: set_lsb(instruction_arguments[0], memory[memory[get_word(instruction_arguments[1], instruction_arguments[2])]]); break;
		case 0x07: set_msb(instruction_arguments[0], memory[memory[get_word(instruction_arguments[1], instruction_arguments[2])]]); break;
		case 0x08: memory[memory[get_word(instruction_arguments[1], instruction_arguments[2])]] = get_lsb(instruction_arguments[0]); break;
		case 0x09: memory[memory[get_word(instruction_arguments[1], instruction_arguments[2])]] = get_msb(instruction_arguments[0]); break;
		case 0x0a: registers[get_msn(instruction_arguments[0])] = registers[get_lsn(instruction_arguments[0])]; break;
		case 0x0b: set_lsb(get_msn(instruction_arguments[0]), memory[registers[get_lsn(instruction_arguments[0])]]); break;
		case 0x0c: set_msb(get_msn(instruction_arguments[0]), memory[registers[get_lsn(instruction_arguments[0])]]); break;
		case 0x0d: memory[registers[get_lsn(instruction_arguments[0])]] = get_lsb(registers[get_msn(instruction_arguments[0])]); break;
		case 0x0e: memory[registers[get_lsn(instruction_arguments[0])]] = get_msb(registers[get_msn(instruction_arguments[0])]); break;

		case 0x0f: 
			old_register_value = registers[get_msn(instruction_arguments[0])];
			registers[get_msn(instruction_arguments[0])] += registers[get_lsn(instruction_arguments[0])];
			if (registers[get_msn(instruction_arguments[0])] < old_register_value)
				overflow_flag = true;
			break;
		case 0x10:
			old_register_value = registers[get_msn(instruction_arguments[0])];
			registers[get_msn(instruction_arguments[0])] -= registers[get_lsn(instruction_arguments[0])];
			if (registers[get_msn(instruction_arguments[0])] > old_register_value)
				underflow_flag = true;
			break;
		case 0x11:
			old_register_value = registers[get_msn(instruction_arguments[0])];
			registers[get_msn(instruction_arguments[0])] *= registers[get_lsn(instruction_arguments[0])];
			if (registers[get_msn(instruction_arguments[0])] < old_register_value)
				overflow_flag = true;
			break;
		case 0x12: registers[get_msn(instruction_arguments[0])] /= registers[get_lsn(instruction_arguments[0])]; break;
		case 0x13: registers[get_msn(instruction_arguments[0])] /= static_cast<short>(registers[get_lsn(instruction_arguments[0])]); break;
		case 0x14: registers[get_msn(instruction_arguments[0])] %= registers[get_lsn(instruction_arguments[0])]; break;

		case 0x15: registers[get_msn(instruction_arguments[0])] = registers[get_msn(instruction_arguments[0])] >  registers[get_lsn(instruction_arguments[0])]; break;
		case 0x16: registers[get_msn(instruction_arguments[0])] = registers[get_msn(instruction_arguments[0])] >= registers[get_lsn(instruction_arguments[0])]; break;
		case 0x17: registers[get_msn(instruction_arguments[0])] = registers[get_msn(instruction_arguments[0])] <  registers[get_lsn(instruction_arguments[0])]; break;
		case 0x18: registers[get_msn(instruction_arguments[0])] = registers[get_msn(instruction_arguments[0])] <= registers[get_lsn(instruction_arguments[0])]; break;
		case 0x19: registers[get_msn(instruction_arguments[0])] = registers[get_msn(instruction_arguments[0])] == registers[get_lsn(instruction_arguments[0])]; break;

		case 0x1a: instruction_pointer = get_word(instruction_arguments[0], instruction_arguments[1]); break;
		case 0x1b: if (registers[instruction_arguments[0]] == 0) { instruction_pointer = get_word(instruction_arguments[1], instruction_arguments[2]); } break;
		case 0x1c: if (registers[instruction_arguments[0]] == 1) { instruction_pointer = get_word(instruction_arguments[1], instruction_arguments[2]); } break;
		case 0x1d: if (overflow_flag)  { instruction_pointer = get_word(instruction_arguments[0], instruction_arguments[1]); overflow_flag  = false; } break;
		case 0x1e: if (underflow_flag) { instruction_pointer = get_word(instruction_arguments[0], instruction_arguments[1]); underflow_flag = false; } break;

		case 0x1f: stack.push_back(registers[instruction_arguments[0]]); stack_pointer++; break;
		case 0x20: registers[get_msn(instruction_arguments[0])] = stack[stack_pointer - registers[get_lsn(instruction_arguments[0])]]; break;
		case 0x21: for(word i = 0; i < registers[instruction_arguments[0]]; i++) { stack.pop_back(); stack_pointer--; } break;
		case 0x22: 
			stack.push_back(instruction_pointer);
			stack_pointer++;
			instruction_pointer = registers[instruction_arguments[0]];
			break;
		case 0x23: instruction_pointer = stack[stack_pointer - registers[instruction_arguments[0]]]; break;

		case 0x24: std::cout << static_cast<int>(registers[instruction_arguments[0]]); break;
		case 0x25: std::cout << static_cast<short>(registers[instruction_arguments[0]]); break;
		case 0x26: std::cout << static_cast<char>(registers[instruction_arguments[0]]); break;
		case 0x27:
			getline(std::cin, input);
			input = input.substr(0, 255);
			for (byte i = 0; i < input.length(); i++) {
				memory[get_word(instruction_arguments[0], instruction_arguments[1]) + i] = input[i];
			}
			break;
		case 0x28:
			for(word i = 0; i < registers[instruction_arguments[0]]; i++) {
				std::cout << memory[get_word(instruction_arguments[1], instruction_arguments[2]) + i];
			}
			break;
		default:
			std::cout << "Unknown instruction: " << std::hex << static_cast<int>(op_code) << ", halting.";
			halted = true;
	}
}

bool cpu::is_halted() {
	return halted;
}