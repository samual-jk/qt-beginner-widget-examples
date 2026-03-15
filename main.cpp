#include "ui/MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("NotepadPro");
    QApplication::setOrganizationName("ArjunKorde");

    MainWindow window;
    window.show();
    return app.exec();
}
