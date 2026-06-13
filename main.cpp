#include <bits/stdc++.h>
#include<iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>    

using namespace std;


unordered_map<string, double> exchangeRates = {
    {"INR", 1.0},
    {"USD", 83.5},
    {"EUR", 91.2},
    {"GBP", 107.0},
    {"AED", 22.7},
    {"JPY",  0.56}
};
double convert(double amount, const string& from, const string& to) {
    if (exchangeRates.find(from) == exchangeRates.end() ||
        exchangeRates.find(to)   == exchangeRates.end()) return -1;
    double inr = amount * exchangeRates[from];
    return inr / exchangeRates[to];
}

void showRates() {
    cout << "\n  Currency Rates (1 unit = ? INR)\n";
    printDash();
    for (auto& r : exchangeRates)
        cout << "  1 " << r.first << " = " << fixed << setprecision(2) << r.second << " INR\n";
}


struct Slip {
    int    id;
    string type;          // DEPOSIT / WITHDRAW / CONVERT / TRANSFER
    string fromCurr;
    string toCurr;
    double fromAmount;
    double toAmount;
    string date;
    string status;        // PENDING / APPROVED / REJECTED
    string approvedBy;
    string note;

    void print() {
        printLine();
        cout << "        TRANSACTION SLIP\n";
        printLine();
        cout << "  Slip No    : TXN-" << setw(4) << setfill('0') << id << setfill(' ') << "\n";
        cout << "  Type       : " << type       << "\n";
        cout << "  Date       : " << date       << "\n";
        printDash();
        if (type == "CONVERT" || type == "TRANSFER") {
            cout << "  From       : " << fixed << setprecision(2) << fromAmount
                 << " " << fromCurr  << "\n";
            cout << "  To         : " << fixed << setprecision(2) << toAmount
                 << " " << toCurr    << "\n";
        } else {
            cout << "  Currency   : " << fromCurr << "\n";
            cout << "  Amount     : " << fixed << setprecision(2) << fromAmount << "\n";
        }
        if (!note.empty())
            cout << "  Note       : " << note << "\n";
        printDash();
        cout << "  Status     : " << status << "\n";
        if (!approvedBy.empty())
            cout << "  Approved By: " << approvedBy << "\n";
        printLine();
    }
};


class BankAccount {
    string accountNo;
    string ifsc;
    string bankName;
public:
    BankAccount() : accountNo(""), ifsc(""), bankName("") {}
    BankAccount(string acc, string i, string b)
        : accountNo(acc), ifsc(i), bankName(b) {}

    bool isSet() { return !accountNo.empty(); }

    void display() {
        cout << "  Bank       : " << bankName  << "\n";
        cout << "  Account No : " << accountNo << "\n";
        cout << "  IFSC       : " << ifsc      << "\n";
    }

    string getAccNo()   { return accountNo; }
    string getBankName(){ return bankName;  }
};

class Person {
    protected:
        string name;
        string email;
        string password;
    public:
    Person(string n, string e, string p)
        : name(n), email(e), password(p) {}

    string getName()  { return name;  }
    string getEmail() { return email; }

    bool checkPassword(string p) { return p == password; }

    virtual void login() {
        cout << "  Logged in as: " << name << " (" << email << ")\n";
    }
    virtual ~Person() {}
};

class User : public Person {
    int     userId;
    string  DOB;
    string  nationality;

    // Wallet: holds multiple currencies
    unordered_map<string, double> wallet;

    BankAccount bankAccount;
    vector<Slip> slipHistory;
    int slipCounter;

public:
    User(int id, string n, string e, string p, string dob, string nat)
        : Person(n, e, p), userId(id), DOB(dob), nationality(nat), slipCounter(1) {}

    // ── login ──
    void login() override {
        cout << "\n"; printDash();
        cout << "  [USER LOGIN]\n";
        cout << "  Name        : " << name        << "\n";
        cout << "  Nationality : " << nationality  << "\n";
        printDash();
    }

