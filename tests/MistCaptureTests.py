"""The old opaque teleport must fail even when screenshots are non-degenerate."""
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parent.parent/'tools'))
from mist_capture import validate_mist_drift


def fixture():
    rows = []
    for frame in range(1, 65):
        density = max(0, .2-abs(frame-1)*.01) if frame < 24 else min(.2, (frame-24)*.01)
        puffs = [f"{frame*.01 if frame<24 else 32+frame*.01},1,0,{density}"] + ['0,1,2,0.3']*9
        rows.append(f"wetland drift frame={frame} clock={frame*.1:.3f} subject=0 visible=10 puffs={';'.join(puffs)}")
    return '\n'.join(rows)


class MistCaptureTests(unittest.TestCase):
    def test_live_recycling(self):
        result = validate_mist_drift(fixture(), 'wetland')
        self.assertEqual(result['events'][0]['frame'], 24)
        validate_mist_drift(fixture().replace('wetland', 'valley'), 'valley')

    def test_rejects_opaque_teleport(self):
        with self.assertRaises(RuntimeError):
            validate_mist_drift(fixture().replace('32.24,1,0,0.0', '32.24,1,0,0.5'), 'wetland')

    def test_rejects_incomplete_stale_or_invalid_proof(self):
        good = fixture()
        for bad in ('\n'.join(good.splitlines()[:-1]), good+'\n'+good.splitlines()[0],
                    good.replace('clock=0.200', 'clock=nan'), good.replace('clock=0.200', 'clock=0.100'),
                    good.replace('subject=0', 'subject=11'), good.replace('visible=10', 'visible=0'),
                    good.replace(',0.3', ',-0.1'), good.replace('32.', '0.'),
                    good.replace('subject=0', 'subject=2')):
            with self.subTest(bad=bad[:100]), self.assertRaises(RuntimeError):
                validate_mist_drift(bad, 'wetland')


if __name__ == '__main__':
    unittest.main()
