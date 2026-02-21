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
	int rightPtr = mid + 1; //현재 정렬 중인 오른쪽 좌표

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


void mergeSort(int A[], int B[], int size) { //분할하는 함수
	for (int width = 1; width < size; width *= 2) { // 부분 배열의 크기를 1, 2, 4, 8로 증가
		//width는 각 부분 배열의 크기
		for (int i = 0; i < size; i += 2 * width) { //부분배열 2개를 합쳐 정렬하므로 i += 2*width
			//두 블록의 시작과 끝은 i, i+2*width-1이 될 것이다.
			int left = i;
			int mid = (i + width - 1 < size - 1) ? i + width - 1 : size - 1;
			//mid는 i+width-1과 size-1 중 더 작은 것을 선택
			int right = (i + 2 * width - 1 < size - 1) ? i + 2 * width - 1 : size - 1;
			//right도 i+2*width-1과 size-1 중 더 작은 것을 선택

			//mid와 right가 ?:을 사용하는 이유는 mid/right가 배열의 크기 size를 벗어나지 않게 하기 위해서.
			merge(A, B, left, mid, right, size); // 부분 배열들을 병합
		}
	}
}

int main() {
	int A[] = { 30, 20, 40, 35, 5, 50, 45, 10, 25, 15 }; //배열
	int B[10];
	int size = 10;

	cout << "2021111935 김시환 비순환적 합병정렬 프로그램" << endl;
	cout << "정렬 전 배열: ";
	printArray(A, size);
	cout << endl;

	cout << "정렬 과정: " << endl;
	mergeSort(A, B, size);
	cout << endl << "정렬 후 배열: ";
	printArray(A, size);

	return 0;
}