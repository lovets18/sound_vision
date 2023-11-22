#include "soundanalyze.h"
#include "./ui_soundanalyze.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include <string>

// beamforming_main.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//#include "gnuplot-iostream.h"
#include "mathematical_functions.h"
#include <fstream>


using namespace std;

vector<double> linspace(double start, double stop, double num) {    // генерирует вектор значений между заданным начальным и конечным значениями с заданным количеством элементов
    vector<double> result(num);
    double step = static_cast<double>(stop - start) / (num - 1);
    for (int i = 0; i < num; i++) {
        result[i] = start + i * step;
    }
    return result;
}

/*
На вход поступает текстовый файл с данными, полученными от микрофонов.
Функция создает матрицу, элементы которой равны значением из файла.
Значения начинаются со строки под номер start_line, чтобы при каждой итерации алгоритма брать значения из определенного диапозона
*/

std::vector<std::vector<double>> createMatrixFromFile(const std::string& file_path, int start_line, int start_column) {
    std::ifstream file(file_path);
    std::vector<std::vector<double>> matrix(M, std::vector<double>(X, 0.0));

    if (file.is_open()) {
        // Пропуск строк до указанной start_line
        for (int i = 1; i < start_line; i++) {
            file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }

        // Считывание значений из файла
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < start_column; j++) {
                file.ignore((std::numeric_limits<std::streamsize>::max)(), '\t'); // Пропуск значения
            }
            for (int j = 0; j < X; j++) {
                if (!(file >> matrix[i][j])) {
                    std::cerr << "Error reading file" << std::endl;
                    file.close();
                    return matrix;
                }
            }
            file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');  // Пропуск символа новой строки
        }
        file.close();
    }
    else {
        std::cerr << "Не удалось открыть файл." << std::endl;
    }

    return matrix;
}


void main_algorithm (vector<double> phi, vector<double> phi_q, std::string source_file, std::string result_file) {
    //    Gnuplot gp("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\"");
    int start_line = 1;
    int start_column = 0;
    int num_of_iteration = 0;
    std::string file_path = source_file;
    ofstream myfile;
    myfile.open (result_file);
//    /Users/vladimir/Desktop/University/Sound/sound5/chair_center.txt
    for (double start_time = 0; start_time < num_of_seconds - dur; start_time += 5 * dur) {
        std::cout << start_time << std::endl;
        double end_t = start_time + dur;
        int strt_p = std::round(start_time * Fs) + 1;
        int end_p = std::round(end_t * Fs);
        std::cout << "phases start - end" << std::endl;
        std::cout << strt_p << std::endl;
        std::cout << end_p << std::endl;


        std::vector<std::vector<double>> part_of_data = createMatrixFromFile(file_path, start_line, start_column);
        // Вывод матрицы
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < X; j++) {
                std::cout << part_of_data[i][j] << '\t';
            }
            std::cout << std::endl;
        }
        std::cout << "matrix size:" << part_of_data.size();
        //детрендирование
        for (int i = 0; i < 5; i++) {
            std::vector<double> column(part_of_data.size());
            for (int j = 0; j < part_of_data.size(); j++) {
                column[j] = part_of_data[j][i];
            }
            std::vector<double> detrended_column = myDetrendMean(column, 10);
            for (int j = 0; j < part_of_data.size(); j++) {
                part_of_data[j][i] = detrended_column[j];
            }
        }
        /*
        for (int i = 0; i < part_of_data.size(); i++) {
            for (int j = 0; j < 24; j++) {
                std::cout << part_of_data[i][j] << " ";
            }
            std::cout << std::endl;
        }
        */
        // амплитуда
        std::vector<double> amp = determineAmplitude(part_of_data, K);

        // Вывод результатов
        //for (int i = 0; i < amp.size(); i++) {
        //    std::cout << amp[i] << " ";
        //}
        //std::cout << std::endl;

        // интерполяция
        std::vector<double> amp_q = interpolate(phi, amp, phi_q);



