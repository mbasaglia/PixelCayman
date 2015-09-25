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
#ifndef MISC_WIDGETS_SIZE_WIDGET_HPP
#define MISC_WIDGETS_SIZE_WIDGET_HPP

#include <QWidget>
#include <QIcon>
#include <QToolButton>
#include <QSpinBox>
#include <QGridLayout>

#include "misclib/util.hpp"

/**
 * \brief Widget to edit a size
 */
class SizeWidget : public QWidget
{
    Q_OBJECT

    /**
     * \brief Value as shown on the widget
     */
    Q_PROPERTY(QSize value READ value WRITE setValue NOTIFY valueChanged)

    /**
     * \brief Minimum possible value
     */
    Q_PROPERTY(QSize minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)

    /**
     * \brief Maximum possible size
     */
    Q_PROPERTY(QSize maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)

    /**
     * \brief Whether to keep the ratio between width and height when editing
     * \note This will change \b ratio to match the current value
     */
    Q_PROPERTY(bool keepRatio READ keepRatio WRITE setKeepRatio NOTIFY keepRatioChanged)
    /**
     * \brief Ratio between width and height
     *
     * If \b keepRatio is true, changing the width or height will change the
     * other parameter to keep the ratio consistent
     */
    Q_PROPERTY(qreal ratio READ ratio NOTIFY ratioChanged)

    /**
     * \brief Icon to use for the button when \b keepRatio is true
     */
    Q_PROPERTY(QIcon ratioLocked READ ratioLocked WRITE setRatioLocked)
    /**
     * \brief Icon to use for the button when \b keepRatio is false
     */
    Q_PROPERTY(QIcon ratioFree READ ratioFree WRITE setRatioFree)
    /**
     * \brief Text to use for the ratio button
     */
    Q_PROPERTY(QString ratioText READ ratioText WRITE setRatioText)

    /**
     * \brief Suffix for the input fields
     */
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)

public:
    explicit SizeWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        auto gridLayout = new QGridLayout(this);
        gridLayout->setContentsMargins(0, 0, 0, 0);

        spin_width = new QSpinBox(this);
        spin_width->setMinimum(1);
        spin_width->setMaximum(1024);
        gridLayout->addWidget(spin_width, 0, 0, 1, 1);
        connect(spin_width, util::overload<int>(&QSpinBox::valueChanged), this,
        [this](int val)
        {
            if ( keepRatio() && val != 0 )
            {
                spin_height->blockSignals(true);
                spin_height->setValue(qRound(val/ratio_));
                spin_height->blockSignals(false);
            }

            emit valueChanged(value());
        });

        spin_height = new QSpinBox(this);
        spin_height->setMinimum(1);
        spin_height->setMaximum(1024);
        gridLayout->addWidget(spin_height, 1, 0, 1, 1);
        connect(spin_height, util::overload<int>(&QSpinBox::valueChanged), this,
        [this](int val)
        {
            if ( keepRatio() && val != 0 )
            {
                spin_width->blockSignals(true);
                spin_width->setValue(qRound(val*ratio_));
                spin_width->blockSignals(false);
            }

            emit valueChanged(value());
        });

        icon_free = QIcon::fromTheme("object-unlocked");
        icon_locked = QIcon::fromTheme("object-locked");
        button_ratio = new QToolButton(this);
        button_ratio->setCheckable(true);
        button_ratio->setIcon(icon_free);
        button_ratio->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        gridLayout->addWidget(button_ratio, 0, 1, 2, 1);
        connect(button_ratio, &QToolButton::toggled, this, &SizeWidget::setKeepRatio);
    }

    QSize value() const
    {
        return {spin_width->value(), spin_height->value()};
    }

    QSize minimum() const
    {
        return {spin_width->minimum(), spin_height->minimum()};
    }

    QSize maximum() const
    {
        return {spin_width->maximum(), spin_height->maximum()};
    }

    bool keepRatio() const
    {
        return ratio_ > 0;
    }

    qreal ratio() const
    {
        return ratio_;
    }

    QString ratioText() const
    {
        return button_ratio->text();
    }

    QIcon ratioFree() const
    {
        return icon_free;
    }

    QIcon ratioLocked() const
    {
        return icon_locked;
    }

    QString suffix() const
    {
        return spin_width->suffix();
    }

public slots:
    void setValue(const QSize& val)
    {
        spin_width->blockSignals(true);
        spin_height->blockSignals(true);

        spin_width->setValue(val.width());
        spin_height->setValue(val.height());

        spin_width->blockSignals(false);
        spin_height->blockSignals(false);

        if ( keepRatio() )
            adjustRatio();

        emit valueChanged(value());
    }

    void setMinimum(const QSize& minimum)
    {
        spin_width->setMinimum(minimum.width());
        spin_height->setMinimum(minimum.height());
    }

    void setMaximum(const QSize& maximum)
    {
        spin_width->setMaximum(maximum.width());
        spin_height->setMaximum(maximum.height());
    }

    void setKeepRatio(bool keepRatio)
    {
        bool keeps_ratio = ratio_ > 0;
        if ( keeps_ratio == keepRatio )
            return;

        if ( keepRatio )
        {
            adjustRatio();
        }
        else
        {
            ratio_ = 0;
            setKeepRatioImpl(false);
        }
    }

    void setRatioText(const QString& ratioText)
    {
        button_ratio->setText(ratioText);
        button_ratio->setToolTip(ratioText);
    }

    void setRatioFree(const QIcon& ratioFree)
    {
        icon_free = ratioFree;
    }

    void setRatioLocked(const QIcon& ratioLocked)
    {
        icon_locked = ratioLocked;
    }

    void setSuffix(const QString& suffix)
    {
        spin_width->setSuffix(suffix);
        spin_height->setSuffix(suffix);
    }

signals:
    void valueChanged(const QSize& value);
    void minimumChanged(const QSize& minimum);
    void maximumChanged(const QSize& maximum);
    void keepRatioChanged(bool keepRatio);
    void ratioChanged(qreal ratio);

private:
    void adjustRatio()
    {
        QSize size = value();
        bool kept_ratio = ratio_ > 0;

        if ( size.width() == 0 || size.height() == 0 )
            ratio_ = 0;
        else
            ratio_ = qreal(size.width()) / size.height();

        emit ratioChanged(ratio_);

        bool keeps_ratio = ratio_ > 0;
        if ( keeps_ratio != kept_ratio )
            setKeepRatioImpl(keeps_ratio);
    }

    /**
     * \brief Low-level function that changes whether to keep the ratio
     */
    void setKeepRatioImpl(bool keepRatio)
    {
        button_ratio->setChecked(keepRatio);
        button_ratio->setIcon(keepRatio ? icon_locked : icon_free);
        emit keepRatioChanged(keepRatio);
    }

    QSpinBox*    spin_width;
    QSpinBox*    spin_height;
    qreal        ratio_ = 0;
    QToolButton* button_ratio;
    QIcon        icon_locked;
    QIcon        icon_free;
};

#endif // MISC_WIDGETS_SIZE_WIDGET_HPP
