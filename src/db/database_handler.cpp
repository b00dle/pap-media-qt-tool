#include "database_handler.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDebug>

DatabaseHandler::DatabaseHandler(DatabaseApi* api, QObject *parent)
    : QObject(parent)
    , api_(api)
    , category_tree_model_(0)
    , sound_file_table_model_(0)
    , resource_dir_table_model_(0)
    , image_dir_table_model_(0)
    , preset_table_model_(0)
{
    if(api_ != 0) {
        getCategoryTreeModel();
        getSoundFileTableModel();
    }
}

DatabaseApi *DatabaseHandler::getApi() const
{
    return api_;
}

CategoryTreeModel *DatabaseHandler::getCategoryTreeModel()
{
    if(category_tree_model_ == 0) {
        category_tree_model_ = new CategoryTreeModel(api_, this);
        category_tree_model_->select();
    }

    return category_tree_model_;
}

SoundFileTableModel *DatabaseHandler::getSoundFileTableModel()
{
    if(sound_file_table_model_ == 0) {
        sound_file_table_model_ = new SoundFileTableModel(api_, this);
        sound_file_table_model_->select();
    }

    return sound_file_table_model_;
}

ResourceDirTableModel *DatabaseHandler::getResourceDirTableModel()
{
    if(resource_dir_table_model_ == 0) {
        resource_dir_table_model_ = new ResourceDirTableModel(api_, this);
        resource_dir_table_model_->select();
    }

    return resource_dir_table_model_;
}

ImageDirTableModel *DatabaseHandler::getImageDirTableModel()
{
    if(image_dir_table_model_ == 0) {
        image_dir_table_model_ = new ImageDirTableModel(api_, this);
        image_dir_table_model_->select();
    }

    return image_dir_table_model_;
}

PresetTableModel *DatabaseHandler::getPresetTableModel()
{
    if(preset_table_model_ == 0) {
        preset_table_model_ = new PresetTableModel(api_, this);
        preset_table_model_->select();
    }

    return preset_table_model_;
}

const QList<SoundFileRecord *> DatabaseHandler::getSoundFileRecordsByCategoryId(int category_id)
{
    QList<SoundFileRecord*> records;

    QList<int> cat_ids = getCategoryTreeModel()->getSubCategoryIdsByCategoryId(category_id);
    cat_ids.append(category_id);

    foreach(int c_id, cat_ids) {
        foreach(int s_id, api_->getRelatedIds(SOUND_FILE, CATEGORY, c_id))
            records.append(getSoundFileTableModel()->getSoundFileById(s_id));
    }

    return records;
}

void DatabaseHandler::deleteAll()
{
    api_->deleteAll();
    getCategoryTreeModel()->update();
    getSoundFileTableModel()->update();
    getResourceDirTableModel()->update();
    getPresetTableModel()->update();
    getImageDirTableModel()->update();
}

void DatabaseHandler::addSoundFile(const QFileInfo& info, const ResourceDirRecord& resource_dir)
{
    getSoundFileTableModel()->addSoundFileRecord(info, resource_dir);
}

void DatabaseHandler::addCategory(QString name, CategoryRecord *parent)
{
    int p_id = -1;
    if(parent != 0)
        p_id = parent->id;
    api_->insertCategory(name, p_id);
    getCategoryTreeModel()->update();
}

void DatabaseHandler::addSoundFileCategory(int sound_file_id, int category_id)
{
    api_->insertSoundFileCategory(sound_file_id, category_id);
}

void DatabaseHandler::insertSoundFilesAndCategories(const QList<Resources::SoundFile>& sound_files)
{
    emit progressChanged(0);
    QCoreApplication::processEvents();

    QElapsedTimer timer;
    timer.start();

    CategoryRecord* cat = 0;
    int i = 1;
    foreach(Resources::SoundFile sf, sound_files) {
        // check if sound_file already imported
        SoundFileRecord* sf_rec = getSoundFileTableModel()->getSoundFileByPath(sf.getFileInfo().filePath());
        if(sf_rec != 0)
            continue;

        // insert new sound_file into DB
        addSoundFile(sf.getFileInfo(), sf.getResourceDir());
        sf_rec = getSoundFileTableModel()->getLastSoundFileRecord();

        // insert new category into DB
        cat = getCategoryTreeModel()->getCategoryByPath(sf.getCategoryPath());
        if(cat == 0) {
            addCategory(sf.getCategoryPath());
            cat = getCategoryTreeModel()->getCategoryByPath(sf.getCategoryPath());
        }

        // insert category sound_file relation into db
        if(cat != 0)
            addSoundFileCategory(sf_rec->id, cat->id);

        if(timer.elapsed() > 100) {
            int progress = (int) (i/(float)sound_files.size() * 100);
            emit progressChanged(progress);
            QCoreApplication::processEvents();
            timer.start();
        }

        ++i;
    }

    emit progressChanged(100);
    QCoreApplication::processEvents();
}

void DatabaseHandler::addCategory(const QStringList &path)
{
    CategoryRecord* parent = 0;
    int j = 0;
    for(int i = 0; i < path.size(); ++i) {
        CategoryRecord* cat = getCategoryTreeModel()->getCategoryByPath(path.mid(0,i+1));
        if(cat == 0) {
            j = i;
            break;
        }
        parent = cat;
    }

    while(j < path.size()) {
        addCategory(path[j], parent);
        parent = getCategoryTreeModel()->getCategoryByPath(path.mid(0,j+1));
        ++j;
    }
}

