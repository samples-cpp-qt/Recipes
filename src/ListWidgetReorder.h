#pragma once

#include <QListWidget>

class ListWidgetReorder: public QListWidget
{
Q_OBJECT
public:
    explicit ListWidgetReorder(QWidget* parent = nullptr);

signals:
    void reordered(int from, int to);

private:
    void moved(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row);
};
