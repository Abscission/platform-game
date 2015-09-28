
#include "Interpreter.h"

Reference Interpreter::Find(ReferenceType RefType, size_t ID) {
	if (RefType == ref_self) return self;

	Reference ref;

	switch (RefType) {
	case ref_entity:
		ref.Type = ref_entity;
		ref.GameObj = level->Entities.Last->Item;
		return ref;
	case ref_level:
		ref.Type = ref_level;
		ref.Lvl = level;
		return ref;
	}

	return{ ref_invalid, nullptr };
}

Interpreter::Interpreter(GameObject* Owner)
{
	self = { self };
	self.GameObj = Owner;
}
