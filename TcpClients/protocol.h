#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGISTER_S "Successfully resgitered"
#define REGISTER_F "Failed to register: Username Already Existed"

#define LOGIN_S "Successfully log in"
#define LOGIN_F "Either Your Username or Passowrd is Incorrect"

#define SEARCH_USER_NOT_FOUND "User not found"
#define SEARCH_USER_ONLINE "User is online"
#define SEARCH_USER_OFFLINE "User is offline"

#define UNKNOWN_ERROR "Unknown error"
#define ADD_FRIEND_ALREADY_EXISTED "Friend already existed"
#define ADD_FRIEND_OFFLINE "User is offline"
#define ADD_FRIEND_NOT_EXISTED "User not existed"

#define DIR_NOT_EXIST "Directionary does not exist"
#define FILE_NAME_ALREADY_EXIST "File name already exist"
#define CREATE_DIR_S "Successfully created!"

#define UPLOAD_FILE_S "Upload file successfully!"
#define UPLOAD_FILE_F "Fail to upload file"

enum ENUM_MSG_TYPE {
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGISTER_REQUEST,     // the register request
    ENUM_MSG_TYPE_REGISTER_RESPOND,     // the resgister respond

    ENUM_MSG_TYPE_LOGIN_REQUEST,
    ENUM_MSG_TYPE_LOGIN_RESPOND,

    ENUM_MSG_TYPE_LOGOFF_REQUEST,
    ENUM_MSG_TYPE_LOGOFF_RESPOND,

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,

    ENUM_MSG_TYPE_ALL_SEARCH_USER_REQUEST,
    ENUM_MSG_TYPE_ALL_SEARCH_USER_RESPOND,

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,

    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,

    ENUM_MSG_TYPE_REFRESH_FILE_REQUEST,
    ENUM_MSG_TYPE_REFRESH_FILE_RESPOND,

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,





    ENUM_MSG_TYPE_MAX = 0x00ffffff,
};

struct PDU {
    uint uiPDULength;       // data length (all)
    uint uiMsgType;         // type of the message
    char caData[64];        // actual message size
    uint uiMsgLength;       // actual message length
    int caMsg[];            // actual message, used this array to get the messsage
};

PDU *mkPDU(uint uiMsgLength);

struct FileInfo {
    char caFileName[32];
    int intFileType;
};

#endif // PROTOCOL_H
