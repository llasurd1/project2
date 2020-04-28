#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) {

	//argv[1] = input file	argv[2] = output file
	vector<long long> address;
	vector<string> behaviors;
	ofstream output;
	output.open(argv[2]);
	unsigned long long addr;
	string behavior;
	ifstream infile(argv[1]);
	
	while (infile >> behavior >> hex >> addr) {
		address.push_back(addr);
		if (behavior == "L") {
			behaviors.push_back("L");
		}		
		else {
			behaviors.push_back("S");
		}
	}

	//1) direct 
	vector<int> siz{1024, 4096, 16384, 32768};
	for (int z = 0; z < 4; z++) {
		int correct = 0;
		int blocks = siz[z] / 32;
		long long cache[blocks];
		int valid[blocks];

		for (int i = 0; i < blocks; i++) {
			cache[i] = -1;
			valid[i] = 0;
		}
		for (int i = 0; i < address.size(); i++) {
		 
		 
			long long addr1 = address[i] >> 5;
			int index = addr1 % blocks;
			int tag = addr1 / blocks;
			if (valid[index] == 1) {
				if (cache[index] == tag) {
					correct++;
				}
				else {
				  cache[index] = tag;
				}
			}
			if (valid[index] == 0) {
				valid[index] = 1;
				cache[index] = tag;
			}
		}
		output << correct << ", " << address.size() << ";	";
		
	}
	output << "\n";
	//2) set associateive
	vector<int>way{2, 4, 8, 16}; 
	for (int z = 0; z < 4; z++) {
	  int correct = 0;
		int sets = 512 / way[z];
		long long cache[sets][way[z]];
		int lru[sets][way[z]];
		for (int i = 0; i < sets; i++) {
			for (int j = 0; j < way[z]; j++) {
				lru[i][j] = -1;
				cache[i][j] = -1;
			}
		}
		for (int i = 0; i < address.size(); i++) {
		  
		 
			long long addr1 = address[i] >> 5;
			int set = addr1 % sets;
			long long tag = addr1 / sets;
			int c = 0;
			if(i<3) {
			  cout << way[z] << " : way\n";
			  cout <<  address[i] << " :address\n";
			  cout << addr1 << " :bit shifted address\n";
			  cout << tag << " :tag\n";
			  cout << set << " :set\n";
			}
			for (int j = 0; j < way[z]; j++) {
			  if(c==0) {
			   if (cache[set][j] == -1) {
					cache[set][j] = tag;
					lru[set][j] = i;
					c = 1;
				       
				       
				}
				else if (cache[set][j] == tag) {
					correct++;
					lru[set][j] = i;
					c = 1;
					
				}
			  }
			}
			if (c == 0) {
				int temp = lru[set][0];
				int x = 0;
				for (int j = 0; j < way[z]; j++) {
					if (lru[set][j]<= temp) {
					  temp = lru[set][j];
						x = j;
					}
				}
				lru[set][x] = i;
				cache[set][x] = tag;
			}
		}
		output << correct << ", " << address.size() << ";	";
	}
	output << "\n";
	//fully associative lru + hot/cold
	for (int z = 0; z < 2; z++) {
		int correct = 0;
		int blocks = 512;
		long long cache[blocks];
		int lru[blocks];
		for (int w = 0; w < blocks; w++) {
			cache[w] = -1;
			lru[w] = -1;
		}
		//lru
		if (z == 0) {
			for (int i = 0; i < address.size(); i++) {
				long long addr1 = address[i] >> 5;
				int c = 0;
				for (int j = 0; j < blocks; j++) {
				  if(c==0) {
				   if (cache[j] == -1) {
						cache[j] = addr1;
						lru[j] = i;
						c = 1;
					}
					else if (cache[j] == addr1) {
						correct++;
						lru[j] = i;
						c = 1;
					}
				  }
				}
				if (c == 0) {
					int temp = lru[0];
					int x = 0;
					for (int j = 0; j < blocks; j++) {
						if (lru[j] < temp) {
							temp = lru[j];
							x = j;
						}
					}
					cache[x] = addr1;
					lru[x] = i;
				}
			}
			
		}
		output << correct << ", " << address.size() << ";\n";
		//hot/cold
		if(z==1){
			
		}
	}
	//set associative with write miss

	for (int z = 0; z < 4; z++) {
		int correct = 0;
		int sets = 512/way[z];
		long long cache[sets][way[z]];
		int lru[sets][way[z]];
	
		for (int i = 0; i < sets; i++) {
			for (int j = 0; j < way[z]; j++) {
				lru[i][j] = -1;
				cache[i][j] = -1;
			}
		}
		for (int i = 0; i < address.size(); i++) {
			long long addr1 = address[i] >> 5;
			int set = addr1 % sets;
			int tag = addr1 / sets;
			int c = 0;
			for (int j = 0; j < way[z]; j++) {
			  if(c==0) {
				if (cache[set][j] == -1) {
						cache[set][j] = tag;
						lru[set][j] = i;
						c = 1; 
				}
				else if (cache[set][j] == tag) {
					correct++;
					lru[set][j] = i;
					c = 1;
				}
			  }
			}
			if (behaviors[i] == "L" && c == 0) {
				int temp = lru[set][0];
				int x = 0;
				for (int j = 0; j < way[z]; j++) {
					if (lru[set][j] < temp) {
					  temp = lru[set][j];
					  x = j;
					}
				}
				lru[set][x] = i;
				cache[set][x] = tag;
			}

		}
		output << correct << ", " << address.size() << ";	";
	}
	output << "\n";
	//prefetch
	for (int z = 0; z < 4; z++) {
		int correct = 0;
		int sets = 512 / way[z];
		long long cache[sets][way[z]];
		int lru[sets][way[z]];
	       
		for (int i = 0; i < sets; i++) {
			for (int j = 0; j < way[z]; j++) {
				lru[i][j] = -1;
				cache[i][j] = -1;
			}
		}
		for (int i = 0; i < address.size(); i++) {
			long long addr1 = address[i] >> 5;
			int set = addr1 % sets;
			int tag = addr1 / sets;
			int set2 = set+1;
			int c = 0;
			int c2 = 0;
			for (int j = 0; j < way[z]; j++) {
			  if(c==0) {
			       if (cache[set][j] == -1) {
					cache[set][j] = tag;
					lru[set][j] = i;
					c = 1;				
				}
				else if (cache[set][j] == tag) {
					correct++;
					lru[set][j] = i;
					c = 1;
				}
			  }
			  if(c2 ==0) {
				if (cache[set2][j] == -1) {
					cache[set2][j] = tag;
					lru[set2][j] = i;
					c2 = 1;
				}
				else if (cache[set2][j] == tag) {
					lru[set2][j] = i;
					c2 = 1;
				}
			  }
			}
			if (c == 0) {
				int temp = lru[set][0];
				int x = 0;
				for (int j = 0; j < way[z]; j++) {
					if (lru[set][j] < temp) {
						x = j;
						temp = lru[set][j];
					}
				}
				lru[set][x] = i;
				cache[set][x] = tag;
			}
			if (c2 == 0) {
				int temp = lru[set2][0];
				int x = 0;
				for (int j = 0; j < way[z]; j++) {
					if (lru[set2][j] < temp) {
					  temp = lru[set2][j];
						x = j;
					}
				}
				lru[set2][x] = i;
				cache[set2][x] = tag;
			}
		}
		output << correct << ", " << address.size() << ";	";
	}
	output << "\n";
	//prefetch on miss
	for (int z = 0; z < 4; z++) {
		int correct = 0;
		int sets = 512 / way[z];
		long long cache[sets][way[z]];
		int lru[sets][way[z]];
	       
		for (int i = 0; i < sets; i++) {
			for (int j = 0; j < way[z]; j++) {
				lru[i][j] = -1;
				cache[i][j] = -1;
			}
		}
		for (int i = 0; i < address.size(); i++) {
			long long addr1 = address[i] >> 5;
			int set = addr1 % sets;
			int tag = addr1 / sets;
			long long addr2;
			int set2;
			int tag2;
			int c = 0;
			int c2 = 0;
			if (i < address.size() - 1) {
				addr2 = address[i + 1] >> 5;
				tag2 = addr2 / sets;
				set2 = addr2 % sets;
			}
			for (int j = 0; j < way[z]; j++) {
			  if(c==0) {
			       if (cache[set][j] == -1) {
					cache[set][j] = tag;
					lru[set][j] = i;
					c = 1; 
				}
				else if (cache[set][j] == tag) {
					correct++;
					lru[set][j] = i;
					c = 1;
				}
			  }
			}
			if (c == 0) {
				int temp = lru[set][0];
				int x = 0;
				for (int j = 0; j < way[z]; j++) {
					if (lru[set][j] < temp) {
						x = j;
						temp = lru[set][j];
					}
				}
				lru[set][x] = i;
				cache[set][x] = tag;
				if (i < address.size() - 1) {
					for (int n = 0; n < way[z]; n++) {
					  if(c2==0) {
						if (cache[set2][n] == -1) {
							cache[set2][n] = tag2;
							lru[set2][n] = i;
							c2 = 1;
						}
						else if (cache[set2][n] == tag2) {
							correct++;
							lru[set2][n] = i;
							c2 = 1;
						}
					  }
					}
					if (c2 == 0) {
						int temp2 = lru[set2][0];
						int x2 = 0;
						for (int j2 = 0; j2 < way[z]; j2++) {
							if (lru[set2][j2] < temp2) {
								x2 = j2;
								temp2 = lru[set2][j2];
							}
						}
						lru[set2][x2] = i;
						cache[set2][x2] = tag2;
					}
				}
			}
		}
		output << correct << ", " << address.size() << ";	";
	}
	infile.close();
}
