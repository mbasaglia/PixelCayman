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
 *
 */

#include <QXmlStreamWriter>
#include <QMimeDatabase>
#include <QImageWriter>
#include <QBuffer>

#include "io/formats.hpp"
#include "plugin.hpp"
#include "external_tools/external_tools.hpp"
#include "plugin/library_plugin.hpp"

static QMimeType mime_type = QMimeDatabase().mimeTypeForName("image/png");
/**
 * \brief Class that traverses a document to output a SVG file
 */
class InkscapeSvgVisitor : public document::Visitor
{
public:
    explicit InkscapeSvgVisitor(QIODevice* output, document::Frame* frame = nullptr)
        : writer(output), frame(frame)
    {
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
    }

    ~InkscapeSvgVisitor()
    {
        writer.writeEndDocument();
    }

    bool enter(document::Document& document) override
    {
        writer.writeStartElement("svg");
        writer.writeNamespace("http://www.w3.org/2000/svg");
        writer.writeNamespace("http://www.w3.org/1999/xlink", "xlink");
        writer.writeNamespace("http://www.inkscape.org/namespaces/inkscape", "inkscape");
        writer.writeAttribute("version", "1.1");
        writeId(document);
        writer.writeAttribute("width", QString::number(document.imageSize().width()));
        writer.writeAttribute("height", QString::number(document.imageSize().height()));
        return true;
    }
    void leave(document::Document& document) override
    {
        /**
         * \todo defs
         */
        /*
        <defs>
            <filter
            inkscape:collect="always"
            style="color-interpolation-filters:sRGB"
            id="filter4141">
            <feBlend
                inkscape:collect="always"
                mode="multiply"
                in2="BackgroundImage" />
            </filter>
        </defs>
        */
        writer.writeEndElement();
    }
    bool enter(document::Layer& layer) override
    {
        writer.writeStartElement("g");
        writeId(layer);
        writer.writeAttribute("inkscape:label", layer.name());
        writer.writeAttribute("inkscape:groupmode", "layer");
        QString css = QString("display:%1;opacity:%2;")
            .arg(layer.visible() ? "inline" : "none")
            .arg(layer.opacity());
         /// \todo filter:url(#filter4141)
        writer.writeAttribute("style", "css");
        return true;
    }
    void leave(document::Layer& layer) override
    {
        writer.writeEndElement();
    }
    void visit(document::Image& image) override
    {
        if ( image.frame() != frame )
            return;
        writer.writeEmptyElement("image");
        writeId(image);
        writer.writeAttribute("style", "image-rendering:optimizeSpeed");

        QByteArray image_data;
        QBuffer buffer(&image_data);
        QImageWriter image_writer(&buffer, mime_type.preferredSuffix().toLatin1());
        image_writer.write(image.image());
        QString image_data_string = QString("data:%1;base64,%2")
            .arg(mime_type.name())
            .arg(QString::fromLatin1(image_data.toBase64()));
        writer.writeAttribute("xlink:href", image_data_string);
    }

private:
    void writeId(const document::DocumentElement& element, const QString& attr = "id")
    {
        if ( !element.objectName().isEmpty() && !attr.isEmpty() )
            writer.writeAttribute(attr, element.objectName());
    }

    QXmlStreamWriter writer;
    document::Frame* frame;
};

/**
 * \brief Inkscape SVG format
 */
class InkscapeSvg : public io::AbstractFormat
{
public:
    QString id() const override { return "inkscape"; }
    QString name() const override { return tr("Inkscape SVG"); }
    bool canSave() const override { return true; }
    QStringList extensions(Action) const override
    {
        return {"svg"};
    }

protected:
    bool onSave(document::Document* input, QIODevice* device) override
    {
        InkscapeSvgVisitor svg(device);
        input->apply(svg);
        return true;
    }
};

/**
 * \brief Inkscape plugin
 */
class InkscapePlugin : public CaymanPlugin
{
protected:
    QList<Dependency> onDependencies() override
    {
        return {{"external_tools"}};
    }

    QString onId() override
    {
        return "inkscape";
    }

    bool onLoad() override
    {
        library = plugin::LibraryPluginFactory::pluginLibrary("external_tools");
        if ( !library )
            return false;
        addTool = library->resolve<bool(const extools::ExternalTool&)>("addTool");
        removeTool = library->resolve<void(const QString&)>("removeTool");
        io::formats().addFormat(new InkscapeSvg);
        /// \todo Re-translatable tool
        if ( addTool )
            addTool( {"inkscape_plugin", tr("Inkscape"),
                "inkscape", {"${temp}.svg"}, "inkscape"});
        return true;
    }

    void onUnload() override
    {
        if ( removeTool )
            removeTool("inkscape_plugin");
        io::formats().deleteFormat(format);
        format = nullptr;
    }

    QString onName() override
    {
        return tr("Inkscape integration");
    }

private:
   InkscapeSvg* format = nullptr;
   plugin::Library* library;
   util::FunctionPointer<bool(const extools::ExternalTool&)> addTool;
   util::FunctionPointer<void(const QString&)> removeTool;
};

INIT_PLUGIN(InkscapePlugin)
