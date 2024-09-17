#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    rgbInput = new QLineEdit(this);
    rgbInput->setPlaceholderText("RGB Input (e.g., 255,255,255)");
    connect(rgbInput, &QLineEdit::editingFinished, this, &MainWindow::updateRGB);

    xyzInput = new QLineEdit(this);
    xyzInput->setPlaceholderText("XYZ Input (e.g., 95.047,100.000,108.883)");
    connect(xyzInput, &QLineEdit::editingFinished, this, &MainWindow::updateXYZ);

    hsvInput = new QLineEdit(this);
    hsvInput->setPlaceholderText("HSV Input (e.g., 360,100,100)");
    connect(hsvInput, &QLineEdit::editingFinished, this, &MainWindow::updateHSV);

    rSlider = new QSlider(Qt::Horizontal, this);
    rSlider->setMaximum(255);
    connect(rSlider, &QSlider::valueChanged, this, &MainWindow::updateFromSliders);

    gSlider = new QSlider(Qt::Horizontal, this);
    gSlider->setMaximum(255);
    connect(gSlider, &QSlider::valueChanged, this, &MainWindow::updateFromSliders);

    bSlider = new QSlider(Qt::Horizontal, this);
    bSlider->setMaximum(255);
    connect(bSlider, &QSlider::valueChanged, this, &MainWindow::updateFromSliders);

    colorDisplay = new QLabel(this);
    colorDisplay->setFixedSize(100, 100);
    colorDisplay->setAutoFillBackground(true);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel("Color:"));
    colorLayout->addWidget(colorDisplay);

    layout->addWidget(new QLabel("RGB:"));
    layout->addWidget(rgbInput);
    layout->addWidget(rSlider);
    layout->addWidget(gSlider);
    layout->addWidget(bSlider);
    layout->addLayout(colorLayout);
    layout->addWidget(new QLabel("XYZ:"));
    layout->addWidget(xyzInput);
    layout->addWidget(new QLabel("HSV:"));
    layout->addWidget(hsvInput);

    setCentralWidget(centralWidget);
}

void MainWindow::updateRGB() {
    QStringList rgbValues = rgbInput->text().split(',');
    if (rgbValues.size() != 3) return;

    bool ok;
    int r = rgbValues[0].toInt(&ok);
    if (!ok || r < 0 || r > 255) return;
    int g = rgbValues[1].toInt(&ok);
    if (!ok || g < 0 || g > 255) return;
    int b = rgbValues[2].toInt(&ok);
    if (!ok || b < 0 || b > 255) return;

    rSlider->blockSignals(true);
    gSlider->blockSignals(true);
    bSlider->blockSignals(true);

    rSlider->setValue(r);
    gSlider->setValue(g);
    bSlider->setValue(b);

    rSlider->blockSignals(false);
    gSlider->blockSignals(false);
    bSlider->blockSignals(false);

    updateFromSliders();
}

void MainWindow::updateXYZ() {
    QStringList xyzValues = xyzInput->text().split(',');
    if (xyzValues.size() != 3) return;

    bool ok;
    double x = xyzValues[0].toDouble(&ok);
    if (!ok) return;
    double y = xyzValues[1].toDouble(&ok);
    if (!ok) return;
    double z = xyzValues[2].toDouble(&ok);
    if (!ok) return;

    double r, g, b;
    xyzToRgb(x, y, z, r, g, b);

    rgbInput->setText(QString("%1,%2,%3").arg(static_cast<int>(r)).arg(static_cast<int>(g)).arg(static_cast<int>(b)));

    rSlider->blockSignals(true);
    gSlider->blockSignals(true);
    bSlider->blockSignals(true);

    rSlider->setValue(static_cast<int>(r));
    gSlider->setValue(static_cast<int>(g));
    bSlider->setValue(static_cast<int>(b));

    rSlider->blockSignals(false);
    gSlider->blockSignals(false);
    bSlider->blockSignals(false);

    updateColorDisplay();
}

