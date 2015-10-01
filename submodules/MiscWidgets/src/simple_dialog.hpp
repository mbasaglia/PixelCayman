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
#ifndef pre_SIMPLE_DIALOG_HPP
#define pre_SIMPLE_DIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

/**
 * \brief A dialog that wraps a widget
 */
class SimpleDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * \brief Creates a new dialog
     * \param parent  Parent widget
     * \param wrapped Wrapped widget, takes ownership
     */
    explicit SimpleDialog(QWidget* parent = nullptr, QWidget* wrapped = nullptr)
        : QDialog(parent)
    {
        layout_ = new QVBoxLayout(this);
        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout_->addWidget(buttons);

        setWidget(wrapped);
    }

    /**
     * \brief Sets the wrapped widget (takes ownership)
     */
    void setWidget(QWidget* widget)
    {
        if ( widget_ == widget )
            return;
        delete widget_;
        widget_ = widget;
        if ( widget )
            layout_->insertWidget(0, widget_);
    }

    /**
     * \brief Returns the wrapped widget
     */
    QWidget* widget() const
    {
        return widget_;
    }

    /**
     * \brief Returns the wrapped widget
     */
    template<class T*>
        T* widget() const
    {
        return qobject_cast<T*>(widget_);
    }

private:
    QWidget* widget_ = nullptr;
    QVBoxLayout* layout_;
};

#endif // pre_SIMPLE_DIALOG_HPP
