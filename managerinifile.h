#ifndef MANAGERINIFILE_H
#define MANAGERINIFILE_H
#include <QString>
#include <QSettings>

class ManagerIniFile
{
public:
    ManagerIniFile(QString strIniFileName);
    void WriteSetting(QString strGroup, QString strValueName,QString strValue);
    QString ReadSetting(QString strGroup, QString strValueName);

private:
    QSettings m_settings;
};

#endif // MANAGERINIFILE_H
