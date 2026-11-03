"""Validate the optional real-AI octopus crawl/forage/swim inspection."""
import math
import re


def validate_octopus_motion(log, subject):
    rows = re.findall(r"^island-simulator: octopus motion stage=(\d+) updates=(\d+) time=(\S+) "
                      r"subject=(\d+) behavior=(\d+) contact=(\S+) visible=(\d+) "
                      r"gap_min=(\S+) gap_max=(\S+) position=(\S+)$", log, re.M)
    if len(rows) != 16 or log.count("octopus motion stage=") != 16:
        raise RuntimeError("octopus motion needs sixteen ordered rendered stages")
    samples = []
    for index, row in enumerate(rows):
        stage, updates, time, selected, behavior, contact, visible, low, high, position = row
        t, c, lo, hi = map(float, (time, contact, low, high))
        p = tuple(map(float, position.split(',')))
        if (int(stage) != index or int(updates) != 1 + index * 4 or int(selected) != subject
                or int(behavior) not in range(4) or not 1 <= int(visible) <= 4 or len(p) != 3
                or not all(map(math.isfinite, (t, c, lo, hi, *p)))
                or abs(t - (1 + index * 4) * .1) > .00001 or not 0 <= c <= 1
                or lo > hi or lo < -.002 or (c == 1 and (lo < .0039 or hi > .0242))):
            raise RuntimeError("invalid octopus clock, subject, visibility or seabed contact")
        samples.append(dict(stage=index, updates=int(updates), time=t, subject=subject,
                            behavior=int(behavior), contact=c, gap_min=lo, gap_max=hi, position=p))
    if not any(s['contact'] == 1 for s in samples):
        raise RuntimeError("octopus motion never reaches seabed contact")
    if subject == 3 and not (any(s['contact'] == 0 for s in samples)
                            and any(0 < s['contact'] < 1 for s in samples)
                            and any(s['behavior'] == 3 for s in samples)):
        raise RuntimeError("octopus swim-to-bed transition was not rendered")
    if max(math.dist(samples[0]['position'], s['position']) for s in samples) < .02:
        raise RuntimeError("octopus motion did not advance its actual AI path")
    return samples
