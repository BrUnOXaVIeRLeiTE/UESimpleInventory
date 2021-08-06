//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SimpleInventory.h"
#include "SimpleInventoryTooltipWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/Border.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Blueprint/IUserObjectListEntry.h"

#include "SimpleInventoryItemWidget.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class SIMPLEINVENTORY_API USimpleInventoryItemWidget : public UUserWidget {
	GENERATED_BODY()
public:
	USimpleInventoryItemWidget(const FObjectInitializer &Initializer);
	virtual ~USimpleInventoryItemWidget();
protected:
	virtual void NativeConstruct() override;
protected:
	UPROPERTY() USimpleInventoryTooltipWidget* ItemTooltip;
protected:
	UPROPERTY(meta=(BindWidget))
	UBorder* Background;
	//
	UPROPERTY(meta=(BindWidget))
	UButton* ItemIcon;
	//
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemCount;
public:
	UPROPERTY(Category="Widget",EditDefaultsOnly)
	TSubclassOf<USimpleInventoryTooltipWidget>TooltipClass;
public:
	UPROPERTY(Category="Inventory", BlueprintSetter=SetItemReference, EditAnywhere, BlueprintReadWrite)
	USimpleInventoryItem* Item;
protected:
	UFUNCTION() void OnItemClicked();
	UFUNCTION() void OnItemRemoved();
protected:
	UFUNCTION(Category="Inventory", BlueprintNativeEvent)
	void OnItemAdded(const int32 Quantity);
	//
	UFUNCTION(Category="Inventory", BlueprintNativeEvent)
	void OnItemUsed(const int32 Quantity, const bool Liquidated, const bool Expired);
public:
	UFUNCTION(Category="Inventory", BlueprintCallable)
	void Reconstruct();
public:
	UFUNCTION(BlueprintSetter)
	void SetItemReference(USimpleInventoryItem* NewItem);
public:
	FORCEINLINE bool operator == (USimpleInventoryItemWidget &Other) const {
		return ((*Item)==(*Other.Item));
	}///
	//
	FORCEINLINE bool operator == (const USimpleInventoryItemWidget &Other) const {
		return ((*Item)==(*Other.Item));
	}///
	//
	FORCEINLINE USimpleInventoryItemWidget &operator = (USimpleInventoryItemWidget* Other) {
		Item = Other->Item; Reconstruct();
	return *this;}
	//
	FORCEINLINE USimpleInventoryItemWidget &operator = (USimpleInventoryItemWidget &Other) {
		Item = Other.Item; Reconstruct();
	return *this;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////