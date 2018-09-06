#include "table_records.h"

namespace DB {

QString const toString(TableIndex idx)
{
    QString idx_str = "";

    switch(idx)
    {
        case SOUND_FILE:
            idx_str = "sound_file";
            break;
        case IMAGE_FILE:
            idx_str = "image_file";
            break;
        case CATEGORY:
            idx_str = "category";
            break;
        case SOUND_FILE_CATEGORY:
            idx_str = "sound_file_category";
            break;
        case RESOURCE_DIRECTORY:
            idx_str = "resource_directory";
            break;
        case TAG:
            idx_str = "tag";
            break;
        case IMAGE_FILE_TAG:
            idx_str = "image_file_tag";
            break;
        case IMAGE_DIRECTORY:
            idx_str = "image_directory";
            break;
        case PRESET:
            idx_str = "preset";
            break;
        default:
            break;
    }

    return idx_str;
}

TableIndex toTableIndex(QString const& idx_str)
{
    if(idx_str.compare("sound_file") == 0) {
        return SOUND_FILE;
    } else if(idx_str.compare("image_file") == 0) {
        return IMAGE_FILE;
    } else if(idx_str.compare("category") == 0) {
        return CATEGORY;
    } else if(idx_str.compare("sound_file_category") == 0) {
        return SOUND_FILE_CATEGORY;
    } else if(idx_str.compare("resource_directory") == 0) {
        return RESOURCE_DIRECTORY;
    } else if(idx_str.compare("tag") == 0) {
        return TAG;
    } else if(idx_str.compare("image_file_tag") == 0) {
        return IMAGE_FILE_TAG;
    } else if(idx_str.compare("image_directory") == 0) {
        return IMAGE_DIRECTORY;
    } else if(idx_str.compare("preset") == 0) {
        return PRESET;
    } else {
        return NONE;
    }
}

}
