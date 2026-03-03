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

const int REFERENCE_LENGTH = 1000000;  // N
const int NUM_SUBSTRINGS = 30000;      // M
const int READ_LENGTH = 75;             // L
const int GAP_DISTANCE = 100;           // e
const int ALLOWED_ERROR = 10;           // D -> 20 변경 시 오차율 약 16%
const int MAXIMUM_EMPTY_STRING = 50000; // C
const double MUTATION_RATE = 0.01;      // ML : 1%
const int RANDOM_INDEX_RANGE = 10;      // index 중첩 검사
/*
* 이론적으로 RIN을 늘리는 데도 한계가 있다.
* 중복된 범위의 값이 나오면 아예 안뽑고 다시 뽑는다는 메커니즘의 특성 상, 인덱스를 많이 뽑을수록 중복 확률도 올라가면서
* 다시 뽑는 횟수도 상승, 페어 제작시간도 상승한다.
* 내가 페어를 n개 만들때 RIN 의 이론상 최대값 m은 m = REFERENCE_LENGTH / NUM_SUBSTRINGS가 될 것이다.
* 기본 세팅값에 따르면 대충 33.3까지가 최대치고, 내가 실행해본 결과 30만 되도 거의 멈춘다.
* 
* 이부분을 개선하거나 에러처리하는 코드도 작성해야할 듯.
*/

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


class TreeNode {
private:
    bool isRed;
    string prefix;
    unique_ptr<vector<pair<string, int>>> suffixes; // suffix and its position
    unique_ptr<TreeNode> left;
    unique_ptr<TreeNode> right;

public:
    TreeNode(const string& pref) :
        isRed(true), prefix(pref),
        suffixes(make_unique<vector<pair<string, int>>>()),
        left(nullptr), right(nullptr) {}

    void addSuffix(const string& suf, int pos) {
        suffixes->push_back({ suf, pos });
    }

    friend class RBTree;
};

class RBTree {
private:
    unique_ptr<TreeNode> root;

    void rotateLeft(unique_ptr<TreeNode>& node) {
        auto temp = move(node->right);
        node->right = move(temp->left);
        temp->left = move(node);
        temp->isRed = temp->left->isRed;
        temp->left->isRed = true;
        node = move(temp);
    }

    void rotateRight(unique_ptr<TreeNode>& node) {
        auto temp = move(node->left);
        node->left = move(temp->right);
        temp->right = move(node);
        temp->isRed = temp->right->isRed;
        temp->right->isRed = true;
        node = move(temp);
    }

    void flipColors(TreeNode* node) {
        node->isRed = true;
        if (node->left) node->left->isRed = false;
        if (node->right) node->right->isRed = false;
    }

public:
    RBTree() : root(nullptr) {}

    void insert(const string& prefix, const string& suffix, int position) {
        root = insertRec(move(root), prefix, suffix, position);
        root->isRed = false;
    }

    unique_ptr<TreeNode> insertRec(unique_ptr<TreeNode> node, const string& prefix,
        const string& suffix, int position) {
        if (!node) {
            auto newNode = make_unique<TreeNode>(prefix);
            newNode->addSuffix(suffix, position);
            return newNode;
        }

        if (prefix < node->prefix)
            node->left = insertRec(move(node->left), prefix, suffix, position);
        else if (prefix > node->prefix)
            node->right = insertRec(move(node->right), prefix, suffix, position);
        else
            node->addSuffix(suffix, position);

        // 밸런싱
        if (node->right && node->right->isRed && (!node->left || !node->left->isRed))
            rotateLeft(node);
        if (node->left && node->left->isRed && node->left->left && node->left->left->isRed)
            rotateRight(node);
        if (node->left && node->left->isRed && node->right && node->right->isRed)
            flipColors(node.get());

        return move(node);
    }

    vector<pair<string, int>> findMatches(const string& prefix) {
        TreeNode* node = root.get();
        while (node) {
            if (prefix == node->prefix)
                return *node->suffixes;
            else if (prefix < node->prefix)
                node = node->left.get();
            else
                node = node->right.get();
        }
        return vector<pair<string, int>>();
    }
};

//최초의 문자열 생성
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

//변이 적용
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

