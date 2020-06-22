#include "MainWindow.hpp"
#include <QtWidgets/QApplication>

using namespace PhotoGraph;

int main(int argc, char* argv[]) {
  QApplication app{argc, argv};
  MainWindow window{};
  window.showMaximized();

  return QApplication::exec();
}
