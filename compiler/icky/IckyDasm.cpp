#include "Icky.h"
#include "IckyRuntime.h"
#include "IckyKeywords.h"

void IckyAsm::dasm(IckyRuntimeData* ird) {
	int asm_size = ird->_asm_ops.size();
	for(int i = 0; i < asm_size; i++) {
		uint8_t opcode = ird->_asm_ops[i];

		switch(opcode) {
			case IckyOpCode::printString:
            case IckyOpCode::printDouble:
            case IckyOpCode::printInteger:
            case IckyOpCode::printCharacter:
            case IckyOpCode::unconditionalJump:
            case IckyOpCode::loadDoubleVar:
            case IckyOpCode::wsPushDouble:
            case IckyOpCode::wsReverse:
            case IckyOpCode::wsClear:
            case IckyOpCode::wsSaveDouble:
            case IckyOpCode::wsAdd:
            case IckyOpCode::wsSubtract:
            case IckyOpCode::wsMultiply:
            case IckyOpCode::wsDivide:
            case IckyOpCode::wsPower:
            case IckyOpCode::bGreaterThan:
            case IckyOpCode::bLessThan:
            case IckyOpCode::bGreaterThanEq:
            case IckyOpCode::bLessThanEq:
            case IckyOpCode::bEq:
            case IckyOpCode::sysRuntime:
            case IckyOpCode::sysWait:
            case IckyOpCode::sysQuit:
            default:
                break;
		}

	}
}
