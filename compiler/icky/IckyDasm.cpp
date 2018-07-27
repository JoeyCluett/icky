#include "Icky.h"
#include "IckyRuntime.h"
#include "IckyKeywords.h"

#include <vector>
#include <algorithm>
#include <iostream>

#define HEX_NUM_SIZE 4

static char lookUpHexChar(int i) {
    const char charLUT[] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };

    if(i >= 0 && i <= 15) {
        return charLUT[i];
    }

    return 'X';
}

static void printHex(int input, int hex_str_size = HEX_NUM_SIZE) {
    std::vector<char> _tmp;
    _tmp.push_back('\0');
    while(input) {
        _tmp.push_back(lookUpHexChar(input & 0x0F));
        input >>= 4;
    }

    while(_tmp.size() < (unsigned int)(hex_str_size+1))
        _tmp.push_back('0');
    std::reverse(std::begin(_tmp), std::end(_tmp));
    std::cout << &_tmp[0];
}

static void printInstrName(uint8_t opcode) {
    switch(opcode) {
        case IckyOpCode::printString:
            std::cout << "PrintString   "; break;
        case IckyOpCode::printDouble:
            std::cout << "PrintDouble   "; break;
        case IckyOpCode::printInteger:
            std::cout << "PrintInteger  "; break;
        case IckyOpCode::printCharacter:
            std::cout << "PrintChar     "; break;
        case IckyOpCode::unconditionalJump:
            std::cout << "Goto          "; break;
        case IckyOpCode::loadDoubleVar:
            std::cout << "LoadDoubleVar "; break;
        case IckyOpCode::wsPushDouble:
            std::cout << "wsPushDouble  "; break;
        case IckyOpCode::wsReverse:
            std::cout << "wsReverse     "; break;
        case IckyOpCode::wsClear:
            std::cout << "wsClear       "; break;
        case IckyOpCode::wsSaveDouble:
            std::cout << "wsSaveDouble  "; break;
        case IckyOpCode::wsAdd:
            std::cout << "wsAdd         "; break;
        case IckyOpCode::wsSubtract:
            std::cout << "wsSubtract    "; break;
        case IckyOpCode::wsMultiply:
            std::cout << "wsMultiply    "; break;
        case IckyOpCode::wsDivide:
            std::cout << "wsDivide      "; break;
        case IckyOpCode::wsPower:
            std::cout << "wsPower       "; break;
        case IckyOpCode::bGreaterThan:
            std::cout << "Branch >      "; break;
        case IckyOpCode::bLessThan:
            std::cout << "Branch <      "; break;
        case IckyOpCode::bGreaterThanEq:
            std::cout << "Branch >=     "; break;
        case IckyOpCode::bLessThanEq:
            std::cout << "Branch <=     "; break;
        case IckyOpCode::bEq:
            std::cout << "Branch ==     "; break;
        case IckyOpCode::sysRuntime:
            std::cout << "SYS Runtime   "; break;
        case IckyOpCode::sysWait:
            std::cout << "SYS Wait      "; break;
        case IckyOpCode::sysQuit:
            std::cout << "SYS Quit      "; break;
        default:
            throw new IckyException("printInstrName: Unknown opcode");
            break;
    }
}

// reverse of map lookup
static void locDouble(IckyRuntimeData* ird, int index) {
    auto BEGIN = ird->_std_var_double_index.begin();
    auto END = ird->_std_var_double_index.end();
    for(; BEGIN != END; BEGIN++) {
        //std::cout << BEGIN->first << "   " << BEGIN->second << std::endl;
        if(BEGIN->second == index) {
            std::cout << BEGIN->first;
            return;
        }
    }
    std::cout << "literal(" << ird->_std_var_double[index] << ')';
}

void IckyAsm::dasm(IckyRuntimeData* ird) {

    std::cout << "ADDR OpC\n---- ---\n";

	int asm_size = ird->_asm_ops.size();
	for(int i = 0; i < asm_size;) {
        //std::cout << "Index: " << i << std::endl;
		uint8_t opcode = ird->_asm_ops.at(i);

        printHex((int)i);
        std::cout << ' ';
        printHex((int)opcode, 3);
        std::cout << ' ';
        printInstrName(opcode);

        int tmp = 0; // used for calculating indices

		switch(opcode) {
			case IckyOpCode::printString:
                std::cout << "["
                        << *(int*)&ird->_asm_ops[i+1] << ":" 
                        << &ird->_std_var_string[*(int*)&ird->_asm_ops[i+1]] << "]";
                i += 5; 
                break;
            case IckyOpCode::printDouble:
                std::cout << "[";
                tmp = *(int*)&ird->_asm_ops[i+1];
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << ":";
                locDouble(ird, tmp);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::printInteger:
                //std::cout << "["
                //        << *(int*)&ird->_asm_ops[i+1] << ":"
                //        << ird->_std_var_integer[*(int*)&ird->_asm_ops[i+1]] << "]";
                throw new IckyException("IckyOpCode::printInteger, this opcode does not exist");
                i += 5; 
                break;
            case IckyOpCode::printCharacter:
                std::cout << "["
                        << (int)ird->_asm_ops[i+1] << "]";
                i += 2; 
                break;
            case IckyOpCode::unconditionalJump:
                std::cout << "Dest: ";
                printHex(*(int*)&ird->_asm_ops[i+1], 8);
                i += 5; 
                break;
            case IckyOpCode::loadDoubleVar:
                std::cout << "[Src:";
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << "|Dest:";
                printHex(*(int*)&ird->_asm_ops[i+5]);
                std::cout << "]";
                i += 9; 
                break;
            case IckyOpCode::wsPushDouble:
                std::cout << "[";
                tmp = *(int*)&ird->_asm_ops[i+1];
                printHex(tmp);
                std::cout << ":";
                locDouble(ird, tmp);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::wsReverse:
                i++; break;
            case IckyOpCode::wsClear:
                i++; break;
            case IckyOpCode::wsSaveDouble:
                std::cout << "[";
                tmp = *(int*)&ird->_asm_ops[i+1];
                printHex(tmp);
                std::cout << ":";
                locDouble(ird, tmp);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::wsAdd:
                i++; break;
            case IckyOpCode::wsSubtract:
                i++; break;
            case IckyOpCode::wsMultiply:
                i++; break;
            case IckyOpCode::wsDivide:
                i++; break;
            case IckyOpCode::wsPower:
                i++; break;
            case IckyOpCode::bGreaterThan:
                std::cout << "[";
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::bLessThan:
                std::cout << "[";
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::bGreaterThanEq:
                std::cout << "[";
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::bLessThanEq:
                std::cout << "[";
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::bEq:
                std::cout << "[";
                printHex(*(int*)&ird->_asm_ops[i+1]);
                std::cout << "]";
                i += 5; 
                break;
            case IckyOpCode::sysRuntime:
                i++; break;
            case IckyOpCode::sysWait:
                i++; break;
            case IckyOpCode::sysQuit:
                i++; break;
            default: 
                break;
		}

        std::cout << std::endl;;
	}
}
