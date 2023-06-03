#include "Recipes.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  Recipes recipes;
  recipes.show();
  return QApplication::exec();
}
