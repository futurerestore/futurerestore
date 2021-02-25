/*==================================================================================================
     File:       CoreAudio/AudioHardwarePlugIn.h

     Contains:   API for the CFPlugIn that implements an audio driver for the HAL
                 from user space.

     Copyright:  (c) 1985-2010 by Apple, Inc., all rights reserved.

     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:

                     http://developer.apple.com/bugreporter/

==================================================================================================*/
#if !defined(CoreAudio_AudioHardwarePlugIn_h)
#define CoreAudio_AudioHardwarePlugIn_h

//==================================================================================================
#pragma mark    Overview

/*!
    @header AudioHardwarePlugIn
    This is the CFPlugIn interface presented by a HAL plug-in. The HAL will create only one instance
    of each interface. This instance is responsible for providing all required services on behalf of
    as many devices of the kind it implements.

    The Initialize method is called to allow the plug-in to set itself up. At this time any devices
    of its kind and their streams can be presented to the system using
    AudioHardwareDevicesCreated() and AudioHardwareStreamsCreated(). The plug-in is also responsible
    for managing its own notifications, and may install any CFRunLoopSources it needs using
    AudioHardwareAddRunLoopSource() at this time as well.

    Teardown() is called when the HAL is unloading itself and the plug-in should dispose of any
    devices and streams it has created using AudioHardwareDevicesDied() and
    AudioHardareStreamsDied().

    The rest of the methods in this interface correspond to the semantics of their similarly named
    counterparts in <CoreAudio/AudioHardware.h>. The HAL basically passes these calls directly to
    the plug-in in this fashion.

    Plug-ins do not have to manage device or stream property listener procs. Instead, a plug-in can
    call AudioHardwareDevicePropertyChanged() or AudioHardwareStreamPropertyChanged() and the HAL
    will take care of calling all the appropriate listeners.
    
    Note that only version 4 or later plug-ins will be loaded into 64 bit processes.
    
    Starting in 10.8, this kind of HAL plug-in is deprecated.
*/

//==================================================================================================
#pragma mark    Includes

#include <CoreAudio/AudioHardware.h>
#include <CoreFoundation/CoreFoundation.h>
#if COREFOUNDATION_CFPLUGINCOM_SEPARATE
    #include <CoreFoundation/CFPlugInCOM.h>
#endif

//==================================================================================================

CF_ASSUME_NONNULL_BEGIN

