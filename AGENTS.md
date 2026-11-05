# TRCAnonymizer - notes for agents

TRCAnonymizer is a Qt 6 Widgets desktop app that removes or replaces patient identity in
Micromed `.TRC` and `.EDF` EEG recordings. It has a GUI only: no command line, no library, no
tests. README.md is the user-side walkthrough; this file is for changing the code.

This tool exists to remove patient identity. Before changing anything that reads or writes a
file, know exactly which bytes it touches (see "What is written where"). A bug here does not
crash: it leaves identifying data in a file the user believes is clean.

## Build

Needs Qt 6 with the `core5compat` module (`QT += core gui widgets core5compat` in
`TRCAnonymizer.pro`) and a C++17 compiler.

```sh
mkdir -p build && cd build
qmake ../TRCAnonymizer.pro CONFIG+=release
make -j8            # Windows: nmake, from a vcvars64 prompt
```

- Build out of tree (as above): an in-tree `qmake && make` drops `*.o`, `moc_*`, `qrc_*`,
  `Makefile` and `TRCAnonymizer.app` next to the sources. `.gitignore` covers them, but never
  stage them.
- macOS: the `.pro` builds `arm64` only, deployment target 10.15. Linux: needs `libgomp1` and
  `libxcb-cursor0` (OpenMP flags are set, no OpenMP code uses them today).
- CI (`.github/workflows/release.yml`) runs only on a pushed tag `v*` / `V*`: it builds with
  Qt 6.5.* on Windows, macOS and Linux, packages each (windeployqt / macdeployqt + ad-hoc
  codesign / plain zip) and makes a GitHub release. Nothing runs on a branch or a PR, so the
  only check before merge is a local build and a manual run.
- The About dialog hardcodes `v1.0.0`; the release version comes from the tag.

## Layout

All sources sit at the repo root.

| File | Role |
|------|------|
| `main.cpp` | Entry point (plus a `WinMain` shim on Windows). |
| `TRCAnonymizer.{h,cpp,ui}` | Main window: file list, header fields, montages, notes, LUT tab, log. Holds the open file in `m_eegFile`. |
| `IFile.{h,cpp}` | Abstract file: patient and record fields, `AnonymizePatientData`, `AnonymizeRecordData`, `SaveAnonymizedData`. `IFile::Create(path)` picks the subclass by extension (`trc` / `edf`, case-insensitive), else `nullptr`. Also `AnonymizationMode`. |
| `MicromedFile.{h,cpp}` | TRC reader/writer: header, notes, montages. |
| `TRCParameters.h` | TRC byte offsets and sizes (`namespace TRC`), note and montage structs. |
| `EdfFile.{h,cpp}` | EDF / EDF+ reader/writer: header only. |
| `INote.h`, `GenericMontage.{h,cpp}` | Format-neutral note and montage label passed to the UI. |
| `Utility.{h,cpp}` | Binary read helpers, padded string write, `split`, EDF date format. |
| `AnonymizationWorker` | "Save Anonymized File": one file, or the same edits applied to every listed file. |
| `LutAnonymizationWorker` | Lookup-table tab: pseudonymization from a CSV, or full anonymization. |
| `InformationExtractionWorker` | Exports each listed file's header (names, birth date, record date and time) to a CSV. |
| `DuplicateCheckWorker` | Lists `.trc` / `.edf` files sharing a file name under a folder. |
| `Docs/img/`, `Resources/`, `trcanonymizer.qrc`, `myapp.rc` | README screenshots, app icons. |

Workers all follow one pattern: built in `TRCAnonymizer.cpp`, `moveToThread` onto a new
`QThread`, `Process()` runs on start, `sendLogInfo` feeds the log, `finished` quits the thread
and deletes both. `m_isAlreadyRunning` blocks a second anonymization while one runs.

## What is written where

Both formats are anonymized the same way: the input is copied byte for byte
(`std::filesystem::copy` to the `_anon` path, unless overwriting), then a few fields are
patched in place. Every byte not listed below keeps its original value.

### Micromed TRC (`MicromedFile::SaveAnonymizedData`, offsets in `TRCParameters.h`)

| Field | Offset | Size | Written as |
|-------|--------|------|-----------|
| Surname | 64 | 22 | text, space-padded |
| Name (first name) | 86 | 20 | text, space-padded |
| Birth month, day, year | 106, 107, 108 | 1 each | byte; year as `year - 1900` |
| Record day, month, year | 128, 129, 130 | 1 each | byte; year as `year - 1900` |
| Record time h, m, s | 131, 132, 133 | 1 each | byte |
| Montage area length | 300 | 4 | `kept montages * 4096` |
| Montage area | from the offset read at 296 | `MAX_MONT * 4096` | zeroed, then kept montages rewritten (label = `description`, 64 bytes) |
| Notes area | from the offset read at 216 | `MAX_NOTE * 44` | zeroed, then kept notes rewritten (4-byte sample + 40-byte text) |

Sampling rate (offset 400) is read for display only.

### EDF / EDF+ (`EdfFile::SaveAnonymizedData`)

