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
#ifndef PIXEL_CAYMAN_ANIMATION_HPP
#define PIXEL_CAYMAN_ANIMATION_HPP

#include "frame.hpp"

namespace document {

class Animation : public DocumentElement
{
    Q_OBJECT
public:
    explicit Animation(const QString& name = {}, int frames_per_second = 24);

    QString name() const;
    void setName(const QString& name);

    int framesPerSecond() const;
    void setFramesPerSecond(int fps);

    QList<Frame*> frames() const;
    int count() const;
    Frame* frame(int i);
    Frame* createFrame(int position = -1, int copy = -1);
    void removeFrame(int position);

    void apply(Visitor& visitor) override;

private:
    QString name_;
    int fps_;
    QList<Frame*> frames_;
};

} // namespace document
#endif // PIXEL_CAYMAN_ANIMATION_HPP
