#ifndef MISC_JSON_MIME_DATA_PARSER_H
#define MISC_JSON_MIME_DATA_PARSER_H

#include <QJsonDocument>
#include <QMimeData>
#include <QJsonObject>
#include <QList>
#include <QPainterPath>

#include "db/table_records.h"
#include "playlist/settings.h"

namespace Misc {

/*
* Class used for conversion of TableRecords to QMimeData.
* TableRecords will be referenced by a Json serialized string,
* inside the text() property of QMimeData.
* Also transfers QMimeData back to TableRecord.
*/

class JsonMimeDataParser
{
public:
    explicit JsonMimeDataParser();
    ~JsonMimeDataParser();

    /*
     * creates QMimeData with text set to a string
     * wrapping a json serialized object, referencing
     * given TableRecord.
     * returns 0 if parsing fails.
    */
    static QMimeData* toJsonMimeData(DB::TableRecord*);

    /*
     * creates QMimeData with text set to a string
     * wrapping a json serialized object, referencing
     * given all given TableRecords.
     * returns 0 if parsing fails.
    */
    static QMimeData* toJsonMimeData(const QList<DB::TableRecord*>&);

    /*
     * Extracts DB::TableRecord from given QMimeData.
     * Parser assumes text in QMimeData is in Json format and.
     * (see toJsonMimeData(...))
     * Returns 0 if parsing fails.
     * Note: None of the returned pointers reference any DB::Model::...
     * elements and should therefore be deleted after use.
    */
    static DB::TableRecord* toTableRecord(const QMimeData*);

    /*
     * Extracts list of DB::TableRecord from given QMimeData.
     * Parser assumes text in QMimeData is in Json format and.
     * (see toJsonMimeData(...))
     * Returns 0 if parsing fails.
     * Note: None of the returned pointers reference any DB::Model::...
     * elements and should therefore be deleted after use.
    */
    static QList<DB::TableRecord*> toTableRecordList(const QMimeData*);

    /*
    * Extracts DB::TableRecord from given QJsonObject.
    * Returns 0 if parsing failes.
    * Note: None of the returned pointers reference any DB::Model::...
    * elements and should therefore be deleted after use.
    */
    static DB::TableRecord* toTableRecord(const QJsonObject&);

    /* Creates JsonObject from given DB::TableRecord */
    static const QJsonObject toJsonObject(DB::TableRecord*);

    /* Creates JsonObject from given DB::SoundFileRecord */
    static const QJsonObject toJsonObject(DB::SoundFileRecord*);

    /* Creates JsonObject from given Playlist::Settings */
    static const QJsonObject toJsonObject(const Playlist::Settings&);

    static Playlist::Settings* toPlaylistSettings(const QJsonObject&);

    /** Creates JsonArray of QPainterPath::Elements */
    static const QJsonArray toJsonArray(const QPainterPath& path);

    /** Creates QPainterPath from JsonArray */
    static const QPainterPath toPainterPath(const QJsonArray& arr);

    /*
     * Creates JsonObject from given DB::CategoryRecord.
     * Serializes only id,index,name,parent_id.
    */
    static const QJsonObject toJsonObject(DB::CategoryRecord*);
};

} // namespace Misc

#endif // MISC_JSON_MIME_DATA_PARSER_H