//리드 생성
vector<pair<string, string>> generateReads(const string& sequence) {
    cout << "Generating paired-end reads..." << endl;
    vector<pair<string, string>> reads;
    vector<bool> isSelected(sequence.length(), false); // 각 인덱스의 선택 여부를 추적
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, sequence.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH);

    // Open file for writing reads
    ofstream readsFile("reads.txt");
    if (!readsFile.is_open()) {
        cerr << "Error: Unable to open reads.txt" << endl;
        return reads;
    }

    while (reads.size() < NUM_SUBSTRINGS) {
        int start = dis(gen); // 랜덤 인덱스 추출

        // 인덱스가 선택된 범위에 속하는지 확인
        bool valid = true;
        for (int i = start; i < start + RANDOM_INDEX_RANGE && i < sequence.length(); ++i) { // 인덱스가 이미 선택되었는지 확인
            if (isSelected[i]) {
                valid = false;
                break;
            }
        }

        // 유효한 인덱스가 나오면 처리, 아니면 다시 랜덤 인덱스 추출
        if (valid) {
            // 유효한 인덱스라면 isSelected 배열에 표시
            for (int i = start; i < start + RANDOM_INDEX_RANGE && i < sequence.length(); ++i) {
                isSelected[i] = true; // 인덱스 표시
            }

            // Read 생성
            string prefix = sequence.substr(start, READ_LENGTH);
            string suffix = sequence.substr(start + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
            reads.push_back({ prefix, suffix });

            // Write to file
            readsFile << "Read " << reads.size() << " Position " << start << ": "
                << prefix << " ... " << suffix << endl;
        }

        // 유효한 인덱스를 찾을 때까지 반복
    }

    showProgress(1.0);
    cout << endl;
    readsFile.close();
    return reads;
}


//빈 문자열 채우기.
//ALLOWED_ERROR 에러 이하면 match가 발생한 것이므로 문자열 채우고 이어서 탐색
string reconstructSequence(const string& reference, const vector<pair<string, string>>& reads) {
    cout << "Building RB Tree..." << endl;
    // Build RB Tree from reference sequence
    RBTree tree;
    int progressCounter = 0;
    int maxProgress = reference.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH;

    for (int i = 0; i < maxProgress; ++i) {
        string prefix = reference.substr(i, READ_LENGTH);
        string suffix = reference.substr(i + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
        tree.insert(prefix, suffix, i);

        if (i % (maxProgress / 100) == 0) {
            showProgress(static_cast<float>(i) / maxProgress);
        }
    }
    showProgress(1.0);
    cout << endl;

    cout << "Reconstructing sequence..." << endl;
    string reconstruction(reference.length(), '_');

    // Try to match reads and fill in the reconstruction
    for (size_t i = 0; i < reads.size(); ++i) {
        auto matches = tree.findMatches(reads[i].first);
        for (const auto& match : matches) {
            // Check if suffix matches within error threshold
            int errors = 0;
            for (int j = 0; j < READ_LENGTH; ++j) {
                if (reads[i].second[j] != match.first[j]) { //다른 부분을 발견할 때마다 error+1
                    errors++;
                    if (errors > ALLOWED_ERROR) break;
                }
            }

            if (errors <= ALLOWED_ERROR) {
                // Fill in the reconstruction at the matched position
                for (int j = 0; j < READ_LENGTH; ++j) {
                    reconstruction[match.second + j] = reads[i].first[j];
                    reconstruction[match.second + READ_LENGTH + GAP_DISTANCE + j] = reads[i].second[j];
                }
            }
        }

        if (i % (reads.size() / 100) == 0) {
            showProgress(static_cast<float>(i) / reads.size());
        }
    }
    showProgress(1.0);
    cout << endl;

    // Fill remaining gaps if below threshold
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

    // 레퍼런스 문자열 생성
    string reference = generateRandomDNA(REFERENCE_LENGTH);
    writeSequenceToFile("reference.txt", reference);
    cout << "Reference genome saved to reference.txt" << endl;

    // 내 DNA(정답) 생성
    string mutated = applyMutations(reference); // 변이 적용
    writeSequenceToFile("mutated.txt", mutated);
    cout << "Mutated sequence saved to mutated.txt" << endl;

    // paired-end read 생성
    auto reads = generateReads(mutated); //리드 생성
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