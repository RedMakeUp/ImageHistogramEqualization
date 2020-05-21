#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QVector>

#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <qcustomplot.h>
#include "ImageDealer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::OpenFile);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::ExitApp);

    ui->imageViewer->setAlignment(Qt::AlignCenter);

//    Plot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Plot(){
    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    ui->plotWidget->addGraph();
    ui->plotWidget->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->plotWidget->xAxis->setLabel("x");
    ui->plotWidget->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->plotWidget->xAxis->setRange(-1, 1);
    ui->plotWidget->yAxis->setRange(0, 1);
    ui->plotWidget->replot();

}

void MainWindow::ShowImage(const QPixmap& pixmap){
    auto scaledPixmap = pixmap.scaled(ui->imageViewer->width(), ui->imageViewer->height(), Qt::KeepAspectRatio);
    ui->imageViewer->setPixmap(scaledPixmap);
}

void MainWindow::OpenFile(){
    // Select an image and return its name
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));

    // Check whether the name is empty
    if(fileName.isNull()){
        qDebug() << "Please Select an image file!";
        return;
    }

    // Load the image
    unsigned char* data = stbi_load(fileName.toStdString().c_str(), &m_imageWidth, &m_imageHeight, &m_imageNumChannels, 0);
    if(data){
        // Copy data into a vector
        m_originRawData.clear();
        m_originRawData.resize(static_cast<size_t>(m_imageWidth) * static_cast<size_t>(m_imageHeight) * static_cast<size_t>(m_imageNumChannels));
        for(size_t i = 0; i < m_originRawData.size(); i++) { m_originRawData[i] = data[i];}

        // Reset gray image, plot and warnning text
        m_grayRawData.clear();
        ui->labelWarn->setText("");
        ui->plotWidget->clearGraphs();
        ui->plotWidget->replot();

        // Calculate the pixmap
        CalcPixmap(m_originRawData, m_originPixmap, m_imageNumChannels);

        // Show the image
        ShowImage(m_originPixmap);
    }
    else{
        qDebug() << "Failed to load image" << fileName;
    }

    // Free memory
    stbi_image_free(data);
}

void MainWindow::ExitApp()
{
    exit(0);
}

void MainWindow::CalcPixmap(const std::vector<unsigned char>& rawData, QPixmap& pixmap, int numChannels)
{
    // Define right format
    auto format = QImage::Format_RGB32;
    if(numChannels == 4) format = QImage::Format_ARGB32;

    // We will decode the image into QImage, and then convert to QPixmap
    QImage image(m_imageWidth, m_imageHeight, format);

    // Decode the image
    if(numChannels == 3){
        for(int i = 0; i < image.width(); i++){
            for(int j = 0; j < image.height(); j++){
                int indexOfPixel = j * image.width() + i;
                int indexOfByte = indexOfPixel * numChannels;

                auto r = static_cast<qint8>(rawData[indexOfByte + 0]);
                auto g = static_cast<qint8>(rawData[indexOfByte + 1]);
                auto b = static_cast<qint8>(rawData[indexOfByte + 2]);

                uint value = 0xff000000;
                value += (r << 16);
                value += (g << 8);
                value += (b);
                image.setPixel(i, j, value);
            }
        }
    }
    else if(numChannels == 4){
        for(int i = 0; i < image.width(); i++){
            for(int j = 0; j < image.height(); j++){
                int indexOfPixel = j * image.width() + i;
                int indexOfByte = indexOfPixel * numChannels;

                auto r = static_cast<qint8>(rawData[indexOfByte + 0]);
                auto g = static_cast<qint8>(rawData[indexOfByte + 1]);
                auto b = static_cast<qint8>(rawData[indexOfByte + 2]);
                auto a = static_cast<qint8>(rawData[indexOfByte + 3]);

                uint value = 0;
                value += (a << 24);
                value += (r << 16);
                value += (g << 8);
                value += (b);
                image.setPixel(i, j, value);
            }
        }
    }else if(numChannels == 1){
        for(int i = 0; i < image.width(); i++){
            for(int j = 0; j < image.height(); j++){
                int indexOfPixel = j * image.width() + i;

                auto r = rawData[indexOfPixel];
                auto g = rawData[indexOfPixel];
                auto b = rawData[indexOfPixel];

                uint value = 0xff000000;
                value += (r << 16);
                value += (g << 8);
                value += (b);
                image.setPixel(i, j, value);
            }
        }
    }

    // Convert to QPixmap for presenting in QLabel
    pixmap = QPixmap::fromImage(image);
}

void MainWindow::ShowHistogram()
{
    QVector<double> X(256);
    for(int i = 0; i < X.size(); i++) X[i] = i;

    QVector<double> Y(256);
    double maxValue = -1.0;
    for(int i = 0; i < Y.size(); i++) {
        Y[i] = m_histogram[i];

        if(Y[i] > maxValue){
            maxValue = Y[i];
        }
    }
    for(int i = 0; i < Y.size(); i++) Y[i] /= maxValue;

    // create graph and assign data to it:
    ui->plotWidget->addGraph();
    ui->plotWidget->graph(0)->setData(X, Y);
    // give the axes some labels:
    ui->plotWidget->xAxis->setLabel("x");
    ui->plotWidget->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->plotWidget->xAxis->setRange(0, 255);
    ui->plotWidget->yAxis->setRange(0, 1.05);
    ui->plotWidget->replot();
}

void MainWindow::GrayScale()
{
    if(m_originRawData.size() == 0) {
        ui->labelWarn->setText("Please load an image first");
        return;
    }
    ui->labelWarn->setText("");

    QPixmap grayPixmap;

    ImageDealer::GrayScale(m_originRawData, m_grayRawData, m_imageWidth, m_imageHeight, m_imageNumChannels);

    ImageDealer::CalcHistogram(m_grayRawData, m_histogram);

    // Calculate the pixmap
    CalcPixmap(m_grayRawData, grayPixmap, 1);

    // Show the image
    ShowImage(grayPixmap);

    ShowHistogram();
}

void MainWindow::Equalize()
{
    if(m_grayRawData.size() == 0) {
        ui->labelWarn->setText("Please grayscale the image first");
        return;
    }
    ui->labelWarn->setText("");

    QPixmap equalizedPixmap;

    ImageDealer::Equalize(m_grayRawData, m_histogram, m_equalizedRawData, m_imageWidth, m_imageHeight);

    ImageDealer::CalcHistogram(m_equalizedRawData, m_histogram);

    // Calculate the pixmap
    CalcPixmap(m_equalizedRawData, equalizedPixmap, 1);

    // Show the image
    ShowImage(equalizedPixmap);

    ShowHistogram();
}

void MainWindow::ShowOriginImage()
{
    if(m_originPixmap.isNull()) {
        ui->labelWarn->setText("Please load an image first");
        return;
    }
    ui->labelWarn->setText("");

    ShowImage(m_originPixmap);

    ui->plotWidget->clearGraphs();
    ui->plotWidget->replot();
}

