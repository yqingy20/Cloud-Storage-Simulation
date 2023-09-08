#include "book.h"
#include "tcpclients.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("Return");
    m_pCreateFolderPB = new QPushButton("New Folder");
    m_pDelFolderPB = new QPushButton("Delete Folder");
    m_pRenameFolderPB = new QPushButton("Rename Folder");
    m_pRefreshPB = new QPushButton("Refresh");

    QVBoxLayout* pFolderVBL = new QVBoxLayout;
    pFolderVBL->addWidget(m_pReturnPB);
    pFolderVBL->addWidget(m_pCreateFolderPB);
    pFolderVBL->addWidget(m_pDelFolderPB);
    pFolderVBL->addWidget(m_pRenameFolderPB);
    pFolderVBL->addWidget(m_pRefreshPB);

    m_pUploadFilePB = new QPushButton("Upload File");
    m_pDownloadFilePB = new QPushButton("Download File");
    m_pDelFilePB = new QPushButton("Delete File");
    m_pShareFilePB = new QPushButton("Share File");

    QVBoxLayout* pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadFilePB);
    pFileVBL->addWidget(m_pDownloadFilePB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout* pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pFolderVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    m_pTimer = new QTimer;

    connect(m_pCreateFolderPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));
    connect(m_pRefreshPB, SIGNAL(clicked(bool)), this, SLOT(refreshFile()));

    connect(m_pUploadFilePB, SIGNAL(clicked(bool)), this, SLOT(uploadFile()));
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(uploadFileData()));
    connect(m_pDownloadFilePB, SIGNAL(clicked(bool)), this, SLOT(downloadFile()));

}

void Book::createDir() {
    QString strNewDir = QInputDialog::getText(this, "Create New Folder", "New Folder's Name:");
    if (!strNewDir.isEmpty()) {
        if (strNewDir.size() > 32) {
            QMessageBox::warning(this, "Create New Folder", "The length of the new folder's name must be within 32 characters. ");
        } else {
            QString strName = TcpClients::getInstance().getLoginName();
            QString strCurPath = TcpClients::getInstance().getCurrentPath();

            PDU *pdu = mkPDU(strCurPath.size() + 1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, strName.toStdString().c_str(), strName.size());
            strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.size());
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());

            TcpClients::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULength);
            free(pdu);
            pdu = NULL;
        }
    } else {
        QMessageBox::warning(this, "Create New Folder", "The name of your new folder can't be null");
    }


}

void Book::refreshFile() {
    QString strCurPath = TcpClients::getInstance().getCurrentPath();
    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_FILE_REQUEST;
    strncpy((char*) pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
//    qDebug() << strCurPath;
    TcpClients::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULength);
    free(pdu);
    pdu = NULL;

}


void Book::updateFileList(const PDU* pdu) {
    if (pdu == NULL) {
        return;
    }

    FileInfo* pFileInfo = NULL;
    int count = pdu->uiMsgLength / sizeof(FileInfo);
    for (int i = 0; i < count; i++) {
        pFileInfo = (FileInfo*)(pdu->caMsg) + i;
        qDebug() << pFileInfo->caFileName << pFileInfo->intFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if (pFileInfo->intFileType == 0) {
            pItem->setIcon(QIcon(QPixmap(":/map/folder.jpg")));
        } else if (pFileInfo->intFileType == 1) {
            pItem->setIcon(QIcon(QPixmap(":/map/file.jpg")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);

    }

//    if (NULL == pdu)
//    {
//        return;
//    }

//    QListWidgetItem *pItemTmp = NULL;
//    int row = m_pBookListW->count();
//    while (m_pBookListW->count()>0)
//    {
//        pItemTmp = m_pBookListW->item(row-1);
//        m_pBookListW->removeItemWidget(pItemTmp);
//        delete pItemTmp;
//        row = row-1;
//    }

//    FileInfo *pFileInfo = NULL;
//    int iCount = pdu->uiMsgLength/sizeof(FileInfo);
//    for (int i=0; i<iCount; i++)
//    {
//        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
//        qDebug() << pFileInfo->caFileName << pFileInfo->intFileType;
//        QListWidgetItem *pItem = new QListWidgetItem;
//        if (0 == pFileInfo->intFileType)
//        {
//            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
//        }
//        else if (1 == pFileInfo->intFileType)
//        {
//            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
//        }
//        pItem->setText(pFileInfo->caFileName);
//        m_pBookListW->addItem(pItem);
//    }
}

void Book::uploadFile() {
    m_strUploadFilePath = QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;
    if (!m_strUploadFilePath.isEmpty())
    {
        //  aa/bb/cc  5  8
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        qDebug() << strFileName;

        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size();  //获得文件大小

        QString strCurPath = TcpClients::getInstance().getCurrentPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        sprintf(pdu->caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);

        TcpClients::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULength);
        free(pdu);
        pdu = NULL;

        m_pTimer->start(1000);
    }
    else
    {
        QMessageBox::warning(this, "Upload File", "The name of your upload file can't be null");
    }
}

void Book::uploadFileData() {
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Upload File", "Fail to Upload File");
        return;
    }

    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while (true)
    {
        ret = file.read(pBuffer, 4096);
        if (ret > 0 && ret <= 4096)
        {
            TcpClients::getInstance().getTcpSocket().write(pBuffer, ret);
        }
        else if (0 == ret)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this, "Upload File", "Fail to Upload File");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;


}

void Book::setDownloadStatus(bool status) {
    m_bDownload = status;
}

bool Book::getDownloadstatus() {
    return m_bDownload;
}

QString Book::getSaveFilePath() {
    return m_strSaveFilePath;
}

void Book::downloadFile() {
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if (NULL == pItem)
    {
        QMessageBox::warning(this, "Download File", "Please choose the file you want to download");
    }
    else
    {
        QString strSaveFilePath = QFileDialog::getSaveFileName();
        if (strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this, "Download File", "Please choose where to save your files");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;
        }

        QString strCurPath = TcpClients::getInstance().getCurrentPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData, strFileName.toStdString().c_str());
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        TcpClients::getInstance().getTcpSocket().write((char*)pdu, pdu->uiMsgLength);
    }
}

