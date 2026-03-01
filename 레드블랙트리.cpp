//2021111935 김시환
#include<iostream>
using namespace std;

enum Color {RED, BLACK};

class Node {
public:
	Node* right;
	Node* left;
	Node* parent;
	Color color;
	int data;

	Node(int data) { //새로운 노드 생성
		this->data = data;
		right = left = parent = nullptr;
		color = RED;
	}
};

class RedBlackTree {
private:
    Node* root;

//좌회전
    void leftRotate(Node* x) {
        //노드 하나가 좌,우,부모까지 3개의 링크를 가지고 있고 이중연결리스트처럼 서로 연결되어 있기 떄문에
        //노드 하나를 수정할 때 부모의 좌/우 링크와 자식의 부모 링크를 모두 변경해줘야한다.
        Node* y = x->right; //기존 y값을 잃어버리지 않기 위한 세팅
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;

        y->parent = x->parent; //y의 부모를 x의 부모로 변경
        //x부모의 자식을 y로 변경
        if (x->parent == nullptr) //null이였다면 root로
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y; //x가 x부모의 왼쪽 자식이였다면 그걸 y의 부모로 바꿔줌.
        else
            x->parent->right = y; //오른쪽이였다면 오른쪽 부모로 바꿔줌.
        y->left = x;
        x->parent = y;
    }

//우회전
    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right; //y의 오른쪽 자식과 x의 관계를 세팅
        if (y->right != nullptr)
            y->right->parent = x;

        y->parent = x->parent; //y와 x의 부모 사이의 관계를 세팅
        if (x->parent == nullptr)
            root = y;
        else if (x->parent->left == x)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->right = x; //x와 y의 관계 세팅
        x->parent = y;
    }

//수정
    void fixInsert(Node* k) {
        while (k->parent != nullptr && k->parent->color == RED) { //루트가 되거나, 부모가 흑색이 되면 종료
            if (k->parent == k->parent->parent->left) { //k의 부모가 조부모의 왼쪽이면, 삼촌은 조부모의 오른쪽
                Node* u = k->parent->parent->right; // 삼촌 노드
                if (u != nullptr && u->color == RED) { //삼촌과 부모 모두 red면, 둘다 black로 변경 후 조부모를 red로
                    k->parent->color = BLACK;
                    u->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent; //변경 후 k는 조부모로 이동후 재검사
                }
                else { //부모는 red인데, 삼촌은 black면
                    if (k == k->parent->right) { //오른쪽 부모면 한 번 좌회전
                        k = k->parent;
                        leftRotate(k);
                    }
                    k->parent->color = BLACK; //부모와 조부모의 색을 바꾼 후,
                    k->parent->parent->color = RED;
                    rightRotate(k->parent->parent); //k의 부모와 k의 조부모가 서로 우회전
                }
            }
            else { //만약 k가 조부모의 오른쪽이면, 삼촌은 조부모의 왼쪽
                Node* u = k->parent->parent->left; // 삼촌 노드
                if (u != nullptr && u->color == RED) { //위와 같음.
                    k->parent->color = BLACK;
                    u->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                }
                else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    leftRotate(k->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    //삽입
    void insert(Node* root, Node* pt) { 
        if (root == nullptr) { //루트가 널이면 루트에 삽입
            root = pt;
        }
        else if (pt->data < root->data) { //루트와 삽입할 pt간의 관계 비교
            if (root->left == nullptr) { //왼쪽이 비면 왼쪽에 삽입
                root->left = pt;
                pt->parent = root;
            }
            else { //이미 다른게 있으면 재귀를 통해 다시 비교
                insert(root->left, pt);
            }
        }
        else if (pt->data >= root->data) { //더 크거나 같으면
            if (root->right == nullptr) { //오른쪽이 비면 그냥 삽입
                root->right = pt;
                pt->parent = root;
            }
            else { //이미 다른게 있다면 재귀를 통해 다시 비교
                insert(root->right, pt);
            }
        }
    }

    void inorderTraversal(Node* root) { //재귀를 통한 출력함수
        if (root == nullptr) //트리가 비어있으면 그냥 종료
            return;
        inorderTraversal(root->left);  //전위순회를 통한 트리 출력
        cout << root->data << "(" << (root->color == RED ? "RED" : "BLACK") << ") ";
        inorderTraversal(root->right);
    }

public:
    RedBlackTree() { root = nullptr; } //레드블랙트리 객체 생성

    void insertValue(int data) { //정수 데이터 삽입
        Node* node = new Node(data);
        if (root == nullptr) { //루트에 삽입됐으면 블랙으로 바꿔줌.
            root = node;
            root->color = BLACK; 
        }
        else {
            insert(root, node); //그렇지 않다면 삽입 후 수정
            fixInsert(node);
        }
    }

    void display() { //출력
        inorderTraversal(root);
        cout << endl;
    }
 
};

int main() {
    RedBlackTree rbTree; //트리 객체 생성

    //20, 15, 14, 12, 13, 1 삽입
    cout << "2021111935 김시환 레드블랙트리 노드 출력" << endl;
    int data[] = { 20, 15, 14, 13, 12, 1 };
    int size = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < size; i++) {
        cout << i + 1 << ": ";
        rbTree.insertValue(data[i]);
        rbTree.display();
    }

	return 0;
}