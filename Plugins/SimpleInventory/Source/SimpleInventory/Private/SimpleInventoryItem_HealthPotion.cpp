//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleInventoryItem_HealthPotion.h"
#include "SimpleInventory_Shared.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UItem_HealthPotion::~UItem_HealthPotion(){}
UItem_HealthPotion::UItem_HealthPotion() {
	if (HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		SGUID = USavior3::MakeObjectGUID(this,EGuidGeneratorMode::StaticNameToGUID);
	} else {SGUID=USavior3::MakeObjectGUID(this,EGuidGeneratorMode::ResolvedNameToGUID);}
	//
	ItemName = FText::FromString(TEXT("Health Potion"));
	Rarity = ESimpleItemRarity::Common;
	CanConsume = true;
	//
	HealPower = 10.f;
	Durability = 0;
	Capacity = 10;
	Amount = 1;
	Weight = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*void UItem_HealthPotion::UseItem(USimpleInventoryComponent* const &Inventory) {
	// Apply Item usage Formula here...
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////