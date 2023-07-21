//
// Created by luca on 16/7/2023.
//
#include <iostream>
#include <map>
#include <vector>
#include <iomanip>
#include <fstream>

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


void outputCounter(map<char, int> &counter, string filename) {
    ofstream file(filename + "-counter");

    file << "----cTable----\n";
    for (const auto &pair: counter) {
        file << pair.first << ": " << pair.second << endl;
    }

    file.close();  // closing the file after writing to it
}

void outputCTable(map<char, int> &cTable, string filename) {
    ofstream file(filename + "-c");

    file << "----cTable----\n";
    for (const auto &pair: cTable) {
        file << pair.first << ": " << pair.second << endl;
    }

    file.close();  // closing the file after writing to it
}

void outputMPosition(map<int, int> &mPosition, string filename) {
    ofstream file(filename + "-position");

    file << "----rlbPosition----\n";
    for (const auto &pair: mPosition) {
        file << pair.first << ": " << pair.second << endl;
    }

    file.close();  // closing the file after writing to it
}

void outputOccTable(vector<map<char, int>> &occTable, string filename) {
    ofstream file(filename + "-occ");
    file << "----occTable-----\n";
    int n = occTable.size();
    file << setw(8) << "\\";
    for (auto it: occTable[n - 1]) {
        file << setw(8) << it.first;
    }
    file << endl;
    for (int i = 0; i < occTable.size(); i++) {
        file << setw(8) << i;
        for (auto it: occTable[n - 1]) {
            if (occTable[i].count(it.first)) {
                file << setw(8) << occTable[i][it.first];
            } else {
                file << setw(8) << occTable[i][it.first];
            }
        }
        file << '\n';
    }
    file << setw(8) << "\\";
    for (auto it: occTable[n - 1]) {
        file << setw(8) << it.first;
    }
    file.close();
}




