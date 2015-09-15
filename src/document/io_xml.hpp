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
#include <QDomDocument>

#include "io.hpp"
#include "builder.hpp"

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

class LoaderXml
{
    Q_DISABLE_COPY(LoaderXml)
public:
    class XmlError : public std::runtime_error
    {
    public:
        XmlError(const QString& message)
            : runtime_error(message.toStdString()) {}
    };

    LoaderXml(QIODevice* device)
    {
        QString error_msg;
        int error_line = 0;
        int error_column = 0;
        bool ok = xml.setContent(device, &error_msg, &error_line, &error_column);
        file_name = misc::fileName(device, QObject::tr("stream"));
        if ( !ok )
            error(error_msg, error_line, error_column);
    }

    ~LoaderXml()
    {
        delete builder.currentDocument();
    }

    Document* document()
    {
        if ( !document_ )
            root();
        return document_;
    }

private:
    void error(const QString& message, int line, int column)
    {
        throw XmlError(QObject::tr("XML error %1:%2:%3: %4")
            .arg(file_name).arg(line).arg(column).arg(message));
    }

    void error(const QString& message)
    {
        throw XmlError(QObject::tr("XML error %1: %4")
            .arg(file_name).arg(message));
    }

    QDomElement requireElement(const QDomNode& node, const QString& name)
    {
        auto element = node.firstChildElement(name);
        if ( element.isNull() )
            error(QObject::tr("Expected <%1>").arg(name));
        return element;
    }

    void root();

    void animations(const QDomElement& node)
    {
        if ( node.isNull() ) return;
        /// \todo Animations
    }

    void formats(const QDomElement& node)
    {
        if ( node.isNull() ) return;
        /// \todo read document_->formatSettings()
    }

    void metadata(const QDomElement& node);
    void layers(const QDomElement& node);
    void layer(const QDomElement& node);
    void image(const QDomElement& node);
    void bitmap(const QDomElement& node);
    void id(const QDomElement& node);

    QDomDocument xml;
    Builder builder;
    Document* document_ = nullptr;
    QString file_name;
};


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
    bool canOpen() const override { return true; }
    Document* open(QIODevice* device) override;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_IO_XML_HPP
