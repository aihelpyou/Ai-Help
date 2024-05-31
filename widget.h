#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemModel>
#include <QTableView>
#include <QStandardItemModel>
#include <QDateTime>
#include <QFileDialog>
#include <QDebug>
#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QHash>
#include "managerinifile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_btnBuild_clicked();

    void on_btnClear_clicked();

    void on_btnAddMainDir_clicked();

    void on_btnAddRootDir_clicked();

    void on_btnAddFile_clicked();

    void on_btnSaveSetting_clicked();

    void on_btnConfigVersionAddOne_clicked();

public:
    void buildVersion();
    void buildFileList();
    void deleteFiles(const QString &path);


private:
    Ui::Widget *ui;

    QString m_rootPath;//输出目录

    QString m_mainPath;//源目录


    QHash<QString, QString> m_fileHashes;

    QStandardItemModel *headerModel;



    void addFileToMap(const QString &filePath);

    void displayOnView();

    QString calculateFileMD5(const QString &filePath);

    void setBtnEnable();

    void saveSettings();

    void loadSettings();

    void versionAddOne();

 private:
    ManagerIniFile *m_pConfig;
    QString m_strGlobalUrl;

    QString m_strServerUpdateFileName;
};

#endif // WIDGET_H
