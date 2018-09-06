#include "api.h"

namespace DB {
namespace Core {

Api::Api(QString const& db_path, QObject *parent)
    : SqliteWrapper(db_path, parent)
{}

QSqlRelationalTableModel *Api::getSoundFileTable()
{
    return getTable(SOUND_FILE);
}

QSqlRelationalTableModel *Api::getCategoryTable()
{
    return getTable(CATEGORY);
}

QSqlRelationalTableModel *Api::getSoundFileCategoryTable()
{
    return getTable(SOUND_FILE_CATEGORY);
}

QSqlRelationalTableModel *Api::getResourceDirTable()
{
    return getTable(RESOURCE_DIRECTORY);
}

QSqlRelationalTableModel *Api::getImageDirTable()
{
    return getTable(IMAGE_DIRECTORY);
}

QSqlRelationalTableModel *Api::getPresetTable()
{
    return getTable(PRESET);
}

void Api::insertSoundFile(const QFileInfo &info, ResourceDirRecord const& resource_dir)
{
    QString rel_path = info.filePath();
    rel_path.remove(0, resource_dir.path.size());

    QString value_block  = "";
    value_block = "(name, path, relative_path) VALUES (";
    value_block += "'" + SqliteWrapper::escape(info.fileName()) + "','";
    value_block += SqliteWrapper::escape(info.filePath()) + "','";
    value_block += SqliteWrapper::escape(rel_path) + "')";

    insertQuery(SOUND_FILE, value_block);
}

void Api::insertCategory(const QString &name, int parent_id)
{
    QString value_block  = "";
    if(parent_id != -1) {
        value_block = "(name, parent_id) VALUES (";
        value_block += "'" + SqliteWrapper::escape(name) + "'," + QString::number(parent_id) + ")";
    }
    else {
        value_block = "(name) VALUES ('" + SqliteWrapper::escape(name) + "')";
    }
    insertQuery(CATEGORY, value_block);
}

void Api::insertSoundFileCategory(int sound_file_id, int category_id)
{
    QString value_block  = "";
    value_block = "(sound_file_id, category_id) VALUES (";
    value_block += QString::number(sound_file_id) + ",";
    value_block += QString::number(category_id) + ")";

    insertQuery(SOUND_FILE_CATEGORY, value_block);
}

void Api::insertResourceDir(const QFileInfo &info)
{
    QString value_block  = "";
    value_block = "(name, path) VALUES (";
    value_block += "'" + SqliteWrapper::escape(info.fileName()) + "','";
    value_block += SqliteWrapper::escape(info.filePath()) + "')";

    insertQuery(RESOURCE_DIRECTORY, value_block);
}

void Api::insertImageDir(const QFileInfo &info)
{
    QString value_block  = "";
    value_block = "(path) VALUES ";
    value_block += "('" + SqliteWrapper::escape(info.filePath()) + "')";

    insertQuery(IMAGE_DIRECTORY, value_block);
}

void Api::insertPreset(const QString &name, const QString &json)
{
    QString value_block  = "";
    value_block = "(name, json) VALUES (";
    value_block += "'" + SqliteWrapper::escape(name) + "','";
    value_block += SqliteWrapper::escape(json) + "')";

    insertQuery(PRESET, value_block);
}

int Api::getSoundFileId(const QString &path)
{
    QString SELECT = "id";
    QString WHERE = "path = '" + SqliteWrapper::escape(path) + "'";
    QList<QSqlRecord> res = selectQuery(SELECT, SOUND_FILE, WHERE);
    if(res.size() > 0)
        return res[0].value(0).toInt();
    return -1;
}

int Api::getResourceDirId(const QString &path)
{
    QString SELECT = "id";
    QString WHERE = "path = '" + SqliteWrapper::escape(path) + "'";
    QList<QSqlRecord> res = selectQuery(SELECT, RESOURCE_DIRECTORY, WHERE);
    if(res.size() > 0)
        return res[0].value(0).toInt();
    return -1;
}

int Api::getImageDirId(const QString &path)
{
    QString SELECT = "id";
    QString WHERE = "path = '" + SqliteWrapper::escape(path) + "'";
    QList<QSqlRecord> res = selectQuery(SELECT, IMAGE_DIRECTORY, WHERE);
    if(res.size() > 0)
        return res[0].value(0).toInt();
    return -1;
}

int Api::getPresetId(const QString &name)
{
    QString SELECT = "id";
    QString WHERE = "name = '" + SqliteWrapper::escape(name) + "'";
    QList<QSqlRecord> res = selectQuery(SELECT, PRESET, WHERE);
    if(res.size() > 0)
        return res[0].value(0).toInt();
    return -1;
}

bool Api::soundFileExists(const QString &path, const QString &name)
{
    QString where = "path = '" + SqliteWrapper::escape(path) + "' and ";
    where += "name = '" + SqliteWrapper::escape(name) + "'";

    return selectQuery("Count(*)", SOUND_FILE, where)[0].value(0).toInt() > 0;
}

bool Api::soundFileCategoryExists(int sound_file_id, int category_id)
{
    QString where = "sound_file_id = " + QString::number(sound_file_id) + " and ";
    where += "category_id = " + QString::number(category_id) + "";

    return selectQuery("Count(*)", SOUND_FILE_CATEGORY, where)[0].value(0).toInt() > 0;
}

const QList<int> Api::getRelatedIds(TableIndex get_table, TableIndex have_table, int have_id)
{
    QList<int> ids;
    if(have_table == NONE || get_table == NONE)
        return ids;

    TableIndex relation_idx = getRelationTable(get_table, have_table);
    if(relation_idx == NONE)
        return ids;

    QString SELECT = toString(get_table) + "_id";
    QString FROM = toString(relation_idx);
    QString WHERE = toString(have_table) + "_id = " +  QString::number(have_id);

    foreach(QSqlRecord rec, selectQuery(SELECT, FROM, WHERE))
        ids.append(rec.value(0).toInt());

    return ids;
}

void Api::deleteAll()
{
    // delete sound_files
    deleteQuery(SOUND_FILE, "id > 0");

    // delete categories
    deleteQuery(CATEGORY, "id > 0");

    // delete sound_file_categories
    deleteQuery(SOUND_FILE_CATEGORY, "id > 0");

    // delete resource_dirs
    deleteQuery(RESOURCE_DIRECTORY, "id > 0");

    // delete presets
    deleteQuery(PRESET, "id > 0");

    // delete image_dirs
    deleteQuery(IMAGE_DIRECTORY, "id > 0");
}

TableIndex Api::getRelationTable(TableIndex first, TableIndex second)
{
    if(first == second)
        return NONE;

    if(first == SOUND_FILE || first == CATEGORY) {
        if(second == SOUND_FILE || second == CATEGORY)
            return SOUND_FILE_CATEGORY;
    }

    return NONE;
}

} // namespace Core
} // namespace DB
