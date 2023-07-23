#include <map>
#include <bitset>
#include <vector>
#include <set>
#include <string>
//#include "helper.h"
using namespace std;
//func
int Occ(int i, char c);

char bwt(int &i);

void buildOccTable();

typedef struct {
    unsigned char c;
    int length;
    int byte;
} RLE;

vector<RLE> handleRLE(vector<unsigned char> &buffer, size_t &bytesRead, vector<unsigned char> &chars);

pair<unsigned char, int> charAndLength(vector<unsigned char> &chars);

bool isNum(unsigned char &c);

void doBuildOccTable(RLE &rle);

RLE clearChars(vector<unsigned char> &chars);

FILE* FD;

typedef struct {
    int idx;
    int pos;
    unsigned char c;
} INFO;

//global
vector<map<char, int>> occTable;
map<char, int> cTable;
int RECORD_NUM = 0;
int BWT_NUM = 0;
int RLB_SIZE = 0;
int RLB_INDEX = 0;
char* FILENAME;
vector<INFO> rlbPosition;
set<int> VISITED;
map<char, int> COUNTER;

int GAP = 0;

int C(char &c) {
    if (cTable.count(c) == 0) {
        return -1;
    }
    return cTable.at(c);
}

bool isChar(unsigned char &c) {
    return !isNum(c);
}

bool isNum(unsigned char &c) {
    return c >> 7 == 1;
}

int LF(int &i, char &c) {
    return cTable.at(c) + Occ(i, c) - 1;
}

