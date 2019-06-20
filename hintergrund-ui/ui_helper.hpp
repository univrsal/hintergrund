#pragma once
#include <QMap>

class QTreeWidget;
class QTreeWidgetItem;
class image;
class folder;

namespace ui_helper {
    /* Images are modified and therefore not constant */
    extern QMap<const QTreeWidgetItem*, image*> image_tree_map;
    extern QMap<const QTreeWidgetItem*, const folder*> folder_tree_map;

    void populate_file_tree(QTreeWidget* tree);
}