//        myfile << "Writing this to a file.\n";

        // Вывод результатов
//        myfile << "AMP_Q size:" << amp_q.size() << endl;



//        myfile << "AMP_Q:" << endl;
        for (int i = 0; i < amp_q.size(); i++) {
            myfile << amp_q[i] << ", ";
        }
        myfile << std::endl;
        myfile << std::endl;


        // Установка параметров полярных координат
        //        gp << "set polar\n";
        //        gp << "set size square\n";  // Чтобы круг выглядел круглым


        // Передача данных в Gnuplot
        //        gp << "plot '-' with lines\n";
        //        gp.send1d(boost::make_tuple(phi_q, amp_q));

//        std::string filename = "C:/Users/admin/Desktop/Desktop/Diploma/graphs/iteration_" + std::to_string(num_of_iteration);  // Путь и имя файла для сохранения

        //        gp << "set term png\n";  // Установка типа терминала как PNG
        //        gp << "set output '" << filename << "'\n";  // Указание пути и имени файла для сохранения
        //        gp << "plot '-' with lines\n";
        //        gp.send1d(boost::make_tuple(phi_q, amp_q));

//        myfile << "NUM OF ITER IS " << num_of_iteration<< endl;


        // Ожидание нажатия клавиши перед закрытием графика
        std::cout << "Press enter to exit..." << std::endl;

        //std::cin.get();
        start_column += 5;
        num_of_iteration++;
//        if (num_of_iteration == 5) {
//            break;
//        }
    }
    myfile.close();
}


//функция выводи значения переменных

void print_variables(vector<double> cosines, vector<double> phi, vector<double> amp, vector<double> phi_q, vector<double> amp_q) {
    std::cout << "c: " << c << std::endl;
    std::cout << "dl: " << dl << std::endl;
    std::cout << "Fs: " << Fs << std::endl;
    std::cout << "T: " << T << std::endl;
    std::cout << "del_cos: " << del_cos << std::endl;
    std::cout << "K: " << K << std::endl;

    std::cout << "cosines: ";
    for (double cosine : cosines) {
        std::cout << cosine << " ";
    }
    std::cout << std::endl;

    std::cout << "phi: ";
    for (double angle : phi) {
        std::cout << angle << " ";
    }
    std::cout << std::endl;

    std::cout << "amp: ";
    for (double amplitude : amp) {
        std::cout << amplitude << " ";
    }
    std::cout << std::endl;

    std::cout << "phi_q: ";
    for (double angle : phi_q) {
        std::cout << angle << " ";
    }
    std::cout << std::endl;

    std::cout << "amp_q: ";
    for (double amplitude : amp_q) {
        std::cout << amplitude << " ";
    }
    std::cout << std::endl;
}

void initialization(std::string source_file, std::string result_file) {
    vector<double> cosines(K * 2 + 1); // вектор косинусов от 1 до -1
    for (int i = K; i >= -K; i--) {
        cosines[K - i] = i * del_cos;
    }

    vector<double> phi(K * 2 + 1); // вектор углов в радианах от 0 до пи
    for (int i = 0; i < phi.size(); i++) {
        phi[i] = std::acos(cosines[i]);
    }

    const double delta_phi = M_PI / 2;
    std::vector<double> phi_q = linspace(M_PI / 2 - delta_phi, M_PI / 2 + delta_phi, 200); // вектор углов для интерполяции

    main_algorithm(phi, phi_q, source_file, result_file);
}




SoundAnalyze::SoundAnalyze(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SoundAnalyze)
{
    ui->setupUi(this);
}

SoundAnalyze::~SoundAnalyze()
{
    delete ui;
}


void SoundAnalyze::on_pushButton_clicked()
{
//    QLineEdit lineEdit;
//    QTextBrowser textBrowser;
    std::cout << "Copyright 2023 " << std::endl;
    ui->textBrowser->setText(ui->lineEdit->text());
    initialization(ui->lineEdit->text().toStdString(), ui->lineEdit_2->text().toStdString());
}

