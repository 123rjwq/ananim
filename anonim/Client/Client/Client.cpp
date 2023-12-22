#include <iostream>
#include <vector>
#include <windows.h>
#include <string>

using namespace std;

const int MAX_SIZE = 100;

int main() {
    // Генерируем вещественные числа
    vector<double> numbers;
    for (int i = 0; i < MAX_SIZE; i++) {
        double number = (double)rand() / RAND_MAX * 100.0;  // Генерация чисел от 0 до 100
        numbers.push_back(number);
    }

    // Получаем размер курсора экрана и размер буфера окна
    int cursorSize, bufferSize;
    cout << "Enter cursor size: ";
    cin >> cursorSize;
    cout << "Enter buffer size: ";
    cin >> bufferSize;

    // Создаем анонимный канал для связи с серверным процессом
    HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    // Отправляем массив процессу Server по анонимному каналу
    cout << "Press any key to send the array to Server...";
    cin.ignore();  // Игнорируем предыдущий символ, чтобы успеть нажать клавишу
    cin.get();

    for (int i = 0; i < numbers.size(); i++) {
        string numberStr = to_string(numbers[i]);
        for (char ch : numberStr) {
            WriteFile(hPipe, &ch, sizeof(char), NULL, NULL); // Передаем элементы массива посимвольно
        }
    }

    CloseHandle(hPipe);

    // Выводим сгенерированный массив на консоль
    cout << "Generated array: ";
    for (int i = 0; i < numbers.size(); i++) {
        cout << numbers[i] << " ";
    }
    cout << endl;

    return 0;
}
