//
// Created by luca on 16/7/2023.
//
#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <fstream>
#include <set>

using namespace std;

void buildTables(const string &bwt, map<char, int> &cTable, vector<map<char, int>> &occTable) {
    map<char, int> occCounter;
    for (char c: bwt) {
        // Update occCounter
        occCounter[c]++;
        // Update occTable
        occTable.push_back(occCounter);
        // Update cTable
        if (cTable.find(c) == cTable.end()) {
            cTable[c] = 0;
        }
    }
    // Calculate cumulative counts for cTable
    int total = 0;
    for (auto &[c, count]: cTable) {
        int oldCount = count;
        cTable[c] = total;
        total += oldCount;
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
    int n = occTable.size();
    for (auto it: occTable[n - 1]) {
        cout << "\t" << it.first;
    }

    cout << endl;
    for (int i = 0; i < occTable.size(); i++) {
        cout << i << "\t";
        for (auto it: occTable[n - 1]) {
            if (occTable[i].count(it.first)) {
                cout << "\033[31m" << occTable[i][it.first] << "\033[0m" << "\t";
            } else {
                cout << occTable[i][it.first] << "\t";
            }
        }
        cout << endl;
    }
    cout << endl;
}





