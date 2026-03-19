// Header
private:
    QCheckBox* m_cbCritical    = nullptr;
    QCheckBox* m_cbDegradable  = nullptr;
    QCheckBox* m_cbServiceable = nullptr;
    QCheckBox* m_cbNoFault     = nullptr;

    struct FaultDefinition {
        int         index;        // 0-127, fixed Y position
        QString     bitName;      // label on Y axis
        bool        visible;      // checkbox state
    };

    enum FaultState { NoFault, Critical, Degradable, Serviceable };

    struct FaultEvent {
        int         faultIndex;   // which fault (maps to FaultDefinition::index)
        qreal       timestamp;
        FaultState  state;
    };

    void applyFilter();
    void rebuildYAxis();
    void rebuildSeriesPoints();

    QList<FaultDefinition>  m_faultDefs;     // all 128 faults
    QList<FaultEvent>       m_events;        // all recorded events

    // One series per visual state
    QScatterSeries*  m_seriesNone        = nullptr;  // green
    QScatterSeries*  m_seriesCritical    = nullptr;  // red
    QScatterSeries*  m_seriesDegradable  = nullptr;  // amber
    QScatterSeries*  m_seriesServiceable = nullptr;  // blue

    QCategoryAxis*   m_axisY = nullptr;
    QDateTimeAxis*   m_axisX = nullptr;
    QChart*          m_chart = nullptr;

    QMap<int, QCheckBox*> m_checkboxes;   // faultIndex → checkbox


void MainWindow::setupChart()
{
    m_chart = new QChart();

    // -- Define your 128 faults (sample subset shown) --
    m_faultDefs = {
        {0,   "OIL_PRESSURE_LOW",    true},
        {1,   "COOLANT_TEMP_HIGH",   true},
        {2,   "BATTERY_VOLTAGE_LOW", true},
        // ... up to index 127
        {43,  "GEARBOX_RATIO_ERR",   true},
        {78,  "THROTTLE_POS_FAULT",  true},
        {99,  "ABS_SENSOR_FAIL",     true},
    };

    // -- Create one series per fault state --
    auto makeSeries = [&](const QString& name, QColor color) {
        auto* s = new QScatterSeries();
        s->setName(name);
        s->setColor(color);
        s->setBorderColor(color.darker(120));
        s->setMarkerSize(10);
        m_chart->addSeries(s);
        return s;
    };

    m_seriesNone        = makeSeries("No Fault",   QColor("#4CAF50"));  // green
    m_seriesCritical    = makeSeries("Critical",   QColor("#F44336"));  // red
    m_seriesDegradable  = makeSeries("Degradable", QColor("#FF9800"));  // amber
    m_seriesServiceable = makeSeries("Serviceable",QColor("#2196F3"));  // blue

    // -- Axes --
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("hh:mm:ss");
    m_axisX->setTitleText("Time");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    m_axisY = new QCategoryAxis();
    m_axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    for (auto* s : {m_seriesNone, m_seriesCritical,
                    m_seriesDegradable, m_seriesServiceable}) {
        s->attachAxis(m_axisX);
        s->attachAxis(m_axisY);
    }

    // -- Populate some sample events --
    auto now = (qreal)QDateTime::currentMSecsSinceEpoch();
    m_events = {
        {0,  now + 1000, NoFault},
        {0,  now + 2000, Critical},
        {0,  now + 3000, NoFault},
        {43, now + 500,  NoFault},
        {43, now + 2500, Degradable},
        {78, now + 1000, NoFault},
        {78, now + 1500, Serviceable},
        {78, now + 3500, Critical},
        {99, now + 2000, NoFault},
    };

    applyFilter();
}

void MainWindow::applyFilter()
{
    rebuildSeriesPoints();
    rebuildYAxis();
}


void MainWindow::setupCheckboxes()
{
    // In a real UI with 128 faults this would be a QListWidget
    // with checkable items rather than raw QCheckBoxes
    auto* row = new QHBoxLayout();

    for (const auto& fd : m_faultDefs) {
        auto* cb = new QCheckBox(fd.bitName);
        cb->setChecked(fd.visible);
        m_checkboxes[fd.index] = cb;

        connect(cb, &QCheckBox::toggled, this, [this](bool) {
            applyFilter();
        });
        row->addWidget(cb);
    }

    auto* layout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    layout->addLayout(row);
}


