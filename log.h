
#ifndef LOG_H
#define LOG_H
#include <QString>
#include <QByteArray>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#define LOG_FILENAME "AiHelpBuildInfoConfig.log"

void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void logInit();

#endif // LOG_H
