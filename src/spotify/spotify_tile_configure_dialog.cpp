#include "spotify_tile_configure_dialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include <QPushButton>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "spotify/spotify_handler.h"

SpotifyTileConfigureDialog::SpotifyTileConfigureDialog(QWidget *parent)
    : QDialog(parent)
    , edit_uri_(0)
    , name_label_(0)
    , image_container_(0)
    , repeat_label_(0)
    , repeat_button_group_(0)
    , radio_repeat_off(0)
    , radio_repeat_track_(0)
    , radio_repeat_context_(0)
    , shuffle_checkbox_(0)
    , btn_submit_(0)
    , btn_cancel_(0)
    , settings_()
    , web_pixmap_()

{
    init();
}

SpotifyTileConfigureDialog::SpotifyTileConfigureDialog(const SpotifyRemoteController::Settings &settings, QWidget *parent)
    : QDialog(parent)
    , edit_uri_(0)
    , name_label_(0)
    , repeat_label_(0)
    , repeat_button_group_(0)
    , radio_repeat_off(0)
    , radio_repeat_track_(0)
    , radio_repeat_context_(0)
    , shuffle_checkbox_(0)
    , btn_submit_(0)
    , btn_cancel_(0)
    , settings_(settings)
{
    init();
}

SpotifyTileConfigureDialog::~SpotifyTileConfigureDialog()
{}

const SpotifyRemoteController::Settings &SpotifyTileConfigureDialog::getSettings() const
{
    return settings_;
}

void SpotifyTileConfigureDialog::setSettings(const SpotifyRemoteController::Settings &settings)
{
    settings_ = settings;

    if(settings.mode != SpotifyRemoteController::RC_Playlist ||
       settings.mode != SpotifyRemoteController::RC_Track)
    {
        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "Error: There was no Mode specified... exiting!";
        return;

    } else {
        if(settings.playlist_uri != "") {
            if(settings.track_uri != "") {
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Error: Cannot set spotify tile with two modes!";
            } else {
                settings_.playlist_uri = settings.playlist_uri;
                edit_uri_->setText(settings.playlist_uri);
            }
        } else if(settings.track_uri != "") {
            if(settings.playlist_uri !="") {
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Error: Cannot set spotify tile with two modes!";
            } else {
                settings_.track_uri = settings.track_uri;
                edit_uri_->setText(settings.track_uri);
            }
        } else {
            qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
            qDebug() << "  >" << "Error: There was neither a track nor a playlist URL given.";
        }
        switch (settings.repeat_mode) {
            case SpotifyRemoteController::RM_Context:
                radio_repeat_context_->setChecked(true);
                break;
            case SpotifyRemoteController::RM_Track:
                radio_repeat_track_->setChecked(true);
                break;
            case SpotifyRemoteController::RM_Off:
                radio_repeat_off->setChecked(true);
            default:
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Error: There was no repeat mode set in the given configuration";
                qDebug() << "     >" << "setting context as default mode!";
                radio_repeat_context_->setChecked(true);
                break;
        }
        shuffle_checkbox_->setChecked(settings.shuffle_enabled);
    }
}

void SpotifyTileConfigureDialog::onSubmit()
{
    switch (settings_.mode) {
        case SpotifyRemoteController::RC_Playlist:
            settings_.playlist_uri = edit_uri_->text();
            break;
        case SpotifyRemoteController::RC_Track:
            settings_.track_uri = edit_uri_->text();
            break;
        default:
            break;
    }
    settings_.repeat_mode = (SpotifyRemoteController::RepeatMode) repeat_button_group_->checkedId();
    settings_.shuffle_enabled = shuffle_checkbox_->isChecked();
    accept();
}

void SpotifyTileConfigureDialog::onTextChanged(const QString &t)
{
    if(t.startsWith("https://"))
        settings_.setFromWebLink(t);
    else
        settings_.setFromURI(t);
    updateUI();
    updatePlaybackInfo();
}

void SpotifyTileConfigureDialog::onWebImageChanged()
{
    image_container_->setPixmap(web_pixmap_.getPixmap().scaled(128, 128, Qt::KeepAspectRatio));
}

void SpotifyTileConfigureDialog::updateUI()
{
    if(settings_.playlist_uri.size() > 0)
        edit_uri_->setText(settings_.playlist_uri);
    else if(settings_.track_uri.size() > 0)
        edit_uri_->setText(settings_.track_uri);
    radio_repeat_off->setChecked(settings_.repeat_mode == SpotifyRemoteController::RM_Off);
    radio_repeat_track_->setChecked(settings_.repeat_mode == SpotifyRemoteController::RM_Track);
    radio_repeat_context_->setChecked(settings_.repeat_mode == SpotifyRemoteController::RM_Context);
    shuffle_checkbox_->setChecked(settings_.shuffle_enabled);
}

void SpotifyTileConfigureDialog::updatePlaybackInfo()
{
    QNetworkReply *reply = nullptr;
    if(settings_.mode == SpotifyRemoteController::RC_Playlist) {
        reply = SpotifyHandler::instance()->playlistInfo(settings_.playlist_uri);
    }
    else if(settings_.mode == SpotifyRemoteController::RC_Track) {
        QStringList track_uri_components = settings_.track_uri.split(":");
        reply = SpotifyHandler::instance()->trackInfo(track_uri_components.last());
    }
    // connect to finished event of reply to get the servers response
    connect(reply, &QNetworkReply::finished,
            this, [=]() {
        QJsonDocument playback_info = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();
        if(!playback_info.isObject() || !playback_info.object().contains("name")) {
            name_label_->setText("INVALID RESOURCE");
            web_pixmap_.setUrl(QUrl());
            return;
        }
        QString name(playback_info.object()["name"].toString());
        if(name.size() == 0)
            name_label_->setText("INVALID RESOURCE");
        else
            name_label_->setText(name);
        QUrl url(getImageUrl(playback_info.object()));
        if(!url.isEmpty())
            web_pixmap_.setUrl(url);
    });
}

