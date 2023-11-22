#pragma once


#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>
#include <fstream>

//#include <opencv2/opencv.hpp>

const double c = 340;                 // скорость звука
const double dl = 13.6e-2;             // расстояние между микрофонами
const int Fs = 20000;                  // частота дискретизации
const double T = 1.0 / Fs;            // период дискретизации
const double del_cos = T * c / dl;    // шаг по косинусу
const double K = floor(1.0 / del_cos);  // количество отступов угла от 90 градусов
const int num_of_seconds = 3;       // количество секунд, в течении которых проводится расчет
const int M = 950;  // Количество строк
 int X = 5;  // Количество столбцов
const double dur = 0.02;           // длительность 20 мс

/*
Функция вычисляет тренд стобца матрицы
На вход поступает вектор и К, определяющая количество соседних элементов, с которыми вычисляется тренд
*/
std::vector<double> myTrend(const std::vector<double>& x, int k) {
    std::vector<double> trend(x.size(), 0.0);

    for (int i = 1 + k; i < x.size() - k; i++) {
        double sum = 0.0;
        for (int j = i - k; j <= i + k; j++) {
            sum += x[j];
        }
        trend[i] = sum / (2 * k + 1);
    }

    double mean = 0.0;
    for (int i = 1; i < x.size(); i++) {
        mean += x[i];
    }
    mean /= x.size();
    for (int i = 1; i < x.size(); i++) {
        trend[i] = mean;
    }

    mean = 0.0;
    for (int i = x.size() - k; i < x.size(); i++) {
        mean += x[i];
    }
    mean /= k;
    for (int i = x.size() - k; i < x.size(); i++) {
        trend[i] = mean;
    }

    return trend;
}

/*
Функция совершает детренд для введенного массива
*/

std::vector<double> myDetrendMean(const std::vector<double>& x, int k) {
    std::vector<double> trend = myTrend(x, k);
    std::vector<double> detrended(x.size());

    for (int i = 0; i < x.size(); i++) {
        detrended[i] = x[i] - trend[i];
    }

    return detrended;
}


/*
Функция сдвигает массив x на n позиций вправо или влево.
Если n положительное число, элементы массива сдвигаются вправо, а новые элементы устанавливаются в ноль.
Если n отрицательное число, элементы массива сдвигаются влево, а новые элементы устанавливаются в ноль.
Функция возвращает измененный массив x.
*/

std::vector<double> moveXByN(std::vector<double> x, int n) {
    int size = x.size();
    std::vector<double> result(size);

    if (n > 0) {
        for (int i = n; i < size; i++) {
            result[i] = x[i - n];
        }
        for (int i = 0; i < (std::min)(n, size); i++) {
            result[i] = 0;
        }
    }
    else if (n < 0) {
        for (int i = 0; i < size + n; i++) {
            result[i] = x[i - n];
        }
        for (int i = size + n; i < size; i++) {
            if (i >= 0) {
                result[i] = 0;
            }
        }
    }
    else {
        result = x;
    }

    return result;
}

/*
 Функция вычисляет амплитуду для заданного массива данных data и количества отступов угла K.

Создается вспомогательная матрица mat размером 8x8, заполненная нулями.
В цикле for вычисляются сдвиги исходных данных data и вычисляется матрица mat путем вычисления среднего произведения сдвинутых столбцов данных.
Вычисляется амплитуда, которая является суммой всех элементов матрицы mat.
Возвращается вектор амплитуды amp.
*/

std::vector<double> determineAmplitude(const std::vector<std::vector<double>>& data, int K) {
    int numK = 2 * K + 1;
    std::vector<double> amp(numK, 0.0);

    for (int i = 1; i < numK; i++) {
        std::vector<std::vector<double>> Y = data;
        for (int j = 1; j < 8; j++) {
            Y[j] = moveXByN(Y[j], (K + 1 - i) * (j - 5));
        }

        std::vector<std::vector<double>> mat(8, std::vector<double>(8, 0.0));
        for (int ii = 1; ii < 8; ii++) {
            for (int jj = ii; jj < 8; jj++) {
                double sum = 0.0;
                for (int k = 0; k < Y[ii].size(); k++) {
                    sum += Y[ii][k] * Y[jj][k];
                }
                mat[ii][jj] = sum / Y[ii].size();
            }
        }

        double totalSum = 0.0;
        for (int ii = 1; ii < 8; ii++) {
            for (int jj = ii; jj < 8; jj++) {
                totalSum += mat[ii][jj];
            }
        }
        amp[i] = totalSum;
    }

    return amp;
}

/*
Данная функция выполняет интерполяцию значений функции на новой сетке точек.
Она принимает на вход три вектора: x, y и x_interp.
Вектор x содержит исходные значения аргументов функции,
вектор y содержит соответствующие значения функции, а вектор x_interp содержит новую сетку точек, на которой требуется произвести интерполяцию.

Функция проходит по каждой точке вектора x_interp и выполняет интерполяцию соответствующего значения функции.
Для каждой точки x_val из x_interp, она ищет ближайшие значения x_prev и x_next в векторе x, а также соответствующие значения функции y_prev и y_next.
Затем она выполняет линейную интерполяцию.
*/

std::vector<double> interpolate(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& x_interp) {
    std::vector<double> y_interp(x_interp.size());

    for (size_t i = 0; i < x_interp.size(); i++) {
        double x_val = x_interp[i];
        size_t j = 0;

        while (j < x.size() && x_val > x[j]) {
            j++;
        }

        if (j == 0) {
            y_interp[i] = y[0];
        }
        else if (j == x.size()) {
            y_interp[i] = y[x.size() - 1];
        }
        else {
            double x_prev = x[j - 1];
            double x_next = x[j];
            double y_prev = y[j - 1];
            double y_next = y[j];

            y_interp[i] = y_prev + (y_next - y_prev) * (x_val - x_prev) / (x_next - x_prev);
        }
    }

    return y_interp;
}