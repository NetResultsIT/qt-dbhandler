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

#include <UniqLogger.h>

class Logger;

namespace NRDBHANDLER {
    const QString SQLITE = "QSQLITE";
    const QString PSQL = "QPSQL";
    const QString MYSQL = "QMYSQL";
};

class DbhConfig
{

public:
    //DB Stuff
    QString dbType, dbConnectionName, dbTimeZone;
    QString dbUser, dbPass, dbName, dbHost;
    int dbPort;

    //Logging stuff
    QString logId, logFilename;
    WriterConfig logwriterConfig;
    UNQL::LogMessagePriorityType logLevel;

    explicit DbhConfig(const QString &i_dbHost, const QString &i_dbName, const QString &i_dbUser="", const QString &i_dbPass="")
        : dbType(NRDBHANDLER::PSQL)
        , dbConnectionName(QLatin1String( QSqlDatabase::defaultConnection ) )
        , dbUser(i_dbUser)
        , dbPass(i_dbPass)
        , dbName(i_dbName)
        , dbHost(i_dbHost)
        , dbPort(5432)
        , logId("NRDBH")
        , logFilename("NRDBH.log")
        , logLevel(UNQL::LOG_INFO)
    {

    }
};


class NrBaseDbHandler : public QObject
{
    Q_OBJECT

protected:
    DbhConfig m_DbConf;
    QSqlDatabase _M_db;
    Logger *m_logger;

    bool openDbConn();
    void closeDbConn();
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

    inline const QString dbHost() const { return m_DbConf.dbHost; }
    inline const QString dbName() const { return m_DbConf.dbName; }
    inline const QString dbUsername() const { return m_DbConf.dbUser; }
    inline const QString dbPassword() const { return m_DbConf.dbPass; }
    inline const QString dbTimezone() const { return m_DbConf.dbTimeZone; }
    inline const QString connectionName() const { return m_DbConf.dbConnectionName; }
    inline int dbPort() const { return m_DbConf.dbPort; }
};

#endif // DBHANDLER_H
