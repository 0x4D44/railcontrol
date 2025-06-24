# RCD File Format Documentation

## Overview

The RCD (Railway Control Data) file format is used by the Rail Control simulator to define the layout, timetable, and operational parameters for a railway simulation. The file uses an INI-like structure, where data is organized into sections, each starting with a header enclosed in square brackets (e.g., `[GENERAL]`). Comments can typically be added by prefixing a line with a semicolon (`;`) or, in some specific cases like the `[TIMETABLE]` section, using a hash (`#`) for end-of-line comments.

Below is a detailed description of each section and its data format.

---

## 1. `[GENERAL]`

This section defines global parameters for the simulation.

- **Purpose**: Specifies overall simulation settings like start/stop times and introductory text.
- **Format**: Each line is a key-value pair.
    - `StartTime=HHMM`
        - `HHMM`: The simulation start time in 24-hour format (e.g., `0700` for 7:00 AM).
    - `StopTime=HHMM`
        - `HHMM`: The simulation end time in 24-hour format (e.g., `1900` for 7:00 PM).
    - `StartText1=string`
        - `string`: First line of text displayed at the simulation start.
    - `StartText2=string`
        - `string`: Second line of text displayed at the simulation start.
    - `StartText3=string`
        - `string`: Third line of text displayed at the simulation start.
    - `DrawSectionNumber=boolean` (Optional)
        - `boolean`: `T` or `t` if section numbers should be drawn on the layout; otherwise, this line can be omitted or set to other values for false.
    - `DrawSelectorNumber=boolean` (Optional)
        - `boolean`: `T` or `t` if selector numbers should be drawn on the layout; otherwise, this line can be omitted or set to other values for false.

---

## 2. `[SECTIONS]`

This section defines the individual track segments (polygons) that make up the railway layout.

- **Purpose**: Describes the geometry of track sections.
- **Format**: Each line defines one track section.
    - `IndexRef,X1,Y1,X2,Y2,X3,Y3,X4,Y4`
        - `IndexRef`: An integer ID for this section (unique, 1-999).
        - `X1,Y1`: Integer coordinates for the first point of the section polygon.
        - `X2,Y2`: Integer coordinates for the second point.
        - `X3,Y3`: Integer coordinates for the third point.
        - `X4,Y4`: Integer coordinates for the fourth point.

---

## 3. `[OVERLAPPING]`

This section defines pairs of track sections that overlap, which is used for conflict detection when setting routes.

- **Purpose**: Specifies which track sections physically overlap.
- **Format**: Each line defines one overlapping pair.
    - `IndexRef,Section1_ID,Section2_ID`
        - `IndexRef`: An integer ID for this overlap definition (unique, 1-49).
        - `Section1_ID`: The `IndexRef` of the first section in the overlap (must exist in `[SECTIONS]`).
        - `Section2_ID`: The `IndexRef` of the second section in the overlap (must exist in `[SECTIONS]`).

---

## 4. `[PLATFORMS]`

This section defines the visual representation (polygons) of platforms on the layout. These are distinct from the operational platform selectors.

- **Purpose**: Describes the geometry of platform indicators.
- **Format**: Each line defines one platform indicator.
    - `IndexRef,X1,Y1,X2,Y2,X3,Y3,X4,Y4`
        - `IndexRef`: An integer ID for this platform indicator (unique, 1-49).
        - `X1,Y1`: Integer coordinates for the first point of the platform polygon.
        - `X2,Y2`: Integer coordinates for the second point.
        - `X3,Y3`: Integer coordinates for the third point.
        - `X4,Y4`: Integer coordinates for the fourth point.
- **Note**: Each platform defined here is typically associated with a selector button defined in the `[SELECTOR]` section.

---

## 5. `[SELECTOR]`

This section defines the interactive buttons on the screen that are used to set routes.

- **Purpose**: Defines clickable selectors for route setting.
- **Format**: Each line defines one selector button.
    - `IndexRef,XPos,YPos,Width,Height,Type,TypeSpecificRef,Text`
        - `IndexRef`: An integer ID for this selector (unique, 1-49).
        - `XPos`: Integer X-coordinate of the button's top-left corner.
        - `YPos`: Integer Y-coordinate of the button's top-left corner.
        - `Width`: Integer width of the button.
        - `Height`: Integer height of the button.
        - `Type`: An integer code representing the selector's function. Common types (defined in `general.h` or equivalent):
            - `SEL_INPUT`: An entry point into the controlled area.
            - `SEL_OUTPUT`: An exit point from the controlled area.
            - `SEL_PLAT`: A standard platform.
            - `SEL_ELECTRIC_PLAT`: A platform specifically for electric trains.
            - `SEL_LOCOYARD`: The locomotive yard.
            - `SEL_HOLDPOINT`: A holding point.
        - `TypeSpecificRef`: An integer reference whose meaning depends on the `Type`:
            - If `Type` is `SEL_PLAT` or `SEL_ELECTRIC_PLAT` (typically codes 3 or 4), this is the `IndexRef` of the corresponding platform defined in the `[PLATFORMS]` section (must be 1-49 and exist).
            - For other types like `SEL_LOCOYARD` or `SEL_HOLDPOINT`, this field might be used for specific pre-defined section IDs or be 0 if not applicable.
        - `Text`: The text label displayed on the selector button.

---

## 6. `[ROUTES]`

This section defines valid routes between pairs of selectors and the track sections that must be clear for each route.

