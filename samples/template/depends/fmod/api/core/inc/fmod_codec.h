/* ======================================================================================== */
/* FMOD Core API - Codec development header file.                                           */
/* Copyright (c), Firelight Technologies Pty, Ltd. 2004-2021.                               */
/*                                                                                          */
/* Use this header if you are wanting to develop your own file format plugin to use with    */
/* FMOD's codec system.  With this header you can make your own fileformat plugin that FMOD */
/* can register and use.  See the documentation and examples on how to make a working       */
/* plugin.                                                                                  */
/*                                                                                          */
/* For more detail visit:                                                                   */
/* https://fmod.com/resources/documentation-api?version=2.0&page=core-api.html              */
/* ======================================================================================== */
#ifndef _FMOD_CODEC_H
#define _FMOD_CODEC_H

/*
    Codec types
*/
typedef struct FMOD_CODEC_STATE      FMOD_CODEC_STATE;
typedef struct FMOD_CODEC_WAVEFORMAT FMOD_CODEC_WAVEFORMAT;

/*
    Codec constants
*/
#define FMOD_CODEC_WAVEFORMAT_VERSION 3

/*
    Codec callbacks
*/
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_OPEN_CALLBACK)         (FMOD_CODEC_STATE *codec_state, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO *userexinfo);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_CLOSE_CALLBACK)        (FMOD_CODEC_STATE *codec_state);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_READ_CALLBACK)         (FMOD_CODEC_STATE *codec_state, void *buffer, unsigned int samples_in, unsigned int *samples_out);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETLENGTH_CALLBACK)    (FMOD_CODEC_STATE *codec_state, unsigned int *length, FMOD_TIMEUNIT lengthtype);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_SETPOSITION_CALLBACK)  (FMOD_CODEC_STATE *codec_state, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETPOSITION_CALLBACK)  (FMOD_CODEC_STATE *codec_state, unsigned int *position, FMOD_TIMEUNIT postype);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_SOUNDCREATE_CALLBACK)  (FMOD_CODEC_STATE *codec_state, int subsound, FMOD_SOUND *sound);
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_GETWAVEFORMAT_CALLBACK)(FMOD_CODEC_STATE *codec_state, int index, FMOD_CODEC_WAVEFORMAT *waveformat);

/*
    Codec functions
*/
typedef FMOD_RESULT (F_CALLBACK *FMOD_CODEC_METADATA_FUNC)         (FMOD_CODEC_STATE *codec_state, FMOD_TAGTYPE tagtype, char *name, void *data, unsigned int datalen, FMOD_TAGDATATYPE datatype, int unique);

/*
    Codec structures
*/
typedef struct FMOD_CODEC_DESCRIPTION
{
    const char                       *name;
    unsigned int                      version;
    int                               defaultasstream;
    FMOD_TIMEUNIT                     timeunits;
    FMOD_CODEC_OPEN_CALLBACK          open;
    FMOD_CODEC_CLOSE_CALLBACK         close;
    FMOD_CODEC_READ_CALLBACK          read;
    FMOD_CODEC_GETLENGTH_CALLBACK     getlength;
    FMOD_CODEC_SETPOSITION_CALLBACK   setposition;
    FMOD_CODEC_GETPOSITION_CALLBACK   getposition;
    FMOD_CODEC_SOUNDCREATE_CALLBACK   soundcreate;
    FMOD_CODEC_GETWAVEFORMAT_CALLBACK getwaveformat;
} FMOD_CODEC_DESCRIPTION;

struct FMOD_CODEC_WAVEFORMAT
{
    const char*        name;
    FMOD_SOUND_FORMAT  format;
    int                channels;
    int                frequency;
    unsigned int       lengthbytes;
    unsigned int       lengthpcm;
    unsigned int       pcmblocksize;
    int                loopstart;
    int                loopend;
    FMOD_MODE          mode;
    FMOD_CHANNELMASK   channelmask;
    FMOD_CHANNELORDER  channelorder;
    float              peakvolume;
};

struct FMOD_CODEC_STATE
{
    int                          numsubsounds;
    FMOD_CODEC_WAVEFORMAT       *waveformat;
    void                        *plugindata;

    void                        *filehandle;
    unsigned int                 filesize;
    FMOD_FILE_READ_CALLBACK      fileread;
    FMOD_FILE_SEEK_CALLBACK      fileseek;
    FMOD_CODEC_METADATA_FUNC     metadata;

    int                          waveformatversion;
};

#endif


