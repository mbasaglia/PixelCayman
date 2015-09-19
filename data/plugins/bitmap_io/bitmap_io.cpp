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

#include "formats.hpp"
#include "plugin.hpp"

namespace bitmapio {


class BitmapIO : public CaymanPlugin
{
protected:
    QString onId() override
    {
        return "bitmapio";
    }

    bool onLoad() override
    {
        addFormat(new FormatPng);
        addFormat(new FormatTiff);
        addFormat(new FormatBmp);
        addFormat(new FormatTga);
        addFormat(new FormatPpm);
        addFormat(new FormatXpm);
        addFormat(new FormatXbm);
        addFormat(new FormatIco);
        addFormat(new FormatWebp);
        addFormat(new FormatEps);
        return true;
    }

    void onUnload() override
    {
        for ( auto fmt : formats )
            io::formats().deleteFormat(fmt);
        formats.clear();
    }

    QString onName() override
    {
        return tr("Bitmap I/O");
    }

private:
    void addFormat(SingleBitmapFormat* fmt)
    {
        formats.push_back(fmt);
        io::formats().addFormat(fmt);
    }

    QList<SingleBitmapFormat*> formats;
};

} // namespace bitmapio

INIT_PLUGIN(bitmapio::BitmapIO)
