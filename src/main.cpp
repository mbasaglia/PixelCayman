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
#include <exception>
#include <QtWidgets/QApplication>
#include <QDebug>
#include "ui/main_window.hpp"
#include "info.hpp"
#include "settings.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    settings::Settings settings_object;

    try
    {
        init_info();

        MainWindow window;
        window.show();
        return app.exec();
    }
    catch ( const std::exception& exc )
    {
        qWarning() << "Exception: " << exc.what();
    }
    catch ( ... )
    {
        qWarning() << "Unknown Exception";
    }

    return 1;
}

settings::Settings* settings::Settings::singleton = nullptr;
