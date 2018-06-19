#include "companion_widget.h"

#include <QDebug>
#include <QDir>
#include <QKeySequence>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>

#include "db/core/api.h"
#include "resources/lib.h"
#include "misc/json_mime_data_parser.h"
#include "spotify/spotify_handler.h"

CompanionWidget::CompanionWidget(QWidget *parent)
    : QWidget(parent)
    , project_name_("")
    , progress_bar_(0)
    , actions_()
    , main_menu_(0)
    , sound_file_view_(0)
    , category_view_(0)
    , preset_view_(0)
    , graphics_view_(0)
    , sound_file_importer_(0)
    , center_h_splitter_(0)
    , left_v_splitter_(0)
    , left_box_(0)
    , right_box_(0)
    , image_browser_(0)
    , spotify_authenticator_widget_(0)
    , left_tabwidget_(0)
    , spotify_menu_(0)
    , db_handler_(0)
{
    initDB();
    initWidgets();
    initLayout();
    initActions();
    initMenu();
}

CompanionWidget::~CompanionWidget()
{
    if(spotify_authenticator_widget_) {
        spotify_authenticator_widget_->deleteLater();
    }
}

QMenu *CompanionWidget::getMenu()
{
    return main_menu_;
}

const QString &CompanionWidget::getStatusMessage() const
{
    return status_message_;
}

QProgressBar *CompanionWidget::getProgressBar() const
{
    return progress_bar_;
}


void CompanionWidget::onProgressChanged(int value)
{
    if(value != 100) {
        if(progress_bar_->isHidden()) {
            progress_bar_->show();
        }
    }
    else {
        progress_bar_->hide();
    }
    progress_bar_->setValue(value);
}

void CompanionWidget::onSelectedCategoryChanged(DB::CategoryRecord *rec)
{
    int id = -1;
    if(rec != 0)
        id = rec->id;

    sound_file_view_->setSoundFiles(db_handler_->getSoundFileRecordsByCategoryId(id));
}

void CompanionWidget::onDeleteDatabase()
{
    db_handler_->deleteAll();
    category_view_->selectRoot();
}

void CompanionWidget::onSaveProjectAs()
{
    QString file_name = QFileDialog::getSaveFileName(
        this, tr("Save Project"),
        Resources::Lib::DEFAULT_PROJECT_PATH,
        tr("JSON (*.json)")
    );

    if(file_name.size() > 0) {
        setProjectPath(file_name);
        onSaveProject();
    }
}

void CompanionWidget::onSaveProject()
{
    if(project_name_.size() > 0) {
        QJsonDocument doc;

        doc.setObject(graphics_view_->toJsonObject());

        QFile json_file(project_name_);
        json_file.open(QFile::WriteOnly);
        json_file.write(doc.toJson());
    }
    else {
        onSaveProjectAs();
    }
}

void CompanionWidget::onOpenProject()
{
    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Open Project"),
        Resources::Lib::DEFAULT_PROJECT_PATH,
        tr("JSON (*.json)")
    );

    if(file_name.size() > 0) {
        QFile json_file(file_name);

        // opening failed
        if(!json_file.open(QFile::ReadOnly)) {
            QMessageBox b;
            b.setText(tr("The selected file could not be opened."));
            b.setInformativeText(tr("Do you wish to select a different file?"));
            b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            b.setDefaultButton(QMessageBox::Yes);
            if(b.exec() == QMessageBox::Yes)
                onOpenProject();
            else
                return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(json_file.readAll());

        // graphics view could not be set from json
        if(!graphics_view_->setFromJsonObject(doc.object())) {
            QMessageBox b;
            b.setText(tr("The selected file does not seem to contain valid project data."));
            b.setInformativeText(tr("Do you wish to select a different file?"));
            b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            b.setDefaultButton(QMessageBox::Yes);
            if(b.exec() == QMessageBox::Yes)
                onOpenProject();
        }

        setProjectPath(file_name);
    }
}

void CompanionWidget::onStartSpotifyControlWidget()
{
    if(spotify_authenticator_widget_ == 0) {
        spotify_authenticator_widget_ = new SpotifyControlPanel;
    }
    if(spotify_authenticator_widget_->isVisible()) {
        spotify_authenticator_widget_->raise();
        spotify_authenticator_widget_->activateWindow();
    }
    else {
        spotify_authenticator_widget_->show();
    }
}

