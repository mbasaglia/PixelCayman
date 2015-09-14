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
#ifndef PIXEL_CAYMAN_DOCUMENT_IO_XML_HPP
#define PIXEL_CAYMAN_DOCUMENT_IO_XML_HPP


#include <QXmlStreamWriter>
#include <QBuffer>
#include <QMimeType>

#include "io.hpp"

namespace document {

namespace visitor {

/**
 * \brief Visitor that recursively writes XML on a IODevice
 */
class SaverXml : public Visitor
{
public:
    explicit SaverXml(QIODevice* output);
    ~SaverXml();

    bool enter(Document& document) override;
    void leave(Document& document) override;
    bool enter(Layer& layer) override;
    void leave(Layer& layer) override;
    void visit(Image& image) override;
    bool enter(Animation& animation) override;
    void leave(Animation& animation) override;
    void visit(Frame& frame) override;

private:
    void writeMetadata(const Metadata& data);
    void writeId(const DocumentElement& element, const QString& attr = "id");
    QXmlStreamWriter writer;

    QMimeType image_format;
};

} // namespace visitor

/**
 * \brief Writes a .mela file
 * \todo Read files
 */
class FormatXmlMela : public AbstractFormat
{
public:
    QString id() const override { return "mela"; }
    QString name() const override { return QObject::tr("Cayman Files"); }
    bool canSave() const override { return true; }
    bool save(Document* input, QIODevice* device) override
    {
        visitor::SaverXml xml(device);
        input->apply(xml);
        return true;
    }
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_IO_XML_HPP
