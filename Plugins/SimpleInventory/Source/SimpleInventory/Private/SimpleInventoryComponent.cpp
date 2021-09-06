//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2021 (C) Bruno Xavier Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleInventoryComponent.h"
#include "SimpleInventory_Shared.h"

#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FInventorySortByTypePredicate {
public:
	FInventorySortByTypePredicate(EInventorySorting InSortDirection) 
		: SortMode(InSortDirection)
	{}//
public:
	bool operator () (const USimpleInventoryItem &A, const USimpleInventoryItem &B) const {
		if (SortMode==EInventorySorting::Ascending) {
			return (A.GetClass()->GetName().GetAllocatedSize() < B.GetClass()->GetName().GetAllocatedSize());
		} else {
			return (A.GetClass()->GetName().GetAllocatedSize() > B.GetClass()->GetName().GetAllocatedSize());
		}///
	}///
public:
	bool operator () (const USimpleInventoryItem* A, const USimpleInventoryItem* B) const {
		if (SortMode==EInventorySorting::Ascending) {
			if (B==nullptr) {return false;} else if (A && B) {
				return (A->GetClass()->GetName().GetAllocatedSize() < B->GetClass()->GetName().GetAllocatedSize());
			} else {return true;}
		} else {
			if (B==nullptr) {return false;} else if (A && B) {
				return (A->GetClass()->GetName().GetAllocatedSize() > B->GetClass()->GetName().GetAllocatedSize());
			} else {return true;}
		}///
	}///
private:
	EInventorySorting SortMode;
};

struct FInventorySortByStackPredicate {
public:
	FInventorySortByStackPredicate(EInventorySorting InSortDirection) 
		: SortMode(InSortDirection)
	{}//
public:
	bool operator () (const USimpleInventoryItem &A, const USimpleInventoryItem &B) const {
		if (SortMode==EInventorySorting::Ascending) {
			return ((A.Amount < B.Amount)||(A.GetClass()->GetName().GetAllocatedSize() < B.GetClass()->GetName().GetAllocatedSize()));
		} else {
			return ((A.Amount > B.Amount)||(A.GetClass()->GetName().GetAllocatedSize() > B.GetClass()->GetName().GetAllocatedSize()));
		}///
	}///
public:
	bool operator () (const USimpleInventoryItem* A, const USimpleInventoryItem* B) const {
		if (SortMode==EInventorySorting::Ascending) {
			if (B==nullptr) {return false;} else if (A && B) {
				return ((A->Amount < B->Amount)||(A->GetClass()->GetName().GetAllocatedSize() < B->GetClass()->GetName().GetAllocatedSize()));
			} else {return true;}
		} else {
			if (B==nullptr) {return false;} else if (A && B) {
				return ((A->Amount > B->Amount)||(A->GetClass()->GetName().GetAllocatedSize() > B->GetClass()->GetName().GetAllocatedSize()));
			} else {return true;}
		}///
	}///
private:
	EInventorySorting SortMode;
};

