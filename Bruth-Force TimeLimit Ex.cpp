#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <fstream>
#include <iomanip>
#include <chrono>
using namespace std;

// 기본 상수
const int REFERENCE_LENGTH = 10000000;  // N
const int NUM_SUBSTRINGS = 300000;      // M
const int READ_LENGTH = 150;            // L, substring
const int ALLOWED_ERROR = 10;           // D
const int MAXIMUM_EMPTY_STRING = 50000; // C
const double MUTATION_RATE = 0.001;     // 0.1%
const int TIME_LIMIT = 300;             // 최대 실행 시간 (초)

// 진행 상태 표시(%)
void showProgress(float progress) {
    int barWidth = 70;
    cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(progress * 100.0) << "%\r";
    cout.flush();
}

// 시퀀스 파일 출력
void writeSequenceToFile(const string& filename, const string& sequence, int lineLength = 100) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    for (size_t i = 0; i < sequence.length(); i += lineLength) {
        file << sequence.substr(i, lineLength) << endl;
    }
    file.close();
}

string generateRandomDNA(int length) { // 랜덤 DNA 생성
    cout << "Generating reference genome..." << endl;
    static const char nucleotides[] = "ACGT";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 3);

    string sequence;
    sequence.reserve(length);
    for (int i = 0; i < length; ++i) {
        sequence += nucleotides[dis(gen)];
        if (i % (length / 100) == 0) {
            showProgress(static_cast<float>(i) / length);
        }
    }
    showProgress(1.0);
    cout << endl;
    return sequence;
}

string applyMutations(const string& reference) { // 변이 적용 알고리즘
    cout << "Applying mutations..." << endl;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    string mutated = reference;
    for (size_t i = 0; i < mutated.length(); ++i) {
        if (dis(gen) < MUTATION_RATE) {
            static const string nucleotides = "ACGT";
            mutated[i] = nucleotides[dis(gen) * 4];
        }
        if (i % (mutated.length() / 100) == 0) {
            showProgress(static_cast<float>(i) / mutated.length());
        }
    }
    showProgress(1.0);
    cout << endl;
    return mutated;
}

bool matchWithErrors(const string& a, const string& b, int allowedErrors) {
    int errors = 0;
    for (size_t i = 0; i < a.length(); ++i) {
        if (a[i] != b[i]) ++errors;
        if (errors > allowedErrors) return false;
    }
    return true;
}

int main() {
    auto startTime = chrono::high_resolution_clock::now();

    string reference = generateRandomDNA(REFERENCE_LENGTH);
    string mutated = applyMutations(reference);

    string reconstructString(mutated.size(), '-');
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, mutated.size() - READ_LENGTH - 1);

    cout << "Bruth-Force Matching..." << endl;
    for (int i = 0; i < NUM_SUBSTRINGS; ++i) {
        int randomIndex = dis(gen);
        string substring = mutated.substr(randomIndex, READ_LENGTH);

        for (size_t j = 0; j <= reference.size() - READ_LENGTH; ++j) {
            string refSegment = reference.substr(j, READ_LENGTH);
            if (matchWithErrors(refSegment, substring, ALLOWED_ERROR)) {
                reconstructString.replace(j, READ_LENGTH, substring);
                break;
            }
        }

        // 시간 제한 확인
        auto currentTime = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = currentTime - startTime;
        if (elapsed.count() > TIME_LIMIT) {
            cout << "\nExecution time exceeded the time limit of " << TIME_LIMIT << " seconds. Exiting program." << endl;
            return 1;
        }

        if (i % (NUM_SUBSTRINGS / 100) == 0) {
            showProgress(static_cast<float>(i) / NUM_SUBSTRINGS);
        }
    }
    showProgress(1.0);
    cout << endl;

    // Calculate difference
    int mismatches = 0;
    for (size_t i = 0; i < mutated.size(); ++i) {
        if (reconstructString[i] != mutated[i]) ++mismatches;
    }
    double differencePercentage = (static_cast<double>(mismatches) / mutated.size()) * 100.0;

    cout << "Difference between mutated and reconstructed strings: " << fixed << setprecision(2) << differencePercentage << "%" << endl;

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> totalElapsed = endTime - startTime;
    cout << "Execution time: " << totalElapsed.count() << " seconds" << endl;

    return 0;
}

//TODO
/* applyMutations을 통해 발생시킨 변이 문자열에서 랜덤한 인덱스 추출.
추출한 인덱스부터 READ_LENGTH 길이 만큼의 문자열을 applyMutation으로 생성한 문자열에서 추출
해당 방법으로 추출한 문자열과 generateRandomDNA() 함수로 생성한 문자열을 BruthForce로 매칭
만약 추출한 문자열과 generateRandomDNA() 함수로 생성한 문자열을 매칭할 때, 오류가 ALLOWED_ERROR 이하면
빈 문자열 reconstructString에 매칭된 인덱스부터 READ_LENGTH의 길이만큼의 문자열을 삽입

이 과정을 NUM_SUBSTRINGS 번 반복

최종적으로 만들어진 reconstructString 문자열과 applyMutations으로 생성한 문자열을 비교.
두 문자열의 차이를 %로 출력

이 프로그램이 실행된 시간을 출력

타임 리미트는 무한대로 실행되는 일을 막고, 일정 시간이 지나도 프로그램이 종료되지 않는다면 강제 종료.

*/


