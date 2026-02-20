//2021111935 김시환
#include <iostream>
using namespace std;
int Size = 8; //배열의 원래 크기

//두 변수의 값을 서로 바꿔주는 함수
void swap(int &a, int &b) {
	int temp;
	temp = a;
	a = b;
	b = temp;
}

//배열의 모든 원소를 출력해주는 함수
void print(int A[], int size) {
	for (int i = 0; i < size; i++) {
		cout << A[i] << ' ';
	}
	cout << endl;
}

//버블 정렬을 한 번만 실행
void bubbleOnce(int A[], int size, int& count) { 
	for (int i = 0; i < size - 1; i++) {
		if (A[i] > A[i + 1]) {
			swap(A[i], A[i + 1]);
			cout << count++ << "번: " << '\t';
			print(A, Size);
		}
	}
}

//bubbleOnce 함수의 크기를 줄여가며 재귀적으로 실행
void bubbleSort(int A[], int size, int& count) { 
	if (size == 1) { return; }
	else {
		bubbleOnce(A, size, count);
	}
	bubbleSort(A, size - 1, count);
}

//순환적 버블정렬
int main() {
	int A[] = { 30, 20, 40, 10, 5, 10, 30, 15 };

	cout << "<순환적 버블 정렬>" << endl;
	cout << "값이 바뀔 때마다 바뀐 값을 보여준다." << endl << endl;
	cout << "처음 배열: "; print(A, 8);
	
	int count = 0;
	bubbleSort(A, 8, count);

	return 0;
}