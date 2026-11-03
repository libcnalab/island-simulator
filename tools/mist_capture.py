"""Semantic checks for live, fixed-clock mist recycling captures (no graphics calls)."""
import math
import re


def validate_mist_drift(log, species):
    if species not in ("wetland", "valley"):
        raise ValueError("unsupported mist field")
    rows = re.findall(rf"{species} drift frame=(\d+) clock=(\S+) subject=(\d+) visible=(\d+) puffs=(\S+)", log)
    if len(rows) != 64 or [int(r[0]) for r in rows] != list(range(1, 65)):
        raise RuntimeError("mist capture requires 64 ordered live frames")
    frames = []
    for frame, clock, subject, visible, values in rows:
        try:
            puffs = [tuple(map(float, p.split(','))) for p in values.split(';')]
            clock, subject, visible = float(clock), int(subject), int(visible)
            valid = (len(puffs) == 10 and 0 <= subject < 10 and 0 <= visible <= 10
                     and math.isfinite(clock) and clock >= 0
                     and all(len(p) == 4 and all(map(math.isfinite, p)) and 0 <= p[3] <= 1 for p in puffs))
        except ValueError:
            valid = False
        if not valid:
            raise RuntimeError("invalid mist geometry, opacity, clock or visibility")
        frames.append(dict(frame=int(frame), clock=clock, subject=subject, visible=visible, puffs=puffs))
    events = []
    maximum_step = 0
    for a, b in zip(frames, frames[1:]):
        if a['subject'] != b['subject'] or not math.isclose(b['clock']-a['clock'], .1, abs_tol=.001):
            raise RuntimeError("mist subject or fixed simulation clock changed")
        for index, (p, q) in enumerate(zip(a['puffs'], b['puffs'])):
            maximum_step = max(maximum_step, abs(p[3]-q[3]))
            jump = math.hypot(p[0]-q[0], p[2]-q[2])
            if jump > 8:
                if p[3] > .001 or q[3] > .001:
                    raise RuntimeError("a visible mist bank teleports when recycled")
                events.append(dict(frame=b['frame'], subject=index, jump=jump, opacity_before=p[3], opacity_after=q[3]))
    if not any(e['subject'] == frames[0]['subject'] for e in events):
        raise RuntimeError("capture did not exercise the selected recycling event")
    if maximum_step > .06 or not any(f['visible'] > 0 and max(p[3] for p in f['puffs']) > .1 for f in frames):
        raise RuntimeError("mist fades abruptly or the field is entirely hidden")
    return dict(frames=64, events=events, maximum_opacity_step=maximum_step)
