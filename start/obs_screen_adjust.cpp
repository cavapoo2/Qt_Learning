void MyDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    adjustForScreen();
}

void MyDialog::adjustForScreen()
{
    // Get the screen this dialog is on
    QScreen *screen = QGuiApplication::screenAt(this->mapToGlobal(QPoint(0, 0)));
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    // Use availableGeometry — excludes taskbars/docks
    QRect availableRect = screen->availableGeometry();
    int screenHeight = availableRect.height();

    if (screenHeight >= 1024)
    {
        // Get the full extent of the scene content
        QRectF sceneRect = m_graphicsView->scene()->itemsBoundingRect();
        int requiredViewHeight = static_cast<int>(sceneRect.height()) + 
                                  m_graphicsView->frameWidth() * 2;

        // How much height is the dialog using outside the view?
        int dialogOverhead = this->height() - m_graphicsView->height();

        // Total dialog height needed to show view without scrollbars
        int requiredDialogHeight = requiredViewHeight + dialogOverhead;

        // Cap to available screen height with some margin
        int maxDialogHeight = availableRect.height() - 40; // 40px breathing room
        int newDialogHeight = qMin(requiredDialogHeight, maxDialogHeight);

        // Resize and reposition, keeping dialog centered
        QPoint center = this->frameGeometry().center();
        this->resize(this->width(), newDialogHeight);
        this->move(center.x() - this->width() / 2,
                   availableRect.top() + (availableRect.height() - newDialogHeight) / 2);

        // Only suppress scrollbars if we had enough room
        if (requiredDialogHeight <= maxDialogHeight)
            m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        else
            m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}
