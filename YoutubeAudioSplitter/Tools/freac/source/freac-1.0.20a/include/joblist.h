 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_JOBLIST_
#define _H_JOBLIST_

#include <smooth.h>
#include "config.h"
#include "track.h"

using namespace smooth;
using namespace smooth::GUI;

namespace BonkEnc
{
	class BEEXPORT JobList : public ListBox
	{
		private:
			DropArea		*droparea;
			Text			*text;

			Button			*button_sel_all;
			Button			*button_sel_none;
			Button			*button_sel_toggle;

			Array<Track *>		 tracks;

			Bool			 AddTrack(Track *);

			Void			 UpdateTextLine();

			const String		&GetTotalDuration();
		public:
						 JobList(const Point &, const Size &);
			virtual			~JobList();

			Int			 GetNOfTracks();
			Track			*GetNthTrack(Int);

			Bool			 RemoveNthTrack(Int);
			Bool			 RemoveAllTracks();

			Track			*GetSelectedTrack();

			Int			 SetMetrics(const Point &, const Size &);

			Void			 AddTrackByDialog();
			Void			 AddTracksByPattern(const String &, const String &);

			Void			 AddTrackByFileName(const String &, const String & = NIL, Bool = True);

			Void			 RemoveSelectedTrack();
		signals:
			Signal1<Void, Track *>	 onSelectTrack;
			Signal0<Void>		 onSelectNone;

			Signal0<Void>		 onRemovePlayingTrack;
		slots:
			Void			 AddTrackByDragAndDrop(const String &);

			Void			 SelectAll();
			Void			 SelectNone();
			Void			 ToggleSelection();

			Void			 LoadList();
			Void			 SaveList();

			Void			 OnRegister(Widget *);
			Void			 OnUnregister(Widget *);

			Void			 OnSelectEntry();

			Void			 OnChangeLanguageSettings();
	};
};

#endif
