#include "Seller.h"

void Seller::Trade()
{
    if (CanTrade())
    {
        Inventory::shopInventory->AddItem(GetItem(), m_itemQuantity);
        Inventory::shopInventory->RemoveGold(m_price);
        m_state = WALK_TO_EXIT;
    }
    else
    {
        std::cerr << "cannot accept sell";
    }

    std::cerr << "Selling: " << GetItem()->name << " (Quantity: " << m_itemQuantity << ", Price: " << m_price << ")" << Inventory::shopInventory->GetGold() << std::endl;
}

bool Seller::CanTrade()
{
    return this->GetItem() != nullptr && this != nullptr && m_state == BUSY && Inventory::shopInventory->GetGold() >= m_price;
}
