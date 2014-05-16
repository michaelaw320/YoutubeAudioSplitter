 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_PICTURE_
#define _H_PICTURE_

#include <smooth.h>

using namespace smooth;

namespace BonkEnc
{
	class BEEXPORT Picture
	{
		public:
						 Picture();
						 Picture(const Picture &);
						~Picture();

			Int			 type;
			String			 mime;
			String			 description;
			Buffer<unsigned char>	 data;
	};
};

#endif
