 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_INPUTFILTER_
#define _H_INPUTFILTER_

#include <smooth/io/filter.h>
#include <smooth/io/driver.h>
#include "../bonkenc.h"

#include <3rdparty/id3.h>

using namespace smooth::IO;

namespace BonkEnc
{
	class BEEXPORT InputFilter : public Filter
	{
		private:
			Bool		 ParseID3V2Tag(ID3Tag *, Track *);
			String		 GetID3V2FrameString(ID3Frame *);
		protected:
			Int		 inBytes;

			Bool		 errorState;
			String		 errorString;

			UnsignedInt	 fileSize;
			Track		*format;
			Config		*currentConfig;

			Bool		 ParseID3V2Tag(unsigned char *, Int, Track *);
			Bool		 ParseID3V2Tag(const String &, Track *);

			String		 GetID3CategoryName(Int);
		public:
					 InputFilter(Config *, Track *);
			virtual		~InputFilter();

			virtual Int	 ReadData(Buffer<UnsignedByte> &, Int) = 0;

			virtual Track	*GetFileInfo(const String &) = 0;
			virtual Bool	 SetFileSize(UnsignedInt);

			Int		 GetInBytes()		{ return inBytes; }

			Bool		 GetErrorState()	{ return errorState; }
			const String	&GetErrorString()	{ return errorString; }
	};
};

#endif
