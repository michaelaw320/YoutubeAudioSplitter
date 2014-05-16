 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <input/filter-in-cdrip.h>
#include <3rdparty/cdrip/cdrip.h>
#include <3rdparty/paranoia/cdda_paranoia.h>
#include <cddb/cddbremote.h>
#include <cddb/cddbcache.h>

#include <dllinterfaces.h>

Bool			 BonkEnc::FilterInCDRip::readActive	= False;

BonkEnc::CDPlayerIni	 BonkEnc::FilterInCDRip::cdPlayer;
Int			 BonkEnc::FilterInCDRip::cdPlayerDiscID	= -1;

BonkEnc::CDText		 BonkEnc::FilterInCDRip::cdText;
Int			 BonkEnc::FilterInCDRip::cdTextDiscID	= -1;

BonkEnc::CDDBInfo	 BonkEnc::FilterInCDRip::cdInfo;
Int			 BonkEnc::FilterInCDRip::cdInfoDiscID	= -1;

BonkEnc::FilterInCDRip::FilterInCDRip(Config *config, Track *format) : InputFilter(config, format)
{
	packageSize	= 0;

	trackNumber	= -1;
	buffer		= NIL;
}

BonkEnc::FilterInCDRip::~FilterInCDRip()
{
	if (buffer != NIL)
	{
		delete [] buffer;

		ex_CR_CloseRipper();

		if (currentConfig->cdrip_locktray) ex_CR_LockCD(false);
	}
}

Int BonkEnc::FilterInCDRip::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (trackNumber == -1) return true;

	if (byteCount >= trackSize)
	{
		if (buffer != NIL)
		{
			delete [] buffer;

			buffer = NIL;

			ex_CR_CloseRipper();

			if (currentConfig->cdrip_locktray) ex_CR_LockCD(false);
		}

		trackNumber = -1;

		return true;
	}

	LONG	 nBytes;
	BOOL	 abort = false;

	ex_CR_RipChunk(buffer, &nBytes, abort);

	byteCount += nBytes;

	size = nBytes;

	data.Resize(size);

	memcpy(data, buffer, size);

	return size;
}

Bool BonkEnc::FilterInCDRip::SetTrack(Int newTrack)
{
	if (buffer != NIL)
	{
		delete [] buffer;

		ex_CR_CloseRipper();

		if (currentConfig->cdrip_locktray) ex_CR_LockCD(false);
	}

	trackNumber = newTrack;

	int	 numTocEntries;
	TOCENTRY entry;

	ex_CR_SetActiveCDROM(currentConfig->cdrip_activedrive);

	ex_CR_ReadToc();

	numTocEntries = ex_CR_GetNumTocEntries();

	entry.btTrackNumber = 0;
	entry.dwStartSector = 0;

	for (int i = 0; i < numTocEntries; i++)
	{
		entry = ex_CR_GetTocEntry(i);

		if (!(entry.btFlag & CDROMDATAFLAG) && (entry.btTrackNumber == trackNumber))	break;
		else										entry.btTrackNumber = 0;
	}

	if (entry.btTrackNumber == 0)
	{
		trackNumber = -1;

		return false;
	}

	int	 startSector	= entry.dwStartSector;
	int	 endSector	= 0;
	TOCENTRY entry2		= ex_CR_GetTocEntry(0);

	for (int j = 1; j <= numTocEntries; j++)
	{
		if (entry2.btTrackNumber == entry.btTrackNumber || entry2.btTrackNumber == 0xAA)

		{
			if ((j > 1) && (entry2.btFlag != ex_CR_GetTocEntry(j).btFlag) && (ex_CR_GetTocEntry(j).btTrackNumber != 0xAA))
			{
				endSector = ex_CR_GetTocEntry(j).dwStartSector - 11250 - 1;
			}
			else
			{
				endSector = ex_CR_GetTocEntry(j).dwStartSector - 1;
			}

			break;
		}

		entry2 = ex_CR_GetTocEntry(j);
	}

	trackSize = (endSector - startSector + 1) * 2352;
	byteCount = 0;

	LONG		 bufferSize = 0;
	CDROMPARAMS	 params;
	int		 nParanoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;

	switch (currentConfig->cdrip_paranoia_mode)
	{
		case 0:
			nParanoiaMode = PARANOIA_MODE_OVERLAP;
			break;
		case 1:
			nParanoiaMode &= ~PARANOIA_MODE_VERIFY;
			break;
		case 2:
			nParanoiaMode &= ~(PARANOIA_MODE_SCRATCH | PARANOIA_MODE_REPAIR);
			break;
	}	
 

	ex_CR_GetCDROMParameters(&params);

	params.nRippingMode		= currentConfig->cdrip_paranoia;
	params.nParanoiaMode		= nParanoiaMode;
	params.bSwapLefRightChannel	= currentConfig->cdrip_swapchannels;
	params.bJitterCorrection	= currentConfig->cdrip_jitter;
	params.bDetectJitterErrors	= currentConfig->cdrip_detectJitterErrors;
	params.bDetectC2Errors		= currentConfig->cdrip_detectC2Errors;
	params.nSpeed			= currentConfig->cdrip_speed;
	params.bEnableMultiRead		= False;
	params.nMultiReadCount		= 0;

	/* Set maximum speed if no limit is requested.
	 */
	if (params.nSpeed == 0) params.nSpeed = 64;

	ex_CR_SetCDROMParameters(&params);

	if (currentConfig->cdrip_locktray) ex_CR_LockCD(true);

	ex_CR_OpenRipper(&bufferSize, startSector, endSector);

	buffer = new unsigned char [bufferSize];

	return true;
}

