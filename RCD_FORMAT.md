# RCD File Format

This document describes the structure of **Rail Control Data (RCD)** files used by the simulator.
RCD files use an INI style format where each section contains comma separated values.

## Sections

The main sections appear in the following order when the file is read:

1. `[SECTIONS]` – track geometry
2. `[GENERAL]` – global settings and options
3. `[OVERLAPPING]` – pairs of mutually exclusive sections
4. `[PLATFORMS]` – platform polygon definitions
5. `[SELECTOR]` – interactive route buttons
6. `[ROUTES]` – route definitions
7. `[LOCOS]` – locomotive stock
8. `[LOCOYARD]` – initial locoyard assignments
9. `[TIMETABLE]` – train arrival and departure information

A brief description of each section is given below.

## `[SECTIONS]`
Defines individual track sections. Each entry has an index followed by four `x,y` coordinate pairs:

```
index, x1, y1, x2, y2, x3, y3, x4, y4
```
Indices range from `1` to `999`.

## `[GENERAL]`
Key/value pairs controlling global behaviour. Recognised keys include:

- `StartTime` – start time of the simulation (HHMM integer)
- `StopTime`  – end time of the simulation (HHMM integer)
- `StartText1`/`StartText2`/`StartText3` – text shown in the start dialog
- `DrawSectionNumber` – `T` or `F` flag to overlay section numbers
- `DrawSelectorNumber` – `T` or `F` flag to overlay selector numbers

## `[OVERLAPPING]`
Specifies pairs of sections that cannot be used simultaneously:

```
index, sectionA, sectionB
```
Indices run from `1` to `49`.

## `[PLATFORMS]`
Each platform is stored as an index and four `x,y` points describing a polygon:

```
index, x1, y1, x2, y2, x3, y3, x4, y4
```
Indices run from `1` to `49`.

## `[SELECTOR]`
Route selector buttons on the diagram. Each line contains the index, window rectangle, selector type and optional platform reference, followed by the display text:

```
index, x, y, width, height, type, platRef, text
```
Selector indices range from `1` to `49`. `type` corresponds to constants such as `SEL_INPUT`, `SEL_OUTPUT`, etc.

## `[ROUTES]`
Defines valid routes between selectors. Each entry lists the starting selector, ending selector and up to six clearing section pairs encoded as `s1 + 1000*s2`:

```
index, fromSel, toSel, clr1, clr2, clr3, clr4, clr5, clr6
```
Indices range from `1` to `999`.

## `[LOCOS]`
Locomotive information. Each entry stores class, number and stock code:

```
index, class, number, type
```
Indices run from `1` to `499`.

## `[LOCOYARD]`
Initial locomotives in the locoyard. The section may start with the word `Disabled` to turn the yard off. Otherwise each line lists a locomotive type and a refuelling offset in minutes from `StartTime`:

```
type, refuelOffset
```
Only up to 15 entries are processed.

## `[TIMETABLE]`
Train schedule entries. Each line begins with a timetable index followed by arrival/departure descriptions and nine numeric fields:

```
index, arrDesc, depDesc,
  arrSel, arrTime, relTime, depTime,
  arrCode, depCode, status, extra1, nextIndex
```
- `arrSel` – input selector number for arrival
- `arrTime` – arrival time HHMM
- `relTime` – stock release time HHMM
- `depTime` – departure time HHMM
- `arrCode`/`depCode` – stock codes (see `general.h`)
- `status` – starting state (e.g. `ST_INPLAT`)
- `extra1` – meaning depends on `status`
- `nextIndex` – timetable entry to inherit the locomotive from

Indices range from `1` to `499`.

---

This format is derived from the parsing logic in `LAYOUT.CPP` and reflects the data structures used by the simulator.
