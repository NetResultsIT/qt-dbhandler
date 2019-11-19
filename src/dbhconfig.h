#ifndef DBHCONFIG_H
#define DBHCONFIG_H

#include <UniqLogger.h>

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

    explicit DbhConfig()
        : dbType("")
        , dbConnectionName("")
        , dbUser("")
        , dbPass("")
        , dbName("")
        , dbHost("")
        , dbPort(0)
        , logId("NRDBH")
        , logFilename("NRDBH.log")
        , logLevel(UNQL::LOG_INFO)
    {
    }

    explicit DbhConfig(const QString &i_dbHost, const QString &i_dbName, const QString &i_dbUser="", const QString &i_dbPass="", const QString &i_dbType=NRDBHANDLER::PSQL, quint16 i_dbPort=5432)
        : dbType(i_dbType)
        , dbConnectionName()
        , dbUser(i_dbUser)
        , dbPass(i_dbPass)
        , dbName(i_dbName)
        , dbHost(i_dbHost)
        , dbPort(i_dbPort)
        , logId("NRDBH")
        , logFilename("NRDBH.log")
        , logLevel(UNQL::LOG_INFO)
    {
    }

    bool isValid() const { return !dbType.isEmpty() && !dbName.isEmpty() && (dbType == NRDBHANDLER::SQLITE ? true : !dbHost.isEmpty()); }

};

#endif // DBHCONFIG_H