Int BonkEnc::FilterInCDRip::GetTrackSize()
{
	if (trackNumber == -1) return 0;

	return trackSize;
}

BonkEnc::Track *BonkEnc::FilterInCDRip::GetFileInfo(const String &inFile)
{
	Track	*nFormat = new Track;

	nFormat->channels = 2;
	nFormat->rate = 44100;
	nFormat->bits = 16;
	nFormat->order = BYTE_INTEL;

	Int	 trackNumber = 0;
	Int	 trackLength = 0;
	Int	 audiodrive = 0;

	if (inFile.StartsWith("/cda"))
	{
		String	 track;

		for (Int j = 4; j < inFile.Length(); j++) track[j - 4] = inFile[j];

		audiodrive = currentConfig->cdrip_activedrive;

		ex_CR_SetActiveCDROM(currentConfig->cdrip_activedrive);

		ex_CR_ReadToc();

		Int	 numTocEntries = ex_CR_GetNumTocEntries();

		TOCENTRY	 entry;
		TOCENTRY	 nextentry;

		entry.btTrackNumber = 0;

		for (Int i = 0; i < numTocEntries; i++)
		{
			entry = ex_CR_GetTocEntry(i);
			nextentry = ex_CR_GetTocEntry(i + 1);

			if ((i > 0) && (entry.btFlag != nextentry.btFlag) && (nextentry.btTrackNumber != 0xAA))
			{
				trackLength = nextentry.dwStartSector - entry.dwStartSector - 11250;
			}
			else
			{
				trackLength = nextentry.dwStartSector - entry.dwStartSector;
			}

			if (!(entry.btFlag & CDROMDATAFLAG) && (entry.btTrackNumber == track.ToInt()))	break;
			else										entry.btTrackNumber = 0;
		}

		trackNumber = entry.btTrackNumber;
	}
	else if (inFile.Length() >= 5)
	{
		if (inFile[inFile.Length() - 4] == '.' && inFile[inFile.Length() - 3] == 'c' && inFile[inFile.Length() - 2] == 'd' && inFile[inFile.Length() - 1] == 'a')
		{
			InStream	*in = new InStream(STREAM_FILE, inFile, IS_READONLY);

			in->Seek(22);

			trackNumber = in->InputNumber(2);

			in->Seek(32);

			trackLength = in->InputNumber(4);

			delete in;

			for (audiodrive = 0; audiodrive < currentConfig->cdrip_numdrives; audiodrive++)
			{
				Bool	 done = false;

				ex_CR_SetActiveCDROM(audiodrive);

				ex_CR_ReadToc();

				Int	 numTocEntries = ex_CR_GetNumTocEntries();

				for (int j = 0; j < numTocEntries; j++)
				{
					TOCENTRY	 entry = ex_CR_GetTocEntry(j);
					TOCENTRY	 nextentry = ex_CR_GetTocEntry(j + 1);
					Int		 length = nextentry.dwStartSector - entry.dwStartSector;

					if (!(entry.btFlag & CDROMDATAFLAG) && entry.btTrackNumber == trackNumber && length == trackLength)
					{
						done = true;
						break;
					}
				}

				if (done) break;
			}

			Int	 numTocEntries = ex_CR_GetNumTocEntries();

			TOCENTRY	 entry;
			TOCENTRY	 nextentry;

			entry.btTrackNumber = 0;

			for (Int i = 0; i < numTocEntries; i++)
			{
				entry = ex_CR_GetTocEntry(i);
				nextentry = ex_CR_GetTocEntry(i + 1);

				if ((i > 0) && (entry.btFlag != nextentry.btFlag) && (nextentry.btTrackNumber != 0xAA))
				{
					trackLength = nextentry.dwStartSector - entry.dwStartSector - 11250;
				}
				else
				{
					trackLength = nextentry.dwStartSector - entry.dwStartSector;
				}

				if (!(entry.btFlag & CDROMDATAFLAG) && (entry.btTrackNumber == trackNumber))	break;
				else										entry.btTrackNumber = 0;
			}
		}
	}

	if (trackNumber == 0)
	{
		delete nFormat;

		return NIL;
	}

	nFormat->length = (trackLength * 2352) / (nFormat->bits / 8);
	nFormat->fileSize = trackLength * 2352;

	CDDBRemote	 cddb(currentConfig);

	cddb.SetActiveDrive(audiodrive);

	Int		 discid = cddb.ComputeDiscID();

	if (cdTextDiscID != discid)
	{
		cdText.ReadCDText();

		cdTextDiscID = discid;
	}

	if (cdPlayerDiscID != discid)
	{
		cdPlayer.ReadCDInfo();

		cdPlayerDiscID = discid;
	}

	if (cdInfoDiscID != discid && !((cdText.GetCDInfo().GetTrackTitle(trackNumber) != NIL || cdPlayer.GetCDInfo().GetTrackTitle(trackNumber) != NIL) && !currentConfig->enable_overwrite_cdtext))
	{
		if (readActive)
		{
			if (currentConfig->enable_cddb_cache) cdInfo = currentConfig->cddbCache->GetCacheEntry(discid);

			cdInfoDiscID = discid;
		}

		if (cdInfo == NIL && currentConfig->enable_auto_cddb)
		{
			Int	 oDrive = currentConfig->cdrip_activedrive;

			currentConfig->cdrip_activedrive = audiodrive;

			cdInfo = currentConfig->appMain->GetCDDBData();

			if (cdInfo != NIL) currentConfig->cddbCache->AddCacheEntry(cdInfo);

			currentConfig->cdrip_activedrive = oDrive;
		}
	}

	if (cdInfo != NIL)
	{
		nFormat->track		= trackNumber;
		nFormat->cdTrack	= trackNumber;
		nFormat->discid		= CDDB::DiscIDToString(cddb.ComputeDiscID());
		nFormat->drive		= audiodrive;
		nFormat->outfile	= NIL;
		nFormat->artist		= (cdInfo.dArtist == "Various" ? cdInfo.trackArtists.GetNth(trackNumber - 1) : cdInfo.dArtist);
		nFormat->title		= cdInfo.trackTitles.GetNth(trackNumber - 1);
		nFormat->album		= cdInfo.dTitle;
		nFormat->genre		= cdInfo.dGenre;
		nFormat->year		= cdInfo.dYear;
	}
	else if (cdText.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
	{
		nFormat->track		= trackNumber;
		nFormat->cdTrack	= trackNumber;
		nFormat->discid		= CDDB::DiscIDToString(cddb.ComputeDiscID());
		nFormat->drive		= audiodrive;
		nFormat->outfile	= NIL;
		nFormat->artist		= cdText.GetCDInfo().GetTrackArtist(trackNumber);
		nFormat->title		= cdText.GetCDInfo().GetTrackTitle(trackNumber);
		nFormat->album		= cdText.GetCDInfo().GetTitle();

		if (nFormat->artist == NIL) nFormat->artist = cdText.GetCDInfo().GetArtist();
	}
	else if (cdPlayer.GetCDInfo().GetTrackTitle(trackNumber) != NIL)
	{
		nFormat->track		= trackNumber;
		nFormat->cdTrack	= trackNumber;
		nFormat->discid		= CDDB::DiscIDToString(cddb.ComputeDiscID());
		nFormat->drive		= audiodrive;
		nFormat->outfile	= NIL;
		nFormat->artist		= cdPlayer.GetCDInfo().GetArtist();
		nFormat->title		= cdPlayer.GetCDInfo().GetTrackTitle(trackNumber);
		nFormat->album		= cdPlayer.GetCDInfo().GetTitle();
	}
	else
	{
		nFormat->track		= trackNumber;
		nFormat->cdTrack	= trackNumber;
		nFormat->discid		= CDDB::DiscIDToString(cddb.ComputeDiscID());
		nFormat->drive		= audiodrive;
		nFormat->outfile	= NIL;
	}

	nFormat->isCDTrack	= True;
	nFormat->origFilename	= String("Audio CD ").Append(String::FromInt(nFormat->drive)).Append(" - Track ");

	if (nFormat->track < 10) nFormat->origFilename.Append("0");

	nFormat->origFilename.Append(String::FromInt(nFormat->track));

	return nFormat;
}

Int BonkEnc::FilterInCDRip::StartDiscRead()
{
	readActive		= True;

	return Success();
}

Int BonkEnc::FilterInCDRip::FinishDiscRead()
{
	readActive		 = False;

	cdPlayer.ClearCDInfo();
	cdPlayerDiscID		= -1;

	cdText.ClearCDInfo();
	cdTextDiscID		= -1;

	cdInfo			= NIL;
	cdInfoDiscID		= -1;

	return Success();
}
