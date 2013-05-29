/**
\file

\author Mattia Basaglia

\section License
This file is part of Knotter.

Copyright (C) 2012-2013 Mattia Basaglia

Knotter is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Knotter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef EXTENTED_TAB_WIDGET_HPP
#define EXTENTED_TAB_WIDGET_HPP

#include <QTabWidget>

class Extended_TabWidget : public QTabWidget
{
    Q_OBJECT

    public:
        Extended_TabWidget(QWidget* parent=NULL);

    signals:
        /// emitted when the user wants to detach tab
        void dragAway(int tab);
};

#endif // EXTENTED_TAB_WIDGET_HPP

