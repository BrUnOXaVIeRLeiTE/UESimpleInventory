//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleInventoryTypes.h"

#include "SimpleInventoryItem.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSimpleItemAdd, const int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSimpleItemUse, const int32, Quantity, const bool, Liquidated, const bool, Expired);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Inventory, Category="Inventory", Abstract, HideDropdown, BlueprintType)
class SIMPLEINVENTORY_API USimpleInventoryItem : public UAutoInstanced {
	GENERATED_BODY()
public:
	friend class USimpleInventoryWidget;
	friend class USimpleInvetoryInterface;
	friend class USimpleInventoryComponent;
	friend class USimpleInventoryItemRecipe;
	friend class USimpleInventoryItemWidget;
protected:
	//virtual void UseItem(USimpleInventoryComponent* const &Inventory) PURE_VIRTUAL(,);
public:
	USimpleInventoryItem();
	virtual ~USimpleInventoryItem();
public:
	virtual void PostLoad() override;
	virtual UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;
	virtual void OnLoaded_Implementation(const FSlotMeta &MetaData) override;
public:
	void SetWorld(UWorld* InWorld);
protected:
	UPROPERTY() UWorld* World;
protected:
	UPROPERTY(Category="Inventory", BlueprintReadOnly)
	USimpleInventoryComponent* ParentInventory;
public:
	/// :: IDENTITY ATTRIBUTES ::
	///
	UPROPERTY(Category="ID", EditDefaultsOnly, BlueprintReadOnly)
	USimpleInventoryItemRecipe* Recipe;
	//
	UPROPERTY(Category="ID", EditDefaultsOnly, BlueprintReadOnly)
	FGuid SGUID;
	//
	UPROPERTY(Category="ID", EditDefaultsOnly, BlueprintReadWrite, SaveGame)
	FText ItemName;
public:
	/// :: BASE ATTRIBUTES ::
	///
	/** Item general value and reputation. */
	UPROPERTY(Category="Attributes", EditDefaultsOnly, BlueprintReadOnly)
	ESimpleItemRarity Rarity;
	//
	/** Item stacking limit. */
	UPROPERTY(Category="Attributes|Base", EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="9999",UIMin="0",UIMax="9999"))
	int32 Capacity;
	//
	/** Inventory weight penalty. */
	UPROPERTY(Category="Attributes|Base", EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0",ClampMax="100",UIMin="0",UIMax="100"))
	int32 Weight;
public:
	/// :: PROGRESSIVE ATTRIBUTES ::
	///
	/** Item crafting/enchanting level. */
	UPROPERTY(Category="Attributes|Progressive", EditDefaultsOnly, BlueprintReadWrite, SaveGame, meta=(ClampMin="1",ClampMax="999",UIMin="0",UIMax="999"))
	int32 Level;
	//
	/** Item stack size. */
	UPROPERTY(Category="Attributes|Progressive", EditDefaultsOnly, BlueprintReadWrite, SaveGame, meta=(ClampMin="0",ClampMax="9999",UIMin="0",UIMax="9999"))
	int32 Amount;
	//
	/** Item wear by use. ZERO = expired/unusable. */
	UPROPERTY(Category="Attributes|Progressive", EditDefaultsOnly, BlueprintReadWrite, SaveGame, meta=(ClampMin="1",ClampMax="9999",UIMin="1",UIMax="9999"))
	int32 Durability;
	//
	UPROPERTY(Category="Attributes|Progressive", EditDefaultsOnly, BlueprintReadWrite, SaveGame)
	bool CanConsume;
	//
	UPROPERTY(Category="Attributes|Progressive", EditDefaultsOnly, BlueprintReadWrite, SaveGame)
	bool CanExpire;
protected:
	/// :: EVENTS ::
	///
	UPROPERTY(BlueprintAssignable)
	FSimpleItemAdd OnItemAdd;
	//
	UPROPERTY(BlueprintAssignable)
	FSimpleItemUse OnItemUse;
	//
	/*UFUNCTION(Category="Inventory", BlueprintImplementableEvent, meta=(Keywords="Inventory,Event"))
	void OnItemAdded(USimpleInventoryComponent* const &Inventory, const int32 Quantity);
	//
	UFUNCTION(Category="Inventory", BlueprintImplementableEvent, meta=(Keywords="Inventory,Event"))
	void OnItemUsed(USimpleInventoryComponent* const &Inventory, const int32 Quantity, const bool Liquidated, const bool Expired);*/
public:
	/// :: API ::
	///
	UFUNCTION(Category="Inventory", BlueprintPure)
	bool IsStackable() const;
	//
	UFUNCTION(Category="Inventory", BlueprintPure)
	bool HasCapacity(int32 IncomingLoad) const;
	//
	UFUNCTION(Category="Inventory", BlueprintPure)
	bool HasExpired() const;
	//
	UFUNCTION(Category="Inventory", BlueprintPure)
	int32 GetFullWeight() const;
public:
	FORCEINLINE bool operator == (USimpleInventoryItem &Other) const {
		return (
			(SGUID==Other.SGUID) &&
			(Recipe==Other.Recipe) &&
			(Rarity==Other.Rarity) &&
			(Weight==Other.Weight) &&
			(Amount==Other.Amount) &&
			(Capacity==Other.Capacity)
		);//
	}///
	//
	FORCEINLINE bool operator == (const USimpleInventoryItem &Other) const {
		return (
			(SGUID==Other.SGUID) &&
			(Recipe==Other.Recipe) &&
			(Rarity==Other.Rarity) &&
			(Weight==Other.Weight) &&
			(Amount==Other.Amount) &&
			(Capacity==Other.Capacity)
		);//
	}///
	//
	FORCEINLINE USimpleInventoryItem &operator = (USimpleInventoryItem* Other) {
		World = Other->World;
		SGUID = Other->SGUID;
		Level = Other->Level;
		Recipe = Other->Recipe;
		Rarity = Other->Rarity;
		Weight = Other->Weight;
		Amount = Other->Amount;
		Capacity = Other->Capacity;
		ItemName = Other->ItemName;
		CanExpire = Other->CanExpire;
		Durability = Other->Durability;
		CanConsume = Other->CanConsume;
	return *this;}
	//
	FORCEINLINE USimpleInventoryItem &operator = (USimpleInventoryItem &Other) {
		World = Other.World;
		SGUID = Other.SGUID;
		Level = Other.Level;
		Recipe = Other.Recipe;
		Rarity = Other.Rarity;
		Weight = Other.Weight;
		Amount = Other.Amount;
		Capacity = Other.Capacity;
		ItemName = Other.ItemName;
		CanExpire = Other.CanExpire;
		Durability = Other.Durability;
		CanConsume = Other.CanConsume;
	return *this;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////