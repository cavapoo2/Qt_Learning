// Setup
QGroupBox *groupBox = new QGroupBox("Advanced Options", this);
groupBox->setCheckable(true);
groupBox->setChecked(false); // Start collapsed/hidden

QVBoxLayout *layout = new QVBoxLayout;
layout->addWidget(new QLabel("Option A"));
layout->addWidget(new QLabel("Option B"));
groupBox->setLayout(layout);

// Connect the toggled signal to show/hide children
connect(groupBox, &QGroupBox::toggled, this, [groupBox](bool checked) {
    for (QWidget *child : groupBox->findChildren<QWidget*>(Qt::FindDirectChildrenOnly)) {
        child->setVisible(checked);
    }
});

// Trigger once to set initial state
emit groupBox->toggled(false);
