﻿/*
    This file is part of JQLibrary

    Copyright: Jason

    Contact email: 188080501@qq.com

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 2.1 or version 3 as published by the Free
    Software Foundation and appearing in the file LICENSE.LGPLv21 and
    LICENSE.LGPLv3 included in the packaging of this file. Please review the
    following information to ensure the GNU Lesser General Public License
    requirements will be met: https://www.gnu.org/licenses/lgpl.html and
    http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef __JQHttpServer_h__
#define __JQHttpServer_h__

#ifndef QT_NETWORK_LIB
#   error("Plwaer add network in pro file")
#endif

#ifndef QT_CONCURRENT_LIB
#   error("Plwaer add concurrent in pro file")
#endif

// C++ lib import
#include <functional>

// Qt lib import
#include <QObject>
#include <QSharedPointer>
#include <QPointer>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QMutex>
#include <QHostAddress>

class QTcpServer;
class QTcpSocket;
class QThreadPool;
class QHostAddress;
class QTimer;

namespace JQHttpServer
{

class Session: public QObject
{
    Q_OBJECT

public:
    Session( const QPointer< QTcpSocket > &tcpSocket );

    ~Session();

    inline void setHandleAcceptedCallback(const std::function< void(const QPointer< Session > &) > &callback)
    { handleAcceptedCallback_ = callback; }

    inline QString requestMethodToken() const { return requestMethodToken_; }

    inline QString requestUrl() const { return requestUrl_; }

    inline QString requestCrlf() const { return requestCrlf_; }

    inline QMap< QString, QString > headersData() const { return headersData_; }

    inline QByteArray requestRawData() const { return requestRawData_; }

public slots:
    void replyText(const QString &replyData);

private:
    void inspectionBufferSetup1();

    void inspectionBufferSetup2();

private:
    QPointer< QTcpSocket > tcpSocket_;
    std::function< void(const QPointer< Session > &) > handleAcceptedCallback_;
    QSharedPointer< QTimer > timerForClose_;

    QByteArray buffer_;

    QString requestMethodToken_;
    QString requestUrl_;
    QString requestCrlf_;

    QMap< QString, QString > headersData_;
    bool headerAcceptedFinish_ = false;

    QByteArray requestRawData_;

    qint64 waitWrittenByteCount_ = 0;
};

class Manage: public QObject
{
    Q_OBJECT

public:
    Manage();

    ~Manage();

    inline void setHttpAcceptedCallback(const std::function< void(const QPointer< Session > &session) > &httpAcceptedCallback)
    { httpAcceptedCallback_ = httpAcceptedCallback; }

    inline bool isListening() const { return !tcpServer_.isNull(); }

public slots:
    bool listen(const QHostAddress &address, const quint16 &port);

    void close();

private:
    bool startTcpSocketThread(const QHostAddress &address, const quint16 &port);

    void stopHandleThread();

    void stopTcpSocketThread();

    void newSession(const QPointer< Session > &session);

    void handleAccepted(const QPointer< Session > &session);

signals:
    void readyToClose();

private:
    QSharedPointer< QThreadPool > handleThreadPool_;
    QSharedPointer< QThreadPool > tcpSocketThreadPool_;
    QPointer< QTcpServer > tcpServer_;

    QMutex mutex_;

    std::function< void(const QPointer< Session > &session) > httpAcceptedCallback_;

    QSet< Session * > availableSessions_;
};

}

#endif//__JQHttpServer_h__
