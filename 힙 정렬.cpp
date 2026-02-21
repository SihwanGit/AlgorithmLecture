//2021111935 김시환
#include<iostream>
using namespace std;

void printArray(int arr[], int n) { //배열의 원소들을 출력하는 함수
    for (int i = 0; i < n; i++) {
        cout << arr[i] << ' ';
    }
    cout << endl;
}

// 힙화 과정에서 사용되는 함수
void heapify(int arr[], int n, int i) {
    int largest = i; // 현재 노드를 가장 큰 노드로 가정
    int left = 2 * i + 1; // 왼쪽 자식 인덱스
    int right = 2 * i + 2; // 오른쪽 자식 인덱스

    // 왼쪽 자식이 현재 노드보다 큰 경우
    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    // 오른쪽 자식이 현재 노드보다 큰 경우
    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    //이 경우에는 왼쪽/오른쪽이 모두 부모 노드보다 큰 경우 오른쪽부터 바꾼다.
    // 가장 큰 노드가 현재 노드가 아닌 경우 스왑하고 재귀적으로 힙화
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
        //자기보다 작은 모든 부분 트리에 대해 반복해야하므로 재귀 사용
    }
}

// 배열을 최대 힙으로 변환하는 함수
void MaxHeap(int arr[], int n) {
    // 마지막 부모 노드부터 시작하여 힙화 과정 수행
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
}

void heapSort(int arr[], int n) { //힙 정렬 함수
    for (int i = n - 1; i > 0; i--) { //맨 뒤의 원소를 맨 앞으로 끌고와야하므로 역순으로 진행
        swap(arr[0], arr[i]); //앞 뒤 바꿈
        heapify(arr, i, 0); 
        //i가 배열의 크기, 0은 맨 앞에서부터 maxHeap 검사를 실행하겠다는 뜻.
    }

}

int main() {
    int A[] = { 4, 1, 3, 2, 16, 9, 10, 14, 8, 7 };
    int size = 10;
    // 최대 힙 생성
    MaxHeap(A, size);

    // 힙 배열 출력
    cout << "2021111935 김시환" << endl;


    cout << "MAX힙: ";
    printArray(A, size);
    cout << "힙정렬: ";
    heapSort(A, size);
    printArray(A, size);
    return 0;
}