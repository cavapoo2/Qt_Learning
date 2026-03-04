#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <cstdlib>
#include <cmath>

// Must be called BEFORE QApplication is constructed
void setDpiScaleFactor()
{
    // Original design resolution
    const double BASE_WIDTH  = 1366.0;
    const double BASE_HEIGHT = 768.0;

    // We need a temporary QGuiApplication to query the screen
    // Instead, use platform-native screen resolution via QT internals
    // The trick: create a minimal app instance just to read screen size

    int    argc = 0;
    char** argv = nullptr;

    // Temporary QGuiApplication (lightweight, no widgets) just to get screen info
    QGuiApplication tempApp(argc, argv);

    QScreen* screen = tempApp.primaryScreen();
    if (!screen) return;

    QSize screenSize = screen->geometry().size();

    double scaleX = screenSize.width()  / BASE_WIDTH;
    double scaleY = screenSize.height() / BASE_HEIGHT;

    // Use the smaller factor to preserve aspect ratio
    double scaleFactor = std::min(scaleX, scaleY);

    // Round to 4 decimal places to avoid floating point noise
    scaleFactor = std::round(scaleFactor * 10000.0) / 10000.0;

    // Set the environment variable — must be done before real QApplication starts
    qputenv("QT_SCALE_FACTOR", QByteArray::number(scaleFactor, 'f', 4));
}

int main(int argc, char* argv[])
{
    // Disable Qt6 automatic DPI scaling so our manual factor takes full control
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_ENABLE_HIGHDPI_SCALING",   "0");

    // Calculate and set QT_SCALE_FACTOR before QApplication is created
    setDpiScaleFactor();

    // Now create the real application with scaling applied
    QApplication app(argc, argv);

    // --- your normal app setup below ---
    MainWindow w;
    w.show();

    return app.exec();
}
```

**The core problem this solves:**

`QT_SCALE_FACTOR` must be set before `QApplication` is constructed, but you need a screen object to read the resolution — which normally requires `QApplication`. The solution is to spin up a lightweight `QGuiApplication` (no widgets, no event loop) purely to query `primaryScreen()`, then destroy it before constructing the real `QApplication`.

**How it fits together:**
```
main()
  │
  ├─ qputenv: disable auto DPI scaling
  │
  ├─ setDpiScaleFactor()
  │     ├─ create temporary QGuiApplication
  │     ├─ query primaryScreen()->geometry()
  │     ├─ compute min(scaleX, scaleY)
  │     └─ qputenv("QT_SCALE_FACTOR", value)   ← env var now set
  │
  └─ QApplication app(argc, argv)              ← picks up QT_SCALE_FACTOR
