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

int Occ(int i, char c);

void buildOccTable();

void buildCTable();
