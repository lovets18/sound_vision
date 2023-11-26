#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#include <string>

#include <sstream>
#include <math.h>


#include "mathematical_functions.h"
#include <fstream>

#include <QFileDialog>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pix("/Users/vladimir/Desktop/University/Sound/analyze/img/logo.png");

    int w = ui->label_6->width();
    int h = ui->label_6->height();

    ui->label_6->setPixmap(pix.scaled(w, h)); //Qt::KeepAspectRatio));



    ui->pushButton->setStyleSheet({"background-color: lightblue"});

    ui->label_5->setStyleSheet({"background-color: white"});

}

MainWindow::~MainWindow()
{
    delete ui;
}





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
            myfile << amp_q[i] << " ";
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
                if (num_of_iteration == 5) {
                    break;
                }
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



void getColorPalette(double maxValue, double minValue, double value, int& red, int& green, int& blue) {
    double range = maxValue - minValue;

    //     RGB
    red = 255 * (value - minValue) / range;
    blue = 255 - red;
    green = 0;
}

double findMaxElement(std :: vector<double> vect, int size) {
    double maxElement = vect[0]; // ,
    //
    for (int i = 1; i < size; i++) {
        if (vect[i] > maxElement) {
                    maxElement = vect[i];
        }
    }

    return maxElement;
}


double findMinElement(std::vector<double> vect, int size) {
    double minElement = vect[1]; // ,

        //
        for (int i = 1; i < size; i++) {
        if (vect[i] < minElement) {
                    minElement = vect[i];
        }
    }

    return minElement;
}



