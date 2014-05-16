 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_FILTER_OUT_BONK_
#define _H_FILTER_OUT_BONK_

#include "outputfilter.h"

namespace BonkEnc
{
	class BEEXPORT FilterOutBONK : public OutputFilter
	{
		private:
			void			*encoder;

			Buffer<signed short>	 samplesBuffer;
			Buffer<unsigned char>	 dataBuffer;
		public:
						 FilterOutBONK(Config *, Track *);
						~FilterOutBONK();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);
	};
};

#endif