#if defined(__cplusplus)
extern "C"
{
#endif

//==================================================================================================
#pragma mark    Types

/*!
    @typedef        AudioHardwarePlugInInterface
    @abstract       Forward declaration of AudioHardwarePlugInInterface.
*/
typedef struct AudioHardwarePlugInInterface AudioHardwarePlugInInterface;

/*!
    @typedef        AudioHardwarePlugInRef
    @abstract       A reference to an AudioHardwarePlugInInterface.
*/
typedef AudioHardwarePlugInInterface* __nullable *  AudioHardwarePlugInRef;

//==================================================================================================
#pragma mark    Constants

/*!
    @defined        kAudioHardwarePlugInTypeID
    @discussion     This is the UUID of the plug-in type (F8BB1C28-BAE8-11D6-9C31-00039315CD46).
*/
#define kAudioHardwarePlugInTypeID                                                                  \
            CFUUIDGetConstantUUIDWithBytes( NULL, 0xF8, 0xBB, 0x1C, 0x28, 0xBA, 0xE8, 0x11, 0xD6,   \
                                            0x9C, 0x31, 0x00, 0x03, 0x93, 0x15, 0xCD, 0x46)

/*!
    @defined        kAudioHardwarePlugInInterfaceID
    @discussion     This is the UUID of version 1 of the plug-in interface
                    (FAFCAFC3-BAE8-11D6-B4A8-00039315CD46).
*/
#define kAudioHardwarePlugInInterfaceID                                                             \
            CFUUIDGetConstantUUIDWithBytes( NULL, 0xFA, 0xFC, 0xAF, 0xC3, 0xBA, 0xE8, 0x11, 0xD6,   \
                                            0xB4, 0xA8, 0x00, 0x03, 0x93, 0x15, 0xCD, 0x46)

/*!
    @defined        kAudioHardwarePlugInInterface2ID
    @discussion     This is the UUID of version 2 of the plug-in interface
                    (5D80CB6C-484F-11D7-8571-000A95771282).
*/
#define kAudioHardwarePlugInInterface2ID                                                            \
            CFUUIDGetConstantUUIDWithBytes( NULL, 0x5D, 0x80, 0xCB, 0x6C, 0x48, 0x4F, 0x11, 0xD7,   \
                                            0x85, 0x71, 0x00, 0x0A, 0x95, 0x77, 0x12, 0x82)

/*!
    @defined        kAudioHardwarePlugInInterface3ID
    @discussion     This is the UUID of version 3 of the plug-in interface
                    (38D78A18-77A5-11D8-B8B8-000A9588787E).
*/
#define kAudioHardwarePlugInInterface3ID                                                            \
            CFUUIDGetConstantUUIDWithBytes( NULL, 0x38, 0xD7, 0x8A, 0x18, 0x77, 0xA5, 0x11, 0xD8,   \
                                            0xB8, 0xB8, 0x00, 0x0A, 0x95, 0x88, 0x78, 0x7E)

/*!
    @defined        kAudioHardwarePlugInInterface4ID
    @discussion     This is the UUID of version 4 of the plug-in interface
                    (E96C3E92-E745-4CB7-BA91-B33C68F2F026).
*/
#define kAudioHardwarePlugInInterface4ID                                                            \
            CFUUIDGetConstantUUIDWithBytes( NULL, 0xE9, 0x6C, 0x3E, 0x92, 0xE7, 0x45, 0x4C, 0xB7,   \
                                            0xBA, 0x91, 0xB3, 0x3C, 0x68, 0xF2, 0xF0, 0x26)

/*!
    @defined        kAudioHardwarePlugInInterface5ID
    @discussion     This is the UUID of version 5 of the plug-in interface
                    (47023157-0C55-44AA-8996-B81F69AE876E).
*/
#define kAudioHardwarePlugInInterface5ID                                                            \
            CFUUIDGetConstantUUIDWithBytes( NULL, 0x47, 0x02, 0x31, 0x57, 0x0C, 0x55, 0x44, 0xAA,   \
                                            0x89, 0x96, 0xB8, 0x1F, 0x69, 0xAE, 0x87, 0x6E)

//==================================================================================================
#pragma mark    Plug-In Interface

/*!
    @interface      AudioHardwarePlugInInterface
    @abstract       The CFPlugIn interface to a HAL plug-in.
    @discussion     In CFPlugIn terms, AudioHardwarePlugInInterface is a subclass of IUnknown.
*/
struct  AudioHardwarePlugInInterface
{

/*!
    @var            _reserved
    @abstract       A standard part of the IUnknown interface.
*/
    void* __nullable    _reserved;

/*!
    @method         QueryInterface
    @abstract       The IUnknown method for finding an interface on a CFPlugIn type.
    @param          inSelf
                        The CFPlugIn type to query.
    @param          inUUID
                        The UUID of the interface to find.
    @param          outInterface
                        The returned interface or NULL if none was found.
    @result         An error code indicating success of failure.
*/
    HRESULT
    (STDMETHODCALLTYPE *QueryInterface)( void* __nullable                inSelf,
                                                    REFIID                          inUUID,
                                                    LPVOID __nullable * __nullable  outInterface);
/*!
    @method         AddRef
    @abstract       The IUnknown method for retaining a reference to a CFPlugIn type.
    @param          inSelf
                        The CFPlugIn type to retain.
    @result         The resulting reference count after the new reference is added.
*/
    ULONG
    (STDMETHODCALLTYPE *AddRef)(void* __nullable inSelf);

/*!
    @method         Release
    @abstract       The IUnknown method for releasing a reference to a CFPlugIn type.
    @param          inSelf
                        The CFPlugIn type to release.
    @result         The resulting reference count after the reference has been removed.
*/
    ULONG
    (STDMETHODCALLTYPE *Release)(void* __nullable    inSelf);

/*!
    @method         Initialize
    @abstract       This method is called to initialize the instance of the plug-in.
    @discussion     As part of initialization, the plug-in should publish all the AudioObjects
                    it knows about at the time.
    @param          inSelf
                        The instance to initialize.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*Initialize)(AudioHardwarePlugInRef __nonnull   inSelf);

/*!
    @method         Teardown
    @abstract       This method is called to tear down the instance of the plug-in.
    @param          inSelf
                        The instance to tear down.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*Teardown)(AudioHardwarePlugInRef __nonnull inSelf);

/*!
    @method         DeviceAddIOProc
    @abstract       Registers the given AudioDeviceIOProc with the AudioDevice.
    @discussion     A client may have multiple IOProcs for a given device, but the device is
                    free to only accept as many as it can handle. Note that it is not
                    recommended for clients to have more than a single IOProc registered at a
                    time as this can be wasteful of system resources. Rather, it is recommended
                    that the client do any necessary mixing itself so that only one IOProc is
                    necessary.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to register the IOProc with.
    @param          inProc
                        The AudioDeviceIOProc to register.
    @param          inClientData
                        A pointer to client data that is passed back to the IOProc when it is
                        called.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceAddIOProc)( AudioHardwarePlugInRef __nonnull    inSelf,
                        AudioDeviceID                       inDevice,
                        AudioDeviceIOProc                   inProc,
                        void* __nullable                    inClientData);

/*!
    @method         DeviceRemoveIOProc
    @abstract       Unregisters the given AudioDeviceIOProc from the AudioDevice.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to unregister the IOProc from.
    @param          inProc
                        The AudioDeviceIOProc to unregister.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceRemoveIOProc)(  AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioDeviceID                       inDevice,
                            AudioDeviceIOProc                   inProc);

/*!
    @method         DeviceStart
    @abstract       Starts IO for the given AudioDeviceIOProcID.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to start the IOProc on.
    @param          inProc
                        The AudioDeviceIOProcID to start. Note that this can be NULL, which starts
                        the hardware regardless of whether or not there are any IOProcs
                        registered. This is necessary if any of the AudioDevice's timing
                        services are to be used. A balancing call to AudioDeviceStop with a NULL
                        IOProc is required to stop the hardware.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceStart)( AudioHardwarePlugInRef __nonnull    inSelf,
                    AudioDeviceID                       inDevice,
                    AudioDeviceIOProcID __nullable      inProc);

/*!
    @method         DeviceStop
    @abstract       Stops IO for the given AudioDeviceIOProcID.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to stop the IOProc on.
    @param          inProc
                        The AudioDeviceIOProcID to stop.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceStop)(  AudioHardwarePlugInRef __nonnull    inSelf,
                    AudioDeviceID                       inDevice,
                    AudioDeviceIOProcID __nullable      inProc);

/*!
    @method         DeviceRead
    @abstract       Read some data from an AudioDevice starting at the given time.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to read from.
    @param          inStartTime
                        An AudioTimeStamp indicating the time from which to read the data. In
                        general, the valid range of time (in frames) is from the current time
                        minus the maximum IO buffer size to the current time minus the safety
                        offset.
    @param          outData
                        An AudioBufferList that must be the same size and shape as that returned
                        by kAudioDevicePropertyStreamConfiguration. Further, the AudioBufferList
                        must have been previously registered with the device via
                        kAudioDevicePropertyRegisterBufferList. On exit, the mDataSize fields
                        will be updated with the amount of data read.
    @result         An OSStatus indicating success or failure.
                    kAudioHardwareUnsupportedOperationError will be returned if the AudioDevice
                    does not support direct reading.
*/
    OSStatus
    (*DeviceRead)(  AudioHardwarePlugInRef __nonnull    inSelf,
                    AudioDeviceID                       inDevice,
                    const AudioTimeStamp*               inStartTime,
                    AudioBufferList*                    outData);

/*!
    @method         DeviceGetCurrentTime
    @abstract       Retrieves the current time from an AudioDevice. Note that the device has to
                    be running.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to from which to get the time.
    @param          outTime
                        An AudioTimeStamp into which the current time is put.
    @result         An OSStatus indicating success or failure. kAudioHardwareNotRunningError
                    will be returned if the AudioDevice isn't running.
*/
    OSStatus
    (*DeviceGetCurrentTime)(    AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioDeviceID                       inDevice,
                                AudioTimeStamp*                     outTime);

/*!
    @method         DeviceTranslateTime
    @abstract       Translates the time in the AudioDevice's time base from one representation
                    to another. Note that the device has to be running
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice whose time base governs the translation.
    @param          inTime
                        An AudioTimeStamp containing the time to be translated.
    @param          outTime
                        An AudioTimeStamp into which the translated time is put. On entry, the
                        mFlags field specifies which representations to translate the input time
                        into. Because not every device supports all time representations, on
                        exit, the mFlags field will indicate which translations were actually
                        done.
    @result         An OSStatus indicating success or failure. kAudioHardwareNotRunningError
                    will be returned if the AudioDevice isn't running.
*/
    OSStatus
    (*DeviceTranslateTime)( AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioDeviceID                       inDevice,
                            const AudioTimeStamp*               inTime,
                            AudioTimeStamp*                     outTime);

/*!
    @method         DeviceGetPropertyInfo
    @abstract       Retrieve information about the given property of an AudioDevice.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice to query.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          outSize
                        A pointer to a UInt32 that receives the size of the property data in
                        bytes on exit. This can be NULL if the size information is not being
                        requested.
    @param          outWritable
                        A pointer to a Boolean that receives indication of whether or not the
                        given property can be set. This can be NULL if the writability is not
                        being requested.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceGetPropertyInfo)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioDeviceID                       inDevice,
                                UInt32                              inChannel,
                                Boolean                             isInput,
                                AudioDevicePropertyID               inPropertyID,
                                UInt32* __nullable                  outSize,
                                Boolean* __nullable                 outWritable);

/*!
    @method         DeviceGetProperty
    @abstract       Queries an the AudioDevice object to get the data of the given property and
                    places it in the provided buffer.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice to query.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          ioPropertyDataSize
                        A UInt32 which on entry indicates the size of the buffer pointed to by
                        outData and on exit indicates how much of the buffer was used.
    @param          outPropertyData
                        The buffer into which the object will put the data for the given
                        property.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceGetProperty)(   AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioDeviceID                       inDevice,
                            UInt32                              inChannel,
                            Boolean                             isInput,
                            AudioDevicePropertyID               inPropertyID,
                            UInt32*                             ioPropertyDataSize,
                            void*                               outPropertyData);

/*!
    @method         DeviceSetProperty
    @abstract       Tells the AudioDevice object to change the value of the given property using
                    the provided data.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to change.
    @param          inWhen
                        A pointer to an AudioTimeStamp that says when to change the property's
                        value relative to the device's time base. NULL means execute the change
                        immediately.
    @param          inChannel
                        The channel of the property to change where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice to change.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to change.
    @param          inPropertyDataSize
                        A UInt32 indicating the size of the buffer pointed to by inData.
    @param          inPropertyData
                        The buffer containing the data to be used to change the property's
                        value.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceSetProperty)(   AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioDeviceID                       inDevice,
                            const AudioTimeStamp* __nullable    inWhen,
                            UInt32                              inChannel,
                            Boolean                             isInput,
                            AudioDevicePropertyID               inPropertyID,
                            UInt32                              inPropertyDataSize,
                            const void*                         inPropertyData);

/*!
    @method         StreamGetPropertyInfo
    @abstract       Retrieve information about the given property of an AudioStream.
    @param          inSelf
                        The plug-in instance that owns the AudioStream.
    @param          inStream
                        The AudioStream to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          outSize
                        A pointer to a UInt32 that receives the size of the property data in
                        bytes on exit. This can be NULL if the size information is not being
                        requested.
    @param          outWritable
                        A pointer to a Boolean that receives indication of whether or not the
                        given property can be set. This can be NULL if the writability is not
                        being requested.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*StreamGetPropertyInfo)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioStreamID                       inStream,
                                UInt32                              inChannel,
                                AudioDevicePropertyID               inPropertyID,
                                UInt32*                             outSize,
                                Boolean*                            outWritable);

/*!
    @method         StreamGetProperty
    @abstract       Queries an the AudioStream object to get the data of the given property and
                    places it in the provided buffer.
    @param          inSelf
                        The plug-in instance that owns the AudioStream.
    @param          inStream
                        The AudioStream to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          ioPropertyDataSize
                        A UInt32 which on entry indicates the size of the buffer pointed to by
                        outData and on exit indicates how much of the buffer was used.
    @param          outPropertyData
                        The buffer into which the object will put the data for the given
                        property.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*StreamGetProperty)(   AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioStreamID                       inStream,
                            UInt32                              inChannel,
                            AudioDevicePropertyID               inPropertyID,
                            UInt32*                             ioPropertyDataSize,
                            void*                               outPropertyData);

/*!
    @method         StreamSetProperty
    @abstract       Tells the AudioStream object to change the value of the given property using
                    the provided data.
    @param          inSelf
                        The plug-in instance that owns the AudioStream.
    @param          inStream
                        The AudioStream to change.
    @param          inWhen
                        A pointer to an AudioTimeStamp that says when to change the property's
                        value relative to the device's time base. NULL means execute the change
                        immediately.
    @param          inChannel
                        The channel of the property to change where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to change.
    @param          inPropertyDataSize
                        A UInt32 indicating the size of the buffer pointed to by inData.
    @param          inPropertyData
                        The buffer containing the data to be used to change the property's
                        value.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*StreamSetProperty)(   AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioStreamID                       inStream,
                            const AudioTimeStamp* __nullable    inWhen,
                            UInt32                              inChannel,
                            AudioDevicePropertyID               inPropertyID,
                            UInt32                              inPropertyDataSize,
                            const void*                         inPropertyData);

/*!
    @method         DeviceStartAtTime
    @abstract       Starts IO for the given AudioDeviceIOProcID and aligns the IO cycle of the
                    AudioDevice with the given time.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to start the IOProc on.
    @param          inProc
                        The AudioDeviceIOProcID to start. Note that this can be NULL, which starts
                        the hardware regardless of whether or not there are any IOProcs
                        registered.
    @param          ioRequestedStartTime
                        A pointer to an AudioTimeStamp that, on entry, is the requested time to
                        start the IOProc. On exit, it will be the actual time the IOProc will
                        start.
    @param          inFlags
                        A UInt32 containing flags that modify how this function behaves.
    @result         An OSStatus indicating success or failure.
                    kAudioHardwareUnsupportedOperationError will be returned if the AudioDevice
                    does not support starting at a specific time and inProc and
                    ioRequestedStartTime are not NULL.
*/
    OSStatus
    (*DeviceStartAtTime)(   AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioDeviceID                       inDevice,
                            AudioDeviceIOProcID                 inProc,
                            AudioTimeStamp*                     ioRequestedStartTime,
                            UInt32                              inFlags);

/*!
    @method         DeviceGetNearestStartTime
    @abstract       Query an AudioDevice to get a time equal to or later than the given time
                    that is the best time to start IO.
    @discussion     The time that is returned is dictated by the constraints of the device and
                    the system. For instance, the driver of a device that provides both audio
                    and video data may only allow start times that coincide with the edge of a
                    video frame. Also, if the device already has one or more active IOProcs, the
                    start time will be shifted to the beginning of the next IO cycle so as not
                    to cause discontinuities in the existing IOProcs. Another reason the start
                    time may shift is to allow for aligning the buffer accesses in an optimal
                    fashion. Note that the device must be running to use this function.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to query.
    @param          ioRequestedStartTime
                        A pointer to an AudioTimeStamp that, on entry, is the requested start
                        time. On exit, it will have the a time equal to or later than the
                        requested time, as dictated by the device's constraints.
    @param          inFlags
                        A UInt32 containing flags that modify how this function behaves.
    @result         An OSStatus indicating success or failure. kAudioHardwareNotRunningError
                    will be returned if the AudioDevice isn't running.
                    kAudioHardwareUnsupportedOperationError will be returned if the AudioDevice
                    does not support starting at a specific time.
*/
    OSStatus
    (*DeviceGetNearestStartTime)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                    AudioDeviceID                       inDevice,
                                    AudioTimeStamp*                     ioRequestedStartTime,
                                    UInt32                              inFlags);

/*!
    @method         InitializeWithObjectID
    @abstract       This method is called to initialize the instance of the plug-in.
    @discussion     As part of initialization, the plug-in should publish all the AudioObjects
                    it knows about at the time. This method supersedes the Initialize method for
                    version 3 plug-ins so that an AudioObjectID can be associated with the plug-in
                    right from the start.
    @param          inSelf
                        The instance to initialize.
    @param          inObjectID
                        The AudioObjectID the plug-in is to use to identify itself.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*InitializeWithObjectID)(  AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioObjectID                       inObjectID);

/*!
    @method         ObjectShow
    @abstract       Prints to standard out a textural description of the AudioObject.
    @param          inSelf
                        The plug-in instance that owns the AudioObject.
    @param          inObjectID
                        The AudioObject to show.
*/
    void
    (*ObjectShow)(  AudioHardwarePlugInRef __nonnull    inSelf,
                    AudioObjectID                       inObjectID);

/*!
    @method         ObjectHasProperty
    @abstract       Queries an AudioObject about whether or not it has the given property.
    @param          inSelf
                        The plug-in instance that owns the AudioObject.
    @param          inObjectID
                        The AudioObject to query.
    @param          inAddress
                        An AudioObjectPropertyAddress indicating which property is being
                        queried.
    @result         A Boolean indicating whether or not the AudioObject has the given property.
*/
    Boolean
    (*ObjectHasProperty)(   AudioHardwarePlugInRef __nonnull    inSelf,
                            AudioObjectID                       inObjectID,
                            const AudioObjectPropertyAddress*   inAddress);

/*!
    @method         ObjectIsPropertySettable
    @abstract       Queries an AudioObject about whether or not the given property can be set
                    using AudioObjectSetPropertyData.
    @param          inSelf
                        The plug-in instance that owns the AudioObject.
    @param          inObjectID
                        The AudioObject to query.
    @param          inAddress
                        An AudioObjectPropertyAddress indicating which property is being
                        queried.
    @param          outIsSettable
                        A Boolean indicating whether or not the property can be set.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*ObjectIsPropertySettable)(    AudioHardwarePlugInRef __nonnull    inSelf,
                                    AudioObjectID                       inObjectID,
                                    const AudioObjectPropertyAddress*   inAddress,
                                    Boolean*                            outIsSettable);

/*!
    @method         ObjectGetPropertyDataSize
    @abstract       Queries an AudioObject to find the size of the data for the given property.
    @param          inSelf
                        The plug-in instance that owns the AudioObject.
    @param          inObjectID
                        The AudioObject to query.
    @param          inAddress
                        An AudioObjectPropertyAddress indicating which property is being
                        queried.
    @param          inQualifierDataSize
                        A UInt32 indicating the size of the buffer pointed to by
                        inQualifierData. Note that not all properties require qualification, in
                        which case this value will be 0.
    @param          inQualifierData,
                        A buffer of data to be used in determining the data of the property
                        being queried. Note that not all properties require qualification, in
                        which case this value will be NULL.
    @param          outDataSize
                        A UInt32 indicating how many bytes the data for the given property
                        occupies.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*ObjectGetPropertyDataSize)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                    AudioObjectID                       inObjectID,
                                    const AudioObjectPropertyAddress*   inAddress,
                                    UInt32                              inQualifierDataSize,
                                    const void* __nullable              inQualifierData,
                                    UInt32* __nonnull                   outDataSize);

/*!
    @method         ObjectGetPropertyData
    @abstract       Queries an AudioObject to get the data of the given property and places it
                    in the provided buffer.
    @param          inSelf
                        The plug-in instance that owns the AudioObject.
    @param          inObjectID
                        The AudioObject to query.
    @param          inAddress
                        An AudioObjectPropertyAddress indicating which property is being
                        queried.
    @param          inQualifierDataSize
                        A UInt32 indicating the size of the buffer pointed to by
                        inQualifierData. Note that not all properties require qualification, in
                        which case this value will be 0.
    @param          inQualifierData,
                        A buffer of data to be used in determining the data of the property
                        being queried. Note that not all properties require qualification, in
                        which case this value will be NULL.
    @param          ioDataSize
                        A UInt32 which on entry indicates the size of the buffer pointed to by
                        outData and on exit indicates how much of the buffer was used.
    @param          outData
                        The buffer into which the AudioObject will put the data for the given
                        property.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*ObjectGetPropertyData)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioObjectID                       inObjectID,
                                const AudioObjectPropertyAddress*   inAddress,
                                UInt32                              inQualifierDataSize,
                                const void* __nullable              inQualifierData,
                                UInt32*                             ioDataSize,
                                void*                               outData);

/*!
    @method         ObjectSetPropertyData
    @abstract       Tells an AudioObject to change the value of the given property using the
                    provided data.
    @param          inSelf
                        The plug-in instance that owns the AudioObject.
    @param          inObjectID
                        The AudioObject to change.
    @param          inAddress
                        An AudioObjectPropertyAddress indicating which property is being
                        changed.
    @param          inQualifierDataSize
                        A UInt32 indicating the size of the buffer pointed to by
                        inQualifierData. Note that not all properties require qualification, in
                        which case this value will be 0.
    @param          inQualifierData
                        A buffer of data to be used in determining the data of the property
                        being queried. Note that not all properties require qualification, in
                        which case this value will be NULL.
    @param          inDataSize
                        A UInt32 indicating the size of the buffer pointed to by inData.
    @param          inData
                        The buffer containing the data to be used to change the property's
                        value.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*ObjectSetPropertyData)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioObjectID                       inObjectID,
                                const AudioObjectPropertyAddress*   inAddress,
                                UInt32                              inQualifierDataSize,
                                const void* __nullable              inQualifierData,
                                UInt32                              inDataSize,
                                const void*                         inData);

/*!
    @function       DeviceCreateIOProcID
    @abstract       Creates an AudioDeviceIOProcID from an AudioDeviceIOProc and a client data
                    pointer.
    @discussion     AudioDeviceIOProcIDs allow for the client to register the same function pointer
                    with a device multiple times
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice to register the IOProc with.
    @param          inProc
                        The AudioDeviceIOProc to register.
    @param          inClientData
                        A pointer to client data that is passed back to the IOProc when it is
                        called.
    @param          outIOProcID
                        The newly created AudioDeviceIOProcID.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceCreateIOProcID)(    AudioHardwarePlugInRef __nonnull            inSelf,
                                AudioDeviceID                               inDevice,
                                AudioDeviceIOProc                           inProc,
                                void* __nullable                            inClientData,
                                AudioDeviceIOProcID __nullable * __nonnull  outIOProcID);

/*!
    @function       DeviceDestroyIOProcID
    @abstract       Destroys an AudioDeviceIOProcID.
    @discussion     AudioDeviceIOProcIDs allow for the client to register the same function pointer
                    with a device multiple times
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          inDevice
                        The AudioDevice from which the ID came.
    @param          inIOProcID
                        The AudioDeviceIOProcID to get rid of.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceDestroyIOProcID)(   AudioHardwarePlugInRef __nonnull    inSelf,
                                AudioDeviceID                       inDevice,
                                AudioDeviceIOProcID                 inIOProcID);

/*!
    @function       DeviceCreateIOProcIDWithBlock
    @abstract       Creates an AudioDeviceIOProcID from an AudioDeviceIOBlock.
    @param          inSelf
                        The plug-in instance that owns the AudioDevice.
    @param          outIOProcID
                        The newly created AudioDeviceIOProcID.
    @param          inDevice
                        The AudioDevice to register the Block with.
    @param          inDispatchQueue
                        The dispatch queue on which the IOBlock will be dispatched. All
                        IOBlocks are dispatched synchronously. Note that this dispatch queue will be
                        retained until a matching call to AudioDeviceDestroyIOProcID is made. If
                        this value is NULL, then the IOBlock will be directly invoked.
    @param          inBlock
                        The AudioDeviceIOBlock to register.  Note that this block will be
                        Block_copy'd and the reference maintained until a matching call to
                        AudioDeviceDestroyIOProcID is made.
    @result         An OSStatus indicating success or failure.
*/
    OSStatus
    (*DeviceCreateIOProcIDWithBlock)(   AudioHardwarePlugInRef __nonnull            inSelf,
                                        AudioDeviceIOProcID __nullable * __nonnull  outIOProcID,
                                        AudioDeviceID                               inDevice,
                                        dispatch_queue_t __nullable                 inDispatchQueue,
                                        AudioDeviceIOBlock                          inBlock);

};