void MainWindow::on_pushButton_clicked()
{
    //    QLineEdit lineEdit;
    //    QTextBrowser textBrowser;

//    QPixmap pix("/Users/vladimir/Desktop/University/Sound/analyze/center_color.png");
//    int w = 800; //ui->label_5->width(1200);
//    int h = 600; //ui->label_5->height();

//    ui->label_5->setPixmap(pix.scaled(w, h)); //Qt::KeepAspectRatio));

    QMovie movie( "/Users/vladimir/Desktop/University/Sound/analyze/img/loading.gif" );
    ui->label_5->setMovie( &movie ); // label имеет тип QLabel*
    movie.start();



    std::cout << "Copyright 2023 " << std::endl;
//    ui->textBrowser->setText(ui->lineEdit->text());
    initialization(ui->lineEdit->text().toStdString(), ui->lineEdit_2->text().toStdString());


//    cv::Mat image = imread("/Users/vladimir/Desktop/University/Sound/beamforming-main/beamforming_main/cars.png", cv::IMREAD_COLOR);

//    imshow("Rabbit", image);
//    waitKey(0);



    //
    std::ifstream file(ui->lineEdit_2->text().toStdString());
    std::string line;
    std::vector<double> first_line, second_line, third_line, fourth_line, fifth_line;
    std::vector<std::vector<double>*> arrays = { &first_line, &second_line, &third_line, &fourth_line, &fifth_line};

    if (file.is_open()) {
        int currentArrayIndex = 0;
        while (std::getline(file, line)) {
                    if (!line.empty()) {
                std::istringstream iss(line);
                double num;
                while (iss >> num) {
                    arrays[currentArrayIndex]->push_back(num);
                }
                std::cout << "!!!!!!!!" << first_line.size() << std::endl;
                    }
                    else {
                currentArrayIndex++;
                    }
        }
        file.close();

        //
        /*for (int i = 0; i < arrays.size(); i++) {
            std::cout << "Array " << i + 1 << ": ";
            for (const auto& num : *(arrays[i])) {
                std::cout << num << " ";
            }
            std::cout << std::endl;
        }*/
    }
    else {
        std::cerr << "Cant open file" << std::endl;
    }

    /*std::cout << first_line.size() << std::endl;
    std::cout << second_line.size() << std::endl;
    std::cout << third_line.size() << std::endl;
    std::cout << fourth_line.size() << std::endl;
    std::cout << fifth_line.size() << std::endl;*/

    const std::string filename = ui->lineEdit_3->text().toStdString();
    const std::string outputFilename = ui->lineEdit_4->text().toStdString();

    //
    cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        std::cerr << "Cant open file " << filename << std::endl;
//        return 1;
    }

    //
    int width = image.cols;
    int length = image.rows;


    int size = first_line.size();

    //
    int coloring_width = std::ceil(width / size);
    int coloring_length = std::ceil(length / size);

    std::cout << "color width " << coloring_width << std::endl;
    std::cout << "color length " << coloring_length << std::endl;

    double arrMax = findMaxElement(first_line,size);
    double arrMin = findMinElement(first_line, size);
    double arrMax2 = findMaxElement(second_line, size);
    double arrMin2 = findMinElement(second_line, size);
    double arrMax3 = findMaxElement(third_line, size);
    double arrMin3 = findMinElement(third_line, size);
    double arrMax4 = findMaxElement(fourth_line, size);
    double arrMin4 = findMinElement(fourth_line, size);
    double arrMax5 = findMaxElement(fifth_line, size);
    double arrMin5 = findMinElement(fifth_line, size);

    std :: vector<double> maxels = { arrMax, arrMax2, arrMax3, arrMax4, arrMax5 };
    std :: vector<double> minels = { arrMin, arrMin2, arrMin3, arrMin4, arrMin5 };

    arrMax = findMaxElement(maxels, 5);
    arrMin = findMinElement(minels, 5);

    for (int j = 0; j < 5; j++) { //
        //
        int step = 0;
        for (int i = 0; i < size; i++) { //

                    int red, green, blue;

                    if (j == 0) {
                getColorPalette(arrMax, arrMin, first_line[i], red, green, blue);
                    }
                    else if (j == 1) {
                getColorPalette(arrMax, arrMin, second_line[i], red, green, blue);
                    }
                    else if (j == 2) {
                getColorPalette(arrMax, arrMin, third_line[i], red, green, blue);
                    }
                    else if (j == 3) {
                getColorPalette(arrMax, arrMin, fourth_line[i], red, green, blue);
                    }
                    else if (j == 4) {
                getColorPalette(arrMax, arrMin, fifth_line[i], red, green, blue);
                    }

                    //   (   )
                    cv::Scalar targetColor(blue, green, red);  // BGR

                    //
                    cv::Mat coloredImage = image.clone();  //
                    cv::Rect roi(step, (image.rows / 5) * j, coloring_width, image.rows / 5);  //    (ROI) -  50

                    //    ,
                    if (i == size - 1) {
                getColorPalette(arrMax, arrMin, first_line[i], red, green, blue);
                cv::Scalar targetColor(blue, green, red);
                cv::Rect roi(1791, (image.rows / 5)* j, 128, image.rows / 5);
                std::cout << i << std::endl;
                std::cout << step << std::endl;
                std::cout << width % 200 << std::endl;
                std::cout << image.rows / 5 << std::endl;
                    }


                    coloredImage(roi) += targetColor;

                    //
                    cv::imwrite(outputFilename, coloredImage);

                    //  ,
//                    std::string mid_filename = "/Users/vladimir/Desktop/University/Sound/analyze/center_output.png";
                    image = cv::imread(outputFilename, cv::IMREAD_UNCHANGED); //nid_filename
                    step += coloring_width;
        }
    }

//    QPixmap pix("/Users/vladimir/Desktop/University/Sound/analyze/center_color.png");
    QPixmap pix(QString::fromStdString(outputFilename));

//    int w = 800; //ui->label_5->width(1200);
//    int h = 600; //ui->label_5->height();
    int w = ui->label_5->width();
    int h = ui->label_5->height();

    ui->label_5->setPixmap(pix.scaled(w, h)); //Qt::KeepAspectRatio));


//    cv::Mat resultImage = imread(ui->lineEdit_4->text().toStdString(), cv::IMREAD_COLOR);

//    imshow("Rabbit", resultImage);
    std::cout << "Image changed" << std::endl;

}


void MainWindow::on_pushButton_2_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Выбрать файл");
    ui->lineEdit->setText(str);
}


void MainWindow::on_pushButton_3_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Выбрать файл");
    ui->lineEdit_2->setText(str);
}


void MainWindow::on_pushButton_4_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Выбрать файл");
    ui->lineEdit_3->setText(str);
}


void MainWindow::on_pushButton_5_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, "Выбрать файл");
    ui->lineEdit_4->setText(str);
}

