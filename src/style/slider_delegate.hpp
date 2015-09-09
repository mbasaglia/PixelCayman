/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2015 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PIXEL_CAYMAN_SLIDER_DELEGATE_HPP
#define PIXEL_CAYMAN_SLIDER_DELEGATE_HPP

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QRect>
#include "math.hpp"

/**
 * \brief Displays a slider for a real value
 */
class SliderDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit SliderDelegate(QObject *parent = nullptr)
        : QAbstractItemDelegate(parent) {}

    explicit SliderDelegate(qreal min,
                              qreal max,
                              QObject *parent = nullptr)
        : QAbstractItemDelegate(parent),
          min_(min),
          max_(max)
    {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyle *style = option.widget->style();
        style->drawControl(QStyle::CE_ItemViewItem, &option, painter, nullptr);

        auto progress_option = progressBar(option, index);
        QRect rect = style->subElementRect(QStyle::SE_ProgressBarGroove,
                                            &progress_option, option.widget);
        QRect bgrect(
            QPoint(qMax(rect.left() - 1, option.rect.left()), option.rect.top()),
            QPoint(qMin(rect.right() + 1, option.rect.right()), option.rect.bottom())
        );
        QColor bgcolor = option.palette.color(QPalette::Active, QPalette::Base);
        bgcolor.setAlpha(128);
        painter->fillRect(bgrect, bgcolor);
        style->drawControl(QStyle::CE_ProgressBar, &progress_option, painter, option.widget);
    }

    bool editorEvent(QEvent * event,
                     QAbstractItemModel * model,
                     const QStyleOptionViewItem & option,
                     const QModelIndex & index) override
    {

        if ( event->type() == QEvent::MouseButtonPress ||
             event->type() == QEvent::MouseMove )
        {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

            if ( (mouse_event->buttons()& Qt::LeftButton) &&
                ( index.flags() & Qt::ItemIsEditable) )
            {
                QStyle *style = option.widget->style();
                auto progress_option = progressBar(option, index);
                QRect rect = style->subElementRect(QStyle::SE_ProgressBarGroove,
                                                   &progress_option, option.widget);

                qreal value = math::normalize(mouse_event->x(), rect.left(), rect.right());
                value = math::denormalize(value, min_, max_);
                model->setData(index, qBound(min_, value, max_));
            }

            return true;
        }

        return QAbstractItemDelegate::editorEvent(event, model, option, index);
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        return QSize(24, 16);
    }

    qreal min() const
    {
        return min_;
    }

    void setMin(qreal min)
    {
        min_ = min;
    }

    qreal max() const
    {
        return max_;
    }

    void setMax(qreal max)
    {
        max_ = max;
    }

private:
    QStyleOptionProgressBar progressBar(const QStyleOptionViewItem & option,
                                        const QModelIndex &index) const
    {
        QStyleOptionProgressBar progress_option;
        ((QStyleOption&)progress_option) = option;

        qreal value = index.data().toDouble();

        progress_option.minimum = 0;
        progress_option.maximum = 100;
        progress_option.progress = math::normalize(value, min_, max_) * 100;
        progress_option.text = QString::number(value, 'f', 2);
        progress_option.textAlignment = Qt::AlignCenter;
        progress_option.textVisible = true;
        progress_option.orientation = Qt::Horizontal;
        progress_option.invertedAppearance = false;
        progress_option.bottomToTop = false;

        return progress_option;
    }

    qreal min_ = 0;
    qreal max_ = 1;
};


#endif // PIXEL_CAYMAN_SLIDER_DELEGATE_HPP
