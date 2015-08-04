#include "Interpreter.h"

void Interpreter::Run() {

	OPCODE O;

	while (O == script[pc]) {

		switch (O) {

		//mnemonic: NOP
		//opcode: 0x00
		//description: Do nothing

		case NOP:
			pc++;
			break;

		//mnemonic: FIND
		//opcode: 0x01
		//description: Get a reference

		case FIND:
			byte reg = script[pc + 1];
			break;

		}
	}
}
