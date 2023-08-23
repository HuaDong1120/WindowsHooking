#include <Windows.h>
#include <iostream>

void function() {
	std::cout << "normal working state on:" << *function << std::endl;
}

int main() {

	SetConsoleTitle(TEXT("TESTAPP"));

	std::cout << "program start:" << std::endl;
	while (true) {
		Sleep(1000);
		function();
	}
	getchar();
	return 0;
}