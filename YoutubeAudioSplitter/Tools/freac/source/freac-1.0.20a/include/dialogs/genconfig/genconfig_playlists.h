 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_GENCONFIG_PLAYLISTS_
#define _H_GENCONFIG_PLAYLISTS_

#include <main.h>

namespace BonkEnc
{
	class GeneralSettingsLayerPlaylists : public Layer
	{
		private:
			GroupBox	*group_options;
			CheckBox	*check_createPlaylists;
			CheckBox	*check_createCueSheets;
			GroupBox	*group_outdir;
			CheckBox	*check_useEncOutdir;
			EditBox		*edit_outdir;
			Button		*button_outdir_browse;
			GroupBox	*group_filename;
			EditBox		*edit_filename;
			ListBox		*list_filename;

			Bool		 createPlaylists;
			Bool		 createCueSheets;

			Bool		 useEncOutdir;
 
			Config		*currentConfig;
		slots:
			Void		 SelectDir();
			Void		 ToggleCreatePlaylists();
			Void		 ToggleUseEncOutdir();
		public:
					 GeneralSettingsLayerPlaylists();
					~GeneralSettingsLayerPlaylists();

			Bool		 GetCreatePlaylists();
			Bool		 GetCreateCueSheets();

			Bool		 GetUseEncOutdir();

			String		 GetOutputDirectory();
			String		 GetFilenamePattern();
	};
};

#endif
