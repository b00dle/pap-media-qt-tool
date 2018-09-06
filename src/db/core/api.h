#ifndef DB_CORE_API_H
#define DB_CORE_API_H

#include <QObject>

#include <QFileInfo>
#include <QList>

#include "sqlite_wrapper.h"

namespace DB {
namespace Core {

/*
 * Class that Provides interface to DB::SqliteWrapper,
 * based on structure of application database.
*/
class Api : public SqliteWrapper
{
    Q_OBJECT
public:
    Api(QString const& db_path, QObject *parent = 0);

    QSqlRelationalTableModel* getSoundFileTable();
    QSqlRelationalTableModel* getCategoryTable();
    QSqlRelationalTableModel* getSoundFileCategoryTable();
    QSqlRelationalTableModel* getResourceDirTable();
    QSqlRelationalTableModel* getImageDirTable();
    QSqlRelationalTableModel* getPresetTable();

    void insertSoundFile(QFileInfo const& info, ResourceDirRecord const& resource_dir);
    void insertCategory(QString const& name, int parent_id = -1);
    void insertSoundFileCategory(int sound_file_id, int category_id);
    void insertResourceDir(QFileInfo const& info);
    void insertImageDir(QFileInfo const& info);
    void insertPreset(QString const& name, QString const& json);

    int getSoundFileId(QString const& path);
    int getResourceDirId(QString const& path);
    int getImageDirId(QString const& path);
    int getPresetId(QString const& name);

    bool soundFileExists(QString const& path, QString const& name);
    bool soundFileCategoryExists(int sound_file_id, int category_id);

    /*
     * Gets a list of ids from table referenced by 'get_table'
     * related to element with id 'have_id' from table referenced by
     * 'have_table'.
    **/
    QList<int> const getRelatedIds(TableIndex get_table, TableIndex have_table, int have_id);

    /*
     * deletes all contents of the database
    */
    void deleteAll();

signals:

public slots:

protected:
    /*
     * gets the index of the table descibing relations between given TableIndexes.
     * Returns NONE if non exists
    */
    TableIndex getRelationTable(TableIndex first, TableIndex second);
};

} // namespace Core
} // namespace DB

#endif // API_H
