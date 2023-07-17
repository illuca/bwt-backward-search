//
// Created by luca on 16/7/2023.
//
#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <set>

using namespace std;

extern set<char> letters;
extern vector<map<char, int>> occTable;
extern map<char, int> cTable;
extern string bwt;
extern int recordNum;

int Occ(int i, char c) {
    return occTable[i][c];
}


void buildOccTable() {
    //occTable has the same size of bwt code
    //but for large file, we need gap
    occTable.resize(bwt.size());
    //for each character, we count its occurrence at any position
    map<char, int> count;
    for (int i = 0; i < bwt.size(); i++) {
        for (auto c: letters) {
            if (!count.count(c)) {
                count[c] = 0;
            }
        }
        char c = bwt[i];
        count[c]++;
        occTable[i] = count;
    }
}

void buildCTable() {
    map<char, int> count;
    for (int i = 0; i < bwt.size(); i++) {
        //build cTable
        char c = bwt[i];
        count[c]++;
        if(c=='['){
            recordNum++;
        }
    }
    int total = 0;
    for (auto it: count) {
        //value of current key = total sum of values up to previous key
        char c = it.first;
        cTable[c] = total;
        total += it.second;
        letters.insert(c);
    }
}

void printCTable(map<char, int> &cTable) {
    cout << "----cTable----\n";
    for (const auto &pair: cTable) {
        cout << pair.first << ": " << pair.second << endl;
    }
    cout << endl;
}

void printOccTable(vector<map<char, int>> &occTable) {
    cout << "----occTable-----\n";
    for (auto c: letters) {
        cout << "\t" << c;
    }
    cout << endl;
    for (int i = 0; i < occTable.size(); i++) {
        cout << i << "\t";
        for (auto pair: occTable[i]) {
            if (pair.second != 0) {
                cout << "\033[31m" << pair.second << "\033[0m" << "\t";
            } else {
                cout << pair.second << "\t";
            }
        }
        cout << endl;
    }
    cout << endl;
}


