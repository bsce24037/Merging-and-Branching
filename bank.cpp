// ----------------------------Implementation file--------------------------------

#include "bank.h"
#include <iostream>
#include <fstream>

// exceptional handling line 89,
using namespace Banking;
using namespace Banking::Exceptions;


// Initialize static member
template<typename B>
Bank<B>* Bank<B>::instance = nullptr;


// ========================= User class implementation ========================
User::User(const string& uname, const string& pwd, const string& r, const string& acc )
           {
             username=uname;
             password=pwd;
             role=r;
             associatedAccount=acc;
            }
    
        // Getters
        string User::getUsername() const
        {
            return username;
        }
        string User::getRole() const
        {
            return role;
        }
        string User::getAssociatedAccount() const
        {
            return associatedAccount;
        }
        // Verify password
        bool User::verifyPassword(const string& pwd) const
        {
            return password == pwd;
        }



// ========================= BankMember class implementation ========================




BankMember::BankMember(const string& id, const string& n, const string& design,
    double sal, const string& account)
: employeeID(id), name(n), designation(design), salary(sal), accountNumber(account)
{
    // Exception handling for empty fields
if (id.empty() || n.empty()) {
throw AccountException("Employee ID and name cannot be empty");
}
}

// Getters implementation
string BankMember::getEmployeeID() const { 
    return employeeID; 
}

string BankMember::getName() const { 
    return name; 
}

string BankMember::getDesignation() const { 
    return designation; 
}

double BankMember::getSalary() const { 
    return salary; 
}

string BankMember::getAccountNumber() const { 
    return accountNumber; 
}

// Setters implementation
void BankMember::setSalary(double sal) { 
    salary = sal; 
}

void BankMember::setAccountNumber(const string& account) { 
    accountNumber = account; 
}


// Template implementation for BankMember::paySalary
template<typename B>
void BankMember::paySalary(Bank<B>* bank) {
    if (!bank) {
        throw TransactionException("Bank instance is null");// Check if bank instance is null using exception handling
    }
    if (BankAccount<B>* account = bank->findAccount(accountNumber)) {
        account->updateBalance(salary);
        cout << "Paid salary of $" << salary << " to " << name << endl;
    } else {
        throw AccountException("Account not found for salary payment");// Check if account exists using exception handling
    }
}


// ========================= BankAccount<B> class implementation ========================
template<typename B>
BankAccount<B>::BankAccount()
{  
    balance = B();
    accountNumber = "";
    customerInfo.name = "";
    customerInfo.openingDate = time(nullptr);
}
template<typename B>
BankAccount<B>::BankAccount(string accountNum, B balance, PersonalInfo info) 
{
    accountNumber = accountNum;
    this->balance = balance;
    this->customerInfo = info; 
    customerInfo.openingDate = time(nullptr);
}
template<typename B>
//----- Copy constructor (deep copy)------
BankAccount<B>::BankAccount(const BankAccount& other)
{
    balance = other.balance;
    accountNumber = other.accountNumber;
    customerInfo = other.customerInfo;
}
template<typename B>
void BankAccount<B>::setAccountNumber(const string& accountNum)
{
    // Exception handling for empty account number
    if (accountNum.empty()) {
        throw AccountException("Account number cannot be empty");
    }
    accountNumber = accountNum;
}
template<typename B>
// Setters functions:
void BankAccount<B>::setBalance(B balance)
{
     this->balance = balance;
}
template<typename B>
// Setters functions:
void BankAccount<B>::setCustomerInfo(const PersonalInfo& info)
{
    customerInfo = info;
}
template<typename B>
// Getters functions:
string BankAccount<B>::getAccountNumber() const
{
    return accountNumber;
}
template<typename B>
B BankAccount<B>::getBalance() const
{
    return balance;
}
template<typename B>
PersonalInfo BankAccount<B>::getCustomerInfo() const
{
    return customerInfo;
}
template<typename B>
// Member virtaul functions:
 void BankAccount<B>::displayAccountInfo() const
{
    cout << customerInfo;
    cout << "Account Number: " << accountNumber << endl;
    cout << "Current Balance: $" << balance << endl;
}
template<typename B>
 B BankAccount<B>::updateBalance(B amount)
{
    balance += amount;
    cout << "Balance updated: $" << balance << endl;
    return balance;
}
template<typename B>
B BankAccount<B>::withdraw(B amount)
{
    if (amount <= 0) {
        throw TransactionException("Withdrawal amount must be positive");
    }
    if (amount > balance) {
        throw TransactionException("Insufficient balance");
    }
    balance -= amount;
    cout << "Withdrawal successful!" << endl;
    return amount;
}



