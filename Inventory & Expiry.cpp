#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

// ==========================================
// 1. ITEM CLASS (Data Model)
// ==========================================
class Item {
private:
    int id;
    std::string name;
    int quantity;
    double price;
    std::string expiryDate; // Format: YYYY-MM-DD

public:
    Item(int id, std::string name, int quantity, double price, std::string expiryDate) {
        this->id = id;
        this->name = name;
        this->quantity = quantity;
        this->price = price;
        this->expiryDate = expiryDate;
    }

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    std::string getExpiryDate() const { return expiryDate; }

    // Setter
    void setQuantity(int q) { quantity = q; }

    // Display formatted item details
    void display() const {
        std::cout << "ID: " << id 
                  << " | Name: " << name 
                  << " | Qty: " << quantity 
                  << " | Price: Rs." << price 
                  << " | Expiry: " << expiryDate << "\n";
    }
};

// ==========================================
// 2. INVENTORY MANAGER CLASS (Business Logic)
// ==========================================
class InventoryManager {
private:
    std::vector<Item> items;
    int reorderThreshold = 5; // Low stock limit

public:
    void addItem(const Item& newItem) {
        items.push_back(newItem);
    }

    void displayAll() const {
        if (items.empty()) {
            std::cout << "\n[Info] Inventory is currently empty.\n";
            return;
        }
        std::cout << "\n---------------- CURRENT INVENTORY LIST ----------------\n";
        for (const auto& item : items) {
            item.display();
        }
        std::cout << "--------------------------------------------------------\n";
    }

    void checkLowStock() const {
        std::cout << "\n--- LOW STOCK ALERTS (Quantity <= " << reorderThreshold << ") ---\n";
        bool found = false;
        for (const auto& item : items) {
            if (item.getQuantity() <= reorderThreshold) {
                item.display();
                found = true;
            }
        }
        if (!found) {
            std::cout << "All items are sufficiently stocked.\n";
        }
    }

    void checkExpiry(const std::string& currentDate) const {
        std::cout << "\n--- EXPIRY CHECK (Compared against: " << currentDate << ") ---\n";
        bool found = false;
        for (const auto& item : items) {
            // String comparison works directly for YYYY-MM-DD format
            if (item.getExpiryDate() <= currentDate) { 
                std::cout << "[EXPIRED/RISK] ";
                item.display();
                found = true;
            }
        }
        if (!found) {
            std::cout << "No items are expired.\n";
        }
    }

    const std::vector<Item>& getItems() const { 
        return items; 
    }
};

// ==========================================
// 3. FILE MANAGER CLASS (Data Persistence)
// ==========================================
class FileManager {
public:
    static void saveData(const std::string& filename, const std::vector<Item>& items) {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "\n[Error] Unable to open file for saving data.\n";
            return;
        }

        for (const auto& item : items) {
            outFile << item.getId() << ","
                    << item.getName() << ","
                    << item.getQuantity() << ","
                    << item.getPrice() << ","
                    << item.getExpiryDate() << "\n";
        }
        outFile.close();
        std::cout << "\n[Success] Data saved to '" << filename << "' successfully.\n";
    }

    static void loadData(const std::string& filename, InventoryManager& manager) {
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cout << "\n[Info] No previous record file found. Starting fresh inventory.\n";
            return;
        }

        std::string line;
        int count = 0;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::string idStr, name, qtyStr, priceStr, expiry;

            std::getline(ss, idStr, ',');
            std::getline(ss, name, ',');
            std::getline(ss, qtyStr, ',');
            std::getline(ss, priceStr, ',');
            std::getline(ss, expiry, ',');

            if (!idStr.empty()) {
                Item item(std::stoi(idStr), name, std::stoi(qtyStr), std::stod(priceStr), expiry);
                manager.addItem(item);
                count++;
            }
        }
        inFile.close();
        std::cout << "\n[Success] Loaded " << count << " items from '" << filename << "'.\n";
    }
};

// ==========================================
// 4. MAIN INTERACTIVE APPLICATION
// ==========================================
int main() {
    InventoryManager manager;
    const std::string filename = "inventory_data.txt";

    // Load existing inventory from file automatically at startup
    FileManager::loadData(filename, manager);

    int choice = 0;
    do {
        std::cout << "\n====================================\n";
        std::cout << "   INVENTORY & EXPIRY TRACKER MENU  \n";
        std::cout << "====================================\n";
        std::cout << "1. Add New Item\n";
        std::cout << "2. Display All Items\n";
        std::cout << "3. Check Low Stock Items\n";
        std::cout << "4. Check Expired Items\n";
        std::cout << "5. Save & Exit\n";
        std::cout << "Enter your choice (1-5): ";
        
        if (!(std::cin >> choice)) {
            std::cout << "\n[Error] Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            int id, qty;
            double price;
            std::string name, expiry;

            std::cout << "\nEnter Item ID: "; 
            std::cin >> id;
            std::cout << "Enter Item Name: "; 
            std::cin.ignore(); 
            std::getline(std::cin, name);
            std::cout << "Enter Quantity: "; 
            std::cin >> qty;
            std::cout << "Enter Price: "; 
            std::cin >> price;
            std::cout << "Enter Expiry Date (YYYY-MM-DD): "; 
            std::cin >> expiry;

            manager.addItem(Item(id, name, qty, price, expiry));
            std::cout << "\n[Success] Item added to inventory!\n";
        } 
        else if (choice == 2) {
            manager.displayAll();
        } 
        else if (choice == 3) {
            manager.checkLowStock();
        } 
        else if (choice == 4) {
            std::string todayDate;
            std::cout << "\nEnter Today's Date (YYYY-MM-DD): ";
            std::cin >> todayDate;
            manager.checkExpiry(todayDate);
        } 
        else if (choice == 5) {
            FileManager::saveData(filename, manager.getItems());
            std::cout << "Exiting program. Goodbye!\n";
        } 
        else {
            std::cout << "\n[Error] Choice out of range. Enter 1 to 5.\n";
        }

    } while (choice != 5);

    return 0;
}