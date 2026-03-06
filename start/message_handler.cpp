void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);   // thread-safe

    QFile logFile("app.log");
    if (logFile.open(QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered))
    {
        QTextStream stream(&logFile);
        stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
               << " " << msg << Qt::endl;
    }
}
