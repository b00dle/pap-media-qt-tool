#ifndef SPOTIFY_TILE_CONFIGURE_DIALOG_H
#define SPOTIFY_TILE_CONFIGURE_DIALOG_H

#include <QDialog>

#include <QLineEdit>
#include <QSlider>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>

#include "spotify_remote_controller.h"
#include "resources/web_pixmap.h"

class SpotifyTileConfigureDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SpotifyTileConfigureDialog(QWidget *parent = nullptr);
    explicit SpotifyTileConfigureDialog(const SpotifyRemoteController::Settings &settings,
                                    QWidget *parent = nullptr);
    virtual ~SpotifyTileConfigureDialog();

    const SpotifyRemoteController::Settings &getSettings() const;
    void setSettings(const SpotifyRemoteController::Settings & settings);

signals:

public slots:
    void onSubmit();

private slots:
    void onTextChanged(const QString&);
    void onWebImageChanged();

private:
    void updateUI();
    void updatePlaybackInfo();

    void init();
    void initWidgets();
    void initLayout();

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    static const QUrl getImageUrl(const QJsonObject& info);

    QLineEdit *edit_uri_;
    QLabel* name_label_;
    QLabel* image_container_;
    QLabel* repeat_label_;
    QButtonGroup *repeat_button_group_;
    QRadioButton *radio_repeat_off;
    QRadioButton *radio_repeat_track_;
    QRadioButton *radio_repeat_context_;
    QCheckBox *shuffle_checkbox_;
    QPushButton *btn_submit_;
    QPushButton *btn_cancel_;
    SpotifyRemoteController::Settings settings_;
    WebPixmap web_pixmap_;
};

#endif // SPOTIFY_TILE_CONFIGURE_DIALOG_H
