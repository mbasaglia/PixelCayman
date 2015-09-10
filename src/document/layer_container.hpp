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
#ifndef PIXEL_CAYMAN_DOCUMENT_LAYER_CONTAINER_HPP
#define PIXEL_CAYMAN_DOCUMENT_LAYER_CONTAINER_HPP

#include "document_element.hpp"

namespace document {
class Layer;

/**
 * \brief Base class for elements that contain layers
 */
class LayerContainer : public DocumentElement
{
    Q_OBJECT

public:
    explicit LayerContainer(const Metadata& metadata = {});

    /**
     * \brief Child layers
     */
    QList<Layer*> layers()
    {
        return layers_;
    }

    void insertLayer(Layer* layer, int index = -1);

    Layer* layer(int index);

    /**
     * \brief Calls apply() on all the layers
     */
    void apply(Visitor& visitor) override;

protected:
    /**
     * \brief Inserts a layer without generating a command
     */
    void insertLayerRaw(Layer* layer, int index);

    /**
     * \brief Actions to perform when inserting a layer
     */
    virtual void onInsert(Layer* layer) = 0;

signals:
    /**
     * \brief Emitted on operations that changes the layer layout
     *
     * Eg: New layers added, re-ordered etc
     */
    void layersChanged();

private:
    QList<Layer*> layers_;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_LAYER_CONTAINER_HPP
