#include "widget.h"
#include "ui_widget.h"

#include <QStandardItem>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/new/prefix1/logo.ico"));

    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->labelDateTime->setText(currentDateTime.toString("yyyy-MM-dd hh:mm:ss"));

    ui->lineEditURL->setText("http://");

    headerModel = new QStandardItemModel(this);
    headerModel->setColumnCount(3); // 设置列数为3
    headerModel->setHorizontalHeaderLabels(QStringList() << QStringLiteral("文件名") << QStringLiteral("相对路径") << QStringLiteral("MD5值"));
    ui->tableView->setModel(headerModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 让列自动拉伸

    m_strServerUpdateFileName = QString("ServerUpdateInfo.ini");
    loadSettings();

    setBtnEnable();

}

Widget::~Widget()
{
    saveSettings();
    delete ui;

}



void Widget::on_btnBuild_clicked()
{
    m_rootPath =   ui->lineEditRootDir->text();

    QString destFilePath = m_rootPath + '/' + m_strServerUpdateFileName;
    QString destDirPath = QFileInfo(destFilePath).path();

    QFile file(destFilePath);
    if(file.exists())
    {
        file.remove();
    }
    QDir().mkpath(destDirPath); // 创建相应文件夹

    versionAddOne();

    QString globalURL = ui->lineEditURL->text();
    if (!globalURL.endsWith('/'))
        globalURL.append('/');

    QString version = ui->lineEditVersion->text();

    buildVersion();
    buildFileList();
    saveSettings();
    QMessageBox::information(this, "Success", "Server version info generated successfully.");
}


void Widget::addFileToMap(const QString &filePath)
{
    QString fileName = QFileInfo(filePath).fileName();
    QString relativePath = QDir(m_mainPath).relativeFilePath(filePath);
    QString fileHash = calculateFileMD5(filePath);

    m_fileHashes.insert(relativePath, fileHash);
    qDebug() <<fileName<<"-"<<relativePath<<"-"<<fileHash;

    QList<QStandardItem*> rowItems;
    rowItems.append(new QStandardItem(fileName));
    rowItems.append(new QStandardItem(relativePath));
    rowItems.append(new QStandardItem(fileHash));
    headerModel->appendRow(rowItems);
}

void Widget::displayOnView()
{
    ui->tableView->setModel(headerModel);

}

QString Widget::calculateFileMD5(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file for reading: " << filePath;
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    if(hash.addData(&file))
    {
        return hash.result().toHex();
    }

    qDebug() << "Failed to calculate MD5 for file: " << filePath;
    return QString();
}

void Widget::setBtnEnable()
{
    if(ui->lineEditMainDir->text().isEmpty() || ui->lineEditMainDir->text().isEmpty())
    {
        ui->btnAddFile->setEnabled(false);
        ui->btnBuild->setEnabled(false);
        ui->btnClear->setEnabled(false);
    }
    else
    {
        ui->btnAddFile->setEnabled(true);
        ui->btnBuild->setEnabled(true);
        ui->btnClear->setEnabled(true);
    }
}

void Widget::on_btnClear_clicked()
{
    m_rootPath = ui->lineEditRootDir->text();

    deleteFiles(m_rootPath);

    m_fileHashes.clear();

    // 获取行数
    int rowCount =  headerModel->rowCount();

    // 从末尾开始删除行
    for (int i = rowCount - 1; i >= 0; --i) {
         headerModel->removeRow(i);
    }
    displayOnView();

    ui->btnBuild->setEnabled(false);
    ui->btnClear->setEnabled(false);
}

void Widget::on_btnAddMainDir_clicked()
{
    //选择源目录
    QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("选择文件夹"), QDir::homePath());
    if(!path.isEmpty())
    {
       ui->lineEditMainDir->setText(path);
       m_mainPath = path;
       //ui->btnAddFile->setEnabled(true);
    }
    setBtnEnable();
}

void Widget::on_btnAddRootDir_clicked()
{
    // 选择输出目录
    QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("选择文件夹"), QDir::homePath());
    if(!path.isEmpty())
    {
       ui->lineEditRootDir->setText(path);
       m_rootPath = path;
       //ui->btnAddFile->setEnabled(true);
    }
    setBtnEnable();
}

void Widget::on_btnAddFile_clicked()
{
    m_mainPath = ui->lineEditMainDir->text();
    QStringList fileNames = QFileDialog::getOpenFileNames(this, QStringLiteral("选择文件"), m_mainPath);


    m_strGlobalUrl = ui->lineEditURL->text();
    //将文件信息存入Map
    for(const QString &fileName : fileNames)
    {
        addFileToMap(fileName);
    }

    displayOnView();
    ui->btnClear->setEnabled(true);
    ui->btnBuild->setEnabled(true);
}

