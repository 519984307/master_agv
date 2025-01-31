﻿#include "asmorderdb.h"
#include <QVariant>
#include "comm/config/cfgsetting.h"
#include "comm/codeDecode/simplecrypt.h"
#include <QSqlError>
#include <QMutex>
#include <QMutexLocker>
#include <QSqlQuery>
#include <QThread>


#define LOGD(MSG) qDebug() << MSG

STATEMENTINSTANCE(ASMOrderDB)

ASMOrderDB::ASMOrderDB()
{
//    pThis = this;
    PTHISSETTHIS
    getDatebaseConnectPara();
    testConnect = "SELECT 1";
    maxWaitTime  = 1000;
    waitInterval = 200;
    maxConnectionCount  = 1000;
    type = "ASMOrderDB";
}

//ASMOrderDB *ASMOrderDB::getInstace()
//{
//    if (nullptr == pThis) {
//        QMutexLocker locker(mutex);
//        if (pThis == nullptr) {
//            pThis = new ASMOrderDB();
//        }
//    }
//    return pThis;
//}

void ASMOrderDB::getDatebaseConnectPara()
{
    bool isOk = false;
    QVariant value;
    //hostName
    if(setting::getInstance()->getSettingValueInterface("ASMOrderDB/DataBaseHost", value))
    {
        hostName = value.toString();
        qDebug()<<"hostName"<<hostName;
    }
    else
    {
        LOGD("get ASMOrderDB DataBaseHost error");
        hostName = "127.0.0.1";
    }
    //dbName
    if(setting::getInstance()->getSettingValueInterface("ASMOrderDB/DataBaseName", value))
    {
        datebaseName = value.toString();
        qDebug()<<"datebaseName"<<datebaseName;
    }
    else
    {
        LOGD("get ASMOrderDB DataBaseName error");
        datebaseName = "ASM_AGV";
    }
    //uid
    if(setting::getInstance()->getSettingValueInterface("ASMOrderDB/DataBaseUseName", value))
    {
        userName = value.toString();
        SimpleCrypt crypt(decodeKey);
        userName = crypt.decryptToString(userName);
    }
    else
    {
        LOGD("get ASMOrderDB DataBaseUseName error");
        userName = "sa";
    }
    //pwd
    if(setting::getInstance()->getSettingValueInterface("ASMOrderDB/DataBasePassword", value))
    {
        password = value.toString();
        SimpleCrypt crypt(decodeKey);
        password = crypt.decryptToString(password);
    }
    else
    {
        LOGD("get ASMOrderDB DataBasePassword error");
        password = "123456";
    }
}

QSqlDatabase ASMOrderDB::createConnection(QString ConnectName)
{
    if(QSqlDatabase::contains(ConnectName))
    {
        QSqlDatabase db =  QSqlDatabase::database(ConnectName);

        // 返回连接前访问数据库，如果连接断开，重新建立连接
        QSqlQuery query(db);
        query.exec(testConnect);

        if (query.lastError().type() != QSqlError::NoError && db.open()) {
            qDebug() << "Open datatabase error:" << db.lastError().text();
            return QSqlDatabase();
        }
        return db;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", ConnectName);
    db.setDatabaseName(QString("DRIVER={SQL SERVER};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "UID=%3;"
                               "PWD=%4;").arg(hostName)
                       .arg(datebaseName)
                       .arg(userName)
                       .arg(password));

    if (!db.open()) {
        qDebug() << "Open datatabase error:" << db.lastError().text();
        return QSqlDatabase();
    }
    return db;
}
