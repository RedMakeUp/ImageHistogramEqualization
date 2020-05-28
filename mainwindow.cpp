#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QVector>

#include <vector>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <qcustomplot.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_funcPane(std::make_unique<QWidget>())
{
    ui->setupUi(this);

//    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::OpenFile);
//    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::ExitApp);

//    ui->imageViewer->setAlignment(Qt::AlignCenter);

//    Plot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitLayout(){

    // Center Splitter
    {
        float pos = 0.59f;
        int leftWidth = static_cast<int>(pos * this->width());
        int rightWidth = static_cast<int>((1 - pos) * this->width());
        ui->splitter->setSizes(QList<int>() << leftWidth << rightWidth);
        ui->splitter->setGeometry(QRect(0, 0, this->width(), this->height()));
    }

    // Left Splitter
    {
        float pos = 0.5;
        int topHeight = static_cast<int>(pos * ui->leftPane->height());
        int bottomHeight = static_cast<int>((1 - pos) * ui->leftPane->height());
        ui->splitter_2->setSizes(QList<int>() << topHeight << bottomHeight);
        ui->splitter_2->setGeometry(QRect(0, 0, ui->leftPane->width(), ui->leftPane->height()));
    }

    // Right Splitter
    {
        float pos = 0.4;
        int topHeight = static_cast<int>(pos * ui->rightPane->height());
        int bottomHeight = static_cast<int>((1 - pos) * ui->rightPane->height());
        ui->splitter_3->setSizes(QList<int>() << topHeight << bottomHeight);
        ui->splitter_3->setGeometry(QRect(0, 0, ui->rightPane->width(), ui->rightPane->height()));
    }

    // Right-bottom function pane
    {
        m_funcPane->setMinimumSize(m_funcPaneMinWidth, m_funcPaneMinHeight);

        ui->widget_5->setParent(m_funcPane.get());
        ui->widget_5->move(ui->widget_5->pos().x(), 20);

        ui->scrollArea->setWidget(m_funcPane.get());
        ui->scrollArea->setGeometry(QRect(0, 0, ui->widget_4->width(), ui->widget_4->height()));
        m_funcPane->setGeometry(QRect(0, 0, m_funcPaneMinWidth, m_funcPaneMinHeight));
    }

    // Center image viewers
    CenterImageViewer();
}

void MainWindow::CenterImageViewer()
{
    ui->imageViewer_origin->setGeometry(0, 0, ui->widget_2->width() * 0.95, ui->widget_2->height() * 0.95);
    ui->imageViewer_gray->setGeometry(0, 0, ui->widget_3->width() * 0.95, ui->widget_3->height() * 0.95);

    QPoint center1 = QPoint(ui->widget_2->width()/2, ui->widget_2->height()/2);
    center1 -= QPoint(ui->imageViewer_origin->width()/2, ui->imageViewer_origin->height()/2);
    ui->imageViewer_origin->move(center1);

    QPoint center2 = QPoint(ui->widget_3->width()/2, ui->widget_3->height()/2);
    center2 -= QPoint(ui->imageViewer_gray->width()/2, ui->imageViewer_gray->height()/2);
    ui->imageViewer_gray->move(center2);
}

QPixmap MainWindow::CalcPixmap(const std::shared_ptr<RawImage>& rawImage)
{
    auto numChannels = rawImage->numChannels;

    // Define right format
    auto format = QImage::Format_RGB32;
    if(numChannels == 4) format = QImage::Format_ARGB32;

    // We will decode the image into QImage, and then convert to QPixmap
    QImage image(rawImage->width, rawImage->height, format);

    // Decode the image
    if(numChannels == 3){
        for(int i = 0; i < image.width(); i++){
            for(int j = 0; j < image.height(); j++){
                int indexOfPixel = j * image.width() + i;
                int indexOfByte = indexOfPixel * 3;

                QRgb rgb = qRgb(rawImage->data[indexOfByte + 0],
                                rawImage->data[indexOfByte + 1],
                                rawImage->data[indexOfByte + 2]);

                image.setPixel(i, j, rgb);
            }
        }
    }
    else if(numChannels == 4){
        for(int i = 0; i < image.width(); i++){
            for(int j = 0; j < image.height(); j++){
                int indexOfPixel = j * image.width() + i;
                int indexOfByte = indexOfPixel * 4;

                QRgb rgba = qRgba(rawImage->data[indexOfByte + 0],
                                  rawImage->data[indexOfByte + 1],
                                  rawImage->data[indexOfByte + 2],
                                  rawImage->data[indexOfByte + 3]);
                image.setPixel(i, j, rgba);
            }
        }
    }else if(numChannels == 1){
        for(int i = 0; i < image.width(); i++){
            for(int j = 0; j < image.height(); j++){
                int indexOfPixel = j * image.width() + i;

                auto gray = rawImage->data[indexOfPixel];
                QRgb rgb = qRgb(gray, gray, gray);
                image.setPixel(i, j, rgb);
            }
        }
    }

    // Convert to QPixmap for presenting in QLabel
    return QPixmap::fromImage(image).copy();
}

