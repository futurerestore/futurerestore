/*!
	@file		AudioToolbox.h
	@framework	AudioToolbox.framework
	@copyright	(c) 2002-2015 by Apple, Inc., all rights reserved.
	@abstract	Umbrella header for AudioToolbox framework.
*/


#ifndef AudioToolbox_AudioToolbox_h
#define AudioToolbox_AudioToolbox_h

#define AUDIO_TOOLBOX_VERSION 1060

#include <Availability.h>
#include <TargetConditionals.h>
#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <AudioToolbox/AUComponent.h>
	#include <AudioToolbox/AUGraph.h>
	#include <AudioToolbox/AudioComponent.h>
	#include <AudioToolbox/AudioConverter.h>
	#include <AudioToolbox/AudioFile.h>
	#include <AudioToolbox/AudioFileStream.h>
	#include <AudioToolbox/AudioFormat.h>
	#include <AudioToolbox/AudioOutputUnit.h>
	#include <AudioToolbox/AudioQueue.h>
	#include <AudioToolbox/AudioServices.h>
	#include <AudioToolbox/AudioUnitParameters.h>
	#include <AudioToolbox/AudioUnitProperties.h>
	#include <AudioToolbox/CAFFile.h>
	#include <AudioToolbox/ExtendedAudioFile.h>
	#include <AudioToolbox/MusicDevice.h>
	#include <AudioToolbox/MusicPlayer.h>

		// OS X only
		#include <AudioToolbox/AudioCodec.h>
		#include <AudioToolbox/AudioFileComponent.h>
		#include <AudioToolbox/AudioUnitUtilities.h>
		#include <AudioToolbox/AUMIDIController.h>
		#include <AudioToolbox/CoreAudioClock.h>

	#ifdef __OBJC2__
		// iOS (all architectures), OS X 64-bit only
		#import <AudioToolbox/AUAudioUnit.h>
		#import <AudioToolbox/AUAudioUnitImplementation.h>
		#import <AudioToolbox/AUParameters.h>
	#endif

#else
	#include <AUComponent.h>
	#include <AUGraph.h>
	#include <AudioComponent.h>
	#include <AudioConverter.h>
	#include <AudioFile.h>
	#include <AudioFileComponent.h>
	#include <AudioFileStream.h>
	#include <AudioFormat.h>
	#include <AudioOutputUnit.h>
	#include <AudioQueue.h>
	#include <AudioServices.h>
	#include <AudioUnitParameters.h>
	#include <AudioUnitProperties.h>
	#include <CAFFile.h>
	#include <ExtendedAudioFile.h>
	#include <MusicDevice.h>
	#include <MusicPlayer.h>

	#include <AudioCodec.h>
	#include <AudioUnitUtilities.h>
	#include <AUMIDIController.h>
	#include <CoreAudioClock.h>
#endif

/*!	@mainpage

	@section section_intro			Introduction

	The AudioUnit framework contains a set of related API's dealing with:
	
	- Audio components, providing various types of plug-in functionality.
	- Audio Units, audio processing plug-ins.
	- Audio codecs, plug-ins which decode and encode compressed audio.
	
	@section section_component		Audio Components
	
	See AudioComponent.h for API's to find and use audio components, as well as information
	on how audio components are packaged and built.
	
	In addition, `<AVFoundation/AVAudioUnitComponent.h>` provides a higher-level interface for
	finding audio unit components.
	
	See @ref AUExtensionPackaging and AUAudioUnitImplementation.h for information on creating
	version 3 audio units.
	
	@section section_audiounit		Audio Units
*/

#include <stdio.h>

CF_ASSUME_NONNULL_BEGIN

#if defined(__cplusplus)
extern "C"
{
#endif

// prints out the internal state of an object to stdio
extern void CAShow (void* inObject) 
											__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_2_0);

// prints out the internal state of an object to the supplied FILE
extern void CAShowFile (void* inObject, FILE* inFile) 
											__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_2_0);
	
// this will return the name of a sound bank from a sound bank file
// the name should be released by the caller
struct FSRef;
extern OSStatus GetNameFromSoundBank (const struct FSRef *inSoundBankRef, CFStringRef __nullable * __nonnull outName)
											__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_2,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);

/*!
    @function		CopyNameFromSoundBank
	 
    @discussion		This will return the name of a sound bank from a DLS or SF2 bank.
					The name should be released by the caller.

    @param			inURL
						The URL for the sound bank.
    @param			outName
						A pointer to a CFStringRef to be created and returned by the function.
    @result			returns noErr if successful.
*/

extern OSStatus
CopyNameFromSoundBank (CFURLRef inURL, CFStringRef __nullable * __nonnull outName)
											__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_7_0);

/*!
    @function		CopyInstrumentInfoFromSoundBank
	 
    @discussion		This will return a CFArray of CFDictionaries, one per instrument found in the DLS or SF2 bank.
					Each dictionary will contain four items accessed via CFStringRef versions of the keys kInstrumentInfoKey_MSB,
 					kInstrumentInfoKey_LSB, kInstrumentInfoKey_Program, and kInstrumentInfoKey_Name.
 						MSB: An NSNumberRef for the most-significant byte of the bank number.  GM melodic banks will return 120 (0x78).
 							 GM percussion banks will return 121 (0x79).  Custom banks will return their literal value.
						LSB: An NSNumberRef for the least-significant byte of the bank number.  All GM banks will return
							 the bank variation number (0-127).
 						Program Number: An NSNumberRef for the program number (0-127) of an instrument within a particular bank.
 						Name: A CFStringRef containing the name of the instrument.
 
					Using these MSB, LSB, and Program values will guarantee that the correct instrument is loaded by the DLS synth
					or Sampler Audio Unit.
					The CFArray should be released by the caller.

    @param			inURL
	 					The URL for the sound bank.
    @param			outInstrumentInfo
						A pointer to a CFArrayRef to be created and returned by the function.
    @result			returns noErr if successful.
*/

extern OSStatus CopyInstrumentInfoFromSoundBank (CFURLRef inURL, CFArrayRef __nullable * __nonnull outInstrumentInfo)
														__OSX_AVAILABLE_STARTING(__MAC_10_8,__IPHONE_7_0);
	
#define kInstrumentInfoKey_Name		"name"
#define kInstrumentInfoKey_MSB		"MSB"
#define kInstrumentInfoKey_LSB		"LSB"
#define kInstrumentInfoKey_Program	"program"

#if defined(__cplusplus)
}
#endif

CF_ASSUME_NONNULL_END

#endif // AudioToolbox_AudioToolbox_h
