#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>  // Add this for date functions
#include <iomanip>  // Add this for setw and setprecision
using namespace std;

#ifdef _WIN32
    #include <direct.h>  // For Windows _mkdir
#else
    #include <sys/stat.h>  // For Unix mkdir
    #include <sys/types.h>
#endif

using namespace std;

struct Product {
    string name;
    double price;
    int quantity;
    string category;
};

struct CartItem {
    string productName;
    double price;
    int quantity;
    string category;
};

struct Order {
    string customerUsername;
    vector<CartItem> items;
    double totalAmount;
    string orderDate;
    string status; // "Pending", "Approved", "Delivered", etc.
};

// Add these global variables
string currentUser;
vector<CartItem> shoppingCart;

// Add these function declarations before the implementation
void viewCart();
void addToCart();
void removeFromCart();
void checkout();
void viewOrderHistory(const string& username);
string getCurrentDate();
void saveOrder(const Order& order);
vector<Order> loadOrders(const string& username);
void viewAllOrders();
void updateOrderStatus(const string& adminUsername);
vector<Order> loadAllOrders();

enum UserType { ADMIN = 1, CUSTOMER = 2, STAFF = 3 };

// Function declarations
void signUp(bool isFirstAdmin = false);
bool login();
void adminMenu(const string& adminUsername);
void createStaffAccount(const string& adminUsername);
bool isAdmin(const string& username);
bool isFirstRun();
void adminSignUp(const string& adminUsername);
void staffMenu();
void customerMenu();
void addProduct();
void viewProducts();
void updateProduct();
void saveProduct(const Product& product);
vector<Product> loadProducts();
void adminInventoryMenu();
void deleteProduct();
void addInitialProducts();
void viewCartMenu();
void placeOrder();
void saveToUserProfile(const string& username, const Order& order);
void ensureDirectoryExists();

