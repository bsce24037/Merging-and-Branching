#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;

// exceptional handling line 353

// ----------------------------------Exception classes-----------------------------------
namespace Banking
{
    namespace Exceptions
    {
        class FileException : public std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };
    
        class AccountException : public std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };
        
        class TransactionException : public std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };
    }


// -----------------------------structure for customer personal information-------------------

struct PersonalInfo
{
    string name;
    string dob;
    string cnic;
    string address;
    time_t openingDate;
    
    string getFormattedOpeningDate() const
    {
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&openingDate));
        return string(buffer);
    }

    // Operator overloading for <<
    friend ostream& operator<<(ostream& os, const PersonalInfo& info)
    {
        os << "Name: " << info.name << "\n"
           << "DOB: " << info.dob << "\n"
           << "CNIC: " << info.cnic << "\n"
           << "Address: " << info.address << "\n"
           << "Account Opening Date: " << ctime(&info.openingDate);
           return os;
        }
    };

    using namespace Banking;

    //----------------------------- Forward declarations---------------------------------------


template<typename B> class Bank;  // Forward declaration of Bank class
template<typename B> class BankAccount;
template<typename B> class SavingAccount;
template<typename B> class BusinessAccount;
class Loan;
class BankMember;


//=================================== Class of user ===============================

class User
{
    public:
        string username;
        string password;
        string role; // "admin" or "customer"
        string associatedAccount; // for customers
    
        User() = default; // Default constructor
        // Parameterized constructor
        User(const string& uname, const string& pwd, const string& r, const string& acc);
        // Getters
        string getUsername() const;
        string getRole() const;
        string getAssociatedAccount() const;
    
        // Verify password
        bool verifyPassword(const string& pwd) const;
    };

 // ===========================Services class friend with Saving and Business Account=========================
 class Services
 {
 private:
     map<string, vector<string>> availableServices;  // Changed from 'services' to 'availableServices'
     map<string, bool> serviceStatus;
     
 public:
     Services() {
         availableServices["Saving"] = {"Mobile Banking", "Online Banking", "ATM Access", 
                                      "Debit Card", "Credit Card", "Investment Advisory"};
         availableServices["Business"] = {"Business Online Banking", "Merchant Services", 
                                        "Business Credit Card", "Payroll Services", 
                                        "Commercial Loans"};
         
         // Initialize all services as inactive
         for (const auto& pair : availableServices) {
             for (const auto& service : pair.second) {
                 serviceStatus[service] = false;
             }
         }
     }
 
     template<typename B>
     void activatePremiumFeatures(SavingAccount<B>& acc) {
         if (acc.getBalance() >= 50000) { // Example condition
             cout << "Premium Features Activated for Account: " << acc.getAccountNumber() << endl;
             serviceStatus["Saving-Credit Card"] = true;
             serviceStatus["Saving-Investment Advisory"] = true;
         } else {
             cout << "Account " << acc.getAccountNumber() << " does not qualify for Premium Features." << endl;
         }
     }
     
     template<typename B>
     void activateAllServices(BankAccount<B>* acc) {
         string type = acc->accountType();
         for (const auto& service : availableServices[type]) {
             serviceStatus[service] = true;
         }
     }
 
     void displayServices(const string& accType) const {
         const map<string, vector<string>> services = {
             {"Saving", {"Mobile Banking", "Zakat Calculation", "ATM Card"}},
             {"Business", {"Merchant Services", "Business Loans"}}
         };
 
         if (services.count(accType)) {
             cout << "\nAvailable Services (" << accType << "):\n";
             for (const auto& service : services.at(accType)) {
                 cout << "- " << service << "\n";
             }
         } else {
             cout << "No services available for this account type\n";
         }
     }
 };



//--------------------------------------- Bank Account template class-------------------------
template<typename B>
class BankAccount
{
protected:
 B balance;
 string accountNumber;
 PersonalInfo customerInfo;              // Composition with PersonalInfo struct

public:
// Default constructor
 BankAccount();
 // parametarized constructor
 BankAccount(string accountNum, B balance, PersonalInfo info);
 
 //----- Copy constructor (deep copy)------
 BankAccount(const BankAccount& other);

 virtual ~BankAccount() {}               // Virtual destructor for proper cleanup

 // Setters functions:
 void setAccountNumber(const string& accountNum);
 void setBalance(B balance);
 void setCustomerInfo(const PersonalInfo& info);

