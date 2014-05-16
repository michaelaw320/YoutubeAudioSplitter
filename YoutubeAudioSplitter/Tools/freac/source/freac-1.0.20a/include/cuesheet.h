 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_CUESHEET_
#define _H_CUESHEET_

#include <smooth.h>

using namespace smooth;

namespace BonkEnc
{
	class BEEXPORT CueSheet
	{
		private:
			Array<String>	 fileNames;
			Array<Int>	 trackOffsets;
			Array<String>	 trackTitles;
			Array<String>	 trackArtists;
			Array<String>	 trackAlbums;
		public:
			Bool		 AddTrack(const String &, Int, const String &, const String &, const String &);

			Bool		 Save(const String &);
	};
};

#endif