// Check if directory exists by attempting to open a file in it
bool directoryExists(const string& dirName) {
    if (FILE *file = fopen((dirName + "/.dircheck").c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

// Create directory function that's compatible with older C++ standards
void ensureDirectoryExists() {
    #ifdef _WIN32
        // Windows - ignore return value
        _mkdir("user_orders");
    #else
        // Unix - ignore return value
        mkdir("user_orders", 0777);
    #endif
    // We don't care about errors - if directory exists, that's fine
}

int main() {
    ensureDirectoryExists();
    
    if (isFirstRun()) {
        cout << "Initial setup - Create admin account\n";
        signUp(true);
        return 0;
    }

    int choice;
    cout << "1. Sign Up\n2. Log In\nEnter your choice: ";
    cin >> choice;
    
    if (choice == 1) {
        signUp(false);
    } else if (choice == 2) {
        if (!login()) {
            cout << "Invalid credentials!\n";
        }
    } else {
        cout << "Invalid choice!\n";
    }
    return 0;
}

bool isFirstRun() {
    ifstream file("users.txt");
    return file.peek() == ifstream::traits_type::eof();
}

bool isAdmin(const string& username) {
    ifstream infile("users.txt");
    string u, p;
    int type;
    
    while (infile >> u >> p >> type) {
        if (u == username && type == ADMIN) {
            return true;
        }
    }
    return false;
}

void signUp(bool isFirstAdmin) {
    string username, password;
    int userType;
    
    cout << "Enter a username: ";
    cin >> username;
    cout << "Enter a password: ";
    cin >> password;

    if (isFirstAdmin) {
        userType = ADMIN;
    } else {
        // Regular sign-up is only for customers
        
        userType = CUSTOMER;
        cout << "Creating customer account...\n";
    }

    ofstream outfile("users.txt", ios::app);
    outfile << username << " " << password << " " << userType << "\n";
    outfile.close();

    cout << "Sign-up successful!\n";
}

bool login() {
    string username, password, u, p;
    int storedType;
    
    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    ifstream infile("users.txt");
    while (infile >> u >> p >> storedType) {
        if (u == username && p == password) {
            currentUser = username; // Store the current user
            string userTypeStr;
            switch (storedType) {
                case ADMIN: 
                    userTypeStr = "Admin";
                    cout << "Welcome " << userTypeStr << " " << username << "!\n";
                    adminMenu(username);
                    break;
                case STAFF: 
                    userTypeStr = "Staff";
                    cout << "Welcome " << userTypeStr << " " << username << "!\n";
                    staffMenu();
                    break;
                case CUSTOMER: 
                    userTypeStr = "Customer";
                    cout << "Welcome " << userTypeStr << " " << username << "!\n";
                    customerMenu();
                    break;
            }
            return true;
        }
    }
    return false;
}

void adminMenu(const string& adminUsername) {
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Create new admin account\n";
        cout << "2. Create new staff account\n";
        cout << "3. Manage Inventory\n";
        cout << "4. View All Orders\n";
        cout << "5. Update Order Status\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                adminSignUp(adminUsername);
                break;
            case 2:
                createStaffAccount(adminUsername);
                break;
            case 3:
                adminInventoryMenu();
                break;
            case 4:
                viewAllOrders();
                break;
            case 5:
                updateOrderStatus(adminUsername);
                break;
            case 6:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

void createStaffAccount(const string& adminUsername) {
    if (!isAdmin(adminUsername)) {
        cout << "Only admins can create staff accounts!\n";
        return;
    }

    string newUsername, newPassword;
    cout << "Create new staff account\n";
    cout << "Enter username: ";
    cin >> newUsername;
    cout << "Enter password: ";
    cin >> newPassword;

    ofstream outfile("users.txt", ios::app);
    outfile << newUsername << " " << newPassword << " " << STAFF << "\n";
    outfile.close();

    cout << "New staff account created successfully!\n";
}

void adminSignUp(const string& adminUsername) {
    if (!isAdmin(adminUsername)) {
        cout << "Only admins can create new admin accounts!\n";
        return;
    }

    string newUsername, newPassword;
    cout << "Create new admin account\n";
    cout << "Enter username: ";
    cin >> newUsername;
    cout << "Enter password: ";
    cin >> newPassword;

    ofstream outfile("users.txt", ios::app);
    outfile << newUsername << " " << newPassword << " " << ADMIN << "\n";
    outfile.close();

    cout << "New admin account created successfully!\n";
}

void staffMenu() {
    while (true) {
        cout << "\nStaff Menu:\n";
        cout << "1. Add Product\n";
        cout << "2. View Products\n";
        cout << "3. Update Product\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                addProduct();
                break;
            case 2:
                viewProducts();
                break;
            case 3:
                updateProduct();
                break;
            case 4:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

void customerMenu() {
    // Clear the shopping cart at the start of a new session
    shoppingCart.clear();
    
    while (true) {
        cout << "\nCustomer Menu:\n";
        cout << "1. Browse Products\n";
        cout << "2. View Cart\n";
        cout << "3. Place Order\n";
        cout << "4. Order History\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                viewProducts();
                break;
            case 2:
                viewCartMenu();
                break;
            case 3:
                placeOrder();
                break;
            case 4:
                viewOrderHistory(currentUser);
                break;
            case 5:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

void addProduct() {
    Product product;
    cout << "Enter product name: ";
    cin.ignore();
    getline(cin, product.name);
    cout << "Enter product price: ";
    cin >> product.price;
    cout << "Enter product quantity: ";
    cin >> product.quantity;
    
    cout << "Select category:\n";
    cout << "1. Bakery Products\n";
    cout << "2. Dry Goods\n";
    cout << "3. Snacks\n";
    cout << "4. Household Cleaning\n";
    cout << "5. Paper Products\n";
    cout << "6. Laundry Supplies\n";
    cout << "7. Personal Care\n";
    cout << "8. Baby Food & Kids' Snacks\n";
    cout << "9. Drinks\n";
    cout << "10. Fruits\n";
    cout << "11. Vegetables\n";
    cout << "Enter choice (1-11): ";
    
    int categoryChoice;
    cin >> categoryChoice;
    
    switch(categoryChoice) {
        case 1: product.category = "Bakery Products"; break;
        case 2: product.category = "Dry Goods"; break;
        case 3: product.category = "Snacks"; break;
        case 4: product.category = "Household Cleaning"; break;
        case 5: product.category = "Paper Products"; break;
        case 6: product.category = "Laundry Supplies"; break;
        case 7: product.category = "Personal Care"; break;
        case 8: product.category = "Baby Food & Kids' Snacks"; break;
        case 9: product.category = "Drinks"; break;
        case 10: product.category = "Fruits"; break;
        case 11: product.category = "Vegetables"; break;
        default: product.category = "Other";
    }
    
    saveProduct(product);
    cout << "Product added successfully!\n";
}

void saveProduct(const Product& product) {
    ofstream outfile("products.txt", ios::app);
    outfile << product.name << "|" << product.price << "|" << product.quantity << "|" << product.category << "\n";
    outfile.close();
}

vector<Product> loadProducts() {
    vector<Product> products;
    ifstream infile("products.txt");
    string line;
    
    while (getline(infile, line)) {
        Product product;
        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|", pos1 + 1);
        size_t pos3 = line.find("|", pos2 + 1);
        
        product.name = line.substr(0, pos1);
        product.price = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
        product.quantity = stoi(line.substr(pos2 + 1, pos3 - pos2 - 1));
        product.category = line.substr(pos3 + 1);
        
        products.push_back(product);
    }
    
    return products;
}

void viewProducts() {
    vector<Product> products = loadProducts();
    
    if (products.empty()) {
        cout << "No products available.\n";
        return;
    }
    
    // Get unique categories
    vector<string> categories;
    for (const auto& product : products) {
        bool found = false;
        for (const auto& category : categories) {
            if (category == product.category) {
                found = true;
                break;
            }
        }
        if (!found) {
            categories.push_back(product.category);
        }
    }
    
    while (true) {
        cout << "\nView Products Options:\n";
        cout << "1. View All Products\n";
        cout << "2. View Products by Category\n";
        cout << "3. Add to Cart\n";
        cout << "4. Return to Previous Menu\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            // Display all products with better alignment
            cout << "\nAll Products:\n";
            cout << left << setw(5) << "ID" 
                 << setw(20) << "Category" 
                 << setw(30) << "Name" 
                 << setw(10) << "Price" 
                 << setw(10) << "Quantity" << endl;
            cout << string(75, '-') << endl;
            
            int id = 1;
            for (const auto& product : products) {
                cout << left << setw(5) << id++ 
                     << setw(20) << product.category 
                     << setw(30) << product.name 
                     << setw(10) << product.price 
                     << setw(10) << product.quantity << endl;
            }
        } 
        else if (choice == 2) {
            // Display categories
            cout << "\nSelect Category:\n";
            for (size_t i = 0; i < categories.size(); i++) {
                cout << (i + 1) << ". " << categories[i] << "\n";
            }
            
            cout << "Enter category number: ";
            int catChoice;
            cin >> catChoice;
            
            if (catChoice >= 1 && catChoice <= static_cast<int>(categories.size())) {
                string selectedCategory = categories[catChoice - 1];
                
                cout << "\nProducts in category: " << selectedCategory << "\n";
                cout << left << setw(5) << "ID" 
                     << setw(30) << "Name" 
                     << setw(10) << "Price" 
                     << setw(10) << "Quantity" << endl;
                cout << string(55, '-') << endl;
                
                int id = 1;
                for (const auto& product : products) {
                    if (product.category == selectedCategory) {
                        cout << left << setw(5) << id++ 
                             << setw(30) << product.name 
                             << setw(10) << product.price 
                             << setw(10) << product.quantity << endl;
                    }
                }
            } else {
                cout << "Invalid category selection.\n";
            }
        }
        else if (choice == 3) {
            addToCart();
        }
        else if (choice == 4) {
            return;
        }
        else {
            cout << "Invalid choice.\n";
        }
    }
}

void updateProduct() {
    string productName;
    cout << "Enter product name to update: ";
    cin.ignore();
    getline(cin, productName);
    
    vector<Product> products = loadProducts();
    bool found = false;
    
    for (auto& product : products) {
        if (product.name == productName) {
            found = true;
            cout << "Product found: " << product.name << " (Category: " << product.category << ")\n";
            cout << "Current price: " << product.price << ", Current quantity: " << product.quantity << "\n";
            
            cout << "Enter new price: ";
            cin >> product.price;
            cout << "Enter new quantity: ";
            cin >> product.quantity;
            
            cout << "Do you want to change the category? (y/n): ";
            char choice;
            cin >> choice;
            
            if (choice == 'y' || choice == 'Y') {
                cout << "Select new category:\n";
                cout << "1. Bakery Products\n";
                cout << "2. Dry Goods\n";
                cout << "3. Snacks\n";
                cout << "4. Household Cleaning\n";
                cout << "5. Paper Products\n";
                cout << "6. Laundry Supplies\n";
                cout << "7. Personal Care\n";
                cout << "8. Baby Food & Kids' Snacks\n";
                cout << "9. Drinks\n";
                cout << "10. Fruits\n";
                cout << "11. Vegetables\n";
                cout << "Enter choice (1-11): ";
                
                int categoryChoice;
                cin >> categoryChoice;
                
                switch(categoryChoice) {
                    case 1: product.category = "Bakery Products"; break;
                    case 2: product.category = "Dry Goods"; break;
                    case 3: product.category = "Snacks"; break;
                    case 4: product.category = "Household Cleaning"; break;
                    case 5: product.category = "Paper Products"; break;
                    case 6: product.category = "Laundry Supplies"; break;
                    case 7: product.category = "Personal Care"; break;
                    case 8: product.category = "Baby Food & Kids' Snacks"; break;
                    case 9: product.category = "Drinks"; break;
                    case 10: product.category = "Fruits"; break;
                    case 11: product.category = "Vegetables"; break;
                    default: cout << "Invalid choice, keeping current category.\n";
                }
            }
            break;
        }
    }
    
    if (found) {
        // Save all products back to file
        ofstream outfile("products.txt");
        for (const auto& product : products) {
            outfile << product.name << "|" << product.price << "|" << product.quantity << "|" << product.category << "\n";
        }
        outfile.close();
        
        cout << "Product updated successfully!\n";
    } else {
        cout << "Product not found!\n";
    }
}

void adminInventoryMenu() {
    while (true) {
        cout << "\nInventory Management:\n";
        cout << "1. Add Product\n";
        cout << "2. View Products\n";
        cout << "3. Update Product\n";
        cout << "4. Delete Product\n";
        cout << "5. Add Initial Product List\n";
        cout << "6. Return to Admin Menu\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                addProduct();
                break;
            case 2:
                viewProducts();
                break;
            case 3:
                updateProduct();
                break;
            case 4:
                deleteProduct();
                break;
            case 5:
                addInitialProducts();
                break;
            case 6:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

void deleteProduct() {
    string productName;
    cout << "Enter product name to delete: ";
    cin.ignore();
    getline(cin, productName);
    
    vector<Product> products = loadProducts();
    bool found = false;
    
    ofstream outfile("products.txt");
    for (const auto& product : products) {
        if (product.name != productName) {
            outfile << product.name << "|" << product.price << "|" << product.quantity << "|" << product.category << "\n";
        } else {
            found = true;
            cout << "Deleting: " << product.name << " from category " << product.category << "\n";
        }
    }
    outfile.close();
    
    if (found) {
        cout << "Product deleted successfully!\n";
    } else {
        cout << "Product not found!\n";
    }
}

void addInitialProducts() {
    // First clear any existing products
    ofstream clearFile("products.txt", ios::trunc);
    clearFile.close();
    
    vector<Product> products = {
        // BAKERY PRODUCTS
        {"Bread", 50, 100, "Bakery Products"},
        {"Croissants", 40, 50, "Bakery Products"},
        {"Muffins", 30, 60, "Bakery Products"},
        {"Cakes", 500, 30, "Bakery Products"},
        {"Cookies", 300, 50, "Bakery Products"},
        {"Bagels", 35, 40, "Bakery Products"},
        {"Doughnuts", 40, 40, "Bakery Products"},
        {"Pizza Base", 60, 20, "Bakery Products"},
        {"Puff Pastry", 250, 15, "Bakery Products"},
        {"Brownies", 400, 20, "Bakery Products"},
        {"Buns", 25, 80, "Bakery Products"},
        {"Breadsticks", 70, 30, "Bakery Products"},
        {"Tarts", 450, 10, "Bakery Products"},
        {"Danish Pastries", 50, 20, "Bakery Products"},
        {"Rolls", 25, 40, "Bakery Products"},
        {"Cupcakes", 40, 30, "Bakery Products"},
        {"Flatbread", 60, 25, "Bakery Products"},
        {"Pretzels", 300, 15, "Bakery Products"},
        {"Scones", 30, 20, "Bakery Products"},
        {"Biscuits (Bakery)", 30, 40, "Bakery Products"},
        
        // DRY GOODS
        {"Rice", 70, 200, "Dry Goods"},
        {"Wheat Flour", 40, 150, "Dry Goods"},
        {"Pulses (Lentils)", 80, 100, "Dry Goods"},
        {"Sugar", 50, 100, "Dry Goods"},
        {"Salt", 20, 80, "Dry Goods"},
        {"Dry Spices", 300, 20, "Dry Goods"},
        {"Cornflakes", 150, 100, "Dry Goods"},
        {"Oats", 120, 80, "Dry Goods"},
        {"Pasta", 90, 70, "Dry Goods"},
        {"Noodles", 40, 150, "Dry Goods"},
        {"Semolina", 50, 50, "Dry Goods"},
        {"Barley", 45, 40, "Dry Goods"},
        {"Chickpeas", 80, 60, "Dry Goods"},
        {"Millet", 70, 50, "Dry Goods"},
        {"Quinoa", 250, 30, "Dry Goods"},
        {"Vermicelli", 60, 40, "Dry Goods"},
        {"Breadcrumbs", 150, 30, "Dry Goods"},
        {"Tapioca", 100, 20, "Dry Goods"},
        {"Popcorn Kernels", 80, 40, "Dry Goods"},
        {"Dried Beans", 90, 60, "Dry Goods"},
        
        // SNACKS
        {"Chips", 20, 200, "Snacks"},
        {"Biscuits", 30, 150, "Snacks"},
        {"Namkeen", 200, 80, "Snacks"},
        {"Popcorn", 50, 100, "Snacks"},
        {"Chocolates", 100, 100, "Snacks"},
        {"Candy", 300, 50, "Snacks"},
        {"Energy Bars", 200, 50, "Snacks"},
        {"Nachos", 150, 40, "Snacks"},
        {"Crackers", 70, 60, "Snacks"},
        {"Peanuts", 100, 30, "Snacks"},
        {"Wafers", 20, 100, "Snacks"},
        {"Trail Mix", 250, 30, "Snacks"},
        {"Pretzels (Snack)", 300, 15, "Snacks"},
        {"Salted Cashews", 700, 20, "Snacks"},
        {"Almonds", 800, 50, "Snacks"},
        {"Cheese Puffs", 250, 40, "Snacks"},
        {"Sweet Corn Snack", 30, 30, "Snacks"},
        {"Spicy Mixes", 300, 25, "Snacks"},
        {"Sesame Sticks", 150, 20, "Snacks"},
        {"Granola Bars", 100, 50, "Snacks"},
        
        // HOUSEHOLD (CLEANING SUPPLY)
        {"Detergent (Powder)", 80, 80, "Household Cleaning"},
        {"Detergent (Liquid)", 200, 60, "Household Cleaning"},
        {"Floor Cleaner", 150, 50, "Household Cleaning"},
        {"Dishwashing Liquid", 100, 40, "Household Cleaning"},
        {"Toilet Cleaner", 120, 40, "Household Cleaning"},
        {"Glass Cleaner", 150, 20, "Household Cleaning"},
        {"All-Purpose Cleaner", 180, 30, "Household Cleaning"},
        {"Scrub Pads", 20, 100, "Household Cleaning"},
        {"Dustbin Liners", 50, 150, "Household Cleaning"},
        {"Air Fresheners", 200, 50, "Household Cleaning"},
        {"Sponges", 15, 100, "Household Cleaning"},
        {"Surface Wipes", 80, 100, "Household Cleaning"},
        {"Mops", 500, 40, "Household Cleaning"},
        {"Cleaning Cloths", 120, 60, "Household Cleaning"},
        {"Broomsticks", 150, 50, "Household Cleaning"},
        {"Garbage Bags", 60, 200, "Household Cleaning"},
        {"Bleach", 100, 40, "Household Cleaning"},
        {"Laundry Brushes", 70, 80, "Household Cleaning"},
        {"Toilet Brushes", 200, 100, "Household Cleaning"},
        {"Disinfectants", 150, 40, "Household Cleaning"},
        
        // PAPER PRODUCTS
        {"Toilet Paper", 30, 200, "Paper Products"},
        {"Tissues", 20, 150, "Paper Products"},
        {"Paper Towels", 40, 120, "Paper Products"},
        {"Napkins", 25, 80, "Paper Products"},
        {"Hand Towels", 70, 50, "Paper Products"},
        {"Table Mats (Paper)", 150, 40, "Paper Products"},
        {"Paper Plates", 50, 100, "Paper Products"},
        {"Paper Cups", 40, 100, "Paper Products"},
        {"Wrapping Paper", 70, 60, "Paper Products"},
        {"Baking Paper", 80, 50, "Paper Products"},
        {"Toilet Seat Covers", 100, 40, "Paper Products"},
        {"Cardstock Sheets", 150, 30, "Paper Products"},
        {"Disposable Paper Masks", 50, 100, "Paper Products"},
        {"Sticky Notes", 30, 150, "Paper Products"},
        {"Envelopes", 40, 80, "Paper Products"},
        {"Paper Trays", 100, 50, "Paper Products"},
        {"Food Wraps", 120, 40, "Paper Products"},
        {"Cartons", 150, 60, "Paper Products"},
        {"Kraft Paper", 100, 50, "Paper Products"},
        {"Paper Gift Bags", 80, 70, "Paper Products"},
        
        // LAUNDRY SUPPLIES
        {"Laundry Detergent (Powder)", 90, 80, "Laundry Supplies"},
        {"Laundry Detergent (Liquid)", 250, 60, "Laundry Supplies"},
        {"Fabric Softener", 200, 40, "Laundry Supplies"},
        {"Bleach (Laundry)", 120, 40, "Laundry Supplies"},
        {"Laundry Stain Remover", 150, 20, "Laundry Supplies"},
        {"Detergent Pods", 300, 40, "Laundry Supplies"},
        {"Laundry Bars", 70, 80, "Laundry Supplies"},
        {"Oxygen Bleach Powder", 250, 25, "Laundry Supplies"},
        {"Fragrance Boosters", 400, 30, "Laundry Supplies"},
        {"Laundry Hampers", 500, 50, "Laundry Supplies"},
        {"Drying Racks", 1000, 30, "Laundry Supplies"},
        {"Washing Machine Cleaner", 250, 40, "Laundry Supplies"},
        {"Starch Sprays", 150, 30, "Laundry Supplies"},
        {"Laundry Nets", 150, 60, "Laundry Supplies"},
        {"Fabric Shaver", 300, 30, "Laundry Supplies"},
        {"Lint Rollers", 50, 100, "Laundry Supplies"},
        {"Color Catcher Sheets", 200, 40, "Laundry Supplies"},
        {"Washing Balls", 150, 30, "Laundry Supplies"},
        {"Anti-Microbial Additives", 300, 20, "Laundry Supplies"},
        
        // PERSONAL CARE
        {"Shampoo", 200, 80, "Personal Care"},
        {"Conditioner", 250, 60, "Personal Care"},
        {"Soap", 50, 200, "Personal Care"},
        {"Face Wash", 300, 100, "Personal Care"},
        {"Moisturizer", 400, 50, "Personal Care"},
        {"Lip Balm", 100, 120, "Personal Care"},
        {"Deodorant", 250, 80, "Personal Care"},
        {"Body Lotion", 350, 50, "Personal Care"},
        {"Hair Oil", 200, 100, "Personal Care"},
        {"Face Masks", 150, 60, "Personal Care"},
        {"Hair Serums", 400, 40, "Personal Care"},
        {"Hand Sanitizer", 100, 100, "Personal Care"},
        {"Eye Cream", 500, 30, "Personal Care"},
        {"Nail Polish", 150, 80, "Personal Care"},
        {"Makeup Remover", 300, 50, "Personal Care"},
        {"Body Wash", 200, 80, "Personal Care"},
        {"Sunscreen", 350, 50, "Personal Care"},
        {"Hair Spray", 400, 40, "Personal Care"},
        {"Perfume", 600, 30, "Personal Care"},
        {"Beard Oil", 300, 40, "Personal Care"},
        
        // BABY FOOD & KIDS' SNACKS
        {"Baby Cereal", 200, 100, "Baby Food & Kids' Snacks"},
        {"Infant Formula", 500, 50, "Baby Food & Kids' Snacks"},
        {"Baby Food Pouches", 150, 80, "Baby Food & Kids' Snacks"},
        {"Baby Snacks", 100, 60, "Baby Food & Kids' Snacks"},
        {"Kids' Juice Boxes", 60, 100, "Baby Food & Kids' Snacks"},
        {"Yogurt for Babies", 120, 40, "Baby Food & Kids' Snacks"},
        {"Fruit Purees", 180, 50, "Baby Food & Kids' Snacks"},
        {"Rice Rusks", 150, 70, "Baby Food & Kids' Snacks"},
        {"Teething Biscuits", 100, 60, "Baby Food & Kids' Snacks"},
        {"Milk Powder", 400, 40, "Baby Food & Kids' Snacks"},
        {"Oatmeal for Babies", 150, 60, "Baby Food & Kids' Snacks"},
        {"Kids' Crackers", 50, 80, "Baby Food & Kids' Snacks"},
        {"Mini Muffins", 70, 50, "Baby Food & Kids' Snacks"},
        {"Kids' Popcorn", 50, 50, "Baby Food & Kids' Snacks"},
        {"Vegetable Puffs", 80, 40, "Baby Food & Kids' Snacks"},
        {"Small Cheese Cubes", 150, 30, "Baby Food & Kids' Snacks"},
        {"Baby Smoothies", 150, 50, "Baby Food & Kids' Snacks"},
        {"Kids' Granola Bars", 100, 60, "Baby Food & Kids' Snacks"},
        {"Kids' Chocolates", 100, 50, "Baby Food & Kids' Snacks"},
        {"Baby Porridge", 200, 40, "Baby Food & Kids' Snacks"},
        
        // DRINKS
        {"Cold Drinks (Soda)", 50, 500, "Drinks"},
        {"Juice (Packaged)", 100, 300, "Drinks"},
        {"Energy Drinks", 120, 150, "Drinks"},
        {"Tea", 400, 100, "Drinks"},
        {"Coffee", 500, 80, "Drinks"},
        {"Milkshakes", 100, 50, "Drinks"},
        {"Hot Chocolate", 600, 30, "Drinks"},
        {"Mineral Water", 20, 500, "Drinks"},
        {"Flavored Water", 40, 200, "Drinks"},
        {"Smoothies", 150, 80, "Drinks"},
        {"Lemonade", 50, 100, "Drinks"},
        {"Coconut Water", 40, 150, "Drinks"},
        {"Iced Tea", 80, 100, "Drinks"},
        {"Herbal Teas", 400, 50, "Drinks"},
        {"Espresso Shots", 200, 50, "Drinks"},
        {"Protein Shakes", 150, 80, "Drinks"},
        {"Green Tea", 500, 70, "Drinks"},
        {"Fruit Punch", 100, 40, "Drinks"},
        {"Tonic Water", 70, 60, "Drinks"},
        {"Kombucha", 200, 40, "Drinks"},
        
        // FRUITS
        {"Apple", 150, 10, "Fruits"},
        {"Banana", 50, 20, "Fruits"},
        {"Orange", 80, 15, "Fruits"},
        {"Mango", 120, 12, "Fruits"},
        {"Grapes", 90, 10, "Fruits"},
        {"Pineapple", 100, 8, "Fruits"},
        {"Watermelon", 40, 25, "Fruits"},
        {"Papaya", 60, 18, "Fruits"},
        {"Strawberry", 250, 5, "Fruits"},
        {"Guava", 70, 10, "Fruits"},
        {"Blueberry", 400, 3, "Fruits"},
        {"Pear", 130, 8, "Fruits"},
        {"Peach", 140, 6, "Fruits"},
        {"Cherry", 300, 5, "Fruits"},
        {"Kiwi", 200, 4, "Fruits"},
        {"Pomegranate", 150, 9, "Fruits"},
        {"Avocado", 250, 5, "Fruits"},
        {"Lychee", 180, 7, "Fruits"},
        {"Dragon Fruit", 350, 3, "Fruits"},
        {"Raspberry", 400, 4, "Fruits"},
        
        // VEGETABLES
        {"Tomato", 30, 20, "Vegetables"},
        {"Potato", 20, 25, "Vegetables"},
        {"Carrot", 40, 15, "Vegetables"},
        {"Broccoli", 120, 5, "Vegetables"},
        {"Spinach", 30, 10, "Vegetables"},
        {"Cauliflower", 50, 8, "Vegetables"},
        {"Cucumber", 30, 12, "Vegetables"},
        {"Eggplant", 40, 15, "Vegetables"},
        {"Green Beans", 70, 10, "Vegetables"},
        {"Onion", 25, 20, "Vegetables"},
        {"Peas", 60, 8, "Vegetables"},
        {"Bell Pepper", 80, 7, "Vegetables"},
        {"Zucchini", 70, 6, "Vegetables"},
        {"Pumpkin", 30, 10, "Vegetables"},
        {"Radish", 20, 12, "Vegetables"},
        {"Bitter Gourd", 50, 8, "Vegetables"},
        {"Turnip", 40, 9, "Vegetables"},
        {"Cabbage", 30, 10, "Vegetables"},
        {"Lettuce", 50, 5, "Vegetables"},
        {"Beetroot", 40, 8, "Vegetables"}
    };
    
    for (const auto& product : products) {
        saveProduct(product);
    }
    
    cout << "Added " << products.size() << " products to inventory successfully!\n";
}

void viewAllOrders() {
    vector<Order> orders = loadAllOrders();
    
    if (orders.empty()) {
        cout << "No orders in the system.\n";
        return;
    }
    
    cout << "\nAll Orders:\n";
    for (size_t i = 0; i < orders.size(); i++) {
        cout << "\n" << string(70, '=') << endl;
        cout << "Order #" << (i + 1) << endl;
        cout << "Customer: " << orders[i].customerUsername << endl;
        cout << "Date: " << orders[i].orderDate << endl;
        cout << "Status: " << orders[i].status << endl;
        cout << "Total: ₹" << fixed << setprecision(2) << orders[i].totalAmount << endl;
        cout << string(70, '-') << endl;
        
        cout << left << setw(25) << "Item" 
             << setw(15) << "Category" 
             << setw(6) << "Qty" 
             << setw(8) << "Price" 
             << right << setw(10) << "Amount" << endl;
        cout << string(70, '-') << endl;
        
        for (const auto& item : orders[i].items) {
            double amount = item.price * item.quantity;
            cout << left << setw(25) << item.productName 
                 << setw(15) << item.category 
                 << setw(6) << item.quantity 
                 << setw(8) << item.price 
                 << right << setw(10) << fixed << setprecision(2) << amount << endl;
        }
        
        cout << string(70, '=') << endl;
    }
}

void updateOrderStatus(const string& adminUsername) {
    if (!isAdmin(adminUsername)) {
        cout << "Only admins can update order status!\n";
        return;
    }
    
    vector<Order> orders = loadAllOrders();
    
    if (orders.empty()) {
        cout << "No orders in the system.\n";
        return;
    }
    
    cout << "\nSelect Order to Update:\n";
    for (size_t i = 0; i < orders.size(); i++) {
        cout << (i + 1) << ". Customer: " << orders[i].customerUsername 
             << ", Date: " << orders[i].orderDate 
             << ", Status: " << orders[i].status 
             << ", Total: ₹" << orders[i].totalAmount << "\n";
    }
    
    cout << "Enter order number: ";
    int choice;
    cin >> choice;
    
    if (choice < 1 || choice > static_cast<int>(orders.size())) {
        cout << "Invalid selection.\n";
        return;
    }
    
    cout << "Current status: " << orders[choice - 1].status << "\n";
    cout << "Select new status:\n";
    cout << "1. Pending\n";
    cout << "2. Approved\n";
    cout << "3. In Process\n";
    cout << "4. Shipped\n";
    cout << "5. Delivered\n";
    cout << "6. Cancelled\n";
    cout << "Enter choice: ";
    
    int statusChoice;
    cin >> statusChoice;
    
    string newStatus;
    switch (statusChoice) {
        case 1: newStatus = "Pending"; break;
        case 2: newStatus = "Approved"; break;
        case 3: newStatus = "In Process"; break;
        case 4: newStatus = "Shipped"; break;
        case 5: newStatus = "Delivered"; break;
        case 6: newStatus = "Cancelled"; break;
        default: 
            cout << "Invalid choice.\n";
            return;
    }
    
    orders[choice - 1].status = newStatus;
    
    // Save all orders back to file
    ofstream outfile("orders.txt");
    for (const auto& order : orders) {
        outfile << order.customerUsername << "|"
                << order.orderDate << "|"
                << order.status << "|"
                << order.totalAmount;
        
        for (const auto& item : order.items) {
            outfile << "|" << item.productName << "," 
                    << item.quantity << "," 
                    << item.price << "," 
                    << item.category;
        }
        
        outfile << "\n";
    }
    outfile.close();
    
    cout << "Order status updated successfully!\n";
}

vector<Order> loadAllOrders() {
    vector<Order> orders;
    ifstream infile("orders.txt");
    string line;
    
    while (getline(infile, line)) {
        size_t pos1 = line.find("|");
        string orderUsername = line.substr(0, pos1);
        
        Order order;
        order.customerUsername = orderUsername;
        
        size_t pos2 = line.find("|", pos1 + 1);
        order.orderDate = line.substr(pos1 + 1, pos2 - pos1 - 1);
        
        size_t pos3 = line.find("|", pos2 + 1);
        order.status = line.substr(pos2 + 1, pos3 - pos2 - 1);
        
        size_t pos4 = line.find("|", pos3 + 1);
        order.totalAmount = stod(line.substr(pos3 + 1, pos4 - pos3 - 1));
        
        // Parse items
        string itemsStr = line.substr(pos4 + 1);
        size_t itemPos = 0;
        while ((itemPos = itemsStr.find("|")) != string::npos) {
            string itemData = itemsStr.substr(0, itemPos);
            
            // Parse individual item data
            size_t comma1 = itemData.find(",");
            size_t comma2 = itemData.find(",", comma1 + 1);
            size_t comma3 = itemData.find(",", comma2 + 1);
            
            CartItem item;
            item.productName = itemData.substr(0, comma1);
            item.quantity = stoi(itemData.substr(comma1 + 1, comma2 - comma1 - 1));
            item.price = stod(itemData.substr(comma2 + 1, comma3 - comma2 - 1));
            item.category = itemData.substr(comma3 + 1);
            
            order.items.push_back(item);
            
            // Move to next item
            itemsStr = itemsStr.substr(itemPos + 1);
        }
        
        // Process the last item if there's no trailing pipe
        if (!itemsStr.empty()) {
            size_t comma1 = itemsStr.find(",");
            size_t comma2 = itemsStr.find(",", comma1 + 1);
            size_t comma3 = itemsStr.find(",", comma2 + 1);
            
            CartItem item;
            item.productName = itemsStr.substr(0, comma1);
            item.quantity = stoi(itemsStr.substr(comma1 + 1, comma2 - comma1 - 1));
            item.price = stod(itemsStr.substr(comma2 + 1, comma3 - comma2 - 1));
            item.category = itemsStr.substr(comma3 + 1);
            
            order.items.push_back(item);
        }
        
        orders.push_back(order);
    }
    
    return orders;
}

void viewOrderHistory(const string& username) {
    vector<Order> orders = loadOrders(username);
    
    if (orders.empty()) {
        cout << "You have no order history.\n";
        return;
    }
    
    cout << "\nOrder History:\n";
    for (size_t i = 0; i < orders.size(); i++) {
        cout << "\n" << string(60, '=') << endl;
        cout << "Order #" << (i + 1) << endl;
        cout << "Date: " << orders[i].orderDate << endl;
        cout << "Status: " << orders[i].status << endl;
        cout << "Total: ₹" << fixed << setprecision(2) << orders[i].totalAmount << endl;
        cout << string(60, '-') << endl;
        
        cout << left << setw(25) << "Item" 
             << setw(15) << "Category" 
             << setw(6) << "Qty" 
             << setw(8) << "Price" 
             << right << setw(10) << "Amount" << endl;
        cout << string(60, '-') << endl;
        
        for (const auto& item : orders[i].items) {
            double amount = item.price * item.quantity;
            cout << left << setw(25) << item.productName 
                 << setw(15) << item.category 
                 << setw(6) << item.quantity 
                 << setw(8) << item.price 
                 << right << setw(10) << fixed << setprecision(2) << amount << endl;
        }
        
        cout << string(60, '=') << endl;
    }
}

void addToCart() {
    string productName;
    cout << "Enter product name to add to cart: ";
    cin.ignore();
    getline(cin, productName);
    
    vector<Product> products = loadProducts();
    bool found = false;
    
    for (const auto& product : products) {
        if (product.name == productName) {
            found = true;
            
            if (product.quantity <= 0) {
                cout << "Sorry, this product is out of stock.\n";
                return;
            }
            
            int quantity;
            cout << "Available quantity: " << product.quantity << "\n";
            cout << "Enter quantity to add to cart: ";
            cin >> quantity;
            
            if (quantity <= 0) {
                cout << "Invalid quantity.\n";
                return;
            }
            
            if (quantity > product.quantity) {
                cout << "Not enough stock available. Maximum available: " << product.quantity << "\n";
                return;
            }
            
            // Check if product already in cart
            bool inCart = false;
            for (auto& item : shoppingCart) {
                if (item.productName == productName) {
                    inCart = true;
                    item.quantity += quantity;
                    cout << "Updated quantity in cart.\n";
                    break;
                }
            }
            
            if (!inCart) {
                CartItem newItem;
                newItem.productName = product.name;
                newItem.price = product.price;
                newItem.quantity = quantity;
                newItem.category = product.category;
                shoppingCart.push_back(newItem);
                cout << "Product added to cart.\n";
            }
            
            break;
        }
    }
    
    if (!found) {
        cout << "Product not found.\n";
    }
}

void viewCart() {
    if (shoppingCart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }
    
    double total = 0;
    cout << "\nShopping Cart:\n";
    cout << left << setw(30) << "Product" 
         << setw(20) << "Category" 
         << setw(10) << "Price" 
         << setw(10) << "Quantity" 
         << setw(15) << "Subtotal" << endl;
    cout << string(85, '-') << endl;
    
    for (const auto& item : shoppingCart) {
        double subtotal = item.price * item.quantity;
        total += subtotal;
        cout << left << setw(30) << item.productName 
             << setw(20) << item.category 
             << setw(10) << item.price 
             << setw(10) << item.quantity 
             << setw(15) << subtotal << endl;
    }
    
    cout << string(85, '-') << endl;
    cout << right << setw(70) << "Total: ₹" << total << endl;
}

void removeFromCart() {
    if (shoppingCart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }
    
    cout << "\nYour Cart:\n";
    for (size_t i = 0; i < shoppingCart.size(); i++) {
        cout << (i + 1) << ". " << shoppingCart[i].productName 
             << " (Quantity: " << shoppingCart[i].quantity << ")\n";
    }
    
    cout << "Enter item number to remove (1-" << shoppingCart.size() << "): ";
    int choice;
    cin >> choice;
    
    if (choice < 1 || choice > static_cast<int>(shoppingCart.size())) {
        cout << "Invalid selection.\n";
        return;
    }
    
    cout << "Remove how many? (1-" << shoppingCart[choice-1].quantity << "): ";
    int quantity;
    cin >> quantity;
    
    if (quantity < 1 || quantity > shoppingCart[choice-1].quantity) {
        cout << "Invalid quantity.\n";
        return;
    }
    
    if (quantity == shoppingCart[choice-1].quantity) {
        // Remove the entire item
        shoppingCart.erase(shoppingCart.begin() + choice - 1);
        cout << "Item removed from cart.\n";
    } else {
        // Reduce the quantity
        shoppingCart[choice-1].quantity -= quantity;
        cout << "Updated quantity in cart.\n";
    }
}

string getCurrentDate() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return string(buffer);
}

void saveOrder(const Order& order) {
    ofstream outfile("orders.txt", ios::app);
    
    // Format: username|date|status|totalAmount|item1,qty1,price1,category1|item2,qty2,price2,category2|...
    outfile << order.customerUsername << "|"
            << order.orderDate << "|"
            << order.status << "|"
            << order.totalAmount;
    
    for (const auto& item : order.items) {
        outfile << "|" << item.productName << "," 
                << item.quantity << "," 
                << item.price << "," 
                << item.category;
    }
    
    outfile << "\n";
    outfile.close();
}

void checkout() {
    if (shoppingCart.empty()) {
        cout << "Your cart is empty. Nothing to checkout.\n";
        return;
    }
    
    viewCart();
    cout << "\nProceed to checkout? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice != 'y' && choice != 'Y') {
        cout << "Checkout cancelled.\n";
        return;
    }
    
    // Calculate total
    double total = 0;
    for (const auto& item : shoppingCart) {
        total += item.price * item.quantity;
    }
    
    // Create order
    Order newOrder;
    newOrder.customerUsername = currentUser;
    newOrder.items = shoppingCart;
    newOrder.totalAmount = total;
    newOrder.orderDate = getCurrentDate();
    newOrder.status = "Pending";
    
    // Save order
    saveOrder(newOrder);
    
    // Update inventory
    vector<Product> products = loadProducts();
    for (const auto& cartItem : shoppingCart) {
        for (auto& product : products) {
            if (product.name == cartItem.productName) {
                product.quantity -= cartItem.quantity;
                break;
            }
        }
    }
    
    // Save updated inventory
    ofstream outfile("products.txt");
    for (const auto& product : products) {
        outfile << product.name << "|" << product.price << "|" 
                << product.quantity << "|" << product.category << "\n";
    }
    outfile.close();
    
    cout << "Order placed successfully!\n";
    cout << "Order total: ₹" << total << "\n";
    cout << "Order status: " << newOrder.status << "\n";
    
    // Clear the cart
    shoppingCart.clear();
}

void viewCartMenu() {
    while (true) {
        viewCart();
        
        cout << "\nCart Options:\n";
        cout << "1. Remove items from cart\n";
        cout << "2. Return to Customer Menu\n";
        cout << "Enter choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                removeFromCart();
                break;
            case 2:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }
}

void placeOrder() {
    if (shoppingCart.empty()) {
        cout << "Your cart is empty. Nothing to order.\n";
        return;
    }
    
    // Show the cart contents first
    viewCart();
    
    cout << "\nProceed to checkout? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice != 'y' && choice != 'Y') {
        cout << "Order cancelled.\n";
        return;
    }
    
    // Calculate total
    double total = 0;
    for (const auto& item : shoppingCart) {
        total += item.price * item.quantity;
    }
    
    // Create order
    Order newOrder;
    newOrder.customerUsername = currentUser;
    newOrder.items = shoppingCart;
    newOrder.totalAmount = total;
    newOrder.orderDate = getCurrentDate();
    newOrder.status = "Pending";
    
    // Generate and display bill with better formatting
    cout << "\n" << string(60, '=') << endl;
    cout << setw(35) << right << "INVOICE" << endl;
    cout << string(60, '=') << endl;
    cout << "Order Date: " << newOrder.orderDate << endl;
    cout << "Customer: " << currentUser << endl;
    cout << string(60, '-') << endl;
    
    cout << left << setw(25) << "Item" 
         << setw(15) << "Category" 
         << setw(6) << "Qty" 
         << setw(8) << "Price" 
         << right << setw(10) << "Amount" << endl;
    cout << string(60, '-') << endl;
    
    for (const auto& item : shoppingCart) {
        double amount = item.price * item.quantity;
        cout << left << setw(25) << item.productName 
             << setw(15) << item.category 
             << setw(6) << item.quantity 
             << setw(8) << item.price 
             << right << setw(10) << fixed << setprecision(2) << amount << endl;
    }
    
    cout << string(60, '-') << endl;
    cout << left << setw(50) << "Total Amount: " 
         << right << setw(10) << fixed << setprecision(2) << total << endl;
    cout << string(60, '-') << endl;
    cout << "Status: " << newOrder.status << endl;
    cout << "\nThank you for shopping with us!" << endl;
    cout << string(60, '=') << endl;
    
    // Save order
    saveOrder(newOrder);
    
    // Save to user profile
    saveToUserProfile(currentUser, newOrder);
    
    // Update inventory
    vector<Product> products = loadProducts();
    for (const auto& cartItem : shoppingCart) {
        for (auto& product : products) {
            if (product.name == cartItem.productName) {
                product.quantity -= cartItem.quantity;
                break;
            }
        }
    }
    
    // Save updated inventory
    ofstream outfile("products.txt");
    for (const auto& product : products) {
        outfile << product.name << "|" << product.price << "|" 
                << product.quantity << "|" << product.category << "\n";
    }
    outfile.close();
    
    cout << "Order placed successfully! Your order will be processed soon.\n";
    
    // Clear the cart
    shoppingCart.clear();
}

void saveToUserProfile(const string& username, const Order& order) {
    // Just use a simple filename pattern in the current directory
    string filename = username + "_orders.txt";
    ofstream outfile(filename, ios::app);
    
    // Format: date|status|totalAmount|item1,qty1,price1,category1|...
    outfile << order.orderDate << "|"
            << order.status << "|"
            << order.totalAmount;
    
    for (const auto& item : order.items) {
        outfile << "|" << item.productName << "," 
                << item.quantity << "," 
                << item.price << "," 
                << item.category;
    }
    
    outfile << "\n";
    outfile.close();
}

vector<Order> loadOrders(const string& username) {
    vector<Order> orders;
    ifstream infile("orders.txt");
    string line;
    
    while (getline(infile, line)) {
        size_t pos1 = line.find("|");
        string orderUsername = line.substr(0, pos1);
        
        if (orderUsername == username) {
            Order order;
            order.customerUsername = orderUsername;
            
            size_t pos2 = line.find("|", pos1 + 1);
            order.orderDate = line.substr(pos1 + 1, pos2 - pos1 - 1);
            
            size_t pos3 = line.find("|", pos2 + 1);
            order.status = line.substr(pos2 + 1, pos3 - pos2 - 1);
            
            size_t pos4 = line.find("|", pos3 + 1);
            order.totalAmount = stod(line.substr(pos3 + 1, pos4 - pos3 - 1));
            
            // Parse items
            string itemsStr = line.substr(pos4 + 1);
            size_t itemPos = 0;
            while ((itemPos = itemsStr.find("|")) != string::npos) {
                string itemData = itemsStr.substr(0, itemPos);
                
                // Parse individual item data
                size_t comma1 = itemData.find(",");
                size_t comma2 = itemData.find(",", comma1 + 1);
                size_t comma3 = itemData.find(",", comma2 + 1);
                
                CartItem item;
                item.productName = itemData.substr(0, comma1);
                item.quantity = stoi(itemData.substr(comma1 + 1, comma2 - comma1 - 1));
                item.price = stod(itemData.substr(comma2 + 1, comma3 - comma2 - 1));
                item.category = itemData.substr(comma3 + 1);
                
                order.items.push_back(item);
                
                // Move to next item
                itemsStr = itemsStr.substr(itemPos + 1);
            }
            
            // Process the last item if there's no trailing pipe
            if (!itemsStr.empty()) {
                size_t comma1 = itemsStr.find(",");
                size_t comma2 = itemsStr.find(",", comma1 + 1);
                size_t comma3 = itemsStr.find(",", comma2 + 1);
                
                CartItem item;
                item.productName = itemsStr.substr(0, comma1);
                item.quantity = stoi(itemsStr.substr(comma1 + 1, comma2 - comma1 - 1));
                item.price = stod(itemsStr.substr(comma2 + 1, comma3 - comma2 - 1));
                item.category = itemsStr.substr(comma3 + 1);
                
                order.items.push_back(item);
            }
            
            orders.push_back(order);
        }
    }
    
    return orders;
}