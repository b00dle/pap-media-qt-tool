#include "lib.h"

#include <QFile>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>

namespace Resources {

/*
* init function
*/
void Lib::init()
{
#ifdef __APPLE__
    DATABASE_PATH = "/../../../../../companion-shared-files/companion.db";
#elif __linux__
    DATABASE_PATH = "/../../companion-shared-files/companion.db";
#else
    DATABASE_PATH = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString("CoG/companion/companion.db"));
#endif
//DATABASE_PATH = "../../../../companion-shared-files/companion.db";

#ifdef __APPLE__
    DARK_STYLE = loadFileToString("/../../../../src/_Res/dark_style.css");
#elif __linux__
    DARK_STYLE = loadFileToString("/../src/_RES/dark_style.css"); //linux
#else
    DARK_STYLE = loadFileToString(":/styles/dark_style.css");
#endif
    if(!TRACKER_MODEL)
        TRACKER_MODEL = new TrackerTableModel;
    if(PX_COMPANION == 0)
        PX_COMPANION = new QPixmap(IMG_COMPANION);
    if(PX_CRACKED_STONE == 0)
        PX_CRACKED_STONE = new QPixmap(IMG_CRACKED_STONE_PATH);
    if(PX_CRACKED_STONE_INV == 0)
        PX_CRACKED_STONE_INV = new QPixmap(IMG_CRACKED_STONE_INV_PATH);
    if(PX_PLAY == 0)
        PX_PLAY = new QPixmap(IMG_PLAY_PATH);
    if(PX_PAUSE == 0)
        PX_PAUSE = new QPixmap(IMG_PAUSE_PATH);
    if(PX_STOP == 0)
        PX_STOP = new QPixmap(IMG_STOP_PATH);
    if(PX_SOUND_FILE_DRAG == 0)
        PX_SOUND_FILE_DRAG = new QPixmap(IMG_SOUND_FILE_DRAG_PATH);
    if(PX_FOLDER == 0)
        PX_FOLDER = new QPixmap(IMG_FOLDER_PATH);
    if(PX_SPUNGIFY == 0)
        PX_SPUNGIFY = new QPixmap(IMG_SPUNGIFY_PATH);
    if(PX_VISIBLE == 0)
        PX_VISIBLE = new QPixmap(IMG_VISIBLE_PATH);
    if(PX_INVISIBLE == 0)
        PX_INVISIBLE = new QPixmap(IMG_INVISIBLE_PATH);
    if(PX_BACK_BUTTON == 0)
        PX_BACK_BUTTON = new QPixmap(IMG_BACK_BUTTON_PATH);
    if(PX_0_KEY == 0)
        PX_0_KEY = new QPixmap(IMG_KEY_0_PATH);
    if(PX_1_KEY == 0)
        PX_1_KEY = new QPixmap(IMG_KEY_1_PATH);
    if(PX_2_KEY == 0)
        PX_2_KEY = new QPixmap(IMG_KEY_2_PATH);
    if(PX_3_KEY == 0)
        PX_3_KEY = new QPixmap(IMG_KEY_3_PATH);
    if(PX_4_KEY == 0)
        PX_4_KEY = new QPixmap(IMG_KEY_4_PATH);
    if(PX_5_KEY == 0)
        PX_5_KEY = new QPixmap(IMG_KEY_5_PATH);
    if(PX_6_KEY == 0)
        PX_6_KEY = new QPixmap(IMG_KEY_6_PATH);
    if(PX_7_KEY == 0)
        PX_7_KEY = new QPixmap(IMG_KEY_7_PATH);
    if(PX_8_KEY == 0)
        PX_8_KEY = new QPixmap(IMG_KEY_8_PATH);
    if(PX_9_KEY == 0)
        PX_9_KEY = new QPixmap(IMG_KEY_9_PATH);
    if(PX_A_KEY == 0)
        PX_A_KEY = new QPixmap(IMG_KEY_A_PATH);
    if(PX_B_KEY == 0)
        PX_B_KEY = new QPixmap(IMG_KEY_B_PATH);
    if(PX_C_KEY == 0)
        PX_C_KEY = new QPixmap(IMG_KEY_C_PATH);
    if(PX_D_KEY == 0)
        PX_D_KEY = new QPixmap(IMG_KEY_D_PATH);
    if(PX_E_KEY == 0)
        PX_E_KEY = new QPixmap(IMG_KEY_E_PATH);
    if(PX_F_KEY == 0)
        PX_F_KEY = new QPixmap(IMG_KEY_F_PATH);
    if(PX_G_KEY == 0)
        PX_G_KEY = new QPixmap(IMG_KEY_G_PATH);
    if(PX_H_KEY == 0)
        PX_H_KEY = new QPixmap(IMG_KEY_H_PATH);
    if(PX_I_KEY == 0)
        PX_I_KEY = new QPixmap(IMG_KEY_I_PATH);
    if(PX_J_KEY == 0)
        PX_J_KEY = new QPixmap(IMG_KEY_J_PATH);
    if(PX_K_KEY == 0)
        PX_K_KEY = new QPixmap(IMG_KEY_K_PATH);
    if(PX_L_KEY == 0)
        PX_L_KEY = new QPixmap(IMG_KEY_L_PATH);
    if(PX_M_KEY == 0)
        PX_M_KEY = new QPixmap(IMG_KEY_M_PATH);
    if(PX_N_KEY == 0)
        PX_N_KEY = new QPixmap(IMG_KEY_N_PATH);
    if(PX_O_KEY == 0)
        PX_O_KEY = new QPixmap(IMG_KEY_O_PATH);
    if(PX_P_KEY == 0)
        PX_P_KEY = new QPixmap(IMG_KEY_P_PATH);
    if(PX_Q_KEY == 0)
        PX_Q_KEY = new QPixmap(IMG_KEY_Q_PATH);
    if(PX_R_KEY == 0)
        PX_R_KEY = new QPixmap(IMG_KEY_R_PATH);
    if(PX_S_KEY == 0)
        PX_S_KEY = new QPixmap(IMG_KEY_S_PATH);
    if(PX_T_KEY == 0)
        PX_T_KEY = new QPixmap(IMG_KEY_T_PATH);
    if(PX_U_KEY == 0)
        PX_U_KEY = new QPixmap(IMG_KEY_U_PATH);
    if(PX_V_KEY == 0)
        PX_V_KEY = new QPixmap(IMG_KEY_V_PATH);
    if(PX_W_KEY == 0)
        PX_W_KEY = new QPixmap(IMG_KEY_W_PATH);
    if(PX_X_KEY == 0)
        PX_X_KEY = new QPixmap(IMG_KEY_X_PATH);
    if(PX_Y_KEY == 0)
        PX_Y_KEY = new QPixmap(IMG_KEY_Y_PATH);
    if(PX_Z_KEY == 0)
        PX_Z_KEY = new QPixmap(IMG_KEY_Z_PATH);
}

