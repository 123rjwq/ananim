#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

using namespace std;

const int MAX_SIZE = 100;

int main() {
    vector<double> numbers;
    double number;
    char ch;

    // Получаем массив чисел по анонимному каналу от процесса Server, посредством нажатия правой клавиши мыши
    HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    cout << "Press right mouse button to receive the array from Server...";
    do {
        ReadFile(hPipe, &ch, sizeof(char), NULL, NULL);

        if (ch != ' ') {
            number = atof(&ch);
            ReadFile(hPipe, &ch, sizeof(char), NULL, NULL);
            while (ch != ' ') {
                number = number * 10 + atof(&ch);
                ReadFile(hPipe, &ch, sizeof(char), NULL, NULL);
            }

            numbers.push_back(number);
        }
    } while (ch != ' ');

    CloseHandle(hPipe);

    // Запрашиваем вещественное число N
    double N;
    cout << "Enter a real number N: ";
    cin >> N;

    // Передаем числа > N по анонимному каналу процессу Server
    hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    for (int i = 0; i < numbers.size(); i++) {
        if (numbers[i] > N) {
            string numberStr = to_string(numbers[i]);
            for (char ch : numberStr) {
                WriteFile(hPipe, &ch, sizeof(char), NULL, NULL);
            }
            WriteFile(hPipe, " ", sizeof(char), NULL, NULL);
        }
    }
    WriteFile(hPipe, " ", sizeof(char), NULL, NULL); // Отправляем символ-разделитель, чтобы Server знал, что массив окончен

    CloseHandle(hPipe);

    // Выводим полученные числа на консоль
    cout << "Received numbers: ";
    for (int i = 0; i < numbers.size(); i++) {
        cout << numbers[i] << " ";
    }
    cout << endl;

    return 0;
}
