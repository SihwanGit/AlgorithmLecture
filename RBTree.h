// RBTree.h
#include <iostream>
#include <string>
using namespace std;
class LinkedList {
public:
    int index;
    string suffix;
    LinkedList* next;
    LinkedList(string suffix, int index) {
        this->index = index;
        this->suffix = suffix;
        next = NULL;
    }
    void print(int cnt) {
        if (next) {
            cnt++;
            next->print(cnt);
        }
        else {
            cout << "suffix 개수 : " << cnt;
        }
    }
    void link(LinkedList* newInfo) { newInfo->next = this; }
};

class RBNode {
private:
    string color;
    string prefix;
    LinkedList* info;
    RBNode* parent;
    RBNode* left;
    RBNode* right;

public:
    RBNode(string prefix, string suffix, int index, int isRoot = 0)
        : color(isRoot ? "BLACK" : "RED"), prefix(prefix),
        info(new LinkedList(suffix, index)), parent(nullptr), left(nullptr),
        right(nullptr) {}

    void toggleColor() { // 현재 설정된 색상 반전
        if (this->color == "RED") {
            this->color = "BLACK";
        }
        else {
            this->color = "RED";
        }
    }

    void print(int n = 0) {
        if (this->right) {
            this->right->print(n + 1);
        }
        else {
            for (int i = 0; i < n + 1; i++) {
                cout << "       ";
            }
            cout << "Nil" << endl;
        }

        for (int i = 0; i < n; i++) {
            cout << "       ";
        }

        cout << this->prefix << " ";
        this->info->print(1);
        cout << "(" << this->color << ")" << endl;

        if (this->left) {
            this->left->print(n + 1);
        }
        else {
            for (int i = 0; i < n + 1; i++) {
                cout << "       ";
            }
            cout << "Nil" << endl;
        }
    }


    RBNode* RBTree::insert(RBNode* root, char value) {
        // 삽입 로직 (RBTree의 규칙에 따라 노드를 삽입)
        // 노드 초기화 및 메모리 할당
        RBNode* newNode = new RBNode(value);

        // 기본 트리 삽입 로직
        if (!root) {
            return newNode; // 트리가 비어 있으면 새 노드가 루트
        }

        if (value < root->data) {
            root->left = insert(root->left, value);
        }
        else if (value > root->data) {
            root->right = insert(root->right, value);
        }

        // 균형 유지 로직 (RBTree 규칙)
        return balance(root);
    }
    
    /*
    void insert(string prefix, string suffix, int index) {
        int ErrorRate = 20; //허용 오차
        int len = prefix.length();
        int cnt = 0;

        for (int i = 0; i < len; i++) {
            if (this->prefix[i] != prefix[i]) {
                cnt++;
                if (cnt > ErrorRate)
                    break;
            }
        }
        if (cnt <= ErrorRate) {
            LinkedList* newInfo = new LinkedList(suffix, index);
            info->link(newInfo);
            info = newInfo;
        }
        else if (this->prefix < prefix) {
            if (this->right) {
                this->right->insert(prefix, suffix, index);
            }
            else {
                RBNode* node = new RBNode(prefix, suffix, index);
                this->right = node;
                node->parent = this;
                this->fixTree(node);
            }
        }
        else if (this->prefix > prefix) {
            if (this->left) {
                this->left->insert(prefix, suffix, index);
            }
            else {
                RBNode* node = new RBNode(prefix, suffix, index);
                this->left = node;
                node->parent = this;
                this->fixTree(node);
            }
        }
        // 루트 노드로 이동
        RBNode* root = this->getRoot();
    }
    */

    void turnLeft() {
        if (!this->parent)
            return; // 부모가 없으면 바로 리턴
        RBNode* p = this->parent;
        RBNode* gp = p->parent;

        if (!gp) { // 조부모가 없으면 루트 노드 처리
            p->right = this->left;
            if (this->left)
                this->left->parent = p;
            this->left = p;
            p->parent = this;
            this->parent = nullptr; // 루트 노드
        }
        else {
            if (p == gp->left) {
                p->right = this->left;
                if (this->left)
                    this->left->parent = p;
                this->left = p;
                p->parent = this;
                gp->left = this;
                this->parent = gp;
            }
            else {
                p->right = this->left;
                if (this->left)
                    this->left->parent = p;
                this->left = p;
                p->parent = this;
                gp->right = this;
                this->parent = gp;
            }
        }
    }

    void turnRight() {
        if (!this->parent)
            return; // 부모가 없으면 바로 리턴
        RBNode* p = this->parent;
        RBNode* gp = p->parent;

        if (!gp) { // 조부모가 없으면 루트 노드 처리
            p->left = this->right;
            if (this->right)
                this->right->parent = p;
            this->right = p;
            p->parent = this;
            this->parent = nullptr; // 루트 노드
        }
        else {
            if (p == gp->left) {
                p->left = this->right;
                if (this->right)
                    this->right->parent = p;
                this->right = p;
                p->parent = this;

                gp->left = this;
                this->parent = gp;
            }
            else {
                p->left = this->right;
                if (this->right)
                    this->right->parent = p;
                this->right = p;
                p->parent = this;

                gp->right = this;
                this->parent = gp;
            }
        }
    }