// ========================= Saving account class implementation ========================
template<typename B>
SavingAccount<B>::SavingAccount()
{
    zakat = 0;
    yearSaved = 0;
    canWithdraw = true;
    this->balance = B();  
    this->accountNumber = "";
    this->customerInfo.name = "";
}

template<typename B>
SavingAccount<B>::SavingAccount(string accountNum, B balance, PersonalInfo info, B zakat, int year, bool canWithdraw)
    : BankAccount<B>(accountNum, balance, info)
{
    this->zakat = zakat;
    yearSaved = year;
    this->canWithdraw = canWithdraw; 
}

// Function overloading
template<typename B>
void SavingAccount<B>::updateYears()
{
    yearSaved++;
}

template<typename B>
void SavingAccount<B>::updateYears(int years)
{
    yearSaved = years;
}

template<typename B>
// Inline function
inline bool SavingAccount<B>::isZakatApplicable() const
{
    return this->balance >= 20000;
}

template<typename B>
B SavingAccount<B>::withdraw(B amount) 
{
    if (!canWithdraw)
    {
        cout << "Withdrawals not allowed for this account!" << endl;
        return 0;
    }
    return BankAccount<B>::withdraw(amount);
}

template<typename B>
// Calculate Zakat function
void SavingAccount<B>::calculateZakat()
{
    if (this->balance >= 20000) {  // Nisab amount
        zakat = this->balance * 0.025;  // 2.5%
        this->balance -= zakat;
        cout << "Zakat of $" << zakat << " deducted from account "
             << this->accountNumber << endl;
    } else {
        cout << "Balance below Nisab, no Zakat due\n";
    }
}

template<typename B>
// Account type function
string SavingAccount<B>::accountType() 
{
    return "Saving";
}

template<typename B>
// Getters/Setters
B SavingAccount<B>::getZakat() const
{
    return zakat;
}

template<typename B>
void SavingAccount<B>::setZakat(B zakat)
{
    this->zakat = zakat;
}




// ========================= Business account class implementation ========================
template<typename B>
BusinessAccount<B>::BusinessAccount(string accountNum, B balance, PersonalInfo info, string system)
    : BankAccount<B>(accountNum, balance, info)
{
    LinkedManagementSystem = system;
}

template<typename B>
// Account type function
string BusinessAccount<B>::accountType() 
{
    return "Business";
}

template<typename B>
// Overriding withdraw function
B BusinessAccount<B>::withdraw(B amount) 
{
    if (amount > this->balance)
    {
        cout << "Insufficient balance!" << endl;
        return 0;
    }
    this->balance -= amount;
    cout << "Withdrawal successful from business account!" << endl;
    return 1;
}

template<typename B>
// Getters/Setters
string BusinessAccount<B>::getLinkedSystem() const
{ 
    return LinkedManagementSystem;
}

template<typename B>
void BusinessAccount<B>::setLinkedSystem(string system)
{ 
    LinkedManagementSystem = system;
}




// ========================= Loan class implementation ========================
// Static member initialization
int Loan::totalLoans = 0;

Loan::Loan(const string& id, double amt, const string& stat, const string& type)
    : loanId(id), amount(amt), status(stat), loanType(type)
{
    // Increment total loans count 
    totalLoans++;
}

