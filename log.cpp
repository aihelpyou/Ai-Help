#include "log.h"

void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("[%1] File[%2] Line[%3] Function:%4 Message:%5")
                             .arg(strDateTime)
                             .arg(context.file)
                             .arg(context.line)
                             .arg(context.function)
                             .arg(localMsg.constData());

    QFile file(QCoreApplication::applicationDirPath() + "/log/" + LOG_FILENAME);
    file.open(QIODevice::ReadWrite | QIODevice::Text| QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}
void logInit()
{
    const int SIZE = 1024 * 1024 *4;
    QString path = QCoreApplication::applicationDirPath();
    if(QFile::exists(path + LOG_FILENAME) && (SIZE < QFile(path + LOG_FILENAME).size()))
    {
        QFile::remove(path + LOG_FILENAME);
    }

    qInstallMessageHandler(MessageOutput);
}
