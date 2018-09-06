#ifndef DB_SQLITE_WRAPPER_H
#define DB_SQLITE_WRAPPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlRecord>

#include "db/table_records.h"

namespace DB {
namespace Core {

/*
 * Class that can establish and manage connection to a Sqlite database.
 * Provides low-level access to data contained in db.
*/
class SqliteWrapper : public QObject
{
    Q_OBJECT
public:
    SqliteWrapper(QString const& db_path, QObject* parent = 0);

    /* Get a QSqlTableModel of a databse table identified by given TableIndex */
    QSqlRelationalTableModel* getTable(TableIndex index);

    /* Perform Select query with */
    QList<QSqlRecord> const selectQuery(QString const& SELECT, QString const& FROM, QString const& WHERE = "");
    QList<QSqlRecord> const selectQuery(QString const& SELECT, TableIndex FROM, QString const& WHERE = "");

    void insertQuery(TableIndex index, QString const& value_block);

    void deleteQuery(TableIndex index, QString const& WHERE);

    void open();
    void close();

    /* returns a safe version of given string as string value for sql query */
    static QString const escape(QString const& str);

protected:
    void initDB(QString const&);

    QList<QSqlRecord> const executeQuery(QString const&);

    QSqlDatabase db_;
};

} // namespace Core
} // namespace DB

#endif // DB_SQLITE_WRAPPER_H
