//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleInventoryItem.h"

#include "SimpleInventoryItem_HealthPotion.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Inventory, Category="Inventory", BlueprintType, Blueprintable, meta=(DisplayName="HealthPotion"))
class SIMPLEINVENTORY_API UItem_HealthPotion : public USimpleInventoryItem {
	GENERATED_BODY()
public:
	UItem_HealthPotion();
	virtual ~UItem_HealthPotion();
protected:
	//virtual void UseItem(USimpleInventoryComponent* const &Inventory) override;
public:
	/// :: BASE ATTRIBUTES ::
	///
	UPROPERTY(Category="Attributes|Potion", EditDefaultsOnly, BlueprintReadWrite, SaveGame, meta=(ClampMin="0.0",ClampMax="9999.9",UIMin="0.0",UIMax="9999.9"))
	float HealPower;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////