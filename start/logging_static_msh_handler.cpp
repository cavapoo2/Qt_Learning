// In a global or your main.cpp
static QFile logFile;
static QTextStream logStream;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if (!logFile.isOpen())
        return;

    QString level;
    switch (type) {
        case QtDebugMsg:    level = "DEBUG"; break;
        case QtWarningMsg:  level = "WARN "; break;
        case QtCriticalMsg: level = "ERROR"; break;
        case QtFatalMsg:    level = "FATAL"; break;
        default:            level = "INFO "; break;
    }

    logStream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
              << " [" << level << "] "
              << msg << Qt::endl;  // Qt::endl flushes each time
}

// Call this once at startup in main()
bool initLogging()
{
    logFile.setFileName("app.log");
    if (!logFile.open(QIODevice::Append | QIODevice::Text))
        return false;

    logStream.setDevice(&logFile);
    qInstallMessageHandler(messageHandler);
    return true;
}