    // ── bank account ──
    void addBankAccount(string acc, string ifsc, string bank) {
        bankAccount = BankAccount(acc, ifsc, bank);
        cout << "  Bank account linked.\n";
        bankAccount.display();
    }

    bool hasBankAccount() { return bankAccount.isSet(); }

    // ── wallet display ──
    void showWallet() {
        cout << "\n"; printLine();
        cout << "        WALLET - " << name << "\n";
        printLine();
        if (wallet.empty()) {
            cout << "  (empty)\n";
        } else {
            for (auto& w : wallet)
                cout << "  " << w.first << "  :  "
                     << fixed << setprecision(2) << w.second << "\n";
        }
        printLine();
    }

      // ── deposit ──
       Slip deposit(double amount, const string& curr) {
        Slip s;
        s.id         = slipCounter++;
        s.type       = "DEPOSIT";
        s.fromCurr   = curr;
        s.toCurr     = curr;
        s.fromAmount = amount;
        s.toAmount   = amount;
        s.date       = now();
        s.status     = "PENDING";

        // will be finalised after approval
        slipHistory.push_back(s);
        return s;
    }

    void finaliseDeposit(Slip& s) {
        if (s.status == "APPROVED") {
            wallet[s.fromCurr] += s.fromAmount;
            cout << "\n  Deposited " << fixed << setprecision(2)
                 << s.fromAmount << " " << s.fromCurr
                 << " into your wallet.\n";
        }
        updateSlip(s);
    }

    // ── withdraw ──
    Slip withdraw(double amount, const string& curr) {
        Slip s;
        s.id         = slipCounter++;
        s.type       = "WITHDRAW";
        s.fromCurr   = curr;
        s.toCurr     = curr;
        s.fromAmount = amount;
        s.toAmount   = amount;
        s.date       = now();
        s.status     = "PENDING";

        if (wallet.find(curr) == wallet.end() || wallet[curr] < amount) {
            s.status = "REJECTED";
            s.note   = "Insufficient " + curr + " in wallet";
        }
        slipHistory.push_back(s);
        return s;
    }

    void finaliseWithdraw(Slip& s) {
        if (s.status == "APPROVED") {
            wallet[s.fromCurr] -= s.fromAmount;
            if (wallet[s.fromCurr] < 0.001) wallet.erase(s.fromCurr);
            cout << "\n  Withdrawn " << fixed << setprecision(2)
                 << s.fromAmount << " " << s.fromCurr << " from wallet.\n";
        }
        updateSlip(s);
    }
    // ── convert currency ──
    Slip convertCurrency(double amount, const string& from, const string& to) {
        Slip s;
        s.id         = slipCounter++;
        s.type       = "CONVERT";
        s.fromCurr   = from;
        s.toCurr     = to;
        s.fromAmount = amount;
        s.date       = now();
        s.status     = "PENDING";

        // check user has enough of `from`
        if (wallet.find(from) == wallet.end() || wallet[from] < amount) {
            s.status = "REJECTED";
            s.note   = "You don't have enough " + from + " in wallet";
            slipHistory.push_back(s);
            return s;
        }
        double converted = convert(amount, from, to);
        s.toAmount = converted;
        s.note     = "Rate: 1 " + from + " = " +
                     to_string(exchangeRates[from] / exchangeRates[to]) + " " + to;
        slipHistory.push_back(s);
        return s;
    }

    void finaliseConvert(Slip& s) {
        if (s.status == "APPROVED") {
            wallet[s.fromCurr] -= s.fromAmount;
            if (wallet[s.fromCurr] < 0.001) wallet.erase(s.fromCurr);
            wallet[s.toCurr] += s.toAmount;
            cout << "\n  Converted " << fixed << setprecision(2)
                 << s.fromAmount << " " << s.fromCurr
                 << "  ->  " << s.toAmount << " " << s.toCurr << "\n";
        }
        updateSlip(s);
    }

