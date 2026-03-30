qreal pointToSegmentDistance(QPointF p, QPointF a, QPointF b)
{
    QLineF seg(a, b);
    if (seg.length() == 0) return QLineF(p, a).length();

    qreal t = QPointF::dotProduct(p - a, b - a) / (seg.length() * seg.length());
    t = qBound(0.0, t, 1.0);
    QPointF closest = a + t * (b - a);
    return QLineF(p, closest).length();
}
