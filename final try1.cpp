#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <queue>
using namespace std;

class Order{
	string name;
	string cli_id;
	string ord_id;
	string reason;
	
	int side;
	double price;
	int quantity;
	int rem_qty;
	int value;
	int priority;
	
	Order (string*,string*,string*,string*,string*);
	bool validity();
	void valid_check;
	
	
};
vector<string> instruments[5] ={"Lotus","Lavender","Tulip","Orchid"."Rose"};

void Order::valid_check(string *cli_id, string *name, string *side, string *qty, string *price) {
    if(!((*cli_id).length() && (*name).length() && (*side).length() && (*price).length() && (*qty).length())) {
        reason = "Invalid fields";
        value =0;    
        return;
    }
    if(find(begin(instruments), end(instruments), *name) == end(instruments)) {
        reason = "Invalid instrument";
        value =0;    
        return;
    }
    int s = stoi(*side);
    if(!(s == 1 || s == 2)) {
        reason = "Invalid side";
        value =0;    
        return;  
    }
    double p = stod(*price);
    if(p <= 0.0) {
        reason = "Invalid price";
        value =0;    
        return;
    }
    int q = stoi(*qty);
    if(q%10 != 0 || q < 10 || q > 1000) {
        reason = "Invalid size";
        value =0;    
        return;   
    }
    reason = "";
    value =1;    
}   

bool Order::validity() {
    if(value == 0)
        return false;
    return true;
}
void Order::f_write(ofstream &fout, int execQty) {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    time_t t = chrono::system_clock::to_time_t(now);
    tm* tm = localtime(&t);
    string stat = "";
    if(status == 0) stat = "Reject";
    else if(status == 1) stat = "New";
    else if(status == 2) stat = "Fill";
    else if(status == 3) stat = "PFill";
    fout << cli_id << "," << ord_id << "," << name << "," << side << "," << price << "," << execQty << "," << stat << "," 
    << reason << "," << put_time(tm, "%Y.%m.%d-%H.%M.%S") << "." << setfill('0') << setw(3) << to_string(now_ms.count()) << "\n"; 
}

void classify_data(const string &filename)
{
    ifstream fin;
    fin.open("order.csv", ios::in);
    ofstream fout;
    fout.open("execution_rep.csv", ios::out);
    fout << "Client Order ID,Order ID,Instrument,Side,Price,Quantity,Status,Reason,Transaction Time\n";

    vector<vector<string>> lotus(2);
    vector<vector<string>> lavender(2);
    vector<vector<string>> tulip(2);
    vector<vector<string>> orchid(2);
    vector<vector<string>> rose(2);
    
    vector<vector<Order>> ord_book[5] ={lotus, lavender, tulip, orchid, rose};
    
    string line, word;
    vector<string> row;    

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);
        
        while (getline(s, word, ',')) {
            row.push_back(word);
        }
        
        Order new_ord(*row[0],*row[1],*row[2],*row[3],*row[4]);
        
        if (validity(new_ord)){
        	
        	if (new_ord.side==1){
        		
                while(!(orderBook[1].empty()) && (ord_book[1][0].price <= new_ord.price)) {
                    if(new_ord.remQty == ord_book[1][0].remQty) {           
                        new_ord.status = 2;
                        ord_book[1][0].status = 2;
                        new_ord.price = ord_book[1][0].price;
                        new_ord.f_write(fout);
                        ord_book[1][0].f_write(fout);
                        new_ord.remQty = 0;
                        ord_book[1][0].remQty = 0;
                        ord_book[1].erase(begin(ord_book[1]));
                        break;
                    } else if(new_ord.remQty > orderBook[1][0].remQty) {      
                        double temp = new_ord.price;
                        new_ord.status = 3;
                        ord_book[1][0].status = 2;
                        new_ord.price = ord_book[1][0].price;
                        new_ord.f_write(fout, ord_book[1][0].remQty);
                        ord_book[1][0].f_write(fout);
                        new_ord.remQty = new_ord.remQty - ord_book[1][0].remQty;
                        ord_book[1][0].remQty = 0;
                        newOrder.price = temp;
                        ord_book[1].erase(begin(ord_book[1]));
                    } else {                                                  
                        new_ord.status = 2;
                        ord_book[1][0].status = 3;
                        new_ord.price = ord_book[1][0].price;
                        new_ord.f_write(fout);
                        ord_book[1][0].f_write(fout, new_ord.remQty);
                        ord_book[1][0].remQty = ord_book[1][0].remQty - new_ord.remQty;
                        new_ord.remQty = 0;
                        break;
                    }
                }        		
			}
			
			else if (new_ord == 2){
        		
                while(!(ord_book[0].empty()) && (ord_book[0][0].price <= new_ord.price)) {
                    if(new_ord.remQty == ord_book[1][0].remQty) {           
                        new_ord.value = 2;
                        ord_book[0][0].value = 2;
                        new_ord.price = ord_book[1][0].price;
                        new_ord.f_write(fout);
                        ord_book[0][0].f_write(fout);
                        new_ord.remQty = 0;
                        ord_book[0][0].remQty = 0;
                        ord_book[0].erase(begin(ord_book[0]));
                        break;
                    } else if(new_ord.remQty > orderBook[0][0].remQty) {      
                        double temp = new_ord.price;
                        new_ord.value = 3;
                        ord_book[0][0].value = 2;
                        new_ord.price = ord_book[0][0].price;
                        new_ord.f_write(fout, ord_book[0][0].remQty);
                        ord_book[0][0].f_write(fout);
                        new_ord.remQty = new_ord.remQty - ord_book[0][0].remQty;
                        ord_book[0][0].remQty = 0;
                        newOrder.price = temp;
                        ord_book[0].erase(begin(ord_book[0]));
                    } else {                                                  
                        new_ord.value = 2;
                        ord_book[0][0].value = 3;
                        new_ord.price = ord_book[0][0].price;
                        new_ord.f_write(fout);
                        ord_book[0][0].f_write(fout, new_ord.remQty);
                        ord_book[0][0].remQty = ord_book[0][0].remQty - new_ord.remQty;
                        new_ord.remQty = 0;
                        break;
                    }
            	}
                    
                if(new_ord.value == 1) {
                    new_ord.f_write(fout);
                }

                // If the aggressive order is not fully executed, then put it into the order book 
                if(new_ord.remQty > 0.0) {
                    ord_book[0]->emplace(new_ord);
                }
                        		
			}				
				
				
			}
        	
        	
        	
		}
        
        
}