    // ── transfer to another bank account ──
    // currency must be present in wallet; if not, user must convert first
    Slip transfer(double amount, const string& curr,
                  const string& toBankName, const string& toAccNo,
                  const string& note = "") {
        Slip s;
        s.id         = slipCounter++;
        s.type       = "TRANSFER";
        s.fromCurr   = curr;
        s.toCurr     = curr;
        s.fromAmount = amount;
        s.toAmount   = amount;
        s.date       = now();
        s.status     = "PENDING";
        s.note       = "To: " + toAccNo + " @ " + toBankName;
        if (!note.empty()) s.note += " | " + note;

        if (wallet.find(curr) == wallet.end() || wallet[curr] < amount) {
            s.status = "REJECTED";
            s.note   = "Insufficient " + curr + ". Convert another currency first.";
        }
        slipHistory.push_back(s);
        return s;
    }

    void finaliseTransfer(Slip& s) {
        if (s.status == "APPROVED") {
            wallet[s.fromCurr] -= s.fromAmount;
            if (wallet[s.fromCurr] < 0.001) wallet.erase(s.fromCurr);
            cout << "\n  Transferred " << fixed << setprecision(2)
                 << s.fromAmount << " " << s.fromCurr
                 << "  ->  " << s.note << "\n";
        }
        updateSlip(s);
    }

    // ── slip history (report) ──
    void printReport() {
        cout << "\n"; printLine();
        cout << "        ACCOUNT STATEMENT - " << name << "\n";
        cout << "        " << now() << "\n";
        printLine();
        if (bankAccount.isSet()) bankAccount.display();
        printDash();
        if (slipHistory.empty()) {
            cout << "  No transactions yet.\n";
        } else {
            for (auto& s : slipHistory) s.print();
        }
        printLine();
    }

    // helpers
    int getId()    { return userId; }
    unordered_map<string, double>& getWallet() { return wallet; }

private:
    void updateSlip(Slip& s) {
        for (auto& existing : slipHistory)
            if (existing.id == s.id) { existing = s; return; }
    }
};


class Employee : public Person {
protected:
    int    empId;
    string role;
    string branchId;
public:
    Employee(int id, string n, string e, string p, string r, string b)
        : Person(n, e, p), empId(id), role(r), branchId(b) {}

    void login() override {
        cout << "\n"; printDash();
        cout << "  [EMPLOYEE LOGIN]\n";
        cout << "  Name   : " << name    << "\n";
        cout << "  Role   : " << role    << "\n";
        cout << "  Branch : " << branchId << "\n";
        printDash();
    }

    // Employee verifies a pending slip
    virtual bool verifySlip(Slip& slip) {
        cout << "\n  [Employee " << name << "] Verifying slip TXN-"<< setw(4) << setfill('0') << slip.id << setfill(' ') << "\n";
        slip.print();
        cout << "  Verify this transaction? (1=Yes / 0=No): ";
        int c; cin >> c;
        if (c == 1) {
            cout << "  [" << name << "] Transaction VERIFIED.\n";
            return true;
        }
        cout << "  [" << name << "] Transaction REJECTED by employee.\n";
        slip.status = "REJECTED";
        slip.approvedBy = name;
        return false;
    }

    string getRole() { return role; }
    virtual ~Employee() {}
};


// ─────────────────────────────────────────────
// HELPER: pick currency from list
// ─────────────────────────────────────────────
string pickCurrency() {
    cout << "\n  Available currencies:\n";
    int i = 1;
    vector<string> keys;
    for (auto& r : exchangeRates) {
        cout << "  " << i++ << ". " << r.first << "\n";
        keys.push_back(r.first);
    }
    cout << "  Choose (1-" << keys.size() << "): ";
    int c; cin >> c;
    if (c < 1 || c > (int)keys.size()) return "";
    return keys[c - 1];
}

