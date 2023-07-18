#include <iostream>
#include <map>
#include <fstream>
using namespace std;
int main(int argc, char *argv[]) {
    char* filename = argv[1];
    FILE* file = fopen(filename, "r");
    size_t size=1024;
    char buffer[size];
    map<unsigned char, int> counter;
    while(size_t bytesRead = fread(buffer, 1, size, file)) {
        for(size_t i=0; i<bytesRead; i++) {
            counter[buffer[i]]++;
        }
    }
    fclose(file);
    
    string outFilename = filename;
    ofstream out(outFilename+"-count");
    out << "----cTable----" << endl;
    for(auto it:counter) {
        out << it.first << ": " << it.second << endl;
    }
    out.close();
    
    
    return 0;
}