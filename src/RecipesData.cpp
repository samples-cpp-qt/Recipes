#include "RecipesData.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

void RecipesData::addNewRecipe(const QString& name)
{
    Recipe recipe;
    recipe.name = name;
    m_recipes.append(recipe);
    emit recipeAdded(m_recipes.back());
    emit updated();
}

void RecipesData::removeRecipe(int recipeIndex)
{
    m_recipes.removeAt(recipeIndex);
    emit updated();
}

void RecipesData::updateRecipeName(int recipeIndex, const QString& name)
{
    m_recipes[recipeIndex].name = name;
    emit updated();
}

void RecipesData::moveRecipe(int fromIndex, int toIndex)
{
    auto recipe = m_recipes.at(fromIndex);
    m_recipes.removeAt(fromIndex);
    m_recipes.insert(toIndex, recipe);
    emit updated();
}

void RecipesData::addRecipeStep(int recipeIndex, const Recipe::Step& step)
{
    auto& recipe = m_recipes[recipeIndex];
    recipe.steps.append(step);
    recipe.totalTime += step.timeMinutes;
    emit updated();
}

void RecipesData::removeRecipeStep(int recipeIndex, int stepIndex)
{
    auto& recipe = m_recipes[recipeIndex];
    recipe.totalTime -= recipe.steps.at(stepIndex).timeMinutes;
    recipe.steps.removeAt(stepIndex);
    emit updated();
}

void RecipesData::updateRecipeStep(int recipeIndex, int stepIndex, const Recipe::Step& step)
{
    auto& recipe = m_recipes[recipeIndex];
    auto& recipeStep = recipe.steps[stepIndex];
    recipe.totalTime -= recipeStep.timeMinutes;
    recipeStep = step;
    recipe.totalTime += step.timeMinutes;
    emit updated();
}

void RecipesData::moveRecipeStep(int recipeIndex, int fromIndex, int toIndex)
{
    auto& steps = m_recipes[recipeIndex].steps;
    auto step = steps.at(fromIndex);
    steps.removeAt(fromIndex);
    steps.insert(toIndex, step);
    emit updated();
}

void RecipesData::addRecipeIngredient(int recipeIndex, const Recipe::Ingredient& ingredient)
{
    auto & recipe = m_recipes[recipeIndex];
    recipe.ingredients.append(ingredient);
    recipe.totalWeight += ingredient.weightGrams;
    emit updated();
}

void RecipesData::removeRecipeIngredient(int recipeIndex, int ingredientIndex)
{
    auto& recipe = m_recipes[recipeIndex];
    recipe.totalWeight -= recipe.ingredients.at(ingredientIndex).weightGrams;
    recipe.ingredients.removeAt(ingredientIndex);
    emit updated();
}

void RecipesData::updateRecipeIngredient(int recipeIndex, int ingredientIndex, const Recipe::Ingredient& ingredient)
{
    auto& recipe = m_recipes[recipeIndex];
    auto& recipeIngredient = recipe.ingredients[ingredientIndex];
    recipe.totalWeight -= recipeIngredient.weightGrams;
    recipeIngredient = ingredient;
    recipe.totalWeight += ingredient.weightGrams;
    emit updated();
}

void RecipesData::moveRecipeIngredient(int recipeIndex, int fromIndex, int toIndex)
{
    auto& ingredients = m_recipes[recipeIndex].ingredients;
    auto ingredient = ingredients.at(fromIndex);
    ingredients.removeAt(fromIndex);
    ingredients.insert(toIndex, ingredient);
    emit updated();
}

void RecipesData::save(const QString& filePath) const
{
    QJsonArray data;
    for (const auto& recipe : recipes())
    {
        QJsonArray stepsArray;
        for (const auto& step : recipe.steps)
        {
            stepsArray.append(QJsonObject{
                    {"action", step.action},
                    {"timeMinutes", step.timeMinutes}
                }
            );
        }
        QJsonArray ingredientsArray;
        for (const auto& ingredient : recipe.ingredients)
        {
            ingredientsArray.append(QJsonObject{
                {"name", ingredient.name},
                {"weightGrams", ingredient.weightGrams},
            });
        }
        data.append(QJsonObject{
            {"name", recipe.name},
            {"steps", stepsArray},
            {"ingredients", ingredientsArray},
        });
    }
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) return;
    file.write(QJsonDocument(data).toJson());
}

bool RecipesData::load(const QString& filePath)
{
    m_recipes.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    auto doc = QJsonDocument::fromJson(data);

    if (!doc.isArray()) return false;
    const auto& docArray = doc.array();

    for (const auto& recipeJson : docArray)
    {
        if (!recipeJson.isObject()) return false;
        const auto& recipeObj = recipeJson.toObject();

        addNewRecipe(recipeObj["name"].toString());
        int recipeIndex = recipes().size() - 1;

        const auto& stepsJson = recipeObj["steps"];
        if (!stepsJson.isArray()) return false;
        const auto& stepsArray = recipeObj["steps"].toArray();
        for (const auto& stepJson : stepsArray)
        {
            if (!stepJson.isObject()) return false;
            const auto& stepObj = stepJson.toObject();
            Recipe::Step step;
            step.action = stepObj["action"].toString();
            step.timeMinutes = stepObj["timeMinutes"].toInt();
            addRecipeStep(recipeIndex, step);
        }

        const auto& ingredientsJson = recipeObj["ingredients"];
        if (!ingredientsJson.isArray()) return false;
        const auto& ingredientsArray = ingredientsJson.toArray();
        for (const auto& ingredientJson : ingredientsArray)
        {
            if (!ingredientJson.isObject()) return false;
            const auto& ingredientObj = ingredientJson.toObject();
            Recipe::Ingredient ingredient;
            ingredient.name = ingredientObj["name"].toString();
            ingredient.weightGrams = ingredientObj["weightGrams"].toInt();
            addRecipeIngredient(recipeIndex, ingredient);
        }
    }
    return true;
}