void CompanionWidget::setProjectPath(const QString &path)
{
    if (path.size() == 0) {
        project_name_ = "";
        actions_["Save Project"]->setText("Save Project");
    }
    else {
        project_name_ = path;
        QString file_name = path.split("/").back();
        actions_["Save Project"]->setText("Save Project '"+file_name.split(".").front()+"'");
    }
}

void CompanionWidget::initWidgets()
{
    sound_file_view_ = new SoundFile::MasterView(
        db_handler_->getSoundFileTableModel()->getSoundFiles(),
        this
    );

    progress_bar_ = new QProgressBar;
    progress_bar_->setMaximum(100);
    progress_bar_->setMinimum(0);
    progress_bar_->setValue(100);
    progress_bar_->hide();

    graphics_view_ = new Tile::GraphicsView(this);
    graphics_view_->setSoundFileModel(db_handler_->getSoundFileTableModel());
    graphics_view_->setPresetModel(db_handler_->getPresetTableModel());

    sound_file_importer_ = new Resources::Importer(
        db_handler_->getResourceDirTableModel(),
        this
    );

    category_view_ = new Category::TreeView(this);
    category_view_->setCategoryTreeModel(db_handler_->getCategoryTreeModel());

    preset_view_ = new Preset::PresetView(this);
    preset_view_->setPresetTableModel(db_handler_->getPresetTableModel());

    left_box_ = new QGroupBox(this);
    right_box_ = new QGroupBox(this);

    center_h_splitter_ = new QSplitter(Qt::Horizontal);
    center_h_splitter_->addWidget(left_box_);
    center_h_splitter_->addWidget(right_box_);
    center_h_splitter_->setStretchFactor(0, 0);
    center_h_splitter_->setStretchFactor(1, 10);

    left_tabwidget_ = new QTabWidget(this);

    left_v_splitter_ = new QSplitter(Qt::Vertical, left_tabwidget_);
    left_v_splitter_->addWidget(category_view_);
    left_v_splitter_->addWidget(sound_file_view_);
    left_v_splitter_->setStretchFactor(0, 2);
    left_v_splitter_->setStretchFactor(1, 8);

    image_browser_ = new Image::Browser(left_tabwidget_);
    image_browser_->layout()->setMargin(0);
    image_browser_->setImageDirTableModel(db_handler_->getImageDirTableModel());

    left_tabwidget_->addTab(left_v_splitter_, tr("Sounds"));
    left_tabwidget_->addTab(image_browser_, tr("Images"));
    left_tabwidget_->addTab(preset_view_, tr("Presets"));

    connect(sound_file_importer_, SIGNAL(folderImported(QList<Resources::SoundFile> const&)),
            db_handler_, SLOT(insertSoundFilesAndCategories(QList<Resources::SoundFile> const&)));
    connect(sound_file_importer_, SIGNAL(folderImported()),
            category_view_, SLOT(selectRoot()));
    connect(db_handler_, SIGNAL(progressChanged(int)),
            this, SLOT(onProgressChanged(int)));
    connect(category_view_, SIGNAL(categorySelected(DB::CategoryRecord*)),
            this, SLOT(onSelectedCategoryChanged(DB::CategoryRecord*)));
    connect(sound_file_view_, SIGNAL(deleteSoundFileRequested(int)),
            db_handler_->getSoundFileTableModel(), SLOT(deleteSoundFile(int)));
    connect(db_handler_->getSoundFileTableModel(), SIGNAL(aboutToBeDeleted(DB::SoundFileRecord*)),
            sound_file_view_, SLOT(onSoundFileAboutToBeDeleted(DB::SoundFileRecord*)));
    connect(graphics_view_, SIGNAL(dropAccepted()),
            sound_file_view_, SLOT(onDropSuccessful()));
}

void CompanionWidget::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    // left layout
    QVBoxLayout* l_layout = new QVBoxLayout;
    l_layout->addWidget(left_tabwidget_);
    left_box_->setLayout(l_layout);

    // right layout
    QVBoxLayout* r_layout = new QVBoxLayout;
    r_layout->addWidget(graphics_view_);
    right_box_->setLayout(r_layout);

    layout->addWidget(center_h_splitter_);

    setLayout(layout);
}

