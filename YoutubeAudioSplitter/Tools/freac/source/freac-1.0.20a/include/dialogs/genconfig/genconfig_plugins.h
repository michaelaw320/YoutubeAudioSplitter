 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_GENCONFIG_PLUGINS_
#define _H_GENCONFIG_PLUGINS_

#include <main.h>

namespace BonkEnc
{
	class GeneralSettingsLayerPlugins : public Layer
	{
		private:
			TabWidget	*tabs_plugins;
			Layer		*layer_input;
			ListBox		*list_input;
			Button		*button_input;
			Button		*button_input_about;
			Layer		*layer_output;
			ListBox		*list_output;
			Button		*button_output;
			Button		*button_output_about;

			Config		*currentConfig;
		slots:
			Void		 SelectInputPlugin();
			Void		 SelectOutputPlugin();
			Void		 ConfigureInputPlugin();
			Void		 ConfigureOutputPlugin();
			Void		 AboutInputPlugin();
			Void		 AboutOutputPlugin();
		public:
					 GeneralSettingsLayerPlugins();
					~GeneralSettingsLayerPlugins();

			Int		 GetSelectedOutputPlugin();
	};
};

#endif
