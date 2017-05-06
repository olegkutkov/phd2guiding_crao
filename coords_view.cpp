/*
 *  coords_view.cpp
 *  PHD Guiding
 *
 *  Created by Oleg Kutkov
 *  Copyright (c) 2017 Crimean Astrophysical observatory
 *  All rights reserved.
 *
 *  This source code is distributed under the following "BSD" license
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *    Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *    Neither the name of Bret McKee, Dad Dog Development Ltd, nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#include "phd.h"
#include "coords_view.h"

BEGIN_EVENT_TABLE(CoordsView, wxWindow)
    EVT_PAINT(CoordsView::OnPaint)
END_EVENT_TABLE()

CoordsView::CoordsView(wxWindow *parent):
	wxWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, "Scope position")
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->visible = false;
}

CoordsView::~CoordsView()
{
}

void CoordsView::UpdateData(double ha, double ra, double dec, double ra_sp, double dec_sp)
{
	curr_ha = ha;
	curr_ra = ra;
	curr_dec = dec;
	curr_ra_sp = ra_sp;
	curr_dec_sp = dec_sp;

    if (this->visible) {
        Refresh();
	}
}

void CoordsView::SetState(bool is_active)
{
    this->visible = is_active;
    if (is_active)
        Refresh();
}

void CoordsView::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	wxAutoBufferedPaintDC dc(this);

	dc.SetBackground(wxColour(0,0,0));

	dc.Clear();

	if (!pFrame || !pFrame->pGuider || pFrame->pGuider->GetState() == STATE_UNINITIALIZED) return;

#if defined (__APPLE__)
    const wxFont& smallFont = *wxSMALL_FONT;
#else
    const wxFont& smallFont = *wxSWISS_FONT;
#endif

	const int ysize = this->GetSize().GetY();

	wxFont largeFont = smallFont.Scaled(1.25);

    dc.SetFont(largeFont);

	dc.SetTextForeground(*wxWHITE);

	dc.DrawText("t: ", 10, 5);
	

	dc.DrawText("RA: ", 10, 30);
	dc.DrawText("DEC: ", 10, 55);
	dc.DrawText("RA SPEED: ", 10, 80);
	dc.DrawText("DEC SPEED: ", 10, 110);
}



