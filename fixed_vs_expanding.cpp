
QHBoxLayout *layout = new QHBoxLayout;

QLabel *label = new QLabel("Username:");
label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
// equivalent shorthand — Fixed in both axes:
// label->setFixedWidth(80);

QLineEdit *input = new QLineEdit;
input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

layout->addWidget(label);
layout->addWidget(input);
