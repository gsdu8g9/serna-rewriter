#include <QtGui/QApplication>
#include "mainwindow.h"
//#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    a.addLibraryPath("libs");
    //qDebug()<<a.libraryPaths();
    w.setWindowTitle("Serna rewriter");
    w.setWindowIcon(QIcon("libs/moonkin.png"));
    //w.setWindowIconText("");
    w.show();

    return a.exec();
}
