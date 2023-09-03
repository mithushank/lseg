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
// this is class for store the details of the order
class Order {
public: 
    static int curOrderId;
    string ord_id;             
    string cli_id;             
    string name;               
    int side;
    double price;
    int qty;
    int rem_qty;
    int status;
    int priority;
    string reason;
    Order() {}
    Order(string *,string *,string *,string *,string *);
    void validation(string *,string *,string *,string *,string *);  // function  for the checking the validity of the order
    bool validate();                                           // function to verify the the status
    void execute(ofstream&, int);                                   // function to Write to the execution_rep file
                                     
};

/*
    Comparator for priority_queue
    * if two orders have the same price, then priority will be the tie-breaker
    * if the orders are buy orders, then the order with the higher price will get higher priority
    * if the orders are sell orders, then the order with the lower price will get lower priority
*/
struct Compare {
    bool operator() (Order ord1, Order ord2) {
        if(ord1.price == ord2.price) {
            return ord1.priority < ord2.priority;
        }
        
        if(ord1.side == 1) {
            return ord1.price < ord2.price;
        } else {
            return ord1.price > ord2.price;
        }
    }
};

string instruments[5] = {"Rose","Lavender","Lotus","Tulip","Orchid"};   
int Order::curOrderId = 1;

int main(void) {
    // priority_queues for each side of all the instruments
    priority_queue<Order, vector<Order>, Compare> roseBuy;
    priority_queue<Order, vector<Order>, Compare> roseSell;
    priority_queue<Order, vector<Order>, Compare> lavenderBuy;
    priority_queue<Order, vector<Order>, Compare> lavenderSell;
    priority_queue<Order, vector<Order>, Compare> lotusBuy;
    priority_queue<Order, vector<Order>, Compare> lotusSell;
    priority_queue<Order, vector<Order>, Compare> tulipBuy;
    priority_queue<Order, vector<Order>, Compare> tulipSell;
    priority_queue<Order, vector<Order>, Compare> orchidBuy;
    priority_queue<Order, vector<Order>, Compare> orchidSell;

    // abstraction of priority_queues
    priority_queue<Order, vector<Order>, Compare>* rose[2] = {&roseBuy, &roseSell};
    priority_queue<Order, vector<Order>, Compare>* lavender[2] = {&lavenderBuy, &lavenderSell};
    priority_queue<Order, vector<Order>, Compare>* lotus[2] = {&lotusBuy, &lotusSell};
    priority_queue<Order, vector<Order>, Compare>* tulip[2] = {&tulipBuy, &tulipSell};
    priority_queue<Order, vector<Order>, Compare>* orchid[2] = {&orchidBuy, &orchidSell};

    priority_queue<Order, vector<Order>, Compare>** orderBooks[5] = {rose,lavender,lotus,tulip,orchid};

    // I/O file handling
    ifstream fin;
    fin.open("orders.csv", ios::in);
    if (!fin.is_open()) {
    cerr << "Error: Unable to open input file 'order.csv'" << endl;
    return 1;
	} else {
		cout << "fin open successfully"<< endl;
	}

    
    ofstream fout;
    fout.open("execution_rep.csv", ios::out);
    fout << "Client Order ID,Order ID,Instrument,Side,Price,Quantity,Status,Reason,Transaction Time\n";
	if (!fout.is_open()) {
	    cerr << "Error: Unable to open output file 'execution_rep.csv'" << endl;
	    return 1;
	}

    // Auxilaries for file parsing
    vector<string> row;
    string line, word, temp;   

    // Parsing order.csv file
    while(getline(fin, line)) {
        row.clear();
        stringstream s(line);
        while(getline(s, word, ',')) {
            row.push_back(word);
        }
        // Constructing a new order 
        Order new_ord(&row[0], &row[1], &row[2], &row[3], &row[4]);

        if(new_ord.validate()) {
            // Get the index of the instrument 
            int index = -1; // Initialize index to -1, indicating not found

            for (int i = 0; i < 5; ++i) 
            {
                if (instruments[i] == new_ord.name) 
                {
                index = i; // Found the element, set the index
                break;     // Exit the loop
                }
            }

            priority_queue<Order, vector<Order>, Compare>** ord_book = orderBooks[index];

            if(new_ord.side == 1) {
                /* 
                    If the sell side is not empty 
                   and the most attractive sell order can fulfill the buyer's buy order 
                */
                while(!(ord_book[1]->empty()) && ((ord_book[1]->top()).price <= new_ord.price)) {
                    Order top= (ord_book[1]->top());
                    if(new_ord.rem_qty == top.rem_qty) {             // most attractive sell order quantity = buy order quantity
                        new_ord.status = 2;
                        top.status = 2;
                        new_ord.price = top.price;
                        new_ord.execute(fout, new_ord.rem_qty);
                        top.execute(fout, new_ord.rem_qty); 
                        new_ord.rem_qty = 0;
                        top.rem_qty = 0;
                        ord_book[1]->pop();
                        break;
                    } else if(new_ord.rem_qty > top.rem_qty) {       // most attractive sell order quantity < buy order quantity
                        double temp = new_ord.price;
                        new_ord.status = 3;
                        top.status = 2;
                        new_ord.price = top.price;
                        
                        new_ord.execute(fout, top.rem_qty);
                        top.execute(fout, new_ord.rem_qty);
                        new_ord.rem_qty = new_ord.rem_qty - top.rem_qty;
                        top.rem_qty = 0;
                        new_ord.price = temp;
                        ord_book[1]->pop();
                    } else {                                             // most attractive sell order quantity > buy order quantity
                        new_ord.status = 2;
                        top.status = 3;
                        new_ord.price = top.price;
                        new_ord.execute(fout, new_ord.rem_qty);
                        top.execute(fout, new_ord.rem_qty);
                        top.rem_qty = top.rem_qty - new_ord.rem_qty;
                        top.priority++;
                        ord_book[1]->pop();
                        ord_book[1]->emplace(top);
                        new_ord.rem_qty = 0;
                        break;
                    }
                }

                // Depicts a 'New' order being put into the order book
                if(new_ord.status == 0) {
                    new_ord.execute(fout, new_ord.rem_qty);
                }

                // If the aggressive order is not fully executed, then put it into the order book 
                if(new_ord.rem_qty > 0.0) {
                    ord_book[0]->emplace(new_ord);
                }

            // Sell Side
            } else if(new_ord.side == 2) {
                /* 
                    If the buy side is not empty 
                    and the most attractive buy order can fulfill the seller's sell order
                */
                while(!(ord_book[0]->empty()) && ((ord_book[0]->top()).price >= new_ord.price)) {
                    Order top= ord_book[0]->top();
                    if(new_ord.rem_qty == top.rem_qty) {                 // most attractive buy order quantity = sell order quantity
                        new_ord.status = 2;
                        top.status = 2;
                        new_ord.price = top.price; 
                        new_ord.execute(fout, new_ord.rem_qty);
                        top.execute(fout,new_ord.rem_qty);
                        new_ord.rem_qty = 0;
                        top.rem_qty = 0;
                        ord_book[0]->pop();
                        break;
                    } else if(new_ord.rem_qty > top.rem_qty) {           // most attractive buy order quantity < sell order quantity
                        double temp2 = new_ord.price;
                        new_ord.status = 3;
                        top.status = 2;
                        new_ord.price = top.price; 
                        new_ord.execute(fout,top.rem_qty);
                        new_ord.rem_qty = new_ord.rem_qty - top.rem_qty;
                        top.execute(fout,new_ord.rem_qty);
                        top.rem_qty = 0;
                        new_ord.price = temp2;
                        ord_book[0]->pop();
                    } else {                                                        // most attractive buy order quantity > sell order quantity
                        new_ord.status = 2;
                        top.status = 3;
                        new_ord.price = top.price;
                        new_ord.execute(fout,new_ord.rem_qty);
                        top.execute(fout, new_ord.rem_qty);
                        top.rem_qty = top.rem_qty - new_ord.rem_qty;
                        top.priority++;
                        ord_book[0]->pop();
                        ord_book[0]->emplace(top);
                        new_ord.rem_qty = 0;
                        break;
                    }
                }

                // Depicts a 'New' order being put into the order book
                if(new_ord.status == 0) {
                    new_ord.execute(fout,new_ord.rem_qty);
                }

                // If the aggressive order is not fully executed, then put it into the order book 
                if(new_ord.rem_qty > 0.0) {
                    ord_book[1]->emplace(new_ord);
                }
            }
        } else {
            new_ord.execute(fout,new_ord.rem_qty);
        }
    }
    fin.close();
    fout.close();
    return 0;
}