// ─────────────────────────────────────────────
// APPROVAL FLOW:
//   Employee verifies -> Manager approves
//   Returns true if both approved
// ─────────────────────────────────────────────
bool approvalFlow(Slip& slip, Employee* emp, Manager* mgr) {
    // if already rejected (e.g. insufficient funds) skip flow
    if (slip.status == "REJECTED") {
        cout << "\n  [AUTO-REJECT] " << slip.note << "\n";
        slip.print();
        return false;
    }

    cout << "\n  --- Transaction needs approval ---\n";
    bool verified = emp->verifySlip(slip);
    if (!verified) return false;

    bool approved = mgr->approveSlip(slip);
    return approved;
}


int main(){
    // ── setup: one employee, one manager (shared across system) ──
    Employee* emp = new Employee(1, "Rahul Sharma", "rahul@bank.com","emp123", "Cashier", "B001");
    Manager*  mgr = new Manager (2, "Priya Mehta",  "priya@bank.com","mgr123", "B001");

    vector<User*> users;
    Admin admin("Admin", "admin@exchange.com", "admin@123");

    bool running = true;

    cout << "\n"; printLine();
    cout << "   ONLINE CURRENCY EXCHANGE SYSTEM\n";
    cout << "   IIIT Surat | CS302 - OOT Project\n";
    printLine();

    while (running) {
        cout << "\n  MAIN MENU\n"; printDash();
        cout << "  1. Register / Login as User\n";
        cout << "  2. Admin Panel\n";
        cout << "  3. Employee Login\n";
        cout << "  4. Manager Login\n";
        cout << "  5. Exit\n";
        cout << "  Choice: ";
        int choice; cin >> choice;

        // ══════════════════════════════
        // USER
        // ══════════════════════════════
        if (choice == 1) {
            cout << "\n  1. New registration\n  2. Existing user login\n  Choice: ";
            int sub; cin >> sub;

            User* u = nullptr;

            if (sub == 1) {
                string n, e, p, dob, nat;
                cout << "  Name        : "; cin >> n;
                cout << "  Email       : "; cin >> e;
                cout << "  Password    : "; cin >> p;
                cout << "  DOB (dd-mm-yyyy): "; cin >> dob;
                cout << "  Nationality : "; cin >> nat;
                u = new User((int)users.size() + 101, n, e, p, dob, nat);
                users.push_back(u);
                cout << "\n  Registration successful! UserID = "
                     << u->getId() << "\n";

            } else {
                if (users.empty()) { cout << "  No users yet.\n"; continue; }
                cout << "  Email   : "; string e; cin >> e;
                cout << "  Password: "; string p; cin >> p;
                for (auto x : users)
                    if (x->getEmail() == e && x->checkPassword(p)) { u = x; break; }
                if (!u) { cout << "  Wrong credentials.\n"; continue; }
            }

            u->login();

            bool uRun = true;
            while (uRun) {
                cout << "\n  USER MENU - " << u->getName() << "\n"; printDash();
                cout << "  1. Add bank account\n";
                cout << "  2. View wallet\n";
                cout << "  3. Deposit money\n";
                cout << "  4. Withdraw money\n";
                cout << "  5. Convert currency\n";
                cout << "  6. Transfer to another bank\n";
                cout << "  7. View my statement (all slips)\n";
                cout << "  8. View exchange rates\n";
                cout << "  9. Logout\n";
                cout << "  Choice: ";
                int uc; cin >> uc;

                switch (uc) {

                // ── add bank ──
                case 1: {
                    if (u->hasBankAccount()) {
                        cout << "  Bank account already linked.\n"; break;
                    }
                    string acc, ifsc, bank;
                    cout << "  Account No : "; cin >> acc;
                    cout << "  IFSC       : "; cin >> ifsc;
                    cout << "  Bank Name  : "; cin >> bank;
                    u->addBankAccount(acc, ifsc, bank);
                    break;
                }

                // ── wallet ──
                case 2:
                    u->showWallet();
                    break;

                // ── deposit ──
                case 3: {
                    cout << "  Deposit in which currency?\n";
                    string curr = pickCurrency();
                    if (curr.empty()) { cout << "  Invalid.\n"; break; }
                    double amt;
                    cout << "  Amount: "; cin >> amt;
                    if (amt <= 0) { cout << "  Invalid amount.\n"; break; }

                    Slip s = u->deposit(amt, curr);
                    bool ok = approvalFlow(s, emp, mgr);
                    u->finaliseDeposit(s);
                    if (ok) {
                        cout << "\n  [NOTIFICATION] " << u->getName()
                             << " - Deposit of " << amt << " " << curr
                             << " APPROVED. | " << now() << "\n";
                    }
                    s.print();
                    break;
                }

                // ── withdraw ──
                case 4: {
                    u->showWallet();
                    auto& w = u->getWallet();
                    if (w.empty()) { cout << "  Wallet is empty.\n"; break; }
                    cout << "  Withdraw which currency?\n";
                    string curr = pickCurrency();
                    if (curr.empty()) break;
                    double amt;
                    cout << "  Amount: "; cin >> amt;

                    Slip s = u->withdraw(amt, curr);
                    bool ok = approvalFlow(s, emp, mgr);
                    u->finaliseWithdraw(s);
                    if (ok) {
                        cout << "\n  [NOTIFICATION] " << u->getName()
                             << " - Withdrawal of " << amt << " " << curr
                             << " APPROVED. | " << now() << "\n";
                    }
                    s.print();
                    break;
                }

                // ── convert ──
                case 5: {
                    u->showWallet();
                    auto& w = u->getWallet();
                    if (w.empty()) {
                        cout << "  Wallet is empty. Please deposit first.\n"; break;
                    }

                    cout << "  Convert FROM which currency in your wallet?\n";
                    // only show what user holds
                    int i = 1;
                    vector<string> held;
                    for (auto& kv : w) {
                        cout << "  " << i++ << ". " << kv.first
                             << " (" << fixed << setprecision(2) << kv.second << ")\n";
                        held.push_back(kv.first);
                    }
                    cout << "  Choice: "; int fc; cin >> fc; fc--;
                    if (fc < 0 || fc >= (int)held.size()) { cout << "  Invalid.\n"; break; }
                    string from = held[fc];

                    double amt;
                    cout << "  Amount of " << from << " to convert: "; cin >> amt;

                    cout << "  Convert TO which currency?\n";
                    string to = pickCurrency();
                    if (to.empty() || to == from) { cout << "  Invalid.\n"; break; }

                    Slip s = u->convertCurrency(amt, from, to);
                    if (s.status == "REJECTED") {
                        cout << "\n  " << s.note << "\n";
                        s.print();
                        break;
                    }

                    cout << "\n  Conversion preview:\n";
                    cout << "  " << amt << " " << from << "  ->  "
                         << fixed << setprecision(2) << s.toAmount << " " << to << "\n";

                    bool ok = approvalFlow(s, emp, mgr);
                    u->finaliseConvert(s);
                    if (ok) {
                        cout << "\n  [NOTIFICATION] " << u->getName()
                             << " - Converted " << amt << " " << from
                             << " to " << s.toAmount << " " << to
                             << " | " << now() << "\n";
                    }
                    s.print();
                    break;
                }

                // ── transfer ──
                case 6: {
                    u->showWallet();
                    auto& w = u->getWallet();
                    if (w.empty()) {
                        cout << "  Wallet empty. Deposit or convert first.\n"; break;
                    }

                    cout << "\n  Transfer in which currency from your wallet?\n";
                    int i = 1;
                    vector<string> held;
                    for (auto& kv : w) {
                        cout << "  " << i++ << ". " << kv.first
                             << " (" << fixed << setprecision(2) << kv.second << ")\n";
                        held.push_back(kv.first);
                    }
                    cout << "  Choice: "; int fc; cin >> fc; fc--;
                    if (fc < 0 || fc >= (int)held.size()) {
                        cout << "  Invalid choice.\n"; break;
                    }
                    string curr = held[fc];

                    double amt;
                    cout << "  Amount of " << curr << " to transfer: "; cin >> amt;

                    string toBankName, toAccNo, note;
                    cout << "  Recipient bank name   : "; cin >> toBankName;
                    cout << "  Recipient account no  : "; cin >> toAccNo;
                    cout << "  Remarks (or NA)       : "; cin >> note;

                    Slip s = u->transfer(amt, curr, toBankName, toAccNo, note);
                    if (s.status == "REJECTED") {
                        cout << "\n  " << s.note << "\n";
                        s.print();
                        break;
                    }

                    bool ok = approvalFlow(s, emp, mgr);
                    u->finaliseTransfer(s);
                    if (ok) {
                        cout << "\n  [NOTIFICATION] " << u->getName()
                             << " - Transfer of " << amt << " " << curr
                             << " to " << toAccNo << " @ " << toBankName
                             << " APPROVED. | " << now() << "\n";
                    }
                    s.print();
                    break;
                }

                // ── statement ──
                case 7:
                    u->printReport();
                    break;

                // ── rates ──
                case 8:
                    showRates();
                    break;

                case 9:
                    uRun = false;
                    break;

                default:
                    cout << "  Invalid option.\n";
                }
            }
        }

        // ══════════════════════════════
        // ADMIN
        // ══════════════════════════════
        else if (choice == 2) {
            cout << "  Admin password: "; string p; cin >> p;
            if (!admin.checkPassword(p)) { cout << "  Wrong password.\n"; continue; }
            admin.login();

            bool aRun = true;
            while (aRun) {
                cout << "\n  ADMIN PANEL\n"; printDash();
                cout << "  1. View exchange rates\n";
                cout << "  2. Update exchange rate\n";
                cout << "  3. Add new currency\n";
                cout << "  4. View all users\n";
                cout << "  5. Back\n";
                cout << "  Choice: ";
                int ac; cin >> ac;
                switch (ac) {
                case 1: admin.showAllRates(); break;
                case 2: {
                    string code; double rate;
                    cout << "  Currency code (e.g. USD): "; cin >> code;
                    cout << "  New rate (1 " << code << " = ? INR): "; cin >> rate;
                    admin.setRate(code, rate);
                    break;
                }
                case 3: {
                    string code; double rate;
                    cout << "  New currency code: "; cin >> code;
                    cout << "  Rate (1 " << code << " = ? INR): "; cin >> rate;
                    admin.addCurrency(code, rate);
                    break;
                }
                case 4: admin.showAllUsers(users); break;
                case 5: aRun = false; break;
                default: cout << "  Invalid.\n";
                }
            }
        }

        // ══════════════════════════════
        // EMPLOYEE LOGIN (just info)
        // ══════════════════════════════
        else if (choice == 3) {
            cout << "  Password: "; string p; cin >> p;
            if (!emp->checkPassword(p)) { cout << "  Wrong password.\n"; continue; }
            emp->login();
            cout << "  Employee is ready to verify transactions.\n";
            cout << "  (Verification happens automatically when a user makes a transaction.)\n";
        }

        // ══════════════════════════════
        // MANAGER LOGIN (just info)
        // ══════════════════════════════
        else if (choice == 4) {
            cout << "  Password: "; string p; cin >> p;
            if (!mgr->checkPassword(p)) { cout << "  Wrong password.\n"; continue; }
            mgr->login();
            cout << "  Manager is ready to approve transactions.\n";
            cout << "  (Approval happens automatically when a user makes a transaction.)\n";
        }

        else if (choice == 5) {
            running = false;
            printLine();
            cout << "  Thank you! Goodbye.\n";
            printLine();
        }

        else {
            cout << "  Invalid choice.\n";
        }
    }

    for (auto u : users) delete u;
    delete emp;
    delete mgr;
    return 0;
}
