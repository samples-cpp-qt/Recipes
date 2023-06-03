#include "ListWidgetReorder.h"

ListWidgetReorder::ListWidgetReorder(QWidget* parent)
    : QListWidget(parent)
{
    connect(this->model(), &QAbstractItemModel::rowsMoved,
        this, &ListWidgetReorder::moved);
}

void ListWidgetReorder::moved(const QModelIndex&, int start, int end, const QModelIndex&, int row)
{
    if (start != end) return;
    if (start < row) --row;
    emit reordered(start, row);
}
