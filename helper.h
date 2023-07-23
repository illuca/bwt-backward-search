//
// Created by luca on 16/7/2023.
//
#include<map>

using namespace std;

#ifndef BWTSEARCH_HELPER_H
#define BWTSEARCH_HELPER_H

#endif //BWTSEARCH_HELPER_H

void printCTable(map<char, int> &cTable);

void printOccTable(vector<map<char, int>> &occTable);

void outputCounter(map<char, int> &counter, string filename);

//void buildOccTable(vector<map<char, int>> &occTable, const string &bwt, map<char, int> &COUNTER, set<char> &letters);

void outputCTable(map<char, int> &cTable, string filename);
void outputOccTable(vector<map<char, int>> &occTable, string filename);


//void buildCTable();

void buildTables(const string& bwt, map<char, int>& cTable, vector<map<char, int>>& occTable);
