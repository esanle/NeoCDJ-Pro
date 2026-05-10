# Mock USB Drive

This directory simulates a Pioneer CDJ USB flash drive in the simulator.

Press **`U`** in the simulator to toggle insert/eject.

## Recommended layout

```
mock_usb/
├── PIONEER/
│   └── USBANLZ/
│       └── <track hash>/
│           ├── ANLZ0000.DAT   ← Rekordbox beat grid + cue points
│           └── ANLZ0000.EXT   ← Waveform data
├── 01 - Track One.flac
├── 02 - Track Two.wav
└── ...
```

Audio files in the root or subdirectories are scanned by the file browser (Phase 1+).  
`PIONEER/USBANLZ/` is parsed for Rekordbox analysis data (Phase 4+).

Audio files are gitignored — add your own test tracks here.
