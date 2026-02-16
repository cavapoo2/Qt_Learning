#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

QWidget *window = new QWidget;

// Horizontal row of buttons
QHBoxLayout *hbox = new QHBoxLayout;
hbox->addWidget(new QPushButton("Left"));
hbox->addWidget(new QPushButton("Centre"));
hbox->addWidget(new QPushButton("Right"));

// Wrap in a vertical layout with a label on top
QVBoxLayout *vbox = new QVBoxLayout;
vbox->addWidget(new QLabel("Choose an option:"));
vbox->addLayout(hbox);   // nest layouts with addLayout()

window->setLayout(vbox);
window->show();
