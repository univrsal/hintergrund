#include "ui_helper.hpp"
#include "util/config.hpp"
#include <cstring>
#include <QTreeWidget>

namespace ui_helper {

    void populate_file_tree(QTreeWidget* tree)
    {
        tree->clear();
        if (strlen(config::values.library_path) < 1)
            return;
        tree->addTopLevelItem(new QTreeWidgetItem(
                                  QStringList(config::values.library_path), 0));
    }
}