    void fixTree(RBNode* z) {
        if (z->parent) {
            // 조정하고자 하는 노드가 루트 노드가 아닐경우 조정
            // 루트 노드의 조정은 색만 확인

            RBNode* p = z->parent;   // 기준 노드의 부모노드
            RBNode* gp = p->parent;  // 기준 노드의 조부모노드
            if (p->color == "RED") { // 부모노드가 RED -> 조정필요
                if (p == gp->left) { // 부모가 왼쪽 자식일 경우
                    if (gp->right && gp->right->color == "RED") {
                        // 삼촌노드가 적색 노드일 경우
                        p->toggleColor();
                        if (gp->right) {
                            gp->right->toggleColor();
                            // 삼촌노드 흑색으로 설정
                        }
                        gp->toggleColor(); // 조부모 노드 적색으로 설정
                        this->fixTree(gp); // 조부모 노드 기준으로 트리 수정
                    }
                    else {               // 삼촌이 Nil이거나, 흑색노드인 경우
                        if (z == p->right) { // Case 2인 경우
                            z->turnLeft();   // Case 3가 됨
                            z->turnRight();
                            z->toggleColor(); // 조부모가 된 z 흑색으로 설정
                            z->right
                                ->toggleColor(); // 자식이된 노드 적색으로 설정
                        }
                        else { // Case 3의 경우 부모를 기준으로 우회전
                            p->turnRight();
                            p->toggleColor(); // 조부모가 된 p 흑색으로 설정
                            p->right
                                ->toggleColor(); // 자식이된 노드 적색으로 설정
                        }
                    }
                }
                else { // 부모가 오른쪽 자식일 경우
                    if (gp->left && gp->left->color == "RED") {
                        // 삼촌노드가 적색 노드일 경우
                        p->toggleColor(); // 부모노드 흑색으로 설정
                        if (gp->left) {
                            gp->left->toggleColor();
                            // 삼촌노드 흑색으로 설정
                        }
                        gp->toggleColor();  // 조부모 노드 적색으로 설정
                        this->fixTree(gp);  // 조부모 노드 기준으로 트리 수정
                    }
                    else {                // 삼촌이 Nil이거나, 흑색노드인 경우
                        if (z == p->left) { // Case 2인 경우
                            z->turnRight(); // Case 3가 됨
                            z->turnLeft();
                            z->toggleColor(); // 조부모가 된 z 흑색으로 설정
                            z->right
                                ->toggleColor(); // 자식이된 노드 적색으로 설정
                        }
                        else { // Case 3의 경우 부모를 기준으로 좌회전
                            p->turnLeft();
                            p->toggleColor(); // 조부모가 된 p 흑색으로 설정
                            p->left
                                ->toggleColor(); // 자식이된 노드 적색으로 설정
                        }
                    }
                }
            }
        }

        // 루트 노드 흑색으로 설정
        RBNode* tmp = this;
        while (tmp->parent) {
            tmp = tmp->parent;
        }
        tmp->color = "BLACK";
    }

    RBNode* getRoot() {
        RBNode* tmp = this;
        while (tmp->parent) {
            tmp = tmp->parent;
        }
        return tmp;
    }

    RBNode* RBNode::buildTree(const string& data) {
        if (data.empty()) {
            return nullptr; // 데이터가 비어있으면 빈 트리를 반환
        }

        RBNode* root = nullptr;
        for (size_t i = 0; i < data.length(); i++) {
            // 데이터를 트리에 삽입하는 로직
            root = insert(root, data[i]); // insert는 RBTree의 삽입 함수라고 가정
        }

        return root; // 생성된 트리의 루트를 반환
    }

    /*
    RBNode* buildTree(string origin) {
        if (origin.length() < 250) {
            cerr << "Error: Input string is too short to build the tree." << endl;
            return nullptr;
        }

        RBNode* root = nullptr;

        string prefix = origin.substr(0, 75);
        string suffix = origin.substr(175, 75);
        root = new RBNode(prefix, suffix, 0, 1);

        for (int i = 1; i < origin.length() - 250; i++) {
            string prefix = origin.substr(i, 75);
            string suffix = origin.substr(i + 175, 75);
            root->insert(prefix, suffix, i);
            root = root->getRoot();
        }
        cout << endl; // 진행 상태 끝

        return root;
    }
    */

    int findPrefix(string prefix, string suffix) {
        int ErrorRate = 20;
        int len = prefix.length();
        int cntPre = 0;

        // Compare the prefix with the current node's prefix
        for (int i = 0; i < len; i++) {
            if (this->prefix[i] != prefix[i]) {
                cntPre++;
                if (cntPre > ErrorRate)
                    break;
            }
        }

        if (cntPre <= ErrorRate) { // Prefix matches within ErrorRate
            LinkedList* info = this->info;

            // Iterate through the suffixes in the linked list
            while (info) {
                int cntSuf = 0;
                for (int i = 0; i < len; i++) {
                    if (info->suffix[i] != suffix[i]) {
                        cntSuf++;
                        if (cntSuf > ErrorRate)
                            break;
                    }
                }
                if (cntSuf <= ErrorRate) { // Suffix also matches
                    return info->index;
                }
                info = info->next; // Move to the next node in the linked list
            }
        }


        if (this->prefix < prefix) {
            if (this->right) {
                return this->right->findPrefix(prefix, suffix);
            }
        }
        else {
            if (this->left) {
                return this->left->findPrefix(prefix, suffix);
            }
        }

        return -1;
    }
};