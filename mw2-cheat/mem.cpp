#include "Mem.hpp"

bool mem::SetProcessID(const char* sProcessName) {

	HANDLE hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProc == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32  processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProc, &processEntry))
	{
		do
		{
			if (!strcmp(sProcessName, processEntry.szExeFile))
			{
				this->ulProcId = processEntry.th32ProcessID;
				CloseHandle(hProc);
				return true;
			}
		} while (Process32Next(hProc, &processEntry));
	}
	CloseHandle(hProc);

	return false;
}

bool mem::getModule(const char* name, MODULEENTRY32 &mod)
{
	HANDLE hMod = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	if (hMod == INVALID_HANDLE_VALUE)
		return NULL;

	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hMod, &modEntry))
	{
		do
		{
			std::cout << modEntry.szModule << std::endl;
			if (!strcmp(name, modEntry.szModule))
			{
				mod = modEntry;
				CloseHandle(hMod);
				return true;
			}
		} while (Module32Next(hMod, &modEntry));
	}
	CloseHandle(hMod);
	return false;
}

bool mem::CreateHandle() {
	this->hGameHandle = INVALID_HANDLE_VALUE;

	this->hGameHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ |PROCESS_VM_OPERATION, NULL, this->ulProcId);

	if (this->hGameHandle == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

mem::~mem() {
	CloseHandle(this->hGameHandle);
}

mem::mem(const char* sProcessName) {

	while (!this->SetProcessID(sProcessName)) {
		printf("\rSearching for %s  /", sProcessName);
		Sleep(800);
		printf("\rSearching for %s  -", sProcessName);
		Sleep(800);
		printf("\rSearching for %s  \\", sProcessName);
		Sleep(800);
		printf("\rSearching for %s  |", sProcessName);
		Sleep(800);
	}
	system("cls");

	if (!CreateHandle()) {
		std::cout << "Could not Create Handle";
		Sleep(3000);
		exit(-1);
	}
}

unsigned char mem::toByte(const char* sig)
{
	unsigned char byte_ret = 0;

	if (sig[0] >= 'A')
		byte_ret += sig[0] - 'A' + 10;
	else
		byte_ret += sig[0] - '0';
	byte_ret <<= 4;
	if (sig[1] >= 'A')
		byte_ret += sig[1] - 'A' + 10;
	else
		byte_ret += sig[1] - '0';
	return byte_ret;
}

bool mem::compare(unsigned char* bytes, const char* signature)
{
	for (; *signature; *signature == ' ' ? bytes : bytes++,signature++ )
	{
		if (*signature == ' ' || *signature == '?')
			continue;
		if (*bytes != toByte(signature))
			return false;
		signature++;
	}
	return true;
}

DWORD mem::sigScan(DWORD base, unsigned int size, const char* signature, int offset, bool relative, int extra)
{
	int length = sizeof(signature);
	unsigned char *bytes = new unsigned char[size];
	DWORD address = NULL;

	ReadProcessMemory(this->hGameHandle, (LPCVOID)base, (LPVOID)bytes, size, NULL);
	for (unsigned int i = 0; i < size; i++)
	{
		if (compare(bytes + i, signature))
		{
			address = base + i + offset;
			if (relative)
				ReadProcessMemory(this->hGameHandle, (LPCVOID)address, (LPVOID)&address, sizeof(address), NULL);
			address += extra;
			break;
		}
	}
	delete[] bytes;
	return address;
}
