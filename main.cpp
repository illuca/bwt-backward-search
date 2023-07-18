#include <iostream>
#include <cstdio>
#include <map>
#include <fstream>
#include <queue>
#include <bitset>

#include <vector>
#include <set>
#include <chrono>
#include "helper.h"

using namespace std;

//func
FILE* openRLB();

int Occ(int i, char c);

char bwt(int index);

bool isNum(unsigned char c);

FILE* fd;

//global
vector<map<char, int>> occTable;
map<char, int> cTable;
int recordNum=0;
int N = 0;
int byteIndex=0;
ofstream fout;
char* filename;
map<int, int> mPosition;
set<int> visited;

int C(char c) {
    if (cTable.count(c) == 0) {
        return -1;
    }
    return cTable.at(c);
}

bool isChar(unsigned char c) {
    return !isNum(c);
}

bool isNum(unsigned char c) {
    return c >> 7 == 1;
}

int LF(int i, char c) {
    return cTable.at(c) + Occ(i, c) - 1;
}

pair<int, int> backwardSearch(string &p) {
    int start = 0;
    int end = N - 1;
    for (int i = p.size() - 1; i >= 0; i--) {
        char c = p[i];
        start = C(c) + (start > 0 ? Occ(start - 1, c) : 0);
        end = C(c) + Occ(end, c) - 1;
        if (start > end) {
            return {-1, -1};
        }
    }
    return {start, end};
}

void reverseBWT(const int &start, const int &end, set<int> &ids, map<int, string> &result) {
    int i = start;
    int lb, rb;
    string record;
    if (visited.count(i)) {
        return;
    }
    while (true) {
        if (start <= i && i <= end) {
            visited.insert(i);
        }
        char c = bwt(i);
        record = c + record;
        if (c == ']') {
            //]ba
            rb = record.size();
        }
        if (c == '[') {
            //[12]ba
            lb = record.size();
            break;
        }
        i = LF(i, c);
    }
    //lb-rb-1 is the size of "id"
    int currId = stoi(record.substr(1, lb - rb - 1));
    if (ids.count(currId)) {
        return;
    } else {
        ids.insert(currId);
    }
    //find the next record id
    int nextId = currId + 1;
    string nextIdentifier = "[" + to_string(nextId) + "]";
    pair<int, int> range = backwardSearch(nextIdentifier);
    string half;
    if (range.first == -1) {
        //it is the last record
        int firstId = currId - recordNum + 1;
        string firstIdentifier = "[" + to_string(firstId) + "]";
        //find the first record
        range = backwardSearch(firstIdentifier);
    }
    i = range.first;
    while (true) {
        if (start <= i && i <= end) {
            visited.insert(i);
        }
        half = bwt(i) + half;
        i = LF(i, bwt(i));
        if (i == start) {
            break;
        }
    }
    record = record + half;
    result[currId] = record;
}

void buildOccTable(string str, map<char, int> &counter) {
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '[') {
            recordNum++;
        }
        counter[str[i]]++;
        occTable.push_back(counter);
        mPosition[N] = byteIndex;
        N++;
    }
}

char bwt(int index) {
    //if after mapping, what we read from the file is a number, then backward util we find the character
    int ithByte = mPosition[index];
    //the max needs 4 bytes
    int num = 5;
    // 1st byte is 0
    if (ithByte >= 4) {
        fseek(fd, ithByte - 4, SEEK_SET);
    } else {
        fseek(fd, 0, SEEK_SET);
        num = ithByte + 1;
    }
    unsigned char buffer[num];
    char correct = "[[[[[[[gnadend1234567ndbnbbb]]]]]]]nnnngnabbbdaiaaaiaii"[index];

    if (fread(buffer, 1, num, fd) != num) {
        printf("Failed to read bytes at position %d and %d\n", ithByte - 1, ithByte);
        return 1;
    }
    for (int i = num - 1; i >= 0; i--) {
        if (isChar(buffer[i])) {
            return buffer[i];
        }
    }
    return '#';
}

void handleString(vector<unsigned char> &chars, map<char, int> &counter) {
    bitset<28> b;
    string res;
    if (chars.size()==0) {
        return;
    }
    if(chars.size()==1) {
        res = string(1, chars[0]);
    }
    if (chars.size() >= 2) {
        unsigned char c = chars[0];
        // For each byte, extract the least significant 7 bits and append them to our bitset

        for (int i = 1; i < chars.size(); ++i) {
            unsigned char byte = chars[i];
            for (int j = 0; j < 7; ++j) {
                b[(i - 1) * 7 + j] = (byte >> j) & 1;
            }
        }
        unsigned int len = b.to_ulong();
        res = string(len + 3, c);
    }
    string output = filename;
//    fout << res;

    buildOccTable(res, counter);
    byteIndex += chars.size();
}

void readBWT(map<char, int> &counter) {
    const size_t bufferSize = 1024;
    vector<unsigned char> buffer(bufferSize);
    char curr, prev;

    //TODO 可能需要提前分配内存效率更高
    vector<unsigned char> chars;

    string str;
    int totalBytesRead = 0;
    while (size_t bytesRead = fread(buffer.data(), 1, bufferSize, fd)) {
        for (size_t i = 0; i < bytesRead; ++i) {
            totalBytesRead++;
            curr = buffer[i];
            if (isChar(curr)) {
                handleString(chars, counter);
                chars.clear();
                chars.push_back(curr);
            }else {
                chars.push_back(curr);
            }
        }
    }
    cout << "totalBytesRead" << totalBytesRead << endl;
    handleString(chars, counter);
}

int Occ(int i, char c) {
    return occTable[i][c];
}

void buildCTable(const map<char, int> &counter) {
    int total = 0;
    for (auto it: counter) {
        //value of current key = total sum of values up to previous key
        char c = it.first;
        cTable[c] = total;
        total += it.second;
    }
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

    file << "----mPosition----\n";
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

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    //T = mississippi
    //string bwt = "ipssmpissii";
    //string p = "issi";

    //small1 = [1]ban[2]banana[3]band[4]bandage[5]bin[6]bind[7]binding
//    string bwt = "[[[[[[[gnadend1234567ndbnbbb]]]]]]]nnnngnabbbdaiaaaiaii";
//    string bwt;
    string p = argv[2];
    map<char, int> counter;
    filename = argv[1];
    fd = fopen(filename, "r");

    string output = filename;
    fout.open(output + "-bwt", ios::out);
    readBWT(counter);
    set<int> ids;
    buildCTable(counter);

//    printCTable(cTable);
//    printOccTable(occTable);
    outputCounter(counter, filename);
//    outputCTable(cTable, filename);
    outputOccTable(occTable, filename);
    outputMPosition(mPosition, filename);

    pair<int, int> range = backwardSearch(p);
    map<int, string> res;
    if (range.first != -1) {
        int start = range.first;
        int end = range.second;
        cout << "hit: " << +end - start + 1 << "\n";
        while (start <= end) {
            reverseBWT(start, end, ids, res);
            start++;
        }
        for (auto it: res) {
            cout << it.second << endl;
        }
    } else {
        cout << "hit: 0\n";
    }
    fclose(fd);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by function: "
              << duration.count() << " microseconds" << std::endl;
    fout.close();
    return 0;
}

