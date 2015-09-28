#pragma once

#include "Types.h"
#include "Level.h"

enum ReferenceType {
	ref_invalid = -1,
	ref_self,
	ref_entity,
	ref_level
};

enum OPCODE {
	NOP,
	FIND
};

struct Reference {
	ReferenceType Type;
	
	union {
		GameObject* GameObj;
		Level* Lvl;
	};
};

class Interpreter {
private:
	Level* level;
	int pc = 0;
	byte* script;
	Reference self;
public:
	Interpreter(GameObject* Owner);
	Reference Find(ReferenceType RefType, size_t ID);
	void Run();
};