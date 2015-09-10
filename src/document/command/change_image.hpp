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
#ifndef PIXEL_CAYMAN_DOCUMENT_COMMAND_CHANGE_IMAGE_HPP
#define PIXEL_CAYMAN_DOCUMENT_COMMAND_CHANGE_IMAGE_HPP

#include <QUndoCommand>
#include <QImage>

namespace document {

class Image;

namespace command {

/**
 * \brief Command that changes the pixels of an image
 */
class ChangeImage : public QUndoCommand
{
public:
    ChangeImage(const QString& text,
                Image* image,
                QImage before,
                QImage after = QImage(),
                QUndoCommand* parent = nullptr )
        : QUndoCommand(text, parent), image(image), before(before), after(after)
    {}

    void setAfterImage(const QImage& image)
    {
        after = image;
    }

    void undo() override;

    void redo() override;

private:
    Image* image;
    QImage before;
    QImage after;
};

} // namespace command
} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_COMMAND_CHANGE_IMAGE_HPP
