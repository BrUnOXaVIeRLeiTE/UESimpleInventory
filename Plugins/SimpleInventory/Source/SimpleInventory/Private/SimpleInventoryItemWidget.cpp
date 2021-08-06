//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleInventoryItemWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USimpleInventoryItemWidget::~USimpleInventoryItemWidget(){}
USimpleInventoryItemWidget::USimpleInventoryItemWidget(const FObjectInitializer &Initializer) : Super(Initializer) {
	TooltipClass = USimpleInventoryTooltipWidget::StaticClass();
	bIsFocusable = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryItemWidget::NativeConstruct() {
	Super::NativeConstruct();
	//
	if (ItemIcon) {
		ItemIcon->OnClicked.AddUniqueDynamic(this,&USimpleInventoryItemWidget::OnItemClicked);
	}///
}

void USimpleInventoryItemWidget::Reconstruct() {
	if (Item==nullptr||Item->IsPendingKill()) {OnItemRemoved(); return;}
	else if (Item->Amount<=0) {OnItemRemoved(); return;}
	//
	if (Item->Recipe) {
		if (ItemIcon) {ItemIcon->SetStyle(Item->Recipe->Style);}
	}///
	//
	if (ItemCount) {ItemCount->SetText(FText::AsNumber(Item->Amount));}
	//
	if (ItemIcon) {
		if (ItemTooltip==nullptr) {
			if (TooltipClass.Get()) {
				ItemTooltip = CreateWidget<USimpleInventoryTooltipWidget>(this,TooltipClass);
				ItemIcon->SetToolTip(ItemTooltip); ItemTooltip->SetItemReference(Item);
		}} else {ItemTooltip->SetItemReference(Item);}
	}///
	//
	if (Background) {
		switch(Item->Rarity) {
			case ESimpleItemRarity::Rare: {Background->SetContentColorAndOpacity(FLinearColor(10,10,225));} break;
			case ESimpleItemRarity::Epic: {Background->SetContentColorAndOpacity(FLinearColor(225,10,225));} break;
			case ESimpleItemRarity::Common: {Background->SetContentColorAndOpacity(FLinearColor(128,128,128));} break;
			case ESimpleItemRarity::Distinct: {Background->SetContentColorAndOpacity(FLinearColor(10,225,10));} break;
			case ESimpleItemRarity::Special: {Background->SetContentColorAndOpacity(FLinearColor(10,225,225));} break;
			case ESimpleItemRarity::Legendary: {Background->SetContentColorAndOpacity(FLinearColor(225,128,10));} break;
		default: break;}
	}///
	//
	LOG_SV(true,ESeverity::Info,TEXT("Reconstruct Item()!"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryItemWidget::OnItemClicked() {
	if (Item==nullptr||Item->IsPendingKill()) {OnItemRemoved(); return;}
	else if (Item->Amount<=0) {OnItemRemoved(); return;}
	//
	EInventoryAction Result;
	if (Item->ParentInventory) {
		Item->ParentInventory->UseItem_ByRef(Item,1,Result);
	}///
	//
	LOG_SV(true,ESeverity::Info,
		FString::Printf(TEXT("OnItemIconClicked()  ::  (%s)"),*Item->GetName())
	);//
}

void USimpleInventoryItemWidget::OnItemRemoved() {
	RemoveFromParent();
	//
	Item->OnItemAdd.RemoveAll(this);
	Item->OnItemUse.RemoveAll(this);
	//
	ConditionalBeginDestroy(); return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryItemWidget::OnItemAdded_Implementation(const int32 Quantity) {
	Reconstruct();
}

void USimpleInventoryItemWidget::OnItemUsed_Implementation(const int32 Quantity, const bool Liquidated, const bool Expired) {
	Reconstruct();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryItemWidget::SetItemReference(USimpleInventoryItem* NewItem) {
	Item = NewItem;
	//
	if (Item) {
		Item->OnItemAdd.RemoveAll(this);
		Item->OnItemUse.RemoveAll(this);
		//
		Item->OnItemUse.AddUniqueDynamic(this,&USimpleInventoryItemWidget::OnItemUsed);
		Item->OnItemAdd.AddUniqueDynamic(this,&USimpleInventoryItemWidget::OnItemAdded);
	}///
	//
	Reconstruct();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////