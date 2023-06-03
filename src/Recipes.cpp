#include "Recipes.h"
#include "RecipesData.h"
#include "SpinBoxDelegate.h"

#include <ui_Recipes.h>

#include <QFile>
#include <QMessageBox>

QString filePath() { return QApplication::applicationDirPath() + "/recipes.data"; }

Recipes::Recipes()
    : m_ui(new Ui::Recipes())
    , m_data(new RecipesData(this))
{
    m_ui->setupUi(this);

    initRecipes();
    loadRecipes();

    initAddNewRecipe();
    initRemoveRecipe();

    initSteps();
    initIngredients();

    updateRecipes();
    connect(m_data, &RecipesData::updated, [&]{ m_data->save(filePath()); });
}

void Recipes::initRecipes()
{
    connect(m_ui->recipes, &QListWidget::itemChanged, [&](QListWidgetItem* item)
    {
        int recipeIndex = m_ui->recipes->currentRow();
        m_data->updateRecipeName(recipeIndex, item->text());
    });
    connect(m_ui->recipes, &ListWidgetReorder::reordered, [&] (int from, int to)
    {
       m_data->moveRecipe(from, to);
    });
}

void Recipes::initSteps()
{
    connect(m_ui->recipes, &QListWidget::currentRowChanged, this, &Recipes::updateSteps);
    connect(m_ui->steps, &QTreeWidget::itemChanged, [&] (QTreeWidgetItem* item, int column)
    {
        int recipeIndex = m_ui->recipes->currentRow();
        int stepIndex = m_ui->steps->invisibleRootItem()->indexOfChild(item);
        if (stepIndex >= m_ui->steps->invisibleRootItem()->childCount() - 1) return;
        Recipe::Step step = m_data->recipes().at(recipeIndex).steps.at(stepIndex);
        QString text = item->text(column);
        if (column == 0) step.action = text;
        if (column == 1) step.timeMinutes = text.toInt();
        m_data->updateRecipeStep(recipeIndex, stepIndex, step);
        updateTotalTime(recipeIndex);
    });
    m_ui->steps->setItemDelegateForColumn(1, new SpinBoxDelegate(this, 0, 1000, 1));
    auto header = m_ui->steps->header();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    connect(m_ui->addStep, &QToolButton::clicked, [&]
    {
        int recipeIndex = m_ui->recipes->currentRow();
        Recipe::Step step;
        m_data->addRecipeStep(recipeIndex, step);
        m_ui->steps->editItem(addStepsItem(step));
        updateTotalTime(recipeIndex);
    });
    connect(m_ui->removeStep, &QToolButton::clicked, [&]
    {
        int recipeIndex = m_ui->recipes->currentRow();
        int stepIndex = m_ui->steps->invisibleRootItem()->indexOfChild(m_ui->steps->currentItem());
        m_data->removeRecipeStep(recipeIndex, stepIndex);
        delete m_ui->steps->takeTopLevelItem(stepIndex);
        updateTotalTime(recipeIndex);
    });
    connect(m_ui->steps, &TreeWidgetReorder::reordered, [&] (int from, int to)
    {
        int recipeIndex = m_ui->recipes->currentRow();
        if (to >= m_data->recipes().at(recipeIndex).steps.size())
        {
            auto totalItem = m_ui->steps->takeTopLevelItem(m_ui->steps->topLevelItemCount() - 2);
            m_ui->steps->addTopLevelItem(totalItem);
            --to;
        }
        m_data->moveRecipeStep(recipeIndex, from, to);
    });
}

void Recipes::initIngredients()
{
    connect(m_ui->recipes, &QListWidget::currentRowChanged, this, &Recipes::updateIngredients);
    connect(m_ui->ingredients, &QTreeWidget::itemChanged, [&] (QTreeWidgetItem* item, int column)
    {
        int recipeIndex = m_ui->recipes->currentRow();
        int ingredientIndex = m_ui->ingredients->invisibleRootItem()->indexOfChild(item);
        if (ingredientIndex >= m_ui->ingredients->invisibleRootItem()->childCount() - 1) return;
        Recipe::Ingredient ingredient = m_data->recipes().at(recipeIndex).ingredients.at(ingredientIndex);
        QString text = item->text(column);
        if (column == 0) ingredient.name = text;
        if (column == 1) ingredient.weightGrams = text.toInt();
        m_data->updateRecipeIngredient(recipeIndex, ingredientIndex, ingredient);
        updateTotalWeight(recipeIndex);
    });
    m_ui->ingredients->setItemDelegateForColumn(1, new SpinBoxDelegate(this, 0, 10000, 10));
    auto header = m_ui->ingredients->header();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    connect(m_ui->addIngredient, &QToolButton::clicked, [&]
    {
        int recipeIndex = m_ui->recipes->currentRow();
        Recipe::Ingredient ingredient;
        m_data->addRecipeIngredient(recipeIndex, ingredient);
        m_ui->ingredients->editItem(addIngredientItem(ingredient));
        updateTotalWeight(recipeIndex);
    });
    connect(m_ui->removeIngredient, &QToolButton::clicked, [&]
    {
        int recipeIndex = m_ui->recipes->currentRow();
        int ingredientIndex = m_ui->ingredients->invisibleRootItem()->indexOfChild(
            m_ui->ingredients->currentItem());
        m_data->removeRecipeIngredient(recipeIndex, ingredientIndex);
        delete m_ui->ingredients->takeTopLevelItem(ingredientIndex);
        updateTotalWeight(recipeIndex);
    });
    connect(m_ui->ingredients, &TreeWidgetReorder::reordered, [&] (int from, int to)
    {
        int recipeIndex = m_ui->recipes->currentRow();
        if (to >= m_data->recipes().at(recipeIndex).ingredients.size())
        {
            auto totalItem = m_ui->ingredients->takeTopLevelItem(m_ui->ingredients->topLevelItemCount() - 2);
            m_ui->ingredients->addTopLevelItem(totalItem);
            --to;
        }
        m_data->moveRecipeIngredient(recipeIndex, from, to);
    });
}

