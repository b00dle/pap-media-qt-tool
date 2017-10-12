#ifndef CATEGORY_TREE_VIEW_H
#define CATEGORY_TREE_VIEW_H

#include <QTreeView>
#include <QMouseEvent>

#include "db/model/category_tree_model.h"

namespace Category {

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);

    void setCategoryTreeModel(DB::Model::CategoryTreeModel* model);

    void mousePressEvent(QMouseEvent *event);

signals:
    void categorySelected(DB::CategoryRecord* rec);

public slots:
    void selectRoot();

private slots:
    void onClicked(QModelIndex const&);

private:
    DB::Model::CategoryTreeModel* model_;

};

} // namespace Category

#endif // TREEVIEW_H
