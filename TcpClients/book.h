#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>


class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU* pdu);
//    bool m_bDownload;
//    QString m_strSaveFilePath;
    qint64 m_iTotal;

signals:

public slots:
    void createDir();
    void refreshFile();
    void uploadFile();

    void uploadFileData();

    void downloadFile();
    void setDownloadStatus(bool status);

    bool getDownloadstatus();
    QString getSaveFilePath();


private:
    QListWidget* m_pBookListW;
    QPushButton* m_pReturnPB;
    QPushButton* m_pCreateFolderPB;
    QPushButton* m_pDelFolderPB;
    QPushButton* m_pRenameFolderPB;
    QPushButton* m_pRefreshPB;
    QPushButton* m_pUploadFilePB;
    QPushButton* m_pDownloadFilePB;
    QPushButton* m_pDelFilePB;
    QPushButton* m_pShareFilePB;

    QString m_strUploadFilePath;

    QString m_strSaveFilePath;
    bool m_bDownload;

    QTimer* m_pTimer;


    qint64 m_iReceived;
};

#endif // BOOK_H
