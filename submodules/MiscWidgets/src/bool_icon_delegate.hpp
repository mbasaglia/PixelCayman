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
 *
 */
#ifndef PIXEL_CAYMAN_BOOL_ICON_DELEGATE_HPP
#define PIXEL_CAYMAN_BOOL_ICON_DELEGATE_HPP

#include <QAbstractItemDelegate>
#include <QIcon>
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>

/**
 * \brief Displays an icon for a boolean value
 */
class BoolIconDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit BoolIconDelegate(QObject *parent = nullptr)
        : QAbstractItemDelegate(parent) {}

    explicit BoolIconDelegate(const QIcon& icon_true,
                              const QIcon& icon_false,
                              QObject *parent = nullptr)
        : QAbstractItemDelegate(parent),
          icon_true(icon_true),
          icon_false(icon_false)
    {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyle *style = QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &option, painter, nullptr);

        int size = qMin(option.rect.width(), option.rect.height());
        QRect icon_rect(
            option.rect.center().x() - size / 2,
            option.rect.center().y() - size / 2,
            size,
            size );
        const QIcon& icon = index.data().toBool() ? icon_true : icon_false;
        painter->drawPixmap(icon_rect, icon.pixmap(size));
    }

    bool editorEvent(QEvent * event,
                     QAbstractItemModel * model,
                     const QStyleOptionViewItem & option,
                     const QModelIndex & index) override
    {

        if ( event->type() == QEvent::MouseButtonRelease )
        {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

            if ( mouse_event->button() == Qt::LeftButton &&
                ( index.flags() & Qt::ItemIsEditable) )
            {
                model->setData(index, !index.data(Qt::EditRole).toBool());
            }

            return true;
        }

        return QAbstractItemDelegate::editorEvent(event, model, option, index);
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        int sz = QApplication::style()->pixelMetric(QStyle::PM_ListViewIconSize);
        return QSize(sz, sz);
    }


    QIcon iconTrue() const
    {
        return icon_true;
    }

    void setIconTrue(const QIcon& icon)
    {
        icon_true = icon;
    }

    QIcon iconFalse() const
    {
        return icon_false;
    }

    void setIconFalse(const QIcon& icon)
    {
        icon_false = icon;
    }

private:
    QIcon icon_true;
    QIcon icon_false;
};

#endif // PIXEL_CAYMAN_BOOL_ICON_DELEGATE_HPP
