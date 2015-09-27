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
#ifndef PIXEL_CAYMAN_PLUGINS_ANSI_COLOR_PARSER_HPP
#define PIXEL_CAYMAN_PLUGINS_ANSI_COLOR_PARSER_HPP
#include <QImage>
#include <QColor>
#include <QTextStream>

namespace ansi {

/**
 * \brief Parses colors from ansi-formatted text
 */
class ColorParser
{
public:
    /**
     * \brief Color detection
     */
    enum class Option
    {
        Background, ///< Recognize colors by their background (ignores character contents)
        Foreground, ///< Recognizes colors by their foreground (treats spaces as invisible)
        ForeAll,    ///< Recognize colors by their foreground (ignores character contents)
    };

    /**
     * \brief Load from a device
     * \param device            Input device
     * \param option            Color detection option
     * \param pixel_height      Duplicate rows this many times when converting to pixels
     * \param background_color  Used for characters without a color
     */
    explicit ColorParser(QIODevice* device,
                         Option option,
                         int pixel_height = 2,
                         QColor background_color = Qt::transparent)
        : stream(device),
         option(option),
         pixel_height(pixel_height),
         background_color(background_color)
    {
        device->setTextModeEnabled(true);
        stream.setIntegerBase(10);
        parse();
    }

    /**
     * \brief Size of the image in pixels
     */
    QSize size() const
    {
        return {max_w, pixels.size()*pixel_height};
    }

    /**
     * \brief Builds an image from the parsed data
     */
    QImage image() const
    {
        QImage img(size(), QImage::Format_ARGB32);
        img.fill(background_color);
        for ( int y = 0; y < pixels.size(); y++ )
            for ( int i = 0; i < pixel_height; i++ )
                for ( int x = 0; x < pixels[y].size(); x++ )
                    img.setPixel(x, y*pixel_height+i, pixels[y][x].rgba());
        return img;
    }

private:
    /**
     * \brief Push the current color
     */
    void pushPrintable()
    {
        if ( follow_bold && bold )
        {
            pixels.back().push_back(
                QColor::fromHsv(color.hsvHue(), color.hsvSaturation(), 255, color.alpha())
            );
        }
        else
        {
            pixels.back().push_back(color);
        }
    }
    /**
     * \brief Push the current color or the background
     */
    void pushSpace()
    {
        if ( option == Option::Foreground )
            pixels.back().push_back(background_color);
        else
            pushPrintable();
    }

    /**
     * \brief Add a new line of pixels
     */
    void pushLine()
    {
        if ( !pixels.empty() && pixels.back().size() > max_w )
            max_w = pixels.back().size();

        QVector<QColor> line;
        line.reserve(max_w);
        pixels.push_back(line);
    }

    /**
     * \brief Parse the input device
     */
    void parse()
    {
        while ( !stream.status() )
            parseLine();

        if ( pixels.back().empty() )
            pixels.pop_back();
    }

    /**
     * \brief Parse a line of text
     */
    void parseLine()
    {
        pushLine();
        QChar ch;
        while ( !stream.status() )
        {
            stream >> ch;
            if ( ch == '\x1b' )
                parseCode();
            else if ( ch == '\n' )
                break;
            else if ( ch.isSpace() )
                pushSpace();
            else if ( ch.isPrint() )
                pushPrintable();
        }
    }

    /**
     * \brief Parse an ANSI code
     */
    void parseCode()
    {
        QChar ch;
        stream >> ch;
        if ( ch != '[' )
            return;

        QList<int> args;
        do
        {
            int arg;
            stream >> arg >> ch;
            args.push_back(arg);
        }
        while ( !stream.status() && ch != 'm' && ch != '\n' );

        if ( ch != 'm' )
            return;

        for ( int i = 0; i < args.size(); i++ )
        {
            int arg = args[i];
            if ( arg == 0 )
                clearFormat();
            else if ( arg == 1 )
                bold = true;
            else if ( arg == 2 || arg == 21 || arg == 22 )
                bold = false;
            else if ( arg == 7 )
                swapped = true;
            else if ( arg == 8 )
                color.setAlpha(0);
            else if ( arg == 27 )
                swapped = false;
            else if ( arg == 28 )
                color.setAlpha(255);
            else if ( arg >= 30 && arg <= 37 && foreground() )
                set8Color(arg);
            else if ( arg == 38 && foreground() )
                set24bitColor(i, args);
            else if ( arg == 39 && foreground() )
                color = Qt::transparent;
            else if ( arg >= 40 && arg <= 47 && background() )
                set8Color(arg);
            else if ( arg == 48 && background() )
                set24bitColor(i, args);
            else if ( arg == 49 && background() )
                color = Qt::transparent;
            else if ( arg >= 90 && arg <= 97 && foreground() )
                set8Color(arg);
            else if ( arg >= 100 && arg <= 107 && background() )
                set8Color(arg);
        }
    }

    /**
     * \brief Reset to the default
     */
    void clearFormat()
    {
        color = Qt::transparent;
        bold = false;
        swapped = false;
        follow_bold = true;
    }

    /**
     * \brief Whether should get the color from the background codes
     */
    bool background()
    {
        return (option == Option::Background) ^ swapped;
    }

    /**
     * \brief Whether should get the color from the foreground codes
     */
    bool foreground()
    {
        return !background();
    }

    /**
     * \brief Set an indexed/3bit color
     */
    void set8Color(int value)
    {
        follow_bold = true;

        switch ( value % 10 )
        {
            case 0: color = Qt::black; break;
            case 1: color = Qt::darkRed; break;
            case 2: color = Qt::darkGreen; break;
            case 3: color = Qt::darkYellow; break;
            case 4: color = Qt::darkBlue; break;
            case 5: color = Qt::darkMagenta; break;
            case 6: color = Qt::darkCyan; break;
            case 7: color = Qt::gray; break;
        }
    }

    /**
     * \brief Set a 24 bit color
     */
    void set24bitColor(int& i, const QList<int>& args)
    {
        int j = i+1;
        i += 4;
        if ( i >= args.size() || args[j] != 2 )
            return;
        color = QColor(args[j+1], args[j+2], args[j+3]);
        follow_bold = false;
    }

    QVector<QVector<QColor>> pixels;
    QColor color;
    int max_w = 0;
    QTextStream stream;
    Option option;
    bool bold = false;
    bool swapped = false;
    bool follow_bold = true;
    int pixel_height = 2;
    QColor background_color;
};

} // namespace ansi
#endif // PIXEL_CAYMAN_PLUGINS_ANSI_COLOR_PARSER_HPP
