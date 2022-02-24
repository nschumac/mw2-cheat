#pragma once
#include "Header.hpp"

class mem {
private:

	HANDLE hGameHandle;
	DWORD ulProcId;

	bool SetProcessID(const char* processName);

	bool CreateHandle();

	unsigned char toByte(const char* sig);
	bool compare(unsigned char* bytes, const char* signature);


public:

	bool getModule(const char* name, MODULEENTRY32 &mod);

	DWORD sigScan(DWORD base, unsigned int size, const char* signature, int offset, bool relative, int extra);

	template<class T>
	void WriteProcess(DWORD addy, T value);

	template<class T>
	void WriteProcess(DWORD addy, std::vector<T> value, unsigned int amount);

	template<class T>
	void WriteProcess(DWORD addy, T value, BYTE Offset);

	template<class T>
	T ReadProcesss(DWORD addy, unsigned int size = sizeof(T))
	{
		T ret;
		ReadProcessMemory(this->hGameHandle, (LPCVOID)addy, (LPVOID)&ret, size, NULL);
		return ret;
	}

	HANDLE getHandle() { return this->hGameHandle; }

	mem(const char* sProcessName);
	~mem();

};

template<class T>
inline void mem::WriteProcess(DWORD addy, T value) {
	WriteProcessMemory(this->hGameHandle, (LPVOID)addy, (LPCVOID)&value, sizeof(value), NULL);
}


template<class T>
inline void mem::WriteProcess(DWORD addy, T value, BYTE Offset) {
	DWORD newAddy = 0;
	ReadProcessMemory(this->hGameHandle, (LPCVOID)addy, (LPVOID)&newAddy, sizeof(newAddy), NULL);
	if (newAddy)
		WriteProcessMemory(this->hGameHandle, (LPVOID)(newAddy + Offset), (LPCVOID)&value, sizeof(value), NULL);
}
template<class T>
void mem::WriteProcess(DWORD addy, std::vector<T> value, unsigned int amount)
{
	WriteProcessMemory(this->hGameHandle, (LPVOID)addy, value.data(), amount, NULL);
}
