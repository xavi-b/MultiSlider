#ifndef MULTISLIDER_H
#define MULTISLIDER_H

#include <QWidget>
#include <QMouseEvent>

namespace XB
{

class MultiSlider : public QWidget
{
    Q_OBJECT
private:
    Q_DISABLE_COPY(MultiSlider)

    Qt::Orientation _orientation;

    int _minimum;
    int _maximum;

    int _border = 2;

    struct Handle
    {
        int    value   = 0;
        bool   pressed = false;
        QColor color;

        Handle(int value, QColor color) : value(value), color(color)
        {
        }
    };

    std::vector<Handle> _handles;

    std::vector<Handle*> sortedHandles();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

public:
    MultiSlider(Qt::Orientation orientation = Qt::Vertical, QWidget* parent = nullptr);

    QSize minimumSizeHint() const;

    int             value(int index) const;
    int             minimum() const;
    int             maximum() const;
    Qt::Orientation orientation() const;

    void addHandle(int value, QColor color = QColor());
    void insertHandle(int index, int value, QColor color = QColor());
    void removeHandle(int index);

signals:
    void valueChanged(int index, int value);
    void rangeChanged(int min, int max);
    void sliderPressed();
    void sliderReleased();

public slots:
    void setValue(int index, int value);
    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setOrientation(Qt::Orientation orientation);
};

} // namespace XB

#endif // MULTISLIDER_H
