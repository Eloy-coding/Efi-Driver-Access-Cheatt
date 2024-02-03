#include "Netvar.h"

class CEntity {
public:
	NETVAR(CurrentWeapon, "DiscoveryCharacter->InventoryComponent->CurrentEquippedSlot", int)
	NETVAR(Health, "DiscoveryCharacterBase->HealthComponent->GetHealth", float)
};