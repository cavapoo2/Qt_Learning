Why It Happens
Qt historically worked in physical pixels. A button sized at 100x30 pixels looks fine on a 96 DPI screen (standard 1080p monitor) but will appear tiny on a 192 DPI screen (4K/HiDPI), or conversely oversized on a low-DPI screen. Text rendered by the OS at the correct size for that screen's DPI then doesn't fit the pixel-sized button.

The Modern Fix — Enable High DPI Scaling
In Qt 6 high DPI scaling is enabled automatically by default, so if you're on Qt 6 this is largely handled for you. In Qt 5.6+ you need to opt in explicitly, before the QApplication is constructed:
cpp#include <QApplication>

int main(int argc, char *argv[]) {
    // Qt 5 — must be set before QApplication
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    // ...
}
In Qt 6 these attributes are deprecated because scaling is on by default — you don't need them.

DPI Scale Factor Rounding
By default Qt rounds the scale factor to the nearest whole number (1x, 2x, 3x). On a screen that's 1.5x DPI this can still cause layout issues. You can allow fractional scaling:
cpp// Qt 5
QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough  // use exact scale factor
);

// Qt 6 — just set the rounding policy, scaling is already on
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
);
The rounding policy options are:
PolicyBehaviourRoundRounds to nearest integer (default in Qt 5)CeilAlways rounds upFloorAlways rounds downRoundPreferFloorRounds to nearest, preferring floor at 0.5PassThroughUses exact fractional scale factor
PassThrough gives the most accurate results but can cause slightly blurry rendering on some platforms since pixels no longer align perfectly.

Use Layouts, Not Fixed Sizes
This is the most important thing regardless of DPI. If you hard-code pixel positions or sizes, no amount of DPI handling will save you:
cpp// BAD — will break on different screens
button->setGeometry(10, 10, 100, 30);
button->setFixedSize(100, 30);

// GOOD — let the layout and size hints do the work
QHBoxLayout *layout = new QHBoxLayout;
layout->addWidget(button);
When you use layouts properly, Qt calculates widget sizes based on font metrics and platform style at runtime, so they naturally adapt to the screen's DPI.

Use em-relative Sizing When You Must Set Sizes
If you genuinely need to set a size (minimum widths, icon sizes etc.), scale relative to the font size rather than hardcoding pixels. The font is already DPI-aware:
cpp// Get the current font height in pixels — already DPI scaled
int em = QFontMetrics(QApplication::font()).height();

button->setMinimumWidth(em * 8);   // 8 "em" units wide
button->setIconSize(QSize(em, em));
This scales naturally across any DPI because em comes from the actual rendered font size.

Logical vs Physical Pixels
Qt works in logical pixels once scaling is enabled. You specify sizes in logical pixels and Qt maps them to physical pixels internally:
cpp// On a 2x DPI screen, this logical 100px becomes 200 physical pixels
button->setMinimumWidth(100);  // 100 logical pixels
You rarely need to think about physical pixels directly, but if you do (for example when working with QPainter and custom rendering):
cpp// Get the device pixel ratio for the current screen
qreal dpr = QGuiApplication::primaryScreen()->devicePixelRatio();
int physicalWidth = logicalWidth * dpr;

Stylesheets and DPI
If you use Qt Style Sheets, hardcoded pixel values there will also cause problems:
css/* BAD */
QPushButton {
    padding: 4px 8px;
    min-width: 80px;
}
There's no built-in DPI-aware unit in Qt stylesheets (unlike CSS em units in web), so the workaround is to generate the stylesheet at runtime with scaled values:
cppint em = QFontMetrics(QApplication::font()).height();
QString style = QString(
    "QPushButton { padding: %1px %2px; min-width: %3px; }"
).arg(em / 4).arg(em / 2).arg(em * 5);

qApp->setStyleSheet(style);
```

---

## Environment Variable (Quick Test)

You can also force a scale factor via environment variable to test how your UI looks at different DPIs without needing a different monitor:
```
QT_SCALE_FACTOR=1.5   myapp.exe    # scale everything by 1.5x
QT_SCREEN_SCALE_FACTORS=1.5        # per-screen version
QT_ENABLE_HIGHDPI_SCALING=1        # Qt 5 equivalent of AA_EnableHighDpiScaling
This is very handy during development — you can simulate a HiDPI screen on your existing monitor to catch layout issues before deploying.

Summary of Best Practices
The root cause of text escaping buttons is almost always one of two things — either DPI scaling isn't enabled so the font is bigger than the button expects, or sizes are hardcoded in pixels so they don't adapt. Fixing both together eliminates the problem on virtually all screens:

Enable high DPI scaling (or use Qt 6 where it's automatic)
Use layouts instead of fixed geometry
Size things relative to font metrics when you must set sizes explicitly
Avoid hardcoded pixel values in stylesheets
