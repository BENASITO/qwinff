#include "rangeselector.h"
#include <QPainter>
#include <QMouseEvent>

// rounded rectangle radius
#define ROUNDRECT_RADIUS 3.0

// container color
#define COLOR_CONT_BG_GRAD_1 QColor(210, 210, 210)  /* gradient 1 */
#define COLOR_CONT_BG_GRAD_2 QColor(235, 235, 235)  /* gradient 2 */
#define COLOR_CONT_OUT_BORDER QColor(130, 130, 130) /* outer border */
#define COLOR_CONT_IN_BORDER QColor(250, 250, 250)  /* inner border */

// range indicator color 191, 238, 118
#define COLOR_RANGE_BG_GRAD_1 QColor(216, 244, 166) /* gradient 1 */
#define COLOR_RANGE_BG_GRAD_2 QColor(168, 232, 84)  /* gradient 2 */
#define COLOR_RANGE_OUT_BORDER QColor(0, 138, 0)    /* outer border */
#define COLOR_RANGE_IN_BORDER QColor(218, 250, 167) /* inner border */

RangeSelector::RangeSelector(QWidget *parent) :
    QWidget(parent), m_max(255), m_min(0), m_val_begin(0), m_val_end(128),
    m_mouseDown(false)
{
    setMinimumSize(20, 20);
    setMaximumHeight(20);
    emit beginValueChanged(m_val_begin);
    emit endValueChanged(m_val_end);
}

RangeSelector::~RangeSelector()
{
}

int RangeSelector::beginValue() const
{
    return m_val_begin;
}

int RangeSelector::endValue() const
{
    return m_val_end;
}

int RangeSelector::minValue() const
{
    return m_min;
}

int RangeSelector::maxValue() const
{
    return m_max;
}

void RangeSelector::setBeginValue(int value)
{
    if (m_val_begin != value
            && m_min <= value && value <= m_max
            && value < m_val_end) {
        m_val_begin = value;
        repaint();
        emit valueChanged();
        emit beginValueChanged(m_val_begin);
    }
}

void RangeSelector::setEndValue(int value)
{
    if (m_val_end != value
            && m_min <= value && value <= m_max
            && value > m_val_begin) {
        m_val_end = value;
        repaint();
        emit valueChanged();
        emit endValueChanged(m_val_end);
    }
}

void RangeSelector::setMaxValue(int maxValue)
{
    if (maxValue > m_min) {
        m_max = maxValue;
        m_val_begin = m_min;
        m_val_end = m_max;
        repaint();
    }
}

void RangeSelector::mouseDown(QPoint pos)
{
    const int newval = pos_to_val(pos.x());
    if (newval < m_val_begin) {
        setBeginValue(newval);
        m_dragEdge = EDGE_BEGIN;
    } else if (newval > m_val_end) {
        setEndValue(newval);
        m_dragEdge = EDGE_END;
    } else {
        // mouse position is within the range
        if (newval - m_val_begin < m_val_end - newval) {
            // mouse position is closer to begin
            setBeginValue(newval);
            m_dragEdge = EDGE_BEGIN;
        } else {
            // mouse position is closer to end
            setEndValue(newval);
            m_dragEdge = EDGE_END;
        }
    }
}

void RangeSelector::mouseDrag(QPoint newpos)
{
    const int newval = pos_to_val(newpos.x());
    switch (m_dragEdge) {
    case EDGE_BEGIN:
        setBeginValue(newval);
        break;
    case EDGE_END:
        setEndValue(newval);
        break;
    default:
        Q_ASSERT(!"undefined value: m_dragEdge");
    }
}

void RangeSelector::mouseClick(QPoint)
{

}

int RangeSelector::pos_begin()
{
    return width() * m_val_begin / (m_max - m_min);
}

int RangeSelector::pos_end()
{
    return width() * m_val_end / (m_max - m_min);
}

int RangeSelector::pos_to_val(int pos)
{
    if (width() > 0)
        return pos * (m_max - m_min) / width();
    else
        return 0;
}

void RangeSelector::drawContainer(QPainter &painter, QPen &pen)
{
    // background
    QRect background_region(0, 0, width(), height());
    QLinearGradient background_gradient(0, 0, 0, background_region.bottom());
    background_gradient.setColorAt(0.0, COLOR_CONT_BG_GRAD_1);
    background_gradient.setColorAt(1.0, COLOR_CONT_BG_GRAD_2);
    painter.setBrush(background_gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(background_region, ROUNDRECT_RADIUS, ROUNDRECT_RADIUS);

    // outer border
    QRect outer_border_region(0, 0, width()-1, height()-1);
    pen.setColor(COLOR_CONT_OUT_BORDER);
    painter.setPen(pen);
    painter.drawRoundedRect(outer_border_region, ROUNDRECT_RADIUS, ROUNDRECT_RADIUS);

    // inner border
    QRect inner_border_region(1, 1, width()-3, height()-3);
    pen.setColor(COLOR_CONT_IN_BORDER);
    painter.setPen(pen);
    painter.drawRoundedRect(inner_border_region, ROUNDRECT_RADIUS, ROUNDRECT_RADIUS);

}

void RangeSelector::drawRange(QPainter &painter, QPen &pen)
{
    const int begin = pos_begin(), end = pos_end();

    // background
    QRect background_region(begin, 0, end - begin - 1, height() - 1);
    QLinearGradient background_gradient(0, 0, 0, background_region.bottom());
    background_gradient.setColorAt(0.0, COLOR_RANGE_BG_GRAD_1);
    background_gradient.setColorAt(1.0, COLOR_RANGE_BG_GRAD_2);
    painter.setBrush(background_gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(background_region, ROUNDRECT_RADIUS, ROUNDRECT_RADIUS);

    // outer border
    QRect outer_border_region(begin, 0, end-begin-1, height()-1);
    pen.setColor(COLOR_RANGE_OUT_BORDER);
    painter.setPen(pen);
    painter.drawRoundedRect(outer_border_region, ROUNDRECT_RADIUS, ROUNDRECT_RADIUS);

    // inner border
    QRect inner_border_region(begin+1, 1, end-begin-3, height()-3);
    pen.setColor(COLOR_RANGE_IN_BORDER);
    painter.setPen(pen);
    painter.drawRoundedRect(inner_border_region, ROUNDRECT_RADIUS, ROUNDRECT_RADIUS);
}

void RangeSelector::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    drawContainer(painter, pen);
    drawRange(painter, pen);
}

void RangeSelector::mousePressEvent(QMouseEvent *e)
{
    m_mousePos = e->pos();
    m_mouseDown = true;
    mouseDown(m_mousePos);
}

void RangeSelector::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_mouseDown && m_mousePos == e->pos()) {
        mouseClick(m_mousePos); // click event
    }
}

void RangeSelector::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mouseDown) {
        mouseDrag(e->pos());
    }
}