//==================================================================================================
#pragma mark    AudioObject Functions

/*!
    @functiongroup  AudioObject
*/

/*!
    @function       AudioObjectCreate
    @abstract       Instantiates a new AudioObject.
    @discussion     Newly instantiated AudioObjects are not made available to the HAL's clients
                    until the object is published using AudioObjectsPublishedAndDied().
    @param          inOwningPlugIn
                        The plug-in instance instantiating the new AudioObject.
    @param          inOwningObjectID
                        The AudioObjectID of the AudioObject that owns the new AudioObject.
    @param          inClassID
                        The ID of the class of the new AudioObject.
    @param          outAudioObjectID
                        On exit, the AudioObjectID of the new AudioObject.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioObjectCreate(  AudioHardwarePlugInRef __nonnull    inOwningPlugIn,
                    AudioObjectID                       inOwningObjectID,
                    AudioClassID                        inClassID,
                    AudioObjectID*                      outAudioObjectID)                                       __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_4, __MAC_10_8, __IPHONE_2_0, __IPHONE_NA);

/*!
    @function       AudioObjectsPublishedAndDied
    @abstract       Tell the HAL to publish some AudioObjects and tear down others.
    @discussion     Note that all the objects must be owned by inOwningObjectID.
    @param          inOwningPlugIn
                        The plug-in instance that owns the AudioObjects.
    @param          inOwningObjectID
                        The AudioObjectID of the AudioObject that owns the objects being published
                        or destroyed.
    @param          inNumberPublishedAudioObjects
                        The number of items in the inPublishedAudioObjects array.
    @param          inPublishedAudioObjects
                        An array of AudioObjectIDs for the AudioObjects that are to be made
                        available to the HAL's clients.
    @param          inNumberDeadAudioObjects
                        The number of items in the inDeadAudioObjects array.
    @param          inDeadAudioObjects
                        An array of AudioObjectIDs for the AudioObjects that are no longer
                        available.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioObjectsPublishedAndDied(   AudioHardwarePlugInRef __nonnull    inOwningPlugIn,
                                AudioObjectID                       inOwningObjectID,
                                UInt32                              inNumberPublishedAudioObjects,
                                const AudioObjectID* __nullable     inPublishedAudioObjects,
                                UInt32                              inNumberDeadAudioObjects,
                                const AudioObjectID* __nullable     inDeadAudioObjects)                       __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_4, __MAC_10_8, __IPHONE_2_0, __IPHONE_NA);

/*!
    @function       AudioObjectPropertiesChanged
    @abstract       Tell the HAL that properties have changed on the given AudioObject.
    @discussion     The HAL will take care of calling any registered listeners.
    @param          inOwningPlugIn
                        The plug-in instance that owns the AudioObject whose properties have
                        changed.
    @param          inObjectID
                        The AudioObjectID of the AudioObject whose properties have changed.
    @param          inNumberAddresses
                        The number of items in the inAddresses array.
    @param          inAddresses
                        An array of AudioObjectPropertyAddress structs indicating which properties
                        of the device have changed.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioObjectPropertiesChanged(   AudioHardwarePlugInRef __nonnull    inOwningPlugIn,
                                AudioObjectID                       inObjectID,
                                UInt32                              inNumberAddresses,
                                const AudioObjectPropertyAddress*   inAddresses)                  __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_4, __MAC_10_8, __IPHONE_2_0, __IPHONE_NA);

//==================================================================================================
#pragma mark    AudioDevice Functions

/*!
    @functiongroup  AudioDevice
*/

