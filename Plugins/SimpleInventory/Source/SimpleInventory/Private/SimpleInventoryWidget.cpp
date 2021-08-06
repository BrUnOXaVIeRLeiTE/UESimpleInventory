//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleInventoryWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USimpleInventoryWidget::~USimpleInventoryWidget(){}
USimpleInventoryWidget::USimpleInventoryWidget(const FObjectInitializer &Initializer) : Super(Initializer) {
	bIsFocusable = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void USimpleInventoryWidget::Reconstruct() {
	if (Inventory==nullptr||Inventory->IsPendingKill()) {return;}
	//
	if (InventoryView) {
		InventoryView->ClearChildren();
		//
		for (const auto &Item : Inventory->Inventory) {
			CreateItemWidget(Item);
		}///
	}///
	//
	LOG_SV(true,ESeverity::Warning,TEXT("Reconstruct Inventory()!"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryWidget::CreateItemWidget(USimpleInventoryItem* Item) {
	if (InventoryView==nullptr||InventoryView->IsPendingKill()) {return;}
	const TArray<UWidget*>List = InventoryView->GetAllChildren();
	//
	for (const auto &W : List) {
		if (W==nullptr||W->IsPendingKill()) {continue;}
		if (USimpleInventoryItemWidget*Widget=Cast<USimpleInventoryItemWidget>(W)) {
			if (Widget->Item==nullptr) {continue;}
			//
			if ((*Widget->Item)==(*Item)) {
				Widget->SetItemReference(Item); return;
			}///
		}///
	}///
	//
	if (USimpleInventoryItemWidget*ItemWidget=CreateWidget<USimpleInventoryItemWidget>(this,ItemWidgetClass)) {
		InventoryView->AddChildToWrapBox(ItemWidget);
		ItemWidget->SetItemReference(Item);
	}///
	//
	LOG_SV(true,ESeverity::Info,TEXT("CreateItemWidget()!"));
}

void USimpleInventoryWidget::RefreshItemWidget(USimpleInventoryItem* Item) {
	if (InventoryView==nullptr||InventoryView->IsPendingKill()) {return;}
	const TArray<UWidget*>List = InventoryView->GetAllChildren();
	//
	for (const auto &W : List) {
		if (W==nullptr||W->IsPendingKill()) {continue;}
		if (USimpleInventoryItemWidget*Widget=Cast<USimpleInventoryItemWidget>(W)) {
			if (Widget->Item==nullptr) {continue;}
			//
			if ((*Widget->Item)==(*Item)) {
				Widget->SetItemReference(Item); break;
			}///
		}///
	}///
	//
	LOG_SV(true,ESeverity::Info,TEXT("RefreshItemWidget()!"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryWidget::OnSlotLimitUpdated(int32 NewLimit) {
	Reconstruct();
}

void USimpleInventoryWidget::OnItemAdded_Implementation(USimpleInventoryItem* Item, const int32 Quantity) {
	if (InventoryView) {CreateItemWidget(Item);}
	//
	LOG_SV(true,ESeverity::Info,TEXT("OnItemAdded_Widget()!"));
}

void USimpleInventoryWidget::OnItemUsed_Implementation(USimpleInventoryItem* Item, const int32 Quantity, const bool Liquidated, const bool Expired) {
	if (InventoryView) {RefreshItemWidget(Item);}
	//
	LOG_SV(true,ESeverity::Info,TEXT("OnItemUsed_Widget()!"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryWidget::SetInventoryReference(USimpleInventoryComponent* NewInventory) {
	Inventory = NewInventory;
	//
	if (Inventory) {
		Inventory->OnItemAdd.RemoveAll(this);
		Inventory->OnItemUse.RemoveAll(this);
		Inventory->OnSlotLimitChanged.RemoveAll(this);
		//
		Inventory->OnItemUse.AddUniqueDynamic(this,&USimpleInventoryWidget::OnItemUsed);
		Inventory->OnItemAdd.AddUniqueDynamic(this,&USimpleInventoryWidget::OnItemAdded);
		Inventory->OnSlotLimitChanged.AddUniqueDynamic(this,&USimpleInventoryWidget::OnSlotLimitUpdated);
	}///
	//
	Reconstruct();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////