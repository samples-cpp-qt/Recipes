#include "SpinBoxDelegate.h"

#include <QSpinBox>

SpinBoxDelegate::SpinBoxDelegate(QObject* parent, int minimum, int maximum, int step)
    : QStyledItemDelegate(parent)
    , m_minimum(minimum)
    , m_maximum(maximum)
    , m_step(step)
{
}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    auto editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(m_minimum);
    editor->setMaximum(m_maximum);
    editor->setSingleStep(m_step);
    editor->setAlignment(Qt::AlignHCenter);
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    auto spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}