 // Getters functions:
 string getAccountNumber() const;
 B getBalance() const;
 PersonalInfo getCustomerInfo() const;

 // Member virtaul functions:
 virtual void displayAccountInfo() const;
 virtual B updateBalance(B amount);
 virtual B withdraw(B amount);
 
 // Declare friend function
 template<typename T>
 friend bool verifyTransaction(const BankAccount<T>& acc, double amount);

 virtual string accountType() = 0;  // Pure virtual function
};

// Friend function declaration
template<typename B>
bool verifyTransaction(const BankAccount<B>& acc, double amount);



// =============================Bank Member class=====================================
class BankMember
{
private:
 string employeeID;
 string name;
 string designation;
 double salary;
 string accountNumber;
 
public:
 BankMember(const string& id, const string& n, const string& design,
            double sal, const string& account);
 
 // Getters
 string getEmployeeID() const;
 string getName() const;
 string getDesignation() const;
 double getSalary() const;
 string getAccountNumber() const;
 
 // Setters
 void setSalary(double sal);
 void setAccountNumber(const string& account);
 
 // Pay salary to the member's account
 template<typename B>
 void paySalary(Bank<B>* bank);
};




// -----------------------------------------Bank class(Singleton)-----------------------------
template<typename B>
class Bank
{
private:
 static Bank* instance;
 vector<BankAccount<B>*> accounts;
 map<string, BankAccount<B>*> accountMap;  // Using map for fast access
 string filename = "accounts.json";         // json file to store accounts data
 vector<BankMember> employees;
 string employeesFile = "employees.json";   // json file to store Employee data
 map<string, User> users;                // username -> User
 string usersFile = "users.json";                // json file to store user data
 
 // Private constructor
 Bank()
 {
    // Exception handling for file loading
    // Load accounts, employees, and users from files
    try {
        loadAccountsFromFile();
        loadEmployeesFromFile();
        loadUsersFromFile();
    } catch (const Exceptions::FileException& e) {
        cerr << "Initialization error: " << e.what() << endl;
    }
}
 
public:
 // Static function to get instance
 static Bank* getInstance()
 {
     if (!instance)
     {
         instance = new Bank();
     }
     return instance;
 }
 
 // Destructor to clean up dynamically allocated accounts
 ~Bank()
 {
     for (auto* acc : accounts)
     {
         delete acc;
     }
 }
 // Function to get account
 const vector<BankAccount<B>*>& getAccounts() const { return accounts; }
 const map<string, User>& getUsers() const { return users; }
 // Add account with map
 void addAccount(BankAccount<B>* acc)
 {
     accounts.push_back(acc);
     accountMap[acc->getAccountNumber()] = acc;
     saveAccountsToFile();
 }

 // Find account using map
 BankAccount<B>* findAccount(const string& accNum)
 {
     auto it = accountMap.find(accNum);
     return (it != accountMap.end()) ? it->second : nullptr;
 }

 // Remove account
 void removeAccount(const string& accNum)
 {
     auto it = accountMap.find(accNum);
     if (it != accountMap.end())
     {
         delete it->second;
         accountMap.erase(it);
         
         // Remove from vector
         accounts.erase(
             remove_if(accounts.begin(), accounts.end(),
                 [accNum](BankAccount<B>* acc) { 
                     return acc->getAccountNumber() == accNum; 
                 }),
                 accounts.end());
                 
                 saveAccountsToFile();
             }
         }
         
         // Create account (factory method)
         BankAccount<B>* createAccount(const string& accNum, B balance, const string& type, PersonalInfo info)
         {
             BankAccount<B>* acc = nullptr;
                // Exception handling for account creation
             if (accNum.empty()) {
                throw Exceptions::AccountException("Account number cannot be empty");
            }
            // Check if account number already exists
            if (balance < 0) {
                throw Exceptions::AccountException("Initial balance cannot be negative");
            }
            // Check if account already exists

             if (type == "Saving")
             {
                 acc = new SavingAccount<B>(accNum, balance, info, 0.0, 0, true);
     }
     else if (type == "Business")
     {
         acc = new BusinessAccount<B>(accNum, balance, info, "LinkedSystem");
     }
     else
     {
         cout << "Invalid account type!" << endl;
         return nullptr;
     }
     // Add account to map and vector
     if (acc) addAccount(acc);
     return acc;
 }
 
