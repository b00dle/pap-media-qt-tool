#ifndef RESOURCES_RESOURCES_H
#define RESOURCES_RESOURCES_H

#include <QString>
#include <QPixmap>
#include <QMap>
#include <QSettings>
#include <QFile>
#include "tracking/models/tracker_table_model.h"

namespace Resources {

struct Lib
{
    static void init();
    static void cleanup();
    static const QString loadFileToString(QString const& path);

    static QPixmap* getKeyPixmap(const QChar& k);

    /**
     * Global models
    */
    static TrackerTableModel* TRACKER_MODEL;

    /*
    * general paths
    */
    static QString DATABASE_PATH;
    static QString DEFAULT_PROJECT_PATH;

    /*
    * ICONS
    */
    // companion icon
    static QString IMG_COMPANION;
    // drag icon
    static QString IMG_SOUND_FILE_DRAG_PATH;
    // player controls
    static QString IMG_PLAY_PATH;
    static QString IMG_PAUSE_PATH;
    static QString IMG_STOP_PATH;
    // overlay
    static QString IMG_CRACKED_STONE_PATH;
    static QString IMG_CRACKED_STONE_INV_PATH;
    static QString IMG_FOLDER_PATH;
    static QString IMG_SPUNGIFY_PATH;
    static QString IMG_VISIBLE_PATH;
    static QString IMG_INVISIBLE_PATH;
    static QString IMG_BACK_BUTTON_PATH;
    // keys
    static QString IMG_KEY_0_PATH;
    static QString IMG_KEY_1_PATH;
    static QString IMG_KEY_2_PATH;
    static QString IMG_KEY_3_PATH;
    static QString IMG_KEY_4_PATH;
    static QString IMG_KEY_5_PATH;
    static QString IMG_KEY_6_PATH;
    static QString IMG_KEY_7_PATH;
    static QString IMG_KEY_8_PATH;
    static QString IMG_KEY_9_PATH;
    static QString IMG_KEY_A_PATH;
    static QString IMG_KEY_B_PATH;
    static QString IMG_KEY_C_PATH;
    static QString IMG_KEY_D_PATH;
    static QString IMG_KEY_E_PATH;
    static QString IMG_KEY_F_PATH;
    static QString IMG_KEY_G_PATH;
    static QString IMG_KEY_H_PATH;
    static QString IMG_KEY_I_PATH;
    static QString IMG_KEY_J_PATH;
    static QString IMG_KEY_K_PATH;
    static QString IMG_KEY_L_PATH;
    static QString IMG_KEY_M_PATH;
    static QString IMG_KEY_N_PATH;
    static QString IMG_KEY_O_PATH;
    static QString IMG_KEY_P_PATH;
    static QString IMG_KEY_Q_PATH;
    static QString IMG_KEY_R_PATH;
    static QString IMG_KEY_S_PATH;
    static QString IMG_KEY_T_PATH;
    static QString IMG_KEY_U_PATH;
    static QString IMG_KEY_V_PATH;
    static QString IMG_KEY_W_PATH;
    static QString IMG_KEY_X_PATH;
    static QString IMG_KEY_Y_PATH;
    static QString IMG_KEY_Z_PATH;

    // pixmaps
    static QPixmap* PX_COMPANION;
    static QPixmap* PX_CRACKED_STONE;
    static QPixmap* PX_CRACKED_STONE_INV;
    static QPixmap* PX_PLAY;
    static QPixmap* PX_PAUSE;
    static QPixmap* PX_STOP;
    static QPixmap* PX_SOUND_FILE_DRAG;
    static QPixmap* PX_FOLDER;
    static QPixmap* PX_SPUNGIFY;
    static QPixmap* PX_VISIBLE;
    static QPixmap* PX_INVISIBLE;
    static QPixmap* PX_BACK_BUTTON;
    static QPixmap* PX_0_KEY;
    static QPixmap* PX_1_KEY;
    static QPixmap* PX_2_KEY;
    static QPixmap* PX_3_KEY;
    static QPixmap* PX_4_KEY;
    static QPixmap* PX_5_KEY;
    static QPixmap* PX_6_KEY;
    static QPixmap* PX_7_KEY;
    static QPixmap* PX_8_KEY;
    static QPixmap* PX_9_KEY;
    static QPixmap* PX_A_KEY;
    static QPixmap* PX_B_KEY;
    static QPixmap* PX_C_KEY;
    static QPixmap* PX_D_KEY;
    static QPixmap* PX_E_KEY;
    static QPixmap* PX_F_KEY;
    static QPixmap* PX_G_KEY;
    static QPixmap* PX_H_KEY;
    static QPixmap* PX_I_KEY;
    static QPixmap* PX_J_KEY;
    static QPixmap* PX_K_KEY;
    static QPixmap* PX_L_KEY;
    static QPixmap* PX_M_KEY;
    static QPixmap* PX_N_KEY;
    static QPixmap* PX_O_KEY;
    static QPixmap* PX_P_KEY;
    static QPixmap* PX_Q_KEY;
    static QPixmap* PX_R_KEY;
    static QPixmap* PX_S_KEY;
    static QPixmap* PX_T_KEY;
    static QPixmap* PX_U_KEY;
    static QPixmap* PX_V_KEY;
    static QPixmap* PX_W_KEY;
    static QPixmap* PX_X_KEY;
    static QPixmap* PX_Y_KEY;
    static QPixmap* PX_Z_KEY;

    /*
    * STYLES
    */
    static QString DARK_STYLE;

    /*
    * WEB HOSTING CONFIG
    */
    static QString WEB_CONFIG;
    static QString WEB_URL_PREFIX;

    /*
    * LOG FILE CONFIG
    */
    static QFile LOG_FILE;

};

} // namespace Resources

#endif // RESOURCES_H
