#include "mytcpsocket.h"
#include <stdio.h>
#include <mytcpserver.h>
#include <QDebug>
#include <QDir>

#include <iostream>
#include<stdlib.h>
#include<string.h>
#include <QFileInfoList>

#include <QFile>

MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));

    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));

    m_bUpload = false;
    m_pTimer = new QTimer;
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(sendFile()));
}

void MyTcpSocket::recvMsg() {

    if (!m_bUpload) {

        //maybe clash when two data is send at the same time
        qDebug() << this->bytesAvailable();
        uint uiPDULen = 0;
        // first four types will give the size of this sending
        this->read((char*)&uiPDULen, sizeof(uint));
        // acutal size of the sending
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
        //    qDebug() << pdu->uiMsgType << (char*) (pdu->caMsg);

        switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGISTER_REQUEST: {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData + 32, 32);
            //        qDebug() << caName << caPwd << pdu->uiMsgType;
            bool result = OperateDatabase::getInstance().handleRegisteration(caName, caPwd);
            // no more spaces needed
            PDU *respond_pdu = mkPDU(0);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_RESPOND;
            if (result) {
                strcpy(respond_pdu->caData, REGISTER_S);
                //            char command[200];
                // need absolute path, not relative path!
                QString strCommand = QString("mkdir /Users/yanqingyu/Desktop/Project/AllFolders/%1").arg(caName);
                //            strcpy(command, strCommand);
                int result_create_folder = system(strCommand.toStdString().c_str());
                if (result_create_folder == 0) {
                    qDebug() << "sucessfully create folder";
                } else {
                    qDebug() << "fail to create folder";
                }
                //            QDir dir;
                //            qDebug() << "created folders" << dir.mkdir(QString("./%1").arg(caName));
                //            QDir dir;
                //            dir = QDir::root();
                //            qDebug() << "created folders" << dir.mkdir(QString("./%1").arg(caName));

            } else {
                strcpy(respond_pdu->caData, REGISTER_F);
            }

            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;

            break;
        }

        case ENUM_MSG_TYPE_LOGIN_REQUEST: {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData + 32, 32);
            bool result = OperateDatabase::getInstance().handleLogin(caName, caPwd);

            qDebug() << result;

            PDU *respond_pdu = mkPDU(0);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if (result) {
                strcpy(respond_pdu->caData, LOGIN_S);
                m_strName =caName;
            } else {
                strcpy(respond_pdu->caData, LOGIN_F);
            }

            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;

            break;
        }

        case ENUM_MSG_TYPE_LOGOFF_REQUEST: {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);


            OperateDatabase::getInstance().handleOffline(caName);

            PDU *respond_pdu = mkPDU(0);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_LOGOFF_RESPOND;

            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;

            break;
        }

        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST: {

            QStringList result = OperateDatabase::getInstance().handleAllOnline();

            uint uiMsgLength = result.size() * 32;
            PDU *respond_pdu = mkPDU(uiMsgLength);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;

            for (int i=0; i < result.size(); i++) {
                memcpy((char*) (respond_pdu->caMsg) + i*32, result.at(i).toStdString().c_str(), result.at(i).size());
                qDebug() << respond_pdu->caMsg;
            }



            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;

            break;
        }

        case ENUM_MSG_TYPE_ALL_SEARCH_USER_REQUEST: {
            int temp = OperateDatabase::getInstance().handleSearchUser(pdu->caData);
            PDU *respond_pdu = mkPDU(0);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ALL_SEARCH_USER_RESPOND;
            if (temp == -1) {
                strcpy(respond_pdu->caData, SEARCH_USER_NOT_FOUND);
            } else if (temp == 1) {
                strcpy(respond_pdu->caData, SEARCH_USER_ONLINE);
            } else if (temp == 0) {
                strcpy(respond_pdu->caData, SEARCH_USER_OFFLINE);
            }

            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;

            break;

        }

        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: {
            //        char caFriName[32] = {'\0'};
            //        char caMyName[32] = {'\0'};
            //        strncpy(caFriName, pdu->caData, 32);
            //        strncpy(caMyName, pdu->caData + 32, 32);
            //        int result = OperateDatabase::getInstance().handleAddFriend(caFriName, caMyName);
            //        PDU *respond_pdu = NULL;
            //        if (result == -1) {
            //            respond_pdu = mkPDU(0);
            //            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            //            strcpy(respond_pdu->caData, UNKNOWN_ERROR);
            //            write((char*)respond_pdu, respond_pdu->uiPDULength);
            //            free(respond_pdu);
            //            respond_pdu = NULL;
            //        } else if (result == 0) {
            //            respond_pdu = mkPDU(0);
            //            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            //            strcpy(respond_pdu->caData, ADD_FRIEND_ALREADY_EXISTED);
            //            write((char*)respond_pdu, respond_pdu->uiPDULength);
            //            free(respond_pdu);
            //            respond_pdu = NULL;
            //        } else if (result == 1) {
            //            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST; // deleted
            //            MyTcpServer::getInstanceObject().share(caFriName, pdu);

            //        } else if (result == 2) {
            //            respond_pdu = mkPDU(0);
            //            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            //            strcpy(respond_pdu->caData, ADD_FRIEND_OFFLINE);
            //            write((char*)respond_pdu, respond_pdu->uiPDULength);
            //            free(respond_pdu);
            //            respond_pdu = NULL;
            //        } else if (result == 3) {
            //            respond_pdu = mkPDU(0);
            //            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            //            strcpy(respond_pdu->caData, ADD_FRIEND_NOT_EXISTED);
            //            write((char*)respond_pdu, respond_pdu->uiPDULength);
            //            free(respond_pdu);
            //            respond_pdu = NULL;
            //        }
            char caFriName[32] = {'\0'};
            char caMyName[32] = {'\0'};
            strncpy(caFriName, pdu->caData, 32);
            strncpy(caMyName, pdu->caData+32, 32);
            int ret = OperateDatabase::getInstance().handleAddFriend(caFriName, caMyName);
            PDU *respdu = NULL;
            if (-1 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, UNKNOWN_ERROR);
                write((char*)respdu, respdu->uiPDULength);
                free(respdu);
                respdu = NULL;
            }
            else if (0 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_ALREADY_EXISTED);
                write((char*)respdu, respdu->uiPDULength);
                free(respdu);
                respdu = NULL;
            }
            else if (1 == ret)
            {
                MyTcpServer::getInstanceObject().share(caFriName, pdu);
            }
            else if (2 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
                write((char*)respdu, respdu->uiPDULength);
                free(respdu);
                respdu = NULL;
            }
            else if (3 == ret)
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, ADD_FRIEND_NOT_EXISTED);
                write((char*)respdu, respdu->uiPDULength);
                free(respdu);
                respdu = NULL;
            }

            break;
        }

        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST: {
            QDir dir;
            QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
            //        qDebug() << strCurPath;
            bool result = dir.exists(QString(strCurPath));
            PDU* respond_pdu = NULL;
            //        qDebug() << result;
            if (result) {       // direction exists
                char caNewDir[32] = {'\0'};
                memcpy(caNewDir, pdu->caData + 32, 32);
                QString strNewPath = strCurPath + "/" + caNewDir;
                //            qDebug() << strNewPath << result;
                result = dir.exists(strNewPath);
                qDebug() << result;
                if (result) {       // direction already exist
                    respond_pdu = mkPDU(0);
                    respond_pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respond_pdu->caData, FILE_NAME_ALREADY_EXIST);
                    write((char*) respond_pdu, respond_pdu->uiPDULength);
                    free(respond_pdu);
                    respond_pdu = NULL;
                } else {            // direction does not exist
                    QString strCommand = QString("mkdir " + strNewPath);
                    int result_create_folder = system(strCommand.toStdString().c_str());
                    if (result_create_folder == 0) {
                        qDebug() << "sucessfully create folder";
                    } else {
                        qDebug() << "fail to create folder";
                    }

                    respond_pdu = mkPDU(0);
                    respond_pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respond_pdu->caData, CREATE_DIR_S);
                    write((char*) respond_pdu, respond_pdu->uiPDULength);
                    free(respond_pdu);
                    respond_pdu = NULL;
                }
            } else {            // direction does not exist
                respond_pdu = mkPDU(0);
                respond_pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respond_pdu->caData, DIR_NOT_EXIST);
                write((char*) respond_pdu, respond_pdu->uiPDULength);
                free(respond_pdu);
                respond_pdu = NULL;

            }

            break;
        }


        case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            //        char caFriName[32] = {'\0'};
            //        char caMyName[32] = {'\0'};
            //        strncpy(caFriName,pdu->caData,32);
            //        strncpy(caMyName,pdu->caData+32,32);
            //        OperateDatabase::getInstance().handleAddFriend(caFriName,caMyName);
            //        MyTcpServer::getInstanceObject().share(caMyName,pdu);
            //        break;

            char caFriName[32] = {'\0'};
            char caMyName[32] = {'\0'};
            strncpy(caFriName, pdu->caData, 32);
            strncpy(caMyName, pdu->caData+32, 32);
            OperateDatabase::getInstance().handleAddFriendAgree(caFriName, caMyName);

            MyTcpServer::getInstanceObject().share(caMyName, pdu);
            break;
        }

        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            //        char caName[32]= {'\0'};
            //        strncpy(caName,pdu->caData+32,32);
            //        MyTcpServer::getInstanceObject().share(caName,pdu);
            //        break;
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData+32, 32);
            MyTcpServer::getInstanceObject().share(caName, pdu);
            break;
        }

        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST: {
            char caName[32]= {'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList result = OperateDatabase::getInstance().handleFlushFriend(caName);
            uint uiMsgLen = result.size() * 32;
            PDU *respond_pdu = mkPDU(uiMsgLen);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for (int i = 0; i < result.size(); i++) {
                memcpy((char*) (respond_pdu->caMsg) + i * 32, result.at(i).toStdString().c_str(), result.at(i).size());
            }
            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;
            break;
        }

        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST: {
            char caChatName[32] = {'\0'};
            memcpy(caChatName, pdu->caData + 32, 32);
            qDebug() << caChatName;
            MyTcpServer::getInstanceObject().share(caChatName,pdu);

            break;
        }

        case ENUM_MSG_TYPE_REFRESH_FILE_REQUEST: {
            char* pCurPath = new char[pdu->uiMsgLength];
            memcpy(pCurPath, pdu->caMsg, pdu->uiMsgLength);
            //        qDebug() << "reached!";
            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList();
            int iFileCount = fileInfoList.size();
            PDU* respond_pdu = mkPDU(sizeof(FileInfo) * iFileCount);
            respond_pdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_FILE_RESPOND;
            FileInfo *pFileInfo = NULL;
            QString strFileName;

            for(int i = 0; i < iFileCount; i++) {
                pFileInfo = (FileInfo*) (respond_pdu->caMsg) + i;
                strFileName = fileInfoList[i].fileName();
                memcpy(pFileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.size());
                if (fileInfoList[i].isDir()) {
                    pFileInfo->intFileType = 0;     // represents folder
                } else if (fileInfoList[i].isFile()) {
                    pFileInfo->intFileType = 1;     // represents file
                }

                //            qDebug() << "reached!";
                //            qDebug() << fileInfoList[i].fileName() << fileInfoList[i].size()
                //                     << "Folder:" << fileInfoList[i].isDir() << "File:" << fileInfoList[i].isFile();
            }

            write((char*)respond_pdu, respond_pdu->uiPDULength);
            free(respond_pdu);
            respond_pdu = NULL;

            break;
        }

        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST: {
            char caFileName[32] = {'\0'};
            qint64 fileSize = 0;
            sscanf(pdu->caData, "%s %lld", caFileName, &fileSize);
            char *pPath = new char[pdu->uiMsgLength];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLength);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() << strPath;
            delete []pPath;
            pPath = NULL;

            m_file.setFileName(strPath);

            if (m_file.open(QIODevice::WriteOnly))
            {
                m_bUpload = true;
                m_iTotal = fileSize;
                m_iReceived = 0;
            }

            break;
        }

        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST: {
            char caFileName[32] = {'\0'};
            strcpy(caFileName, pdu->caData);
            char *pPath = new char[pdu->uiMsgLength];
            memcpy(pPath, pdu->caMsg, pdu->uiMsgLength);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug() << strPath;
            delete []pPath;
            pPath = NULL;

            QFileInfo fileInfo(strPath);
            qint64 fileSize = fileInfo.size();
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            sprintf(respdu->caData, "%s %lld", caFileName, fileSize);

            write((char*)respdu, respdu->uiMsgLength);
            free(respdu);
            respdu = NULL;

            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_pTimer->start(1000);

            break;
        }
        default:
            break;
        }

        free(pdu);
        pdu = NULL;
    } else {
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;

        QByteArray buff = readAll();
        m_file.write(buff);
        m_iReceived += buff.size();
        if (m_iTotal == m_iReceived)
        {
            m_file.close();
            m_bUpload = false;

            strcpy(respdu->caData, UPLOAD_FILE_S);
            write((char*)respdu, respdu->uiPDULength);
            free(respdu);
            respdu = NULL;
        }
        else if (m_iTotal < m_iReceived)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData, UPLOAD_FILE_F);

            write((char*)respdu, respdu->uiPDULength);
            free(respdu);
            respdu = NULL;
        }
    }
}

void MyTcpSocket::clientOffline() {
    OperateDatabase::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);

}

QString MyTcpSocket::getName() {
    return m_strName;
}

void MyTcpSocket::sendFile() {
    char* pData = new char[4069];
    qint64 result = 0;
    while(true) {
        result = m_file.read(pData, 4096);
        if (result > 0 && result <= 4096) {
            write(pData, result);
        } else if (result == 0) {
            m_file.close();
            break;
        } else if (result < 0) {
            m_file.close();
            qDebug() << "Something is wrong, fail to send the file to the client from the server";
        }
    }
}