 // Save accounts to JSON file
 // Exception handling for file operations
 void saveAccountsToFile() {
    try
    {
        json j;
        for (auto* acc : accounts)
        {
         char buffer[80];
         time_t openTime = acc->getCustomerInfo().openingDate;
         strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&openTime));
         
         j.push_back({
             {"accountNumber", acc->getAccountNumber()},
             {"balance", acc->getBalance()},
             {"type", acc->accountType()},
             {"customerInfo", {
                 {"name", acc->getCustomerInfo().name},
                 {"dob", acc->getCustomerInfo().dob},
                 {"cnic", acc->getCustomerInfo().cnic},
                 {"address", acc->getCustomerInfo().address},
                 {"openingDate", buffer}  // Store as formatted string
             }}
         });
     }
     ofstream file(filename);
     if (!file.is_open()) {
         throw Exceptions::FileException("Failed to open accounts file for writing");
     }
     file << j.dump(4);
     file.close();
 } catch (const exception& e) {
     throw Exceptions::FileException(string("Error saving accounts: ") + e.what());
 }
}
 
 // Load accounts from JSON file
 void loadAccountsFromFile() {
     ifstream file(filename);
     if (!file.is_open()) return;
 
     json j;
     file >> j;
     file.close();
 
     for (auto& item : j) {
         PersonalInfo info;
         info.name = item["customerInfo"]["name"];
         info.dob = item["customerInfo"]["dob"];
         info.cnic = item["customerInfo"]["cnic"];
         info.address = item["customerInfo"]["address"];
 
         // Handle both string and number formats for openingDate
         if (item["customerInfo"]["openingDate"].is_string()) {
             // Parse string format
             struct tm tm = {};
             string dateStr = item["customerInfo"]["openingDate"];
             strptime(dateStr.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
             info.openingDate = mktime(&tm);
         } else if (item["customerInfo"]["openingDate"].is_number()) {
             // Directly use number (Unix timestamp)
             info.openingDate = item["customerInfo"]["openingDate"];
         } else {
             // Fallback to current time
             info.openingDate = time(nullptr);
         }
         
         string accNum = item["accountNumber"];
         B bal = item["balance"];
         string type = item["type"];
         
         createAccount(accNum, bal, type, info);
     }
 }
 
 // Function to add new employee
 void addEmployee(const BankMember& employee)
 {
     employees.push_back(employee);
     saveEmployeesToFile();
 }
 
 // Find an Employee
 BankMember* findEmployee(const string& employeeID)
 {
     for (auto& emp : employees)
     {
         if (emp.getEmployeeID() == employeeID)
         {
             return &emp;
         }
     }
     return nullptr;
 }
 
 // Pay salary to employees
 void payEmployeeSalary(const string& employeeID)
 {
     for (auto& emp : employees) {
         if (emp.getEmployeeID() == employeeID) {
             emp.paySalary<B>(this);  // Explicitly specify template parameter
             return;
         }
     }
     cout << "Employee not found!\n";
 }
 
 // Save Employee data to file
 void saveEmployeesToFile() {
     json j;
     for (const auto& emp : employees) {
         j.push_back({
             {"employeeID", emp.getEmployeeID()},
             {"name", emp.getName()},
             {"designation", emp.getDesignation()},
             {"salary", emp.getSalary()},
             {"accountNumber", emp.getAccountNumber()}
         });
     }

     ofstream file(employeesFile);
     file << j.dump(4);
     file.close();
 }
 
 // Load employee from file
 void loadEmployeesFromFile()
 {
     ifstream file(employeesFile);
     if (!file.is_open()) return;

     json j;
     file >> j;
     file.close();

     for (auto& item : j)
     {
         BankMember emp(
             item["employeeID"],
             item["name"],
             item["designation"],
             item["salary"],
             item["accountNumber"]
         );
         employees.push_back(emp);
     }
 }
 
 // Function to deduct zakat from saving account
 void processZakat(const string& accNum)
 {
     BankAccount<B>* acc = findAccount(accNum);
     if (acc && acc->accountType() == "Saving") {
         SavingAccount<B>* savingAcc = dynamic_cast<SavingAccount<B>*>(acc);
         if (savingAcc) {
             savingAcc->calculateZakat();
             saveAccountsToFile();
         }
     } else {
         cout << "Account not found or not a Saving account\n";
     }
 }
 
 // Display account services (added function)
 void displayAccountServices(const string& accNum)
 {
     BankAccount<B>* acc = findAccount(accNum);
     if (acc) {
         Services services;
         services.displayServices(acc->accountType());
     } else {
         cout << "Account not found!\n";
     }
 }
 
 // Deduct zakat (added function)
 void deductZakat(const string& accNum)
 {
     processZakat(accNum);
 }
 
 void addUser(const User& user) {
    users[user.getUsername()] = user;
    saveUsersToFile();
 }
 
 //  Find user
 User* authenticateUser(const string& username, const string& password) {
    auto it = users.find(username);
    if (it != users.end() && it->second.verifyPassword(password)) {
        return &(it->second);
    }
    return nullptr;
 }
 
 // Save users to file
 void saveUsersToFile() {
    json j;
    for (const auto& pair : users) {
        j.push_back({
            {"username", pair.second.getUsername()},
            {"password", pair.second.password}, // Note: In real system, store hashed passwords
            {"role", pair.second.getRole()},
            {"associatedAccount", pair.second.getAssociatedAccount()}
        });
    }
    ofstream file(usersFile);
    file << j.dump(4);
    file.close();
 }
 
 // Load users from JSON file
 void loadUsersFromFile() {
    ifstream file(usersFile);
    if (!file.is_open()) return;

    json j;
    file >> j;
    file.close();

    for (auto& item : j) {
        User user(
            item["username"],
            item["password"],
            item["role"],
            item.value("associatedAccount", "")
        );
        users[user.getUsername()] = user;
    }
 }
};

