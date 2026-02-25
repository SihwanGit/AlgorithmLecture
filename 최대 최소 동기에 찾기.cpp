//2021111935 김시환
#include<iostream>
#include<random>
using namespace std;

void findMinMax(int a[], int size, int& max, int& min) {
	min = a[0]; //최대
	max = a[0]; //최소
	int small, large; //극소, 극대
	for (int i = 1; i < size - 1; i += 2) {
		if (a[i] >= a[i + 1]) {
			large = a[i];
			small = a[i + 1];
		}
		else {
			large = a[i + 1];
			small = a[i];
		}
		if (large > max) max = large; //극대와 최대 비교
		if (small < min) min = small; //극소와 최소 비교
	}
	return;
}

int main() {
	random_device rd; //랜덤 시드
	mt19937 gen(rd()); //메르센 트위스터 알고리즘 실행
	int randArray[1000];
	uniform_int_distribution<> distribute(1, 100000); //1부터 100000 사이의 균등분포
	for (int i = 0; i < 1000; i++) { //1~100000 사이의 랜덤한 수 1000개 추가.
		randArray[i] = distribute(gen);
	}

	int max, min;
	findMinMax(randArray, 1000, max, min);
	cout << "2021111935 김시환 findMaxMin 함수를 이용한 최대/최소 구하기" << endl;
	cout << "최소값은 " << min << "입니다." << endl;
	cout << "최대값은 " << max << "입니다." << endl;

	return 0;
}