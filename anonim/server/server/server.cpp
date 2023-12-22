#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <process.h>

using namespace std;

const int MAX_SIZE = 100;

HANDLE hMutex;
HANDLE hEvent;

struct SharedData {
    double numbers[MAX_SIZE];
    int arraySize;
    int cursorSize;
    int bufferSize;
    bool terminate;
};

DWORD WINAPI ClientThread(LPVOID lpParam) {
    SharedData* data = (SharedData*)lpParam;

    while (true) {
        WaitForSingleObject(hMutex, INFINITE);

        if (data->terminate) {
            break;
        }

        cout << "Received array from Server: ";
        for (int i = 0; i < data->arraySize; i++) {
            cout << data->numbers[i] << " ";
        }
        cout << endl;

        ReleaseMutex(hMutex);

        Sleep(1000);
    }

    return 0;
}

DWORD WINAPI HigntThread(LPVOID lpParam) {
    SharedData* data = (SharedData*)lpParam;

    while (true) {
        WaitForSingleObject(hMutex, INFINITE);

        if (data->terminate) {
            break;
        }

        cout << "Received cursor size: " << data->cursorSize << endl;
        cout << "Received buffer size: " << data->bufferSize << endl;

        ReleaseMutex(hMutex);

        Sleep(1000);
    }

    return 0;
}

int main() {
    int size;
    cout << "Enter array size: ";
    cin >> size;

    SharedData data;
    data.arraySize = size;
    data.cursorSize = 0;
    data.bufferSize = 0;
    data.terminate = false;

    // Создание анонимного канала
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);

    // Запуск процесса Client
    STARTUPINFO siClient;
    PROCESS_INFORMATION piClient;
    ZeroMemory(&siClient, sizeof(STARTUPINFO));
    siClient.cb = sizeof(STARTUPINFO);
    siClient.hStdOutput = hWritePipe;
    siClient.hStdError = hWritePipe;
    siClient.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess("Client.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siClient, &piClient);

    CloseHandle(hWritePipe);

    // Запуск процесса Hignt
    STARTUPINFO siHignt;
    PROCESS_INFORMATION piHignt;
    ZeroMemory(&siHignt, sizeof(STARTUPINFO));
    siHignt.cb = sizeof(STARTUPINFO);
    siHignt.hStdOutput = hWritePipe;
    siHignt.hStdError = hWritePipe;
    siHignt.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess("Hignt.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siHignt, &piHignt);

    CloseHandle(hWritePipe);

    // Потоки для обработки данных от Client и Hignt
    HANDLE hClientThread = (HANDLE)_beginthreadex(NULL, 0, ClientThread, &data, 0, NULL);
    HANDLE hHigntThread = (HANDLE)_beginthreadex(NULL, 0, HigntThread, &data, 0, NULL);

    hMutex = CreateMutex(NULL, FALSE, NULL);
    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    char ch;

    while (ch != 'E') {
        ch = cin.get();

        switch (ch) {
        case 'E':
            WaitForSingleObject(hMutex, INFINITE);
            data.terminate = true;
            ReleaseMutex(hMutex);
            break;
        default:
            break;
        }
    }

    WaitForSingleObject(hClientThread, INFINITE);
    WaitForSingleObject(hHigntThread, INFINITE);

    CloseHandle(hMutex);
    CloseHandle(hEvent);

    CloseHandle(piClient.hProcess);
    CloseHandle(piClient.hThread);
    CloseHandle(piHignt.hProcess);
    CloseHandle(piHignt.hThread);
    CloseHandle(hReadPipe);

    return 0;
}
