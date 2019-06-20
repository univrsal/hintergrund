#include "ui_helper.hpp"
#include "util/config.hpp"
#include "util/util.hpp"
#include "images/image_library.hpp"
#include <cstring>
#include <QTreeWidget>

namespace ui_helper {
    /* Maps each image item to an image instance */
    QMap<const QTreeWidgetItem*, image*> image_tree_map;
    QMap<const QTreeWidgetItem*, const folder*> folder_tree_map;

    /* Iterates recursively over folder contents and adds them to 'item' */
    void iterate_folder(QTreeWidget* w, QTreeWidgetItem* item, const folder* f, int level)
    {
        /* TODO: item icons? Folder/images */
        if (level > config::values.max_folder_depth) {
            debug("Error: folder depth exceeded maximum of %i!", config::values.max_folder_depth);
            return;
        }

        for (const auto& folder : f->folders()) {
            auto* tree_item = new QTreeWidgetItem(QStringList(folder->path()), 0);
            tree_item->setIcon(0, w->style()->standardIcon(QStyle::SP_DirIcon));
            iterate_folder(w, tree_item, folder.get(), ++level);
            if (tree_item->childCount() > 0) {
                item->addChild(tree_item);
                folder_tree_map[tree_item] = folder.get();
            } else {
                delete tree_item; /* This folder is empty */
            }
        }

        for (auto& file : f->images()) {
            auto* file_item = new QTreeWidgetItem(QStringList(file->name()), 0);
            image_tree_map[file_item] = file.get();
            file_item->setIcon(0, w->style()->standardIcon(QStyle::SP_DesktopIcon));
            item->addChild(file_item);
        }
    }

    void populate_file_tree(QTreeWidget* tree)
    {
        if (!config::values.library->loaded())
            return;
        tree->clear();
        image_tree_map.clear();
        folder_tree_map.clear();

        for (const auto& folder : config::values.library->base_folders()) {
            auto* top_level = new QTreeWidgetItem(QStringList(folder->path()), 0);
            top_level->setIcon(0, tree->style()->standardIcon(QStyle::SP_DirIcon));
            iterate_folder(tree, top_level, folder.get(), 0);
            tree->addTopLevelItem(top_level);
        }
    }
}
