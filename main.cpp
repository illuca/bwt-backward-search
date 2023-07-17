#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <set>
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
int recordNum;
int N = 0;
int byteIndex;
char* filename;
map<int, int> mPosition;

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

void reverseBWT(int &position, set<int> &ids) {
    int i = position;
    int lb, rb;
    string record;
    while (true) {
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
    pair<int, int> res = backwardSearch(nextIdentifier);
    string half;
    if (res.first == -1) {
        //it is the last record
        int firstId = currId - recordNum + 1;
        string firstIdentifier = "[" + to_string(firstId) + "]";
        //find the first record
        res = backwardSearch(firstIdentifier);
    }
    i = res.first;
    while (true) {
        half = bwt(i) + half;
        i = LF(i, bwt(i));
        if (i == position) {
            break;
        }
    }
    record = record + half;
    cout << record << endl;
}

void buildOccTable(string str, map<char, int> &counter) {
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '[') {
            recordNum++;
        }
        counter[str[i]]++;
        occTable.push_back(counter);
//        occTable.push_back(counter);
        mPosition[N + i] = byteIndex;
    }
}

char bwt(int index) {
    //if after mapping, what we read from the file is a number, then backward util we find the character
    int ithByte = mPosition[index];
    int num = 3;
    // 1st byte is 0
    if (ithByte >= 2) {
        fseek(fd, ithByte - 2, SEEK_SET);
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

FILE* openRLB() {
    return fopen(filename, "r");
}

void readBWT(map<char, int> &counter) {
    const size_t bufferSize = 1024;
    vector<unsigned char> buffer(bufferSize);
    unsigned char curr, prev;
    string bwt;

    while (size_t bytesRead = fread(buffer.data(), 1, bufferSize, fd)) {
        for (size_t i = 0; i < bytesRead; ++i) {
            curr = buffer[i];
            if (isNum(curr)) {
                int len = (curr & 0b01111111) + 3;
                bwt += string(len - 1, prev);
                string tmp = string(len - 1, prev);
                buildOccTable(tmp, counter);
                N += len - 1;
                byteIndex++;
            } else {
                bwt += curr;
                string tmp = string(1, curr);
                buildOccTable(tmp, counter);
                N += 1;
                byteIndex++;
                prev = curr;
            }
        }
    }
    cout << "bwt: " << bwt << "\n";
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

int main(int argc, char* argv[]) {
    //T = mississippi
    //string bwt = "ipssmpissii";
    //string p = "issi";

    //small1 = [1]ban[2]banana[3]band[4]bandage[5]bin[6]bind[7]binding
//    string bwt = "[[[[[[[gnadend1234567ndbnbbb]]]]]]]nnnngnabbbdaiaaaiaii";
//    string bwt;
    string p = "in";
    map<char, int> counter;
    filename = argv[1];
    fd = fopen(filename, "r");

    readBWT(counter);
    set<int> ids;
    buildCTable(counter);

//    printCTable(cTable);
//    printOccTable(occTable);
    pair<int, int> res = backwardSearch(p);
    if (res.first != -1) {
        int start = res.first;
        int end = res.second;
        cout << "hit: " << +end - start + 1 << "\n";
        while (start <= end) {
            reverseBWT(start, ids);
            start++;
        }
    } else {
        cout << "hit: 0\n";
    }
    fclose(fd);
    return 0;
}

