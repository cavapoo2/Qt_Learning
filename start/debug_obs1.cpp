void MyDialog::adjustForScreen()
{
    qDebug() << "Dialog size:"       << this->size();
    qDebug() << "GraphicsView size:" << m_graphicsView->size();
    qDebug() << "Dialog height:"     << this->height();
    qDebug() << "View height:"       << m_graphicsView->height();
    qDebug() << "Frame width:"       << m_graphicsView->frameWidth();
    qDebug() << "Scene bounding rect:" << m_graphicsView->scene()->itemsBoundingRect();
    qDebug() << "Overhead:"          << this->height() - m_graphicsView->height();
}