void Lib::cleanup()
{
    if(TRACKER_MODEL)
        TRACKER_MODEL->deleteLater();
    if(PX_COMPANION != 0)
        delete PX_COMPANION;
    if(PX_CRACKED_STONE != 0)
        delete PX_CRACKED_STONE;
    if(PX_CRACKED_STONE_INV != 0)
        delete PX_CRACKED_STONE_INV;
    if(PX_PLAY != 0)
        delete PX_PLAY;
    if(PX_PAUSE != 0)
        delete PX_PAUSE;
    if(PX_STOP != 0)
        delete PX_STOP;
    if(PX_SOUND_FILE_DRAG != 0)
        delete PX_SOUND_FILE_DRAG;
    if(PX_FOLDER != 0)
        delete PX_FOLDER;
    if(PX_SPUNGIFY != 0)
        delete PX_SPUNGIFY;
    if(PX_VISIBLE != 0)
        delete PX_VISIBLE;
    if(PX_INVISIBLE != 0)
        delete PX_INVISIBLE;
    if(PX_BACK_BUTTON != 0)
        delete PX_BACK_BUTTON;
    if(PX_0_KEY != 0)
        delete PX_0_KEY;
    if(PX_1_KEY != 0)
        delete PX_1_KEY;
    if(PX_2_KEY != 0)
        delete PX_2_KEY;
    if(PX_3_KEY != 0)
        delete PX_3_KEY;
    if(PX_4_KEY != 0)
        delete PX_4_KEY;
    if(PX_5_KEY != 0)
        delete PX_5_KEY;
    if(PX_6_KEY != 0)
        delete PX_6_KEY;
    if(PX_7_KEY != 0)
        delete PX_7_KEY;
    if(PX_8_KEY != 0)
        delete PX_8_KEY;
    if(PX_9_KEY != 0)
        delete PX_9_KEY;
    if(PX_A_KEY != 0)
        delete PX_A_KEY;
    if(PX_B_KEY != 0)
        delete PX_B_KEY;
    if(PX_C_KEY != 0)
        delete PX_C_KEY;
    if(PX_D_KEY != 0)
        delete PX_D_KEY;
    if(PX_E_KEY != 0)
        delete PX_E_KEY;
    if(PX_F_KEY != 0)
        delete PX_F_KEY;
    if(PX_G_KEY != 0)
        delete PX_G_KEY;
    if(PX_H_KEY != 0)
        delete PX_H_KEY;
    if(PX_I_KEY != 0)
        delete PX_I_KEY;
    if(PX_J_KEY != 0)
        delete PX_J_KEY;
    if(PX_K_KEY != 0)
        delete PX_K_KEY;
    if(PX_L_KEY != 0)
        delete PX_L_KEY;
    if(PX_M_KEY != 0)
        delete PX_M_KEY;
    if(PX_N_KEY != 0)
        delete PX_N_KEY;
    if(PX_O_KEY != 0)
        delete PX_O_KEY;
    if(PX_P_KEY != 0)
        delete PX_P_KEY;
    if(PX_Q_KEY != 0)
        delete PX_Q_KEY;
    if(PX_R_KEY != 0)
        delete PX_R_KEY;
    if(PX_S_KEY != 0)
        delete PX_S_KEY;
    if(PX_T_KEY != 0)
        delete PX_T_KEY;
    if(PX_U_KEY != 0)
        delete PX_U_KEY;
    if(PX_V_KEY != 0)
        delete PX_V_KEY;
    if(PX_W_KEY != 0)
        delete PX_W_KEY;
    if(PX_X_KEY != 0)
        delete PX_X_KEY;
    if(PX_Y_KEY != 0)
        delete PX_Y_KEY;
    if(PX_Z_KEY != 0)
        delete PX_Z_KEY;

    TRACKER_MODEL = nullptr;
    PX_COMPANION = 0;
    PX_CRACKED_STONE = 0;
    PX_CRACKED_STONE_INV = 0;
    PX_PLAY = 0;
    PX_PAUSE = 0;
    PX_STOP = 0;
    PX_SOUND_FILE_DRAG = 0;
    PX_FOLDER = 0;
    PX_SPUNGIFY = 0;
    PX_VISIBLE = 0;
    PX_INVISIBLE = 0;
    PX_BACK_BUTTON = 0;
    PX_0_KEY = 0;
    PX_1_KEY = 0;
    PX_2_KEY = 0;
    PX_3_KEY = 0;
    PX_4_KEY = 0;
    PX_5_KEY = 0;
    PX_6_KEY = 0;
    PX_7_KEY = 0;
    PX_8_KEY = 0;
    PX_9_KEY = 0;
    PX_A_KEY = 0;
    PX_B_KEY = 0;
    PX_C_KEY = 0;
    PX_D_KEY = 0;
    PX_E_KEY = 0;
    PX_F_KEY = 0;
    PX_G_KEY = 0;
    PX_H_KEY = 0;
    PX_I_KEY = 0;
    PX_J_KEY = 0;
    PX_K_KEY = 0;
    PX_L_KEY = 0;
    PX_M_KEY = 0;
    PX_N_KEY = 0;
    PX_O_KEY = 0;
    PX_P_KEY = 0;
    PX_Q_KEY = 0;
    PX_R_KEY = 0;
    PX_S_KEY = 0;
    PX_T_KEY = 0;
    PX_U_KEY = 0;
    PX_V_KEY = 0;
    PX_W_KEY = 0;
    PX_X_KEY = 0;
    PX_Y_KEY = 0;
    PX_Z_KEY = 0;
}

