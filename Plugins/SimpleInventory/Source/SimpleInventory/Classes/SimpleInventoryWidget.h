//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleInventory.h"
#include "SimpleInventoryItemWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Components/WrapBox.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "SimpleInventoryWidget.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class SIMPLEINVENTORY_API USimpleInventoryWidget : public UUserWidget {
	GENERATED_BODY()
public:
	USimpleInventoryWidget(const FObjectInitializer &Initializer);
	virtual ~USimpleInventoryWidget();
protected:
	virtual void NativeConstruct() override;
protected:
	void CreateItemWidget(USimpleInventoryItem* Item);
	void RefreshItemWidget(USimpleInventoryItem* Item);
protected:
	UPROPERTY(meta=(BindWidget))
	UWrapBox* InventoryView;
public:
	UPROPERTY(Category="Widget",EditDefaultsOnly)
	TSubclassOf<USimpleInventoryItemWidget>ItemWidgetClass;
public:
	UPROPERTY(Category="Inventory", BlueprintSetter=SetInventoryReference, EditAnywhere, BlueprintReadWrite)
	USimpleInventoryComponent* Inventory;
protected:
	UFUNCTION() void OnSlotLimitUpdated(int32 NewLimit);
protected:
	UFUNCTION(Category="Inventory", BlueprintNativeEvent)
	void OnItemAdded(USimpleInventoryItem* Item, const int32 Quantity);
	//
	UFUNCTION(Category="Inventory", BlueprintNativeEvent)
	void OnItemUsed(USimpleInventoryItem* Item, const int32 Quantity, const bool Liquidated, const bool Expired);
public:
	UFUNCTION(Category="Inventory", BlueprintCallable)
	void Reconstruct();
public:
	UFUNCTION(BlueprintSetter)
	void SetInventoryReference(USimpleInventoryComponent* NewInventory);
public:
	FORCEINLINE bool operator == (USimpleInventoryWidget &Other) const {
		return (Inventory==Other.Inventory);
	}///
	//
	FORCEINLINE bool operator == (const USimpleInventoryWidget &Other) const {
		return (Inventory==Other.Inventory);
	}///
	//
	FORCEINLINE USimpleInventoryWidget &operator = (USimpleInventoryWidget* Other) {
		Inventory = Other->Inventory; Reconstruct();
	return *this;}
	//
	FORCEINLINE USimpleInventoryWidget &operator = (USimpleInventoryWidget &Other) {
		Inventory = Other.Inventory; Reconstruct();
	return *this;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////