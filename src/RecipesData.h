#pragma once

#include <QObject>

struct Recipe
{
    struct Step
    {
        QString action;
        int timeMinutes = 0;
    };

    struct Ingredient
    {
        QString name;
        int weightGrams = 0;
    };

    QString name;
    QVector<Step> steps;
    QVector<Ingredient> ingredients;
    int totalTime = 0;
    int totalWeight = 0;
};

class RecipesData: public QObject
{
Q_OBJECT
public:
    using QObject::QObject;

    const QVector<Recipe>& recipes() const { return m_recipes; }

    void addNewRecipe(const QString& name);
    void removeRecipe(int recipeIndex);
    void updateRecipeName(int recipeIndex, const QString& name);
    void moveRecipe(int fromIndex, int toIndex);

    void addRecipeStep(int recipeIndex, const Recipe::Step& step);
    void removeRecipeStep(int recipeIndex, int stepIndex);
    void updateRecipeStep(int recipeIndex, int stepIndex, const Recipe::Step& step);
    void moveRecipeStep(int recipeIndex, int fromIndex, int toIndex);

    void addRecipeIngredient(int recipeIndex, const Recipe::Ingredient& ingredient);
    void removeRecipeIngredient(int recipeIndex, int ingredientIndex);
    void updateRecipeIngredient(int recipeIndex, int ingredientIndex, const Recipe::Ingredient& ingredient);
    void moveRecipeIngredient(int recipeIndex, int fromIndex, int toIndex);

    void save(const QString& filePath) const;
    bool load(const QString& filePath);

signals:
    void recipeAdded(const Recipe& recipe);
    void updated();

private:
    QVector<Recipe> m_recipes;
};