const QString Lib::loadFileToString(const QString &path)
{
    QString load_path = path;
    if(!load_path.startsWith(":")) {
        load_path = QDir::currentPath() + path;
    }
    QFile file(load_path);
    if(file.open(QFile::ReadOnly)){
        return QLatin1String(file.readAll());
    }
    qDebug() << file.errorString();
    qDebug() << " > " << load_path;
    return QString("");
}

QPixmap *Lib::getKeyPixmap(const QChar &k)
{
    switch(k.toLatin1()) {
        case '0':
            return PX_0_KEY;
        case '1':
            return PX_1_KEY;
        case '2':
            return PX_2_KEY;
        case '3':
            return PX_3_KEY;
        case '4':
            return PX_4_KEY;
        case '5':
            return PX_5_KEY;
        case '6':
            return PX_6_KEY;
        case '7':
            return PX_7_KEY;
        case '8':
            return PX_8_KEY;
        case '9':
            return PX_9_KEY;
        case 'A':
            return PX_A_KEY;
        case 'B':
            return PX_B_KEY;
        case 'C':
            return PX_C_KEY;
        case 'D':
            return PX_D_KEY;
        case 'E':
            return PX_E_KEY;
        case 'F':
            return PX_F_KEY;
        case 'G':
            return PX_G_KEY;
        case 'H':
            return PX_H_KEY;
        case 'I':
            return PX_I_KEY;
        case 'J':
            return PX_J_KEY;
        case 'K':
            return PX_K_KEY;
        case 'L':
            return PX_L_KEY;
        case 'M':
            return PX_M_KEY;
        case 'N':
            return PX_N_KEY;
        case 'O':
            return PX_O_KEY;
        case 'P':
            return PX_P_KEY;
        case 'Q':
            return PX_Q_KEY;
        case 'R':
            return PX_R_KEY;
        case 'S':
            return PX_S_KEY;
        case 'T':
            return PX_T_KEY;
        case 'U':
            return PX_U_KEY;
        case 'V':
            return PX_V_KEY;
        case 'W':
            return PX_W_KEY;
        case 'X':
            return PX_X_KEY;
        case 'Y':
            return PX_Y_KEY;
        case 'Z':
            return PX_Z_KEY;
        default:
            return 0;
    }
}