struct FInventorySortByRarityPredicate {
public:
	FInventorySortByRarityPredicate(EInventorySorting InSortDirection) 
		: SortMode(InSortDirection)
	{}//
public:
	bool operator () (const USimpleInventoryItem &A, const USimpleInventoryItem &B) const {
		if (SortMode==EInventorySorting::Ascending) {
			if (A.Rarity==B.Rarity) {
				return (A.Amount < B.Amount);
			} else {return (A.Rarity < B.Rarity);}
		} else {
			if (A.Rarity==B.Rarity) {
				return (A.Amount > B.Amount);
			} else {return (A.Rarity > B.Rarity);}
		}///
	}///
public:
	bool operator () (const USimpleInventoryItem* A, const USimpleInventoryItem* B) const {
		if (SortMode==EInventorySorting::Ascending) {
			if (B==nullptr) {return false;} else if (A && B) {
				if (A->Rarity==B->Rarity) {
					return (A->Amount < B->Amount);
				} else {return (A->Rarity < B->Rarity);}
			} else {return true;}
		} else {
			if (B==nullptr) {return false;} else if (A && B) {
				if (A->Rarity==B->Rarity) {
					return (A->Amount > B->Amount);
				} else {return (A->Rarity > B->Rarity);}
			} else {return true;}
		}///
	}///
private:
	EInventorySorting SortMode;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USimpleInventoryComponent::~USimpleInventoryComponent(){}
USimpleInventoryComponent::USimpleInventoryComponent() {
	if (HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		SGUID = USavior3::MakeComponentGUID(this,EGuidGeneratorMode::StaticNameToGUID);
	} else {SGUID=USavior3::MakeComponentGUID(this,EGuidGeneratorMode::ResolvedNameToGUID);}
	//
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
	//
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	//
	SetComponentTickEnabled(false);
	//
	WeightLimit = 1000;
	TotalWeight = 0;
	SlotLimit = 10;
}

void USimpleInventoryComponent::PostInitProperties() {
	Super::PostInitProperties();
	//
	if (!HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		SGUID = USavior3::MakeComponentGUID(this,EGuidGeneratorMode::ResolvedNameToGUID);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryComponent::OnLoaded_Implementation(const FSlotMeta &MetaData) {
	if (GetWorld()==nullptr||(!GetWorld()->IsGameWorld())) {return;}
	//
	if (OnSlotLimitChanged.IsBound()) {OnSlotLimitChanged.Broadcast(SlotLimit);}
	if (OnWeightLimitChanged.IsBound()) {OnWeightLimitChanged.Broadcast(WeightLimit);}
	//
	LOG_SV(true,ESeverity::Warning,
		FString::Printf(TEXT("Inventory Loaded::   ==>>   %s"),*GetName())
	);//
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryComponent::InsertItem(USimpleInventoryItem* NewItem, const int32 Quantity, EInventoryAction &Result) {
	if (!NewItem->IsValidLowLevelFast()||NewItem->IsPendingKill()) {Result=EInventoryAction::Failed; return;}
	if (Quantity < 1) {Result=EInventoryAction::Failed; return;}
	//
	int32 Index = Inventory.IndexOfByKey(NewItem);
	//
	if (Index==INDEX_NONE) {
		Index = Inventory.Add(NewItem);
	}///
	//
	if (Index > INDEX_NONE) {
		if (NewItem->OnItemAdd.IsBound()) {NewItem->OnItemAdd.Broadcast(Quantity);}
		if (OnItemAdd.IsBound()) {OnItemAdd.Broadcast(NewItem,Quantity);}
		//
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			//
			const auto &Interface = Cast<ISimpleInventoryInterface>(*OBJ);
			if (Interface) {Interface->Execute_OnInventoryItemAdd(*OBJ,this,NewItem,Index,Quantity);} else
			if ((*OBJ)->GetClass()->ImplementsInterface(USimpleInventoryInterface::StaticClass())) {
				ISimpleInventoryInterface::Execute_OnInventoryItemAdd(*OBJ,this,NewItem,Index,Quantity);
			}///
		}///
		//
		if (IsOverWeightLimit()) {
			OnWeightLimitSurpassed(NewItem,TotalWeight,NewItem->GetFullWeight());
		}///
		//
		LOG_SV(true,ESeverity::Warning,
			FString::Printf(TEXT("Item Added  ::  (%s  |  %s  |  %i)"),
				*this->GetName(), *NewItem->ItemName.ToString(), NewItem->Amount
			)///
		);//
		//
		Result = EInventoryAction::Success; return;
	} Result = EInventoryAction::Failed;
}

void USimpleInventoryComponent::InsertItemAtIndex(USimpleInventoryItem* OnItem, const int32 Index, const int32 Quantity, EInventoryAction &Result) {
	if (!OnItem->IsValidLowLevelFast()||OnItem->IsPendingKill()) {Result=EInventoryAction::Failed; return;}
	if (!Inventory.IsValidIndex(Index)) {Result=EInventoryAction::Failed; return;}
	if (Quantity < 1) {Result=EInventoryAction::Failed; return;}
	//
	if (OnItem->OnItemAdd.IsBound()) {OnItem->OnItemAdd.Broadcast(Quantity);}
	if (OnItemAdd.IsBound()) {OnItemAdd.Broadcast(OnItem,Quantity);}
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		//
		const auto &Interface = Cast<ISimpleInventoryInterface>(*OBJ);
		if (Interface) {Interface->Execute_OnInventoryItemAdd(*OBJ,this,OnItem,Index,Quantity);} else
		if ((*OBJ)->GetClass()->ImplementsInterface(USimpleInventoryInterface::StaticClass())) {
			ISimpleInventoryInterface::Execute_OnInventoryItemAdd(*OBJ,this,OnItem,Index,Quantity);
		}///
	}///
	//
	if (IsOverWeightLimit()) {
		OnWeightLimitSurpassed(OnItem,TotalWeight,OnItem->GetFullWeight());
	}///
	//
	LOG_SV(true,ESeverity::Warning,
		FString::Printf(TEXT("Item Added  ::  (%s  |  %s  |  %i)"),
			*this->GetName(), *OnItem->ItemName.ToString(), OnItem->Amount
		)///
	);//
	//
	Result = EInventoryAction::Success;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryComponent::ConsumeItem(USimpleInventoryItem* OnItem, const int32 Quantity, EInventoryAction &Result) {
	if (!OnItem->IsValidLowLevelFast()||OnItem->IsPendingKill()) {Result=EInventoryAction::Failed; return;}
	if (Quantity < 1) {Result=EInventoryAction::Failed; return;}
	//
	int32 Index = Inventory.IndexOfByKey(OnItem);
	//
	const bool Liquidated = (OnItem->Amount<=0);
	const bool Expired = OnItem->HasExpired();
	//
	if (Index > INDEX_NONE) {
		if (OnItem->OnItemUse.IsBound()) {OnItem->OnItemUse.Broadcast(Quantity,Liquidated,Expired);}
		if (OnItemUse.IsBound()) {OnItemUse.Broadcast(OnItem,Quantity,Liquidated,Expired);}
		//
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_BeginDestroyed)) {continue;}
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			//
			const auto &Interface = Cast<ISimpleInventoryInterface>(*OBJ);
			if (Interface) {Interface->Execute_OnInventoryItemUse(*OBJ,this,OnItem,Quantity,Liquidated,Expired);} else
			if ((*OBJ)->GetClass()->ImplementsInterface(USimpleInventoryInterface::StaticClass())) {
				ISimpleInventoryInterface::Execute_OnInventoryItemUse(*OBJ,this,OnItem,Quantity,Liquidated,Expired);
			}///
		}///
		//
		if (Liquidated) {
			OnItem->ConditionalBeginDestroy();
		}///
		//
		LOG_SV(true,ESeverity::Warning,
			FString::Printf(TEXT("Item Used  ::  (%s  |  %s  |  %i)"),
				*this->GetName(), *OnItem->ItemName.ToString(), Quantity
			)///
		);//
		//
		Result = EInventoryAction::Success; return;
	} Result = EInventoryAction::Failed;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USimpleInventoryItem* USimpleInventoryComponent::AddItem(const TSubclassOf<USimpleInventoryItem>ItemType, const int32 Quantity, EInventoryAction &Result) {
	if (ItemType.Get()==nullptr) {goto FAIL;}
	if (IsOverWeightLimit()) {goto FAIL;}
	if (Quantity < 1) {goto FAIL;}
	//
	USimpleInventoryItem* CDO = Cast<USimpleInventoryItem>(ItemType->GetDefaultObject(true));
	if (CDO==nullptr) {goto FAIL;}
	//
	bool AnySuccess = false;
	bool StackRoundTrip = false;
	//
	int32 NewStack = Quantity; 
	USimpleInventoryItem* Item = nullptr;
	//
	//
	STACK_UP:{
		if (USimpleInventoryItem*Stack=FindItem_ByType(ItemType,true)) {
			if (Stack->HasExpired()) {
				if (Stack->IsStackable()) {goto GEN_STACK;} else {goto GEN_FLAT;}
			} else {
				const int32 FreeSpace = FMath::Clamp((Stack->Capacity-Stack->Amount),0,Stack->Capacity);
				const int32 StackLoad = FMath::Clamp(NewStack,0,FreeSpace);
				//
				Item = Stack;
				Stack->Amount += StackLoad;
				NewStack = (NewStack - StackLoad);
				InsertItem(Stack,StackLoad,Result);
				//
				if ((NewStack>0)&&(!StackRoundTrip)) {
					if (Stack->IsStackable()) {goto GEN_STACK;} else {goto GEN_FLAT;}
				} else {return Stack;}
			}///
		} else if (StackRoundTrip) {goto FAIL;} else
		if (CDO->IsStackable()) {goto GEN_STACK;} else {goto GEN_FLAT;}
	}///
	//
	//
	GEN_STACK:{
		if (IsInventoryFull()) {
			if (!StackRoundTrip) {
				StackRoundTrip=true; {goto STACK_UP;}
			} else {
				LOG_SV(true,ESeverity::Warning,TEXT("Inventory is full!"));
				OnSlotLimitSurpassed(Item,SlotLimit); return Item;
			}///
		}///
		//
		while (NewStack > 0) {
			if (USimpleInventoryItem*Stack=NewObject<USimpleInventoryItem>(this,ItemType,NAME_None,RF_NoFlags,CDO)) {
				const int32 FreeSpace = FMath::Clamp((Stack->Capacity-Stack->Amount),0,Stack->Capacity);
				const int32 StackLoad = FMath::Clamp<int32>(NewStack,0,FreeSpace);
				//
				Item = Stack;
				Stack->Amount += StackLoad;
				NewStack = (NewStack - StackLoad);
				InsertItem(Stack,StackLoad,Result);
				//
				if (Result==EInventoryAction::Success) {AnySuccess=true;}
				if (IsInventoryFull()) {OnSlotLimitSurpassed(Stack,SlotLimit); break;}
			} if (AnySuccess) {Result=EInventoryAction::Success;}
		} return Item;
	}///
	//
	//
	GEN_FLAT:{
		if (IsInventoryFull()) {
			if (!StackRoundTrip) {
				StackRoundTrip=true; {goto STACK_UP;}
			} else {
				LOG_SV(true,ESeverity::Warning,TEXT("Inventory is full!"));
				OnSlotLimitSurpassed(Item,SlotLimit); return Item;
			}///
		}///
		//
		for (int32 I=0; I<NewStack; ++I) {
			if (USimpleInventoryItem*Instance=NewObject<USimpleInventoryItem>(this,ItemType,NAME_None,RF_NoFlags,CDO)) {
				Instance->Amount = 1; InsertItem(Instance,1,Result); Item = Instance;
				if (IsInventoryFull()) {OnSlotLimitSurpassed(Instance,SlotLimit); break;}
			} if (Result==EInventoryAction::Success) {AnySuccess=true;}
		} if (AnySuccess) {Result=EInventoryAction::Success;}
		//
		return Item;
	}///
	//
	//
	FAIL:{
		Result = EInventoryAction::Failed; return nullptr;
	}///
}

USimpleInventoryItem* USimpleInventoryComponent::AddItemAtIndex(const TSubclassOf<USimpleInventoryItem>ItemType, int32 SlotIndex, int32 Quantity, EInventoryAction &Result) {
	if (!Inventory.IsValidIndex(SlotIndex)) {goto FAIL;}
	if (ItemType.Get()==nullptr) {goto FAIL;}
	if (IsOverWeightLimit()) {goto FAIL;}
	if (Quantity < 1) {goto FAIL;}
	//
	USimpleInventoryItem* CDO = Cast<USimpleInventoryItem>(ItemType->GetDefaultObject(true));
	if (CDO==nullptr) {goto FAIL;}
	//
	bool AnySuccess = false;
	bool StackRoundTrip = false;
	//
	int32 NewStack = Quantity; 
	USimpleInventoryItem* Item = nullptr;
	//
	//
	STACK_UP:{
		if (USimpleInventoryItem*const&Local=Inventory[SlotIndex]) {
			if (ItemType.Get()!=Local->GetClass()) {goto FAIL;}
			//
			if (Local->HasExpired()) {
				Result = EInventoryAction::Failed; return Local;
			} else {
				const int32 FreeSpace = FMath::Clamp((Local->Capacity-Local->Amount),0,Local->Capacity);
				const int32 StackLoad = FMath::Clamp(Quantity,0,FreeSpace);
				//
				Item = Local;
				Local->Amount += StackLoad;
				InsertItemAtIndex(Local,SlotIndex,StackLoad,Result);
				//
				if ((NewStack>0)&&(!StackRoundTrip)) {
					if (Local->IsStackable()) {goto GEN_STACK;} else {goto GEN_FLAT;}
				} else {return Local;}
			}///
		} else if (StackRoundTrip) {goto FAIL;} else
		if (CDO->IsStackable()) {goto GEN_STACK;} else {goto GEN_FLAT;}
	}///
	//
	//
	GEN_STACK:{
		if (IsInventoryFull()) {
			if (!StackRoundTrip) {
				StackRoundTrip=true; {goto STACK_UP;}
			} else {
				LOG_SV(true,ESeverity::Warning,TEXT("Inventory is full!"));
				OnSlotLimitSurpassed(Item,SlotLimit); return Item;
			}///
		}///
		//
		while (NewStack > 0) {
			if (USimpleInventoryItem*Stack=NewObject<USimpleInventoryItem>(this,ItemType,NAME_None,RF_NoFlags,CDO)) {
				const int32 FreeSpace = FMath::Clamp((Stack->Capacity-Stack->Amount),0,Stack->Capacity);
				const int32 StackLoad = FMath::Clamp<int32>(NewStack,0,FreeSpace);
				//
				Item = Stack;
				Stack->Amount += StackLoad;
				NewStack = (NewStack - StackLoad);
				InsertItemAtIndex(Stack,SlotIndex,StackLoad,Result);
				//
				if (Result==EInventoryAction::Success) {AnySuccess=true;}
				if (IsInventoryFull()) {OnSlotLimitSurpassed(Stack,SlotLimit); break;}
			} if (AnySuccess) {Result=EInventoryAction::Success;}
		} return Item;
	}///
	//
	//
	GEN_FLAT:{
		if (IsInventoryFull()) {
			if (!StackRoundTrip) {
				StackRoundTrip=true; {goto STACK_UP;}
			} else {
				LOG_SV(true,ESeverity::Warning,TEXT("Inventory is full!"));
				OnSlotLimitSurpassed(Item,SlotLimit); return Item;
			}///
		}///
		//
		int32 FlatIndex = SlotIndex;
		for (int32 I=0; I<NewStack; ++I) {
			while (FlatIndex <= SlotLimit) {
				if (!Inventory.IsValidIndex(FlatIndex)) {break;}
				if (Inventory[FlatIndex] == nullptr) {break;}
			++FlatIndex;} if (FlatIndex > SlotLimit) {break;}
			//
			if (USimpleInventoryItem*Instance=NewObject<USimpleInventoryItem>(this,ItemType,NAME_None,RF_NoFlags,CDO)) {
				Instance->Amount = 1; InsertItemAtIndex(Instance,FlatIndex,1,Result); Item = Instance;
				if (IsInventoryFull()) {OnSlotLimitSurpassed(Instance,SlotLimit); break;}
			} if (Result==EInventoryAction::Success) {AnySuccess=true;}
		} if (AnySuccess) {Result=EInventoryAction::Success;}
		//
		return Item;
	}///
	//
	//
	FAIL:{
		Result = EInventoryAction::Failed; return nullptr;
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryComponent::UseItem_ByType(const TSubclassOf<USimpleInventoryItem>ItemType, const int32 Quantity, EInventoryAction &Result) {
	if (ItemType.Get()==nullptr) {goto FAIL;}
	if (Quantity < 1) {goto FAIL;}
	//
	bool AnySuccess = false;
	int32 Tax = Quantity;
	//
	//
	USE:{
		bool HasConsumable = false;
		for (USimpleInventoryItem* const &Item : Inventory) {
			if (Item==nullptr||Item->IsPendingKill()) {continue;}
			if (!Item->GetClass()->IsChildOf(ItemType.Get())) {continue;}
			if (Item->HasExpired()) {continue;}
			//
			if (Item->CanConsume) {HasConsumable=true; break;}
		}///
		//
		if (!HasConsumable) {
			if (AnySuccess) {goto END;} else {goto FAIL;}
		}///
		//
		for (USimpleInventoryItem* const &Item : Inventory) {
			if (Item==nullptr||Item->IsPendingKill()) {continue;}
			//
			if (Item->Amount <= 0) {continue;}
			if (!Item->CanConsume) {continue;}
			if (Item->HasExpired()) {continue;}
			if (!Item->GetClass()->IsChildOf(ItemType.Get())) {continue;}
			//
			const int32 Used = FMath::Clamp(Tax,0,Item->Amount);
			const int32 Deficit = FMath::Abs<int32>(Tax-Used);
			//
			Item->Amount -= Used;
			ConsumeItem(Item,Used,Result);
			if (Result==EInventoryAction::Success) {AnySuccess=true;}
			//
			if (Deficit > 0) {
				Tax = Deficit; {goto USE;}
			} else if (AnySuccess) {goto END;}
		}///
	}///
	//
	//
	FAIL:{
		Result = EInventoryAction::Failed; return;
	}///
	//
	//
	END:{
		Result = EInventoryAction::Success; return;
	}///
}

void USimpleInventoryComponent::UseItem_ByIndex(int32 SlotIndex, const int32 Quantity, EInventoryAction &Result) {
	if (!Inventory.IsValidIndex(SlotIndex)) {goto FAIL;}
	if (Quantity < 1) {goto FAIL;}
	//
	//
	if (USimpleInventoryItem*const&Item=Inventory[SlotIndex]) {
		if (Item==nullptr||Item->IsPendingKill()) {goto FAIL;}
		//
		if (!Item->CanConsume)  {goto FAIL;} else
		if (Item->HasExpired()) {goto FAIL;} else {
			const int32 Used = FMath::Clamp(Quantity,0,Item->Amount);
			//
			Item->Amount -= Used;
			ConsumeItem(Item,Used,Result);
			//
			if (Result==EInventoryAction::Success) {return;}
		}///
	}///
	//
	//
	FAIL:{
		Result = EInventoryAction::Failed; return;
	}///
}

void USimpleInventoryComponent::UseItem_ByGuid(const FGuid &GUID, const int32 Quantity, EInventoryAction &Result) {
	if (Quantity < 1) {goto FAIL;}
	//
	for (USimpleInventoryItem* const &Item : Inventory) {
		if (Item==nullptr||Item->IsPendingKill()) {continue;}
		if (Item->SGUID != GUID) {continue;}
		//
		if (!Item->CanConsume)  {goto FAIL;} else
		if (Item->HasExpired()) {goto FAIL;} else {
			const int32 Used = FMath::Clamp(Quantity,0,Item->Amount);
			//
			Item->Amount -= Used;
			ConsumeItem(Item,Used,Result);
			//
			if (Result==EInventoryAction::Success) {return;}
		}///
	}///
	//
	//
	FAIL:{
		Result = EInventoryAction::Failed; return;
	}///
}

void USimpleInventoryComponent::UseItem_ByRef(USimpleInventoryItem* const &Item, const int32 Quantity, EInventoryAction &Result) {
	if (Item==nullptr||Item->IsPendingKill()) {goto FAIL;}
	//
	if (!Item->CanConsume)  {goto FAIL;} else
	if (Item->HasExpired()) {goto FAIL;} else {
		const int32 Used = FMath::Clamp(Quantity,0,Item->Amount);
		//
		Item->Amount -= Used;
		ConsumeItem(Item,Used,Result);
		//
		if (Result==EInventoryAction::Success) {return;}
	}///
	//
	FAIL:{
		Result = EInventoryAction::Failed; return;
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USimpleInventoryItem* USimpleInventoryComponent::FindItem_ByType(const TSubclassOf<USimpleInventoryItem>ItemType, const bool FreeSpaceOnly) const {
	if (ItemType.Get()==nullptr) {return nullptr;}
	//
	for (USimpleInventoryItem* const &Item : Inventory) {
		if (Item==nullptr) {continue;}
		//
		if (Item->GetClass()->IsChildOf(ItemType.Get())) {
			if (!FreeSpaceOnly) {return Item;} else {
				const int32 FreeSpace = (Item->Capacity - Item->Amount);
				if (FreeSpace > 0) {return Item;}
			}///
		}///
	}///
	//
	return nullptr;
}

USimpleInventoryItem* USimpleInventoryComponent::FindItem_ByIndex(const int32 SlotIndex) const {
	if (SlotIndex < 0) {return nullptr;}
	//
	if (Inventory.IsValidIndex(SlotIndex)) {
		return Inventory[SlotIndex];
	}///
	//
	return nullptr;
}

USimpleInventoryItem* USimpleInventoryComponent::FindItem_ByGuid(const FGuid &GUID) const {
	for (USimpleInventoryItem* const &Item : Inventory) {
		if (Item==nullptr) {continue;}
		if (Item->SGUID==GUID) {return Item;}
	}///
	//
	return nullptr;
}

USimpleInventoryItem* USimpleInventoryComponent::FindItem_ByName(const FText &Name) const {
	const FString SearchName = Name.ToString().TrimStartAndEnd();
	//
	for (USimpleInventoryItem* const &Item : Inventory) {
		if (Item==nullptr) {continue;}
		//
		const FString ItemName = Item->ItemName.ToString().TrimStartAndEnd();
		if (ItemName.Equals(SearchName,ESearchCase::CaseSensitive)) {return Item;}
	}///
	//
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryComponent::SortInventory_ByType(const EInventorySorting SortMode) {
	Inventory.Sort(FInventorySortByTypePredicate(SortMode));
	//
	if (OnSlotLimitChanged.IsBound()) {OnSlotLimitChanged.Broadcast(SlotLimit);}
}

void USimpleInventoryComponent::SortInventory_ByStack(const EInventorySorting SortMode) {
	Inventory.Sort(FInventorySortByStackPredicate(SortMode));
	//
	if (OnSlotLimitChanged.IsBound()) {OnSlotLimitChanged.Broadcast(SlotLimit);}
}

void USimpleInventoryComponent::SortInventory_ByRarity(const EInventorySorting SortMode) {
	Inventory.Sort(FInventorySortByRarityPredicate(SortMode));
	//
	if (OnSlotLimitChanged.IsBound()) {OnSlotLimitChanged.Broadcast(SlotLimit);}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32 USimpleInventoryComponent::GetFirstFreeSlotIndex() const {
	for (int32 I=0; I<Inventory.Num(); ++I) {
		if (Inventory[I]==nullptr) {return I;}
	}///
	//
	return INDEX_NONE;
}

int32 USimpleInventoryComponent::CalculateInventoryWeight() const {
	int32 Weight = 0;
	//
	for (USimpleInventoryItem* const &Item : Inventory) {
		if (Item==nullptr) {continue;}
		//
		Weight += Item->GetFullWeight();
	}///
	//
	return Weight;
}

bool USimpleInventoryComponent::IsOverWeightLimit() const {
	TotalWeight = CalculateInventoryWeight();
	//
	return (TotalWeight >= WeightLimit);
}

bool USimpleInventoryComponent::IsInventoryFull() const {
	return (Inventory.Num() >= SlotLimit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USimpleInventoryComponent::SetWeightLimit(int32 NewLimit) {
	WeightLimit = NewLimit;
	//
	if (OnWeightLimitChanged.IsBound()) {
		OnWeightLimitChanged.Broadcast(WeightLimit);
	}///
}

void USimpleInventoryComponent::SetSlotLimit(int32 NewLimit) {
	SlotLimit = NewLimit;
	//
	if (OnSlotLimitChanged.IsBound()) {
		OnSlotLimitChanged.Broadcast(SlotLimit);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
