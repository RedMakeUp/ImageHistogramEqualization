#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

#include <vector>

#include <bits/ios_base.h>

#include <memory>

#include "ImageDealer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitLayout();

private:
    void CenterImageViewer();
    void ShowOriginImage(const QPixmap& pixmap, bool bestQuality = true);
    void ShowProcessedImage(const QPixmap& pixmap, bool bestQuality = false);
    void PlotHistogram(const std::vector<int>& histogram);
    void Plot(const std::shared_ptr<RawImage> image);
    QPixmap CalcPixmap(const std::shared_ptr<RawImage>& rawImage);


private slots:
    void on_splitter_splitterMoved(int pos, int index);

    void on_splitter_2_splitterMoved(int pos, int index);

    void on_splitter_3_splitterMoved(int pos, int index);

    // Button "load" clicked
    void on_pushButton_clicked();

    // Button "Mirror-X" clicked
    void on_pushButton_3_clicked();

    // Button "Mirror-Y" clicked
    void on_pushButton_4_clicked();

    // Button "Gray Scale" clicked
    void on_pushButton_5_clicked();

    // Button "Equalize" clicked
    void on_pushButton_6_clicked();

    // Button "Filter" clicked
    void on_pushButton_2_clicked();

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_checkBox_4_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    QPixmap m_originPixmap;
    QPixmap m_processedPixmap;
    std::shared_ptr<RawImage> m_originRawImage = nullptr;
    std::shared_ptr<RawImage> m_grayRawImage = nullptr;
    std::shared_ptr<RawImage> m_processedRawImage = nullptr;

    std::vector<unsigned char> m_grayRawData;
    std::vector<unsigned char> m_equalizedRawData;
    std::vector<int> m_histogram;

    std::unique_ptr<QWidget> m_funcPane;


    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageNumChannels = 0;
    int m_funcPaneMinWidth = 500;
    int m_funcPaneMinHeight = 500;
};
#endif // MAINWINDOW_H
