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

#include "ui/main_window.hpp"
#include "cayman/message.hpp"
#include "cayman/application.hpp"

int main(int argc, char** argv)
{
    cayman::Application app(argc, argv);
    try
    {
        app.initSubsystems();
        MainWindow window;
        window.show();
        return app.exec();
    }
    catch ( const std::exception& exc )
    {
        cayman::Message(Msg::AllOutput|Msg::Critical) << "Exception: " << exc.what();
    }
    catch ( ... )
    {
        cayman::Message(Msg::AllOutput|Msg::Critical) << "Unknown Exception";
    }

    return 1;
}
