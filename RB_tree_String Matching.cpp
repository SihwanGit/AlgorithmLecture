// main.cpp
#include "RBTree.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
using namespace std;
using namespace chrono;

string variate(const string& origin) {
    string read = origin;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, origin.length() - 11); // index + 10을 고려한 범위 제한
    uniform_int_distribution<> char_dist(0, 3);
    char bases[] = { 'A', 'C', 'G', 'T' };

    vector<bool> isSelected(origin.length(), false); // 선택 여부를 추적하는 배열

    for (int i = 0; i < 10;) {
        int pos = dist(gen); // 랜덤 인덱스 선택

        // index부터 index + 10 범위에서 isSelected가 true인지 검사
        bool conflict = false;
        for (int j = pos; j < pos + 11; j++) {
            if (isSelected[j]) {
                conflict = true;
                break;
            }
        }

        if (conflict) {
            continue; // 충돌이 있으면 다른 인덱스를 선택
        }

        // 선택된 범위를 isSelected로 표시
        for (int j = pos; j < pos + 11; j++) {
            isSelected[j] = true;
        }

        // 변이를 적용
        char newChar = bases[char_dist(gen)];
        read[pos] = newChar;

        i++; // 유효한 변이가 발생한 경우에만 증가
    }
    return read;
}

string restoreString(const string& read, RBNode* root) {
    string restore(read.length(), ' ');
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> index_dist(0, read.length() - 250);

    for (int i = 0; i < 10000000; i += 33) {
        int start = i;
        string prefix = read.substr(start, 75);
        string suffix = read.substr(start + 175, 75);

        int startIndex = root->findPrefix(prefix, suffix);

        if (startIndex >= 0) {
            for (int j = 0; j < 75; j++) {
                restore[startIndex + j] = prefix[j];
                restore[startIndex + j + 175] = suffix[j];
            }
        }
    }

    return restore;
}

int main() {
    string origin;
    ifstream file("input.txt");
    getline(file, origin);

    string var = variate(origin);
    
    cout << "Start DNA String Matching..." << endl << endl;;
    
    cout << "트리 생성 시작..." << endl;
    system_clock::time_point start = system_clock::now();
    RBNode* root = nullptr; // root 초기화
    root = root->buildTree(origin);
    system_clock::time_point end = system_clock::now();
    seconds sec = duration_cast<seconds>(end - start);
    cout << "트리 생성 sec : " << sec.count() << endl << endl;

    cout << "DNA 재생성 시작..." << endl;
    start = system_clock::now();
    string restore = restoreString(var, root);
    end = system_clock::now();
    sec = duration_cast<seconds>(end - start);
    cout << "재생성 sec : " << sec.count() << endl << endl;

    int cnt = 0;
    for (int i = 0; i < origin.length(); i++) {
        if (var[i] != restore[i]) {
            cnt++;
        }
    }

    cout << "Original: " << origin.substr(0, 100) << "..." << endl;
    cout << "Variated: " << var.substr(0, 100) << "..." << endl;
    cout << "Restored: " << restore.substr(0, 100) << "..." << endl;
    cout << "Error rate: " << (cnt * 100.0 / origin.length()) << "%" << endl;

    return 0;
}


