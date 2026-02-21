//2021111935 김시환
#include<iostream>
using namespace std;

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

int main() {
    int A[] = { 4, 1, 3, 2, 16, 9, 10, 14, 8, 7 };
    int size = 10;
    // 최대 힙 생성
    MaxHeap(A, size);

    // 힙 배열 출력
    cout << "2021111935 김시환 최대 힙으로 재정비된 배열 " << endl;
    for (int i = 0; i < size; i++) {
        cout << A[i] << " ";
    }
    cout << endl;

    return 0;
}