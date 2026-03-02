//2021111935 김시환
#include<iostream>
#include<fstream>
#include<vector>
#include<random>
#include<chrono>
using namespace std;

class StringMatching {
protected:
    vector<char>textString; //전체 스트링
    vector<char>patternString; //찾고자 하는 스트링

public:

    StringMatching(const string& text, const string& pattern) {
        textString.assign(text.begin(), text.end());
        patternString.assign(pattern.begin(), pattern.end());
    }
    virtual void searchPatternString(ofstream& fout) = 0; // 함수 오버라이딩을 위한 순수 가상 함수
    virtual void measureExecutionTime(ofstream& fout) = 0; // 함수 오버라이딩을 위한 순수 가상 함수
};

class Brute_Force : public StringMatching { //직선형 알고리즘
public:
    Brute_Force(const string& text, const string& pattern) : StringMatching(text, pattern) {}

    void searchPatternString(ofstream& fout) override {
        int textLen = textString.size();
        int patternLen = patternString.size();

        if (patternLen > textLen) {
            fout << "패턴이 너무 깁니다.\n";
            return;
        }

        bool found = false; //발견 여부를 판단
        for (int i = 0; i <= textLen - patternLen; i++) { //textString의 길이만큼 반복
            int j;
            for (j = 0; j < patternLen; j++) {
                if (textString[i + j] != patternString[j]) //text와 pattern이 중간에 한번이라도 다르면 끊김
                    break;
            }
            if (j == patternLen) { //patternString의 끝까지 검사하는데 성공했다는 뜻.
                cout << "Pattern found at index " << i << " using Brute Force.\n";
                fout << "직선형 알고리즘을 사용해 Pattern String을 발견한 인덱스: " << i << "\n";
                found = true;
            }
        }

        if (!found) {
            fout << "패턴을 발견하지 못했습니다.\n"; // 패턴이 발견되지 않은 경우
        }
    }

    void measureExecutionTime(ofstream& fout) override {
        auto start = chrono::high_resolution_clock::now(); //실행 전 시간 확인
        searchPatternString(fout); //탐색 시작
        auto end = chrono::high_resolution_clock::now(); //종료 후 시간 확인
        chrono::duration<double> duration = end - start; //실행 기간은 end - start
        cout << "Brute Force Execution Time: " << duration.count() << " seconds\n";
    }
};

class KMP : public StringMatching { //KMP
public:
    KMP(const string& text, const string& pattern) : StringMatching(text, pattern) {}

    void computeLPSArray(vector<int>& lps) { //접두사와 접미사가 일치하는 길이 저장
        int len = 0; //PPT j
        int i = 1; //PPT i
        lps[0] = 0; //PPT SP

        while (i < patternString.size()) {
            if (patternString[i] == patternString[len]) { //겹치면 ++
                len++;
                lps[i] = len;
                i++;
            }
            else { //다르면 SP만큼 뒤로
                if (len != 0) {
                    len = lps[len - 1]; //SP가 0이 아니었다면 SP만큼 뒤로 이동
                }
                else {
                    lps[i] = 0;//SP가 0이였다면 초기화
                    i++;
                }
            }
        }
    }

    void searchPatternString(ofstream& fout) override {
        int textLen = textString.size();
        int patternLen = patternString.size();
        if (patternLen > textLen) {
            fout << "패턴이 너무 깁니다..\n";
            return;
        }

        vector<int> lps(patternLen);

        computeLPSArray(lps);

        int i = 0, j = 0;
        bool found = false;
        while (i < textLen) { //원소끼리 비교
            if (patternString[j] == textString[i]) {
                i++;
                j++;
            }
            if (j == patternLen) { // 끝까지 도착했으면 탐색 성공
                fout << "KMP를 사용해 Pattern String을 발견한 인덱스 " << i - j << ".\n";
                cout << "Pattern found at index " << i - j << " using KMP.\n";
                found = true;
                j = lps[j - 1];
            }
            else if (i < textLen && patternString[j] != textString[i]) {
                if (j != 0) //중간에 다르면 다시 뒤로가서 탐색
                    j = lps[j - 1];
                else
                    i++;
            }
        }

        if (!found) {
            fout << "패턴을 발견하지 못했습니다.\n"; // 패턴이 발견되지 않은 경우
        }
    }

    void measureExecutionTime(ofstream& fout) override { //시간 측정 함수.
        auto start = chrono::high_resolution_clock::now();
        searchPatternString(fout);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "KMP Execution Time: " << duration.count() << " seconds\n";
    }
};

class DNAProcessor {
public:
    static string generateRandomDNASequence(int length) { 
        //DNA 문자열을 length 길이만큼 생성하는 프로그램.
        string dna;
        char dnaBases[] = { 'A', 'T', 'G', 'C' }; // A,T,G,C로 이루어진 문자 배열
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 3); // 0~3 중 하나를 랜덤하게 생성하여 A,T,G,C 중 하나 선택

        for (int i = 0; i < length; ++i) {
            dna += dnaBases[dis(gen)];
        }
        return dna;
    }

    static void saveDNASequenceToFile(const string& dnaSequence, const string& filename = "input.txt") {
        //input.txt 파일에 생성된 문자열을 저장하는 함수
        ofstream fout(filename);
        if (fout.is_open()) {
            fout << dnaSequence;
            fout.close();
            cout << "DNA String이 " << filename <<"에 저장됐습니다." << endl << endl;
        }
        else {
            cerr << "파일을 열지 못했습니다." << endl;
        }
    }

    static void savePatternIndexesToFile(StringMatching& searchAlgorithm, const string& outputFilename) {
        //찾은 패턴스트링의 인덱스를 output.txt에 저장하고 실행시간을 출력하는 함수
        ofstream fout(outputFilename);
        if (fout.is_open()) {
            searchAlgorithm.measureExecutionTime(fout);
            fout.close();
            cout << "Pattern indexes and execution time saved to " << outputFilename << endl;
        }
        else {
            cerr << "Error opening file!" << endl;
        }
    }
};


int main() {
	cout << "2021111935 김시환 DNA String Matching 프로그램" << endl << endl;

    int length1 = 10000000; //원하면 길이 변경 가능
    string dnaSequence = DNAProcessor::generateRandomDNASequence(length1);
    //DNA 문자열 생성

    // DNA 문자열을 input.txt에 저장
    DNAProcessor::saveDNASequenceToFile(dnaSequence, "input.txt");

    int length2 = 10;
    string pattern = DNAProcessor::generateRandomDNASequence(length2); //찾고자 하는 패턴 스트링

    // Brute Force를 이용한 패턴 검색
    Brute_Force bruteForce(dnaSequence, pattern);
    DNAProcessor::savePatternIndexesToFile(bruteForce, "output_brute_force.txt");
    cout << endl;

    // KMP를 이용한 패턴 검색
    KMP kmp(dnaSequence, pattern);
    DNAProcessor::savePatternIndexesToFile(kmp, "output_kmp.txt");

	return 0;
}