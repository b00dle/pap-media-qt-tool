#ifndef DB_TABLES_H
#define DB_TABLES_H

#include <QString>
#include <QList>

namespace DB {

/* used to reference a table in the project db by int **/
enum TableIndex {
    NONE,
    SOUND_FILE,
    IMAGE_FILE,
    CATEGORY,
    SOUND_FILE_CATEGORY,
    RESOURCE_DIRECTORY,
    TAG,
    IMAGE_FILE_TAG,
    IMAGE_DIRECTORY,
    PRESET
};

/* data transfer object encapsulating one row in a db table **/
struct TableRecord {
    TableIndex index;
    int id;
    QString name;

    TableRecord(TableIndex idx, int i, QString const& n)
        : index(idx)
        , id(i)
        , name(n)
    {}

    TableRecord(TableIndex idx)
        : index(idx)
        , id(-1)
        , name("")
    {}

    TableRecord()
        : index(NONE)
        , id(-1)
        , name("")
    {}

    virtual ~TableRecord() {}

    virtual bool copyFrom(TableRecord* rec)
    {
        if(rec->index != index)
            return false;

        id = rec->id;
        name = rec->name;

        return true;
    }
};

/* Row in Category table **/
struct CategoryRecord : TableRecord {
    int parent_id;
    CategoryRecord* parent;
    QList<CategoryRecord*> children;

    CategoryRecord(int i, QString const& n, int p_id = -1, CategoryRecord* p = 0)
        : TableRecord(CATEGORY, i, n)
        , parent_id(p_id)
        , parent(p)
        , children()
    {}

    CategoryRecord()
        : TableRecord(CATEGORY, -1, "")
        , parent_id(-1)
        , parent(0)
    {}

    CategoryRecord(const CategoryRecord& rec)
        : TableRecord(CATEGORY, rec.id, rec.name)
        , parent_id(rec.parent_id)
        , parent(rec.parent)
    {}

    virtual ~CategoryRecord() {}

    virtual bool copyFrom(TableRecord* rec) {
        if(!TableRecord::copyFrom(rec))
            return false;

        CategoryRecord* cat_rec = (CategoryRecord*) rec;
        parent_id = cat_rec->parent_id;
        parent = cat_rec->parent;

        return true;
    }
};

/* Row in SoundFile table **/
struct SoundFileRecord : TableRecord {
    QString path;
    QString relative_path;

    SoundFileRecord(int i, QString const& n, QString const& p = "", QString const& rel_p = "")
        : TableRecord(SOUND_FILE, i, n)
        , path(p)
        , relative_path(rel_p)
    {}

    SoundFileRecord()
        : TableRecord(SOUND_FILE, -1, "")
        , path("")
        , relative_path("")
    {}

    SoundFileRecord(const SoundFileRecord& rec)
        : TableRecord(SOUND_FILE, rec.id, rec.name)
        , path(rec.path)
        , relative_path(rec.relative_path)
    {}

    virtual ~SoundFileRecord() {}

    virtual bool copyFrom(TableRecord* rec) {
        if(!TableRecord::copyFrom(rec))
            return false;

        SoundFileRecord* sf_rec = (SoundFileRecord*) rec;
        path = sf_rec->path;
        relative_path = sf_rec->relative_path;

        return true;
    }
};

/* Row in ImageFile table **/
struct ImageFileRecord : TableRecord {
    QString relative_path;

    ImageFileRecord(int i, QString const& n, QString const& rel_p = "")
        : TableRecord(IMAGE_FILE, i, n)
        , relative_path(rel_p)
    {}

    ImageFileRecord()
        : TableRecord(IMAGE_FILE, -1, "")
        , relative_path("")
    {}

    ImageFileRecord(const SoundFileRecord& rec)
        : TableRecord(IMAGE_FILE, rec.id, rec.name)
        , relative_path(rec.relative_path)
    {}

    virtual ~ImageFileRecord() {}

    virtual bool copyFrom(TableRecord* rec) {
        if(!TableRecord::copyFrom(rec))
            return false;

        ImageFileRecord* if_rec = (ImageFileRecord*) rec;
        relative_path = if_rec->relative_path;

        return true;
    }
};

/* Row in ResourceDirectory table */
struct ResourceDirRecord : TableRecord {
    QString path;

    ResourceDirRecord(int i, QString const& n, QString const& p)
        : TableRecord(RESOURCE_DIRECTORY, i, n)
        , path(p)
    {}

    ResourceDirRecord()
        : TableRecord(RESOURCE_DIRECTORY, -1, "")
        , path("")
    {}

    ResourceDirRecord(const ResourceDirRecord& rec)
        : TableRecord(RESOURCE_DIRECTORY, rec.id, rec.name)
        , path(rec.path)
    {}

    virtual ~ResourceDirRecord() {}

    virtual bool copyFrom(TableRecord* rec) {
        if(!TableRecord::copyFrom(rec))
            return false;

        ResourceDirRecord* rd_rec = (ResourceDirRecord*) rec;
        path = rd_rec->path;

        return true;
    }
};

/* Row in ImageDirectory table */
struct ImageDirRecord : TableRecord {
    QString path;

    ImageDirRecord(int i, QString const& n, QString const& p)
        : TableRecord(RESOURCE_DIRECTORY, i, n)
        , path(p)
    {}

    ImageDirRecord()
        : TableRecord(RESOURCE_DIRECTORY, -1, "")
        , path("")
    {}

    ImageDirRecord(const ImageDirRecord& rec)
        : TableRecord(RESOURCE_DIRECTORY, rec.id, rec.name)
        , path(rec.path)
    {}

    virtual ~ImageDirRecord() {}

    virtual bool copyFrom(TableRecord* rec) {
        if(!TableRecord::copyFrom(rec))
            return false;

        ImageDirRecord* rd_rec = (ImageDirRecord*) rec;
        path = rd_rec->path;

        return true;
    }
};

/* Row in ImageTag table */
struct TagRecord : TableRecord {
    TagRecord(int i, QString const& n)
        : TableRecord(TAG, i, n)
    {}

    TagRecord()
        : TableRecord(TAG, -1, "")
    {}

    TagRecord(const TagRecord& rec)
        : TableRecord(TAG, rec.id, rec.name)
    {}

    virtual ~TagRecord() {}
};

/* Row in ResourceDirectory table */
struct PresetRecord : TableRecord {
    QString json;

    PresetRecord(int i, QString const& n, QString const& j)
        : TableRecord(PRESET, i, n)
        , json(j)
    {}

    PresetRecord()
        : TableRecord(PRESET, -1, "")
        , json("")
    {}

    PresetRecord(const PresetRecord& rec)
        : TableRecord(PRESET, rec.id, rec.name)
        , json(rec.json)
    {}

    virtual ~PresetRecord() {}

    virtual bool copyFrom(TableRecord* rec) {
        if(!TableRecord::copyFrom(rec))
            return false;

        PresetRecord* rd_rec = (PresetRecord*) rec;
        json = rd_rec->json;

        return true;
    }
};

/*
 * Converts a TableIndex to a string
 * containing the name of the referenced table.
 * string will be empty if TableIndex is NONE.
**/
QString const toString(TableIndex idx);

/*
 * Converts given db table name into a TableIndex.
 * Returns NONE if string does not name a table in the db.
 **/
TableIndex toTableIndex(QString const& idx_str);

} // namespace DB

#endif // DB_TABLES_H
