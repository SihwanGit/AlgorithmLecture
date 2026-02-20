//2021111935 김시환
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<stack>
using namespace std;
int Size = 10; //배열의 크기


//배열에 1부터 100000까지의 수를 랜덤으로 부여하는 함수
void randomAraay(int N[], int size) {
	for (int i = 0; i < size; i++) {
		N[i] = (rand() % 99999) + 1; //1부터 10만까지 중 하나 할당
	}
}

//배열의 두 원소의 위치를 바꿔주는 함수,
void swap(int& a, int& b) {
	int temp;
	temp = a;
	a = b;
	b = temp;
}


//배열의 데이터를 출력하는 함수
void print(int N[], int size) {
	for (int i = 0; i < size; i++) {
		cout << N[i] << ' ';
	}
	cout << endl;
}


//파티션 함수, 내림차순
int partition(int N[], int left, int right, int& count) {
	int pivot, temp;
	int low, high;
	low = left;
	high = right + 1;
	//do-while로 시작하자마자 low++, high--를 하기떄문에 low = left, high = right+1을 저장한다.
	pivot = N[left];

	do {
		do {
			low++;
		} while (N[low] > pivot); //a[low]가 pivot보다 작은 애가 발견될 때까지 low++
		do {
			high--;
		} while (N[high] < pivot); //a[high]중에 pivot보다 큰 애가 발견될 떄까지 high--
		if (low < high) { //그리고 low쪽의 큰값과 hjgh쪽의 작은값을 서로 교환
			swap(N[low], N[high]);
			cout << ++count << "번:" << '\t';
			print(N, Size);
		}
	} while (low < high); //위의 과정을 low와 high가 만나거나 교차하는 지점까지 반복
	//do while를 사용한 이유는 반복문이 한 번 이상 실행되야 하기 때문.

	swap(N[high], N[left]); //그 다음 pivot과 교차점의 원소끼리 서로 위치 교환
	return high;
}

//비순환적 퀵정렬
void quick_sort(int N[], int left, int right, int& count) { //size가 아니라 left부터 right까지다.
	stack<pair<int, int>> s;
	s.push({ left, right }); // 스택에 첫 범위(left, right) 저장

	while (!s.empty()) {
		int l = s.top().first;
		int r = s.top().second;
		s.pop();

		if (l < r) {
			int p = partition(N, l, r, count); // 피벗 설정

			// 스택에 정렬할 부분을 쌓는다 (재귀 대체)
			if (p - 1 > l) {
				s.push({ l, p - 1 }); // 피벗의 왼쪽 부분
			}
			if (p + 1 < r) {
				s.push({ p + 1, r }); // 피벗의 오른쪽 부분
			}
		}
	}
}


int main() {
	srand((unsigned)time(NULL));

	int  N[10];
	int size = 10;
	int count = 0; //변화 회수를 카운트

	randomAraay(N, size); //랜덤한 값 할당

	cout << "<비순환적 퀵정렬>" << endl;
	cout << "비순환적 퀵정렬은 for문과 스택을 사용해 구현할 수 있다." << endl;
	cout << "값이 바뀔 때마다 바뀐 값을 보여준다." << endl << endl;
	cout << "처음 배열: ";
	print(N, size);

	quick_sort(N, 0, size - 1, count); //비순환적 퀵정렬 실행

	cout << ++count << "번:" << '\t';
	print(N, size);
	return 0;
}