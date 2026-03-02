//2021111935 김시환
#include<iostream>
#include<string>
#include<vector>
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
	virtual void searchPatternString() = 0; // 함수 오버라이딩을 위한 순수 가상 함수
	virtual void measureExecutionTime() = 0; // 함수 오버라이딩을 위한 순수 가상 함수
};

class Brute_Force : public StringMatching { //직선형 알고리즘
public:
    Brute_Force(const string& text, const string& pattern) : StringMatching(text, pattern) {}

    void searchPatternString() override {
        int textLen = textString.size();
        int patternLen = patternString.size();

        for (int i = 0; i <= textLen - patternLen; i++) { //textString의 길이만큼 반복
            int j;
            for (j = 0; j < patternLen; j++) {
                if (textString[i + j] != patternString[j]) //text와 pattern이 중간에 한번이라도 다르면 끊김
                    break;
            }
            if (j == patternLen) { //patternString의 끝까지 검사하는데 성공했다는 뜻.
                cout << "Pattern found at index " << i << " using Brute Force.\n";
            }
        }
    }

    void measureExecutionTime() override {
        auto start = chrono::high_resolution_clock::now(); //실행 전 시간 확인
        searchPatternString(); //탐색 시작
        auto end = chrono::high_resolution_clock::now(); //종료 후 시간 확인
        chrono::duration<double> duration = end - start; //실행 기간은 end - start
        cout << "Brute Force Execution Time: " << duration.count() << " seconds\n";
    }
};

class Rabin_Karp : public StringMatching { //라빈카프
private:
    int prime = 101;
public:
    Rabin_Karp(const string& text, const string& pattern) : StringMatching(text, pattern) {}

    void searchPatternString() override {
        int textLen = textString.size();
        int patternLen = patternString.size();
        int patternHash = 0, textHash = 0, h = 1;

        for (int i = 0; i < patternLen - 1; i++) //해싱을 위한 기본 세팅
            h = (h * 256) % prime; //256을 곱하는 이유는 아스키코드값과 맞춰주려고

        for (int i = 0; i < patternLen; i++) { //패턴과 텍스트 해싱
            patternHash = (256 * patternHash + patternString[i]) % prime;
            textHash = (256 * textHash + textString[i]) % prime;
        }

        for (int i = 0; i <= textLen - patternLen; i++) {
            if (patternHash == textHash) {
                bool match = true; //해시가 같으면
                for (int j = 0; j < patternLen; j++) { //가짜 적중 여부를 판단
                    if (textString[i + j] != patternString[j]) {
                        match = false; //가짜 적중이면 종료
                        break;
                    }
                }
                if (match) { //진짜면 출력
                    cout << "Pattern found at index " << i << " using Rabin-Karp.\n";
                }
            }
            if (i < textLen - patternLen) { //라빈 카프 공식
                textHash = (256 * (textHash - textString[i] * h) + textString[i + patternLen]) % prime;
                if (textHash < 0)
                    textHash = (textHash + prime);
            }
        }
    }

    void measureExecutionTime() override { //Brute-Force의 시간 측정 함수와 같음.
        auto start = chrono::high_resolution_clock::now();
        searchPatternString();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "Rabin-Karp Execution Time: " << duration.count() << " seconds\n";
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

    void searchPatternString() override {
        int textLen = textString.size();
        int patternLen = patternString.size();
        vector<int> lps(patternLen);

        computeLPSArray(lps);

        int i = 0, j = 0;
        while (i < textLen) { //원소끼리 비교
            if (patternString[j] == textString[i]) {
                i++;
                j++;
            }
            if (j == patternLen) { // 끝까지 도착했으면 탐색 성공
                cout << "Pattern found at index " << i - j << " using KMP.\n";
                j = lps[j - 1];
            }
            else if (i < textLen && patternString[j] != textString[i]) {
                if (j != 0) //중간에 다르면 다시 뒤로가서 탐색
                    j = lps[j - 1];
                else
                    i++;
            }
        }
    }

    void measureExecutionTime() override { //시간 측정 함수, 위와 동일함.
        auto start = chrono::high_resolution_clock::now();
        searchPatternString();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "KMP Execution Time: " << duration.count() << " seconds\n";
    }
};

int main() {
	string textString = "A STRING SEARCHING EXAMPLE CONSISTING OF A GIVEN PATTERN STRING";
	string patternString = "STRING";

    StringMatching * stm[3];
    stm[0] = new Brute_Force(textString, patternString);
    stm[1] = new Rabin_Karp(textString, patternString);
    stm[2] = new KMP(textString, patternString);
    
    cout << "2021111935 김시환 스트링 매치 알고리즘" << endl << endl;
    for (int i = 0; i < 3; i++) {
        stm[i]->measureExecutionTime();
        cout << endl;
    }

	return 0;
}