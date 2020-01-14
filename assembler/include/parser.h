#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

enum lexer_types {
  lt_instruction = 0,
  lt_value,
  lt_address,
  lt_register_l,
  lt_register_m,
  lt_register
};

struct l_instruction_argument {
  std::string argument;
  int type;
  int number_base = 10;
};

struct l_instruction {
  std::pair<std::string, int> instruction;
  std::vector<l_instruction_argument> arguments;
};

struct l_instruction_argument_def {
  bool is_register, is_address, is_full_register;
};

std::unordered_map<std::string, std::vector<l_instruction_argument_def>> instruction_argument_info = {
  {"mov",    {{ true,  true, false}, { true,  true, false}}},
  {"mov_p",  {{ true,  true, false}, { true,  true, false}}},
  {"mov_pr", {{ true,  true, false}, { true,  true, false}}},
  {"add",    {{ true, false,  true}, { true, false,  true}}},
  {"sub",    {{ true, false,  true}, { true, false,  true}}},
  {"mlt",    {{ true, false,  true}, { true, false,  true}}},
  {"divu",   {{ true, false,  true}, { true, false,  true}}},
  {"divs",   {{ true, false,  true}, { true, false,  true}}},
  {"mod",    {{ true, false,  true}, { true, false,  true}}},
  {"lrg",    {{ true, false,  true}, { true, false,  true}}},
  {"lrg_e",  {{ true, false,  true}, { true, false,  true}}},
  {"sml",    {{ true, false,  true}, { true, false,  true}}},
  {"sml_e",  {{ true, false,  true}, { true, false,  true}}},
  {"eql",    {{ true, false,  true}, { true, false,  true}}},
  {"jmp",    {{false,  true, false}}},
  {"jmpz",   {{ true, false,  true}, {false,  true, false}}},
  {"jmpo",   {{ true, false,  true}, {false,  true, false}}},
  {"jmpof",  {{false,  true, false}}},
  {"jmpuf",  {{false,  true, false}}},
  {"push",   {{ true, false,  true}}},
  {"peek",   {{ true, false,  true}, { true, false,  true}}},
  {"pop",    {{ true, false,  true}}},
  {"jmpc",   {{ true, false,  true}}},
  {"jmpp",   {{ true, false,  true}}},
  {"outu",   {{ true, false,  true}}},
  {"outc",   {{ true, false,  true}}},
  {"in",     {{false,  true, false}}},
  {"out",    {{ true, false,  true}, {false,  true, false}}}
};

const std::vector<std::string> partial_registers = {"rel_a", "rel_b", "rel_c", "rel_d", "rel_e", "rel_f", "rel_g", "rel_h",
                                                    "rem_a", "rem_b", "rem_c", "rem_d", "rem_e", "rem_f", "rem_g", "rem_h"};

const std::vector<std::string> full_registers = {"reg_a", "reg_b", "reg_c", "reg_d", "reg_e", "reg_f", "reg_g", "reg_h"};


std::vector<std::pair<std::string, int>> string_split_delimiter(std::string& input, char delimiter, int line_number) {
  std::vector<std::pair<std::string, int>> output;
  std::string token_buffer = "";

  for(int i = 0; i < input.length(); i++) {
    if(input[i] == delimiter) {
      output.push_back({token_buffer, line_number});
      token_buffer = "";
    }else{
      token_buffer += input[i];
    }
  }

  if(token_buffer != "")
    output.push_back({token_buffer, line_number});

  return output;
}

std::vector<std::pair<std::string, int>> tokenify(std::vector<std::string>& input) {
  std::vector<std::pair<std::string, int>> tokens;
  int line_count = 1;

  for(auto& i : input) {
    i = i.substr(0, i.find("--"));
    std::vector<std::pair<std::string, int>> new_tokens = string_split_delimiter(i, ' ', line_count);
    tokens.insert(tokens.end(), new_tokens.begin(), new_tokens.end());
    line_count++;
  }

  return tokens;
}

bool is_number(std::string number, int base) {
  char* p;
  strtol(number.c_str(), &p, base);
  return *p == 0;
}

std::string clean_number(std::string& x, int& base) {
  if(x.length() > 2) {
    if(x.substr(0, 2) == "0x") {
      if(is_number(x.substr(2), 16)) {
        base = 16;
        return x.substr(2);
      }else{
        return "";
      }
    }else if(x.substr(0, 2) == "0b") {
      if(is_number(x.substr(2), 2)) {
        base = 2;
        return x.substr(2);
      }else{
        return "";
      }
    }else{
      if(is_number(x.substr(2), 10)) {
        base = 10;
        return x.substr(2);
      }else{
        return "";
      }
    }
  }else{
    if(is_number(x, 10)) {
      base = 10;
      return x;
    }else{
      return "";
    }
  }
}

