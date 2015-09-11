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
#include "command/move_child_layers.hpp"

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

    /**
     * \brief Number of layers
     */
    int countLayers() const;

    /**
     * \brief Add a layer to be in position \p index
     * \param layer Layer to be added
     * \param index Position it should be in, if an invalid position is given
     *              the layer will be appended
     */
    void insertLayer(Layer* layer, int index = -1);

    /**
     * \brief Remove a layer from the container
     * \return \b true on success
     */
    bool removeLayer(Layer* layer);

    /**
     * \brief Get the layer at the given index
     * \return \b nullptr if \p index is invalid
     */
    Layer* layer(int index);

    /**
     * \brief The index in children() of the given layer
     * \return -1 if the layer doesn't exist
     */
    int layerIndex(Layer* layer) const;

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
     * \brief Removes a layer without generating a command
     */
    bool removeLayerRaw(Layer* layer);

    /**
     * \brief Actions to perform when inserting a layer
     */
    virtual void onInsertLayer(Layer* layer) = 0;
    /**
     * \brief Actions to perform when removing a layer
     */
    virtual void onRemoveLayer(Layer* layer) = 0;

signals:
    /**
     * \brief Emitted when a layer has been added
     */
    void layerAdded(Layer* layer, LayerContainer* parent, int index);
    /**
     * \brief Emitted when a layer has been removed
     */
    void layerRemoved(Layer* layer, LayerContainer* parent, int index);

private:
    QList<Layer*> layers_;

    friend class command::AddLayer;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_LAYER_CONTAINER_HPP
