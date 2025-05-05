#include "bank.h"
#include <map>
#include <limits>
using namespace Banking;

// Function to generate random account numbers
string generateAccountNumber()
{
    static int counter = 24001;  // Starting account number
    return "MDBSCE" + to_string(counter++);
}

// Function to get customer information
PersonalInfo getCustomerInfo()
{
    PersonalInfo info;
    cout << "Enter customer name: ";
    getline(cin, info.name);
    cout << "Enter date of birth (DD-MM-YYYY): ";
    getline(cin, info.dob);
    cout << "Enter CNIC: ";
    getline(cin, info.cnic);
    cout << "Enter address: ";
    getline(cin, info.address);
    return info;
}


// Function to display menu

void adminMenu(Bank<double>* bank)
{
    int choice;
    do {
        cout << "\nAdmin Menu\n";
        cout << "1. Create Account\n";
        cout << "2. Add Employee\n";
        cout << "3. Pay Employee Salary\n";
        cout << "4. View All Accounts\n";
        cout << "5. View All Employees\n";
        cout << "6. View All Loans\n";
        cout << "7. View All Transactions\n";
        cout << "8. Register New Admin\n";
        cout << "9. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1: { // Create Account
                try {
                    string type;
                    double balance;
                    cout << "Enter account type (Saving/Business): ";
                    getline(cin, type);
                    cout << "Enter initial balance: ";
                    cin >> balance;
                    cin.ignore();
                    
                    PersonalInfo info = getCustomerInfo();
                    string accNum = generateAccountNumber();
                    
                    if (bank->createAccount(accNum, balance, type, info)) {
                        cout << "Account created! Number: " << accNum << endl;
                    } else {
                        cout << "Failed to create account!\n";
                    }
                }
                catch (const Banking::Exceptions::AccountException& e) {
                    cout << "Account Error: " << e.what() << "\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            case 2: { // Add Employee
                try {
                    string empId, name, designation, accountNumber;
                    double salary;
                    
                    cout << "Enter employee ID: ";
                    getline(cin, empId);
                    cout << "Enter name: ";
                    getline(cin, name);
                    cout << "Enter designation: ";
                    getline(cin, designation);
                    cout << "Enter salary: ";
                    cin >> salary;
                    cin.ignore(); // Clear newline
                    cout << "Enter account number: ";
                    getline(cin, accountNumber);
                    
                    BankMember newEmployee(empId, name, designation, salary, accountNumber);
                    bank->addEmployee(newEmployee);
                    cout << "Employee added successfully!\n";
                }
                catch (const Banking::Exceptions::AccountException& e) {
                    cout << "Error: " << e.what() << "\n";
                    cout << "Please try again with valid input.\n";
                }
                catch (const exception& e) {
                    cout << "Unexpected error: " << e.what() << "\n";
                }
                break;
            }
    case 3:
           { // Pay Employee Salary
          try {
        string empID;
        cout << "Enter employee ID: ";
        getline(cin, empID);
        bank->payEmployeeSalary(empID);
          }
             catch (const Banking::Exceptions::AccountException& e) 
             {

                cout << "Payment Error: " << e.what() << "\n";
             }
            catch (const exception& e) 
            {
              cout << "Error: " << e.what() << "\n";
            }
           break;
         }
            case 4:
            {
                 // View all accounts using map
                 cout << "\nAll Accounts:\n";
                 // Changed from getAccountMap() to accessing accounts directly
                 for (auto* acc : bank->getAccounts()) {
                     cout << "Account #: " << acc->getAccountNumber() << "\n";
                     acc->displayAccountInfo();
                     cout << "------------------------\n";
                 }
                 break;
            }
            case 5:
            {
                // View all employees
                cout << "\nAll Employees:\n";
                bank->loadEmployeesFromFile();
                break;
               
            }
            case 6:
            { // View all loans
                cout << "\nAll Loans:\n";
                Loan::loadLoans();
                break;
            }
            case 7:
            { // View all transactions
                cout << "\nAll Transactions:\n";
                Transaction::loadTransactions();
                break;
            }
            case 8:
            { // Register New Admin
             try {
                     string uname, pwd;
                     cout << "Enter new admin username: ";
                     getline(cin, uname);
                    cout << "Enter password: ";
                    getline(cin, pwd);
        
                 if (uname.empty() || pwd.empty()) 
                 {
                  throw Banking::Exceptions::AccountException("Username and password cannot be empty");
                 }
        
                    bank->addUser(User(uname, pwd, "admin", "default_account"));
                     cout << "Admin registered successfully!\n";
                }
                 catch (const Banking::Exceptions::AccountException& e)
                 {
                    cout << "Registration Error: " << e.what() << "\n";
                 }
                catch (const exception& e) 
                {
                      cout << "Error: " << e.what() << "\n";
                }
                 break;
                }
            case 9:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    } while (true);
}

