//2021111935 김시환
#include<iostream>
#include<random>
using namespace std;

int Minimum(int a[], int size) { //최소값을 구하는 함수
	int temp = a[0];
	for (int i = 0; i < size; i++) {
		if (temp > a[i]) temp = a[i]; //만약 temp가 a[i]보다 크다면 최소값 변경
	}
	return temp;
}

int Maximum(int a[], int size) { //최대값을 구하는 함수
	int temp = a[0];
	for (int i = 0; i < size; i++) {
		if (temp < a[i]) temp = a[i]; //만약 temp가 a[i]보다 작다면 최대값 변경
	}
	return temp;
}

int main() {
	random_device rd; //랜덤 시드
	mt19937 gen(rd()); //메르센 트위스터 알고리즘 실행
	int randArray[1000];
	uniform_int_distribution<> distribute(1, 100000); //1부터 100000 사이의 균등분포
	for (int i = 0; i < 1000; i++) { //1~100000 사이의 랜덤한 수 1000개 추가.
		randArray[i] = distribute(gen);
	}

	int min = Minimum(randArray, 1000);
	int max = Maximum(randArray, 1000);
	cout << "2021111935 김시환 Maximum함수와 Minimum 함수를 이용한 최대/최소 구하기" << endl;
	cout << "최소값은 " << min << "입니다." << endl;
	cout << "최대값은 " << max << "입니다." << endl;

	return 0;
}