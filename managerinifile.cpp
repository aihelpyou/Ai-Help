#include "managerinifile.h"
#include <QDebug>

ManagerIniFile::ManagerIniFile(QString strIniFileName):m_settings(strIniFileName,QSettings::IniFormat)
{
}

void ManagerIniFile::WriteSetting(QString strGroup, QString strValueName,QString strValue)
{
    m_settings.beginGroup(strGroup);
    m_settings.setValue(strValueName,strValue);
    m_settings.endGroup();
}

QString ManagerIniFile::ReadSetting(QString strGroup, QString strValueName)
{
    QString str = QString("%1/%2").arg(strGroup,strValueName);

    qDebug()<<str;
    return m_settings.value(str).toString();
}
