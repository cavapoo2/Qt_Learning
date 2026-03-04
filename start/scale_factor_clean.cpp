#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QTransform>

int main(int argc, char* argv[])
{
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_ENABLE_HIGHDPI_SCALING",   "0");

    QApplication app(argc, argv);

    // --- Original design resolution ---
    const double BASE_WIDTH  = 1366.0;
    const double BASE_HEIGHT = 768.0;

    // --- Get actual screen size ---
    QScreen* screen = app.primaryScreen();
    QSize screenSize = screen->geometry().size();

    double scaleX = screenSize.width()  / BASE_WIDTH;   // 1920/1366 = 1.4055
    double scaleY = screenSize.height() / BASE_HEIGHT;  // 1200/768  = 1.5625

    // --- Create your main window at original design size ---
    MainWindow* mainWindow = new MainWindow();
    mainWindow->setFixedSize(BASE_WIDTH, BASE_HEIGHT);

    // --- Embed it in a QGraphicsScene ---
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsProxyWidget* proxy = scene->addWidget(mainWindow);

    // --- Apply non-uniform scale transform ---
    QTransform transform;
    transform.scale(scaleX, scaleY);
    proxy->setTransform(transform);

    // --- Display in a frameless fullscreen QGraphicsView ---
    QGraphicsView view(scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setFrameStyle(QFrame::NoFrame);
    view.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    view.resize(screenSize);
    view.showFullScreen();

    // Set scene rect to match scaled output
    scene->setSceneRect(0, 0, BASE_WIDTH, BASE_HEIGHT);

    return app.exec();
}
