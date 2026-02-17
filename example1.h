// mainwindow.h
#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void onSubmit();
private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_emailEdit;
    QLabel    *m_statusLabel;
};
