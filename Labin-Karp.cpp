#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <unordered_map>

using namespace std;

const int REFERENCE_LENGTH = 10000000;  // N
const int NUM_SUBSTRINGS = 300000;      // M
const int READ_LENGTH = 75;             // L
const int GAP_DISTANCE = 100;           // e
const int ALLOWED_ERROR = 20;           // D
const int MAXIMUM_EMPTY_STRING = 50000; // C
const double MUTATION_RATE = 0.01;      // 1% mutation rate
const int PRIME = 1000000007;           // Large prime for hashing
const int BASE = 4;                     // DNA has 4 possible bases

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

// 추가된 부분
// DNA 염기를 숫자로 변환
int charToInt(char c) {
    switch (c) {
    case 'A': return 0;
    case 'C': return 1;
    case 'G': return 2;
    case 'T': return 3;
    default: return 0;
    }
}

// Rabin-Karp 해시 계산
long long computeHash(const string& str) {
    long long hash = 0;
    for (char c : str) {
        hash = (hash * BASE + charToInt(c)) % PRIME;
    }
    return hash;
}

// Rolling hash 계산을 위한 클래스
class RollingHash {
private:
    long long hash;
    long long power;
    vector<int> values;
    int window;

public:
    RollingHash(int windowSize) : hash(0), power(1), window(windowSize) {
        // 미리 BASE^(READ_LENGTH-1) 계산
        for (int i = 0; i < windowSize - 1; i++) {
            power = (power * BASE) % PRIME;
        }
    }

    // 초기 해시값 계산
    long long initHash(const string& str) {
        hash = 0;
        values.clear();
        for (char c : str) {
            int val = charToInt(c);
            values.push_back(val);
            hash = (hash * BASE + val) % PRIME;
        }
        return hash;
    }

    // 다음 위치의 해시값 계산
    long long nextHash(char nextChar) {
        int outVal = values[0];
        values.erase(values.begin());
        int inVal = charToInt(nextChar);
        values.push_back(inVal);

        hash = ((hash - outVal * power) * BASE + inVal) % PRIME;
        if (hash < 0) hash += PRIME;
        return hash;
    }
};

