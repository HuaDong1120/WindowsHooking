#include <Windows.h>
#include <iostream>

DWORD functionAddress = 0x411320;


void hook() {

	std::cout << "Hooked" << std::endl;
}

void writeTheHook() {

	std::cout<<"[+] hookAddress : "<<*hook<<std::endl;

	const int instructionSize = 5;

	DWORD relativeAddress = ((DWORD)hook - (DWORD)functionAddress) - instructionSize;

	std::cout<<"[+] relativeAddress : "<<relativeAddress<<std::endl;

	DWORD defaultProtection;
	BOOL pageChange = VirtualProtect((LPVOID)functionAddress, instructionSize, PAGE_EXECUTE_READWRITE, &defaultProtection);

	if (!pageChange)
	{
		std::cout<<"[-] Fail to VirtualProtect"<<std::endl;
		return;
	}

	byte jmpInstruction[instructionSize];

	jmpInstruction[0] = 0xE9; // jmp
	jmpInstruction[1] = relativeAddress;
	jmpInstruction[2] = (relativeAddress >> 8);
	jmpInstruction[3] = (relativeAddress >> 16);
	jmpInstruction[4] = (relativeAddress >> 24);

	SIZE_T bytesWritten;

	int lpWrite = WriteProcessMemory(GetCurrentProcess(), (LPVOID)functionAddress, jmpInstruction, instructionSize, &bytesWritten);

	if (!lpWrite)
	{
		std::cout<<"[-] Fail to WriteProcessMemory"<<std::endl;
		return;
	}

	std::cout<<"[+] Jump writed at 0x%x"<<functionAddress<<std::endl;

	DWORD oldProtect;
	pageChange = VirtualProtect((LPVOID)functionAddress, instructionSize, defaultProtection, &oldProtect);

	if (!pageChange)
	{
		std::cout << "[-] Fail to VirtualProtect" << std::endl;
		return;
	}
}

DWORD WINAPI thread(LPVOID param) {

	std::cout << "[+] Thread started" << std::endl;

	writeTheHook();

	while (true) {
		Sleep(100);
	}
	FreeLibraryAndExitThread((HMODULE)param, 0);
	return 0;
}

bool WINAPI DllMain(HINSTANCE hinstDLL,	DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{

	case DLL_PROCESS_ATTACH:

		CreateThread(0, 0, thread, hinstDLL, 0, 0);

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;

	}

	return true;
}
