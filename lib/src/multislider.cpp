#include "multislider.h"

#include <QPainter>
#include <QStyle>
#include <QRandomGenerator>
#include <QGuiApplication>

namespace XB
{

MultiSlider::MultiSlider(Qt::Orientation orientation, QWidget* parent)
    : QWidget(parent), _orientation(orientation), _minimum(0), _maximum(100)
{
    setMouseTracking(true);
}

std::vector<MultiSlider::Handle*> MultiSlider::sortedHandles()
{
    std::vector<Handle*> handlesRef;
    for (size_t i = 0; i < _handles.size(); ++i)
        handlesRef.push_back(&_handles[i]);
    std::sort(handlesRef.begin(), handlesRef.end(), [](auto const& a, auto const& b) {
        return a->value > b->value;
    });
    return handlesRef;
}

void MultiSlider::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    int interval = qAbs(_maximum - _minimum);

    for (auto const& h : sortedHandles())
    {
        QColor color(h->color);
        if (!this->isEnabled())
        {
            int gray = qGray(color.rgb());
            color    = QColor(gray, gray, gray);
        }

        QRect r(0, 0, 0, 0);

        if (_orientation == Qt::Horizontal)
        {
            r.setHeight(height());
            r.setWidth(h->value * width() / interval);
        }
        else
        {
            int v = h->value * height() / interval;
            r.setY(height() - v);
            r.setHeight(v);
            r.setWidth(width());
        }

        p.fillRect(r, color);
    }

    if (!this->isEnabled())
        p.setPen(Qt::gray);
    else
        p.setPen(Qt::black);
    p.drawRect(rect().adjusted(0, 0, -1, -1));
}

void MultiSlider::mousePressEvent(QMouseEvent* event)
{
    int interval = qAbs(_maximum - _minimum);

    if (event->buttons() & Qt::LeftButton)
    {
        for (auto& h : sortedHandles())
        {
            if (_orientation == Qt::Horizontal)
            {
                int v      = h->value * width() / interval;
                h->pressed = QRect(v - _border, 0, _border * 2, height()).contains(event->pos());
                if (h->pressed)
                {
                    emit sliderPressed();
                    break; // do not "press" several handles
                }
            }
            else
            {
                int v      = height() - h->value * height() / interval;
                h->pressed = QRect(0, v - _border, width(), _border * 2).contains(event->pos());
                if (h->pressed)
                {
                    emit sliderPressed();
                    break; // do not "press" several handles
                }
            }
        }
    }
}

void MultiSlider::mouseMoveEvent(QMouseEvent* event)
{
    int interval = qAbs(_maximum - _minimum);

    if (event->buttons() & Qt::LeftButton)
    {
        for (size_t i = 0; i < _handles.size(); ++i)
        {
            if (_handles[i].pressed)
            {
                if (_orientation == Qt::Horizontal)
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                    setValue(i, event->pos().x() * interval / width());
                }
                else
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                    int v = (height() - event->pos().y()) * interval / height();
                    setValue(i, v);
                }

                update();
                return;
            }
        }
    }

    for (auto const& h : _handles)
    {
        if (_orientation == Qt::Horizontal)
        {
            int v = h.value * width() / interval;
            if (QRect(v - _border, 0, _border * 2, height()).contains(event->pos()))
            {
                QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                return;
            }
        }
        else
        {
            int v = height() - h.value * height() / interval;
            if (QRect(0, v - _border, width(), _border * 2).contains(event->pos()))
            {
                QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                return;
            }
        }
    }

    QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void MultiSlider::mouseReleaseEvent(QMouseEvent* /*event*/)
{
    for (auto& h : sortedHandles())
    {
        if (h->pressed)
        {
            emit sliderReleased();
            h->pressed = false;
        }
    }
}

QSize MultiSlider::minimumSizeHint() const
{
    if (_orientation == Qt::Horizontal)
        return QSize(50, 20);
    else
        return QSize(20, 50);
}

int MultiSlider::value(int index) const
{
    if (index < 0 || index >= _handles.size())
        return minimum();

    return _handles[index].value;
}

int MultiSlider::minimum() const
{
    return _minimum;
}

int MultiSlider::maximum() const
{
    return _maximum;
}

Qt::Orientation MultiSlider::orientation() const
{
    return _orientation;
}

void MultiSlider::addHandle(int value, QColor color)
{
    if (!color.isValid())
        color = QColor::fromRgb(QRandomGenerator::global()->generate());

    _handles.emplace_back(value, color);
}

void MultiSlider::insertHandle(int index, int value, QColor color)
{
    if (!color.isValid())
        color = QColor::fromRgb(QRandomGenerator::global()->generate());

    Handle h(value, color);

    _handles.insert(_handles.begin() + index, h);
}

void MultiSlider::removeHandle(int index)
{
    if (index >= 0 && index < _handles.size())
        _handles.erase(_handles.begin() + index);
}

void MultiSlider::setValue(int index, int value)
{
    if (value > _maximum)
        value = _maximum;

    if (value < _minimum)
        value = _minimum;

    if (index >= 0 && index < _handles.size())
    {
        if (_handles[index].value != value)
        {
            _handles[index].value = value;
            emit valueChanged(index, value);

            update();
        }
    }
}

void MultiSlider::setMaximum(int max)
{
    if (max >= minimum())
        _maximum = max;
    else
    {
        int oldMin = minimum();
        _maximum   = oldMin;
        _minimum   = max;
    }

    update();

    for (size_t i = 0; i < _handles.size(); ++i)
        if (_handles[i].value > maximum())
            setValue(i, maximum());

    emit rangeChanged(minimum(), maximum());
}

void MultiSlider::setRange(int min, int max)
{
    setMinimum(min);
    setMaximum(max);
}

void MultiSlider::setMinimum(int min)
{
    if (min <= maximum())
        _minimum = min;
    else
    {
        int oldMax = maximum();
        _minimum   = oldMax;
        _maximum   = min;
    }

    update();

    for (size_t i = 0; i < _handles.size(); ++i)
        if (_handles[i].value < minimum())
            setValue(i, minimum());

    emit rangeChanged(minimum(), maximum());
}

void MultiSlider::setOrientation(Qt::Orientation orientation)
{
    if (_orientation == orientation)
        return;

    _orientation = orientation;
    update();
}

} // namespace XB
