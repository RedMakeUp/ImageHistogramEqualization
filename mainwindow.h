#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void Plot();
    void ShowImage(const QPixmap& pixmap);
    void OpenFile();
    void ExitApp();
    void CalcPixmap(const std::vector<unsigned char>& rawData, QPixmap& pixmap, int numChannels = 3);
    void ShowHistogram();

private slots:
    void GrayScale();
    void Equalize();
    void ShowOriginImage();

private:
    Ui::MainWindow *ui;

    QPixmap m_originPixmap;
    std::vector<unsigned char> m_originRawData;

    std::vector<unsigned char> m_grayRawData;
    std::vector<unsigned char> m_equalizedRawData;
    std::vector<int> m_histogram;

    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageNumChannels = 0;
};
#endif // MAINWINDOW_H