- **Purpose**: Lists all possible valid train paths.
- **Format**: Each line defines one route.
    - `IndexRef,From_SelectorID,To_SelectorID,ClearSec1A,ClearSec1B,ClearSec2A,ClearSec2B,ClearSec3A,ClearSec3B,ClearSec4A,ClearSec4B,ClearSec5A,ClearSec5B,ClearSec6A,ClearSec6B`
        - `IndexRef`: An integer ID for this route (unique, 1-999).
        - `From_SelectorID`: The `IndexRef` of the starting selector for this route (must exist in `[SELECTOR]`).
        - `To_SelectorID`: The `IndexRef` of the ending selector for this route (must exist in `[SELECTOR]`).
        - `ClearSec1A` to `ClearSec6B`: Twelve integer fields representing up to six pairs of track section IDs (`IndexRef` from `[SECTIONS]`). These sections must be unoccupied for the route to be set. A value of `0` indicates no section for that part of the path.

---

## 7. `[LOCOS]`

This section defines the types of locomotives available for the simulation.

- **Purpose**: Describes the fleet of available locomotives.
- **Format**: Each line defines one locomotive configuration.
    - `IndexRef,Type,Param1,Param2`
        - `IndexRef`: An integer ID for this locomotive definition (unique, 1-499).
        - `Type`: An integer code representing the class or type of the locomotive (e.g., `SC_HST`, `SC_EMU`, `SC_NORMAL`). These codes are defined in `general.h`.
        - `Param1`, `Param2`: Integer parameters further defining the locomotive's characteristics or initial state. The exact meaning of these parameters depends on the `TLocos` class implementation and might relate to initial fuel/maintenance flags or other operational attributes. `CLAUDE.md` mentions flags like `LF_UNASSIGN`, `LF_ASSIGNED`, `LF_NEEDFUEL`, `LF_REFUEL`, `LF_LOCOYARD`, `LF_MAINTAIN`.

---

## 8. `[LOCOYARD]`

This section defines the initial state of the locoyard, specifying which locomotives are present at the start of the simulation.

- **Purpose**: Lists locomotives initially present in the locoyard.
- **Format**:
    - The section can optionally start with the line `Disabled`. If this line is present, the locoyard functionality is disabled, and no further lines in this section are processed.
    - Otherwise, each subsequent line defines a locomotive to be placed in the yard:
        - `LocoType,RefuelTimeOffset`
            - `LocoType`: An integer code for the locomotive type (referencing types from `[LOCOS]` or defined in `general.h`). Valid types usually range from 1-6 and 10-12.
            - `RefuelTimeOffset`: An integer. If refueling is enabled and this value is greater than 0, it's the time in simulation units (half-minutes, so 2 per minute) from the `StartTime` when this loco finishes refueling and becomes available. If 0, or if refueling is disabled, the loco is available immediately.
- **Note**: The simulation supports a maximum of 16 locomotive slots in the locoyard.

---

## 9. `[TIMETABLE]`

This section lists all scheduled train movements, including arrivals, departures, and internal shunting.

- **Purpose**: Defines all train services and their characteristics.
- **Format**: Each line defines one timetable event/train.
    - `IndexRef,ArrDesc,DepDesc,ArrPnt,ArrTime,RelTime,DepPnt,DepTime,ArrCode,DepCode,InitialLocoPlat,NextTimeTabPos[,#Comment]`
        - `IndexRef`: An integer ID for this timetable entry (unique, 1-`MAX_TIMETABLE` (default 500)).
        - `ArrDesc`: A string describing the arrival (e.g., "1A01" or a headcode).
        - `DepDesc`: A string describing the departure (e.g., "1B02" or a headcode).
        - `ArrPnt`: The `IndexRef` of the selector where the train arrives (must be of `SEL_INPUT` type and exist in `[SELECTOR]`).
        - `ArrTime`: Arrival time in HHMM format.
        - `RelTime`: Release time in HHMM format. This is the time after arrival when the stock is released or becomes ready for its next working. Its interpretation (absolute or relative to arrival) can depend on the train's state.
        - `DepPnt`: The `IndexRef` of the selector from which the train departs. (Note: `LAYOUT.CPP` parsing doesn't seem to directly use this field for route setting, relying more on `ArrPnt` for arrivals and platform states for departures).
        - `DepTime`: Departure time in HHMM format.
        - `ArrCode`: An integer code for the arriving train's stock/locomotive type (e.g., `SC_HST`, `SC_EMU`). These codes are defined in `general.h`.
        - `DepCode`: An integer code for the departing train's stock/locomotive type.
        - `InitialLocoPlat`: If the train starts in a platform (status `ST_INPLAT` or `ST_STOCKOK`), this is the `IndexRef` (1-14) of that platform (from `[PLATFORMS]`). Otherwise, usually 0.
        - `NextTimeTabPos`: The `IndexRef` of another timetable entry that this train/locomotive will form. Used for linking services (e.g., a train arriving and then forming a new departure, or a loco detaching and attaching to another service). A value of `0` means no linked service. If non-zero, the linked entry must exist, and there are consistency checks (e.g., `DepCode` of current entry should match `ArrCode` of linked entry).
        - `#Comment`: (Optional) Any text after a `#` character on the line is treated as a comment. The application specifically parses comments like `#Becomes TrainID` to validate against `NextTimeTabPos`.

---

Refer to `general.h` (or equivalent header files in the source code) for specific integer values of codes like `SEL_TYPE`, `SC_LOCO_TYPE`, `ST_TRAIN_STATUS`, `LF_LOCO_FLAG`, etc.
