#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void calculateSpringConstant();
    void calculateWorkDone();
    void updatePlot();

private:
    Ui::MainWindow *ui;

    QChart *chart;
    QLineSeries *forceSeries;

    void setupPlot();
    void plotSpringForce(double k, double x_max);
};

#endif // MAINWINDOW_H
