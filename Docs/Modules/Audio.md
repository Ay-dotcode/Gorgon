## Services

* Multimedia
* Widgets
* Games

## Uses

* Containers (Wave)
* OS specific audio libraries
 * WASAPI and WinMM (for device listing) on Windows
 * PulseAudio on Linux
* std::thread
* OS specific thread elevation functions

## Data supplier
* Resource
* Multimedia
* Encoding (from various formats)
* Import functionality in Containers::Wave

## Related
* Containers::Wave
* Resource::Wave
* Encoding::Flac
* Encoding::Vorbis
* Multimedia::Music
* Multimedia::Video

## Responsibilities

* Listing audio devices
* Querying speaker configuration
* Enabling positional audio
* Band separation for low frequency speakers
* Rendering audio to devices
* Allowing audio input
* Allow garbage collected wave controllers for ease of use
* Notify games about an update in the audio configuration. Update internal state to reflect changes
* Up or down sampling wave data to match device sampling
* Perform effects like echo, reverb and Doppler shift

## Remarks
* Should apply for every_frame in order to perform garbage collection
* Should create high priority thread for audio loop
* Should create low priority thread to perform low pass filtering

## Stretch goals
* Audio dampening: if too loud audio is played, all sounds would be dampened for a while
* Visual clues when audio hits clipping point.

## Public interface

* Get device list
* Get/set listener position/speed/acceleration/orientation
* Get/set unit of measurement
* Get/set volume (per channel)

### Device
* Name
* Channels

### Controller
* Data source
* Current position

#### Basic
* Play/pause/loop
* Duration
* Current position/seek
* isfinished/isplaying/islooping
* volume

#### Positional
* Current position

#### Multichannel
* Channel balance

#### Advanced : Positional
* Speed/acceleration/direction
* Dynamic (automatic calculation from the change in position

### Wave data
Supplied by Containers::Wave, stream? (update Wave)

### Effect

