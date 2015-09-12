/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C) 2015 Mattia Basaglia
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

#include "tool_paint_widget.hpp"
#include "util.hpp"
#include <QStandardItemModel>

namespace tool {

Brush::Widget::Widget(Brush* tool)
    : tool_(tool)
{
    setupUi(this);

    connect(spin_width, util::overload<int>(&QSpinBox::valueChanged),
    [this](int v) {
        if ( check_ratio_locked->isChecked() )
            spin_height->setValue(v*ratio);
    });
}

void Brush::Widget::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

void Brush::Widget::updatePreview()
{
    QPixmap pixmap(label_preview->size());
    pixmap.fill(Qt::transparent);

    if ( tool_ )
    {
        QPainter painter(&pixmap);
        painter.setBrush(Qt::red);
        painter.setPen(QPen(Qt::white));
        painter.translate(pixmap.rect().center());

        qreal size = qMax(tool_->brush_mask.width(), tool_->brush_mask.height());
        qreal pixsize = qMax(pixmap.width(), pixmap.height());
        if ( size*2 > pixsize )
            painter.scale(pixsize/size, pixsize/size);
        else
            painter.scale(2 ,2);

        painter.drawPath(tool_->brush_path);
        painter.drawImage(tool_->brush_mask.offset(), tool_->brush_mask);
    }

    label_preview->setPixmap(pixmap);
}

void Brush::Widget::updateBrush()
{
    if ( !tool_ )
        return;

    switch ( combo_shapes->currentIndex() )
    {
        case Rectangle:
            tool_->rectangleBrush(QSize(spin_width->value(), spin_height->value()));
            return;
        case Circle:
            tool_->ballBrush(spin_diameter->value(), 2);
            return;
        case SuperCircle:
            tool_->ballBrush(spin_super_diameter->value(), spin_p_norm->value());
            return;
        case Image:
            /// \todo Image brush
            return;
    }
}

void Brush::Widget::setImageBrushEnabled(bool enabled)
{
    auto model = qobject_cast<QStandardItemModel*>(combo_shapes->model());

    if ( enabled )
    {
        model->item(Image)->setFlags(QStandardItem().flags());
    }
    else
    {
        model->item(Image)->setFlags(Qt::NoItemFlags);
        if ( combo_shapes->currentIndex() == Image )
            combo_shapes->setCurrentIndex(Rectangle);
    }
}

void Brush::Widget::setTool(Brush* tool)
{
    this->tool_ = tool;
    updateBrush();
}

Brush* Brush::Widget::tool() const
{
    return tool_;
}

} // namespace tool
