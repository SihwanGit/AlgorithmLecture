#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
using namespace std;
using namespace chrono;

// 상수
const int REFERENCE_LENGTH = 10000000; // N
const int NUM_SUBSTRINGS = 300000;     // M
const int READ_LENGTH = 75;            // L
const int GAP_DISTANCE = 100;          // e
const int ALLOWED_ERROR = 10;          // D
const double MUTATION_RATE = 0.15;     // 15%
//N,M은 실행이슈로 1/10 후 진행

// 순서 쌍, 2가지 값을 하나로 묶기 위해서 사용, (prefix, suffix), (suffix, index)
template <typename T1, typename T2> class Pair {
public:
    T1 first;
    T2 second;

    Pair() : first(), second() {}
    Pair(const T1& f, const T2& s) : first(f), second(s) {}
};

// prefix에 대한 suffix, index 정보를 저장할 Container
// 같은 prefix에 여러 값 존재 가능 -> Container에 담아서 배열처럼 사용
template <typename T>
class Container { // Pair를 담아야 하기 때문에 generic 클래스로 지정
private:
    T* data;         // Container가 저장하는 값
    size_t capacity; // Container의 최대 수용량
    size_t size; // 현재 Container가 가지고 있는 data의 수 마지막 값 다음 index

    // Container의 사이즈를 조정하는 메소드
    void resize(size_t newCapacity) {
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    Container() : data(nullptr), capacity(0), size(0) {}

    ~Container() { delete[] data; }

    // Conatainer에 새로운 값을 추가, 현재 저장된 맨 마지막에 값 추가
    // 공간이 모자라다면, 2배씩 증가
    void push(const T& value) {
        if (size == capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = value; // 마지막에 값 추가
    }

    // index에 해당하는 값 반환
    const T& operator[](size_t index) const { return data[index]; }

    // 현재 Container에 저장된 값 반환
    size_t getSize() const { return size; }

    // Container에 저장된 첫번째, 마지막 주소반환
    // 향상된 for문 사용을 위한 메소드
    const T* begin() const { return data; }
    const T* end() const { return data + size; }
};

// TreeNode
class TreeNode {
public:
    string prefix; // 접두사
    Container<Pair<string, int> >
        suffixes; // 접두사와 짝을 이루는 접미사들, 그때의 접두사의 index
    TreeNode* left;
    TreeNode* right;
    bool isRed;

    TreeNode(const string& pref)
        : prefix(pref), left(nullptr), right(nullptr), isRed(true) {}

    void addSuffix(const string& suffix, int position) {
        suffixes.push(Pair<string, int>(suffix, position));
    }
};

// RBTree Class
class RBTree {
private:
    TreeNode* root;

    // 회전의 기준노드는 루트 노드
    void rotateLeft(TreeNode*& node) {
        TreeNode* temp = node->right;
        node->right = temp->left;
        temp->left = node;
        temp->isRed = node->isRed;
        node->isRed = true;
        node = temp;
    }

    void rotateRight(TreeNode*& node) {
        TreeNode* temp = node->left;
        node->left = temp->right;
        temp->right = node;
        temp->isRed = node->isRed;
        node->isRed = true;
        node = temp;
    }

    // 기준 노드, 자식 색 반전
    void flipColors(TreeNode* node) {
        node->isRed = !node->isRed;
        if (node->left)
            node->left->isRed = !node->left->isRed;
        if (node->right)
            node->right->isRed = !node->right->isRed;
    }

    TreeNode* insert(TreeNode* node, const string& prefix, const string& suffix,
        int position) {
        if (!node) {
            TreeNode* newNode = new TreeNode(prefix);
            newNode->addSuffix(suffix, position);
            return newNode;
        }

        if (prefix < node->prefix) {
            node->left = insert(node->left, prefix, suffix, position);
        }
        else if (prefix > node->prefix) {
            node->right = insert(node->right, prefix, suffix, position);
        }
        else {
            // 정확히 일치하는 prefix는 suffix를 suffixes에 추가
            node->addSuffix(suffix, position);
        }

        // 트리 밸런싱

        // case 1
        if (node->right && node->right->isRed &&
            (!node->left || !node->left->isRed)) {
            rotateLeft(node);
        }
        // case 2
        if (node->left && node->left->isRed && node->left->left &&
            node->left->left->isRed) {
            rotateRight(node);
        }
        // case 3
        if (node->left && node->left->isRed && node->right &&
            node->right->isRed) {
            flipColors(node);
        }
        return node;
    }

public:
    RBTree() : root(nullptr) {}

    void insert(const string& prefix, const string& suffix, int position) {
        root = insert(root, prefix, suffix, position);
        if (root)
            root->isRed = false;
    }

    // prefix와 매치되는 노드 탐색
    Container<Pair<string, int> > findMatches(const string& prefix,
        const string& suffix) const {

        // 매치되는 prefix를 찾은 뒤, suffix까지 매치 되었다면 matches에 추가
        Container<Pair<string, int> > matches;
        TreeNode* current = root;

        while (current != nullptr) { // 더 이상 이동할 노드가 없을 때 까지 반복
            int prefixErrors = 0;
            for (size_t i = 0; i < prefix.size(); ++i) {
                // 현재 노드와 탐색중인 prefix간 오차 계산
                if (i >= current->prefix.size() ||
                    current->prefix[i] != prefix[i]) {
                    prefixErrors++;
                    if (prefixErrors > ALLOWED_ERROR)
                        break;
                }
            }

            // 노드의 prefix와 허용 오차 이내로 일치시 매칭 성공
            if (prefixErrors <= ALLOWED_ERROR) {
                for (Pair<string, int> suffixPair : current->suffixes) {
                    // suffix들에 대해서 오차 계산
                    int suffixErrors = 0;
                    for (size_t i = 0; i < suffix.size(); ++i) {
                        if (i >= suffixPair.first.size() ||
                            suffixPair.first[i] != suffix[i]) {
                            suffixErrors++;
                            if (suffixErrors > ALLOWED_ERROR)
                                break;
                        }
                    }
                    // suffix도 오차 이내라면, matches 컨테이너에 추가
                    if (suffixErrors <= ALLOWED_ERROR) {
                        matches.push(suffixPair);
                    }
                }
            }

            // 다음 노드로 진행
            if (prefix < current->prefix)
                current = current->left;
            else
                current = current->right;
        }

        return matches;
    }
};

// 진행상황
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

// 파일에 결과 작성
void writeSequenceToFile(const string& filename, const string& sequence,
    int lineLength = 100) {
    ofstream file(filename);
    for (size_t i = 0; i < sequence.size(); i += lineLength) {
        file << sequence.substr(i, lineLength) << '\n';
    }
}

// 원본 문자열 (인간 DNA) 생성
string generateRandomDNA(int length) {
    static const char nucleotides[] = "ACGT";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 3);

    string sequence;
    sequence.reserve(length);
    for (int i = 0; i < length; ++i) {
        sequence += nucleotides[dis(gen)];
    }
    return sequence;
}

// 변이 문자열 생성 (내 DNA)
string applyMutations(const string& reference) {
    cout << "변이 적용 중 ..." << endl;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> nucleotide_dis(0, 3);
    static const string nucleotides = "ACGT";
    string mutated = reference;

    for (size_t i = 0; i < mutated.length(); ++i) {
        double mutationType = dis(gen);

        // 변이율에 대해서
        if (mutationType < MUTATION_RATE) {

            // 절반은 임의로 변경
            if (mutationType < MUTATION_RATE / 2) {
                mutated[i] = nucleotides[nucleotide_dis(gen)];
            }
            else { // 절반은 공백으로 변경 (복원 후에 빈칸에 들어갈 문자 결정)
                mutated[i] = '_';
            }
        }
        if (i % (mutated.length() / 100) == 0) {
            showProgress(static_cast<float>(i) / mutated.length());
        }
    }
    showProgress(1.0);
    cout << endl;
    return mutated;
}

// 트리생성
RBTree* buildTree(const string& reference) {
    cout << "트리 생성 중 ..." << endl;
    RBTree* tree = new RBTree();
    int maxProgress =
        reference.length() - READ_LENGTH - GAP_DISTANCE - READ_LENGTH;
    for (int i = 0; i < maxProgress; ++i) {
        string prefix = reference.substr(i, READ_LENGTH);
        string suffix =
            reference.substr(i + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);
        tree->insert(prefix, suffix, i);
        if (i % (maxProgress / 100) == 0) {
            showProgress(static_cast<float>(i) / maxProgress);
        }
    }
    showProgress(1.0);
    cout << endl;
    return tree;
}

// 내 DNA 중 일부 추출 prefix, suffix 순서쌍
Container<Pair<string, string> > generateReads(const string& sequence) {
    cout << "내 DNA 일부 추출 중 ..." << endl;
    Container<Pair<string, string> > reads;
    mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<> dis(0, sequence.length() - READ_LENGTH -
        GAP_DISTANCE - READ_LENGTH - 1);

    for (int i = 0; i < NUM_SUBSTRINGS; ++i) {
        // 부분 문자열 시작 index
        int start = dis(gen);

        // read 길이만큼 prefix
        string prefix = sequence.substr(start, READ_LENGTH);
        // prefix 뒤로 gap만큼 떨어진 곳에서 suffix
        string suffix =
            sequence.substr(start + READ_LENGTH + GAP_DISTANCE, READ_LENGTH);

        reads.push(Pair<string, string>(prefix, suffix));
        if (i % (NUM_SUBSTRINGS / 100) == 0) {
            showProgress(static_cast<float>(i) / NUM_SUBSTRINGS);
        }
    }
    showProgress(1.0);
    cout << endl;
    return reads;
}

// 내 DNA 복원
string reconstructSequence(const string& reference,
    const Container<Pair<string, string> >& reads,
    RBTree* tree) {
    cout << "내 DNA 복원 중 ..." << endl;

    // 복원 문자열 모두 빈칸으로 설정
    string reconstructed(REFERENCE_LENGTH, '_');

    // 생성한 prefix, suffix들로 문자열 매칭 시도
    for (size_t i = 0; i < NUM_SUBSTRINGS; ++i) {
        Container<Pair<string, int> > matches =
            tree->findMatches(reads[i].first, reads[i].second);
        for (const Pair<string, int> match : matches) {
            for (int j = 0; j < READ_LENGTH; ++j) {
                reconstructed[match.second + j] = reads[i].first[j];
                reconstructed[match.second + GAP_DISTANCE + READ_LENGTH + j] =
                    reads[i].second[j];
            }
        }
        if (i % (NUM_SUBSTRINGS / 100) == 0) {
            showProgress(static_cast<float>(i) / NUM_SUBSTRINGS);
        }
    }
    showProgress(1.0);
    cout << endl;

    for (size_t i = 0; i < REFERENCE_LENGTH; ++i) {
        if (reconstructed[i] == '_') {
            reconstructed[i] = reference[i];
        }
    }

    return reconstructed;
}

void fillBlanksInMutated(string& mutated) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> nucleotide_dis(0, 3);
    static const string nucleotides = "ACGT";