void Widget::buildVersion()
{
    QString destFilePath = m_rootPath + '/' + m_strServerUpdateFileName;
    ManagerIniFile iniFile(destFilePath);
    QString strVersion = ui->lineEditVersion->text();
    qDebug()<<"ServerUpdateInfo strVersion "<<strVersion;
    iniFile.WriteSetting("ServerUpdateInfo","version",strVersion);

    QDateTime todayDateTime = QDateTime::currentDateTime();
    QString localUpdateTime = todayDateTime.toString("yyyy-MM-dd hh:mm:ss");
    iniFile.WriteSetting("ServerUpdateInfo","date",localUpdateTime);

}

void Widget::deleteFiles(const QString &path) {
    QDir dir(path);

    if (!dir.exists()) {
        qDebug() << "Folder does not exist!";
        return;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();

    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            QFile::remove(fi.absoluteFilePath());
        } else {
            deleteFiles(fi.absoluteFilePath());
        }
    }

    if (dir.rmpath(dir.absolutePath())) {
        qDebug() << "Folder deleted successfully!";
    } else {
        qDebug() << "Error occurred!";
    }
}
void Widget::buildFileList()
{
    m_mainPath = ui->lineEditMainDir->text();
    m_rootPath = ui->lineEditRootDir->text();

    int index = 1;
    QString destFilePath = m_rootPath + '/' + m_strServerUpdateFileName;
    ManagerIniFile iniFile(destFilePath);

    int nTotal = m_fileHashes.count();
    QString strTotal = QString("%1").arg(nTotal);

    iniFile.WriteSetting("UpdateFileNum","total",strTotal);
    qDebug()<<"buildFileList total : "<<strTotal;

    for (auto it = m_fileHashes.begin(); it != m_fileHashes.end(); ++it)
    {

        QString filePath = it.key();
        QString fileURL = m_strGlobalUrl +  filePath;
        QString fileHash = it.value();

        QString sourceFilePath = ui->lineEditMainDir->text() + '/' + filePath;

        QString destFilePath = m_rootPath + '/' + filePath;
        QString destDirPath = QFileInfo(destFilePath).path();
        QDir().mkpath(destDirPath); // 创建相应文件夹


        bool exist = QFile::exists(sourceFilePath);
        if(exist)
        {
            if (!QFile::copy(sourceFilePath, destFilePath))
            {
                QMessageBox::warning(this, "Error", "Failed to copy file: " + filePath);
                qDebug()<<"filePath:"<<filePath<<"\n destFilePath:"<<destFilePath<<"\n destDirPath:"<<destDirPath;
            }
        }

        QString strUpdateFile = QString("UpdateFile%1").arg(index);
        qDebug()<<strUpdateFile<<" : "<<strUpdateFile;
        iniFile.WriteSetting(strUpdateFile,"name",filePath);
        iniFile.WriteSetting(strUpdateFile,"hash",fileHash);
        iniFile.WriteSetting(strUpdateFile,"url",fileURL);

        index++;
    }

}
void Widget::saveSettings()
{
     ManagerIniFile iniFile("BuildHistory.ini");

    iniFile.WriteSetting("DirectoryConfig","sourceDir",ui->lineEditRootDir->text());
    iniFile.WriteSetting("DirectoryConfig","mainDir",ui->lineEditMainDir->text());
    iniFile.WriteSetting("DirectoryConfig","url",ui->lineEditURL->text());
    iniFile.WriteSetting("DirectoryConfig","version",ui->lineEditVersion->text());
}

void Widget::loadSettings()
{
    ManagerIniFile iniFile("BuildHistory.ini");
    QString strSourceDir = iniFile.ReadSetting("DirectoryConfig","sourceDir");
    ui->lineEditRootDir->setText(strSourceDir);

    QString strMainDir = iniFile.ReadSetting("DirectoryConfig","mainDir");
    ui->lineEditMainDir->setText(strMainDir);

    QString strUrl = iniFile.ReadSetting("DirectoryConfig","url");
    ui->lineEditURL->setText(strUrl);

    QString strVersion = iniFile.ReadSetting("DirectoryConfig","version");
    ui->lineEditVersion->setText(strVersion);
}

void Widget::on_btnSaveSetting_clicked()
{
    saveSettings();
}

void Widget::versionAddOne()
{
    QString strVerison = ui->lineEditVersion->text();
    QStringList list = strVerison.split('.');
    QString lastElement = list.back();
    list.pop_back();
    QString newMinVersion = QString("%1").arg(lastElement.toInt()+1);
    list.push_back(newMinVersion);
    QString newVersion = list.join(".");
    ui->lineEditVersion->setText(newVersion);
}

void Widget::on_btnConfigVersionAddOne_clicked()
{
    versionAddOne();
}

