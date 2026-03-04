#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    const double BASE_WIDTH  = 1366.0;
    const double BASE_HEIGHT = 768.0;

    QScreen* screen = app.primaryScreen();
    QSize screenSize = screen->geometry().size();

    qDebug() << "Screen size (logical):" << screenSize;
    qDebug() << "Device pixel ratio:"    << screen->devicePixelRatio();

    // --- Create your main window at original design size ---
    MainWindow* mainWindow = new MainWindow();
    mainWindow->setFixedSize(BASE_WIDTH, BASE_HEIGHT);

    // --- Embed in scene ---
    QGraphicsScene* scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, BASE_WIDTH, BASE_HEIGHT);
    QGraphicsProxyWidget* proxy = scene->addWidget(mainWindow);
    Q_UNUSED(proxy);

    // --- Create fullscreen view ---
    QGraphicsView* view = new QGraphicsView(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(QFrame::NoFrame);
    view->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    view->setRenderHint(QPainter::Antialiasing);

    // Let Qt fit the scene into the view — handles both axes correctly
    view->resize(screenSize);
    view->showFullScreen();

    // fitInView AFTER show, so the view has its final geometry
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    return app.exec();
}