/**
 * Global models
*/
TrackerTableModel* Lib::TRACKER_MODEL = nullptr;

/*
* DATABASE
*/

QString Lib::DATABASE_PATH = ""; // set in init()
//QString Lib::DATABASE_PATH = "../../../../companion-shared-files/companion.db";
QString Lib::DEFAULT_PROJECT_PATH = "../../companion-shared-files";

/*
* ICONS
*/
QString Lib::IMG_COMPANION = ":/images/companion-icon.png";
QString Lib::IMG_SOUND_FILE_DRAG_PATH = ":/images/dick.png";
QString Lib::IMG_PLAY_PATH = ":/images/icon_play.png";
QString Lib::IMG_PAUSE_PATH = ":/images/icon_pause.png";
QString Lib::IMG_STOP_PATH = ":/images/icon_stop.png";
QString Lib::IMG_CRACKED_STONE_PATH = ":/images/cracked_stone.jpg";
QString Lib::IMG_CRACKED_STONE_INV_PATH = ":/images/cracked_stone_inv.jpg";
QString Lib::IMG_FOLDER_PATH = ":/images/folder.png";
QString Lib::IMG_SPUNGIFY_PATH = ":/images/spungify.png";
QString Lib::IMG_VISIBLE_PATH = ":/images/visible.png";
QString Lib::IMG_INVISIBLE_PATH = ":/images/invisible.png";
QString Lib::IMG_BACK_BUTTON_PATH = ":/images/back_button_round.png";

// keys
QString Lib::IMG_KEY_0_PATH = ":/keys/0_key.png";
QString Lib::IMG_KEY_1_PATH = ":/keys/1_key.png";
QString Lib::IMG_KEY_2_PATH = ":/keys/2_key.png";
QString Lib::IMG_KEY_3_PATH = ":/keys/3_key.png";
QString Lib::IMG_KEY_4_PATH = ":/keys/4_key.png";
QString Lib::IMG_KEY_5_PATH = ":/keys/5_key.png";
QString Lib::IMG_KEY_6_PATH = ":/keys/6_key.png";
QString Lib::IMG_KEY_7_PATH = ":/keys/7_key.png";
QString Lib::IMG_KEY_8_PATH = ":/keys/8_key.png";
QString Lib::IMG_KEY_9_PATH = ":/keys/9_key.png";
QString Lib::IMG_KEY_A_PATH = ":/keys/A_key.png";
QString Lib::IMG_KEY_B_PATH = ":/keys/B_key.png";
QString Lib::IMG_KEY_C_PATH = ":/keys/C_key.png";
QString Lib::IMG_KEY_D_PATH = ":/keys/D_key.png";
QString Lib::IMG_KEY_E_PATH = ":/keys/E_key.png";
QString Lib::IMG_KEY_F_PATH = ":/keys/F_key.png";
QString Lib::IMG_KEY_G_PATH = ":/keys/G_key.png";
QString Lib::IMG_KEY_H_PATH = ":/keys/H_key.png";
QString Lib::IMG_KEY_I_PATH = ":/keys/I_key.png";
QString Lib::IMG_KEY_J_PATH = ":/keys/J_key.png";
QString Lib::IMG_KEY_K_PATH = ":/keys/K_key.png";
QString Lib::IMG_KEY_L_PATH = ":/keys/L_key.png";
QString Lib::IMG_KEY_M_PATH = ":/keys/M_key.png";
QString Lib::IMG_KEY_N_PATH = ":/keys/N_key.png";
QString Lib::IMG_KEY_O_PATH = ":/keys/O_key.png";
QString Lib::IMG_KEY_P_PATH = ":/keys/P_key.png";
QString Lib::IMG_KEY_Q_PATH = ":/keys/Q_key.png";
QString Lib::IMG_KEY_R_PATH = ":/keys/R_key.png";
QString Lib::IMG_KEY_S_PATH = ":/keys/S_key.png";
QString Lib::IMG_KEY_T_PATH = ":/keys/T_key.png";
QString Lib::IMG_KEY_U_PATH = ":/keys/U_key.png";
QString Lib::IMG_KEY_V_PATH = ":/keys/V_key.png";
QString Lib::IMG_KEY_W_PATH = ":/keys/W_key.png";
QString Lib::IMG_KEY_X_PATH = ":/keys/X_key.png";
QString Lib::IMG_KEY_Y_PATH = ":/keys/Y_key.png";
QString Lib::IMG_KEY_Z_PATH = ":/keys/Z_key.png";

