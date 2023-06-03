#pragma once

#include <QAbstractItemModel>
#include <QWidget>

template <typename BaseWidget>
class Reorder: public BaseWidget
{
public:
    explicit Reorder(QWidget* parent = nullptr)
        : BaseWidget(parent)
    {
        connect(this->model(), );
    }
};