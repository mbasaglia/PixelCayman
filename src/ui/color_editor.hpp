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
#ifndef COLOR_EDITOR_HPP
#define COLOR_EDITOR_HPP

#include <QTabWidget>
#include "ui_color_editor.h"


class ColorEditor : public QTabWidget, Ui::ColorEditor
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    using Parent = QTabWidget;

public:
    explicit ColorEditor(QWidget* parent = nullptr);

    QColor color() const;

public slots:
    void setColor(const QColor& color);

private slots:
    void color_from_wheel();
    void color_from_rgb();
    void color_from_hsv();

signals:
    void colorChanged(const QColor& color);

protected:
    void changeEvent(QEvent* event) override;

private:
    void update_color();

    QColor color_;
};

#endif // COLOR_EDITOR_HPP
