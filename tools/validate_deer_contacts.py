"""Check actual rendered deer hoof positions across the existing 64-frame motion views."""
import math
import re


def validate_deer_contacts(text):
    rows = re.findall(r"^island-simulator: deer contact frame=(\d+) heading=(\S+) speed=(\S+) "
                      r"bed=(\S+) valid=(\d+) visible=(\d+) feet=(\S+)$", text, re.M)
    if len(rows) != 64 or text.count("deer contact frame=") != 64:
        raise RuntimeError("deer contact proof requires 64 ordered rendered poses")
    samples = []
    pairs = 0
    maximum_slip = 0.0
    maximum_rotation = 0.0
    for index, row in enumerate(rows):
        frame, heading, speed, bed, valid, visible, encoded = row
        h, v, b = float(heading), float(speed), float(bed)
        feet = [tuple(map(float, foot.split(','))) for foot in encoded.split(';')]
        if (int(frame) != index + 1 or not all(map(math.isfinite, (h, v, b)))
                or not 0 <= v <= 3.101 or not 0 <= b <= 1 or int(valid) not in (0, 1)
                or bool(int(valid)) != (b == 0)
                or not 1 <= int(visible) <= 4 or len(feet) != 4
                or any(len(f) != 6 or not all(map(math.isfinite, f)) or f[3] not in (0, 1) for f in feet)
                or (int(valid) and (b != 0 or sum(f[3] for f in feet) < 2))
                or any(f[3] and (not int(valid) or abs(f[4]) > .0001) for f in feet)):
            raise RuntimeError("invalid deer contact clock, visibility or grounded support")
        if samples:
            for old, new in zip(samples[-1]['feet'], feet):
                if old[3] and new[3]:
                    pairs += 1
                    maximum_slip = max(maximum_slip, math.dist(old[:3], new[:3]))
                    maximum_rotation = max(maximum_rotation, abs(math.remainder(new[5] - old[5], math.tau)))
        samples.append(dict(frame=index + 1, heading=h, speed=v, bed=b, valid=bool(int(valid)), feet=feet))
    if pairs < 40 or maximum_slip > .00003 or maximum_rotation > .00003:
        raise RuntimeError("deer planted hoof slides or rotates against the ground")
    return dict(samples=samples, support_pairs=pairs, max_stance_slip=maximum_slip,
                max_stance_rotation=maximum_rotation)
