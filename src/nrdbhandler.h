/********************************************************************************
 *   Copyright (C) 2010-2018 by NetResults S.r.l. ( http://www.netresults.it )  *
 *   Author(s):                                                                 *
 *              Francesco Lamonica      <f.lamonica@netresults.it>              *
 ********************************************************************************/

#ifndef NR_BASE_DBHANDLER_H
#define NR_BASE_DBHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include "dbhconfig.h"

class Logger;

class NrBaseDbHandler : public QObject
{
    Q_OBJECT

public:
    enum ErrorType
    {
        ERROR_NONE = 0,
        ERROR_OPEN,
        ERROR_COMMIT,
        ERROR_QUERY_PREPARE,
        ERROR_QUERY_EXEC
    };

private:
    void setError(ErrorType errorType, const QString &errorString);
    void resetError();

protected:
    DbhConfig m_DbConf;
    QSqlDatabase _M_db;
    Logger *m_logger;
    QString m_errorString;
    ErrorType m_errorType;

protected:
    bool openDbConn();
    void closeDbConn();
    QSqlQuery createNewQuery();
    bool prepareQuery(QSqlQuery &query, const QString &sql);
    bool executeQuery(QSqlQuery &q);
    void rollbackAndClose(const QString &notice="");
    void commitAndClose(const QString &notice="");

public:
    explicit NrBaseDbHandler(const DbhConfig& dbconf, QObject *parent = nullptr);

    virtual ~NrBaseDbHandler()
    {
        QSqlDatabase::removeDatabase(m_DbConf.dbConnectionName);
    }

    bool testConnection();

    inline const QString& dbHost() const { return m_DbConf.dbHost; }
    inline const QString& dbName() const { return m_DbConf.dbName; }
    inline const QString& dbUsername() const { return m_DbConf.dbUser; }
    inline const QString& dbPassword() const { return m_DbConf.dbPass; }
    inline const QString& dbTimezone() const { return m_DbConf.dbTimeZone; }
    inline const QString& connectionName() const { return m_DbConf.dbConnectionName; }
    inline int dbPort() const { return m_DbConf.dbPort; }
    inline bool hasError() const { return m_errorType != ERROR_NONE; }
    inline ErrorType getErrorType() const { return m_errorType; }
    inline QString getErrorString() const { return m_errorString; }
};

#endif // DBHANDLER_H
