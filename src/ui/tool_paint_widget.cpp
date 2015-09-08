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

namespace tool {

Paint::Widget::Widget(Paint* tool)
    : tool(tool)
{
    setupUi(this);

    connect(spin_width, util::overload<int>(&QSpinBox::valueChanged),
    [this](int v) {
        if ( check_ratio_locked->isChecked() )
            spin_height->setValue(v*ratio);
    });
}

void Paint::Widget::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

void Paint::Widget::updatePreview()
{
    QPixmap pixmap(label_preview->size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setBrush(Qt::red);
    painter.setPen(QPen(Qt::white));
    painter.translate(pixmap.rect().center());

    qreal size = qMax(tool->brush_mask.width(), tool->brush_mask.height());
    qreal pixsize = qMax(pixmap.width(), pixmap.height());
    if ( size*2 > pixsize )
        painter.scale(pixsize/size, pixsize/size);
    else
        painter.scale(2 ,2);

    painter.drawPath(tool->brush_path);
    painter.drawImage(tool->brush_mask.offset(), tool->brush_mask);
    label_preview->setPixmap(pixmap);
}

void Paint::Widget::updateBrush()
{
    switch ( combo_shapes->currentIndex() )
    {
        case Rectangle:
            tool->rectangleBrush(QSize(spin_width->value(), spin_height->value()));
            return;
        case Circle:
            tool->ballBrush(spin_diameter->value(), 2);
            return;
        case SuperCircle:
            tool->ballBrush(spin_super_diameter->value(), spin_p_norm->value());
            return;
        case Image:
            /// \todo
            return;
    }
}

} // namespace tool