void customerMenu(Bank<double>* bank, const string& accountNumber) {
    int choice;
    do {
        cout << "\nCustomer Menu\n";
        cout << "1. Deposit\n";
        cout << "2. Withdraw\n";
        cout << "3. Transfer\n";
        cout << "4. Account Info\n";
        cout << "5. Apply for Loan\n";
        cout << "6. View Transactions\n";
        cout << "7. View Services\n";
        cout << "8. Calculate Zakat\n";
        cout << "9. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch(choice)
        {
            case 1:{ // Deposit
                try {
                    string accNum;
                    double amount;
                    cout << "Enter account number: ";
                    getline(cin, accNum);
                    cout << "Enter amount to deposit: ";
                    cin >> amount;
                    cin.ignore();
                    
                    if (amount <= 0) {
                        throw Banking::Exceptions::TransactionException("Amount must be positive");
                    }
                    
                    auto account = bank->findAccount(accNum);
                    if (account) {
                        account->updateBalance(amount);
                        Transaction("Bank", accNum, amount, "Completed", "Deposit")
                            .saveTransaction();
                        cout << "Deposit successful!\n";
                    } else {
                        throw Banking::Exceptions::AccountException("Account not found");
                    }
                }
                catch (const Banking::Exceptions::TransactionException& e) {
                    cout << "Transaction Error: " << e.what() << "\n";
                }
                catch (const Banking::Exceptions::AccountException& e) {
                    cout << "Account Error: " << e.what() << "\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            
            case 2:{ // Withdraw
                try {
                    string accNum;
                    double amount;
                    cout << "Enter account number: ";
                    getline(cin, accNum);
                    cout << "Enter amount to withdraw: ";
                    cin >> amount;
                    cin.ignore();
                    
                    if (amount <= 0) {
                        throw Banking::Exceptions::TransactionException("Amount must be positive");
                    }
                    
                    auto account = bank->findAccount(accNum);
                    if (account) {
                        if (account->withdraw(amount)) {
                            Transaction(accNum, "Bank", amount, "Completed", "Withdrawal")
                                .saveTransaction();
                            cout << "Withdrawal successful!\n";
                        }
                    } else {
                        throw Banking::Exceptions::AccountException("Account not found");
                    }
                }
                catch (const Banking::Exceptions::TransactionException& e) {
                    cout << "Transaction Error: " << e.what() << "\n";
                }
                catch (const Banking::Exceptions::AccountException& e) {
                    cout << "Account Error: " << e.what() << "\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            
            case 3:{ // Transfer
                try {
                    string fromAcc, toAcc;
                    double amount;
                    cout << "Enter your account number: ";
                    getline(cin, fromAcc);
                    cout << "Enter recipient account number: ";
                    getline(cin, toAcc);
                    cout << "Enter amount to transfer: ";
                    cin >> amount;
                    cin.ignore();
                    
                    if (amount <= 0) {
                        throw Banking::Exceptions::TransactionException("Amount must be positive");
                    }
                    
                    auto fromAccount = bank->findAccount(fromAcc);
                    auto toAccount = bank->findAccount(toAcc);
                    
                    if (!fromAccount || !toAccount) {
                        throw Banking::Exceptions::AccountException("One or both accounts not found");
                    }
                    
                    if (fromAccount->withdraw(amount)) {
                        toAccount->updateBalance(amount);
                        Transaction(fromAcc, toAcc, amount, "Completed", "Transfer")
                            .saveTransaction();
                        cout << "Transfer successful!\n";
                    }
                }
                catch (const Banking::Exceptions::TransactionException& e) {
                    cout << "Transaction Error: " << e.what() << "\n";
                }
                catch (const Banking::Exceptions::AccountException& e) {
                    cout << "Account Error: " << e.what() << "\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            
           
            case 4:
            {  // Account Info
                string accNum;
                cout << "Enter account number: ";
                getline(cin, accNum);
                
                auto account = bank->findAccount(accNum);
                if (account) {
                    account->displayAccountInfo();
                } else {
                    cout << "Account not found!\n";
                }
                break;
            }
            case 5:{ // Apply for Loan
                try {
                    string accNum, loanType;
                    double amount;
                    cout << "Enter your account number: ";
                    getline(cin, accNum);
                    cout << "Enter loan type (Personal/Business): ";
                    getline(cin, loanType);
                    cout << "Enter loan amount: ";
                    cin >> amount;
                    cin.ignore();
                    
                    if (amount <= 0) {
                        throw Banking::Exceptions::TransactionException("Loan amount must be positive");
                    }
                    
                    if (!bank->findAccount(accNum)) {
                        throw Banking::Exceptions::AccountException("Account not found");
                    }
                    
                    string loanId = "LN" + to_string(rand() % 10000);
                    Loan loan(loanId, amount, "Pending", loanType);
                    loan.saveLoan();
                    cout << "Loan application submitted! ID: " << loanId << endl;
                }
                catch (const Banking::Exceptions::TransactionException& e) {
                    cout << "Loan Error: " << e.what() << "\n";
                }
                catch (const Banking::Exceptions::AccountException& e) {
                    cout << "Account Error: " << e.what() << "\n";
                }
                catch (const exception& e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            }
            
            case 6:
            {  // View Transactions
                Transaction::loadTransactions();
                break;
            }
            case 7:
            {  // View Services
                string accNum;
                cout << "Enter account number: ";
                getline(cin, accNum);
                bank->displayAccountServices(accNum);
                break;
            }
            case 8:
            {  // Calculate Zakat
                string accNum;
                cout << "Enter saving account number: ";
                getline(cin, accNum);
                bank->deductZakat(accNum);
                break;
            }
            // ... other cases same as before but using customer's account
            case 9:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    } while (true);
}

int main() {
    Bank<double>* bank = Bank<double>::getInstance();
    bank->loadUsersFromFile();
    
    // Add default admin if none exists
    if (bank->getUsers().empty()) {
        bank->addUser(User("admin", "admin123", "admin", "default_account"));
        cout << "Default admin created. Username: admin, Password: admin123\n";
    }

    while (true) {
        cout << "\nBank Management System\n";
        cout << "1. Login\n";
        cout << "2. Register Customer\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        
        int mainChoice;
        cin >> mainChoice;
        cin.ignore();

        if (mainChoice == 1) {
            string username, password;
            cout << "Username: ";
            getline(cin, username);
            cout << "Password: ";
            getline(cin, password);

            User* user = bank->authenticateUser(username, password);
            if (user) {
                if (user->getRole() == "admin") {
                    adminMenu(bank);
                } else {
                    customerMenu(bank, user->getAssociatedAccount());
                }
            } else {
                cout << "Invalid credentials!\n";
            }
        } 
        else if (mainChoice == 2) {
            // Customer registration
            PersonalInfo info = getCustomerInfo();
            string username, password;
            cout << "Choose username: ";
            getline(cin, username);
            cout << "Choose password: ";
            getline(cin, password);

            // Create account
            string accNum = generateAccountNumber();
            bank->createAccount(accNum, 0.0, "Saving", info);
            
            // Create user
            bank->addUser(User(username, password, "customer", accNum));
            cout << "Registration successful! Your account number is: " << accNum << "\n";
        }
        else if (mainChoice == 3) {
            break;
        }
        else {
            cout << "Invalid choice!\n";
        }
    }

    return 0;
}