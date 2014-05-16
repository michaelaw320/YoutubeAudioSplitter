/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Modified on 2000.09.06 by N. Iwakami */

#include "ChunkHelper.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// �w�b�_�`�����N�쐬�̕⏕�֐��i�`�����N���t�@�C������T�u�`�����N���쐬����j
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// TRAC �^
static
int ReadTracChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk )
{
	// �K�v�ȃf�[�^�̓ǂݍ���
	int trackNumber=0;
	fscanf( fp, "%d", &trackNumber );

	// �T�u�`�����N���쐬�E�ǉ�
	CTracChunk subChunk( id, trackNumber );
	theChunkChunk->PutChunk( subChunk );

	return 0;
}

// YEAR �^
static
int ReadYearChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk )
{
	// �K�v�ȃf�[�^�̓ǂݍ���
	int year = 0;
	int month = 0;
	fscanf( fp, "%d %d", &year, &month );

	// �T�u�`�����N���쐬�E�ǉ�
	CYearChunk subChunk( id, year, month );
	theChunkChunk->PutChunk( subChunk );

	return 0;
}

// ENCD �^
static
int ReadEncdChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk )
{
	// �K�v�ȃf�[�^�̓ǂݍ���
	int year=0, month=0, day=0, hour=0, minute=0, timeZone=0;
	fscanf( fp, "%d %d %d %d %d %d", &year, &month, &day, &hour, &minute, &timeZone );

	// �T�u�`�����N���쐬�E�ǉ�
	CEncdChunk subChunk( id, year, month, day, hour, minute, timeZone );
	theChunkChunk->PutChunk( subChunk );

	return 0;
}

