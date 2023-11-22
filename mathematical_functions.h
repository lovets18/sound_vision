#pragma once


#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>
#include <fstream>

//#include <opencv2/opencv.hpp>

const double c = 340;                 // �������� �����
const double dl = 13.6e-2;             // ���������� ����� �����������
const int Fs = 20000;                  // ������� �������������
const double T = 1.0 / Fs;            // ������ �������������
const double del_cos = T * c / dl;    // ��� �� ��������
const double K = floor(1.0 / del_cos);  // ���������� �������� ���� �� 90 ��������
const int num_of_seconds = 3;       // ���������� ������, � ������� ������� ���������� ������
const int M = 950;  // ���������� �����
 int X = 5;  // ���������� ��������
const double dur = 0.02;           // ������������ 20 ��

/*
������� ��������� ����� ������ �������
�� ���� ��������� ������ � �, ������������ ���������� �������� ���������, � �������� ����������� �����
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
������� ��������� ������� ��� ���������� �������
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
������� �������� ������ x �� n ������� ������ ��� �����.
���� n ������������� �����, �������� ������� ���������� ������, � ����� �������� ��������������� � ����.
���� n ������������� �����, �������� ������� ���������� �����, � ����� �������� ��������������� � ����.
������� ���������� ���������� ������ x.
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
 ������� ��������� ��������� ��� ��������� ������� ������ data � ���������� �������� ���� K.

��������� ��������������� ������� mat �������� 8x8, ����������� ������.
� ����� for ����������� ������ �������� ������ data � ����������� ������� mat ����� ���������� �������� ������������ ��������� �������� ������.
����������� ���������, ������� �������� ������ ���� ��������� ������� mat.
������������ ������ ��������� amp.
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
������ ������� ��������� ������������ �������� ������� �� ����� ����� �����.
��� ��������� �� ���� ��� �������: x, y � x_interp.
������ x �������� �������� �������� ���������� �������,
������ y �������� ��������������� �������� �������, � ������ x_interp �������� ����� ����� �����, �� ������� ��������� ���������� ������������.

������� �������� �� ������ ����� ������� x_interp � ��������� ������������ ���������������� �������� �������.
��� ������ ����� x_val �� x_interp, ��� ���� ��������� �������� x_prev � x_next � ������� x, � ����� ��������������� �������� ������� y_prev � y_next.
����� ��� ��������� �������� ������������.
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