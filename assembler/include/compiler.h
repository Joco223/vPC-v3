#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <utility>

#include "parser.h"

typedef unsigned char byte;
typedef unsigned short word;

struct instruction_signature {
	std::string instruction_name;
	std::vector<int> argument_types;

	bool operator==(const instruction_signature& x) const {
		if(instruction_name != x.instruction_name) {
			return false;
		}

		if(argument_types.size() == x.argument_types.size()) {
			for(int i = 0; i < x.argument_types.size(); i++){
				if(argument_types[i] != x.argument_types[i]) {
					return false;
				}
			}
		}else{
			return false;
		}

		return true;
	}
};

struct instruction_signature_hash {
	std::size_t operator()(const instruction_signature& x) const {
		auto h1 = std::hash<std::string>{}(x.instruction_name);
		auto h2 = std::hash<int>{}(x.argument_types[0]);

		for(int i = 1; i < x.argument_types.size(); i++) {
			h2 = h2 ^ std::hash<int>{}(x.argument_types[i]);
		}

		return h1 ^ h2;
	}
};

std::unordered_map<instruction_signature, byte, instruction_signature_hash> instruction_bytecodes = {
	{{"mov"   , {lt_value     , lt_register  }}, 0x01},
	{{"mov"   , {lt_address   , lt_register_l}}, 0x02},
	{{"mov"   , {lt_address   , lt_register_m}}, 0x03},
	{{"mov"   , {lt_register_l, lt_address   }}, 0x04},
	{{"mov"   , {lt_register_m, lt_address   }}, 0x05},
	{{"mov"   , {lt_register  , lt_register  }}, 0x0a},
	{{"mov_p" , {lt_address   , lt_register_l}}, 0x06},
	{{"mov_p" , {lt_address   , lt_register_m}}, 0x07},
	{{"mov_p" , {lt_register_l, lt_address   }}, 0x08},
	{{"mov_p" , {lt_register_m, lt_address   }}, 0x09},
	{{"mov_pr", {lt_register  , lt_register_l}}, 0x0b},
	{{"mov_pr", {lt_register  , lt_register_m}}, 0x0c},
	{{"mov_pr", {lt_register_l, lt_register  }}, 0x0d},
	{{"mov_pr", {lt_register_m, lt_register  }}, 0x0e},
	{{"add"   , {lt_register  , lt_register  }}, 0x0f},
	{{"sub"   , {lt_register  , lt_register  }}, 0x10},
	{{"mlt"   , {lt_register  , lt_register  }}, 0x11},
	{{"divu"  , {lt_register  , lt_register  }}, 0x12},
	{{"divs"  , {lt_register  , lt_register  }}, 0x13},
	{{"mod"   , {lt_register  , lt_register  }}, 0x14},
	{{"lrg"   , {lt_register  , lt_register  }}, 0x15},
	{{"lrg_e" , {lt_register  , lt_register  }}, 0x16},
	{{"sml"   , {lt_register  , lt_register  }}, 0x17},
	{{"sml_e" , {lt_register  , lt_register  }}, 0x18},
	{{"eql"   , {lt_register  , lt_register  }}, 0x19},
	{{"jmp"   , {lt_address                  }}, 0x1a},
	{{"jmpz"  , {lt_register  , lt_address   }}, 0x1b},
	{{"jmpo"  , {lt_register  , lt_address   }}, 0x1c},
	{{"jmpof" , {lt_address                  }}, 0x1d},
	{{"jmpuf" , {lt_address                  }}, 0x1e},
	{{"push"  , {lt_register                 }}, 0x1f},
	{{"peek"  , {lt_register                 }}, 0x20},
	{{"pop"   , {lt_register                 }}, 0x21},
	{{"jmpc"  , {lt_register                 }}, 0x22},
	{{"jmpp"  , {lt_register                 }}, 0x23},
	{{"outu"  , {lt_register                 }}, 0x24},
	{{"outs"  , {lt_register                 }}, 0x25},
	{{"outc"  , {lt_register                 }}, 0x26},
	{{"in"    , {lt_address                  }}, 0x27},
	{{"out"   , {lt_register  , lt_address   }}, 0x28},
};

std::unordered_map<std::string, int> register_lookup = {
	{"rel_a", 0}, {"rel_b", 1}, {"rel_c", 2}, {"rel_d", 3}, {"rel_e", 4}, {"rel_f", 5}, {"rel_g", 6}, {"rel_h", 7},
	{"rem_a", 0}, {"rem_b", 1}, {"rem_c", 2}, {"rem_d", 3}, {"rem_e", 4}, {"rem_f", 5}, {"rem_g", 6}, {"rem_h", 7},
	{"reg_a", 0}, {"reg_b", 1}, {"reg_c", 2}, {"reg_d", 3}, {"reg_e", 4}, {"reg_f", 5}, {"reg_g", 6}, {"reg_h", 7}
};

