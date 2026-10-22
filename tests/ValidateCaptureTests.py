#!/usr/bin/env python3

"""Regression tests for the deterministic P6 capture validator."""

from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch


PROJECT_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_ROOT))

from tools.validate_capture import (CaptureError, PpmImage, read_ppm, validate_capture,
                                    profile_for_viewpoint, profile_for_arguments)  # noqa: E402


class CaptureValidatorTests(unittest.TestCase):
    def test_profile_follows_last_explicit_camera(self):
        for arguments, expected in (([], "scene"), (["--viewpoint", "litter"], "shaded-detail"),
                (["--viewpoint", "litter-other"], "scene"),
                (["--viewpoint", "snake-head"], "close-natural-detail"),
                (["--viewpoint", "snakes"], "close-natural-detail"),
                (["--viewpoint", "tree-snakes"], "close-natural-detail"),
                (["--viewpoint", "cave-top"], "close-natural-detail"),
                (["--viewpoint", "cave-back"], "dark-cave-detail"),
                (["--viewpoint", "litter", "--overview"], "scene"),
                (["--overview", "--viewpoint", "litter"], "shaded-detail"),
                (["--viewpoint", "litter", "--viewpoint", "grass"], "scene")):
            with self.subTest(arguments=arguments):
                self.assertEqual(profile_for_arguments(arguments), expected)

    def test_shaded_profile_is_explicit_and_not_a_brightness_bypass(self):
        image = PpmImage(640, 360, b"")
        metrics = {"luminanceMean": 50.0, "luminanceStandardDeviation": 15.0,
                   "quantizedColorBins": 100, "spatialContrast": 40.0,
                   "darkPixelFraction": 0.0, "brightPixelFraction": 0.0}
        with patch("tools.validate_capture.analyze_capture", return_value=metrics):
            self.assertFalse(validate_capture(image)["valid"])
            report = validate_capture(image, profile="shaded-detail")
            self.assertTrue(report["valid"])
            self.assertEqual(report["profile"], "shaded-detail")
            self.assertEqual(report["thresholds"]["minimumLuminanceStandardDeviation"], 12.5)
            metrics.update(luminanceMean=160.0, luminanceStandardDeviation=20.0)
            self.assertTrue(validate_capture(image)["valid"])
            self.assertFalse(validate_capture(image, profile="shaded-detail")["valid"])
            metrics.update(luminanceMean=30.0, luminanceStandardDeviation=11.0)
            self.assertFalse(validate_capture(image, profile="shaded-detail")["valid"])
        with self.assertRaises(ValueError):
            validate_capture(image, profile="unknown")
        self.assertEqual(profile_for_viewpoint("litter"), "shaded-detail")
        self.assertEqual(profile_for_viewpoint("spiders"), "close-natural-detail")
        # The rat on the forest floor: reviewed 2026-10-21, a close natural detail view whose
        # litter and fur sat at 63-65 colour bins against the scene floor of 64.
        self.assertEqual(profile_for_viewpoint("rats"), "close-natural-detail")
        self.assertEqual(profile_for_viewpoint("spiders-bright"), "close-natural-detail")
        self.assertEqual(profile_for_viewpoint("snake-head"), "close-natural-detail")
        self.assertEqual(profile_for_viewpoint("snakes"), "close-natural-detail")
        self.assertEqual(profile_for_viewpoint("tree-snakes"), "close-natural-detail")
        self.assertEqual(profile_for_viewpoint("cave-back"), "dark-cave-detail")
        # Underwater views: reviewed 2026-10-21, the water's haze lowers the luminance spread.
        self.assertEqual(profile_for_viewpoint("fish"), "underwater-scene")
        self.assertEqual(profile_for_viewpoint("reef-habitat"), "underwater-scene")
        self.assertEqual(profile_for_viewpoint("dolphins"), "underwater-scene")
        self.assertEqual(profile_for_viewpoint("fish", 21.0), "night-scene")
        # The walker's own shadow on open sand: reviewed 2026-10-22, the sand fills the frame.
        self.assertEqual(profile_for_viewpoint("walker-shadow"), "sand-shadow-detail")
        self.assertEqual(profile_for_viewpoint("walker-shadow", 21.0), "night-scene")
        for name in ("", "overview", "grass", "wet-sand", "litter-other", "interactive"):
            self.assertEqual(profile_for_viewpoint(name), "scene")

    def test_underwater_scene_relaxes_only_the_luminance_spread(self):
        image = PpmImage(640, 360, b"")
        metrics = {"luminanceMean": 150.0, "luminanceStandardDeviation": 12.5,
                   "quantizedColorBins": 120, "spatialContrast": 80.0,
                   "darkPixelFraction": 0.0, "brightPixelFraction": 0.0}
        with patch("tools.validate_capture.analyze_capture", return_value=metrics):
            self.assertTrue(validate_capture(image, profile="underwater-scene")["valid"])
            self.assertFalse(validate_capture(image, profile="scene")["valid"])
        for key, bad in (("luminanceStandardDeviation", 11.5), ("quantizedColorBins", 63),
                         ("spatialContrast", 29.0)):
            with patch("tools.validate_capture.analyze_capture", return_value={**metrics, key: bad}):
                self.assertFalse(validate_capture(image, profile="underwater-scene")["valid"])

    def test_sand_shadow_detail_relaxes_only_the_spread_and_the_colour_bins(self):
        image = PpmImage(640, 360, b"")
        metrics = {"luminanceMean": 123.0, "luminanceStandardDeviation": 12.1,
                   "quantizedColorBins": 21, "spatialContrast": 38.4,
                   "darkPixelFraction": 0.0, "brightPixelFraction": 0.0}
        with patch("tools.validate_capture.analyze_capture", return_value=metrics):
            self.assertTrue(validate_capture(image, profile="sand-shadow-detail")["valid"])
            self.assertFalse(validate_capture(image, profile="scene")["valid"])
        for key, bad in (("luminanceStandardDeviation", 9.5), ("quantizedColorBins", 15),
                         ("spatialContrast", 29.0)):
            with patch("tools.validate_capture.analyze_capture", return_value={**metrics, key: bad}):
                self.assertFalse(validate_capture(image, profile="sand-shadow-detail")["valid"])

    def test_close_natural_detail_retains_bounded_non_degenerate_thresholds(self):
        image = PpmImage(640, 360, b"")
        metrics = {"luminanceMean": 53.0, "luminanceStandardDeviation": 12.0,
                   "quantizedColorBins": 49, "spatialContrast": 18.0,
                   "darkPixelFraction": 0.0, "brightPixelFraction": 0.0}
        with patch("tools.validate_capture.analyze_capture", return_value=metrics):
            report = validate_capture(image, profile="close-natural-detail")
            self.assertTrue(report["valid"])
            self.assertEqual(report["thresholds"]["minimumQuantizedColorBins"], 40)
            self.assertEqual(report["thresholds"]["minimumSpatialContrast"], 15.0)
            for key, bad in (("luminanceStandardDeviation", 9.0),
                             ("quantizedColorBins", 39), ("spatialContrast", 14.0)):
                # Re-run with one deliberately failed gate while preserving all others.
                with patch("tools.validate_capture.analyze_capture",
                           return_value={**metrics, key: bad}):
                    self.assertFalse(validate_capture(
                        image, profile="close-natural-detail")["valid"])

    def test_night_profile_only_for_an_explicit_night_hour(self):
        # The night profile follows an explicit --time-of-day in the dark hours; a black or
        # featureless frame still fails it.
        self.assertEqual(profile_for_arguments(["--viewpoint", "hut", "--time-of-day", "21"]), "night-scene")
        self.assertEqual(profile_for_arguments(["--viewpoint", "hut", "--time-of-day", "2.5"]), "night-scene")
        self.assertEqual(profile_for_arguments(["--viewpoint", "hut", "--time-of-day", "14"]), "scene")
        self.assertEqual(profile_for_arguments(["--viewpoint", "hut"]), "scene")
        self.assertEqual(profile_for_viewpoint("hut", 21.0), "night-scene")
        image = PpmImage(640, 360, b"")
        metrics = {"luminanceMean": 14.0, "luminanceStandardDeviation": 4.5,
                   "quantizedColorBins": 29, "spatialContrast": 22.0,
                   "darkPixelFraction": 0.84, "brightPixelFraction": 0.0}
        with patch("tools.validate_capture.analyze_capture", return_value=metrics):
            self.assertFalse(validate_capture(image)["valid"])
            self.assertTrue(validate_capture(image, profile="night-scene")["valid"])
            metrics.update(luminanceMean=2.0, darkPixelFraction=1.0)
            self.assertFalse(validate_capture(image, profile="night-scene")["valid"])
            metrics.update(luminanceMean=14.0, darkPixelFraction=0.84, luminanceStandardDeviation=1.0)
            self.assertFalse(validate_capture(image, profile="night-scene")["valid"])

    def test_shaded_profile_preserves_other_failure_gates(self):
        image = PpmImage(640, 360, b"")
        good = {"luminanceMean": 50.0, "luminanceStandardDeviation": 15.0,
                "quantizedColorBins": 100, "spatialContrast": 40.0,
                "darkPixelFraction": 0.0, "brightPixelFraction": 0.0}
        for key, bad in (("quantizedColorBins", 63), ("spatialContrast", 29),
                         ("luminanceMean", 23), ("darkPixelFraction", 0.93),
                         ("brightPixelFraction", 0.93)):
            with self.subTest(key=key), patch("tools.validate_capture.analyze_capture",
                                            return_value={**good, key: bad}):
                self.assertFalse(validate_capture(image, profile="shaded-detail")["valid"])
        for rgb in ((0, 0, 0), (255, 255, 255), (50, 45, 30)):
            flat = PpmImage(640, 360, bytes(rgb) * 640 * 360)
            self.assertFalse(validate_capture(flat, profile="shaded-detail")["valid"])

    def test_reads_comment_and_crlf_header(self) -> None:
        pixels = bytes((10, 20, 30, 40, 50, 60))
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "capture.ppm"
            path.write_bytes(b"P6\r\n# generated\r\n2 1\r\n255\r\n" + pixels)
            image = read_ppm(path)
        self.assertEqual((image.width, image.height), (2, 1))
        self.assertEqual(image.pixels, pixels)

    def test_rejects_non_binary_and_truncated_captures(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "capture.ppm"
            path.write_bytes(b"P3\n1 1\n255\n0 0 0")
            with self.assertRaisesRegex(CaptureError, "binary P6"):
                read_ppm(path)
            path.write_bytes(b"P6\n2 1\n255\n\x00\x00\x00")
            with self.assertRaisesRegex(CaptureError, "payload"):
                read_ppm(path)

    def test_rejects_solid_and_dimension_mismatched_images(self) -> None:
        width = 640
        height = 360
        image = PpmImage(width, height, bytes((80, 120, 160)) * width * height)
        report = validate_capture(image, expected_width=1280, expected_height=720)
        self.assertFalse(report["valid"])
        self.assertIn("capture width is 640; expected 1280", report["errors"])
        self.assertIn("capture height is 360; expected 720", report["errors"])
        self.assertIn("capture has insufficient luminance variation", report["errors"])
        self.assertIn("capture has too few quantized colour bins", report["errors"])
        self.assertIn("capture has insufficient large-scale spatial contrast", report["errors"])


if __name__ == "__main__":
    unittest.main()
