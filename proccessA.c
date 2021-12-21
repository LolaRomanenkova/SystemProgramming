#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>

#define MAXLENGTH 101

void main(void)
{		
	DWORD GetProcessID(char*);                                      // функция получения ID процесса
	HANDLE hProcessB;												// дескриптор процесса B
	LPVOID szAddress;												// адрес в памяти процесса B
	DWORD cbWritten, cbRead, dBuf;									// буфферы (имеют промежуточное значение)
	BOOL Success;													// флаг успешного открытия/считывания/записи
	DWORD szSize = sizeof(char) * MAXLENGTH;						// размер вводимой строки
	char* str = (char*)malloc(szSize);								// сама строка
	HANDLE hPipe = INVALID_HANDLE_VALUE;	                        // дескриптор канала 
	wchar_t m1[] = L"lab3_2.exe";

	printf("Enter your string: ");
	gets_s(str, MAXLENGTH - 1);

	printf("Opening ProcessB...");
	hProcessB = OpenProcess(
		PROCESS_ALL_ACCESS,											// устанавливает уровень доступа к объекту процесса
		FALSE,														// параметр дескриптора наследования
		GetProcessID(m1));	                                        // ID процесса (в данном случае - процесса B)
	if (hProcessB == INVALID_HANDLE_VALUE) {
		printf("Failure!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nAllocating memory for ProcessB...");
	szAddress = VirtualAllocEx(
		hProcessB,													// дескриптор процесса (в данном случае - процесса B)
		0,															// система сама решает, где выделять память
		szSize,														// объём резервируемой памяти (размер нашей строки)
		MEM_RESERVE |												// память зарезервирована и освобождена
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);									// атрибут защиты

	printf("Success!\nWriting your string in ProcessB memory...");
	Success = WriteProcessMemory(
		hProcessB,													// дескриптор процесса (в данном случае - процесса B)
		szAddress,													// куда записывать
		str,														// что записывать
		szSize,														// объём записываемых данных
		NULL);														// указатель на переменную, которая получает число байтов, переданное в указанный процесс (игнорируется)
	if (Success == FALSE) {
		printf("Failure!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nConnecting to named pipe...");
	while (hPipe == INVALID_HANDLE_VALUE) {
		hPipe = CreateFile(
			L"\\\\.\\pipe\\pipename",								// имя именованного канала
			GENERIC_READ |											// требуем доступ к чтению и записи
			GENERIC_WRITE,
			0,														// без совместного доступа 
			NULL,													// без дескрипторов защиты
			OPEN_EXISTING,											// открываем существующий канал
			0,														// атрибуты файла по умолчанию 
			NULL);													// без дескриптора файла шаблона
	}

	printf("Success!\nWriting memory address...");
	dBuf = (DWORD)szAddress;
	Success = WriteFile(
		hPipe,														// дескриптор канала 
		&dBuf,														// данные для записи (в данном случае - адрес памяти в процессе B) 
		sizeof(dBuf),												// объём записываемых данных
		&cbWritten,													// количество записанных данных (не используем)
		NULL);														// асинхронный буфер (не используем)
	if (Success == FALSE) {
		printf("Failure!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nWaiting answer from ProcessB...");
	Success = ReadFile(
		hPipe,														// имя именованного канала
		&szAddress,													// буфер для считывания
		sizeof(szAddress),											// объём считываемых данных
		&cbRead,													// количество считанных данных (не используем)
		NULL);														// асинхронный буфер (не используем)
	if (Success == FALSE) {
		printf("Failure!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nReleasing ProcessB memory...");
	Success = VirtualFreeEx(
		hProcessB,
		szAddress,
		0,
		MEM_RELEASE);
	if (Success == FALSE) {
		printf("Failure!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nClosing named pipe...");
	Success = CloseHandle(hPipe);
	if (Success == FALSE) {
		printf("Failure!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nClosing ProcessB...");
	Success = CloseHandle(hProcessB);
	if (Success == FALSE) {
		printf("Failure!!!\n");
		system("pause");
		return -1;
	}

	printf("Success!\nProcessA was finished...\n");
	system("pause");
	return 0;
}

DWORD GetProcessID(TCHAR* process) {
	PROCESSENTRY32 processEntry;
	DWORD processID = 0;
	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Snapshot != INVALID_HANDLE_VALUE) {
		processEntry.dwSize = sizeof(processEntry);
		Process32First(Snapshot, &processEntry);

		do {
			if (wcscmp(processEntry.szExeFile, process) == 0) {
				processID = processEntry.th32ProcessID;
				CloseHandle(Snapshot);
				return processID;
			}
		} while (Process32Next(Snapshot, &processEntry));

		CloseHandle(Snapshot);
	}

	return INVALID_HANDLE_VALUE;
}

	