void MainWindow::updateHSV() {
    QStringList hsvValues = hsvInput->text().split(',');
    if (hsvValues.size() != 3) return;

    bool ok;
    double h = hsvValues[0].toDouble(&ok);
    if (!ok || h < 0 || h > 360) return;
    double s = hsvValues[1].toDouble(&ok) / 100.0;
    if (!ok || s < 0 || s > 1) return;
    double v = hsvValues[2].toDouble(&ok) / 100.0;
    if (!ok || v < 0 || v > 1) return;

    double r, g, b;
    hsvToRgb(h, s, v, r, g, b);

    rgbInput->setText(QString("%1,%2,%3").arg(static_cast<int>(r)).arg(static_cast<int>(g)).arg(static_cast<int>(b)));

    rSlider->blockSignals(true);
    gSlider->blockSignals(true);
    bSlider->blockSignals(true);

    rSlider->setValue(static_cast<int>(r));
    gSlider->setValue(static_cast<int>(g));
    bSlider->setValue(static_cast<int>(b));

    rSlider->blockSignals(false);
    gSlider->blockSignals(false);
    bSlider->blockSignals(false);

    updateColorDisplay();
}

void MainWindow::updateFromSliders() {
    int r = rSlider->value();
    int g = gSlider->value();
    int b = bSlider->value();

    rgbInput->setText(QString("%1,%2,%3").arg(r).arg(g).arg(b));

    double x, y, z;
    rgbToXyz(r, g, b, x, y, z);
    xyzInput->setText(QString("%1,%2,%3").arg(x).arg(y).arg(z));

    double h, s, v;
    rgbToHsv(r, g, b, h, s, v);
    hsvInput->setText(QString("%1,%2,%3").arg(h).arg(s * 100).arg(v * 100));

    updateColorDisplay();
}

void MainWindow::updateColorDisplay() {
    int r = rSlider->value();
    int g = gSlider->value();
    int b = bSlider->value();

    QColor color(r, g, b);
    QString colorStyle = QString("background-color: rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
    colorDisplay->setStyleSheet(colorStyle);
}

void MainWindow::rgbToXyz(double r, double g, double b, double &x, double &y, double &z) {
    r /= 255.0; g /= 255.0; b /= 255.0;
    r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : (r / 12.92);
    g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : (g / 12.92);
    b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : (b / 12.92);
    r *= 100.0; g *= 100.0; b *= 100.0;
    x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    z = r * 0.0193 + g * 0.1192 + b * 0.9505;
}

void MainWindow::xyzToRgb(double x, double y, double z, double &r, double &g, double &b) {
    x /= 100.0; y /= 100.0; z /= 100.0;
    r = x * 3.2406 + y * -1.5372 + z * -0.4986;
    g = x * -0.9689 + y * 1.8758 + z * 0.0415;
    b = x * 0.0557 + y * -0.2040 + z * 1.0570;
    r = (r > 0.0031308) ? (1.055 * pow(r, (1.0 / 2.4)) - 0.055) : (12.92 * r);
    g = (g > 0.0031308) ? (1.055 * pow(g, (1.0 / 2.4)) - 0.055) : (12.92 * g);
    b = (b > 0.0031308) ? (1.055 * pow(b, (1.0 / 2.4)) - 0.055) : (12.92 * b);
    r *= 255.0; g *= 255.0; b *= 255.0;
    r = qBound(0.0, r, 255.0);
    g = qBound(0.0, g, 255.0);
    b = qBound(0.0, b, 255.0);
}

void MainWindow::rgbToHsv(double r, double g, double b, double &h, double &s, double &v) {
    QColor color(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
    h = color.hue();
    s = color.saturationF();
    v = color.valueF();
}

void MainWindow::hsvToRgb(double h, double s, double v, double &r, double &g, double &b) {
    QColor color;
    color.setHsv(static_cast<int>(h), static_cast<int>(s * 255), static_cast<int>(v * 255));
    r = color.red();
    g = color.green();
    b = color.blue();
}