std::unordered_map<int, std::string> argument_type_lookup = {
	{lt_value     , "value"         },
	{lt_address   , "address"       },
	{lt_register_l, "register (LSB)"},
	{lt_register_m, "register (MSB)"},
	{lt_register  , "register"      }
};

byte get_lsb(word argument) {
	return argument & 0b0000000011111111;
}

byte get_msb(word argument) {
	return (argument & 0b1111111100000000) >> 8;
}

byte set_tp_byte(byte argument1, byte argument2) { //Set two part byte
	byte result;
	result |= (argument2 & 0x0F);
	result |= ((argument1 << 4) & 0xF0);
	return result;
}

void compile(std::vector<std::string>& lines, std::string output_file_path) {
  std::vector<l_instruction> parsed_instructions = parse(lines);
	std::vector<byte> compiled_program;

	for(auto& instruction : parsed_instructions) {
		instruction_signature ins_sig;
		ins_sig.instruction_name = instruction.instruction.first;
		for(auto& arg_type : instruction.arguments) { ins_sig.argument_types.push_back(arg_type.type); }

		if(instruction_bytecodes.find(ins_sig) == instruction_bytecodes.end()) {
			std::cout << "Error, couldn't find instruction signature on line: " << instruction.instruction.second << " for:\n";
			std::cout << ins_sig.instruction_name << ' ';
			for(auto& argument : ins_sig.argument_types) { std::cout << argument_type_lookup[argument] << ' '; }
			std::cout << "\nAborting...\n";
			return;
		}else{
			compiled_program.push_back(instruction_bytecodes[ins_sig]);

			if(ins_sig.argument_types.size() == 1) {
				if(ins_sig.argument_types[0] == lt_register || ins_sig.argument_types[0] == lt_register_l || ins_sig.argument_types[0] == lt_register_m)  {
					compiled_program.push_back(set_tp_byte(0, register_lookup[instruction.arguments[0].argument]));
				}else{
					compiled_program.push_back(get_msb((word)std::stoi(instruction.arguments[0].argument, nullptr, instruction.arguments[0].number_base)));
					compiled_program.push_back(get_lsb((word)std::stoi(instruction.arguments[0].argument, nullptr, instruction.arguments[0].number_base)));
				}
			}else{
				if(ins_sig.argument_types[0] == lt_register && ins_sig.argument_types[1] == lt_register) {
					compiled_program.push_back(set_tp_byte(register_lookup[instruction.arguments[0].argument], register_lookup[instruction.arguments[1].argument]));
				}else{
					if((ins_sig.argument_types[0] == lt_register_l || ins_sig.argument_types[0] == lt_register_m) && ins_sig.argument_types[1] == lt_address) {
						compiled_program.push_back(set_tp_byte(0, register_lookup[instruction.arguments[0].argument]));
						compiled_program.push_back(get_msb((word)std::stoi(instruction.arguments[1].argument, nullptr, instruction.arguments[1].number_base)));
						compiled_program.push_back(get_lsb((word)std::stoi(instruction.arguments[1].argument, nullptr, instruction.arguments[1].number_base)));
					}else if(ins_sig.argument_types[0] == lt_address && (ins_sig.argument_types[1] == lt_register_l || ins_sig.argument_types[1] == lt_register_m)) {
						compiled_program.push_back(set_tp_byte(0, register_lookup[instruction.arguments[1].argument]));
						compiled_program.push_back(get_msb((word)std::stoi(instruction.arguments[0].argument, nullptr, instruction.arguments[0].number_base)));
						compiled_program.push_back(get_lsb((word)std::stoi(instruction.arguments[0].argument, nullptr, instruction.arguments[0].number_base)));
					}else{
						compiled_program.push_back(set_tp_byte(0, register_lookup[instruction.arguments[1].argument]));
						compiled_program.push_back(get_msb((word)std::stoi(instruction.arguments[0].argument, nullptr, instruction.arguments[0].number_base)));
						compiled_program.push_back(get_lsb((word)std::stoi(instruction.arguments[0].argument, nullptr, instruction.arguments[0].number_base)));		
					}		
				}
			}
		}
	}

	std::fstream output_file(output_file_path, std::ios::out | std::ios::binary);
	output_file.write((char*)&compiled_program[0], sizeof(byte)*compiled_program.size());
	output_file.close();
}