// Loan methods
int Loan::getTotalLoans()
{
    return totalLoans;
}

// Getters
string Loan::getLoanId() const
{
    return loanId;
}

double Loan::getAmount() const
{
    return amount;
}

string Loan::getStatus() const
{
    return status;
}

string Loan::getLoanType() const
{
    return loanType;
}

// Setters
void Loan::setStatus(const string& newStatus)
{ 
    status = newStatus;
}

// Save to JSON
void Loan::saveLoan(const string& filename)
{
    json j;

    ifstream inFile(filename);
    if (inFile.is_open()) {
        inFile >> j;
        inFile.close();
    }

    j.push_back({
        {"loanId", loanId},
        {"amount", amount},
        {"status", status},
        {"loanType", loanType}
    });

    ofstream outFile(filename);
    outFile << j.dump(4);
    outFile.close();
}

// Static function to load loans
void Loan::loadLoans(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "No loans recorded yet.\n";
        return;
    }

    json j;
    file >> j;
    file.close();

    for (auto& loan : j) {
        cout << "Loan ID: " << loan["loanId"]
            << ", Amount: $" << loan["amount"]
            << ", Status: " << loan["status"]
            << ", Type: " << loan["loanType"] << endl;
    }
}



// ========================= Transaction class implementation ========================
Transaction::Transaction(string fromAcc, string toAcc, double amt, string stat, string type)
    : fromAccount(fromAcc), toAccount(toAcc), amount(amt), 
      status(stat), transactionType(type), transactionDate(time(nullptr))
{
}

// Getters
string Transaction::getFromAccount() const
{
    return fromAccount;
}

string Transaction::getToAccount() const
{
    return toAccount;
}

double Transaction::getAmount() const
{
    return amount;
}

string Transaction::getStatus() const
{
    return status;
}

string Transaction::getTransactionType() const
{
    return transactionType;
}

time_t Transaction::getTransactionDate() const
{
    return transactionDate;
}

// Setters functions:
void Transaction::setStatus(string newStatus)
{
    status = newStatus;
}

void Transaction::setTransactionType(string newType)
{
    transactionType = newType;
}

// Save to JSON
void Transaction::saveTransaction(const string& filename)
{
    json j;

    ifstream inFile(filename);
    if (inFile.is_open())
    {
        inFile >> j;
        inFile.close();
    }

    j.push_back({
        {"fromAccount", fromAccount},
        {"toAccount", toAccount},
        {"amount", amount},
        {"status", status},
        {"transactionType", transactionType},
        {"date", static_cast<long long>(transactionDate)}  // Explicitly cast to long long
    });

    ofstream outFile(filename);
    outFile << j.dump(4);
    outFile.close();
}

// Static function to load transactions
void Transaction::loadTransactions(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "No transactions recorded yet.\n";
        return;
    }
    json j;
    file >> j;
    file.close();
    for (auto& trans : j)
    {
        // Extract time_t value from JSON
        time_t transDate;

        // Handle both integer and string representations
        if (trans["date"].is_number())
        {
            transDate = trans["date"].get<time_t>();
        } 
        else if (trans["date"].is_string())
        {
            string dateStr = trans["date"].get<string>();
            transDate = stol(dateStr);
        } 
        else
        {
            transDate = time(nullptr);  // Default to current time if invalid
        }

        cout << "From: " << trans["fromAccount"].get<string>() 
             << " | To: " << trans["toAccount"].get<string>()
             << " | Amount: $" << trans["amount"].get<double>()
             << " | Type: " << trans["transactionType"].get<string>()
             << " | Date: " << ctime(&transDate);
    }
}

// Explicit template instantiation for common types
template class BankAccount<double>;
template class BankAccount<float>;
template class SavingAccount<double>;
template class SavingAccount<float>;
template class BusinessAccount<double>;
template class BusinessAccount<float>;
template class Bank<double>;
template class Bank<float>;