pair<int, int> backwardSearch(string &p) {
    int start = 0;
    int end = BWT_NUM - 1;
    for (int i = p.size() - 1; i >= 0; i--) {
        char c = p[i];
        int s = C(c);
        start = s + (start > 0 ? Occ(start - 1, c) : 0);
        end = s + Occ(end, c) - 1;
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
    if (VISITED.count(i)) {
        return;
    }
    while (true) {
        if (start <= i && i <= end) {
            VISITED.insert(i);
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
        int firstId = currId - RECORD_NUM + 1;
        string firstIdentifier = "[" + to_string(firstId) + "]";
        //find the first record
        range = backwardSearch(firstIdentifier);
    }
    i = range.first;
    while (true) {
        if (start <= i && i <= end) {
            VISITED.insert(i);
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

char bwt0(int &checkpoint) {
    return rlbPosition[checkpoint].c;
}

string readGap(int &i) {
    int checkpoint = i / GAP;
    int start = checkpoint * GAP;
    int total = 0, rlbi = 0, rlbj = 0;
    int pos = rlbPosition[checkpoint].pos;
    rlbi = rlbPosition[checkpoint].idx;

    if (checkpoint + 1 < rlbPosition.size()) {
        total = GAP + 1;
        rlbj = rlbPosition[checkpoint + 1].idx;
    } else {
        total = i - start + 1;
        rlbj = RLB_SIZE;
    }
    string str = "";
    if (rlbi == rlbj) {
        return string(GAP + 1, bwt0(checkpoint));
    }

    fseek(FD, rlbi, SEEK_SET);
    int bufferSize = GAP + 10;
    vector<unsigned char> buffer(bufferSize);
    size_t bytesRead = fread(buffer.data(), 1, bufferSize, FD);

    vector<unsigned char> chars;
    vector<RLE> rleArr = handleRLE(buffer, bytesRead, chars);
    rleArr.push_back(clearChars(chars));
    for (auto rle: rleArr) {
        int len = str.empty() ? rle.length - pos : rle.length;
        if (str.size() + len >= total) {
            return str + string(total - str.size(), rle.c);
        }
        str += string(len, rle.c);
    }
    return str;
}

RLE clearChars(vector<unsigned char> &chars) {
    pair<unsigned char, int> p = charAndLength(chars);
    RLE rle{p.first, p.second, (int) chars.size()};
    chars.clear();
    return rle;
}

char bwt(int &i) {
    int checkpoint = i / GAP;
    if (i % GAP == 0) {
        return bwt0(checkpoint);
    } else {
        string bwt = readGap(i);
        int start = checkpoint * GAP;
        return bwt[i - start];
    }
}

int countOccurrences(const string &str, char &c) {
    int count = 0;
    for (int i = 1; i < str.size(); ++i) {
        if (str[i] == c) {
            count++;
        }
    }
    return count;
}

int Occ(int i, char c) {
    int checkpoint = i / GAP;
    if (i % GAP == 0) {
        return occTable[checkpoint][c];
    } else {
        int start = checkpoint * GAP;
        string bwt = readGap(i).substr(0, i - start + 1);
        int numC = countOccurrences(bwt, c);
        return occTable[checkpoint][c] + numC;
    }
}

pair<unsigned char, int> charAndLength(vector<unsigned char> &chars) {
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

void doBuildOccTable(RLE &rle) {
    if (rle.c == '[') {
        RECORD_NUM += rle.length;
    }
    for (int i = 0; i < rle.length; i++) {
        BWT_NUM++;
        COUNTER[rle.c]++;
        if ((BWT_NUM - 1) % GAP == 0) {
            rlbPosition.push_back({RLB_INDEX, i, rle.c});
            occTable.push_back(COUNTER);
        }
    }
//    int curr = BWT_NUM / GAP;
//    int maxm = (BWT_NUM + rle.length - 1) / GAP;
//    for (; curr < maxm; ++curr) {
//        int pos = (curr + 1) * GAP + 1 - BWT_NUM;
//        COUNTER[rle.c] += pos;
//        occTable.push_back(COUNTER);
//        rlbPosition.push_back({RLB_INDEX, pos, rle.c});
//    }

    RLB_INDEX += rle.byte;
}


vector<RLE> handleRLE(vector<unsigned char> &buffer, size_t &bytesRead, vector<unsigned char> &chars) {
    unsigned char curr;
    vector<RLE> res;

    for (int i = 0; i < bytesRead; i++) {
        curr = buffer[i];
        if (isChar(curr)) {

            if (chars.empty()) {
                chars.push_back(curr);
            } else {
                res.push_back(clearChars(chars));
                chars.push_back(curr);
            }

        } else {
            chars.push_back(curr);
        }
    }
    return res;
}

void buildCTable() {
    int total = 0;
    for (auto it: COUNTER) {
        //value of current key = total sum of values up to previous key
        char c = it.first;
        cTable[c] = total;
        total += it.second;
    }
}

void buildOccTable() {
    const size_t bufferSize = 2048;
    vector<unsigned char> buffer(bufferSize);
    fseek(FD, 0, SEEK_SET);
    vector<unsigned char> chars;
    while (size_t bytesRead = fread(buffer.data(), 1, bufferSize, FD)) {
        vector<RLE> rleArr = handleRLE(buffer, bytesRead, chars);
        for (auto rle: rleArr) {
            doBuildOccTable(rle);
        }
    }
    if (!chars.empty()) {
        RLE rle = clearChars(chars);
        doBuildOccTable(rle);
    }
}

void output() {
//    outputOccTable(occTable, FILENAME);
//    outputCounter(COUNTER, FILENAME);
//    outputCTable(cTable, FILENAME);
}

void getFileSize() {
    fseek(FD, 0, SEEK_END);
    RLB_SIZE = ftell(FD);
}

int main(int argc, char* argv[]) {
    string p = argv[3];
    string idxFile = argv[2];
    FILENAME = argv[1];
    FD = fopen(FILENAME, "r");
    getFileSize();
    if (RLB_SIZE > 4 * 1024 * 1024) {
        GAP = 9000;
    } else {
        GAP = 5000;
    }
//
    buildOccTable();

    set<int> ids;
    buildCTable();

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
            printf("%s\n", it.second.c_str());
        }
    } else {
//        cout << "hit: 0\n";
    }
    fclose(FD);
    return 0;
}