//-------------------------------------- Saving Account----------------------------------
template<typename B>
class SavingAccount : public BankAccount<B>  // inherit from BankAccount class
{
private:
 B zakat;
 int yearSaved;
 bool canWithdraw;

 // Friend class to access private members
 friend class Services;  // Allow Services class to access private members
 
public:
 SavingAccount();
 SavingAccount(string accountNum, B balance, PersonalInfo info, B zakat, int year, bool canWithdraw);
 
 // Function overloading
 void updateYears();
 void updateYears(int years);
 
 // Inline function
 inline bool isZakatApplicable() const;
 
 // overriding withdraw function
 B withdraw(B amount) override;
 
 // calcluate Zakat function
 void calculateZakat();
 
 // override accountType function
 string accountType() override;
 
 // Getters/Setters
 B getZakat() const;
 void setZakat(B zakat);
};

//------------------------------------- Business Account-----------------------------
template<typename B>
class BusinessAccount : public BankAccount<B>  // inherit from BankAccount class
{
private:
 string LinkedManagementSystem;
 
 // Friend class to access private members
 friend class Services;  // Allow Services class to access private members

public:
 BusinessAccount() : BankAccount<B>() {}
 BusinessAccount(string accountNum, B balance, PersonalInfo info, string system);
 
 // Function overriding
 string accountType() override;
 B withdraw(B amount) override;
 
 // Getters/Setters
 string getLinkedSystem() const;
 void setLinkedSystem(string system);
};

//========================================= Loan class======================================
class Loan
{
private:
 string loanId;
 double amount;
 string status;
 string loanType;
 static int totalLoans;  // Static member

public:
 Loan(const string& id, double amt, const string& stat, const string& type);
 
 // Static function
 static int getTotalLoans();
 
 // Getters
 string getLoanId() const;
 double getAmount() const;
 string getStatus() const;
 string getLoanType() const;
 
 // Setters
 void setStatus(const string& newStatus);
 
 // Save to JSON
 void saveLoan(const string& filename = "loans.json");
 
 // Static function to load loans
 static void loadLoans(const string& filename = "loans.json");
};

//========================================= Transaction class==============================
class Transaction
{
private:
 string fromAccount;
 string toAccount;
 double amount;
 string status;
 string transactionType;
 time_t transactionDate;

public:
 Transaction(string fromAcc, string toAcc, double amt, string stat, string type);
 
 // Getters
 string getFromAccount() const;
 string getToAccount() const;
 double getAmount() const;
 string getStatus() const;
 string getTransactionType() const;
 time_t getTransactionDate() const;

 // Setters
 void setStatus(string newStatus);
 void setTransactionType(string newType);

 // Save to JSON
 void saveTransaction(const string& filename = "transactions.json");

 // Static function to load transactions
 static void loadTransactions(const string& filename = "transactions.json");
};

#endif // FUNCTIONS_H
}