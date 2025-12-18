#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDoubleValidator>
#include <QtMath>
#include <QMessageBox>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>

void MainWindow::setupPlot()
{

    QChartView *chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);

    chart = new QChart();
    chart->setTitle("Force vs. Displacement (F = kx)");
    chart->legend()->hide();

    forceSeries = new QLineSeries();
    chart->addSeries(forceSeries);

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Displacement x (m)");
    axisX->setLabelFormat("%.2f");
    chart->addAxis(axisX, Qt::AlignBottom);
    forceSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Force F (N)");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    forceSeries->attachAxis(axisY);

    chartView->setChart(chart);


    if (ui->plotWidgetContainer->layout() == nullptr) {
        ui->plotWidgetContainer->setLayout(new QVBoxLayout());
    }

    ui->plotWidgetContainer->layout()->addWidget(chartView);
}

void MainWindow::plotSpringForce(double k, double x_max)
{
    forceSeries->clear();
    int numPoints = 100;
    double step = x_max / (numPoints - 1);

    for (int i = 0; i < numPoints; ++i) {
        double x = i * step;
        double f = k * x;
        forceSeries->append(x, f);
    }

    if (x_max > 0) {
        chart->axes(Qt::Horizontal).first()->setRange(0, x_max * 1.1);
        chart->axes(Qt::Vertical).first()->setRange(0, k * x_max * 1.1);
    } else {
        // Default range if no positive displacement is entered
        chart->axes(Qt::Horizontal).first()->setRange(0, 1.0);
        chart->axes(Qt::Vertical).first()->setRange(0, 1.0);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDoubleValidator* validator = new QDoubleValidator(this);
    validator->setNotation(QDoubleValidator::StandardNotation);

    ui->lineEditForce->setValidator(validator);
    ui->lineEditDisplacement->setValidator(validator);

    ui->lineEditInitialDisplacement->setValidator(validator);
    ui->lineEditFinalDisplacement->setValidator(validator);
    ui->lineEditSpringConstantK_Integral->setValidator(validator);

    setupPlot();

    connect(ui->pushButtonCalculateK, &QPushButton::clicked,
            this, &MainWindow::calculateSpringConstant);

    connect(ui->pushButtonCalculateWork, &QPushButton::clicked,
            this, &MainWindow::calculateWorkDone);

    connect(ui->lineEditForce, &QLineEdit::textChanged, this, &MainWindow::updatePlot);
    connect(ui->lineEditDisplacement, &QLineEdit::textChanged, this, &MainWindow::updatePlot);
    connect(ui->lineEditSpringConstantK_Integral, &QLineEdit::textChanged, this, &MainWindow::updatePlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::calculateSpringConstant()
{
    bool forceOk, displacementOk;

    double force = ui->lineEditForce->text().toDouble(&forceOk);
    double displacement = ui->lineEditDisplacement->text().toDouble(&displacementOk);

    if (!forceOk || !displacementOk) {
        ui->labelResultK->setText("Error: Enter valid numbers for Force and Displacement.");
        return;
    }

    if (qAbs(displacement) < 1e-9)
    {
        ui->labelResultK->setText("Error: Displacement cannot be zero.");
    }
    else
    {
        double k = force / displacement;

        ui->labelResultK->setText(QString("Spring Constant $k$ = **%1** N/m").arg(k, 0, 'f', 4));

        ui->lineEditSpringConstantK_Integral->setText(QString::number(k, 'f', 4));

        plotSpringForce(k, qAbs(displacement));
    }
}

void MainWindow::calculateWorkDone()
{
    bool kOk, x1Ok, x2Ok;

    double k = ui->lineEditSpringConstantK_Integral->text().toDouble(&kOk);
    double x1 = ui->lineEditInitialDisplacement->text().toDouble(&x1Ok);
    double x2 = ui->lineEditFinalDisplacement->text().toDouble(&x2Ok);

    if (!kOk || !x1Ok || !x2Ok) {
        ui->labelResultWork->setText("Error: Enter valid numbers for $k$, $x_1$, and $x_2$.");
        return;
    }

    double work = 0.5 * k * (qPow(x2, 2) - qPow(x1, 2));

    ui->labelResultWork->setText(QString("Work Done $W$ = **%1** Joules").arg(work, 0, 'f', 4));

    double max_x = qMax(qAbs(x1), qAbs(x2));
    plotSpringForce(k, max_x);
}

void MainWindow::updatePlot()
{
    bool kOk;
    double k = ui->lineEditSpringConstantK_Integral->text().toDouble(&kOk);

    bool xOk;
    double x_disp = ui->lineEditDisplacement->text().toDouble(&xOk);

    if (kOk) {
        plotSpringForce(k, qAbs(x_disp));
    } else if (xOk) {
        plotSpringForce(1.0, qAbs(x_disp));
    }
}
