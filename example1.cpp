// mainwindow.cpp
#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Qt Layout Demo");
    resize(400, 200);

    QWidget *central = new QWidget(this);
    QVBoxLayout *main = new QVBoxLayout(central);
    main->setContentsMargins(16, 16, 16, 16);
    main->setSpacing(12);

    // Form
    QFormLayout *form = new QFormLayout;
    m_nameEdit  = new QLineEdit;
    m_emailEdit = new QLineEdit;
    form->addRow("Name:",  m_nameEdit);
    form->addRow("Email:", m_emailEdit);
    main->addLayout(form);

    // Status label
    m_statusLabel = new QLabel;
    m_statusLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(m_statusLabel);

    main->addStretch();

    // Buttons
    QHBoxLayout *btnRow = new QHBoxLayout;
    QPushButton *clearBtn  = new QPushButton("Clear");
    QPushButton *submitBtn = new QPushButton("Submit");
    btnRow->addStretch();
    btnRow->addWidget(clearBtn);
    btnRow->addWidget(submitBtn);
    main->addLayout(btnRow);

    setCentralWidget(central);

    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmit);
    connect(clearBtn,  &QPushButton::clicked, this, [this]() {
        m_nameEdit->clear();
        m_emailEdit->clear();
        m_statusLabel->clear();
    });
}

void MainWindow::onSubmit() {
    QString name  = m_nameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    if (name.isEmpty() || email.isEmpty()) {
        m_statusLabel->setText("⚠ Please fill in all fields.");
    } else {
        m_statusLabel->setText(QString("Submitted: %1 <%2>").arg(name, email));
    }
}
