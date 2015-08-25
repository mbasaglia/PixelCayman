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
#include "color_editor.hpp"

using namespace color_widgets;

ColorEditor::ColorEditor(QWidget* parent)
    : Parent(parent)
{
    setupUi(this);

    connect(color_wheel, &ColorWheel::colorChanged, this, &ColorEditor::color_from_wheel);
    connect(slider_alpha_wheel, &QSlider::valueChanged, this, &ColorEditor::color_from_wheel);

    connect(slider_red, &QSlider::valueChanged, this, &ColorEditor::color_from_rgb);
    connect(slider_green, &QSlider::valueChanged, this, &ColorEditor::color_from_rgb);
    connect(slider_blue, &QSlider::valueChanged, this, &ColorEditor::color_from_rgb);
    connect(slider_alpha_rgb, &QSlider::valueChanged, this, &ColorEditor::color_from_rgb);

    connect(slider_hue, &QSlider::valueChanged, this, &ColorEditor::color_from_hsv);
    connect(slider_sat, &QSlider::valueChanged, this, &ColorEditor::color_from_hsv);
    connect(slider_value, &QSlider::valueChanged, this, &ColorEditor::color_from_hsv);
    connect(slider_alpha_hsv, &QSlider::valueChanged, this, &ColorEditor::color_from_hsv);

    setColor(Qt::black);
}

QColor ColorEditor::color() const
{
    return color_;
}

void ColorEditor::setColor(const QColor& color)
{
    if ( color != color_ )
    {
        color_ = color;
        update_color();
        emit colorChanged(color_);
    }
}

void ColorEditor::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        retranslateUi(this);
    }

    Parent::changeEvent(event);
}

void ColorEditor::update_color()
{
    bool blocked = signalsBlocked();
    blockSignals(true);
    foreach(QWidget* w, findChildren<QWidget*>())
        w->blockSignals(true);

    // wheel
    color_wheel->setColor(color_);

    slider_alpha_wheel->setValue(color_.alpha());
    slider_alpha_wheel->setFirstColor(QColor(color_.red(),color_.green(),color_.blue(),0));
    slider_alpha_wheel->setLastColor(QColor(color_.red(),color_.green(),color_.blue(),255));

    // rgb
    slider_red->setValue(color_.red());
    slider_red->setFirstColor(QColor(0,color_.green(),color_.blue(),255));
    slider_red->setLastColor(QColor(255,color_.green(),color_.blue(),255));

    slider_green->setValue(color_.green());
    slider_green->setFirstColor(QColor(color_.red(),0,color_.blue(),255));
    slider_green->setLastColor(QColor(color_.red(),255,color_.blue(),255));

    slider_blue->setValue(color_.blue());
    slider_blue->setFirstColor(QColor(color_.red(),color_.green(),0,255));
    slider_blue->setLastColor(QColor(color_.red(),color_.green(),255,255));

    slider_alpha_rgb->setValue(color_.alpha());
    slider_alpha_rgb->setFirstColor(QColor(color_.red(),color_.green(),color_.blue(),0));
    slider_alpha_rgb->setLastColor(QColor(color_.red(),color_.green(),color_.blue(),255));

    // hsv
    slider_hue->setValue(color_.hue());
    slider_hue->setColorSaturation(color_.saturationF());
    slider_hue->setColorValue(color_.valueF());

    slider_sat->setValue(color_.saturation());
    slider_sat->setFirstColor(QColor::fromHsv(color_.hue(),0,color_.value(),255));
    slider_sat->setLastColor(QColor::fromHsv(color_.hue(),255,color_.value(),255));

    slider_value->setValue(color_.value());
    slider_value->setFirstColor(QColor::fromHsv(color_.hue(),color_.saturation(),0,255));
    slider_value->setLastColor(QColor::fromHsv(color_.hue(),color_.saturation(),255,255));

    slider_alpha_hsv->setValue(color_.alpha());
    slider_alpha_hsv->setFirstColor(QColor(color_.red(),color_.green(),color_.blue(),0));
    slider_alpha_hsv->setLastColor(QColor(color_.red(),color_.green(),color_.blue(),255));

    blockSignals(blocked);
    foreach(QWidget* w, findChildren<QWidget*>())
        w->blockSignals(false);
}

void ColorEditor::color_from_wheel()
{
    QColor color = color_wheel->color();
    color.setAlpha(slider_alpha_wheel->value());
    setColor(color);
}

void ColorEditor::color_from_hsv()
{
    setColor(QColor::fromHsv(
        slider_hue->value(),
        slider_sat->value(),
        slider_value->value(),
        slider_alpha_hsv->value()
    ));
}

void ColorEditor::color_from_rgb()
{
    setColor(QColor(
        slider_red->value(),
        slider_green->value(),
        slider_blue->value(),
        slider_alpha_rgb->value()
    ));
}