| Field | Offset | Size | Written as |
|-------|--------|------|-----------|
| Local patient ID | 8 | 80 | `X X <dd-Mmm-yyyy> <name>_<surname>`, space-padded (hospital code and sex become `X`) |
| Local recording ID | 88 | 80 | `StartDate <dd-Mmm-yyyy> X X X`, space-padded |

Not written for EDF: the start date (offset 168, `dd.mm.yy`) and start time (176) header
fields, and anything in the data records (EDF+ annotations included). Leaving the start date
and time is deliberate: users must still be able to process the data with its timing, while
patient information is removed. Do not "fix" it without the owner's decision. Notes and
montages are no-ops for EDF (`EdfFile` returns no notes; the montage methods are empty).

### Values each path writes

- Manual tab, "Anonymize Header" button: fills the UI with name `Ymous`, surname `Anon`,
  birth date and record date 1/1/1900. Record time is left as is (commented out on purpose).
  The values only reach the file through the edit fields' `editingFinished` handlers, then
  "Save Anonymized File".
- `AnonymizationWorker`, one file: writes the open `m_eegFile` with its edited header, montages
  and notes, to the `_anon` copy. Never overwrites the original.
- `AnonymizationWorker`, "Process all files in list": reopens each listed file, applies the open
  file's name, surname, birth date and record date, and its montage list (by index), keeps each
  file's own record time and notes. Ticking the box discards note edits and disables the Notes
  tab.
- `LutAnonymizationWorker`, pseudonymization: name and surname from the CSV, birth date
  1/1/1900, record date and time unchanged.
- `LutAnonymizationWorker`, full anonymization: `Anon Ymous`, birth date 1/1/1900, record date
  1/1/1900, or shifted so the earliest recording per folder (or a chosen reference date) lands on
  1/1/1900 and intervals are kept. Record time unchanged.
- Both LUT modes: optional note filter (comma-separated, case-insensitive substring; delete
  matching notes or keep only matching ones). Montage labels are not touched, although they can
  carry patient names. With "overwrite" ticked the original file is patched in place, with no
  copy.

## Known issues (to fix later)

Confirmed by the owner; not fixed yet, no hurry. Fix each one on its own, on the owner's word.

- Strings are not truncated on write: `Utility::WriteCompleteString` writes the whole string and
  pads only if it is shorter than the field. A TRC name over 20 characters runs into the
  birth-date bytes and beyond; a surname over 22 runs into the name. Nothing in the UI limits
  the length.
- Lookup-table mode (both pseudonymization and full anonymization) never touches montage
  labels, although they can carry patient names. The owner wants to review how this should
  work before it changes.

## Gotchas

- The TRC notes and montage areas are zeroed for the maximum size (`MAX_NOTE * 44`,
  `MAX_MONT * 4096`), not for the length read from the header. Changing those constants, or the
  struct layouts in `TRCParameters.h`, changes which bytes get overwritten.
- TRC notes are read until the first note with sample 0; montage slots with an empty label are
  skipped on read and dropped on write.
- `Utility::BinaryStringExtraction` skips bytes that read as a negative `char`, so non-ASCII
  characters vanish from the displayed and exported values (on platforms where `char` is
  unsigned, they are kept). The field is still fully overwritten on save.
- The `_anon` path replaces the last 4 characters of the path with `_anon.trc` / `_anon.edf`:
  it assumes a 3-letter extension and always writes it in lower case. An existing `_anon` file
  is deleted first.
- LUT matching: each key is looked for as `/<key>/` in the lower-cased full path, so a key
  names a folder (one patient per folder), not a file. If a path contains several keys, the
  one found first in `QHash` order wins. The CSV must have exactly 3 `;`-separated fields on
  every line, header line included, or the whole table is dropped. The value is stored as
  `Surname#Firstname`; the worker takes the last part as the first name and joins the rest
  with `_` as the surname.
- The generated template's example row (`PAT_1;John;Doe`) puts a first name in the Surname
  column; the README example is the other way round.
- `InformationExtractionWorker` writes real names and dates into a CSV by design. Its output,
  like any input file, must never enter the repo.
- `std::strncpy` is used without including `<cstring>`; it builds through transitive includes.
- `RemoveSelectedMontages` destroys list items with an explicit `~QListWidgetItem()` call.

## Rules for agents

- Public repo, and a de-identification tool: no real file names, patient names, dates or header
  values anywhere (code, comments, commits, PRs, issues, tests, fixtures). Use the code's own
  placeholders: `Anon` / `Ymous`, 1/1/1900, `PAT_<n>`.
- There are no tests and no sample data in the repo. A change to a read or write path needs a
  synthetic file built for the purpose, checked byte by byte against the tables above; say what
  was not checked.
- Never commit build output, `.claude/settings.local.json`, `CLAUDE.local.md`, `.temp/` or
  `*.pro.user`. Stage explicit paths.
- Keep this file in step with the code: a change to an offset, a written field or a worker's
  defaults updates "What is written where" in the same commit.
