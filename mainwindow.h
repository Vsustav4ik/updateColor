#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QColor>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void updateRGB();
    void updateXYZ();
    void updateHSV();
    void updateFromSliders();
    void updateColorDisplay();

private:
    QLineEdit *rgbInput;
    QLineEdit *xyzInput;
    QLineEdit *hsvInput;
    QSlider *rSlider;
    QSlider *gSlider;
    QSlider *bSlider;
    QLabel *colorDisplay;

    void setupUI();
    void rgbToXyz(double r, double g, double b, double &x, double &y, double &z);
    void xyzToRgb(double x, double y, double z, double &r, double &g, double &b);
    void rgbToHsv(double r, double g, double b, double &h, double &s, double &v);
    void hsvToRgb(double h, double s, double v, double &r, double &g, double &b);
};

#endif // MAINWINDOW_H
