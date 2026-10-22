#!/usr/bin/env python3
"""Keeps the capture evidence directories from filling the disk.

Each run's frames are stored as PNG (lossless, a fraction of a raw PPM's 2.7 MB) once the run has
validated them, and only the newest capture sets are kept, besides any set whose stamp handoff.md
cites. A browser run's left-over Chrome profile (hundreds of MB of cache after a timeout) goes too.
"""

from __future__ import annotations

import re
import shutil
import struct
import sys
import zlib
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from validate_capture import read_ppm  # noqa: E402

KEEP_SOFTWARE_CAPTURE_SETS = 400   # some three full sweeps of the native views
KEEP_BROWSER_SETS = 100
SET_STAMP = re.compile(r"^(20\d{6}T\d{6})")
CITED_STAMP = re.compile(r"20\d{6}T\d{6}")


def write_png(path: Path, width: int, height: int, rgb: bytes) -> None:
    """Writes 8-bit RGB pixels as a PNG (no filtering, zlib level 6)."""
    stride = width * 3
    raw = b"".join(b"\x00" + rgb[row * stride:(row + 1) * stride] for row in range(height))

    def chunk(kind: bytes, data: bytes) -> bytes:
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF)

    header = struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)
    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", header) + chunk(b"IDAT", zlib.compress(raw, 6))
                     + chunk(b"IEND", b""))


def compress_captures(directory: Path) -> int:
    """Replaces every PPM directly in `directory` by a PNG of the same name; returns how many."""
    converted = 0
    for ppm in sorted(directory.glob("*.ppm")):
        image = read_ppm(ppm)
        png = ppm.with_suffix(".png")
        write_png(png, image.width, image.height, image.pixels)
        ppm.unlink()
        converted += 1
    return converted


def cited_stamps(root: Path) -> set[str]:
    """The capture stamps handoff.md cites (their sets are kept whatever their age)."""
    try:
        return set(CITED_STAMP.findall((root / "handoff.md").read_text()))
    except OSError:
        return set()


def prune_sets(parent: Path, keep: int, cited: set[str]) -> list[Path]:
    """Removes all but the newest `keep` stamped sets under `parent`, sparing cited stamps."""
    if not parent.is_dir():
        return []
    sets = sorted((entry for entry in parent.iterdir() if entry.is_dir() and SET_STAMP.match(entry.name)),
                  key=lambda entry: entry.name, reverse=True)
    removed = []
    for entry in sets[keep:]:
        if SET_STAMP.match(entry.name).group(1) in cited:
            continue
        shutil.rmtree(entry, ignore_errors=True)
        removed.append(entry)
    return removed


def remove_browser_profiles(directory: Path) -> int:
    """Removes Chrome user-data directories a browser run left in its evidence."""
    removed = 0
    for profile in directory.glob("chrome-profile-*"):
        if profile.is_dir():
            shutil.rmtree(profile, ignore_errors=True)
            removed += 1
    return removed