// �ėp�^�i������^���܂ށj
static
int ReadRawChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk ){
	// �`�����N�쐬
	CChunk subChunk( id );

	// �K�v�ȃf�[�^�̓ǂݍ���
	char buf;
	while ( fread( &buf, 1, sizeof(char), fp ) ){
		subChunk.PutData( 1, &buf );
	}

	// �T�u�`�����N��ǉ�
	theChunkChunk->PutChunk( subChunk );

	return 0;
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// �`�����N�ǂݍ��݃e�[�u��
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define ReadStringChunk ReadRawChunk

typedef struct {
	char* id;
	int (*reader)( FILE *fp, std::string id, CChunkChunk *theChunkChunk );
} CHUNK_READ_TABLE;

static CHUNK_READ_TABLE primaryReadTable[] = {
	{ (char *) "TRAC", ReadTracChunk },	// �g���b�N�ԍ�
	{ (char *) "YEAR", ReadYearChunk },	// �^�����ꂽ�N
	{ (char *) "ENCD", ReadEncdChunk },	// �G���R�[�h�̓��t
	{ (char *) "GUID", ReadRawChunk },	// Globally Unique Identifier
	{ (char *) "ISRC", ReadRawChunk },	// International Standard Record Code
	{ (char *) "ALBM", ReadStringChunk },	// �A���o���^�C�g��
	{ (char *) "LYRC", ReadStringChunk },	// �̎�
	{ (char *) "WORD", ReadStringChunk },	// �쎌��
	{ (char *) "MUSC", ReadStringChunk },	// ��Ȏ�
	{ (char *) "ARNG", ReadStringChunk },	// �ҋȎ�
	{ (char *) "PROD", ReadStringChunk },	// �v���f���[�T�[
	{ (char *) "REMX", ReadStringChunk },	// ���~�b�N�X
	{ (char *) "CDCT", ReadStringChunk },	// �w����
	{ (char *) "SING", ReadStringChunk },	// �̎�
	{ (char *) "LABL", ReadStringChunk },	// ���[�x��
	{ (char *) "NOTE", ReadStringChunk },	// ���C�i�[�m�[�c
	{ (char *) "PRSN", ReadStringChunk },	// ���t�����o�[
	{ (char *) "BAND", ReadStringChunk },	// �o���h��
	{ NULL },
};

static CHUNK_READ_TABLE secondaryReadTable[] = {
	{ (char *) "NAME", ReadStringChunk },	// �Ȗ�
	{ (char *) "AUTH", ReadStringChunk },	// �A�[�e�B�X�g
	{ (char *) "COMT", ReadStringChunk },	// �R�����g
	{ (char *) "(c) ", ReadStringChunk },	// ���쌠

	{ (char *) "ALBM", ReadStringChunk },	// �A���o���^�C�g��
	{ (char *) "LYRC", ReadStringChunk },	// �̎�
	{ (char *) "WORD", ReadStringChunk },	// �쎌��
	{ (char *) "MUSC", ReadStringChunk },	// ��Ȏ�
	{ (char *) "ARNG", ReadStringChunk },	// �ҋȎ�
	{ (char *) "PROD", ReadStringChunk },	// �v���f���[�T�[
	{ (char *) "REMX", ReadStringChunk },	// ���~�b�N�X
	{ (char *) "CDCT", ReadStringChunk },	// �w����
	{ (char *) "SING", ReadStringChunk },	// �̎�
	{ (char *) "LABL", ReadStringChunk },	// ���[�x��
	{ (char *) "NOTE", ReadStringChunk },	// ���C�i�[�m�[�c
	{ (char *) "PRSN", ReadStringChunk },	// ���t�����o�[
	{ (char *) "BAND", ReadStringChunk },	// �o���h��
	{ NULL },
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// �w�b�_�`�����N�쐬�֐�
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------*/
/* Name:        PutTextChunk()                                                */
/* Description: ������`�����N���쐬���� TWIN �`�����N�ɕt��������            */
/* Return:      �Ȃ�                                                          */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
void PutTextChunk( char* id, char* theData, CChunkChunk* twinChunk ) {
	if ( strlen( theData ) ) {
		CStringChunk theStringChunk( id, theData );
		twinChunk->PutChunk( theStringChunk );
	}
}

/*----------------------------------------------------------------------------*/
/* Name:        PutStandardChunks()                                           */
/* Description: TWIN �`�����N�ɕW���`�����N��t��������                       */
/* Return:      (int) �G���[������΂P�A�Ȃ���΂O                            */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int PutStandardChunks( headerInfo* info, CChunkChunk* twinChunk )
{
	// COMM �`�����N���쐬���� TWIN �`�����N�ɕt��������
	std::string version = twinChunk->GetID();
	CCommChunk theCommChunk( info->channelMode, info->bitRate, info->samplingRate, info->securityLevel );
	twinChunk->PutChunk( theCommChunk );

	// �W��������`�����N�����쐬���� TWIN �`�����N�ɕt��������
	PutTextChunk( (char *) "NAME", info->Name, twinChunk );
	PutTextChunk( (char *) "COMT", info->Comt, twinChunk );
	PutTextChunk( (char *) "AUTH", info->Auth, twinChunk );
	PutTextChunk( (char *) "(c) ", info->Cpyr, twinChunk );
	PutTextChunk( (char *) "FILE", info->File, twinChunk );

	// �f�[�^�T�C�Y�`�����N���쐬���ĕt��������
	if ( info->Dsiz > 0 ) {
		CDsizChunk dsizChunk( "DSIZ", info->Dsiz );
		twinChunk->PutChunk( dsizChunk );
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
/* Name:        PutChunkInfo()                                                */
/* Description: �`�����N�����t�@�C������ǂݍ��� twin �`�����N�ɒǉ�����    */
/* Return:      (int) �G���[������΂P�A�Ȃ���΂O                            */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int PutChunkInfo ( CHUNK_READ_TABLE theTable[], CChunkChunk *twinChunk, int mode, std::string directory )
{
	std::string infoDir[] = { "/", "/SCND/" };

	if ( mode >= 2 ) return 1;

	while ( theTable->id ) {
		/* �t�@�C��������� */
		std::string fileName = theTable->id;
		std::string pathName = directory + infoDir[mode] + fileName;
		
		/* �t�@�C�������`�F�b�N���A�t�@�C��������Ώ�����荞�� */
		FILE *fp;
		if ( (fp=fopen(pathName.c_str(), "rb")) ) {
			(*theTable->reader)( fp, theTable->id, twinChunk );
			fclose ( fp );
		}
		++theTable;
	}

	return 0;
}

/*============================================================================*/
/* Name:        TvqCreateHeaderChunk()                                        */
/* Description: �w�b�_�`�����N���쐬����B                                    */
/* Return:      �w�b�_�`�����N�B�쐬�Ɏ��s�����ꍇ NULL ��߂�                */
/* Access:      external                                                      */
/*============================================================================*/
CChunkChunk* TvqCreateHeaderChunk( headerInfo* setupInfo, std::string directory )
{
	/* Create empty "TWIN" chunk */
	CChunkChunk* twinChunk = new CChunkChunk( setupInfo->ID );

	/* Put standard chunks in the "TWIN" chunk */
	if ( PutStandardChunks( setupInfo, twinChunk ) ) return NULL;

	/* Put extended chunks (primary) in the "TWIN" chunk */
	if ( PutChunkInfo( primaryReadTable, twinChunk, PRIMARY, directory ) ) return NULL;

	/* Put extended chunks (secondary) in the "TWIN" chunk */
	{
		CChunkChunk scndChunk( "SCND" );
		if ( PutChunkInfo( secondaryReadTable, &scndChunk, SECONDARY, directory ) ) return NULL;
		if ( scndChunk.GetSize() ) {
			twinChunk->PutChunk( scndChunk );
		}
	}

	return twinChunk;
}
