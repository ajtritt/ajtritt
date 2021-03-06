#include <cstdio>
#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>

using namespace std;

void printStats(char* file) {
	ifstream fa_in(file);

	char c;
	if (fa_in) {
		c = (char) fa_in.get();
		if (c != '>') {
			cerr << "File " << file << " not in fasta format\n";
			return;
		}
	} else {
		cerr << file << " does not exist.\n";
		return;
	}
	int tot = 0;
	int numN = 0;
	int numNuc = 0;
	int len = 0;
	int numGaps = 0;
 	char buf[256];	
	fa_in.getline(buf,256,'\n');	
	vector<int> data;
	bool inGap = false;
	while (fa_in.peek() != -1){
		c = (char) fa_in.get();
		if (c == '>'){
			data.push_back(len);
			len = 0;
			fa_in.getline(buf,256,'\n');
		} else if (c == '\n') {
			continue;
		}  else {
			if (c == 'n' || c == 'N') {
				numN++;
				inGap = true;
			} else {
				numNuc++;
				if (inGap) {
					inGap = false;
					numGaps++;	
				}
			}
			len++;
		}
	}
	data.push_back(len);
	tot = numN + numNuc;
	sort (data.begin(),data.end());
	int tally = 0;
	int n50 = 0;
	int half = tot/2;
	for (vector<int>::iterator it = data.begin(); it != data.end(); it++){
		tally += *it;
		if (tally > half) {
			n50 = *it;
			break;
		}
	}

	cout << "Fasta_File:   " << file << endl	
	     << "Num Ctgs:     " << data.size() << endl	
	     << "L50:          " << n50 << endl 	
	     << "Avg Ctg Len:  " << (tot/data.size()) << endl
	     << "Max Ctg Len:  " << data.back() << endl  
	     << "Min Ctg Len:  " << *data.begin() << endl  
	     << "Total Bases:  " << tot << endl
	     << "Num Nucs:     " << numNuc << endl
	     << "Num Unks:     " << numN << endl
	     << "Num Gaps:     " << numGaps << endl;
}

void usage() {
	cout << "Usage: sumfasta [options] <fasta1> <fasta2> ... <fastaN>\n" <<
	        "   where options are:\n" <<
	        "   --header           print a header indicating the fields of the output\n" <<
	        "   -h, --help         print this message and exit\n";
}

int main(int argc, char** argv) {
	if (argc == 1 || (argc == 2 && (strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0))) {
		usage();
		return 1;	
	}
	int start = 1;
	if (strcmp(argv[1],"--header") == 0 ){
		cout << "Fasta_File\tNum_Ctgs\tN50\tAvgCtgLen\tMaxCtgLen\tMinCtgLen\tTotal_Bases\tNum_Nucs\tNum_Unks\tNumGaps\n"; 
		start++;
	}

	for (int i = start; i < argc; i++){
		printStats(argv[i]);
		
	}

}	
