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
#ifndef PIXEL_CAYMAN_APPLICATION_HPP
#define PIXEL_CAYMAN_APPLICATION_HPP

#include <QApplication>
#include "settings.hpp"

namespace cayman {

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application();

    void initSubsystems();

protected:
    bool event(QEvent* event) override;

private:
    void initPlugins();
    void initFormats();
    void initTools();

    settings::Settings* settings_;
};

} // namespace cayman
#endif // PIXEL_CAYMAN_APPLICATION_HPP
