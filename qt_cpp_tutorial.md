# Qt C++ Tutorial: Getting Back Up to Speed

This tutorial covers Qt C++ fundamentals with special focus on layouts, size policies, DPI scaling, and common Visual Studio setup issues.

---

## Table of Contents

1. [Project Structure Basics](#project-structure-basics)
2. [Signals & Slots](#signals--slots)
3. [Layouts - The Core Topic](#layouts---the-core-topic)
4. [QSizePolicy Examples](#qsizepolicy-examples)
5. [QMainWindow Structure](#qmainwindow-structure)
6. [Complete Working Example](#complete-working-example)
7. [Visual Studio Setup Issues](#visual-studio-setup-issues)
8. [Qt License Information](#qt-license-information)
9. [DPI Scaling and High-DPI Support](#dpi-scaling-and-high-dpi-support)
10. [Stylesheet DPI Scaling](#stylesheet-dpi-scaling)

---

## Project Structure Basics

A typical Qt project has:

```
MyApp/
├── MyApp.pro        # qmake project file (or CMakeLists.txt)
├── main.cpp
├── mainwindow.h
├── mainwindow.cpp
└── mainwindow.ui    # optional Designer file
```

A minimal `main.cpp`:

```cpp
#include <QApplication>
#include <QMainWindow>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();  // starts the event loop
}
```

---

## Signals & Slots

The modern syntax (Qt 5+) uses function pointers rather than the old `SIGNAL()`/`SLOT()` macros — it's type-safe and catches errors at compile time:

```cpp
// Old way (still works, but avoid for new code)
connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

// Modern way — preferred
connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);

// Lambda — great for short handlers
connect(button, &QPushButton::clicked, this, [this]() {
    label->setText("Clicked!");
});
```

---

## Layouts - The Core Topic

Qt layouts automatically manage the size and position of child widgets. You should **never hard-code pixel positions** for production UIs — layouts handle resizing, DPI scaling, and platform differences for you.

### The Layout Classes

| Class | Behaviour |
|---|---|
| `QHBoxLayout` | Arranges widgets left → right |
| `QVBoxLayout` | Arranges widgets top → bottom |
| `QGridLayout` | Row/column grid |
| `QFormLayout` | Label + field pairs |
| `QStackedLayout` | Only one widget visible at a time |

---

### QHBoxLayout & QVBoxLayout

```cpp
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

QWidget *window = new QWidget;

// Horizontal row of buttons
QHBoxLayout *hbox = new QHBoxLayout;
hbox->addWidget(new QPushButton("Left"));
hbox->addWidget(new QPushButton("Centre"));
hbox->addWidget(new QPushButton("Right"));

// Wrap in a vertical layout with a label on top
QVBoxLayout *vbox = new QVBoxLayout;
vbox->addWidget(new QLabel("Choose an option:"));
vbox->addLayout(hbox);   // nest layouts with addLayout()

window->setLayout(vbox);
window->show();
```

---

### Stretch Factors

Stretch controls how extra space is distributed between widgets:

```cpp
QHBoxLayout *layout = new QHBoxLayout;

// Button gets no extra space; label takes all remaining space
layout->addWidget(new QPushButton("Fixed"), 0);   // stretch = 0
layout->addWidget(new QLabel("Expanding"),  1);   // stretch = 1

// Two widgets sharing extra space 1:2
layout->addWidget(widgetA, 1);
layout->addWidget(widgetB, 2);  // widgetB gets twice the extra space
```

You can also insert blank space with `addStretch()`:

```cpp
hbox->addWidget(new QPushButton("Left-aligned"));
hbox->addStretch();   // pushes everything after it to the right
hbox->addWidget(new QPushButton("Right-aligned"));
```

---

### Spacing & Margins

```cpp
QVBoxLayout *layout = new QVBoxLayout;

layout->setContentsMargins(12, 12, 12, 12); // left, top, right, bottom (px)
layout->setSpacing(8);                       // gap between widgets
```

---

### QGridLayout

Widgets are placed by `(row, column)`. You can span multiple rows/columns:

```cpp
QGridLayout *grid = new QGridLayout;

grid->addWidget(new QLabel("Name:"),     0, 0);  // row 0, col 0
grid->addWidget(new QLineEdit(),         0, 1);  // row 0, col 1

grid->addWidget(new QLabel("Email:"),    1, 0);
grid->addWidget(new QLineEdit(),         1, 1);

// Span a widget across 2 columns: addWidget(widget, row, col, rowSpan, colSpan)
grid->addWidget(new QPushButton("Submit"), 2, 0, 1, 2);

// Control column sizing
grid->setColumnStretch(0, 1);  // label column
grid->setColumnStretch(1, 3);  // input column gets 3x more space
```

---

### QFormLayout

The cleanest way to build data-entry forms:

```cpp
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>

QFormLayout *form = new QFormLayout;

form->addRow("Username:",  new QLineEdit);
form->addRow("Age:",       new QSpinBox);
form->addRow("Email:",     new QLineEdit);

// Alignment of labels
form->setLabelAlignment(Qt::AlignRight);

window->setLayout(form);
```

---

### Nesting Layouts (Real-World Pattern)

Most real UIs combine layouts. Here's a typical dialog layout:

```cpp
// Main vertical layout
QVBoxLayout *main = new QVBoxLayout;

// --- Form section ---
QFormLayout *form = new QFormLayout;
form->addRow("First name:", new QLineEdit);
form->addRow("Last name:",  new QLineEdit);
form->addRow("Email:",      new QLineEdit);
main->addLayout(form);

main->addStretch();   // pushes buttons to the bottom

// --- Button row ---
QHBoxLayout *buttons = new QHBoxLayout;
buttons->addStretch();                          // right-align the buttons
buttons->addWidget(new QPushButton("Cancel"));
buttons->addWidget(new QPushButton("OK"));
main->addLayout(buttons);

window->setLayout(main);
```

---

### Size Policies

Each widget has a `QSizePolicy` that tells the layout how it should grow/shrink:

```cpp
// Make a widget expand horizontally but stay fixed vertically
widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

// Set a preferred minimum size hint
widget->setMinimumWidth(200);
widget->setMinimumHeight(30);
```

Common policy values: `Fixed`, `Minimum`, `Maximum`, `Preferred`, `Expanding`, `MinimumExpanding`, `Ignored`.

---

## QSizePolicy Examples

`QSizePolicy` tells a layout how a widget wants to grow or shrink. It has two axes (horizontal, vertical) set independently, plus a stretch factor.

### The Policy Values Explained

```cpp
QSizePolicy::Fixed        // stays exactly at sizeHint() — never grows or shrinks
QSizePolicy::Minimum      // can grow, but won't shrink below sizeHint()
QSizePolicy::Maximum      // can shrink, but won't grow beyond sizeHint()
QSizePolicy::Preferred    // sizeHint() is ideal, but can grow or shrink (default for most widgets)
QSizePolicy::Expanding    // actively wants extra space — will grab it from Preferred neighbours
QSizePolicy::MinimumExpanding // like Expanding but won't shrink below sizeHint()
QSizePolicy::Ignored      // sizeHint() is ignored entirely — takes whatever space is available
```

The key distinction to internalise: **Preferred** widgets *accept* extra space passively, while **Expanding** widgets *request* it actively. When both are in the same layout, `Expanding` wins the extra room.

---

### Example 1 — Fixed vs Expanding

A label that stays a fixed size next to an input that takes all remaining width:

```cpp
QHBoxLayout *layout = new QHBoxLayout;

QLabel *label = new QLabel("Username:");
label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
// equivalent shorthand — Fixed in both axes:
// label->setFixedWidth(80);

QLineEdit *input = new QLineEdit;
input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

layout->addWidget(label);
layout->addWidget(input);
```

Result: the label stays at its natural width; the input stretches to fill all remaining horizontal space.

---

### Example 2 — Preferred vs Expanding (the subtle one)

```cpp
QHBoxLayout *layout = new QHBoxLayout;

QPushButton *btnA = new QPushButton("Preferred");
btnA->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

QPushButton *btnB = new QPushButton("Expanding");
btnB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

layout->addWidget(btnA);
layout->addWidget(btnB);
```

Result: `btnB` takes the lion's share of the space. `btnA` sits at its `sizeHint()` width while `btnB` absorbs everything left over. This is useful when you want one dominant widget alongside a naturally-sized companion.

---

### Example 3 — Stretch Factor to Split Space

When multiple `Expanding` widgets compete, stretch factors decide the ratio:

```cpp
QHBoxLayout *layout = new QHBoxLayout;

QTextEdit *editor = new QTextEdit;
editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

QTextEdit *preview = new QTextEdit;
preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

// Editor gets 2/3 of the width, preview gets 1/3
layout->addWidget(editor,  2);
layout->addWidget(preview, 1);
```

This is how you'd build a side-by-side editor/preview pane — the ratio holds when the window is resized.

---

### Example 4 — Vertical Policies in a VBoxLayout

```cpp
QVBoxLayout *layout = new QVBoxLayout;

QLabel *title = new QLabel("Report");
title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
// Fixed vertically — won't grow taller than its text

QTextEdit *body = new QTextEdit;
body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
// Expands to fill all remaining vertical space

QLabel *footer = new QLabel("Page 1 of 1");
footer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

layout->addWidget(title);
layout->addWidget(body);
layout->addWidget(footer);
```

Result: the title and footer stay slim; the text editor fills all vertical space between them.

---

### Example 5 — Maximum Policy (capping growth)

```cpp
QVBoxLayout *layout = new QVBoxLayout;

QLineEdit *searchBox = new QLineEdit;
searchBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
searchBox->setMaximumWidth(300);
// Won't grow beyond 300px even in a wide window

QTextEdit *results = new QTextEdit;
results->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

layout->addWidget(searchBox);
layout->addWidget(results);
```

---

### Example 6 — Ignored Policy (spacer-like widget)

```cpp
// A custom divider widget that takes up all available space
// regardless of what sizeHint() returns
QFrame *spacer = new QFrame;
spacer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
spacer->setFrameShape(QFrame::HLine);

layout->addWidget(spacer);
```

`Ignored` is rarely needed day-to-day — `QSpacerItem` or `addStretch()` are usually better for blank space — but it's useful for custom widgets that should be completely layout-neutral.

---

### Example 7 — Setting Both Axes at Once with the Constructor

Instead of calling `setSizePolicy()` with two separate enum values, you can use the `QSizePolicy` constructor directly when you want to also set the stretch or other flags:

```cpp
// Constructor: QSizePolicy(horizontal, vertical)
QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
policy.setHorizontalStretch(2);   // this widget gets 2x horizontal stretch
policy.setRetainSizeWhenHidden(true);  // keeps its space even when hidden

widget->setSizePolicy(policy);
```

`setRetainSizeWhenHidden(true)` is particularly handy for toolbar buttons or status indicators that toggle visibility — without it, hiding a widget causes the layout to reflow and shift everything else around.

---

### Quick Reference Card

| You want... | Policy to use |
|---|---|
| Fixed-size widget (never resizes) | `Fixed` / `Fixed` |
| Label beside an input field | `Fixed` / `Fixed` label, `Expanding` / `Fixed` input |
| Full-area canvas or editor | `Expanding` / `Expanding` |
| Button that doesn't stretch | `Minimum` / `Fixed` |
| Sidebar with max width | `Maximum` / `Expanding` |
| Two panes split at 2:1 ratio | `Expanding` both, stretch factors `2` and `1` |
| Widget that holds space when hidden | `setRetainSizeWhenHidden(true)` |

---

## QMainWindow Structure

`QMainWindow` has dedicated zones you slot widgets into:

```cpp
class MainWindow : public QMainWindow {
public:
    MainWindow() {
        // Central widget holds your main layout
        QWidget *central = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(central);
        layout->addWidget(new QLabel("Hello, Qt!"));
        setCentralWidget(central);

        // Toolbar
        QToolBar *toolbar = addToolBar("Main");
        toolbar->addAction("New");

        // Status bar
        statusBar()->showMessage("Ready");

        // Dock widget (side panel)
        QDockWidget *dock = new QDockWidget("Properties", this);
        dock->setWidget(new QWidget);
        addDockWidget(Qt::RightDockWidgetArea, dock);
    }
};
```

---

## Complete Working Example

This puts everything together — a simple form with proper layout nesting:

```cpp
// mainwindow.h
#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void onSubmit();
private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_emailEdit;
    QLabel    *m_statusLabel;
};
```

```cpp
// mainwindow.cpp
#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Qt Layout Demo");
    resize(400, 200);

    QWidget *central = new QWidget(this);
    QVBoxLayout *main = new QVBoxLayout(central);
    main->setContentsMargins(16, 16, 16, 16);
    main->setSpacing(12);

    // Form
    QFormLayout *form = new QFormLayout;
    m_nameEdit  = new QLineEdit;
    m_emailEdit = new QLineEdit;
    form->addRow("Name:",  m_nameEdit);
    form->addRow("Email:", m_emailEdit);
    main->addLayout(form);

    // Status label
    m_statusLabel = new QLabel;
    m_statusLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(m_statusLabel);

    main->addStretch();

    // Buttons
    QHBoxLayout *btnRow = new QHBoxLayout;
    QPushButton *clearBtn  = new QPushButton("Clear");
    QPushButton *submitBtn = new QPushButton("Submit");
    btnRow->addStretch();
    btnRow->addWidget(clearBtn);
    btnRow->addWidget(submitBtn);
    main->addLayout(btnRow);

    setCentralWidget(central);

    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmit);
    connect(clearBtn,  &QPushButton::clicked, this, [this]() {
        m_nameEdit->clear();
        m_emailEdit->clear();
        m_statusLabel->clear();
    });
}

void MainWindow::onSubmit() {
    QString name  = m_nameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    if (name.isEmpty() || email.isEmpty()) {
        m_statusLabel->setText("⚠ Please fill in all fields.");
    } else {
        m_statusLabel->setText(QString("Submitted: %1 <%2>").arg(name, email));
    }
}
```

---

## Visual Studio Setup Issues

### Problem: Squiggly Lines on Qt Types

Visual Studio's IntelliSense engine parses your code independently from the actual compiler. If it can't find the Qt headers, it doesn't know what `QLineEdit`, `QString` etc. are, so it flags them as unknown — even if MSBuild can compile the project perfectly using the Qt VS Tools extension.

### Fix 1 — Install the Qt Visual Studio Tools Extension (Most Important)

In Visual Studio go to **Extensions → Manage Extensions**, search for **Qt Visual Studio Tools**, install it, then restart. After installing, go to **Extensions → Qt VS Tools → Qt Versions** and point it at your Qt installation, for example:

```
C:\Qt\6.5.0\msvc2019_64
```

This lets the extension tell IntelliSense where the Qt headers live.

---

### Fix 2 — Check Your Include Paths in the Project

Even with the extension, IntelliSense sometimes needs the paths explicitly. Right-click your project → **Properties**, then navigate to:

```
Configuration Properties
  → C/C++
    → General
      → Additional Include Directories
```

Add the Qt include paths you need, for example:

```
C:\Qt\6.5.0\msvc2019_64\include
C:\Qt\6.5.0\msvc2019_64\include\QtWidgets
C:\Qt\6.5.0\msvc2019_64\include\QtCore
C:\Qt\6.5.0\msvc2019_64\include\QtGui
```

The key one that catches people out is `QtWidgets` — `QLineEdit`, `QPushButton`, `QLabel` etc. all live there. If you only add the top-level include folder, IntelliSense may still not resolve them.

---

### Fix 3 — Force IntelliSense to Rescan

Sometimes the paths are correct but IntelliSense just has a stale cache. Try:

- **Edit → IntelliSense → Rescan Solution** (older VS versions)
- Delete the `.vs` hidden folder in your solution directory, then reopen the solution — this clears the IntelliSense database entirely and forces a fresh parse
- Close and reopen the specific file with the squiggles

---

### Fix 4 — Check Your `#include` Statements

Make sure you're including the right headers in your `.cpp` or `.h` files. A common mistake is relying on transitive includes that happen to work in one configuration but not another:

```cpp
// Be explicit — include what you actually use
#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
```

Qt does provide convenience headers that pull in a whole module at once, which can help during development:

```cpp
#include <QtWidgets>   // pulls in all of QtWidgets — fine for prototyping
#include <QtCore>      // pulls in all of QtCore
```

---

### Fix 5 — CMake Projects: Check `CMakeSettings.json`

If you're using a CMake-based Qt project rather than a `.vcxproj`, IntelliSense reads its include paths from the CMake configuration. Make sure your `CMakeLists.txt` is correct:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/msvc2019_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(MyApp main.cpp mainwindow.cpp)

target_link_libraries(MyApp PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
```

The `target_link_libraries` call is what propagates the Qt include directories to IntelliSense via CMake's tooling. If that line is missing or wrong, IntelliSense won't find the headers even if the build works.

After changing `CMakeLists.txt`, go to **Project → Configure Cache** (or delete `CMakeCache.txt`) to force a reconfigure.

---

### Distinguishing Real Errors from IntelliSense Noise

A useful sanity check: look at the **Error List** panel and use the dropdown to switch between **Build + IntelliSense** and **Build Only**. If the errors disappear when you select Build Only, they're pure IntelliSense complaints and your code is actually fine. Actually building the project (**Ctrl+Shift+B**) is the definitive test.

The squiggles are genuinely annoying but in the worst case you can work with them while the build succeeds — though getting Fix 1 and Fix 2 right together usually eliminates them entirely.

---

## Qt License Information

### Qt Maintenance Tool

The easiest method. Open the **Qt Maintenance Tool** which is installed alongside Qt, usually found at:

```
C:\Qt\MaintenanceTool.exe          # Windows
~/Qt/MaintenanceTool               # Linux/macOS
```

When it opens, it will show your registered account email and licence type at the top of the screen. This is the quickest way to confirm what you're licensed for.

---

### Qt Account Online

Go to **https://account.qt.io** and log in with the account you used when installing Qt. Under the **Licences** section you'll see:

- Licence type (Open Source, Commercial, etc.)
- Expiry date if applicable
- Which products are covered

---

### From Within Qt Creator

Open Qt Creator and go to **Help → About Qt Creator**. This shows the Qt Creator version and the Qt version it was built against. For more licence detail go to **Help → About Plugins** which lists all components and their origins.

---

### Check the Installation Directory Directly

Qt ships licence text files with every installation. Look in:

```
C:\Qt\6.x.x\msvc2019_64\           # Windows
~/Qt/6.x.x/gcc_64/                 # Linux
~/Qt/6.x.x/macos/                  # macOS
```

You'll find files such as:

```
LICENSE.GPL3
LICENSE.LGPL3
LICENSE.FDL
LICENSES/
```

These tell you which licence terms apply to that particular installation.

---

### What Licence Type You Likely Have

If you downloaded Qt from the official installer at qt.io, you almost certainly have one of:

- **Open Source (LGPL/GPL)** — free, requires you to make your application source available or dynamically link to Qt libraries
- **Commercial** — paid, allows closed-source applications and static linking without source disclosure obligations

The open source installer requires creating a Qt account during installation, so your account at **account.qt.io** is the definitive record either way.

---

### Checking for Device Creation License

The clearest way to check specifically for a **Device Creation licence** is through your Qt Account.

Go to **https://account.qt.io**, log in, and look under the **Licences** tab. Device Creation is a specific commercial add-on licence that will be explicitly listed there if you have it — it won't be present at all if you're on a standard Open Source or standard Commercial licence.

In the licences tab you're looking for something explicitly labelled along the lines of:

```
Qt for Device Creation
Qt Device Creation
```

If you only see entries like **Qt Commercial** or nothing beyond the open source terms, you don't have it.

Device Creation is a separate paid tier on top of the standard commercial licence. It unlocks:

- Qt for embedded/bare metal targets
- The Boot to Qt software stack
- Additional embedded platform support (INTEGRITY, QNX, VxWorks etc.)
- The Qt Automotive Suite components (if licensed separately)

Without it you're limited to the desktop and mobile targets covered by the standard commercial or LGPL licence.

---

## DPI Scaling and High-DPI Support

This is one of the most common Qt deployment headaches. It comes down to **DPI scaling** — your development screen and the target screen have different pixel densities, and Qt needs to know how to compensate.

### Why It Happens

Qt historically worked in **physical pixels**. A button sized at 100x30 pixels looks fine on a 96 DPI screen (standard 1080p monitor) but will appear tiny on a 192 DPI screen (4K/HiDPI), or conversely oversized on a low-DPI screen. Text rendered by the OS at the correct size for that screen's DPI then doesn't fit the pixel-sized button.

---

### The Modern Fix — Enable High DPI Scaling

In **Qt 6** high DPI scaling is enabled automatically by default, so if you're on Qt 6 this is largely handled for you. In **Qt 5.6+** you need to opt in explicitly, before the `QApplication` is constructed:

```cpp
#include <QApplication>

int main(int argc, char *argv[]) {
    // Qt 5 — must be set before QApplication
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    // ...
}
```

In Qt 6 these attributes are deprecated because scaling is on by default — you don't need them.

---

### DPI Scale Factor Rounding

By default Qt rounds the scale factor to the nearest whole number (1x, 2x, 3x). On a screen that's 1.5x DPI this can still cause layout issues. You can allow fractional scaling:

```cpp
// Qt 5
QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough  // use exact scale factor
);

// Qt 6 — just set the rounding policy, scaling is already on
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
);
```

The rounding policy options are:

| Policy | Behaviour |
|---|---|
| `Round` | Rounds to nearest integer (default in Qt 5) |
| `Ceil` | Always rounds up |
| `Floor` | Always rounds down |
| `RoundPreferFloor` | Rounds to nearest, preferring floor at 0.5 |
| `PassThrough` | Uses exact fractional scale factor |

`PassThrough` gives the most accurate results but can cause slightly blurry rendering on some platforms since pixels no longer align perfectly.

---

### Use Layouts, Not Fixed Sizes

This is the most important thing regardless of DPI. If you hard-code pixel positions or sizes, no amount of DPI handling will save you:

```cpp
// BAD — will break on different screens
button->setGeometry(10, 10, 100, 30);
button->setFixedSize(100, 30);

// GOOD — let the layout and size hints do the work
QHBoxLayout *layout = new QHBoxLayout;
layout->addWidget(button);
```

When you use layouts properly, Qt calculates widget sizes based on font metrics and platform style at runtime, so they naturally adapt to the screen's DPI.

---

### Use `em`-relative Sizing When You Must Set Sizes

If you genuinely need to set a size (minimum widths, icon sizes etc.), scale relative to the font size rather than hardcoding pixels. The font is already DPI-aware:

```cpp
// Get the current font height in pixels — already DPI scaled
int em = QFontMetrics(QApplication::font()).height();

button->setMinimumWidth(em * 8);   // 8 "em" units wide
button->setIconSize(QSize(em, em));
```

This scales naturally across any DPI because `em` comes from the actual rendered font size.

---

### Logical vs Physical Pixels

Qt works in **logical pixels** once scaling is enabled. You specify sizes in logical pixels and Qt maps them to physical pixels internally:

```cpp
// On a 2x DPI screen, this logical 100px becomes 200 physical pixels
button->setMinimumWidth(100);  // 100 logical pixels
```

You rarely need to think about physical pixels directly, but if you do (for example when working with `QPainter` and custom rendering):

```cpp
// Get the device pixel ratio for the current screen
qreal dpr = QGuiApplication::primaryScreen()->devicePixelRatio();
int physicalWidth = logicalWidth * dpr;
```

---

### Environment Variable (Quick Test)

You can also force a scale factor via environment variable to test how your UI looks at different DPIs without needing a different monitor:

```
QT_SCALE_FACTOR=1.5   myapp.exe    # scale everything by 1.5x
QT_SCREEN_SCALE_FACTORS=1.5        # per-screen version
QT_ENABLE_HIGHDPI_SCALING=1        # Qt 5 equivalent of AA_EnableHighDpiScaling
```

This is very handy during development — you can simulate a HiDPI screen on your existing monitor to catch layout issues before deploying.

---

### Summary of Best Practices

The root cause of text escaping buttons is almost always one of two things — either DPI scaling isn't enabled so the font is bigger than the button expects, or sizes are hardcoded in pixels so they don't adapt. Fixing both together eliminates the problem on virtually all screens:

1. Enable high DPI scaling (or use Qt 6 where it's automatic)
2. Use layouts instead of fixed geometry
3. Size things relative to font metrics when you must set sizes explicitly
4. Avoid hardcoded pixel values in stylesheets

---

## Stylesheet DPI Scaling

The problem is that Qt Style Sheets (QSS) don't have built-in DPI-aware units like CSS on the web does. When you write this:

```css
QPushButton {
    padding: 4px 8px;
    min-width: 80px;
}
```

Those `4px`, `8px`, and `80px` values are **physical pixels** — they don't automatically scale with the screen's DPI. On a 2x DPI screen (like a 4K monitor or retina display), that padding becomes way too small relative to the font size, and the button looks cramped.

---

### The Runtime Solution

Since QSS doesn't have `em` units, you build the stylesheet as a string at runtime and inject scaled pixel values based on the current font size:

```cpp
// Step 1: Get the font height in pixels (already DPI-scaled by Qt)
QFontMetrics fm(QApplication::font());
int em = fm.height();

// Step 2: Calculate scaled sizes as multiples of font height
int paddingVertical   = em / 4;    // e.g. 4px on 96 DPI, 8px on 192 DPI
int paddingHorizontal = em / 2;    // e.g. 8px on 96 DPI, 16px on 192 DPI  
int minWidth          = em * 5;    // e.g. 80px on 96 DPI, 160px on 192 DPI

// Step 3: Build the stylesheet string with those calculated values
QString stylesheet = QString(
    "QPushButton {"
    "    padding: %1px %2px;"
    "    min-width: %3px;"
    "}"
).arg(paddingVertical)
 .arg(paddingHorizontal)
 .arg(minWidth);

// Step 4: Apply it to the application
qApp->setStyleSheet(stylesheet);
```

---

### Breaking Down `.arg()`

`QString::arg()` is Qt's type-safe replacement mechanism. It works like this:

```cpp
QString template = "Hello %1, you have %2 messages";
QString result = template.arg("Alice").arg(5);
// result: "Hello Alice, you have 5 messages"
```

Each call to `.arg()` replaces the next `%1`, `%2`, `%3` etc. with the provided value. You can chain them:

```cpp
QString("padding: %1px %2px;").arg(4).arg(8)
// Result: "padding: 4px 8px;"
```

So in the stylesheet example:
- `%1` gets replaced with `paddingVertical` (e.g. 4)
- `%2` gets replaced with `paddingHorizontal` (e.g. 8)  
- `%3` gets replaced with `minWidth` (e.g. 80)

---

### Why Use Font Height as the Base Unit?

The font is already correctly scaled by Qt based on the screen DPI and user preferences. By measuring everything relative to it, your UI automatically adapts:

```cpp
QFontMetrics fm(QApplication::font());
int em = fm.height();
```

On a standard 96 DPI screen, `em` might be **16 pixels**.  
On a 192 DPI (2x) screen, `em` might be **32 pixels**.

So when you write `em / 4` for padding:
- 96 DPI screen: 16 / 4 = **4px** padding
- 192 DPI screen: 32 / 4 = **8px** padding

The **visual** size stays consistent — it always looks like about 1/4 of a line height — even though the pixel count doubles.

---

### A More Complete Real-World Example

Here's how you'd apply this to multiple widgets with different sizing needs:

```cpp
#include <QApplication>
#include <QFontMetrics>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Calculate base unit from font
    QFontMetrics fm(QApplication::font());
    int em = fm.height();
    
    // Define all sizes as multiples of em
    int smallPadding  = em / 4;   // tight padding
    int mediumPadding = em / 2;   // comfortable padding
    int largePadding  = em;       // spacious padding
    int buttonHeight  = em * 2;   // buttons are 2 lines tall
    int inputHeight   = em * 1.5; // inputs slightly taller than text
    int borderRadius  = em / 4;   // subtle rounded corners
    int iconSize      = em;       // icons same size as text line
    
    // Build the stylesheet
    QString style = QString(
        "QPushButton {"
        "    padding: %1px %2px;"
        "    min-height: %3px;"
        "    border: 1px solid #999;"
        "    border-radius: %4px;"
        "    background-color: #f0f0f0;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e0e0e0;"
        "}"
        "QLineEdit {"
        "    padding: %5px %2px;"
        "    min-height: %6px;"
        "    border: 1px solid #ccc;"
        "    border-radius: %4px;"
        "}"
    ).arg(smallPadding)      // %1 - button vertical padding
     .arg(mediumPadding)     // %2 - horizontal padding (shared)
     .arg(buttonHeight)      // %3 - button min height
     .arg(borderRadius)      // %4 - border radius (shared)
     .arg(smallPadding / 2)  // %5 - input vertical padding (tighter)
     .arg(inputHeight);      // %6 - input min height
    
    app.setStyleSheet(style);
    
    // Now create your UI
    QMainWindow window;
    // ... your widgets ...
    window.show();
    
    return app.exec();
}
```

---

### When to Calculate the Stylesheet

**At startup** (in `main()` before showing any windows) is usually best, since the DPI typically doesn't change during the application's lifetime. However, if your app supports moving between monitors with different DPIs, or if the user changes system DPI settings while running, you may need to recalculate:

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        applyScaledStylesheet();
    }
    
protected:
    // Called when the window moves to a different screen
    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::ScreenChangeEvent) {
            applyScaledStylesheet();
        }
        QMainWindow::changeEvent(event);
    }
    
private:
    void applyScaledStylesheet() {
        QFontMetrics fm(QApplication::font());
        int em = fm.height();
        
        QString style = QString(
            "QPushButton { padding: %1px %2px; }"
        ).arg(em / 4).arg(em / 2);
        
        setStyleSheet(style);
    }
};
```

---

### Alternative: Per-Widget Inline Styles

If you only need to style one or two widgets, you can skip the global stylesheet and set it directly:

```cpp
QFontMetrics fm(button->font());
int em = fm.height();

QString style = QString(
    "padding: %1px %2px; min-width: %3px;"
).arg(em / 4).arg(em / 2).arg(em * 5);

button->setStyleSheet(style);
```

This gives you more granular control and avoids affecting the whole application.

---

### Common Pitfall: Using Literal Strings

Don't do this — the values stay hardcoded:

```cpp
// WRONG — still using hardcoded pixel values!
QString style = QString(
    "QPushButton { padding: 4px 8px; }"  
);
```

The `QString()` wrapper doesn't magically make it DPI-aware — you still need to inject calculated values with `.arg()`:

```cpp
// CORRECT — values calculated at runtime
QString style = QString(
    "QPushButton { padding: %1px %2px; }"
).arg(em / 4).arg(em / 2);
```

---

### Summary

Qt stylesheets are powerful but pixel values are literal, not DPI-aware. The workaround is measuring the font height and using it as your base unit, injecting calculated pixel values into the stylesheet string at runtime with `.arg()`. This makes your styled UI scale naturally with screen DPI just like your layouts do.

---

## Key Things That May Have Changed Since You Last Used Qt

If you used Qt 4 or early Qt 5, the main things to be aware of are that **Qt 6** (released 2020) made some breaking changes: `QString` and container APIs were tightened up, `QRegExp` was removed in favour of `QRegularExpression`, and CMake is now the recommended build system over qmake. The modern signal/slot syntax with function pointers is also much more widely adopted now. Qt Creator itself has also improved significantly, with better CMake integration and a more capable UI designer.
