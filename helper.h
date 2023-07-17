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

//void buildOccTable(vector<map<char, int>> &occTable, const string &bwt, map<char, int> &counter, set<char> &letters);

//void buildCTable();

void buildTables(const string& bwt, map<char, int>& cTable, vector<map<char, int>>& occTable);
