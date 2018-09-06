#include "settings_widget.h"

#include <QHBoxLayout>
#include <QRadioButton>
#include <QFileDialog>

#include "misc/volume_mapper.h"

namespace Playlist{

SettingsWidget::SettingsWidget(const Playlist::Settings& settings, QWidget *parent)
    : QWidget(parent)
    , settings_(settings)
    , name_edit_(0)
    , loop_checkbox_(0)
    , interval_groupbox_(0)
    , min_interval_slider_(0)
    , max_interval_slider_(0)
    , interval_label_(0)
    , volume_slider_(0)
    , volume_label_(0)
    , normal_radio_button_(0)
    , shuffle_radio_button_(0)
    , weighted_radio_button_(0)
    , save_button_(0)
    , close_button_(0)
    , image_path_edit_(0)
    , image_path_button_(0)
    , image_clear_button_(0)
{
    initWidgets();
    initLayout();
}

SettingsWidget::~SettingsWidget()
{
}

void SettingsWidget::onCloseClicked(bool)
{
    emit closed();
}

void SettingsWidget::onSaveClicked(bool)
{
    Settings new_settings;

    //set loop settings
    if (loop_checkbox_->isChecked()){
        new_settings.loop_flag = true;
    } else {
        new_settings.loop_flag = false;
    }

    //set delay interval settings
    if (interval_groupbox_->isChecked()){
        new_settings.min_delay_interval = min_interval_slider_->value();
        new_settings.max_delay_interval = max_interval_slider_->value();

        if (new_settings.max_delay_interval == 0){
            new_settings.interval_flag = false;
        } else {
            new_settings.interval_flag = true;
        }
    } else {
        new_settings.interval_flag = false;
        new_settings.min_delay_interval = 0;
        new_settings.max_delay_interval = 0;
    }

    //set playorder settings
    if (normal_radio_button_->isChecked())
    {
        new_settings.order = PlayOrder::ORDERED;
    } else if (shuffle_radio_button_->isChecked())
    {
        new_settings.order = PlayOrder::SHUFFLE;
    } else if (weighted_radio_button_->isChecked())
    {
        new_settings.order = PlayOrder::WEIGHTED;
    }

    //set volume
    new_settings.volume = VolumeMapper::logarithmicToLinear(volume_slider_->value());
    new_settings.name = name_edit_->text();

    if (image_path_edit_->text().size() > 0)
        new_settings.image_path = image_path_edit_->text();

    emit saved(new_settings);
}

void SettingsWidget::popOpen()
{
    if(isHidden())
        showNormal();
    else
        show();
    raise();
    activateWindow();
}

void SettingsWidget::onExternalVolumeChanged(int v)
{
    volume_slider_->setValue(VolumeMapper::linearToLogarithmic(v));
}

void SettingsWidget::onMinIntervalSliderChanged(int val)
{
    int max = max_interval_slider_->value();
    interval_label_->setText(QString::number(val) + "-" + QString::number(max) + " sec");
}

void SettingsWidget::onMaxIntervalSliderChanged(int val)
{
    int min = min_interval_slider_->value();
    interval_label_->setText(QString::number(min) + "-" + QString::number(val) + " sec");
}


void SettingsWidget::onVolumeSliderChanged(int val)
{
    volume_label_->setText(QString::number(val) + " %");
    emit volumeSettingsChanged(VolumeMapper::logarithmicToLinear(val));
}

void SettingsWidget::onOpenImage()
{
    // QString s = QFileDialog::getOpenFileName(this, tr("Open Image"), "", "Image Files (*.png *.jpg *.bmp)");
    QString s = QFileDialog::getOpenFileName(this, tr("Open Image"), "", "Image Files (*.png *.jpg *.bmp)",nullptr, QFileDialog::DontUseNativeDialog);
    if(s.size() > 0) {
        qDebug() << s;
        image_path_edit_->setText(s);
    }
}

void SettingsWidget::initWidgets()
{
    name_edit_ = new QLineEdit(this);
    name_edit_->setPlaceholderText("Playlist Name");
    if(settings_.name.size() > 0)
        name_edit_->setText(settings_.name);

    loop_checkbox_ = new QCheckBox(tr("Loop Contents"),this);
    if (settings_.loop_flag){
        loop_checkbox_->setChecked(true);
    } else {
        loop_checkbox_->setChecked(false);
    }

    interval_groupbox_ = new QGroupBox(tr("Interval Playback"));
    interval_groupbox_->setCheckable(true);
    if (settings_.interval_flag){
        interval_groupbox_->setChecked(true);
    } else {
        interval_groupbox_->setChecked(false);
    }

    min_interval_slider_ = new QSlider(Qt::Horizontal,this);
    min_interval_slider_->setMinimum(0);
    min_interval_slider_->setMaximum(60);
    max_interval_slider_ = new QSlider(Qt::Horizontal,this);
    interval_label_ = new QLabel(this);

    connect(min_interval_slider_, SIGNAL(valueChanged(int)),
            this, SLOT(onMinIntervalSliderChanged(int)));
    connect(max_interval_slider_, SIGNAL(valueChanged(int)),
            this, SLOT(onMaxIntervalSliderChanged(int)));

    min_interval_slider_->setValue(settings_.min_delay_interval);
    max_interval_slider_->setMinimum(0);
    max_interval_slider_->setMaximum(60);
    max_interval_slider_->setValue(settings_.max_delay_interval);

    volume_slider_ = new QSlider(Qt::Horizontal,this);
    volume_label_ = new QLabel(this);
    volume_slider_->setMinimum(0);
    volume_slider_->setMaximum(100);

    connect(volume_slider_, SIGNAL(valueChanged(int)),
            this, SLOT(onVolumeSliderChanged(int)));

    int log_volume = VolumeMapper::linearToLogarithmic(settings_.volume);
    volume_slider_->setValue(log_volume);

    normal_radio_button_ = new QRadioButton(tr("Normal"),this);
    shuffle_radio_button_ = new QRadioButton(tr("Random"),this);
    weighted_radio_button_ = new QRadioButton(tr("Weighted"),this);

    if (settings_.order == PlayOrder::ORDERED){
        normal_radio_button_->setChecked(true);
    } else if (settings_.order == PlayOrder::SHUFFLE){
        shuffle_radio_button_->setChecked(true);
        normal_radio_button_->setChecked(false);
    } else if (settings_.order == PlayOrder::WEIGHTED){
        weighted_radio_button_->setChecked(true);
    }

    image_path_edit_ = new QLineEdit(this);
    image_path_edit_->setReadOnly(true);
    image_path_edit_->setText(settings_.image_path);
    image_path_button_ = new QPushButton(tr("Import..."), this);
    image_clear_button_ = new QPushButton(tr("Clear"), this);

    connect(image_path_button_, SIGNAL(clicked()),
            this, SLOT(onOpenImage()));
    connect(image_clear_button_, SIGNAL(clicked()),
            image_path_edit_, SLOT(clear()));

    save_button_ = new QPushButton("Save", this);
    close_button_ = new QPushButton("Cancel", this);

    connect(close_button_, SIGNAL(clicked(bool)),
            this, SLOT(onCloseClicked(bool)));
    connect(save_button_, SIGNAL(clicked(bool)),
            this, SLOT(onSaveClicked(bool)));
}

void SettingsWidget::initLayout()
{
    QWidget::setWindowFlags(Qt::Tool);
    QWidget::setWindowTitle("Playlist Settings");

    QGroupBox* name_box = new QGroupBox(tr("Playlist Name"),this);
    QVBoxLayout *name_layout = new QVBoxLayout;
    name_layout->addWidget(name_edit_);
    name_layout->addWidget(loop_checkbox_);
    name_box->setLayout(name_layout);

    //playmode Settings
    QGroupBox *playmode_box = new QGroupBox(tr("Playmode Options"),this);
    QVBoxLayout *playmode_layout = new QVBoxLayout;
    playmode_layout->addWidget(normal_radio_button_);
    playmode_layout->addWidget(shuffle_radio_button_);
    playmode_layout->addWidget(weighted_radio_button_);
    playmode_layout->addStretch(1);
    playmode_box->setLayout(playmode_layout);

    //playmode Settings
    QVBoxLayout *interval_layout = new QVBoxLayout;
    interval_layout->addWidget(min_interval_slider_);
    interval_layout->addWidget(max_interval_slider_);
    interval_layout->addWidget(interval_label_);
    interval_groupbox_->setLayout(interval_layout);

    QGroupBox *volume_box = new QGroupBox(tr("Volume Options"),this);
    QVBoxLayout *volume_layout = new QVBoxLayout;
    volume_layout->addWidget(volume_slider_);
    volume_layout->addWidget(volume_label_);
    volume_box->setLayout(volume_layout);


    //Grid for all settings
    QGridLayout *grid_layout = new QGridLayout;
    grid_layout->addWidget(name_box,0,0,1,1);
    grid_layout->addWidget(interval_groupbox_,0,1,1,1);
    grid_layout->addWidget(playmode_box,1,0,1,1);
    grid_layout->addWidget(volume_box,1,1,1,1);

    //image open
    QGroupBox* image_box = new QGroupBox(tr("Background Image"), this);
    QHBoxLayout* image_layout = new QHBoxLayout;
    image_layout->addWidget(image_path_edit_, 9);
    image_layout->addWidget(image_path_button_, 1);
    image_layout->addWidget(image_clear_button_, 0);
    image_box->setLayout(image_layout);

    //name,save, close button
    QGroupBox *bottom_box = new QGroupBox(this);
    QHBoxLayout *bottom_layout = new QHBoxLayout;
    bottom_layout->addWidget(save_button_, 1);
    bottom_layout->addWidget(close_button_, -1);
    bottom_box->setLayout(bottom_layout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(grid_layout);
    layout->addWidget(image_box);
    layout->addWidget(bottom_box);
    setLayout(layout);

    setFixedHeight(400);
    setFixedWidth(600);
}

void SettingsWidget::closeEvent(QCloseEvent *e)
{
    emit closed();
    QWidget::closeEvent(e);
}

} //namespace Playlist
