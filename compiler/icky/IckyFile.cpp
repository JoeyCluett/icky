#ifndef __JJC__ICKY__FILE__H__
#define __JJC__ICKY__FILE__H__

#include "Icky.h"
#include "IckyRuntime.h"

#include <string>
#include <fstream>
#include <vector>
#include <map>

struct string_int_map_entry {
    std::string first;
    int second;
};

static void getMapEntry(std::ifstream& is, string_int_map_entry& sime) {
    std::vector<char> first;
    int second;

    char input_char = '\0';
    do {
        is.read(&input_char, 1);
        first.push_back(input_char); // assemble a null-terminated string
    } while(input_char != '\0');

    is.read((char*)&second, 4);

    sime.first = &first[0];
    sime.second = second;
}

static void writeMapToFile(std::ofstream& os, std::map<std::string, int>& is_map) {
    int map_size = is_map.size();
    os.write((char*)&map_size, 4);

    for(auto& m : is_map) {
        os.write((char*)m.first.c_str(), m.first.size() + 1); // +1 includes null-terminator
        os.write((char*)&m.second, 4); // corresponding integer
    }
}

static void readMapFromFile(std::ifstream& is, std::map<std::string, int>& is_map) {
    int map_size = 0;
    is.read((char*)&map_size, 4);

    string_int_map_entry sime;

    for(int i = 0; i < map_size; i++) {
        getMapEntry(is, sime);
        is_map[sime.first] = sime.second;
    }
}

void IckyAsm::saveRuntimeData(IckyRuntimeData* ird, std::string filename) {
    std::ofstream outfile(filename, std::ios::out | std::ios::binary);
    if(!outfile) {
        throw new IckyException(
                std::string("IckyAsm::saveRuntimeData: Could not open file: ") + filename);
    }

    // write the opcodes to the binary file
    int opcode_size = (int)ird->_asm_ops.size();
    outfile.write((char*)&opcode_size, 4);
    outfile.write((char*)&ird->_asm_ops[0], opcode_size);

    // write the ridiculous number of string/int maps to the binary file
    writeMapToFile(outfile, ird->_std_var_double_index);
    writeMapToFile(outfile, ird->_std_var_string_index);
    writeMapToFile(outfile, ird->_jump_table_index);

    // write raw data vectors to the binary file
    int vec_size = 0;

    vec_size = ird->_std_var_double.size();
    outfile.write((char*)&vec_size, 4);
    outfile.write((char*)&ird->_std_var_double[0], ird->_std_var_double.size()*8);

    vec_size = ird->_std_var_string.size();
    outfile.write((char*)&vec_size, 4);
    outfile.write((char*)&ird->_std_var_string[0], ird->_std_var_string.size());

    vec_size = ird->_jump_table.size();
    outfile.write((char*)&vec_size, 4);
    outfile.write((char*)&ird->_jump_table[0], ird->_jump_table.size()*4);
}

void IckyAsm::loadRuntimeData(IckyRuntimeData* ird, std::string filename) {
    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if(!infile) {
        throw new IckyException(
                std::string("IckyAsm::loadRuntimeData: Could not open file: ") + filename);
    }

    // load the opcodes from the binary file
    int opcode_size = 0;
    infile.read((char*)&opcode_size, 4);
    ird->_asm_ops.resize(opcode_size);
    infile.read((char*)&ird->_asm_ops[0], opcode_size);

    // read the various maps into the runtime
    readMapFromFile(infile, ird->_std_var_double_index);
    readMapFromFile(infile, ird->_std_var_string_index);
    readMapFromFile(infile, ird->_jump_table_index);

    // read data vector into the runtime
    int vec_size = 0;

    infile.read((char*)&vec_size, 4);
    ird->_std_var_double.resize(vec_size);
    infile.read((char*)&ird->_std_var_double[0], vec_size*8);

    infile.read((char*)&vec_size, 4);
    ird->_std_var_string.resize(vec_size);
    infile.read((char*)&ird->_std_var_string[0], vec_size);

    infile.read((char*)&vec_size, 4);
    ird->_jump_table.resize(vec_size);
    infile.read((char*)&ird->_jump_table[0], vec_size);
}

#endif // __JJC__ICKY__FILE__H__
// i never finish anyth