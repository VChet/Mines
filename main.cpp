#include "MainWindow.hpp"
#include <QApplication>

#include <QQuickView>
#include <QQmlContext>
#include <QtQml>

#include "Field.hpp"
#include "Cell.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qmlRegisterType<Field>("GameComponents", 1, 0, "Field");
    qmlRegisterType<Cell>("GameComponents", 1, 0, "Cell");

    MainWindow w;
    w.show();

    QQuickView view;
    view.rootContext()->setContextProperty("field", w.field());
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    QObject::connect(view.rootContext()->engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    return a.exec();
}
