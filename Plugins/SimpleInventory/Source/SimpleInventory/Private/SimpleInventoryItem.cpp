//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleInventoryItem.h"
#include "SimpleInventory_Shared.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USimpleInventoryItem::~USimpleInventoryItem(){}
USimpleInventoryItem::USimpleInventoryItem() {
	if (HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		SGUID = USavior3::MakeObjectGUID(this,EGuidGeneratorMode::StaticNameToGUID);
	} else {SGUID=USavior3::MakeObjectGUID(this,EGuidGeneratorMode::ResolvedNameToGUID);}
	//
	ItemName = FText::FromString(TEXT("Item"));
	CanConsume = false;
	CanExpire = false;
	//
	Durability = 100;
	Capacity = 1;
	Amount = 1;
	Weight = 1;
	Level = 1;
	//
	Rarity = ESimpleItemRarity::Common;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryItem::PostLoad() {
	Super::PostLoad();
	//
	if (HasAnyFlags(RF_ClassDefaultObject)) {
		SGUID = USavior3::MakeObjectGUID(this,EGuidGeneratorMode::ResolvedNameToGUID);
	}///
}

void USimpleInventoryItem::PostInitProperties() {
	Super::PostInitProperties();
	//
	if (World==nullptr) {
		if (AActor*const&Owner=GetTypedOuter<AActor>()) {
			SetWorld(Owner->GetWorld());
		}///
	}///
	//
	if (USimpleInventoryComponent*const&Inventory=GetTypedOuter<USimpleInventoryComponent>()) {
		ParentInventory = Inventory;
	}///
	//
	if (!HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		SGUID = USavior3::MakeObjectGUID(this,EGuidGeneratorMode::ResolvedNameToGUID);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryItem::OnLoaded_Implementation(const FSlotMeta &MetaData) {
	if (GetWorld()==nullptr||(!GetWorld()->IsGameWorld())) {return;}
	//
	if (ParentInventory) {
		LOG_SV(true,ESeverity::Info,
			FString::Printf(TEXT("OnLoaded()  ==>>  %s  ::  %s"),
				*ParentInventory->GetName(),
				*ItemName.ToString()
			)///
		);//
	}///
}

UWorld* USimpleInventoryItem::GetWorld() const {
	UActorComponent* const &Owner = GetTypedOuter<UActorComponent>();
	//
	if (Owner) {return Owner->GetWorld();}
	//
	return World;
}

void USimpleInventoryItem::SetWorld(UWorld* InWorld) {
	World = InWorld;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool USimpleInventoryItem::IsStackable() const {
	return (Capacity > 1);
}

bool USimpleInventoryItem::HasCapacity(int32 IncomingLoad) const {
	return ((Amount + IncomingLoad) <= Capacity);
}

bool USimpleInventoryItem::HasExpired() const {
	return (CanExpire && (Durability <= 0));
}

int32 USimpleInventoryItem::GetFullWeight() const {
	return (Weight * Amount);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////