QPixmap* Lib::PX_COMPANION = 0;
QPixmap* Lib::PX_CRACKED_STONE = 0;
QPixmap* Lib::PX_CRACKED_STONE_INV = 0;
QPixmap* Lib::PX_PLAY = 0;
QPixmap* Lib::PX_PAUSE = 0;
QPixmap* Lib::PX_STOP = 0;
QPixmap* Lib::PX_SOUND_FILE_DRAG = 0;
QPixmap* Lib::PX_FOLDER = 0;
QPixmap* Lib::PX_SPUNGIFY = 0;
QPixmap* Lib::PX_VISIBLE = 0;
QPixmap* Lib::PX_INVISIBLE = 0;
QPixmap* Lib::PX_BACK_BUTTON = 0;
QPixmap* Lib::PX_0_KEY = 0;
QPixmap* Lib::PX_1_KEY = 0;
QPixmap* Lib::PX_2_KEY = 0;
QPixmap* Lib::PX_3_KEY = 0;
QPixmap* Lib::PX_4_KEY = 0;
QPixmap* Lib::PX_5_KEY = 0;
QPixmap* Lib::PX_6_KEY = 0;
QPixmap* Lib::PX_7_KEY = 0;
QPixmap* Lib::PX_8_KEY = 0;
QPixmap* Lib::PX_9_KEY = 0;
QPixmap* Lib::PX_A_KEY = 0;
QPixmap* Lib::PX_B_KEY = 0;
QPixmap* Lib::PX_C_KEY = 0;
QPixmap* Lib::PX_D_KEY = 0;
QPixmap* Lib::PX_E_KEY = 0;
QPixmap* Lib::PX_F_KEY = 0;
QPixmap* Lib::PX_G_KEY = 0;
QPixmap* Lib::PX_H_KEY = 0;
QPixmap* Lib::PX_I_KEY = 0;
QPixmap* Lib::PX_J_KEY = 0;
QPixmap* Lib::PX_K_KEY = 0;
QPixmap* Lib::PX_L_KEY = 0;
QPixmap* Lib::PX_M_KEY = 0;
QPixmap* Lib::PX_N_KEY = 0;
QPixmap* Lib::PX_O_KEY = 0;
QPixmap* Lib::PX_P_KEY = 0;
QPixmap* Lib::PX_Q_KEY = 0;
QPixmap* Lib::PX_R_KEY = 0;
QPixmap* Lib::PX_S_KEY = 0;
QPixmap* Lib::PX_T_KEY = 0;
QPixmap* Lib::PX_U_KEY = 0;
QPixmap* Lib::PX_V_KEY = 0;
QPixmap* Lib::PX_W_KEY = 0;
QPixmap* Lib::PX_X_KEY = 0;
QPixmap* Lib::PX_Y_KEY = 0;
QPixmap* Lib::PX_Z_KEY = 0;

/*
* STYLE
*/
QString Lib::DARK_STYLE = ""; // set in init()

/*
* WEB HOSTING CONFIG
*/
QString Lib::WEB_CONFIG = "../src/_Res/webapp1.ini";
QString Lib::WEB_URL_PREFIX = "http://remote.hg3n.com/init/ipcheck/";

/*
* LOG FILE CONFIG
*/
QFile Lib::LOG_FILE("../src/_RES/log.txt");

} // namespace Resources
