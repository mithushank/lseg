#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void create() {
	// existing file pointer
    fstream fin;

    // Open an existing file
    fin.open("reportcard.csv", ios::in);

	
	
    // file pointer
    fstream fout;
    fstream ff;

    // opens an existing csv file or creates a new file.
    fout.open("executive.csv", ios::out | ios::app);
    ff.open("exe_report.csv", ios::out | ios::app);

    if (!fin.is_open()) {
        cout << "Error opening the file." << endl;
        return;
    }
    
    if (!ff.is_open()) {
        cout << "Error opening the final file." << endl;
        return;
    }
    
    string line, word;
    vector<string> row;
    int count =0;
    while (!fin.eof()){
    	row.clear();
    	getline (fin, line);
    	stringstream s(line);
    	
    	while (getline(s, word, ',')){
    		row.push_back(word);
		}
		row.push_back("0");
		
		int row_size = row.size();
		
		if (!fin.eof()){
			count+=1;
			fout << row[row_size-1]<< "\n";
			string str = "Ord";
			fout <<  str <<",";	
					
			for (int i=0;i < row_size-1; i++){
				fout << row[i]<<",";
			}
		}
		
		if (fin.eof()){
			break;
		}
	}


    fout.close(); // Close the file after writing
 
    cout << "Data written to file." << endl;
}

int main() {
    create(); // Call the create function
    return 0;
}

