#include "calendar.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    calendar w;
    w.show();
    return QApplication::exec();
}
