//2021111935 김시환
#include <iostream>
using namespace std;

//두 변수의 값을 서로 바꿔주는 함수
void swap(int& a, int& b) {
	int temp;
	temp = a;
	a = b;
	b = temp;
}

//배열의 모든 원소를 출력해주는 함수
void print(int a[], int size) {
	for (int i = 0; i < size; i++) {
		cout << a[i] << ' ';
	}
	cout << endl;
}

//비순환적 버블정렬
int main() {
	int A[] = { 30, 20, 40, 10, 5, 10, 30, 15 };
	cout << "<비순환적 버블 정렬>" << endl;
	cout << "값이 바뀔 때마다 바뀐 값을 보여준다." << endl << endl;
	cout << "처음 배열: "; print(A, 8);

	int size = 8;
	int count = 1; //바뀐 회수
	for (int i = 0; i < size-1; i++) { //인접한 애들끼리 비교
		for (int j = 0; j < size-1; j++) {
			if (A[j] > A[j+1]) { //앞의 숫자가 더 크면 변경
				swap(A[j], A[j + 1]);
				cout << count++ << "번: " << '\t';
				print(A, 8); //숫자를 바꾸는 경우에만 출력
			}
		}
	}
	return 0;
}