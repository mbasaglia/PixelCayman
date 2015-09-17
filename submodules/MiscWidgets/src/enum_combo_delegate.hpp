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
#ifndef PIXEL_CAYMAN_ENUM_COMBO_DELEGATE_HPP
#define PIXEL_CAYMAN_ENUM_COMBO_DELEGATE_HPP

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QVector>
#include <QPair>
#include "misclib/util.hpp"

class EnumComboDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EnumComboDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {}

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        QComboBox* box = new QComboBox(parent);

        for ( const auto& pair : names_ )
            box->addItem(pair.second, pair.first);

        if ( index.isValid() )
        {
            connect(box, util::overload<int>(&QComboBox::currentIndexChanged),
            [index, box, this]{
                setModelData(box, const_cast<QAbstractItemModel*>(index.model()), index);
            });
        }

        return box;

    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        if ( QComboBox* box = qobject_cast<QComboBox*>(editor) )
            box->setCurrentIndex(box->findData(index.data(Qt::EditRole)));
    }

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        if ( QComboBox* box = qobject_cast<QComboBox*>(editor) )
            if ( index.data(Qt::EditRole) != box->currentData() )
                model->setData(index, box->currentData());
    }

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        if ( value.canConvert<int>() )
        {
            int intval = value.toInt();
            for ( const auto& pair : names_ )
                if ( pair.first == intval )
                    return pair.second;
        }

        return unknown_value_name;
    }

    /**
     * \brief Assign all the names
     * \tparam Container A range containing pairs, the first element of which
     *                   must be convertible to \b int
     *                   and the second to \b QString
     * \param container  Container mapping the required values
     */
    template<class Container>
        void setNames(const Container& container)
        {
            names_.clear();
            for ( const auto pair : container )
                names_.push_back({int(pair.first), QString(pair.second)});
        }

    /**
     * \brief Add a name to a value
     */
    void addName(int value, const QString& name)
    {
        names_.push_back({value, name});
    }

    const QVector<QPair<int, QString>>& names() const
    {
        return names_;
    }

    void setUnknownValueName(const QString& name)
    {
        unknown_value_name = name;
    }

    /**
     * \brief Name used for elements that don't have a name associated to them
     */
    QString unknownValueName() const
    {
        return unknown_value_name;
    }

private:
    QVector<QPair<int, QString>> names_;
    QString unknown_value_name;
};

#endif // PIXEL_CAYMAN_ENUM_COMBO_DELEGATE_HPP
