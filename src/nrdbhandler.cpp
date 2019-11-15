/********************************************************************************
 *   Copyright (C) 2010-2018 by NetResults S.r.l. ( http://www.netresults.it )  *
 *   Author(s):                                                                 *
 *              Francesco Lamonica      <f.lamonica@netresults.it>              *
 ********************************************************************************/

#include "nrdbhandler.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlResult>
#include <QFile>
#include <QDebug>
#include <QVariant>

#include <UniqLogger.h>

//included for debug statements in ctor
#include <QCoreApplication>
#include <QStringList>


NrBaseDbHandler::NrBaseDbHandler(const DbhConfig &i_dbconf, QObject *parent)
    : QObject(parent)
    , m_DbConf(i_dbconf)
{

    UniqLogger *ul = UniqLogger::instance(m_DbConf.logId);

    m_logger = ul->createFileLogger("DB", m_DbConf.logFilename, m_DbConf.logwriterConfig);
    m_logger->setVerbosityAcceptedLevel(m_DbConf.logLevel);

    //check plugins path
    foreach (QString path, QCoreApplication::libraryPaths())
        *m_logger << UNQL::LOG_INFO << "library path: " << path << UNQL::eom;
    foreach (QString plugin, QSqlDatabase::drivers())
        *m_logger << UNQL::LOG_INFO << "library path: " << plugin << UNQL::eom;

    _M_db = QSqlDatabase::addDatabase(m_DbConf.dbType, m_DbConf.dbConnectionName);

    _M_db.setHostName(m_DbConf.dbHost);
    _M_db.setPort(m_DbConf.dbPort);
    _M_db.setDatabaseName(m_DbConf.dbName);
    _M_db.setUserName(m_DbConf.dbUser);
    _M_db.setPassword(m_DbConf.dbPass);
}



/*!
  \brief opens a connection towards the already configured Database
  \return true if the DB is opened successfully, false otherwise
  */
bool
NrBaseDbHandler::openDbConn()
{
    bool ok = true;
    /* qDebug() << "OpenDbConn() -- db is open (" << _M_db.isOpen() <<") -- db is valid ("
            << _M_db.isValid() <<") -- db is openError ("<< _M_db.isOpenError() <<")"; */

    if ( !_M_db.isOpen() || !_M_db.isValid() ) {
        if (_M_db.isOpen()) {
            _M_db.close();
            *m_logger << UNQL::LOG_CRITICAL << "Database was INVALID but OPEN! -> Closing" << UNQL::eom;
        }
        ok = _M_db.open();
        if (!ok) {
            *m_logger << UNQL::LOG_CRITICAL << "------ DB ERROR: " <<  _M_db.lastError().text() << UNQL::eom;
        } else {
            QSqlQuery q("set timezone to '" + m_DbConf.dbTimeZone + "'", _M_db);
            Q_UNUSED(q);
        }
        //TODO: what if the connection fails???
    }
    return ok;
}



/*!
  \brief Closes the connection that the QSqlDatabase may have opened
  */
void
NrBaseDbHandler::closeDbConn()
{
    /* qDebug() << "closeDbConn() -- db is open (" << _M_db.isOpen() <<") -- db is valid ("
            << _M_db.isValid() <<") -- db is openError ("<< _M_db.isOpenError() <<")"; */

    if ( !_M_db.isOpen() ) {        
        *m_logger << UNQL::LOG_CRITICAL << "Database was NOT OPEN! -> Closing anyway" << UNQL::eom;
    }

    _M_db.close();
}


/*!
  \brief a commodity function to create a new query for current connection
  */
QSqlQuery
NrBaseDbHandler::createNewQuery()
{
    return QSqlQuery(_M_db);
}


bool
NrBaseDbHandler::prepareQuery(QSqlQuery &query, const QString &sql)
{
    bool ok = query.prepare(sql);
    if(!ok) {
        *m_logger << UNQL::LOG_CRITICAL << "PREPARE QUERY ERROR - got error: " << query.lastError().text() << " for query (" << query.lastQuery() << ")" << UNQL::eom;
    }

    return ok;
}

/*!
  \brief a commodity function to call a prepared query and log the results
  \param query the query to be executed;
  */
bool
NrBaseDbHandler::executeQuery(QSqlQuery &query)
{
    bool ok = query.exec();
    if(!ok) {        
        *m_logger << UNQL::LOG_CRITICAL << "EXECUTE QUERY ERROR - got error: " << query.lastError().text() << " for query (" << query.lastQuery() << ")" << UNQL::eom;
    }

    return ok;
}



/*!
    \brief a simple test method to check whether the DB is reachable and user can login
    \return true if openDbConn() is successful and, in case Db type is SQLITE, if Db file exists; false otherwise
  */
bool
NrBaseDbHandler::testConnection()
{
    if (m_DbConf.dbType == NRDBHANDLER::SQLITE && !QFile::exists(m_DbConf.dbName))
    {
        return false;
    }
    if (!this->openDbConn())
    {
        return false;
    }
    this->closeDbConn();
    return true;
}



void
NrBaseDbHandler::rollbackAndClose(const QString &notice)
{
    *m_logger << UNQL::LOG_CRITICAL << "Query" << notice << "failed... rolling back transaction" << UNQL::EOM;
    _M_db.rollback();
    closeDbConn();
}



void
NrBaseDbHandler::commitAndClose(const QString &notice)
{
    bool dbact = _M_db.commit();
    if (!dbact) {
        *m_logger << UNQL::LOG_CRITICAL << "Transaction " << notice << " commit failed" << UNQL::EOM;
    }

    closeDbConn();
}