void SpotifyTileConfigureDialog::init()
{
    initWidgets();
    initLayout();
    setAcceptDrops(true);
    connect(&web_pixmap_, &WebPixmap::contentChanged,
            this, &SpotifyTileConfigureDialog::onWebImageChanged);
}

void SpotifyTileConfigureDialog::initWidgets()
{
    name_label_ = new QLabel(this);
    QFont f(name_label_->font());
    f.setPointSize(f.pointSize()*3);
    name_label_->setFont(f);
    name_label_->setWordWrap(true);

    image_container_ = new QLabel(this);
    image_container_->setMinimumSize(QSize(128,128));

    edit_uri_ = new QLineEdit(this);
    edit_uri_->setReadOnly(true);
    edit_uri_->setPlaceholderText(tr("Drag and drop playlist or track URI/link here"));
    connect(edit_uri_, &QLineEdit::textChanged,
            this, &SpotifyTileConfigureDialog::onTextChanged);

    repeat_label_ = new QLabel("Repeat Mode:", this);
    radio_repeat_off = new QRadioButton("Off");
    radio_repeat_track_ = new QRadioButton("Track");
    radio_repeat_context_ = new QRadioButton("Context");
    repeat_button_group_ = new QButtonGroup;
    repeat_button_group_->addButton(radio_repeat_context_);
    repeat_button_group_->setId(radio_repeat_context_, SpotifyRemoteController::RM_Context);
    repeat_button_group_->addButton(radio_repeat_track_);
    repeat_button_group_->setId(radio_repeat_track_, SpotifyRemoteController::RM_Track);
    repeat_button_group_->addButton(radio_repeat_off);
    repeat_button_group_->setId(radio_repeat_off, SpotifyRemoteController::RM_Off);

    shuffle_checkbox_ = new QCheckBox("Shuffle", this);

    btn_submit_ = new QPushButton("Submit", this);
    connect(btn_submit_, &QPushButton::clicked,
            this, &SpotifyTileConfigureDialog::onSubmit);

    btn_cancel_ = new QPushButton("Cancel", this);
    connect(btn_cancel_, &QPushButton::clicked,
            this, &SpotifyTileConfigureDialog::reject);

    updateUI();
}

void SpotifyTileConfigureDialog::initLayout()
{
    QHBoxLayout* header_layout = new QHBoxLayout;
    header_layout->addWidget(image_container_, 1);
    header_layout->addWidget(name_label_, 3);
    header_layout->setAlignment(name_label_, Qt::AlignCenter);
    header_layout->setContentsMargins(0,0,0,0);

    // mode group
    QHBoxLayout *edit_layout = new QHBoxLayout;
    edit_layout->addWidget(edit_uri_);
    QGroupBox *edit_group = new QGroupBox("URI");
    edit_group->setLayout(edit_layout);

    QGroupBox *option_group = new QGroupBox("Options");
    QHBoxLayout *repeat_layout = new QHBoxLayout;
    repeat_layout->addWidget(repeat_label_);
    repeat_layout->addWidget(radio_repeat_context_);
    repeat_layout->addWidget(radio_repeat_track_);
    repeat_layout->addWidget(radio_repeat_off);

    QVBoxLayout  *inner_option = new QVBoxLayout;
    inner_option->addItem(repeat_layout);
    inner_option->addWidget(shuffle_checkbox_);
    option_group->setLayout(inner_option);

    QHBoxLayout *finalize_layout = new QHBoxLayout;
    finalize_layout->addStretch(2);
    finalize_layout->addWidget(btn_submit_, 1);
    finalize_layout->addWidget(btn_cancel_, 1);

    QVBoxLayout *root = new QVBoxLayout;
    root->addLayout(header_layout);
    root->addWidget(edit_group);
    root->addWidget(option_group);
    root->addLayout(finalize_layout, -1);

    setLayout(root);
    setMinimumWidth(400);
    setFixedSize(minimumSize());
}

void SpotifyTileConfigureDialog::dragEnterEvent(QDragEnterEvent *event)
{
    QString mime_text(event->mimeData()->text());
    if (event->source() != this && mime_text.contains("spotify")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
        return;
    }
}

void SpotifyTileConfigureDialog::dragMoveEvent(QDragMoveEvent *event)
{
    QString mime_text(event->mimeData()->text());
    if (event->source() != this && mime_text.contains("spotify")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
        return;
    }
}

void SpotifyTileConfigureDialog::dropEvent(QDropEvent *event)
{
    QString mime_text(event->mimeData()->text());
    if (event->source() != this && mime_text.contains("spotify")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
        edit_uri_->setText(mime_text);
        return;
    }
}

const QUrl SpotifyTileConfigureDialog::getImageUrl(const QJsonObject& info)
{
    QJsonArray img_array;
    if(info.contains("images") && info["images"].isArray()) {
        img_array = info["images"].toArray();
    }
    else if(info.contains("album") && info["album"].isObject()) {
        QJsonObject album = info["album"].toObject();
        if(album.contains("images") && album["images"].isArray())
            img_array = album["images"].toArray();
    }

    if(img_array.size() == 0 || !img_array[0].toObject().contains("url"))
        return QUrl();

    return QUrl(img_array[0].toObject()["url"].toString());
}
