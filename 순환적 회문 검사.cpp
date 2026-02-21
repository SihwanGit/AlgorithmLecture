#include <iostream>
#include<fstream>
#include<string>
using namespace std;

bool isPalindrome(string& str, int left, int right) {
	if (left >= right) { //left+1과 right-1을 반복하다가 left와 right가 교차하면 회문
		return true;
	}
	// 양 끝의 문자가 다르면 회문이 아님
	if (str[left] != str[right]) {
		return false;
	}
	// 내부 문자를 재귀적으로 비교
	return isPalindrome(str, left + 1, right - 1);
}

int main() {
	cout << "2021111935 김시환 순환적 회문 검사 프로그램" << endl;
	ifstream fin("회문.txt");
	if (!fin) { //에러 검사
		cout << "텍스트 파일을 여는데 실패했습니다." << endl << endl;
		return 0;
	}
	else {
		cout << "회문.txt 파일 오픈 성공" << endl << endl;
	}
	string str;
	while (getline(fin, str)) {
		if (isPalindrome(str, 0, str.length() - 1)) {
			cout << str << "은 회문입니다." << endl;
		}
		else {
			cout << str << "은 회문이 아닙니다." << endl;
		}

	}
	
	cout << endl << "회문.txt 닫기" << endl;
	fin.close(); //파일 닫기
	return 0;
}