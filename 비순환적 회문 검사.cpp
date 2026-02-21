#include <iostream>
#include<fstream>
#include<string>
using namespace std;

void isPalindrome(string str);

int main() {
	cout << "2021111935 김시환 비순환적 회문 검사 프로그램" << endl;
	ifstream fin("회문.txt");
	if (!fin) { //에러 검사
		cout << "텍스트 파일을 여는데 실패했습니다." << endl << endl;
		return 0;
	}
	else {
		cout << "회문.txt 파일 오픈 성공" << endl << endl;
	}

	string str;
	while (getline(fin, str)) { //파일에서 문장별로, 파일이 끝날 때까지 읽는다.
		isPalindrome(str);
	}

	cout << endl << "회문.txt 닫기" << endl;
	fin.close(); //파일 닫기
	return 0;
}

void isPalindrome(string str) { //회문을 검사하는 함수
	char* ch = new char[str.length()];
	for (int i = 0; i < str.length(); i++) {
		ch[i] = str[i];
	} //문자열을 앞에서부터 배열에 넣는다.

	bool palindromeCheck = true;
	for (int i = 0; i < str.length(); i++) {
		if (ch[i] != ch[str.length() - 1 - i]) {
			palindromeCheck = false; //만약 뒷부분과 앞부분이 같은 문자가 아니라면 flase다.
		}
	}

	if (palindromeCheck == true) {
		cout << str << "은 회문입니다." << endl;
	}
	else {
		cout << str << "은 회문이 아닙니다." << endl;
	}

}