//2021111935 김시환
#include<iostream>
using namespace std;

void printArray(int A[], int size) { //배열의 원소들을 출력하는 함수
	for (int i = 0; i < size; i++) {
		cout << A[i] << " ";
	}
	cout << endl;
}

void merge(int A[], int B[], int left, int mid, int right, int size) { //합병하는 함수
	int bufferPtr = left;
	int leftPtr = left; // 현재 정렬 중인 왼쪽 좌표
	int rightPtr = mid+1; //현재 정렬 중인 오른쪽 좌표
	
	while ((leftPtr <= mid) && (rightPtr <= right)) { //오른쪽과 왼쪽을 비교해가며 삽입
		if (A[leftPtr] > A[rightPtr]) B[bufferPtr++] = A[rightPtr++];
		else if (A[leftPtr] <= A[rightPtr]) B[bufferPtr++] = A[leftPtr++];
	}

	while (leftPtr <= mid) { //왼쪽 배열이 남은 경우
		B[bufferPtr++] = A[leftPtr++];
	}

	while (rightPtr <= right) { //오른쪽 배열이 남은 경우
		B[bufferPtr++] = A[rightPtr++];
	}

	for (int i = left; i <= right; i++) {
		A[i] = B[i];
	}
	printArray(A, size);
}

void mergeSort(int A[],int B[], int left, int right, int size) { //분할하는 함수
	if (left < right) { //left가 right보다 작을 떄까지 분할
		int mid = left + (right - left) / 2; //mid는 left와 right 중간

		mergeSort(A, B, left, mid, size);
		mergeSort(A, B, mid + 1, right, size);
		//왼쪽과 오른쪽으로 나눠서 분할

		merge(A, B, left, mid, right, size);
		//합병
	}
}

int main() {
	int A[] = { 30, 20, 40, 35, 5, 50, 45, 10, 25, 15 }; //배열
	int B[10]; //배열을 임시로 저장할 배열
	int size = 10; //배열의 크기
	
	cout << "2021111935 김시환 순환적 합병정렬 프로그램" << endl;
	cout << "정렬 전 배열: ";
	printArray(A, size);
	cout << endl;

	cout << "정렬 과정: " << endl;
	mergeSort(A, B, 0, size-1, size);
	cout << endl << "정렬 후 배열: ";
	printArray(A, size);

	return 0;
}