void MainWindow::setupStateCheckboxes()
{
    auto* row = new QHBoxLayout();

    struct Entry {
        QString     label;
        QColor      color;
        bool        defaultOn;
        QCheckBox** member;
    };

    const QList<Entry> entries = {
        {"Critical",    QColor("#F44336"), true,  &m_cbCritical},
        {"Degradable",  QColor("#FF9800"), true,  &m_cbDegradable},
        {"Serviceable", QColor("#2196F3"), false, &m_cbServiceable},
        {"No Fault",    QColor("#4CAF50"), false, &m_cbNoFault},
    };

    for (const auto& e : entries) {
        auto* cb = new QCheckBox(e.label);
        cb->setChecked(e.defaultOn);

        // Tint the checkbox label to match the series colour
        cb->setStyleSheet(QString("QCheckBox { color: %1; font-weight: bold; }")
                          .arg(e.color.name()));

        *e.member = cb;

        connect(cb, &QCheckBox::toggled, this, [this](bool) {
            applyFilter();
        });

        row->addWidget(cb);
    }

    row->addStretch();

    auto* layout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    layout->addLayout(row);
}

void MainWindow::rebuildSeriesPoints()
{
    m_seriesNone->clear();
    m_seriesCritical->clear();
    m_seriesDegradable->clear();
    m_seriesServiceable->clear();

    // Which fault states are currently enabled
    bool showNone        = m_cbNoFault     && m_cbNoFault->isChecked();
    bool showCritical    = m_cbCritical    && m_cbCritical->isChecked();
    bool showDegradable  = m_cbDegradable  && m_cbDegradable->isChecked();
    bool showServiceable = m_cbServiceable && m_cbServiceable->isChecked();

    // Which individual faults are visible
    QSet<int> visibleFaults;
    for (const auto& fd : m_faultDefs)
        if (m_checkboxes.contains(fd.index) &&
            m_checkboxes[fd.index]->isChecked())
            visibleFaults.insert(fd.index);

    for (const auto& ev : m_events) {
        if (!visibleFaults.contains(ev.faultIndex))
            continue;

        qreal y = ev.faultIndex;

        switch (ev.state) {
            case NoFault:
                if (showNone)        m_seriesNone->append(ev.timestamp, y);
                break;
            case Critical:
                if (showCritical)    m_seriesCritical->append(ev.timestamp, y);
                break;
            case Degradable:
                if (showDegradable)  m_seriesDegradable->append(ev.timestamp, y);
                break;
            case Serviceable:
                if (showServiceable) m_seriesServiceable->append(ev.timestamp, y);
                break;
        }
    }
}

void MainWindow::rebuildYAxis()
{
    for (auto* s : {m_seriesNone, m_seriesCritical,
                    m_seriesDegradable, m_seriesServiceable})
        s->detachAxis(m_axisY);

    m_chart->removeAxis(m_axisY);
    m_axisY->deleteLater();

    m_axisY = new QCategoryAxis();
    m_axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    // Collect indices that actually have a plotted point after filtering
    QSet<int> plottedIndices;
    for (auto* s : {m_seriesNone, m_seriesCritical,
                    m_seriesDegradable, m_seriesServiceable})
        for (const auto& pt : s->points())
            plottedIndices.insert((int)pt.y());

    // Filter fault definitions down to only those with plotted points
    QList<FaultDefinition> visibleFaults;
    for (const auto& fd : m_faultDefs)
        if (plottedIndices.contains(fd.index))
            visibleFaults.append(fd);  // already sorted by index

    if (visibleFaults.isEmpty()) {
        m_axisY->setMin(0);
        m_axisY->setMax(1);
    } else {
        m_axisY->setMin(visibleFaults.first().index - 0.5);
        m_axisY->setMax(visibleFaults.last().index  + 0.5);

        for (const auto& fd : visibleFaults)
            m_axisY->append(fd.bitName, fd.index);
    }

    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    for (auto* s : {m_seriesNone, m_seriesCritical,
                    m_seriesDegradable, m_seriesServiceable})
        s->attachAxis(m_axisY);
}

