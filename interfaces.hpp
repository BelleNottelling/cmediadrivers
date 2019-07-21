/*
Copyright (c) 2006-2008 dogbert <dogber1@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _INTERFACES_HPP_
#define _INTERFACES_HPP_

#include <portcls.h>
#include <stdunk.h>

#define MAXLEN_DMA_BUFFER       0x18000
#define MAX_OUTPUT_STREAMS      1
#define MAX_INPUT_STREAMS       1
#define MAX_AC3_INPUT_STREAMS   1

// PCM Info
#define MIN_CHANNELS            2
#define MAX_CHANNELS_PCM        8
#define MIN_BITS_PER_SAMPLE_PCM 16
#define MAX_BITS_PER_SAMPLE_PCM 32
#define MIN_SAMPLE_RATE         44100
#define MAX_SAMPLE_RATE         96000
#define MAX_SAMPLE_RATE_MULTI   48000

// AC3 Info
#define MAX_CHANNELS_AC3        2
#define MIN_BITS_PER_SAMPLE_AC3 16
#define MAX_BITS_PER_SAMPLE_AC3 16
#define MIN_SAMPLE_RATE_AC3     48000
#define MAX_SAMPLE_RATE_AC3     48000

// WMA-Pro Info
#define MAX_CHANNELS_WMA        2
#define MIN_BITS_PER_SAMPLE_WMA 16
#define MAX_BITS_PER_SAMPLE_WMA 16
#define MIN_SAMPLE_RATE_WMA     44100
#define MAX_SAMPLE_RATE_WMA     48000

#define PCM_OUT_STREAM 0
#define PCM_IN_STREAM  1
#define AC3_OUT_STREAM 2

#define CHAN_LEFT   0
#define CHAN_RIGHT  1
#define CHAN_MASTER (-1)

#define IN_CHANNEL  0
#define OUT_CHANNEL 1

#define UInt32	ULONG
#define UInt16	USHORT
#define UInt8	BYTE
#define Int32   LONG

#define KSSTATE_STOP_AC3  (KSSTATE)5
#define KSSTATE_RUN_AC3   (KSSTATE)6

// Interface ICMITopology
DECLARE_INTERFACE_(ICMITopology,IMiniportTopology)
{
	STDMETHOD_(NTSTATUS,GetDescription)
	(	THIS_
		PPCFILTER_DESCRIPTOR*  OutFilterDescriptor
	)	PURE;
	STDMETHOD_(NTSTATUS,DataRangeIntersection)
	(	THIS_
		ULONG			PinId,
		PKSDATARANGE	DataRange,
		PKSDATARANGE	MatchingDataRange,
		ULONG			OutputBufferLength,
		PVOID			ResultantFormat,
		PULONG			ResultantFormatLength
	)	PURE;
	STDMETHOD_(NTSTATUS,Init)
	(	THIS_
		PUNKNOWN		UnknownAdapter,
		PRESOURCELIST	ResourceList,
		PPORTTOPOLOGY	Port
	)	PURE;
	STDMETHOD_(NTSTATUS,loadMixerSettingsFromMemory)
	(	THIS_
		void
	)	PURE;
	STDMETHOD_(NTSTATUS,storeMixerSettingsToMemory)
	(	THIS_
		void
	)	PURE;
};
typedef ICMITopology *PCMITOPOLOGY;

// Interface IMiniportWaveCMI
DECLARE_INTERFACE_(IMiniportWaveCMI,IUnknown)
{
	STDMETHOD_(void,ServiceWaveISR)
	(	THIS_
		ULONG	channel
	)	PURE;

	STDMETHOD_(void,powerUp)
	(	THIS_
		void
	)	PURE;
	STDMETHOD_(void,powerDown)
	(	THIS_
		void
	)	PURE;
};
typedef IMiniportWaveCMI *PCMIWAVE;

// structure to avoid the fucking COM crapola
typedef struct CMI8738Info {
	int             chipVersion;
	int             maxChannels;
	bool            canAC3HW, canAC3SW;
	bool            canMultiChannel;
	bool            doAC3SW;
	bool            hasDualDAC;
	UInt32*         IOBase;
	UInt32*         MPUBase;
	PCMIWAVE        WaveMiniport;
	PCMITOPOLOGY    TopoMiniport;
	bool            enableSPDIFOut;
	bool            enableSPDIFIn;
	bool            enableSPDIFInMonitor;
	UInt32          formatMask;
	UInt32          regFUNCTRL0;
} CMI8738Info;
typedef CMI8738Info* PCMI8738Info;

typedef struct VolumeTable {
	UInt32          node;
	UInt8           reg;
	UInt8           mask;
	UInt8           shift;
	Int32           max;
	Int32           min;
	Int32           step;
	Int32           dbshift;
} VolumeTable;

// Interface ICMIAdapter
DECLARE_INTERFACE_(ICMIAdapter,IUnknown)
{
	STDMETHOD_(NTSTATUS,init)
	( THIS_
		PRESOURCELIST	ResourceList,
		PDEVICE_OBJECT	DeviceObject
	) PURE;
	STDMETHOD_(PINTERRUPTSYNC,getInterruptSync)
	( THIS_
		void
	) PURE;
	STDMETHOD_(PDEVICE_OBJECT,getDeviceObject)
	( THIS_
		void
	) PURE;
	STDMETHOD_(PCMI8738Info,getCMI8738Info)
	( THIS_
		void
	) PURE;
	STDMETHOD_(UInt8, readUInt8)
	( THIS_
		UInt8 reg
	) PURE;
	STDMETHOD_(void, writeUInt8)
	(   THIS_
		UInt8 reg,
		UInt8 value
	)   PURE;
	STDMETHOD_(void, setUInt8Bit)
	(   THIS_
		UInt8 reg,
		UInt8 flag
	)   PURE;
	STDMETHOD_(void, clearUInt8Bit)
	(   THIS_
		UInt8 reg,
		UInt8 flag
	)   PURE;
	STDMETHOD_(UInt16, readUInt16)
	(   THIS_
		UInt8 reg
	)   PURE;
	STDMETHOD_(void, writeUInt16)
	(   THIS_
		UInt8 reg,
		UInt16 value
	)   PURE;
	STDMETHOD_(UInt32, readUInt32)
	(   THIS_
		UInt8 reg
	)   PURE;
	STDMETHOD_(void, writeUInt32)
	(   THIS_
		UInt8 reg,
		UInt32 value
	)   PURE;
	STDMETHOD_(void, setUInt32Bit)
	(   THIS_
		UInt8 reg,
		UInt32 flag
	)   PURE;
	STDMETHOD_(void, clearUInt32Bit)
	(   THIS_
		UInt8 reg,
		UInt32 flag
	)   PURE;
	STDMETHOD_(UInt8, readMixer)
	(   THIS_
		UInt8 index
	)   PURE;
	STDMETHOD_(void, writeMixer)
	(   THIS_
		UInt8 index,
		UInt8 value
	)   PURE;
	STDMETHOD_(void, setMixerBit)
	(   THIS_
		UInt8 index,
		UInt8 flag
	)   PURE;
	STDMETHOD_(void, clearMixerBit)
	(   THIS_
		UInt8 index,
		UInt8 flag
	)   PURE;
	STDMETHOD_(void,resetMixer)
	(   THIS_
	)   PURE;
	STDMETHOD_(NTSTATUS,activateMPU)
	(   THIS_
		ULONG*          MPUBase
	)   PURE;
	STDMETHOD_(NTSTATUS,loadSBMixerFromMemory)
	(   THIS_
	)   PURE;
};
typedef ICMIAdapter *PCMIADAPTER;

// GUIDs

// {34B338A2-BB40-4ecd-B97A-AEB03F325825}
DEFINE_GUID(IID_ICMIAdapter, 0x34b338a2, 0xbb40, 0x4ecd, 0xb9, 0x7a, 0xae, 0xb0, 0x3f, 0x32, 0x58, 0x25);
// {60BB55CF-C960-4ddf-B7A4-243978B2FE7B}
DEFINE_GUID(IID_ICMITopolgy, 0x60bb55cf, 0xc960, 0x4ddf, 0xb7, 0xa4, 0x24, 0x39, 0x78, 0xb2, 0xfe, 0x7b);
// {E8698236-D854-4da7-BF31-8332C614CE91}
DEFINE_GUID(IID_IMiniportWaveCMI, 0xe8698236, 0xd854, 0x4da7, 0xbf, 0x31, 0x83, 0x32, 0xc6, 0x14, 0xce, 0x91);


// {9DB14E9A-7BE7-480d-A2FA-32932489DE9C}
#define STATIC_PRODUCT_CM8738 \
	0x9db14e9a, 0x7be7, 0x480d, 0xa2, 0xfa, 0x32, 0x93, 0x24, 0x89, 0xde, 0x9c
DEFINE_GUIDSTRUCT("9DB14E9A-7BE7-480d-A2FA-32932489DE9C", PRODUCT_CM8738);
#define PRODUCT_CM8738 DEFINE_GUIDNAMED(PRODUCT_CM8738)

// {9DB14E9A-7BE7-480d-A2FA-32932489DE9D}
#define STATIC_COMPONENT_CM8738 \
	0x9db14e9a, 0x7be7, 0x480d, 0xa2, 0xfa, 0x32, 0x93, 0x24, 0x89, 0xde, 0x9d
DEFINE_GUIDSTRUCT("9DB14E9A-7BE7-480d-A2FA-32932489DE9D", COMPONENT_CM8738);
#define COMPONENT_CM8738 DEFINE_GUIDNAMED(COMPONENT_CM8738)

// {9DB14E9A-7BE7-480d-A2FA-32932489DE9E}
#define STATIC_MANUFACTURER_CM8738 \
	0x9db14e9a, 0x7be7, 0x480d, 0xa2, 0xfa, 0x32, 0x93, 0x24, 0x89, 0xde, 0x9e
DEFINE_GUIDSTRUCT("9DB14E9A-7BE7-480d-A2FA-32932489DE9E", MANUFACTURER_CM8738);
#define MANUFACTURER_CM8738 DEFINE_GUIDNAMED(MANUFACTURER_CM8738)


#define CMIPCI_VERSION   0x00

// topology node names (refer to .inf file)
// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF0}
#define STATIC_CMINAME_IEC_5V \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF0
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF0", _STATIC_CMINAME_IEC_5V);
#define CMINAME_IEC_5V DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_5V)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF1}
#define STATIC_CMINAME_IEC_OUT \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF1
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF1", _STATIC_CMINAME_IEC_OUT);
#define CMINAME_IEC_OUT DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_OUT)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF2}
#define STATIC_CMINAME_IEC_INVERSE \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF2
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF2", _STATIC_CMINAME_IEC_INVERSE);
#define CMINAME_IEC_INVERSE DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_INVERSE)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF3}
#define STATIC_CMINAME_IEC_MONITOR \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF3
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF3", _STATIC_CMINAME_IEC_MONITOR);
#define CMINAME_IEC_MONITOR DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_MONITOR)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF4}
#define STATIC_CMINAME_DAC \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF4
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF4", _STATIC_CMINAME_DAC);
#define CMINAME_DAC DEFINE_GUIDNAMED(_STATIC_CMINAME_DAC)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF5}
#define STATIC_CMINAME_IEC_SELECT \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF5
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF5", _STATIC_CMINAME_IEC_SELECT);
#define CMINAME_IEC_SELECT DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_SELECT)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF6}
#define STATIC_CMINAME_XCHG_FB \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF6
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF6", _STATIC_CMINAME_XCHG_FB);
#define CMINAME_XCHG_FB DEFINE_GUIDNAMED(_STATIC_CMINAME_XCHG_FB)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF7}
#define STATIC_CMINAME_BASS2LINE \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF7
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF7", _STATIC_CMINAME_BASS2LINE);
#define CMINAME_BASS2LINE DEFINE_GUIDNAMED(_STATIC_CMINAME_BASS2LINE)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF8}
#define STATIC_CMINAME_CENTER2LINE \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF8
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF8", _STATIC_CMINAME_CENTER2LINE);
#define CMINAME_CENTER2LINE DEFINE_GUIDNAMED(_STATIC_CMINAME_CENTER2LINE)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF9}
#define STATIC_CMINAME_IEC_COPYRIGHT \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xF9
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DF9", _STATIC_CMINAME_IEC_COPYRIGHT);
#define CMINAME_IEC_COPYRIGHT DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_COPYRIGHT)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFA}
#define STATIC_CMINAME_IEC_POLVALID \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xFA
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFA", _STATIC_CMINAME_IEC_POLVALID);
#define CMINAME_IEC_POLVALID DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_POLVALID)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFB}
#define STATIC_CMINAME_IEC_LOOP \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xFB
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFB", _STATIC_CMINAME_IEC_LOOP);
#define CMINAME_IEC_LOOP DEFINE_GUIDNAMED(_STATIC_CMINAME_IEC_LOOP)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFC}
#define STATIC_CMINAME_REAR2LINE \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xFC
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFC", _STATIC_CMINAME_REAR2LINE);
#define CMINAME_REAR2LINE DEFINE_GUIDNAMED(_STATIC_CMINAME_REAR2LINE)

// {2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFD}
#define STATIC_CMINAME_CENTER2MIC \
    0x2B81CDBB, 0xEE6C, 0x4ECC, 0x8A, 0xA5, 0x9A, 0x18, 0x8B, 0x02, 0x3D, 0xFD
DEFINE_GUIDSTRUCT("2B81CDBB-EE6C-4ECC-8AA5-9A188B023DFD", _STATIC_CMINAME_CENTER2MIC);
#define CMINAME_CENTER2MIC DEFINE_GUIDNAMED(_STATIC_CMINAME_CENTER2MIC)

//topology pins
enum
{
	PIN_WAVEOUT_SOURCE = 0,
	PIN_SPDIFIN_SOURCE,
	PIN_MIC_SOURCE,
	PIN_CD_SOURCE,
	PIN_LINEIN_SOURCE,
	PIN_AUX_SOURCE,
	PIN_DAC_SOURCE,
	PIN_LINEOUT_DEST,
	PIN_WAVEIN_DEST,
	PIN_SPDIF_AC3_SOURCE,
	PIN_SPDIF_AC3_DEST,

	PIN_INVALID
};

//topology nodes
enum
{
	KSNODE_TOPO_WAVEOUT_VOLUME = 0,
	KSNODE_TOPO_WAVEOUT_MUTE,
	KSNODE_TOPO_MICOUT_VOLUME,
	KSNODE_TOPO_LINEOUT_MIX,
	KSNODE_TOPO_LINEOUT_VOLUME,
	KSNODE_TOPO_WAVEIN_SUM,
	KSNODE_TOPO_CD_VOLUME,
	KSNODE_TOPO_LINEIN_VOLUME,
	KSNODE_TOPO_AUX_VOLUME,
	KSNODE_TOPO_MICIN_VOLUME,
	KSNODE_TOPO_MICIN_LOUDNESS,
	KSNODE_TOPO_MICOUT_LOUDNESS,
	KSNODE_TOPO_CD_MUTE,
	KSNODE_TOPO_LINEIN_MUTE,
	KSNODE_TOPO_MICOUT_MUTE,
	KSNODE_TOPO_AUX_MUTE,
	KSNODE_TOPO_LINEIN_MUTE_IN,
	KSNODE_TOPO_MIC_MUTE_IN,
	KSNODE_TOPO_AUX_MUTE_IN,
	KSNODE_TOPO_CD_MUTE_IN,
	KSNODE_TOPO_WAVEOUT_MUTE_IN,
	KSNODE_TOPO_IEC_5V,
	KSNODE_TOPO_IEC_OUT,
	KSNODE_TOPO_IEC_INVERSE,
	KSNODE_TOPO_IEC_MONITOR,
	KSNODE_TOPO_IEC_SELECT,
	KSNODE_TOPO_SPDIF_AC3_MUTE,
	KSNODE_TOPO_SPDIF_AC3_MUX,
	KSNODE_TOPO_XCHG_FB,
	KSNODE_TOPO_BASS2LINE,
	KSNODE_TOPO_CENTER2LINE,
	KSNODE_TOPO_IEC_COPYRIGHT,
	KSNODE_TOPO_IEC_POLVALID,
	KSNODE_TOPO_IEC_LOOP,
	KSNODE_TOPO_REAR2LINE,
	KSNODE_TOPO_CENTER2MIC,
	KSNODE_TOPO_MASTER_MUTE_DUMMY,

	KSNODE_TOPO_INVALID
};

//wave miniport pins
enum
{
	PIN_WAVE_CAPTURE_SINK = 0,
	PIN_WAVE_CAPTURE_SOURCE,
	PIN_WAVE_RENDER_SINK,
	PIN_WAVE_RENDER_SOURCE,
	PIN_WAVE_AC3_RENDER_SINK,
	PIN_WAVE_AC3_RENDER_SOURCE,

	PIN_WAVE_INVALID
};

//wave miniport nodes
enum
{
	KSNODE_WAVE_ADC = 0,
	KSNODE_WAVE_VOLUME1,
	KSNODE_WAVE_3D_EFFECTS,
	KSNODE_WAVE_SUPERMIX,
	KSNODE_WAVE_VOLUME2,
	KSNODE_WAVE_SRC,
	KSNODE_WAVE_SUM,
	KSNODE_WAVE_DAC,
	KSNODE_WAVE_SPDIF,

	KSNODE_WAVE_INVALID
};

#endif //_INTERFACES_HPP_