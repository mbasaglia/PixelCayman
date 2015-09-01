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
#ifndef PIXEl_CAYMAN_DOCUMENT_IO_HPP
#define PIXEl_CAYMAN_DOCUMENT_IO_HPP
#include "visitor.hpp"
#include <QXmlStreamWriter>
#include <QFile>
#include <QBuffer>

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
};

} // namespace visitor

/**
 * \brief Save the document to an IODevice as XML
 * \todo Const-correct version
 */
inline void save_xml(Document& doc, QIODevice* device)
{
    visitor::SaverXml xml(device);
    doc.apply(xml);
}

/**
 * \brief Save the document to its own file as XML
 * \todo Const-correct version
 * \return \b true on success
 */
bool save_xml(Document& doc);


} // namespace document
#endif // PIXEl_CAYMAN_DOCUMENT_IO_HPP
