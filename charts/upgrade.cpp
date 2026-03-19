void MainWindow::ingestEventData()
{
    m_events.clear();

    const int timeCount = static_cast<int>(m_timestamps.size());

    for (int i = 0; i < timeCount; ++i)
    {
        const qreal timestamp = m_timestamps[i];

        for (int index = 0; index < 128; ++index)
        {
            if ((int)m_event[index].size() <= i)
                continue;

            const char value = m_event[index][i];

            FaultEvent ev;
            ev.faultIndex = index;
            ev.timestamp  = timestamp;
            ev.state      = (value == 1) ? NoFault
                                         : m_faultDefs[index].severity;
            m_events.append(ev);
        }
    }

    // Now let the filter/axis/scroll pipeline do all series writing
    applyFilter();
}
```

Then the full pipeline flows cleanly in one direction:
```
m_event[128] + m_timestamps
        │
        ▼
ingestEventData()        — reads raw data, builds m_events, calls applyFilter()
        │
        ▼
applyFilter()            — orchestrates the three steps below in order
        │
        ├──▶ rebuildSeriesPoints()   — clears all series, replays m_events
        │                              through both filter gates, appends points
        │
        ├──▶ rebuildYAxis()          — inspects what was actually plotted,
        │                              builds axis labels for those rows only



void MainWindow::applyFilter()
{
    rebuildSeriesPoints();
    rebuildYAxis();
    updateScrollBar();
}

void MainWindow::rebuildSeriesPoints()
{
    m_seriesNone->clear();
    m_seriesCritical->clear();
    m_seriesDegradable->clear();
    m_seriesServiceable->clear();

    // Gate 1: which fault states are enabled
    const bool showNone        = m_cbNoFault    ->isChecked();
    const bool showCritical    = m_cbCritical   ->isChecked();
    const bool showDegradable  = m_cbDegradable ->isChecked();
    const bool showServiceable = m_cbServiceable->isChecked();

    for (const auto& ev : m_events)
    {
        // Gate 2: is this individual fault's checkbox enabled
        if (!m_checkboxes.contains(ev.faultIndex) ||
            !m_checkboxes[ev.faultIndex]->isChecked())
            continue;

        const qreal y = (qreal)ev.faultIndex;

        switch (ev.state)
        {
            case NoFault:
                if (showNone)
                    m_seriesNone->append(ev.timestamp, y);
                break;
            case Critical:
                if (showCritical)
                    m_seriesCritical->append(ev.timestamp, y);
                break;
            case Degradable:
                if (showDegradable)
                    m_seriesDegradable->append(ev.timestamp, y);
                break;
            case Serviceable:
                if (showServiceable)
                    m_seriesServiceable->append(ev.timestamp, y);
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

    // Collect the fault indices that have at least one plotted point
    QSet<int> plottedIndices;
    for (auto* s : {m_seriesNone, m_seriesCritical,
                    m_seriesDegradable, m_seriesServiceable})
        for (const auto& pt : s->points())
            plottedIndices.insert((int)pt.y());

    // Match back to fault definitions to get the bit names, sorted by index
    QList<FaultDefinition> visibleFaults;
    for (const auto& fd : m_faultDefs)
        if (plottedIndices.contains(fd.index))
            visibleFaults.append(fd);

    if (visibleFaults.isEmpty())
    {
        m_axisY->setMin(-0.5);
        m_axisY->setMax(0.5);
    }
    else
    {
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


        │
        └──▶ updateScrollBar()       — sizes scroll window to plotted row count
