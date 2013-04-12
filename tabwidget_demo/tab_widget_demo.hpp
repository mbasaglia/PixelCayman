/**
  
\file

\author Mattia Basaglia

\section License
This file is part of Knotter.

Copyright (C) 2012-2013  Mattia Basaglia

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

#ifndef TAB_WIDGET_DEMO_HPP
#define TAB_WIDGET_DEMO_HPP

#include "ui_tab_widget_demo.h"

class Tab_Widget_Demo : public QMainWindow, private Ui::Tab_Widget_Demo
{
    Q_OBJECT
    
public:
    explicit Tab_Widget_Demo(QWidget *parent = 0);
private slots:
    void on_tabby_dragAway(int tab);
    void on_tabby_tabCloseRequested(int index);
};

#endif // TAB_WIDGET_DEMO_HPP
