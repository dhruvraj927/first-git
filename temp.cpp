#include<iostream>
#include<vector>
#include<string>
using namespace std;

class user{
private:
    string name;
    int mobile;
    string email;
    string address;
    int pin;
    double wallet;
    string bank;
    string trans;
    int trans_id;

public:
    user(string uname,int no,string mail,string add ,int code){
        name=uname;
        mobile=no;
        email=mail;
        address=add;
        pin=code;
        wallet=0;
    }
    void addbank(){
        cout << "ENTER THE BANK NAME = ";
        cin >> bank;
    }

    void tran(int t,int i){
        trans_id=i;

        if(t==1) trans="upi";
        else if(t==2) trans="online";
        else trans="phone pay";
    }
    void dep(int m){
        wallet+=m;
    }
    void trs(int m){
        wallet-=m;
    }
    void mon(int m){
        wallet+=m;
    }
    int need(){
        return wallet;

    }


    virtual void login(){
        cout << "User is login" << endl << "NAME = " << name << endl << "MOBILE = " << mobile;
        
    }

    virtual ~user(){}
};

class employee {
   private:
        string name;
        int id;
        int mobile;
        string email;
        string role;
        string bankid;
    
    public:
        employee(string n,int i,int m,string e,string r,string b){
            name=n;
            id=i;
            mobile=m;
            email=e;
            role=r;
            bankid=b;
        }
    
    virtual void login(){
        cout << "employee is login" << endl << "NAME = " << name << endl << "MOBILE = " << mobile << endl;
        cout << "BANK =" << bankid << endl << "ROLE = " << role << endl;
        
    }

    virtual ~employee(){}

};

class manager :public employee{
    public: manager(string n,int i,int m,string e,string r,string b):employee(n,i,m,e,r,b){}

    void login(){
         cout << "MANAGER IS LOGIN" << endl;
    }
};

class trnascation{
    int type;
    int id;
    int money;
    public:
    trnascation(){
        cout << "ENTER TRNASATION TYPLE " << endl << "UPI(Gpay) = 1" << endl << "ONLINE =2" << endl << "Phone pay=3" << endl;
        int t;
        cin >> t;
        type=t;
        id=190;
        
        cout << "money => (+ or -)";
        cin >> money;
    }

    int trna_type(){
        return type;
    }
    int trns_id(){
        return id+10;
    }
    int mon(){
        return money;
    }
};

class deposite{
    public:
    //deposite(){}
    int depo(){
        cout << "ENTER DEPOSITE AMOUNT = ";
        int d;
        cin >> d;
        if(d<0){
            cout << "ENTER THE WRITE AMOUNT!!!!!";
            return 0;
        }
        return d;


    }

};

class exch{
public:
    static double rate; // static variable

    static double convert(int wall) {
        double amount;
        cout << "amount ";
        cin >> amount; 
        if(amount>wall){
            cout << "YOU DONT HAVE MORE MONEY !!!!";
            return 0;
        }
        cout << "money convert => " << amount * rate;

        return amount * rate;
    }

};
double exch::rate = 83.0; 
 
class notification{

    void pin(int r,int wal,int i){
        cout << "current balance is = " << wal << endl <<"exhange credit money is = " << r << "VIA ";
        if(i==1) cout << "UPI(Gpay)";
        else if(i==2) cout << "online";
        else cout << "Phone pay" << endl;
    }
};

int main(){
    vector<user*>use;
    int i=-1;
    bool run=true;
    while(run){
        int i;
        cout << "For user = 1" << endl << "MANAGER = 2" << endl << "EMPLOYEE = 3" << endl << "exit=4" << endl ;
        cout << "ENTER => ";
        cin >> i;
        switch (i)
        {
        case 1 :
        
            use.push_back(new user("Dhruvraj",785990,"kumar2004shanu@gmail.com","N.E.B",244));
            int sw;
            cin >> sw;
            switch (sw)
            {
            case 1:
                 deposite d;
                int m=d.depo();
                use.back()->dep(m);
                break;
            case 2:
                trnascation t;
               int s= t.trna_type();
               int n= t.mon();
               int id= t.trns_id();
               use.back()->tran(s,id);
               use.back()->mon(n);
                break;
            
            case 3:
                exch e;
                int w=use.back()->need();
                int c=e.convert(w);
                use.back()->dep(c);

                break;
            
            case 4:
                break;
            }
            break;

        case 2:
            
            break;

        case 3:
            break;

        case 4:
            run=false;
            break;

    }


    }
}