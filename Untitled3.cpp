#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
void update_record(vector<string> row, const string& inputFileName, const string& outputFileName) {
	// Traverse the file
	double price1 = stod(row[4]);
	int quantity1 = stoi(row[3]);
	int side1 = stoi(row[2]);
	string type1 = row[1];
	string id1 = row[0];
	

    ifstream fout(inputFileName);
    ofstream ff(outputFileName, ios::app);
     if (!fout.is_open() || !ff.is_open()) {
        cout << "Error opening files." << endl;
        return;
    }   
	string line,word;
	vector<string> field;
	int status =0;
	while (getline(fout,line)) {

		field.clear();
		getline(fout, line);
		stringstream s(line);

		while (getline(s, word, ",")) {
			field.push_back(word);
		}
		// 2 for sell 1 for buy
		
		int side2 = stoi(field[2]);  // already exist data
		int quantity2 = stoi(field[3]);	
		int price2 = stoi(field[4]);	
		int field_size = field.size();
		
		if (side1 != side2){
			if (side1==1){
				if (price1 >= price2){ // new entry is buyer
					if (quantity1 < quantity2){
						//update the field entry
						field[3] = to_string(quantity2-quantity1);						
						// fill for buy  and pfill for sell
						ff   << row[0]<<","
						<< row[1]<< ","
						<< row[2]<< ","
						<< "fill"<<","
						<< row[3]<< ","						
						<< row[4]<< "\n";
						status=1;
						// no need to include in fout
						
						ff<< field[0]<<","
						<< field[1]<< ","
						<< field[2]<< ","
						<< "pfill"<<","
						<< field[3]<< ","						
						<< field[4]<< "\n";	
						// need to update pfill in fout																																		
						
					} else if (quantity1 == quantity2){
						// fill for sell and buy
						ff<< field[0]<<","
						<< field[1]<< ","
						<< field[2]<< ","
						<< "fill"<<","
						<< field[3]<< ","						
						<< field[4]<< "\n";
						//need to  delete the entry of field from fout
						
						// fill for sell and buy
						ff   << row[0]<<","
						<< row[1]<< ","
						<< row[2]<< ","
						<< "fill"<<","
						<< row[3]<< ","						
						<< row[4]<< "\n";
						
						// no need to include in fout file
						
						status=1;
						
						//delete(fout)
						
						
						
					} else {
						// update row[3]  
						row[3] = to_string(quantity1 - quantity2);
						// fill for sell pfill for buy
						
						ff<< field[0]<<","
						<< field[1]<< ","
						<< field[2]<< ","
						<< "fill"<<","
						<< field[3]<< ","						
						<< field[4]<< "\n";
						// need to delete the entry  from out
						ff   << row[0]<<","
						<< row[1]<< ","
						<< row[2]<< ","
						<< "pfill"<<","
						<< row[3]<< ","						
						<< row[4]<< "\n";	
						// need to include in fout	
						
						status =1;																						
					}	
				} 
				// need to stop further looping
				break;
				
				
			} else {
				if (price1 <= price2){
					if (quantity1 > quantity2){   //  here 1 is seller 2 is buyer
						//update the row[3]
						row[3] = to_string(quantity1 - quantity2);
					
						// fill for buy  and pfill for sell						
						ff<< field[0]<<","
						<< field[1]<< ","
						<< field[2]<< ","
						<< "fill"<<","
						<< field[3]<< ","						
						<< field[4]<< "\n";	
						// need to delete the entry from fout
						
						ff   << row[0]<<","
						<< row[1]<< ","
						<< row[2]<< ","
						<< "pfill"<<","
						<< row[3]<< ","						
						<< row[4]<< "\n";											
						// need to include in fout

						
						status =1;
					} else if (quantity1 == quantity2){
						// fill for sell and buy
						ff   << field[0]<<","
						<< field[1]<< ","
						<< field[2]<< ","
						<< "fill"<<","
						<< field[3]<< ","						
						<< field[4]<< "\n";
						status=1;
						// need to delete in fout
						
						ff   << row[0]<<","
						<< row[1]<< ","
						<< row[2]<< ","
						<< "fill"<<","
						<< row[3]<< ","						
						<< row[4]<< "\n";						
						// no need to include in fout
						
						status = 1;
					} else {
						// fill for sell pfill for buy
						
						//update field[3]
						field[3] = to_string(quantity2 - quantity1);
						//fill for sell						
						ff   << row[0]<<","
						<< row[1]<< ","
						<< row[2]<< ","
						<< "fill"<<","
						<< row[3]<< ","						
						<< row[4]<< "\n";
						// no need to include in fout
						
						// pfill for buy					
						ff   << field[0]<<","
						<< field[1]<< ","
						<< field[2]<< ","
						<< "pfill"<<","
						<< field[3]<< ","						
						<< field[4]<< "\n";
						status=1;	
						// need to update in fout					
					}
					
				}
				// need to stop further looping
				break; 				
			}
			
		} 
		
	}
	if (status==0){
		
		ff   << row[0]<<","
		<< row[1]<< ","
		<< row[2]<< ","
		<< "new"<<","
		<< row[3]<< ","						
		<< row[4]<< "\n";
		// need to include in fout		
	}

//	if (count == 0)
//		cout << "Record not found\n";

	ff.close();
	fout.close();

//	// removing the existing file
//	remove("execution.csv");
//
//	// renaming the updated file with the existing file name
//	rename("reportcardnew.csv", "exe_report.csv");
}

int main() {
    vector<string> rowData = {"ID", "Rose", "1", "100", "55"};
    string inputFileName = "execution.csv";
    string outputFileName = "exe_report.csv";

    update_record(rowData, inputFileName, outputFileName);

    return 0;
}