bool is_valid_register_arg(const std::string& x, bool full_register, bool full_register_only) {
  if(full_register_only) {
    if(std::find(full_registers.begin(), full_registers.end(), x) != full_registers.end()) {
      return true;
    }else{
      return false;
    }
  }else{
    if(full_register) {
      if(std::find(partial_registers.begin(), partial_registers.end(), x) != partial_registers.end() || std::find(full_registers.begin(), full_registers.end(), x) != full_registers.end()) {
        
        return true;
      }else{
        return false;
      }
    }else{
      if(std::find(partial_registers.begin(), partial_registers.end(), x) != partial_registers.end()) {
        return true;
      }else{
        return false;
      }
    }
  }
}

bool is_invalid_token_char(const char& x) {
  if((x >= '0' && x <= '9') || (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_' || x == '#') {
    return false;
  }else{
    return true;
  }
}

bool has_invalid_token_char(const std::string& x) {
  for(auto& i : x) {
    if(is_invalid_token_char(i))
      return true;
  }
  return false;
}

void clean_tokens(std::vector<std::pair<std::string, int>>& tokens) {
  for(auto& i : tokens) {
    if(has_invalid_token_char(i.first))
      i.first.erase(std::remove_if(i.first.begin(), i.first.end(), is_invalid_token_char));
  }
}

void print_error(std::pair<std::string, int>& token, std::pair<std::string, int>& instruction_token, int argument_index) {
  std::cout << "Invalid #" << (argument_index+1) << " argument: " << token.first << " on line: " << token.second << ", aborting...\n";
  std::cout << "Argument should be: ";

  if(instruction_argument_info[instruction_token.first][argument_index].is_register)
    std::cout << "A register ";

  if(instruction_argument_info[instruction_token.first][argument_index].is_full_register)
    std::cout << " (full registers only) ";

  if(instruction_argument_info[instruction_token.first][argument_index].is_address)
    std::cout << "/ an address";

  std::cout << '\n';
}

std::vector<l_instruction> parse(std::vector<std::string>& lines) {
  std::vector<std::pair<std::string, int>> tokens = tokenify(lines);
  clean_tokens(tokens);
  std::vector<l_instruction> parsed_instructions;

  for(int i = 0; i < tokens.size(); i++) {
    l_instruction new_instruction;
    if(instruction_argument_info.find(tokens[i].first) == instruction_argument_info.end()) {
      std::cout << "Unknown instruction: " << tokens[i].first << " on line: " << tokens[i].second << ", aborting compilation.\n";
      parsed_instructions.empty();
      return parsed_instructions;
    }else{
      new_instruction.instruction = tokens[i];
      int arg_count = instruction_argument_info[tokens[i].first].size();
      int argument_base = 10;
      for(int j = 0; j < arg_count; j++) {
        if(tokens[i + j + 1].first[0] == '#') {
          if(!instruction_argument_info[tokens[i].first][j].is_address) {
            std::cout << "Error, instruction: " << tokens[i].first << " on line: " << tokens[i].second << " doesn't accept address as #" << (j+1) << " argument. Aborting.\n";
            parsed_instructions.empty();
            return parsed_instructions;
          }else{
            tokens[i + j + 1].first = tokens[i + j + 1].first.substr(1);
            std::string output = clean_number(tokens[i + j + 1].first, argument_base);
            if(output == "") {
              print_error(tokens[i + j + 1], tokens[i], j);
              parsed_instructions.empty();
              return parsed_instructions;
            }else{
              new_instruction.arguments.push_back({output, lt_address, argument_base});
            }
          }
        }else{
          bool added = false;
          if(instruction_argument_info[tokens[i].first][j].is_register) {
            if(instruction_argument_info[tokens[i].first][j].is_full_register) {
              if (is_valid_register_arg(tokens[i + j + 1].first, true, true)) {
                new_instruction.arguments.push_back({tokens[i + j + 1].first, lt_register});
                added = true;
              }
            }else{
              if (is_valid_register_arg(tokens[i + j + 1].first, true, false)) {
                if(tokens[i + j + 1].first[2] == 'l') {
                  new_instruction.arguments.push_back({tokens[i + j + 1].first, lt_register_l});
                }else if(tokens[i + j + 1].first[2] == 'm') {
                  new_instruction.arguments.push_back({tokens[i + j + 1].first, lt_register_m});
                }else{
                  new_instruction.arguments.push_back({tokens[i + j + 1].first, lt_register});
                }
                added = true;
              }
            }
          }

          if(instruction_argument_info[tokens[i].first][j].is_address && !added) {
            std::string output = clean_number(tokens[i + j + 1].first, argument_base);
            if(output == "") {
              print_error(tokens[i + j + 1], tokens[i], j);
              parsed_instructions.empty();
              return parsed_instructions;
            }else{
              new_instruction.arguments.push_back({output, lt_value, argument_base});
            }
          }else{
            if(!added) {
              print_error(tokens[i + j + 1], tokens[i], j);
              parsed_instructions.empty();
              return parsed_instructions;
            }
          }
        }
      }

      parsed_instructions.push_back(new_instruction);
      i += arg_count;
    }
  }

  return parsed_instructions;
}