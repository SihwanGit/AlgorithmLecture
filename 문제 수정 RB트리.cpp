#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

using namespace std;

// Constants
const int REFERENCE_LENGTH = 1000000; // N
const int NUM_SUBSTRINGS = 30000;     // M
const int READ_LENGTH = 75;            // L
const int GAP_DISTANCE = 50;           // e
const int ALLOWED_ERROR = 20;          // D
const double MUTATION_RATE = 0.05;     // 1% <- 이게 왜 1%?
//이거 실행이 안되서 N,M의 크기를 1/10으로 줄임.

// Type Definitions
using SuffixList = vector<pair<string, int> >;

// Utility Functions
void showProgress(float progress) {
    static int prevProgress = -1;
    int barWidth = 70;
    int currProgress = int(progress * 100);
    if (currProgress == prevProgress)
        return;
    prevProgress = currProgress;
    cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            cout << "=";
        else if (i == pos)
            cout << ">";
        else
            cout << " ";
    }
    cout << "] " << currProgress << "%\r";
    cout.flush();
}


void writeSequenceToFile(const string& filename, const string& sequence, int lineLength = 100) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }
    for (size_t i = 0; i < sequence.size(); i += lineLength) {
        file << sequence.substr(i, lineLength) << "\n";
    }
}


// TreeNode Class
class TreeNode {
public:
    string prefix;
    SuffixList suffixes;
    shared_ptr<TreeNode> left, right;
    bool isRed;

    TreeNode(const string& pref)
        : prefix(pref), isRed(true), left(nullptr), right(nullptr) {}

    void addSuffix(const string& suffix, int pos) {
        suffixes.emplace_back(suffix, pos);
    }
};
//기본적인 트리노드


// RBTree Class
class RBTree {
private:
    shared_ptr<TreeNode> root;

    void rotateLeft(shared_ptr<TreeNode>& node) { //회전 파트가 우리가 배운 것과는 약간 다르다고 함.
        auto temp = node->right;
        node->right = temp->left;
        temp->left = node;
        temp->isRed = node->isRed;
        node->isRed = true;
        node = temp;
    }

    void rotateRight(shared_ptr<TreeNode>& node) { //회전을 자식 기준이 아니라 부모 기준에서 적용하는 것
        auto temp = node->left;
        node->left = temp->right;
        temp->right = node;
        temp->isRed = node->isRed;
        node->isRed = true;
        node = temp;
    }

    void flipColors(shared_ptr<TreeNode>& node) { //색변환
        node->isRed = !node->isRed;
        if (node->left)
            node->left->isRed = !node->left->isRed;
        if (node->right)
            node->right->isRed = !node->right->isRed;
    }

    shared_ptr<TreeNode> insert(shared_ptr<TreeNode> node, const string& prefix, const string& suffix, int position) {
        if (!node) {
            auto newNode = make_shared<TreeNode>(prefix);
            newNode->addSuffix(suffix, position);
            return newNode;
        }
        if (prefix < node->prefix)
            node->left = insert(node->left, prefix, suffix, position);
        else if (prefix > node->prefix)
            node->right = insert(node->right, prefix, suffix, position);
        else
            node->addSuffix(suffix, position);

        if (node->right && node->right->isRed && (!node->left || !node->left->isRed))
            rotateLeft(node);
        if (node->left && node->left->isRed && node->left->left && node->left->left->isRed)
            rotateRight(node);
        if (node->left && node->left->isRed && node->right && node->right->isRed)
            flipColors(node);

        return node;
    }
    //노드 삽입


    void findMatchesRecursive(shared_ptr<TreeNode> node, const string& prefix, const string& suffix, 
        int error_rate, SuffixList& bestMatches) const {

        if (!node)
            return;

        int prefixErrors = 0;
        for (size_t i = 0; i < prefix.length(); ++i) {
            if (node->prefix[i] != prefix[i]) {
                prefixErrors++;
                if (prefixErrors > error_rate)
                    break;
            }
        }

        if (prefixErrors <= error_rate) {   //prefix가 에러레이트보다 적다면 삽입
            for (const auto& entry : node->suffixes) {
                int suffixErrors = 0;       //이러면 prefix따로 suffix따로 검사하게 되는 거 아님???
                for (size_t i = 0; i < suffix.length(); ++i) {
                    if (entry.first[i] != suffix[i]) {
                        suffixErrors++;
                        if (suffixErrors > error_rate)
                            break;
                    }
                }
                if (suffixErrors <= error_rate) {
                    bestMatches.push_back(entry);
                    if (prefixErrors == 0 && suffixErrors == 0)
                        return;
                }
            }
        }

        if (prefix < node->prefix)
            findMatchesRecursive(node->left, prefix, suffix, error_rate, bestMatches);
        else
            findMatchesRecursive(node->right, prefix, suffix, error_rate, bestMatches);
    }

public:
    RBTree() : root(nullptr) {}

