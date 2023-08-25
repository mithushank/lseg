#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <numeric>
#include <vector>
#include <sstream>

using namespace std;

struct Investment
{
    int client_id;
    string name;
    int side;
    int amount;
    int prize;
    int order_id;
};

void get_data(const string &filename, vector<Investment> &data)
{
    ifstream fin(filename);

    if (!fin.is_open())
        return;

    string line;
    if (!getline(fin, line))
        return;

    while (getline(fin, line))
    {
        stringstream s(line);
        vector<string> fields;
        string field;

        while (getline(s, field, ','))
        {
            fields.push_back(field);
        }

        Investment inv;
        inv.name = fields[1];
        inv.client_id = stoi(fields[0]);
        inv.side = stoi(fields[2]);
        inv.prize = stoi(fields[3]);
        inv.order_id = 0;

        data.push_back(inv);
    }
}

void Save_Data(const string &filename, const vector<Investment> &data)
{

    fstream fout(filename);
    if (!fout.is_open())
        return;

    fout << "client_id, name, side, prize, order_id\n";

    for (auto &inv : data)
    {
        fout << inv.client_id << ","
             << inv.name << ","
             << inv.side << ","
             << inv.prize << ","
             << inv.order_id << "\n";
    }



}

int main()
{
    vector<Investment> Data_investment;
    get_data("file.csv", Data_investment);


    int count = 1;
    for (auto &inv : Data_investment)
    {

        inv.order_id = count;
        count = count + 1;
    }

    Save_Data("file.csv", Data_investment);

    return 0;
}