    for (size_t i = 0; i < REFERENCE_LENGTH; ++i) {
        if (mutated[i] == '_') {
            mutated[i] = nucleotides[nucleotide_dis(gen)];
        }
    }
}

int main() {
    high_resolution_clock::time_point overallStart =
        high_resolution_clock::now();

    string reference = generateRandomDNA(REFERENCE_LENGTH);
    writeSequenceToFile("reference.txt", reference);

    high_resolution_clock::time_point treeStart = high_resolution_clock::now();
    RBTree* tree = buildTree(reference);
    high_resolution_clock::time_point treeEnd = high_resolution_clock::now();
    cout << "트리 생성 시간 : "
        << duration_cast<seconds>(treeEnd - treeStart).count() << " seconds"
        << endl;

    for (int i = 0; i < 5; ++i) {
        string mutated = applyMutations(reference);
        writeSequenceToFile("mutated.txt", mutated);

        Container<Pair<string, string> > reads = generateReads(mutated);

        high_resolution_clock::time_point restoreStart =
            high_resolution_clock::now();
        string reconstructed = reconstructSequence(reference, reads, tree);

        high_resolution_clock::time_point restoreEnd =
            high_resolution_clock::now();

        fillBlanksInMutated(mutated);
        writeSequenceToFile("reconstructed.txt", reconstructed);

        int differences = 0;
        for (size_t j = 0; j < mutated.length(); ++j) {
            if (mutated[j] != reconstructed[j])
                differences++;
        }
        cout << i + 1 << "번째 반복" << endl;
        cout << "오차 개수: " << differences << endl;
        cout << "정확도: " << fixed << setprecision(2)
            << (1.0 - static_cast<double>(differences) / REFERENCE_LENGTH) *
            100
            << "%" << endl;
    }

    delete tree;
    return 0;
}
