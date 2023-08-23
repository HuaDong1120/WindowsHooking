#include <Windows.h>
#include <iostream>

int main()
{
	DWORD pid;

	//����library
	HMODULE kernel32 = LoadLibrary("kernel32.dll");

	LPVOID loadLibraryA = GetProcAddress(kernel32, "LoadLibraryA");

	HWND hwnd = FindWindow(0, TEXT("TESTAPP"));

	if (!hwnd) {
		std::cout<<"[-] Fail to FindWindow"<<std::endl;
		return 1;
	}
	GetWindowThreadProcessId(hwnd, &pid);

	std::cout<<"[+] PID of target : "<<pid<<std::endl;

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 1, pid);

	if (hProc == INVALID_HANDLE_VALUE) {
		std::cout<<"[-] Fail to OpenProcess"<<std::endl;
		return 1;
	}

	// put your path
	const char* path = "D:\\lib.dll";

	//�����ڴ�ռ�
	LPVOID pathAddress = VirtualAllocEx(hProc, NULL, strlen(path), (MEM_COMMIT | MEM_RESERVE), 0x40);

	if (!pathAddress)
	{
		std::cout<<"[-] Fail to VirtualAllocEx"<<std::endl;
		return 1;
	}

	std::cout<<"[+] pathAddress in target : 0x%x"<<pathAddress<<std::endl;

	SIZE_T bytesWritten = 0;

	int lpWrite = WriteProcessMemory(hProc, pathAddress, path, strlen(path), &bytesWritten);

	if (!lpWrite)
	{
		std::cout<<"[-] Fail to WriteProcessMemory"<<std::endl;
		return 1;
	}

	std::cout<<"[+] Bytes written : "<<bytesWritten<<std::endl;

	HANDLE remoteProc = CreateRemoteThread(hProc, NULL,	0, (LPTHREAD_START_ROUTINE)loadLibraryA, pathAddress, 0, NULL);
	if (!remoteProc)
	{
		std::cout<<"[-] Fail to CreateRemoteThread"<<std::endl;
		return 1;
	}

	std::cout<<"[+] Injected"<<std::endl;
	
	CloseHandle(hProc);
	
	getchar();

	return 0;
}