/*!
    @function       AudioHardwareClaimAudioDeviceID
    @abstract       Reserve a new AudioDeviceID.
    @param          inOwner
                        The plug-in instance requesting the new AudioDeviceID.
    @param          outAudioDeviceID
                        On exit, the new AudioDeviceID.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareClaimAudioDeviceID(    AudioHardwarePlugInRef __nonnull    inOwner,
                                    AudioDeviceID*                      outAudioDeviceID)                       __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareDevicesCreated
    @abstract       Tell the HAL that new AudioDevices have come into being.
    @discussion     The plug-in must have previously reserved the AudioDeviceIDs using
                    AudioHardwareClaimAudioDeviceID().
    @param          inOwner
                        The plug-in instance that owns the new AudioDevices.
    @param          inNumberDevices
                        The number of items in the inAudioDeviceIDs array.
    @param          inAudioDeviceIDs
                        An array of AudioDeviceIDs that refer to the new AudioDevices.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareDevicesCreated(    AudioHardwarePlugInRef __nonnull    inOwner,
                                UInt32                              inNumberDevices,
                                const AudioDeviceID*                inAudioDeviceIDs)                           __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareDevicesDied
    @abstract       Tell the HAL that some AudioDevices have gone away.
    @param          inOwner
                        The plug-in instance that owns the dead AudioDevices.
    @param          inNumberDevices
                        The number of items in the inAudioDeviceIDs array.
    @param          inAudioDeviceIDs
                        An array of AudioDeviceIDs that refer to the dead AudioDevices.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareDevicesDied(   AudioHardwarePlugInRef __nonnull    inOwner,
                            UInt32                              inNumberDevices,
                            const AudioDeviceID*                inAudioDeviceIDs)                               __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareDevicePropertyChanged
    @abstract       Tell the HAL that the property of an AudioDevice owned by the plug-in has
                    changed.
    @discussion     The HAL will take care of calling any registered listeners.
    @param          inOwner
                        The plug-in instance that owns the AudioDevice whose property has changed.
    @param          inDeviceID
                        The AudioDeviceID of the AudioDevice whose property has changed.
    @param          inChannel
                        The channel of the AudioDevice whose property has changed.
    @param          isInput
                        The section of the AudioDevice whose proeprty has changed.
    @param          inPropertyID
                        The AudioObjectPropertySelector of the property that changed.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareDevicePropertyChanged( AudioHardwarePlugInRef __nonnull    inOwner,
                                    AudioDeviceID                       inDeviceID,
                                    UInt32                              inChannel,
                                    Boolean                             isInput,
                                    AudioDevicePropertyID               inPropertyID)                           __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

//==================================================================================================
#pragma mark    AudioStream Functions

/*!
    @functiongroup  AudioStream
*/

