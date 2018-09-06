#include "map_tile.h"

#include <QAction>
#include <QMenu>
#include <QLayout>
#include <QJsonArray>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>

#include "resources/lib.h"
#include "image/interactive/interactive_image.h"

namespace Tile {

MapTile::MapTile(QGraphicsItem *parent)
    : BaseTile(parent)
    , map_description_()
    , display_widget_(0)
    , image_(nullptr)
{
}

MapTile::~MapTile()
{
}

void MapTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    BaseTile::paint(painter, option, widget);

    QRectF p_rect(getPaintRect());
    if(p_rect.width() > 0 && p_rect.height() > 0) {
        painter->drawPixmap(
                    (int) p_rect.x(),
                    (int) p_rect.y(),
                    (int) p_rect.width(),
                    (int) p_rect.height(),
                    getPlayStatePixmap()
                    );
    }
}

const QJsonObject MapTile::toJsonObject() const
{
    QJsonObject obj = BaseTile::toJsonObject();
    obj["contents"] = map_description_;
    return obj;
}

bool MapTile::setFromJsonObject(const QJsonObject &obj)
{
    if(!BaseTile::setFromJsonObject(obj))
        return false;

    if(obj.contains("contents") && obj["contents"].isObject())
        map_description_ = obj["contents"].toObject();

    return true;
}

void MapTile::setMapDescription(const QJsonObject &obj)
{
    map_description_ = obj;
}

const QJsonObject &MapTile::getMapDescription() const
{
    return map_description_;
}

void MapTile::setImageDisplay(ImageDisplayWidget* display_widget)
{
    display_widget_ = display_widget;
}

ImageDisplayWidget *MapTile::getDisplayWidget() const
{
    return display_widget_;
}

void MapTile::onActivate()
{
    if(is_activated_) {
        display_widget_->popOpen();
        return;
    }

    BaseTile::onActivate();

    if(is_activated_) {
        if(!display_widget_)
            return;

        if(map_description_.isEmpty())
            return;

        image_ = new InteractiveImage(QSize(100,100));
        display_widget_->getView()->setItem(image_);
        image_->setFromJsonObject(map_description_);
        display_widget_->popOpen();
        connect(image_, &InteractiveImage::destroyed,
                [=](){
            saveChanges();
            image_ = nullptr;
            if(is_activated_) {
                is_activated_ = !is_activated_;
                emit activated();
                update();
            }
        });
    }
}

void MapTile::onConfigure()
{
    QString file_name = QFileDialog::getOpenFileName(
        0, tr("Open Map"),
        Resources::Lib::DEFAULT_PROJECT_PATH,
        tr("COMPANION MAP (*.cmpn.map)")
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
                onConfigure();
            else
                return;
        }

        QFileInfo file_info(json_file.fileName());
        QString clean_name = file_info.fileName();
        if(clean_name.endsWith(".cmpn.map"))
            clean_name.remove(clean_name.size()-9, 9);
        setName(clean_name);

        QJsonDocument doc = QJsonDocument::fromJson(json_file.readAll());
        map_description_ = doc.object();

        if(isActivated() && image_) {
            image_->setFromJsonObject(map_description_);
        }
    }
}

void MapTile::saveChanges()
{
    if(!image_)
        return;

    map_description_ = image_->toJsonObject();
}

void MapTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(mode_ != MOVE && e->button() == Qt::LeftButton) {
        if(e->modifiers() & Qt::ControlModifier)
            return BaseTile::mouseReleaseEvent(e);
        onActivate();
    }

    BaseTile::mouseReleaseEvent(e);
}

void MapTile::createContextMenu()
{
    QAction* configure_act = new QAction(tr("Configure..."), this);
    connect(configure_act, &QAction::triggered,
            this, &MapTile::onConfigure);

    context_menu_->addAction(configure_act);
    context_menu_->addSeparator();

    BaseTile::createContextMenu();
}

const QPixmap MapTile::getPlayStatePixmap() const
{
    if(is_activated_)
        return *Resources::Lib::PX_INVISIBLE;
    else
        return *Resources::Lib::PX_VISIBLE;
}

}
