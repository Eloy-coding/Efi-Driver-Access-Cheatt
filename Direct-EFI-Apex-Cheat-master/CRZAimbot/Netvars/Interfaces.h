#pragma once
#include "Entity.h"
#include "Datatable.h"

class EntityList {
private:
	virtual void Unused_0() = 0;
	virtual void Unused_1() = 0;
	virtual void Unused_2() = 0;

public:
	virtual CEntity* GetClientEntity(uintptr_t index) = 0;
};

class Client {
private :
	virtual void Unused_0() = 0;
	virtual void Unused_1() = 0;
	virtual void Unused_2() = 0;
	virtual void Unused_3() = 0;
	virtual void Unused_4() = 0;
	virtual void Unused_5() = 0;
	virtual void Unused_6() = 0;
	virtual void Unused_7() = 0;

public :
	virtual ClientClass* GetAllClasses(void) = 0;
};

inline EntityList* entityList = nullptr;
inline Client* client = nullptr;