/*!
    @function       AudioHardwareClaimAudioStreamID
    @abstract       Reserve a new AudioStreamID.
    @param          inOwner
                        The plug-in instance requesting the new AudioStreamID.
    @param          inOwningDeviceID
                        The AudioDeviceID of the AudioDevice that is to own the AudioStream.
    @param          outAudioStreamID
                        On exit, the new AudioStreamID.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareClaimAudioStreamID(    AudioHardwarePlugInRef __nonnull    inOwner,
                                    AudioDeviceID                       inOwningDeviceID,
                                    AudioStreamID*                      outAudioStreamID)                       __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareStreamsCreated
    @abstract       Tell the HAL that new AudioStreams have come into being.
    @discussion     The plug-in must have previously reserved the AudioStreamIDs using
                    AudioHardwareClaimAudioStreamID().
    @param          inOwner
                        The plug-in instance that owns the new AudioStreams.
    @param          inOwningDeviceID
                        The AudioDeviceID of the AudioDevice that owns the AudioStreams.
    @param          inNumberStreams
                        The number of items in the inAudioStreamIDs array.
    @param          inAudioStreamIDs
                        An array of AudioStreamIDs that refer to the new AudioStreams.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareStreamsCreated(    AudioHardwarePlugInRef __nonnull    inOwner,
                                AudioDeviceID                       inOwningDeviceID,
                                UInt32                              inNumberStreams,
                                const AudioStreamID*                inAudioStreamIDs)                           __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareStreamsDied
    @abstract       Tell the HAL that some AudioStreams have gone away.
    @param          inOwner
                        The plug-in instance that owns the dead AudioStreams.
    @param          inOwningDeviceID
                        The AudioDeviceID of the AudioDevice that owns the AudioStreams.
    @param          inNumberDevices
                        The number of items in the inAudioStreamIDs array.
    @param          inAudioStreamIDs
                        An array of AudioStreamIDs that refer to the dead AudioStreams.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareStreamsDied(   AudioHardwarePlugInRef __nonnull    inOwner,
                            AudioDeviceID                       inOwningDeviceID,
                            UInt32                              inNumberStreams,
                            const AudioStreamID*                inAudioStreamIDs)                               __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareStreamPropertyChanged
    @abstract       Tell the HAL that the property of an AudioStream owned by the plug-in has
                    changed.
    @discussion     The HAL will take care of calling any registered listeners.
    @param          inOwner
                        The plug-in instance that owns the AudioStream whose property has changed.
    @param          inOwningDeviceID
                        The AudioDeviceID of the AudioDevice that owns the AudioStream.
    @param          inStreamID
                        The AudioStreamID of the AudioStream whose property has changed.
    @param          inChannel
                        The channel of the AudioStream whose property has changed.
    @param          inPropertyID
                        The AudioObjectPropertySelector of the property that changed.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareStreamPropertyChanged( AudioHardwarePlugInRef __nonnull    inOwner,
                                    AudioDeviceID                       inOwningDeviceID,
                                    AudioStreamID                       inStreamID,
                                    UInt32                              inChannel,
                                    AudioDevicePropertyID               inPropertyID)                           __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

//==================================================================================================

#if defined(__cplusplus)
}
#endif

CF_ASSUME_NONNULL_END

#endif  //  CoreAudio_AudioHardwarePlugIn_h
