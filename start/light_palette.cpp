#include <QApplication>
#include <QMainWindow>
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette lightPalette = app.palette();
    lightPalette.setColor(QPalette::Window,          QColor(240, 240, 240));
    lightPalette.setColor(QPalette::WindowText,      Qt::black);
    lightPalette.setColor(QPalette::Base,            QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase,   QColor(233, 233, 233));
    lightPalette.setColor(QPalette::ToolTipBase,     Qt::white);
    lightPalette.setColor(QPalette::ToolTipText,     Qt::black);
    lightPalette.setColor(QPalette::Text,            Qt::black);
    lightPalette.setColor(QPalette::Button,          QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText,      Qt::black);
    lightPalette.setColor(QPalette::BrightText,      Qt::red);
    lightPalette.setColor(QPalette::Highlight,       QColor(0, 120, 215));
    lightPalette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(lightPalette);

    QMainWindow window;
    window.show();
    return app.exec();
}
