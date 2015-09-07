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
#ifndef PIXEL_CAYMAN_DOCUMENT_ELEMENT_HPP
#define PIXEL_CAYMAN_DOCUMENT_ELEMENT_HPP
#include <QHash>
#include <QString>
#include <QObject>

namespace document {

class Visitor;
class Document;

using Metadata = QHash<QString, QString>;

/**
 * \brief Base class for all document components
 */
class DocumentElement : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DocumentElement)

public:
    explicit DocumentElement(const Metadata& metadata = {})
        : metadata_(metadata) {}

    virtual ~DocumentElement() {}

    /**
     * \brief Ensures the visitor visits the current object and all of its children
     */
    virtual void apply(Visitor& visitor) = 0;


    /**
     * \brief Document this element belongs to
     */
    virtual Document* parentDocument() const = 0;

    const Metadata& metadata() const
    {
        return metadata_;
    }

    Metadata& metadata()
    {
        return metadata_;
    }

signals:
    /**
     * \brief Emitted when any kind of drawing data (not the metadata) changes
     *        in this element or any of its children
     *
     * Intended to be used to signal that the element should be re-drawn
     */
    void edited();

private:
    Metadata metadata_;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_ELEMENT_HPP
