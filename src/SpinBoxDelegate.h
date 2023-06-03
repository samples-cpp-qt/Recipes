#pragma once

#include <QStyledItemDelegate>

class SpinBoxDelegate: public QStyledItemDelegate
{
public:
    SpinBoxDelegate(QObject* parent, int minimum, int maximum, int step);

    QWidget* createEditor(
        QWidget* parent,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(
        QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

private:
    int m_minimum;
    int m_maximum;
    int m_step;
};