void CompanionWidget::initActions()
{
    actions_["Import Resource Folder..."] = new QAction(tr("Import Resource Folder..."), this);
    actions_["Import Resource Folder..."]->setToolTip(tr("Imports a folder of resources into the program."));
    actions_["Import Resource Folder..."]->setShortcut(QKeySequence(tr("Ctrl+Shift+O")));

    actions_["Delete Database Contents..."] = new QAction(tr("Delete Database Contents..."), this);
    actions_["Delete Database Contents..."]->setToolTip(tr("Deletes all contents from application database."));

    actions_["Save Project As..."] = new QAction(tr("Save Project As..."), this);
    actions_["Save Project As..."]->setToolTip(tr("Saves the current work state to a file."));
    actions_["Save Project As..."]->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));

    actions_["Save Project"] = new QAction(tr("Save Project"), this);
    actions_["Save Project"]->setToolTip(tr("Saves the current work state to a file."));
    actions_["Save Project"]->setShortcut(QKeySequence(tr("Ctrl+S")));

    actions_["Open Project..."] = new QAction(tr("Open Project..."), this);
    actions_["Open Project..."]->setToolTip(tr("Opens a previously saved state from a file."));
    actions_["Open Project..."]->setShortcut(QKeySequence(tr("Ctrl+O")));

    actions_["Connect to Spotify"] = new QAction(tr("Connect to Spotify"), this);
    actions_["Connect to Spotify"]->setToolTip(tr("Requests connection to the Spotify music streaming service."));
    actions_["Connect to Spotify"]->setCheckable(true);
    actions_["Connect to Spotify"]->setChecked(false);
    actions_["Spotify Control Panel..."] = new QAction(tr("Spotify Control Panel..."), this);
    actions_["Spotify Control Panel..."]->setToolTip(tr("Shows the Spotify control panel."));
    actions_["Spotify Control Panel..."]->setEnabled(false);
    actions_["Spotify Control Panel..."]->setShortcut(QKeySequence(tr("Ctrl+Y")));
    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
            this, [=]() {
        actions_["Connect to Spotify"]->setChecked(true);
        actions_["Connect to Spotify"]->setEnabled(false);
        actions_["Spotify Control Panel..."]->setEnabled(true);
    });

    connect(actions_["Import Resource Folder..."] , SIGNAL(triggered(bool)),
            sound_file_importer_, SLOT(startBrowseFolder(bool)));
    connect(actions_["Delete Database Contents..."], SIGNAL(triggered()),
            this, SLOT(onDeleteDatabase()));
    connect(actions_["Save Project As..."], SIGNAL(triggered()),
            this, SLOT(onSaveProjectAs()));
    connect(actions_["Save Project"], SIGNAL(triggered()),
            this, SLOT(onSaveProject()));
    connect(actions_["Open Project..."], SIGNAL(triggered()),
            this, SLOT(onOpenProject()));
    connect(actions_["Connect to Spotify"], SIGNAL(triggered()),
            this, SLOT(onStartSpotifyControlWidget()));
    connect(actions_["Spotify Control Panel..."], SIGNAL(triggered()),
            this, SLOT(onStartSpotifyControlWidget()));
}

void CompanionWidget::initMenu()
{
    main_menu_ = new QMenu(tr("DsaMediaControlKit"));

    QMenu* file_menu = main_menu_->addMenu(tr("File"));
    file_menu->addAction(actions_["Save Project"]);
    file_menu->addAction(actions_["Save Project As..."]);
    file_menu->addAction(actions_["Open Project..."]);
    file_menu->addSeparator();
    file_menu->addAction(actions_["Import Resource Folder..."]);
    file_menu->addSeparator();
    file_menu->addAction(actions_["Delete Database Contents..."]);
    QMenu* tool_menu = main_menu_->addMenu(tr("Tools"));
    spotify_menu_ = tool_menu->addMenu(tr("Spotify"));
    spotify_menu_->addAction(actions_["Connect to Spotify"]);
    spotify_menu_->addAction(actions_["Spotify Control Panel..."]);

    main_menu_->addMenu(file_menu);
    main_menu_->addMenu(tool_menu);
}

void CompanionWidget::initDB()
{
    DB::Core::Api* db_api = nullptr;
#ifdef _WIN32
    db_api = new DB::Core::Api(Resources::Lib::DATABASE_PATH, this);
#else
    db_api = new DB::Core::Api(QCoreApplication::applicationDirPath() + Resources::Lib::DATABASE_PATH, this);
#endif
    db_handler_ = new DB::Handler(db_api, this);
}
