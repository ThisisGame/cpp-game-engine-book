/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2020 Audiokinetic Inc.
*******************************************************************************/

// Platform.h
/// \file 
/// Contains platform-specific helpers for the Windows platform.

#pragma once

//#define RELATIVE_PATH L"..\\..\\..\\..\\samples\\IntegrationDemo\\WwiseProject\\GeneratedSoundBanks\\Windows\\"
#define RELATIVE_PATH L"..\\data\\audio\\"

#include <AK/SoundEngine/Platforms/Windows/AkTypes.h>

#define SOUND_BANK_PATH GetSoundBanksPath()
AkOSChar* GetSoundBanksPath();

#define AK_ENABLE_RESOURCE_MONITOR

// Defines for button name strings
#define UG_BUTTON1_NAME  "Enter/A"
#define UG_BUTTON2_NAME  "Escape/B"
#define UG_BUTTON3_NAME  "Q/X"
#define UG_BUTTON4_NAME  "E/Y"
#define UG_BUTTON5_NAME  "Shift/LB"
#define UG_BUTTON6_NAME  "R/RB"
#define UG_BUTTON7_NAME  "F1/START"
#define UG_BUTTON8_NAME  "BACK"
#define UG_BUTTON9_NAME  "Left Thumbstick"
#define UG_BUTTON10_NAME "Right Thumbstick"  
#define UG_BUTTON11_NAME "F2"
#define UG_RIGHT_STICK	 "Right Stick"
#define UG_LEFT_STICK	 "Left Stick"
#define OS_BUTTON_NAME   "Alt"
#define DIRECTIONAL_TYPE "Arrow Keys/Dpad"

#define RESOURCE_BUTTON	UG_BUTTON11_NAME
#define HELP_BUTTON			UG_BUTTON7_NAME
#define ACTION_BUTTON		UG_BUTTON1_NAME
#define BACK_BUTTON			UG_BUTTON2_NAME

#ifndef AK_WINGC
#define INTEGRATIONDEMO_MICROPHONE
#endif

#define AK_PLATFORM_SUPPORTS_MOTION
#define AK_PLATFORM_SUPPORTS_3D_AUDIO

#define __AK_OSCHAR_SNPRINTF _snwprintf

#define CODECTYPE_STANDARD	AKCODECID_ADPCM

#if !defined(INTDEMO_RENDER_D3D11) && !defined(INTDEMO_RENDER_D3D12)
#define INTDEMO_RENDER_D3D11
#endif

#define INTDEMO_DEFERRED_IO
