#include <iostream>
#include <cstdio>
#include <map>
#include <bitset>
#include <vector>
#include <set>
#include <chrono>
#include "helper.h"

using namespace std;

//func
FILE* openRLB();

int Occ(int i, char c);

char bwt(int i);

void readBWT();

void buildOccTable(vector<unsigned char> &chars);

string handleString(vector<unsigned char> &chars, int pos);

bool isNum(unsigned char c);

FILE* fd;

//global
vector<map<char, int>> occTable;
map<char, int> cTable;
int recordNum = 0;
int N = 0;
int byteIndex = 0;
char* filename;
vector<pair<int, int>> rlbPosition;
set<int> visited;
map<char, int> counter;
int gap = 2;
bool tableBuilt = false;

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
    int lb = 0, rb = 0;
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
        char c = bwt(i);
        half = c + half;
        i = LF(i, c);
        if (i == start) {
            break;
        }
    }
    record = record + half;
    result[currId] = record;
}

char bwt0(int checkpoint) {
    int ithByte = rlbPosition[checkpoint].first;
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
    fread(buffer, 1, num, fd);
    for (int x = num - 1; x >= 0; x--) {
        if (isChar(buffer[x])) {
            return buffer[x];
        }
    }
}

int getBwtI(int checkpoint) {
    return rlbPosition[checkpoint].first + rlbPosition[checkpoint].second;
}

string readGap(int i, int j) {

    return "";
}

char bwt(int i) {
    int checkpoint = i / gap;
    if (i % gap == 0) {
        return bwt0(checkpoint);
    } else {
        int bwtI = getBwtI(checkpoint);
        string bwt = readGap(bwtI, i);
        return bwt[i - bwtI];
    }
}

int OCC(int i, char c) {

}

int Occ(int i, char c) {
    int checkpoint = i / gap;
    int offset = i % gap;
    if (offset == 0) {
        return occTable[checkpoint][c];
    } else {
        int bwtI = checkpoint * gap;
        int rlbI = rlbPosition[checkpoint].first;
        size_t bytes = 0;
        //TODO 优化成从-5开始读, 这样就不用在下面调用bwt(rlbI)
        char prev = bwt(bwtI);

        //rlbPosition[bwtI] has been recorded
        fseek(fd, rlbI + 1, SEEK_SET);
        int bufferSize = offset + 10;
        vector<unsigned char> buffer(bufferSize);
        unsigned char curr;
        vector<unsigned char> chars;

        int numC = 0;
        string str = "";
        size_t bytesRead = fread(buffer.data(), 1, bufferSize, fd);
        for (size_t i = 0; i < bytesRead; i++) {
            curr = buffer[i];
            if (isChar(curr)) {
                str += handleString(chars, 0);
                chars.clear();
                chars.push_back(curr);
            } else {
                if (chars.empty()) {
                    chars.push_back(prev);
                    chars.push_back(curr);
                } else {
                    chars.push_back(curr);
                }
            }
        }
        str += handleString(chars, 0);
        int duplicate = rlbPosition[checkpoint].second;
        if (isNum(buffer[0])) {
            //bwtI, real_bwtI, i
            // start from bwtI+1 to i-bwtI
            //start with bwtI-duplicate, bwtI, i
            if (prev == c) {
                numC = count(str.begin() + duplicate + 1, str.begin() + i - (bwtI - duplicate) + 1, c);
            } else {
                numC = count(str.begin() + duplicate + 1, str.begin() + i - (bwtI - duplicate) + 1, c);
            }
        } else {
            //start with bwtI-duplicate, bwtI
            numC = count(str.begin(), str.begin() + i - bwtI, c);
        }
        return occTable[checkpoint][c] + numC;
    }
}

