#include "json_mime_data_parser.h"

#include <QJsonArray>
#include <QDebug>

namespace Misc {

JsonMimeDataParser::JsonMimeDataParser()
{}

QMimeData* JsonMimeDataParser::toJsonMimeData(DB::TableRecord* rec)
{
    QMimeData* data = 0;
    if(rec == 0)
        return data;

    QJsonObject obj(toJsonObject(rec));
    if(obj.isEmpty())
        return data;

    QJsonDocument doc(obj);

    data = new QMimeData;
    data->setText(QString(doc.toJson()));

    return data;
}

QMimeData *JsonMimeDataParser::toJsonMimeData(const QList<DB::TableRecord *>& records)
{
    QMimeData* data = 0;
    if(records.size() == 0)
        return data;

    QJsonArray arr;
    foreach(DB::TableRecord* rec, records) {
        QJsonObject obj(toJsonObject(rec));
        if(obj.isEmpty())
            continue;
        arr.append(obj);
    }

    QJsonDocument doc(arr);

    data = new QMimeData;
    data->setText(QString(doc.toJson()));

    return data;
}

DB::TableRecord *JsonMimeDataParser::toTableRecord(const QMimeData* mime)
{
    if(mime == 0 || !mime->hasText())
        return 0;

    QJsonDocument doc = QJsonDocument::fromJson(mime->text().toUtf8());
    if(doc.isNull() || doc.isEmpty() || !doc.isObject())
        return 0;

    return toTableRecord(doc.object());
}

QList<DB::TableRecord *> JsonMimeDataParser::toTableRecordList(const QMimeData* mime)
{
    QList<DB::TableRecord*> records;
    if(mime == 0 || !mime->hasText())
        return records;

    QJsonDocument doc = QJsonDocument::fromJson(mime->text().toUtf8());
    if(doc.isNull() || doc.isEmpty())
        return records;

    if(doc.isObject()) {
        DB::TableRecord* rec = toTableRecord(mime);
        if(rec == 0)
            return records;
        records.append(rec);
    }
    else if(doc.isArray()) {
        foreach(QJsonValue val, doc.array()) {
            if(!val.isObject())
                continue;

            DB::TableRecord* rec = toTableRecord(val.toObject());
            if(rec == 0)
                continue;
            records.append(rec);
        }
    }

    return records;
}

DB::TableRecord *JsonMimeDataParser::toTableRecord(const QJsonObject& obj)
{
    DB::TableRecord* rec = 0;

    if(!obj.contains("type"))
        return rec;

    if(obj["type"] == DB::SOUND_FILE) {
        int id = -1;
        QString name = "";
        QString path = "";
        QString relative_path = "";
        if(obj.contains("id"))
            id = obj["id"].toInt();
        if(obj.contains("name") || obj["name"].isString())
            name = obj["name"].toString();
        if(obj.contains("path") || obj["path"].isString())
            path = obj["path"].toString();
        if(obj.contains("relative_path") || obj["relative_path"].isString())
            relative_path = obj["relative_path"].toString();

        rec = new DB::SoundFileRecord(id, name, path, relative_path);
    }
    else if(obj["type"] == DB::CATEGORY) {
        int id = -1;
        QString name = "";
        int parent_id = -1;
        if(obj.contains("id"))
            id = obj["id"].toInt();
        if(obj.contains("name") || obj["name"].isString())
            name = obj["name"].toString();
        if(obj.contains("parent_id"))
            parent_id = obj["parent_id"].toInt();

        rec = new DB::CategoryRecord(id, name, parent_id);
    }

    return rec;
}

const QJsonObject JsonMimeDataParser::toJsonObject(DB::TableRecord* rec)
{
    QJsonObject obj;

    if(rec == 0)
        return obj;

    switch(rec->index) {
        case DB::SOUND_FILE:
            obj = toJsonObject((DB::SoundFileRecord*) rec);
            break;

        case DB::CATEGORY:
            obj = toJsonObject((DB::CategoryRecord*) rec);
            break;

        case DB::SOUND_FILE_CATEGORY:
        case DB::NONE:
        default:
            break;
    }

    return obj;
}

const QJsonObject JsonMimeDataParser::toJsonObject(DB::SoundFileRecord* rec)
{
    QJsonObject obj;

    if(rec == 0)
        return obj;

    obj.insert("type", QJsonValue(rec->index));
    obj.insert("id", QJsonValue(rec->id));
    obj.insert("name", QJsonValue(rec->name));
    obj.insert("path", QJsonValue(rec->path));
    obj.insert("relative_path", QJsonValue(rec->relative_path));

    return obj;
}

const QJsonObject JsonMimeDataParser::toJsonObject(const Playlist::Settings& settings)
{
    QJsonObject obj;

    obj.insert("name", QJsonValue(settings.name));
    obj.insert("order", QJsonValue(settings.order));
    obj.insert("loop_flag", QJsonValue(settings.loop_flag));
    obj.insert("interval_flag", QJsonValue(settings.interval_flag));
    obj.insert("min_interval_val", QJsonValue(settings.min_delay_interval));
    obj.insert("max_interval_val", QJsonValue(settings.max_delay_interval));
    obj.insert("volume", QJsonValue(settings.volume));

    return obj;

}

Playlist::Settings* JsonMimeDataParser::toPlaylistSettings(const QJsonObject& obj)
{
    Playlist::Settings* set = 0;

    if(!obj.contains("interval_flag") || !obj.contains("min_interval_val") ||
       !obj.contains("max_interval_val") || !obj.contains("loop_flag") ||
       !obj.contains("name") || !obj.contains("volume") ||
       !obj.contains("order")) {
        qDebug() << "FAILURE: Could not parse playlist settings from JSON";
        qDebug() << " > Missing attribute description.";
        return set;
    }

    //set name
    set = new Playlist::Settings;
    set->name = obj["name"].toString();

    //set interval
    if(obj["interval_flag"] == true) {
        set->interval_flag = true;
        set->min_delay_interval = obj["min_interval_val"].toInt();
        set->max_delay_interval = obj["max_interval_val"].toInt();
    }
    else if(obj["interval_flag"] == false) {
        set->interval_flag = true;
        set->min_delay_interval = obj["min_interval_val"].toInt();
        set->max_delay_interval = obj["max_interval_val"].toInt();
    }

    //set volume
    if(obj["volume"].toInt() <= 100 && obj["volume"].toInt() >= 0) {
        set->volume = obj["volume"].toInt();
    }

    // set loop_flag
    if(obj["loop_flag"] == true) {
        set->loop_flag = true;
    } else if (obj["loop_flag"] == false) {
        set->loop_flag = false;
    }

    // set order
    if(obj["order"] == 0) {
        set->order = Playlist::ORDERED;
    } else if (obj["order"] == 1) {
        set->order = Playlist::SHUFFLE;
    } else if (obj["order"] == 2) {
        set->order = Playlist::WEIGHTED;
    }

    return set;
}

const QJsonArray JsonMimeDataParser::toJsonArray(const QPainterPath &path)
{
    QJsonArray path_arr;
    QJsonObject e_obj;
    QPainterPath::Element e;
    for(int i = 0; i < path.elementCount(); ++i) {
        e = path.elementAt(i);
        e_obj["x"] = e.x;
        e_obj["y"] = e.y;
        e_obj["type"] = e.type;
        path_arr.append(e_obj);
    }
    return path_arr;
}

const QPainterPath JsonMimeDataParser::toPainterPath(const QJsonArray &arr)
{
    QPainterPath p;
    QJsonObject e_obj;
    bool is_well_formed;
    foreach(auto value, arr) {
        if(!value.isObject())
            continue;
        e_obj = value.toObject();
        is_well_formed = e_obj.contains("type") &&
            e_obj.contains("x") && e_obj["x"].isDouble() &&
            e_obj.contains("y") && e_obj["y"].isDouble();
        if(!is_well_formed)
            continue;
        switch(e_obj["type"].toInt()) {
            case QPainterPath::MoveToElement:
                p.moveTo(e_obj["x"].toDouble(),e_obj["y"].toDouble());
                break;
            case QPainterPath::LineToElement:
                p.lineTo(e_obj["x"].toDouble(),e_obj["y"].toDouble());
                break;
            case QPainterPath::CurveToElement:
                qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
                qDebug() << "  > QPainterPath::CurveToElement " << e_obj;
                break;
            case QPainterPath::CurveToDataElement:
                qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
                qDebug() << "  > QPainterPath::CurveToDataElement " << e_obj;
                break;
            default: break;
        }
    }
    return p;
}

const QJsonObject JsonMimeDataParser::toJsonObject(DB::CategoryRecord* rec)
{
    QJsonObject obj;

    if(rec == 0)
        return obj;

    obj.insert("type", QJsonValue(rec->index));
    obj.insert("id", QJsonValue(rec->id));
    obj.insert("name", QJsonValue(rec->name));
    obj.insert("parent_id", QJsonValue(rec->parent_id));

    return obj;
}

} // namespace Misc
