#include "soundanalyze.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SoundAnalyze w;
    w.show();
    return a.exec();
}
