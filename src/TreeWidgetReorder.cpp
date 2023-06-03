#include "TreeWidgetReorder.h"

TreeWidgetReorder::TreeWidgetReorder(QWidget* parent)
    : QTreeWidget(parent)
{
    connect(this->model(), &QAbstractItemModel::rowsRemoved,
        this, &TreeWidgetReorder::removed);
    connect(this->model(), &QAbstractItemModel::rowsInserted,
        this, &TreeWidgetReorder::inserted);
}

void TreeWidgetReorder::dropEvent(QDropEvent* event)
{
    m_isDropping = true;
    QTreeWidget::dropEvent(event);
}

void TreeWidgetReorder::removed(const QModelIndex&, int first, int last)
{
    if (!m_isDropping || first != last) return;
    m_from = first;
}

void TreeWidgetReorder::inserted(const QModelIndex&, int first, int last)
{
    if (!m_isDropping || first != last) return;
    m_isDropping = false;
    if (m_from != first) emit reordered(m_from, first);
}
