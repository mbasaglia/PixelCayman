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
#ifndef PIXEL_CAYMAN_LAYER_OPERATION_HPP
#define PIXEL_CAYMAN_LAYER_OPERATION_HPP

#include "document_layer.hpp"

#include <QUndoCommand>

/**
 * \brief An action that modifies a layer image
 */
class LayerImageAction
{
public:
    virtual ~LayerImageAction(){}

    /**
     * \brief Executes the action on the image
     * \param input Starting image
     * \param output Image to overwrite, at the beginning it shares data with \p input
     * \param layer Layer being edited
     */
    virtual void apply(const QImage& input, const QImage& output, const DocumentLayer* layer) const = 0;
};

/**
 * \brief A command that changes the image on the layer
 */
class LayerImageCommand : public QUndoCommand
{
public:
    /**
     * \pre layer != nullptr && action != nullptr
     */
    explicit LayerImageCommand(const QString& text, LayerImageAction* action,
                            DocumentLayer* layer, QUndoCommand *parent = nullptr)
        : QUndoCommand(text, parent),
        layer_(layer),
        before_(layer.image().copy()),
        after_(before_)
    {
        action->apply(before_, after_, layer);
    }

    virtual ~LayerOperation() {}

    /**
     * \brief Applies the operation to the layer
     */
    void redo() override
    {
        layer_->setImage(after_);
    }

    /**
     * \brief Restores the layer to its previous state
     */
    void undo() override
    {
        layer_->setImage(before_);
    }

    /**
     * \brief The affected layer
     */
    DocumentLayer* layer() const
    {
        return layer;
    }

protected:
    /**
     * \brief Performs the action on the layer image
     * \return \b true On success
     */
    virtual bool on_apply(QImage& image, DocumentLayer* layer) = 0;

private:
    DocumentLayer* layer_;
    QImage before_;
    QImage after_;
};

#endif // PIXEL_CAYMAN_LAYER_OPERATION_HPP