pair<char, int> charAndLength(vector<unsigned char> &chars) {
    bitset<28> b;
    string res;
    if (chars.size() == 1) {
        return {chars[0], 1};
    }

    // chars.size() >= 2
    unsigned char c = chars[0];
    // For each byte, extract the least significant 7 bits and append them to our bitset
    for (int i = 1; i < chars.size(); ++i) {
        unsigned char byte = chars[i];
        for (int j = 0; j < 7; ++j) {
            b[(i - 1) * 7 + j] = (byte >> j) & 1;
        }
    }
    unsigned int len = b.to_ulong();
    len = len + 3;
    return {c, len};
}

string handleString(vector<unsigned char> &chars, int pos) {
    bitset<28> b;
    string res;
    if (chars.empty()) {
        return "";
    } else if (chars.size() == 1) {
        res = string(1, chars[0]);
    } else {
        // chars.size() >= 2
        unsigned char c = chars[0];
        // For each byte, extract the least significant 7 bits and append them to our bitset
        for (int i = 1; i < chars.size(); ++i) {
            unsigned char byte = chars[i];
            for (int j = 0; j < 7; ++j) {
                b[(i - 1) * 7 + j] = (byte >> j) & 1;
            }
        }
        unsigned int len = b.to_ulong();
        len = len - pos + 3;
        res = string(len, c);
    }
    return res;
}

void doBuildOccTable(string &str) {
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '[') {
            recordNum++;
        }
        N++;
        counter[str[i]]++;
        //TODO 取余可能要优化
        //TODO mPosition也需要gap, 可以给counter加一个, 这样occTable就包含了mPosition
        if ((N - 1) % gap == 0) {
            rlbPosition.emplace_back(byteIndex, i);
            occTable.push_back(counter);
        }
    }
}

void buildOccTable(vector<unsigned char> &chars) {
    string str = handleString(chars, 0);
    // do buildOccTable
    doBuildOccTable(str);
    byteIndex += chars.size();
}

void buildCTable() {
    int total = 0;
    for (auto it: counter) {
        //value of current key = total sum of values up to previous key
        char c = it.first;
        cTable[c] = total;
        total += it.second;
    }
}

void buildOCCTable() {
    const size_t bufferSize = 2048;
    unsigned char buffer[bufferSize];
    char curr;
    vector<unsigned char> chars;
    while (size_t bytesRead = fread(buffer, 1, bufferSize, fd)) {
        for (size_t i = 0; i < bytesRead; ++i) {
            curr = buffer[i];
            if (isChar(curr)) {
                if (chars.empty()) {
                    chars.push_back(curr);
                } else {
                    pair<char, int> rle = charAndLength(chars);

                }
            } else {
                chars.push_back(curr);
            }
        }
    }
    buildOccTable(chars);

}

void readBWT() {
    const size_t bufferSize = 2048;
    vector<unsigned char> buffer(bufferSize);
    //TODO 可能需要提前分配内存效率更高
    char curr;
    vector<unsigned char> chars;
    while (size_t bytesRead = fread(buffer.data(), 1, bufferSize, fd)) {
        for (size_t i = 0; i < bytesRead; ++i) {
            curr = buffer[i];
            if (isChar(curr)) {
                buildOccTable(chars);
                chars.clear();
                chars.push_back(curr);
            } else {
                chars.push_back(curr);
            }
        }
    }
    buildOccTable(chars);
}

int main(int argc, char* argv[]) {
    string p = argv[3];
    string idxFile = argv[2];
    filename = argv[1];
    fd = fopen(filename, "r");
    readBWT();
    set<int> ids;
    buildCTable();
//    counter.clear();

    pair<int, int> range = backwardSearch(p);
    map<int, string> res;
    if (range.first != -1) {
        int start = range.first;
        int end = range.second;
//        cout << "hit: " << +end - start + 1 << "\n";
        while (start <= end) {
            reverseBWT(start, end, ids, res);
            start++;
        }
        for (auto it: res) {
            cout << it.second << endl;
        }
    } else {
//        cout << "hit: 0\n";
    }
    fclose(fd);
    return 0;
}