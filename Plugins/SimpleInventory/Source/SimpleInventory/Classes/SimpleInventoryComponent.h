//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleInventoryTypes.h"

#include "Runtime/Engine/Classes/Components/ActorComponent.h"

#include "SimpleInventoryComponent.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FInventorySortByTypePredicate;
struct FInventorySortByStackPredicate;
struct FInventorySortByRarityPredicate;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventorySlotLimitChange, const int32, NewSlotLimit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryWeightLimitChange, const int32, NewWeightLimit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemAdd, USimpleInventoryItem*, NewItem, const int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInventoryItemUse, USimpleInventoryItem*, OnItem, const int32, Quantity, const bool, Liquidated, const bool, Expired);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Inventory, Category="Inventory", BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), hideCategories=("Collision","Cooking","Sockets","Variable","ComponentReplication"))
class SIMPLEINVENTORY_API USimpleInventoryComponent : public UActorComponent, public ISAVIOR_Serializable {
	GENERATED_BODY()
public:
	friend class USimpleInventoryItem;
	friend class USimpleInventoryWidget;
	friend class USimpleInvetoryInterface;
	friend class USimpleInventoryItemRecipe;
	friend class USimpleInventoryItemWidget;
public:
	USimpleInventoryComponent();
	virtual ~USimpleInventoryComponent();
public:
	virtual void PostInitProperties() override;
	virtual void OnLoaded_Implementation(const FSlotMeta &MetaData) override;
protected:
	/// :: INVENTORY ::
	///
	UPROPERTY(Category="Inventory", VisibleAnywhere, SaveGame)
	TArray<USimpleInventoryItem*>Inventory;
protected:
	bool IsOverWeightLimit() const;
protected:
	void InsertItem(USimpleInventoryItem* NewItem, const int32 Quantity, EInventoryAction &Result);
	void ConsumeItem(USimpleInventoryItem* OnItem, const int32 Quantity, EInventoryAction &Result);
	void InsertItemAtIndex(USimpleInventoryItem* AtItem, const int32 Index, const int32 Quantity, EInventoryAction &Result);
protected:
	//bool MoveItem(USimpleInventoryItem* Item, const int32 ToIndex)
	//bool TransferItem(USimpleInventoryItem* Item, USimpleInventoryComponent* ToInventory)
public:
	/// :: IDENTITY ::
	///
	UPROPERTY(Category="ID", EditDefaultsOnly, BlueprintReadOnly)
	FGuid SGUID;
public:
	/// :: ATTRIBUTES ::
	///
	/** Inventory maximum load. */
	UPROPERTY(Category="Attributes", BlueprintSetter=SetWeightLimit, EditDefaultsOnly, BlueprintReadWrite, SaveGame, meta=(ClampMin="10",ClampMax="10000",UIMin="10",UIMax="10000"))
	int32 WeightLimit;
	//
	/** Inventory maximum load. */
	UPROPERTY(Category="Attributes", BlueprintSetter=SetSlotLimit, EditDefaultsOnly, BlueprintReadWrite, SaveGame, meta=(ClampMin="5",ClampMax="100",UIMin="5",UIMax="100"))
	int32 SlotLimit;
	//
	/** Inventory current load. */
	UPROPERTY(Category="Attributes", BlueprintReadOnly)
	mutable int32 TotalWeight;
protected:
	/// :: EVENTS ::
	///
	UPROPERTY(BlueprintAssignable)
	FInventoryItemAdd OnItemAdd;
	//
	UPROPERTY(BlueprintAssignable)
	FInventoryItemUse OnItemUse;
	//
	UPROPERTY(BlueprintAssignable)
	FInventoryWeightLimitChange OnWeightLimitChanged;
	//
	UPROPERTY(BlueprintAssignable)
	FInventorySlotLimitChange OnSlotLimitChanged;
	//
	UFUNCTION(Category="Inventory", BlueprintImplementableEvent, meta=(Keywords="Inventory,Event"))
	void OnWeightLimitSurpassed(USimpleInventoryItem* OnItem, const int32 InventoryWeight, const int32 ItemWeight);
	//
	UFUNCTION(Category="Inventory", BlueprintImplementableEvent, meta=(Keywords="Inventory,Event"))
	void OnSlotLimitSurpassed(USimpleInventoryItem* OnItem, const int32 InventoryLimit);
public:
	/// :: API ::
	///
	/** Add new item/s, of target class type, to inventory. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Add Item (by type)",ExpandEnumAsExecs="Result"))
	USimpleInventoryItem* AddItem(const TSubclassOf<USimpleInventoryItem>ItemType, int32 Quantity, EInventoryAction &Result);
	//
	/** Add new item/s, of target class type, to inventory. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Add Item (by index)",ExpandEnumAsExecs="Result"))
	USimpleInventoryItem* AddItemAtIndex(const TSubclassOf<USimpleInventoryItem>ItemType, int32 SlotIndex, int32 Quantity, EInventoryAction &Result);
	//
	//
	/** Try to consume item of target type from inventory. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Use Item (by type)",ExpandEnumAsExecs="Result"))
	void UseItem_ByType(const TSubclassOf<USimpleInventoryItem>ItemType, const int32 Quantity, EInventoryAction &Result);
	//
	/** Try to consume target item from inventory. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Use Item (by index)",ExpandEnumAsExecs="Result"))
	void UseItem_ByIndex(const int32 SlotIndex, const int32 Quantity, EInventoryAction &Result);
	//
	/** Try to consume target item from inventory. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Use Item (by id)",ExpandEnumAsExecs="Result"))
	void UseItem_ByGuid(const FGuid &GUID, const int32 Quantity, EInventoryAction &Result);
	//
	/** Try to consume target item from inventory. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Use Item (by ref)",ExpandEnumAsExecs="Result"))
	void UseItem_ByRef(USimpleInventoryItem* const &Item, const int32 Quantity, EInventoryAction &Result);
	//
	//
	UFUNCTION(Category="Inventory", BlueprintPure, meta=(DisplayName="Find Item (by type)"))
	USimpleInventoryItem* FindItem_ByType(const TSubclassOf<USimpleInventoryItem>ItemType, const bool FreeSpaceOnly=false) const;
	//
	UFUNCTION(Category="Inventory", BlueprintPure, meta=(DisplayName="Find Item (by index)"))
	USimpleInventoryItem* FindItem_ByIndex(const int32 SlotIndex) const;
	//
	UFUNCTION(Category="Inventory", BlueprintPure, meta=(DisplayName="Find Item (by id)"))
	USimpleInventoryItem* FindItem_ByGuid(const FGuid &GUID) const;
	//
	UFUNCTION(Category="Inventory", BlueprintPure, meta=(DisplayName="Find Item (by name)"))
	USimpleInventoryItem* FindItem_ByName(const FText &Name) const;
	//
	//
	/** Sort inventory by type. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Sort Inventory (by type)"))
	void SortInventory_ByType(const EInventorySorting SortMode);
	//
	/** Sort inventory by item amount. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Sort Inventory (by stack)"))
	void SortInventory_ByStack(const EInventorySorting SortMode);
	//
	/** Sort inventory by item amount. */
	UFUNCTION(Category="Inventory", BlueprintCallable, meta=(DisplayName="Sort Inventory (by rarity)"))
	void SortInventory_ByRarity(const EInventorySorting SortMode);
public:
	/// :: UTILITY ::
	///
	/** Try to find a free space in inventory.
	Return INDEX_NONE (-1), if inventory is full. */
	UFUNCTION(Category="Inventory", BlueprintPure)
	int32 GetFirstFreeSlotIndex() const;
	//
	/** Try to find a free space in inventory.
	Return INDEX_NONE (-1), if inventory is full. */
	UFUNCTION(Category="Inventory", BlueprintPure)
	int32 CalculateInventoryWeight() const;
	//
	/** Check if inventory is full. */
	UFUNCTION(Category="Inventory", BlueprintPure)
	bool IsInventoryFull() const;
public:
	/// :: INTERNAL ::
	///
	UFUNCTION(BlueprintSetter)
	void SetWeightLimit(int32 NewLimit);
	//
	UFUNCTION(BlueprintSetter)
	void SetSlotLimit(int32 NewLimit);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////