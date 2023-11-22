#ifndef SOUNDANALYZE_H
#define SOUNDANALYZE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SoundAnalyze; }
QT_END_NAMESPACE

class SoundAnalyze : public QMainWindow
{
    Q_OBJECT

public:
    SoundAnalyze(QWidget *parent = nullptr);
    ~SoundAnalyze();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SoundAnalyze *ui;
};
#endif // SOUNDANALYZE_H
