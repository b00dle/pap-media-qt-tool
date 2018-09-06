#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <QObject>

#include <QSqlRelationalTableModel>

#include "core/api.h"
#include "resources/sound_file.h"
#include "model/category_tree_model.h"
#include "model/sound_file_table_model.h"
#include "model/resource_dir_table_model.h"
#include "model/image_dir_table_model.h"
#include "model/preset_table_model.h"

namespace DB {

/*
 * Class that Provides high-level interface to DB::Core::Api,
 * which is specific to this application.
 */
class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(DB::Core::Api* api, QObject *parent = 0);

    DB::Core::Api* getApi() const;

    Model::CategoryTreeModel* getCategoryTreeModel();
    Model::SoundFileTableModel* getSoundFileTableModel();
    Model::ResourceDirTableModel* getResourceDirTableModel();
    Model::ImageDirTableModel* getImageDirTableModel();
    Model::PresetTableModel* getPresetTableModel();

    /*
     * Gets a list of SoundFileRecords,
     * associated with Category referenced by given id.
    */
    QList<SoundFileRecord*> const getSoundFileRecordsByCategoryId(int category_id = -1);

signals:
    void progressChanged(int);

public slots:
    /*
    * deletes all contents of database
    */
    void deleteAll();

    /*
     * Add SoundFile to DB
    */
    void addSoundFile(QFileInfo const&, ResourceDirRecord const&);

    /*
     * Add Category to DB
    */
    void addCategory(QString name, CategoryRecord* parent = 0);

    /*
     * Add SoundFileCategory relation to DB
    */
    void addSoundFileCategory(int sound_file_id, int category_id);

    /*
     * Inserts new SoundFiles based on list given.
     * Will also insert new Categories in case any SoundFile
     * describes a new path through the category tree.
    */
    void insertSoundFilesAndCategories(QList<Resources::SoundFile> const&);

private:
    void addCategory(QStringList const& path);

    Core::Api* api_;

    Model::CategoryTreeModel* category_tree_model_;
    Model::SoundFileTableModel* sound_file_table_model_;
    Model::ResourceDirTableModel* resource_dir_table_model_;
    Model::ImageDirTableModel* image_dir_table_model_;
    Model::PresetTableModel* preset_table_model_;
};

} // namespace DB

#endif // DB_HANDLER_H