string generateRandomDNA(int length) {
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

string applyMutations(const string& reference) {
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

vector<pair<string, string>> generateReads(const string& sequence) {
    // 이전과 동일
    cout << "Generating paired-end reads..." << endl;
    vector<pair<string, string>> reads;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, sequence.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH);

    ofstream readsFile("reads.txt");
    if (!readsFile.is_open()) {
        cerr << "Error: Unable to open reads.txt" << endl;
        return reads;
    }

    for (int i = 0; i < NUM_SUBSTRINGS; ++i) {
        int start = dis(gen);
        string prefix = sequence.substr(start, READ_LENGTH);
        string suffix = sequence.substr(start + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
        reads.push_back({ prefix, suffix });

        readsFile << "Read " << setw(6) << i + 1 << " Position " << setw(8) << start << ": "
            << prefix << " ... " << suffix << endl;

        if (i % (NUM_SUBSTRINGS / 100) == 0) {
            showProgress(static_cast<float>(i) / NUM_SUBSTRINGS);
        }
    }
    showProgress(1.0);
    cout << endl;
    readsFile.close();
    return reads;
}

string reconstructSequence(const string& reference, const vector<pair<string, string>>& reads) {
    cout << "Building hash table..." << endl;

    // 레퍼런스의 모든 위치에 대해 prefix와 suffix의 해시값을 저장
    unordered_map<long long, vector<pair<int, string>>> prefixHash; // hash -> (position, suffix)
    RollingHash hasher(READ_LENGTH);

    // 초기 해시값 계산
    hasher.initHash(reference.substr(0, READ_LENGTH));

    // Rolling hash를 사용하여 모든 위치의 해시값 계산
    for (int i = 0; i <= reference.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH; ++i) {
        string prefix = reference.substr(i, READ_LENGTH);
        string suffix = reference.substr(i + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
        long long hash = (i == 0) ? hasher.initHash(prefix) : hasher.nextHash(reference[i + READ_LENGTH - 1]);
        prefixHash[hash].push_back({ i, suffix });

        if (i % ((reference.length() - READ_LENGTH) / 100) == 0) {
            showProgress(static_cast<float>(i) / (reference.length() - READ_LENGTH));
        }
    }
    showProgress(1.0);
    cout << endl;

    cout << "Reconstructing sequence..." << endl;
    string reconstruction(reference.length(), '_');

    // 각 read에 대해 매칭 시도
    for (size_t i = 0; i < reads.size(); ++i) {
        long long readHash = computeHash(reads[i].first);

        // 해시값이 일치하는 모든 위치에 대해 검사
        auto it = prefixHash.find(readHash);
        if (it != prefixHash.end()) {
            for (const auto& match : it->second) {
                int pos = match.first;
                const string& refSuffix = match.second;

                // suffix 비교
                int errors = 0;
                for (int j = 0; j < READ_LENGTH; ++j) {
                    if (reads[i].second[j] != refSuffix[j]) {
                        errors++;
                        if (errors > ALLOWED_ERROR) break;
                    }
                }

                if (errors <= ALLOWED_ERROR) {
                    // 매칭된 위치에 시퀀스 복원
                    for (int j = 0; j < READ_LENGTH; ++j) {
                        reconstruction[pos + j] = reads[i].first[j];
                        reconstruction[pos + READ_LENGTH + GAP_DISTANCE + j] = reads[i].second[j];
                    }
                }
            }
        }

        if (i % (reads.size() / 100) == 0) {
            showProgress(static_cast<float>(i) / reads.size());
        }
    }
    showProgress(1.0);
    cout << endl;

    // 남은 gap 채우기
    int gaps = count(reconstruction.begin(), reconstruction.end(), '_');
    if (gaps <= MAXIMUM_EMPTY_STRING) {
        cout << "Filling " << gaps << " remaining gaps..." << endl;
        for (size_t i = 0; i < reconstruction.length(); ++i) {
            if (reconstruction[i] == '_') {
                reconstruction[i] = reference[i];
            }
            if (i % (reconstruction.length() / 100) == 0) {
                showProgress(static_cast<float>(i) / reconstruction.length());
            }
        }
        showProgress(1.0);
        cout << endl;
    }

    return reconstruction;
}

int main() {
    auto startTime = chrono::high_resolution_clock::now();

    // 레퍼런스 문자열 읽기
    string reference;
    ifstream file("reference.txt");

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            reference += line;
        }
        file.close();
    }
    else {
        cout << "reference.txt를 열 수 없습니다." << endl;
        return 1;
    }

    // 내 DNA(정답) 생성
    string mutated = applyMutations(reference); //변이 적용
    writeSequenceToFile("mutated.txt", mutated);
    cout << "Mutated sequence saved to mutated.txt" << endl;

    // paired-end read 생성
    auto reads = generateReads(mutated);
    cout << "Paired-end reads saved to reads.txt" << endl;

    // read 시퀀싱
    string reconstructed = reconstructSequence(reference, reads);
    writeSequenceToFile("reconstructed.txt", reconstructed);
    cout << "Reconstructed sequence saved to reconstructed.txt" << endl;

    // 결과 분석
    int differences = 0;
    int gaps = 0;
    for (size_t i = 0; i < mutated.length(); ++i) {
        if (mutated[i] != reconstructed[i]) differences++;
        if (reconstructed[i] == '_') gaps++;
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);

    cout << "\nFinal Statistics:" << endl;
    cout << "----------------" << endl;
    cout << "Total runtime: " << duration.count() << " seconds" << endl;
    cout << "Sequence length: " << REFERENCE_LENGTH << " bp" << endl;
    cout << "Number of reads: " << NUM_SUBSTRINGS << endl;
    cout << "Reconstruction accuracy: "
        << fixed << setprecision(2)
        << (1.0 - static_cast<double>(differences) / REFERENCE_LENGTH) * 100
        << "%" << endl;
    cout << "Remaining gaps: " << gaps << " ("
        << fixed << setprecision(2)
        << (static_cast<double>(gaps) / REFERENCE_LENGTH) * 100
        << "%)" << endl;

    return 0;
}