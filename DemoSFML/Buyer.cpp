#include "Buyer.h"
void Buyer::Trade() 
{
    if (CanTrade())
    {
        Inventory::shopInventory->RemoveItem(GetItem(), m_itemQuantity);
        Inventory::shopInventory->AddGold(m_price);
        m_state = WALK_TO_EXIT;
    }
    else
    {
        std::cerr << "cannot accept buy";
    }

    std::cerr << "Buying: " << GetItem()->name << " (Quantity: " << m_itemQuantity << ", Price: " << m_price << ")" << Inventory::shopInventory->GetGold() << std::endl;
}

bool Buyer::CanTrade() 
{
    return this->GetItem() != nullptr && this != nullptr && m_state == BUSY && Inventory::shopInventory->Contains(this->GetItem()->name, m_itemQuantity);
}
