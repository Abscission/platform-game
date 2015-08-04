#pragma once

#include "Types.h"

enum ReferenceType {
	ref_self,
	ref_entity,
	ref_level
};

enum OPCODE {
	NOP,
	FIND
};

class Interpreter {
private:
	int pc = 0;
	byte* script;
public:
	void Run();
};