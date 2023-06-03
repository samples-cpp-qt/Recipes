#pragma once

#include "RecipesData.h"

#include <QWidget>

class QTreeWidget;
class QTreeWidgetItem;

namespace Ui
{
class Recipes;
}

class Recipes: public QWidget
{
public:
    Recipes();

private:
    void initRecipes();
    void initSteps();
    void initIngredients();
    void initAddNewRecipe();
    void initRemoveRecipe();
    void loadRecipes();
    void addNewRecipe();
    void removeRecipe();
    void updateRecipes();
    void updateSteps(int recipeIndex);
    void updateTotalTime(int recipeIndex);
    void updateIngredients(int recipeIndex);
    void updateTotalWeight(int recipeIndex);
    void addRecipeItem(const QString& recipeName);
    void addTotalItem(QTreeWidget* widget, int totalValue);
    QTreeWidgetItem* addStepsItem(const Recipe::Step& step);
    QTreeWidgetItem* addIngredientItem(const Recipe::Ingredient& ingredient);

    Ui::Recipes* m_ui;
    RecipesData* m_data;
};
