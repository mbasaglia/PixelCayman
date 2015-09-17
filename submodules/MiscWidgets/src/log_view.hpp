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
#ifndef MISCWIDGETS_LOG_VIEW_HPP
#define MISCWIDGETS_LOG_VIEW_HPP

#include <QTextEdit>

class LogView : public QTextEdit
{
    Q_OBJECT

    Q_PROPERTY(Stream visibleLog READ visibleLog WRITE setVisibleLog)
    Q_PROPERTY(QColor stdoutColor READ stdoutColor WRITE setStdoutColor)
    Q_PROPERTY(QColor stderrColor READ stderrColor WRITE setStderrColor)


public:
    enum Stream
    {
        None   = 0,
        Stdout = 1,
        Stderr = 2,
        Both   = Stdout|Stderr
    };

private:
    struct Line
    {
        QString text;
        Stream stream;
    };

    Q_ENUMS(Line)

public:
   explicit LogView(QWidget* parent = nullptr) : QTextEdit(parent)
   {
       setReadOnly(true);
       setUndoRedoEnabled(false);
       setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);
       setLineWrapMode(QTextEdit::NoWrap);
   }

    QStringList lines(Stream stream = Both) const
    {
        QStringList ret;
        for ( const auto& line : lines_ )
            if ( line.stream & stream )
                ret.push_back(line.text);
        return ret;
    }

    void clearLog(Stream stream = Both)
    {
        if ( stream == Both )
            lines_.clear();

        for ( auto it = lines_.begin(); it != lines_.end(); )
        {
            if ( it->stream == stream )
                it = lines_.erase(it);
            else
                ++it;
        }

        clear();
    }

    QColor stdoutColor() const
    {
        return stdout_color;
    }

    void setStdoutColor(const QColor& color)
    {
        stdout_color = color;
        redrawAll();
    }

    QColor stderrColor() const
    {
        return stderr_color;
    }

    void setStderrColor(const QColor& color)
    {
        stderr_color = color;
        redrawAll();
    }

    Stream visibleLog() const
    {
        return visible_;
    }

    void setVisibleLog(Stream log)
    {
        visible_ = log;
        redrawAll();
    }

public slots:
    void logStdout(const QString& text)
    {
        lines_.push_back({text, Stdout});
        drawLine(lines_.back());
    }

    void logStderr(const QString& text)
    {
        lines_.push_back({text, Stderr});
        drawLine(lines_.back());
    }

signals:
    void requestSave();

private:
    void drawLine(const Line& line)
    {
        if ( !(line.stream & visible_) )
            return;

        QColor color = line.stream == Stderr ? stderr_color : stdout_color;
        if ( color.isValid() )
            setTextColor(color);
        append(line.text);
    }

    void redrawAll()
    {
        clear();
        for ( const auto& line : lines_ )
            drawLine(line);
    }

    QList<Line> lines_;
    Stream visible_ = Both;
    QColor stdout_color;
    QColor stderr_color;
};


#endif // MISCWIDGETS_LOG_VIEW_HPP
