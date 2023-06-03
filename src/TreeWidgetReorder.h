#pragma once

#include <QTreeWidget>

class TreeWidgetReorder: public QTreeWidget
{
Q_OBJECT
public:
    explicit TreeWidgetReorder(QWidget* parent = nullptr);

protected:
    void dropEvent(QDropEvent* event) override;

signals:
    void reordered(int from, int to);

private:
    void removed(const QModelIndex &parent, int first, int last);
    void inserted(const QModelIndex &parent, int first, int last);

    bool m_isDropping = false;
    int m_from = -1;
};
