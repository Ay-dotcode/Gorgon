# System design

## Core modules

### Operating system ([n] OS)

* [f]    Name
* [f]    Version
* [f][d] GetEnv
* [f][d] Open
* [f][d] OpenTerminal
* [f][d] Start
* [f][d] DisplayMessage
* [f][d] GetAppDataPath
* [f][d] GetAppSettingPath
* [f]    IsInternetConnected
* [f]    IsNetworkConnected
* [f]    IsWifiConnected
* [f]    BatteryStatus
* [f]    BatterPercentage
* [f]    AvailableMemory
* [f]    FreeMemory
* [f]    ProcessorInfo
* [f]    GPUInfo
* [f]    FileTypeData
* [f]    SystemFonts -> vector(filename)
* [f]    SystemSound -> map(tag, filename)

#### [c] CPU
* [m] Brand
* [m] Model
* [m] Speed
* [m] Cores
* [m] Threads

#### [c] GPU
* [m] Memory
* [m] Brand
* [m] Model

#### [c] FileType
* [m] Name
* [m] Description
* [m] Icon

#### [n] User
* [f][d] GetName
* [f][d] GetUsername
* [f][d] GetDocumentsPath
* [f][d] GetHomePath
* [f][d] GetDataPath
* [f]    IsAdmin

### Window manager

### Graphics related

### Audio (optional)

### Multimedia (optional)

### Animation

### Resource

### Input



## Supportive modules

### [n] Filesystem

* [f] CreateDirectory
* [f] Delete
* [f] IsFile
* [f] IsDirectory
* [f] Exists
* [f] Cononicalize
* [f] TryCanonoicalize
* [f] GetCurrentDir
* [f] IsHidden
* [f] IsReadOnly
* [f] RelativePath
* [f] Join
* [f] GetFilename
* [f] GetDirectory
* [f] ConvertToNative
* [f] ConvertFromNative
* [f] ConvertToURI
* [f] Copy
* [f] Move
* [f] Trash
* [f] ExeDirectory
* [f] ExePath
* [f] ExeFilename
* [f] StartupDirectory
* [f] FreeSpace
* [f] TotalSpace
* [f] Drives -> map(path, name)
* [f] MakeReadonly
* [f] MakeHidden
* [f] GetSize
* [f] IsRemovable
* [f] IsNetwork
* [f] DriveType
* [f] Directory

#### [c] DirectoryIterator
	Standard iterator
	
#### [c] DirectoryListing
* [f] begin
* [f] end

### [n] Time

* [f] GetDate
* [f] GetTime
* [f] FrameStart
* [f] DeltaTime
* [f] Timeout
* [f] Interval
* [f] Cancel
* [f] LocalTimezone

#### [c] Timer
* [f] Start
* [f] Tick
* [f] Get
* [f] Set
* [f] ShowMessage

#### [c] Date
* [m]  Millisecond
* [m]  Second
* [m]  Minute
* [m]  Hour
* [m]  Day
* [m]  Month
* [m]  Year
* [m]  Timezone
* [m]  WeekDay
* [f]  AddDays
* [f]  AddMonths
* [f]  AddYears
* [f]  AddSeconds
* [f]  AddMinutes
* [f]  AddHours
* [f]  Date_En
* [f]  ISODate
* [f]  ISODateTime
* [f]  MonthName_En
* [f]  ShortMonthName_En
* [f]  WeekdayName_En
* [f]  ShortWeekdayName_En
* [f]  Time
* [f]  ShortTime
* [f]  Timezone_GMT
* [f]  IsSet
* [f]  Unset
* [f]  Normalize
* [f]  Save
* [f]  Load
* [f]  Timestamp
* [f]  Normalize
* [sf] Now 

### Logger

### Containers

### Encoding

### Geometry

### Template meta programming

### Compiler

### Console

### String



## Additional modules

### Data services

### CGI

### Effects

### Widgets

### Network

### Scripting



## Gaming

### World

### Physics

### Actors

### Additional shaders


