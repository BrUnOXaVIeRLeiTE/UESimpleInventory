//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleInventoryTypes.h"

#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"

#include "SimpleInventoryRecipe.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Inventory, Category="Inventory", BlueprintType)
class SIMPLEINVENTORY_API USimpleInventoryItemRecipe : public UDataAsset {
	GENERATED_BODY()
public:
	friend class USimpleInventoryItem;
	friend class USimpleInvetoryInterface;
	friend class USimpleInventoryComponent;
public:
	USimpleInventoryItemRecipe();
	virtual ~USimpleInventoryItemRecipe();
public:
	/// :: IDENTITY ::
	///
	UPROPERTY(Category="ID", EditDefaultsOnly, BlueprintReadOnly)
	FGuid TypeID;
public:
	/// :: VISUAL RESOURCES ::
	///
	/* UI large item portrait. */
	UPROPERTY(Category="UI", EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Banner;
	//
	/** Armor/Weapon body visuals, for pieces of modular sets. */
	UPROPERTY(Category="UI", EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* Mesh;
	//
	/* UI item interaction style. */
	UPROPERTY(Category="UI|Widget", EditDefaultsOnly, BlueprintReadOnly)
	FButtonStyle Style;
public:
	/// :: ATTRIBUTES ::
	///
	/** Recipe general value and reputation. */
	UPROPERTY(Category="Attributes", EditDefaultsOnly, BlueprintReadOnly)
	ESimpleItemRarity Rarity;
	//
	/** Iron, Bronze, Gold, Platinum, etc. */
	UPROPERTY(Category="Attributes", EditDefaultsOnly, BlueprintReadOnly)
	FText Domain;
	//
	/** Item type/class/ description, lore. */
	UPROPERTY(Category="Attributes", EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine=true))
	FText Description;
	//
	/** Warrior, Mage, Assassin, etc. */
	UPROPERTY(Category="Attributes", EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor>Specialization;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////