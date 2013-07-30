// $Id: WMain.h 59 2007-07-17 14:43:23Z tb $

/*
 * STX Expression Parser Demo Program v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _WMain_H_
#define _WMain_H_

#include "WMain_wxg.h"

class WMain : public WMain_wxg
{
public:
    WMain();

    void	OnButtonEvaluate(wxCommandEvent &);

    void	OnButtonAddVariable(wxCommandEvent &);
    void	OnButtonDelVariable(wxCommandEvent &);

    DECLARE_EVENT_TABLE();
};

#endif // _WMain_H_