    void insert(const string& prefix, const string& suffix, int position) {
        root = insert(root, prefix, suffix, position);
        if (root)
            root->isRed = false;
    }

    SuffixList findMatches(const string& prefix, const string& suffix, int error_rate) const {
        SuffixList bestMatches;
        findMatchesRecursive(root, prefix, suffix, error_rate, bestMatches);
        return bestMatches;
    }
};


// DNA Sequence Functions
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
// 변이 적용

RBTree* buildTree(const string& reference) {
    cout << "Building RB Tree..." << endl;
    auto* tree = new RBTree();
    int maxProgress = reference.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH;
    for (int i = 0; i < maxProgress; ++i) {
        string prefix = reference.substr(i, READ_LENGTH);
        string suffix = reference.substr(i + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
        tree->insert(prefix, suffix, i);
        if (i % (maxProgress / 100) == 0) {
            showProgress(static_cast<float>(i) / maxProgress);
        }
    }
    showProgress(1.0);
    cout << endl;
    return tree;
}
//트리만들기

vector<pair<string, string> > generateReads(const string& sequence) {
    cout << "Generating paired-end reads..." << endl;
    vector<pair<string, string> > reads;
    mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<> dis(0, sequence.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH - 1);
    for (int i = 0; i < NUM_SUBSTRINGS; ++i) {
        int start = dis(gen);
        string prefix = sequence.substr(start, READ_LENGTH);
        string suffix = sequence.substr(start + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
        reads.push_back(make_pair(prefix, suffix));
        if (i % (NUM_SUBSTRINGS / 100) == 0) {
            showProgress(static_cast<float>(i) / NUM_SUBSTRINGS);
        }
    }
    showProgress(1.0);
    cout << endl;
    return reads;
}

string reconstructSequence(const string& reference, const vector<pair<string, string> >& reads, const RBTree* tree) {
    cout << "Reconstructing sequence..." << endl;
    string reconstruction(reference.length(), '_');
    for (size_t i = 0; i < reads.size(); ++i) {
        auto matches = tree->findMatches(reads[i].first, reads[i].second, ALLOWED_ERROR);
        for (const auto& match : matches) {
            for (int j = 0; j < READ_LENGTH; ++j) {
                reconstruction[match.second + j] = reads[i].first[j];
                reconstruction[match.second + READ_LENGTH + GAP_DISTANCE + j] = reads[i].second[j];
            }
        }
        if (i % (reads.size() / 100) == 0) {
            showProgress(static_cast<float>(i) / reads.size());
        }
    }
    showProgress(1.0);
    cout << endl;
    return reconstruction;
}

// Main Function
int main() {
    auto overallStart = chrono::high_resolution_clock::now(); //시간 재기
    string reference = generateRandomDNA(REFERENCE_LENGTH); //원본 생성
    writeSequenceToFile("reference.txt", reference); //파일 저장 ?
    //근데 input.txt에서 가져오기로 하지 않음?

    auto treeStart = chrono::high_resolution_clock::now();
    RBTree* tree = buildTree(reference); //트리 생성
    auto treeEnd = chrono::high_resolution_clock::now();
    auto treeDuration = chrono::duration_cast<chrono::seconds>(treeEnd - treeStart);
    cout << "Tree creation time: " << treeDuration.count() << " seconds" << endl; 
    //트리 생성 및 생성 시간 출력

    int totalRestorationTime = 0;
    for (int i = 0; i < 5; i++) {
        string mutated = applyMutations(reference); //변이 적용
        auto reads = generateReads(mutated); //리드 생성

        auto restoreStart = chrono::high_resolution_clock::now();
        string reconstructed = reconstructSequence(reference, reads, tree);
        auto restoreEnd = chrono::high_resolution_clock::now();
        auto restoreDuration = chrono::duration_cast<chrono::seconds>(restoreEnd - restoreStart);
        cout << "Restoration time for iteration " << i + 1 << ": " << restoreDuration.count() << " seconds" << endl;
        
        totalRestorationTime += restoreDuration.count();
        writeSequenceToFile("reconstructed.txt", reconstructed);

        int differences = 0, gaps = 0;
        for (size_t j = 0; j < mutated.length(); ++j) {
            if (mutated[j] != reconstructed[j])
                differences++;
            if (reconstructed[j] == '_')
                gaps++;
        }
        cout << "Iteration " << i + 1 << ":" << endl;
        cout << "Differences: " << differences << endl;
        cout << "Gaps: " << gaps << endl;
        cout << "Accuracy: " << fixed << setprecision(2)
            << (1.0 - static_cast<double>(differences) / REFERENCE_LENGTH) * 100 << "%" << endl;
    }

    auto overallEnd = chrono::high_resolution_clock::now();
    auto overallDuration = chrono::duration_cast<chrono::seconds>(overallEnd - overallStart);
    cout << "Total restoration time: " << totalRestorationTime << " seconds" << endl;
    cout << "Total execution time: " << overallDuration.count() << " seconds" << endl;

    delete tree;
    return 0;
}
