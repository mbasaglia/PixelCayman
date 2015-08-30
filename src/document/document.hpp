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
#ifndef PIXEL_CAYMAN_DOCUMENT_HPP
#define PIXEL_CAYMAN_DOCUMENT_HPP

#include "animation.hpp"
#include "layer.hpp"

namespace document {

class Document : public DocumentElement
{
public:
    explicit Document(const QSize& size = {},
                      const QString& file_name = {},
                      const Metadata& metadata = {});

    explicit Document(const QImage& image, const QString& file_name);

    ~Document();
    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;

    QString fileName() const;
    void setFileName(const QString& file_name);

    QSize imageSize() const;

    QList<const Animation*> animations() const;
    QList<Animation*> animations();
    const Animation* animation(const QString & name) const;
    Animation* animation(const QString & name);
    Animation* addAnimation(const QString& name);
    void removeAnimation(Animation* animation);

    const Layer* rootLayer() const;
    Layer* rootLayer();

    void apply(Visitor& visitor) override;

private:
    Layer* root;
    QList<Animation*> animations_;
    QSize image_size;
    QString file_name;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_HPP