Order::Order(string *_clientOrderId, string *_instrument, string *_side, string *_qty, string *_price) {
    ord_id = "ord" + to_string(curOrderId++);
    validation(_clientOrderId, _instrument, _side, _qty, _price);
    cli_id = *_clientOrderId;
    name = *_instrument;
    side = stoi(*_side);
    price = stod(*_price);
    qty = stoi(*_qty);
    rem_qty = qty;
    priority = 1;
}

void Order::validation(string *_clientOrderId, string *_instrument, string *_side, string *_qty, string *_price) {
    if(!((*_clientOrderId).length() && (*_instrument).length() && (*_side).length() && (*_price).length() && (*_qty).length())) {
        reason = "Invalid fields";
        status = 1;    
        return;
    }
    if(find(begin(instruments), end(instruments), *_instrument) == end(instruments)) {
        reason = "Invalid instrument";
        status = 1;   
        return;
    }
    int s = stoi(*_side);
    if(!(s == 1 || s == 2)) {
        reason = "Invalid side";
        status = 1;   
        return;  
    }
    double p = stod(*_price);
    if(p <= 0.0) {
        reason = "Invalid price";
        status = 1;  
        return;
    }
    int q = stoi(*_qty);
    if(q%10 != 0 || q < 10 || q > 1000) {
        reason = "Invalid size";
        status = 1;  
        return;   
    }
    reason = "";
    status = 0;
}   

bool Order::validate() {
    if(status == 1)
        return false;
    return true;
}
// function to write in the execution report file
void Order::execute(ofstream &fout, int execQty) {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    time_t t = chrono::system_clock::to_time_t(now);
    tm* tm = localtime(&t);
    string stat = "";
    if(status == 0) stat = "New";
    else if(status == 1) stat = "Reject";
    else if(status == 2) stat = "Fill";
    else if(status == 3) stat = "PFill";
    fout << cli_id << "," << ord_id << "," << name << "," << side << "," << price << "," << execQty << "," << stat << "," 
    << reason << "," << put_time(tm, "%Y.%m.%d-%H.%M.%S") << "." << setfill('0') << setw(3) << to_string(now_ms.count()) << "\n";
	fout.flush(); 
}