void Recipes::initAddNewRecipe()
{
    connect(m_ui->addNewRecipe, &QPushButton::clicked, this, &Recipes::addNewRecipe);
    connect(m_ui->recipeName, &QLineEdit::returnPressed, this, &Recipes::addNewRecipe);
}

void Recipes::initRemoveRecipe()
{
    connect(m_ui->removeRecipe, &QPushButton::clicked, this, &Recipes::removeRecipe);
    connect(m_ui->recipes, &QListWidget::currentRowChanged, [&] (int row)
    {
        m_ui->removeRecipe->setEnabled(row != -1);
    });
}

void Recipes::loadRecipes()
{
    if (QFile::exists(filePath()))
    {
        if (!m_data->load(filePath()))
            QMessageBox::warning(
                this,
                QString::fromUtf16(u"Ошибка"),
                QString::fromUtf16(u"Файл с рецептами не может быть загружен!")
            );
    }
}

void Recipes::addNewRecipe()
{
    const auto& name = m_ui->recipeName->text();
    if (!name.isEmpty())
    {
        m_data->addNewRecipe(name);
        m_ui->recipeName->clear();
        addRecipeItem(name);
        m_ui->recipes->setCurrentRow(m_ui->recipes->count() - 1);
    }
}

void Recipes::removeRecipe()
{
    int recipeIndex = m_ui->recipes->currentRow();
    QString recipeName = m_data->recipes().at(recipeIndex).name;
    auto button = QMessageBox::warning(
        this,
        QString::fromUtf16(u"Удаление Рецепта"),
        QString::fromUtf16(u"Удалить Рецепт \"%1\"?").arg(recipeName),
        QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::Yes)
    {
        m_data->removeRecipe(recipeIndex);
        delete m_ui->recipes->takeItem(recipeIndex);
    }
}

void Recipes::updateRecipes()
{
    m_ui->recipes->clear();
    for (const auto& recipe : m_data->recipes())
        addRecipeItem(recipe.name);
    m_ui->recipes->setCurrentRow(0);
}

void Recipes::updateSteps(int recipeIndex)
{
    m_ui->steps->clear();
    if (recipeIndex < 0 || recipeIndex >= m_data->recipes().size()) return;
    const auto& recipe = m_data->recipes().at(recipeIndex);
    addTotalItem(m_ui->steps, recipe.totalTime);
    for (const auto& step : recipe.steps)
        addStepsItem(step);
}

void Recipes::updateTotalTime(int recipeIndex)
{
    auto rootItem = m_ui->steps->invisibleRootItem();
    auto totalItem = rootItem->child(rootItem->childCount() - 1);
    totalItem->setText(1, QString::number(m_data->recipes().at(recipeIndex).totalTime));
}

void Recipes::updateIngredients(int recipeIndex)
{
    m_ui->ingredients->clear();
    if (recipeIndex < 0 || recipeIndex >= m_data->recipes().size()) return;
    const auto& recipe = m_data->recipes().at(recipeIndex);
    addTotalItem(m_ui->ingredients, recipe.totalWeight);
    for (const auto& ingredient : recipe.ingredients)
        addIngredientItem(ingredient);
}

void Recipes::updateTotalWeight(int recipeIndex)
{
    auto rootItem = m_ui->ingredients->invisibleRootItem();
    auto totalItem = rootItem->child(rootItem->childCount() - 1);
    totalItem->setText(1, QString::number(m_data->recipes().at(recipeIndex).totalWeight));
}

void Recipes::addRecipeItem(const QString& recipeName)
{
    auto item = new QListWidgetItem(recipeName);
    item->setFlags((item->flags() | Qt::ItemIsEditable) & ~Qt::ItemIsDropEnabled);
    m_ui->recipes->addItem(item);
}

void Recipes::addTotalItem(QTreeWidget* widget, int totalValue)
{
    auto totalItem = new QTreeWidgetItem({QString::fromUtf16(u"Итого:"), QString::number(totalValue)});
    totalItem->setTextAlignment(0, Qt::AlignRight);
    totalItem->setTextAlignment(1, Qt::AlignCenter);
    totalItem->setFlags(Qt::ItemIsEnabled);
    widget->invisibleRootItem()->addChild(totalItem);
}

QTreeWidgetItem* Recipes::addStepsItem(const Recipe::Step& step)
{
    auto item = new QTreeWidgetItem({step.action, QString::number(step.timeMinutes)});
    item->setFlags((item->flags() | Qt::ItemIsEditable) & ~Qt::ItemIsDropEnabled);
    item->setTextAlignment(1, Qt::AlignCenter);
    auto rootItem = m_ui->steps->invisibleRootItem();
    rootItem->insertChild(rootItem->childCount() - 1, item);
    return item;
}

QTreeWidgetItem* Recipes::addIngredientItem(const Recipe::Ingredient& ingredient)
{
    auto item = new QTreeWidgetItem({ingredient.name, QString::number(ingredient.weightGrams)});
    item->setFlags((item->flags() | Qt::ItemIsEditable) & ~Qt::ItemIsDropEnabled);
    item->setTextAlignment(1, Qt::AlignCenter);
    auto rootItem = m_ui->ingredients->invisibleRootItem();
    rootItem->insertChild(rootItem->childCount() - 1, item);
    return item;
}
