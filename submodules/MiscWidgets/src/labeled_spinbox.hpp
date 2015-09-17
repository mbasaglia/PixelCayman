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
#ifndef MISCWIDGETS_LABELED_SPINBOX_HPP
#define MISCWIDGETS_LABELED_SPINBOX_HPP

#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>

class LabeledSpinBox : public QWidget
{
    Q_OBJECT
public:
    explicit LabeledSpinBox(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
        label_ = new QLabel(this);
        layout->addWidget(label_);
        spin_box = new QSpinBox(this);
        layout->addWidget(spin_box);
        setLayout(layout);
    }

    const QLabel* label() const
    {
        return label_;
    }

    const QSpinBox* spinBox() const
    {
        return spin_box;
    }

    QLabel* label()
    {
        return label_;
    }

    QSpinBox* spinBox()
    {
        return spin_box;
    }

private:
    QLabel* label_;
    QSpinBox* spin_box;
};


#endif // MISCWIDGETS_LABELED_SPINBOX_HPP