void MainWindow::ShowOriginImage(const QPixmap& pixmap, bool bestQuality){
    QPixmap scaledPixmap;
    if(bestQuality){
        scaledPixmap = pixmap.scaled(ui->imageViewer_origin->width(), ui->imageViewer_origin->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }else{
        scaledPixmap = pixmap.scaled(ui->imageViewer_origin->width(), ui->imageViewer_origin->height(), Qt::KeepAspectRatio);
    }
    ui->imageViewer_origin->setPixmap(scaledPixmap);
}

void MainWindow::ShowProcessedImage(const QPixmap &pixmap, bool bestQuality)
{
    QPixmap scaledPixmap;
    if(bestQuality){
        scaledPixmap = pixmap.scaled(ui->imageViewer_gray->width(), ui->imageViewer_gray->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }else{
        scaledPixmap = pixmap.scaled(ui->imageViewer_gray->width(), ui->imageViewer_gray->height(), Qt::KeepAspectRatio);
    }
    ui->imageViewer_gray->setPixmap(scaledPixmap);
}

void MainWindow::PlotHistogram(const std::vector<int>& histogram)
{
    QVector<double> X(histogram.size()), Y(histogram.size());

    double maxValue = *std::max_element(histogram.begin(), histogram.end());
    for(size_t i = 0; i < histogram.size(); i++){
        X[i] = i;
        Y[i] = histogram[i] * 1.0 / maxValue;
    }

    // create graph and assign data to it:
    ui->plotWidget->addGraph();
    ui->plotWidget->graph(0)->setData(X, Y);
    // give the axes some labels:
    ui->plotWidget->xAxis->setLabel("Gray Level");
    ui->plotWidget->yAxis->setLabel("Occurrence");
    // set axes ranges, so we see all data:
    ui->plotWidget->xAxis->setRange(0, 256);
    ui->plotWidget->yAxis->setRange(0, 1);
    ui->plotWidget->replot();
}

void MainWindow::Plot(const std::shared_ptr<RawImage> image)
{
    std::vector<int> histogram;
    ImageDealer::CalcHistogram(image->data, histogram);
    PlotHistogram(histogram);
}




void MainWindow::on_splitter_splitterMoved(int, int)
{
    ui->splitter_2->setGeometry(QRect(0, 0, ui->leftPane->width(), ui->leftPane->height()));
    ui->splitter_3->setGeometry(QRect(0, 0, ui->rightPane->width(), ui->rightPane->height()));
    ui->scrollArea->setGeometry(QRect(0, 0, ui->widget_4->width(), ui->widget_4->height()));

    CenterImageViewer();
}

void MainWindow::on_splitter_2_splitterMoved(int, int)
{
    CenterImageViewer();

    if(!m_originPixmap.isNull()) ShowOriginImage(m_originPixmap);
    if(!m_processedPixmap.isNull()) ShowProcessedImage(m_processedPixmap);
}

void MainWindow::on_splitter_3_splitterMoved(int, int)
{
    ui->scrollArea->setGeometry(QRect(0, 0, ui->widget_4->width(), ui->widget_4->height()));
}

void MainWindow::on_pushButton_clicked()
{
    // Select an image and return its name
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));

    // Check whether the name is empty
    if(fileName.isNull()){
        qDebug() << "Please Select an image file!";
        return;
    }

    // Show the name in UI
    ui->lineEdit_imageName->setText(fileName);

    // Load the image
    m_originRawImage = ImageDealer::LoadRawImage(fileName.toStdString().c_str());
    if(m_originRawImage){
        // Show origin image
        m_originPixmap = CalcPixmap(m_originRawImage);
        ShowOriginImage(m_originPixmap);

        // And show its gray image
        m_grayRawImage = ImageDealer::GrayScale(m_originRawImage);
        m_processedRawImage = m_grayRawImage;
        m_processedPixmap = CalcPixmap(m_processedRawImage);
        ShowProcessedImage(m_processedPixmap);

        // Plot gray histogram
        Plot(m_grayRawImage);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(!m_grayRawImage) return;

    ui->pushButton_3->setDisabled(true);

    // Mirror vertically
    m_processedRawImage = ImageDealer::Mirror(m_processedRawImage);
    m_processedPixmap = CalcPixmap(m_processedRawImage);
    ShowProcessedImage(m_processedPixmap);

    // Plot gray histogram
    Plot(m_processedRawImage);

    ui->pushButton_3->setDisabled(false);
}

void MainWindow::on_pushButton_4_clicked()
{
    if(!m_grayRawImage) return;

    ui->pushButton_4->setDisabled(true);

    // Mirror horizontally
    m_processedRawImage = ImageDealer::Mirror(m_processedRawImage, false);
    m_processedPixmap = CalcPixmap(m_processedRawImage);
    ShowProcessedImage(m_processedPixmap);

    // Plot gray histogram
    Plot(m_processedRawImage);

    ui->pushButton_4->setDisabled(false);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(!m_originRawImage) return;

    ui->pushButton_5->setDisabled(true);

    // If origin image exists, gray image must exist
    m_processedRawImage = m_grayRawImage;
    m_processedPixmap = CalcPixmap(m_processedRawImage);
    ShowProcessedImage(m_processedPixmap);

    // Plot gray histogram
    Plot(m_grayRawImage);

    ui->pushButton_5->setDisabled(false);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(!m_grayRawImage) return;

    ui->pushButton_6->setDisabled(true);

    m_processedRawImage = ImageDealer::Equalize(m_processedRawImage);
    m_processedPixmap = CalcPixmap(m_processedRawImage);
    ShowProcessedImage(m_processedPixmap);

    // Plot gray histogram
    Plot(m_processedRawImage);

    ui->pushButton_6->setDisabled(false);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(!m_grayRawImage) return;

    ui->pushButton_2->setDisabled(true);

    if(ui->checkBox_4->isChecked()){
        static const std::regex rule(R"(\{[ *-?\d+.?d+ *,? *]+\})");

        auto str = ui->lineEdit_filter->text().toStdString();
        std::vector<QString> rows;
        std::vector<float> filter;

        std::sregex_iterator iter(str.begin(), str.end(), rule);
        std::sregex_iterator end;

        while (iter != end)
        {
            QString row = QString::fromStdString((*iter)[0].str());
            rows.push_back(row.mid(1, row.size() - 2));
            ++iter;
        }

        for(const auto& row: rows){
            for(const auto& numStr: row.split(',', Qt::SkipEmptyParts)){
                filter.push_back(numStr.trimmed().toFloat());
            }
        }

        if(rows.size() > 0){
            m_processedRawImage = ImageDealer::WeightedFilter(m_processedRawImage, filter, rows.size(), ui->checkBox->isChecked());

            if(m_processedRawImage){
                m_processedPixmap = CalcPixmap(m_processedRawImage);
                ShowProcessedImage(m_processedPixmap);

                // Plot gray histogram
                Plot(m_processedRawImage);
            }
        }
    }
    else{
        auto filterShape = ui->lineEdit_filter->text().toInt();

        if(ui->checkBox_2->isChecked()){
            if(filterShape){
                if((m_processedRawImage = ImageDealer::MedianFilter(m_processedRawImage, filterShape)) != nullptr){
                    m_processedPixmap = CalcPixmap(m_processedRawImage);
                    ShowProcessedImage(m_processedPixmap);

                    // Plot gray histogram
                    Plot(m_processedRawImage);
                }
            }
        }else if(ui->checkBox_3->isChecked()){
            if(filterShape){
                if((m_processedRawImage = ImageDealer::MaxFilter(m_processedRawImage, filterShape)) != nullptr){
                    m_processedPixmap = CalcPixmap(m_processedRawImage);
                    ShowProcessedImage(m_processedPixmap);

                    // Plot gray histogram
                    Plot(m_processedRawImage);
                }
            }
        }
    }

    ui->pushButton_2->setDisabled(false);
}

void MainWindow::on_checkBox_2_stateChanged(int)
{
    if(ui->checkBox_2->isChecked()){
        ui->checkBox_3->setChecked(false);
    }else{
        ui->checkBox_3->setChecked(true);
    }
}

void MainWindow::on_checkBox_3_stateChanged(int)
{
    if(ui->checkBox_3->isChecked()){
        ui->checkBox_2->setChecked(false);
    }else{
        ui->checkBox_2->setChecked(true);
    }
}

void MainWindow::on_checkBox_4_stateChanged(int)
{
    if(ui->checkBox_4->isChecked()){
        ui->checkBox_3->setDisabled(true);
        ui->checkBox_2->setDisabled(true);
        ui->checkBox->setDisabled(false);
    }else{
        ui->checkBox_3->setDisabled(false);
        ui->checkBox_2->setDisabled(false);
        ui->checkBox->setDisabled(true);
    }
}
