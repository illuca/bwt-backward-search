#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <set>
#include "helper.h"

using namespace std;

vector<map<char, int>> occTable;
map<char, int> cTable;
set<char> letters;
set<int> ids;
int recordNum;

string T = "mississippi";
//string bwt = "ipssmpissii";
//string p = "issi";

//small1
// [1]ban[2]banana[3]band[4]bandage[5]bin[6]bind[7]binding
string bwt = "[[[[[[[gnadend1234567ndbnbbb]]]]]]]nnnngnabbbdaiaaaiaii";
string p = "in";

int LF(int i, char c) {
    return cTable.at(c) + Occ(i, c) - 1;
}

pair<int, int> backwardSearch(string &p) {
    int start = 0;
    int end = bwt.size() - 1;
    for (int i = p.size() - 1; i >= 0; i--) {
        char c = p[i];
        if (cTable.count(c) == 0) {
            return {-1, -1};  // Pattern not found
        }
        start = cTable.at(c) + (start > 0 ? Occ(start - 1, c) : 0);
        end = cTable.at(c) + Occ(end, c) - 1;
        if (start > end) {
            return {-1, -1};
        }
    }
    return {start, end};
}

void reverseBWT(int position) {
    //if we only get [2]bana, then we can find the remaining by sandwich
    //read until get the identifier
    //we record the ] position as right
    //then identifier = record[0-right]
    //then we can know the next record's identifier
    //find the [
    //and get the remaining chars
    int i = position;
    int lb, rb;
    string record;
    while (true) {
        record = bwt[i] + record;

        if (bwt[i] == ']') {
            //]ba
            rb = record.size();
        }
        if (bwt[i] == '[') {
            //[12]ba
            lb = record.size();
            break;
        }
        i = LF(i, bwt[i]);
    }
    //lb-rb-1 is the size of "id"
    int currId = stoi(record.substr(1, lb - rb - 1));
    if(ids.count(currId)) {
        return;
    }else{
        ids.insert(currId);
    }
    //find the next record id
    int nextId = currId + 1;
    string nextIdentifier = "[" + to_string(nextId) + "]";
    pair<int,int> res = backwardSearch(nextIdentifier);
    string half;
    if (res.first == -1) {
        //it is the last record
        int firstId = currId - recordNum + 1;
        string firstIdentifier = "[" + to_string(firstId) + "]";
        //find the first record
        res = backwardSearch(firstIdentifier);
    }
    i = res.first;
    while(true){
        half=bwt[i]+half;
        i = LF(i, bwt[i]);
        if(i==position){
            break;
        }
    }

    record = record + half;
    cout << record << endl;
    //if it is the last record, we cannot use the sandwich
    //but we know the size of bwt, we can record the smallest identifier
    //and the max identifier during building cTable

}

int main(int argc, char* argv[]) {
    map<char, vector<pair<int, int>>> occurrence;
    buildCTable();
    buildOccTable();
    printCTable(cTable);
    printOccTable(occTable);
    pair<int, int> res = backwardSearch(p);
    if (res.first != -1) {
        int start = res.first;
        int end = res.second;
        cout << "hit: " << +end - start + 1 << "\n";
        while (start <= end) {
            reverseBWT(start);
            start++;
        }
    } else {
        cout << "hit: 0\n";
    }
    return 0;
}

