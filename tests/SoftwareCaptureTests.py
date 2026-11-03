#!/usr/bin/env python3
"""CPU-only preflight checks; never launches a compositor or graphics process."""
from pathlib import Path
from contextlib import ExitStack
import json
import re
import signal
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "tools"))
from run_software_capture import (
    check_configuration, software_environment, validate_graphics_log, run_monitored_capture,
    inspection_arguments, natural_props_seed_arguments, validate_antialiasing_log, validate_monkey_capture_log,
    validate_monkey_tree_stages, validate_monkey_climb_stages, validate_snake_motion_stages,
    validate_goat_motion_stages,
    validate_boar_motion_stages,
    validate_deer_motion_stages,
    validate_deer_contacts,
    validate_octopus_motion,
    validate_bunker_quad_entry, validate_bunker_quad_turn, validate_crocodile_land, validate_elephant_walk, validate_elephant_contacts, validate_trex_contacts,
    validate_spider_motion_stages,
    validate_spider_render_log,
    validate_spider_web_render_log,
    validate_spider_lighting,
    validate_spider_habitat_selection,
    validate_campfire_stages,
    validate_stream_stages,
    validate_resident_stages,
    validate_capture_clock,
    validate_free_motion_stages,
    capture_tide_arguments, validate_capture_tide,
)
from gpu_incident_qualification import (MonitorError, contained_command, artifact_identity,
                                        verify_artifact_content_unchanged)
from run_fullscreen_software import (geometry, worker as fullscreen_worker,
                                     validate_boar_audio_trace, validate_goat_audio_trace,
                                     validate_snake_audio_trace)
from run_fullscreen_software import validate_control_transitions, owned_runtime_log
from run_fullscreen_software import validate_diving_trace, validate_frame_windows
from run_fullscreen_software import validate_cave_trace
from run_fullscreen_software import validate_fire_audio_trace
from run_fullscreen_software import validate_stream_crossing_trace
from run_software_capture import validate_build_configuration
from run_software_capture import main as capture_main
from run_fullscreen_software import main as fullscreen_main


class SoftwareCaptureTests(unittest.TestCase):
    def test_octopus_motion_requires_real_contacts_and_swim_transition(self):
        lines=[]
        for stage in range(16):
            contact=[0,.4,1][min(stage,2)]
            behavior=3 if stage==0 else 0
            lines.append(f"island-simulator: octopus motion stage={stage} updates={1+4*stage} "
                         f"time={(1+4*stage)*.1} subject=3 behavior={behavior} contact={contact} "
                         f"visible=1 gap_min=.004 gap_max=.024 position={stage*.1},-2,0\n")
        good=''.join(lines)
        self.assertEqual(len(validate_octopus_motion(good,3)),16)
        for bad in [''.join(lines[:-1]),good+lines[0],good.replace('updates=5 ','updates=6 ',1),
                    good.replace('visible=1','visible=0',1),good.replace('contact=1','contact=nan',1),
                    good.replace('gap_min=.004','gap_min=-.01',1),good.replace('gap_max=.024','gap_max=.10'),
                    good.replace('contact=0 ','contact=1 ',1),good.replace('contact=0.4 ','contact=1 ',1)]:
            with self.subTest(bad=bad[:100]):
                with self.assertRaises(RuntimeError):validate_octopus_motion(bad,3)

    def test_deer_contacts_check_actual_stance_and_orientation(self):
        lines=[]
        for frame in range(1,65):
            lines.append(f"island-simulator: deer contact frame={frame} heading=0 speed=.2 bed=0 valid=1 visible=1 "
                         +"feet="+";".join(["0,3.5,0,1,0,0"]*4)+"\n")
        good=''.join(lines)
        result=validate_deer_contacts(good)
        self.assertEqual(result['support_pairs'],252)
        self.assertEqual(result['max_stance_slip'],0)
        self.assertEqual(result['max_stance_rotation'],0)
        for bad in [good.replace('frame=32 ','frame=33 ',1),''.join(lines[:-1]),
                    good.replace('bed=0 valid=1','bed=.5 valid=1',1),
                    good.replace('feet=0,3.5,0,1,0,0','feet=.01,3.5,0,1,0,0',1),
                    good.replace('feet=0,3.5,0,1,0,0','feet=0,3.5,0,1,0,.01',1),
                    good.replace('feet=0,3.5,0,1,0,0','feet=0,3.5,0,1,.01,0',1),
                    good.replace('feet=0,3.5,0,1,0,0','feet=nan,3.5,0,1,0,0',1)]:
            with self.subTest(bad=bad[:100]):
                with self.assertRaises(RuntimeError):validate_deer_contacts(bad)

    def test_natural_props_seed_requires_scoped_uint32_inspection(self):
        self.assertEqual(natural_props_seed_arguments(None,"jungle"),[])
        for seed in (0,2,0xffffffff):
            self.assertEqual(natural_props_seed_arguments(seed,"turtle-nest"),
                             ["--natural-props-seed",str(seed)])
        for seed,view in ((-1,"turtle-nest"),(0x100000000,"turtle-nest"),
                          (True,"turtle-nest"),("2","turtle-nest"),(2,"jungle")):
            with self.subTest(seed=seed,view=view),self.assertRaises(ValueError):
                natural_props_seed_arguments(seed,view)

    def test_natural_props_seed_marker_cannot_qualify_another_world(self):
        marker="island-simulator: QA natural-props seed=2 turtle-nest-target=last"
        validate_graphics_log(marker,natural_props_seed=2)
        for log,seed in ((marker,None),(marker,17),("",2),
                         (marker+"\nQA natural-props seed=17 turtle-nest-target=last",2)):
            with self.subTest(log=log,seed=seed),self.assertRaises(RuntimeError):
                validate_graphics_log(log,natural_props_seed=seed)

    def test_capture_clock_rejects_wall_time_catchup(self):
        good="capture-profile frames=4 updates=4"
        self.assertEqual(validate_capture_clock(good)["frames"],4)
        self.assertEqual(validate_capture_clock(good+"\n"+good)["updates"],4)
        for bad in ("", "capture-profile frames=0 updates=0",
                    "capture-profile frames=4 updates=49",
                    good+"\ncapture-profile frames=8 updates=8"):
            with self.subTest(log=bad),self.assertRaises(RuntimeError):
                validate_capture_clock(bad)

    def test_stream_crossing_requires_both_banks_channel_and_shared_support(self):
        good=[(-110,5.0,-34+i*.55,-4+i*.55,abs(-4+i*.55),0,0) for i in range(14)]
        result=validate_stream_crossing_trace(good)
        self.assertGreater(result["nearest"],0)
        for index,replacement in ((0,(-110,5,-1,0,0,0,0)),
                                  (7,(-110,5,-30,.1,.1,.2,0)),
                                  (7,(-110,5,-30,.1,.1,0,1))):
            bad=list(good);bad[index]=replacement
            with self.assertRaises(RuntimeError):validate_stream_crossing_trace(bad)
        with self.assertRaises(RuntimeError):validate_stream_crossing_trace(good[:4])
        reversed_route=list(good);reversed_route[8]=(-110,5,reversed_route[7][2]-.2,
            reversed_route[8][3],reversed_route[8][4],0,0)
        with self.assertRaises(RuntimeError):validate_stream_crossing_trace(reversed_route)

    def test_fire_audio_trace_requires_volume_collision_mute_resume_and_range(self):
        good=[(1,0,1,.26,2.4,.04,4)]*5+[(1,1,1,0,2.4,.04,4)]*25
        good+=[(1,0,1,.26,2.4,.04,4)]*5+[(1,0,0,.001,27,25,4)]
        validate_fire_audio_trace(good)
        for index,point in ((0,(0,0,1,.26,2.4,.04,4)),(7,(1,1,1,.2,2.4,.04,4)),
                            (9,(1,1,0,0,2.4,.04,4)),(35,(1,0,1,.2,27,25,4)),
                            (35,(1,0,0,.001,2.4,.04,4)),(0,(1,0,1,.26,2.4,-.1,4)),
                            (0,(1,0,1,.26,2.4,.04,7)),(2,(1,0,1,float("nan"),2.4,.04,4))):
            bad=list(good);bad[index]=point
            with self.assertRaises(RuntimeError):validate_fire_audio_trace(bad)
        for bad in ([],good[:5],[(1,0,1,0,2.4,.04,4)]*5+good[5:],good[:30]+[good[-1]]):
            with self.assertRaises(RuntimeError):validate_fire_audio_trace(bad)
    def test_campfire_animation_stages(self):
        good="\n".join(f"campfire motion stage={i} updates={(i+1)*8} time={(i+1)*.8:.1f} draws=2 effect-triangles=66" for i in range(4))
        self.assertEqual(len(validate_campfire_stages(good)),4)
        for bad in ("",good+"\n"+good,good.replace("stage=1","stage=0"),good.replace("time=0.8","time=nan"),
                    good.replace("updates=8 ","updates=9 "),good.replace("draws=2","draws=0"),
                    good.replace("triangles=66","triangles=660")):
            with self.assertRaises(RuntimeError):validate_campfire_stages(bad)
    def test_stream_animation_stages(self):
        good="\n".join(f"stream motion stage={i} frames={(i+1)*8} time={(i+1)*.8:.1f} draws=1 triangles=6080" for i in range(4))
        self.assertEqual(len(validate_stream_stages(good)),4)
        for bad in ("",good+"\n"+good,good.replace("stage=1","stage=0"),
                    good.replace("time=0.8","time=nan"),good.replace("frames=8 ","frames=9 "),
                    good.replace("draws=1","draws=0"),good.replace("triangles=6080","triangles=6081")):
            with self.assertRaises(RuntimeError):validate_stream_stages(bad)
    def test_resident_gait_stages_and_corruptions(self):
        rows=[]
        for stage in range(8):
            distance=max(0,stage-2)*.35
            speed=0 if stage<3 or stage==7 else .78
            swing=-.24 if stage%2==0 else .24
            rows.append(f"resident motion stage={stage} updates={(stage+1)*8} visible=1 "
                        f"position={42-distance},23.8,{-12+distance} speed={speed} mode={1 if stage<3 else 0} "
                        f"feet={41.9-distance},23.8,{-12+distance},1;{42.1-distance},23.8,{-11.9+distance},0 "
                        f"wrists={41.8-distance+.12*(-1)**stage},24.3,{-12+distance};"
                        f"{42.2-distance-.12*(-1)**stage},24.3,{-12+distance} "
                        f"arm-swing={swing},{-swing} "
                        "triangles=3638/24")
        good="\n".join(rows)
        result=validate_resident_stages(good)
        self.assertGreater(result["displacement"],1)
        self.assertGreater(min(result["arm_swing_ranges"]),.15)
        for bad in ("",good+"\n"+good,good.replace("stage=1","stage=0"),
                    good.replace("updates=8 ","updates=9 "),good.replace("visible=1","visible=0"),
                    good.replace("triangles=3638/24","triangles=10/24"),
                    good.replace("triangles=3638/24","triangles=3638/0"),
                    good.replace("wrists=", "wrists=nan,", 1),
                    good.replace("arm-swing=-0.24,0.24", "arm-swing=nan,0.24", 1),
                    good.replace("position=42.0,23.8,-12.0","position=nan,23.8,-12.0")):
            with self.assertRaises(RuntimeError):validate_resident_stages(bad)
    def test_capture_tide_phase_bounds_and_clock_proof(self):
        self.assertEqual(capture_tide_arguments(None), [])
        for phase,level in ((0,0),(.25,.18),(.75,-.18)):
            self.assertEqual(capture_tide_arguments(phase), ["--capture-tide-phase",str(phase)])
            log=(f"capture tide frame=1 phase={phase} level={level}\n"
                 f"capture tide frame=4 phase={phase} level={level}\n"
                 "capture-profile frames=4 updates=4")
            self.assertEqual(validate_capture_tide(log,phase)["level"], level)
            for bad in (log.replace('frame=1','frame=2'),log.replace('frame=4','frame=3'),
                        log+f"\ncapture tide frame=4 phase={phase} level={level}",
                        log.replace(f'level={level}','level=nan'),log.replace(f'level={level}','level=2'),
                        log.replace(f'phase={phase}','phase=broken'),log.replace(f'level={level}','level=broken'),
                        log.replace('updates=4','updates=8')):
                with self.assertRaises(RuntimeError):validate_capture_tide(bad,phase)
            with self.assertRaises(RuntimeError):validate_capture_tide(log,None)
        for phase in (-.1,1,float('nan'),float('inf')):
            with self.assertRaises(ValueError):capture_tide_arguments(phase)
        self.assertIsNone(validate_capture_tide('capture-profile frames=4 updates=4',None))
        with self.assertRaises(RuntimeError):validate_capture_tide('capture-profile frames=4 updates=4',.25)

    def test_live_fish_and_bird_sequence_rejects_frozen_or_invalid_subjects(self):
        for species in ("fish", "bird"):
            rows = [f"island-simulator: {species} motion stage={i} updates={(i+1)*2} "
                    f"time={(i+1)*.2:.1f} near=2 position={i*.1},-0.7,3 "
                    f"heading={i*.01} bank=0.1 animation={(-1)**i*.5} clearance=0.8 tier=0"
                    for i in range(16)]
            good = "\n".join(rows)
            result = validate_free_motion_stages(good, species)
            self.assertAlmostEqual(result["travel"], 1.5)
            if species == "fish":
                with self.assertRaises(RuntimeError):
                    validate_free_motion_stages(good.replace("bank=0.1", "bank=0.3"),species)
            frozen = re.sub(r"position=\S+", "position=0,-0.7,3", good)
            for bad in ("", "\n".join(rows[:-1]), good+"\n"+rows[0],
                        good.replace("stage=1 ", "stage=0 "), good.replace("tier=0", "tier=1"),
                        good.replace("near=2", "near=0"), good.replace("clearance=0.8", "clearance=-0.1"),
                        good.replace("time=0.2", "time=nan"), good.replace("updates=2 ", "updates=3 "),
                        good.replace("bank=0.1", "bank=inf"), frozen,
                        re.sub(r"animation=\S+", "animation=0.2", good),
                        good.replace("heading=0.0 ", "heading=2.0 ")):
                with self.subTest(species=species,bad=bad[:100]),self.assertRaises(RuntimeError):
                    validate_free_motion_stages(bad,species)

    def test_cave_traversal_and_corruptions(self):
        points=[(0,0,-8,0,0),(0,.49,19.65,0,0),(0,1.5,13.5,1,0),
                (0,.3375,13.5,0,0),(0,0,-6,0,0)]
        samples=[p for p in points for _ in range(5)]
        stages=dict(zip(("start","back","ceiling","landed","exit"),(4,9,14,19,24)))
        validate_cave_trace(samples,stages)
        for index,point in ((0,(0,0,-8,0,1)),(4,(0,0,0,0,0)),(9,(0,.5,21,0,0)),
                            (14,(0,7,13.5,1,0)),(14,(0,1.5,13.5,0,0)),
                            (14,(0,1.7,19.65,1,0)),
                            (19,(0,1.7,13.5,0,0)),(24,(0,0,4,0,0)),
                            (2,(float("nan"),0,0,0,0)),(3,(2,0,0,0,0))):
            bad=list(samples);bad[index]=point
            with self.subTest(index=index,point=point),self.assertRaises(RuntimeError):
                validate_cave_trace(bad,stages)
        for bad_stages in ({},dict(stages,back=3),dict(stages,exit=25),dict(stages,start=-1)):
            with self.assertRaises(RuntimeError):validate_cave_trace(samples,bad_stages)
        with self.assertRaises(RuntimeError):validate_cave_trace(samples[:5],stages)

    def test_runtime_build_configuration_matches_tree(self):
        for mode in ("Debug", "Release", "RelWithDebInfo", ""):
            cache = f"CMAKE_BUILD_TYPE:STRING={mode}\n"
            log = f"island-simulator: build configuration {mode}\n"
            self.assertEqual(validate_build_configuration(cache, log), mode)
            for bad in ("", log + log, log.replace(mode or "configuration", "unexpected")):
                with self.subTest(mode=mode, bad=bad), self.assertRaises(RuntimeError):
                    validate_build_configuration(cache, bad)
        with self.assertRaises(RuntimeError):
            validate_build_configuration("", "island-simulator: build configuration Release\n")

    def test_live_frame_accounting_and_corruptions(self):
        good = ("island-simulator: frame-window seconds=5.000 frames=150 fps=30.000 "
                "updates-per-frame=2.000 update-ms-per-frame=8.000 draw-submit-ms-per-frame=4.000 "
                "wildlife-ms-per-update(spider,goat,snake,monkey)=1.000,0.500,1.000,0.500 "
                "flight=1 camera=1,60,3")
        self.assertEqual(validate_frame_windows(good)[0]["fps"], 30)
        for bad in ("", good.replace("fps=30.000", "fps=60.000"),
                    good.replace("seconds=5.000", "seconds=nan"),
                    good.replace("frames=150", "frames=0"),
                    good.replace("updates-per-frame=2.000", "updates-per-frame=0"),
                    good.replace("1.000,0.500,1.000,0.500", "4,4,4,4"),
                    good.replace("draw-submit-ms-per-frame=4.000", "draw-submit-ms-per-frame=-1"),
                    good.replace("flight=1", "flight=2"), good.replace("camera=1,60,3", "camera=1,inf,3")):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_frame_windows(bad)

    def test_diving_trace_and_rejections(self):
        good=[(0,.58,-270,0,0),(0,-.5,-270,.5,0),(0,-1,-270,1,0),(0,.58,-270,0,0)]
        validate_diving_trace(good)
        for level in (-.18,.18):
            validate_diving_trace([(x,y+level,z,depth,level) for x,y,z,depth,_ in good])
        for bad in ([],good[:3],good[1:],[(0,.58,-270,0,0)]*4,
                    [good[0],(0,-1,-270,0,0),*good[2:]],
                    [good[0],(0,float('nan'),-270,0,0),*good[2:]],
                    [good[0],(0,5,-270,0,0),*good[2:]],
                    [good[0],(0,-1,-270,1,float('nan')),*good[2:]],
                    [good[0],(0,-1,-270,1,.18),*good[2:]],
                    [good[0],(0,-1,-270),*good[2:]]):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):
                validate_diving_trace(bad)

    def test_boar_capture_clock_and_corruptions(self):
        good = ''.join(f"island-simulator: boar motion stage={i} updates={(i+1)*8} time={(i+1)*.8:.1f} "
                       f"visible=1 position={i},2,3 speed=0 phase=0 rooting=0.5 tier=0\n" for i in range(8))
        self.assertEqual(len(validate_boar_motion_stages(good)["samples"]), 8)
        for bad in ("", good+good, good.replace("stage=7", "stage=6"),
                    good.replace("updates=8 ", "updates=9 "), good.replace("time=0.8", "time=nan"),
                    good.replace("visible=1", "visible=0"), good.replace("1,2,3", "1,inf,3"),
                    good.replace("speed=0", "speed=-1"), good.replace("phase=0", "phase=7"),
                    good.replace("rooting=0.5", "rooting=2"),
                    good.replace("tier=0", "tier=1"), good.replace("tier=0", "tier=2"),
                    good.replace(" tier=0", ""),
                    re.sub(r"position=\d,2,3", "position=1,2,3", good)):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_boar_motion_stages(bad)

    def test_trex_contacts_include_all_three_toes(self):
        good=''
        for i in range(1,65):
            air=i%16 in (8,9,10,11,12,13)
            left=f"0,{.12 if air else .07},0,{0 if air else 1},.3,.02,0,.4,.02,.1,.3,.02,.2"
            good+=f"island-simulator: trex contact frame={i} heading={i*.022} speed=0 behavior=6 visible=1 triangles=12212 feet={left};1,.07,0,1,1.3,.02,0,1.4,.02,.1,1.3,.02,.2\n"
        self.assertGreater(validate_trex_contacts(good)["stopped_swing_samples"],4)
        for bad in ('',good+good,good.replace('frame=64','frame=0'),good.replace('triangles=12212','triangles=1'),
                    good.replace('heading=0.044 speed=0 behavior=6 visible=1 triangles=12212 feet=0,0.07,0,1,.3',
                                 'heading=0.044 speed=0 behavior=6 visible=1 triangles=12212 feet=0,0.07,0,1,.5'),
                    good.replace('1.3,.02,.2','nan,.02,.2')):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):validate_trex_contacts(bad)

    def test_elephant_contacts_reject_world_slip_and_stationary_turn(self):
        good=''
        for i in range(1,65):
            air=i%8 in (4,5,6)
            good+=f"island-simulator: elephant contact frame={i} heading={i*.025} speed=0 behavior=5 visible=1 triangles=8493 feet=0,{.05 if air else 0},0,{0 if air else 1};1,0,0,1;0,0,1,1;1,0,1,1\n"
        self.assertGreater(validate_elephant_contacts(good,turning=True)["stopped_swing_samples"],4)
        for bad in ('',good+good,good.replace('frame=64','frame=63'),good.replace('visible=1','visible=0'),
                    good.replace('triangles=8493','triangles=18000'),good.replace('heading=0.05','heading=nan'),
                    good.replace('heading=0.05 speed=0 behavior=5 visible=1 triangles=8493 feet=0,0,0,1',
                                 'heading=0.05 speed=0 behavior=5 visible=1 triangles=8493 feet=.1,0,0,1'),
                    good.replace('0,0.05,0,0','0,0,0,1')):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):validate_elephant_contacts(bad,turning=True)

    def test_elephant_walk_requires_drawn_contact_and_real_transition(self):
        good=''.join(f"island-simulator: elephant walk stage={i} frame={(i+1)*8} "
                     f"position={0 if i<4 else (i-3)*.3},0,0 speed={0 if i<4 else .5} "
                     f"behavior={1 if i<4 else 0} planted=3 sole-error=0,0\n" for i in range(8))
        self.assertGreater(validate_elephant_walk(good)["travel"],.5)
        for bad in ('',good+good,good.replace('frame=64','frame=63'),
                    good.replace('sole-error=0,0','sole-error=-.08,.04'),
                    good.replace('sole-error=0,0','sole-error=nan,0'),
                    good.replace('planted=3','planted=1'),good.replace('speed=0.5','speed=0'),
                    good.replace('behavior=0','behavior=1')):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):validate_elephant_walk(bad)

    def test_crocodile_land_rejects_rest_pedalling_and_missing_crawl(self):
        good=''.join(f"island-simulator: crocodile land stage={i} frame={(i+1)*10} "
                     f"position={0 if i<8 else (i-7)*.3},0,0 heading=0 speed={0 if i<8 else .34} "
                     f"phase={0 if i<8 else i*.2} submerged=0 behavior={0 if i<8 else 1} near=1\n" for i in range(16))
        self.assertGreater(validate_crocodile_land(good)["travel"],.5)
        for bad in ('',good+good,good.replace('frame=160','frame=159'),
                    good.replace('phase=0 ','phase=nan '),good.replace('near=1','near=0'),
                    good.replace('stage=1 frame=20 position=0,0,0 heading=0 speed=0 phase=0',
                                 'stage=1 frame=20 position=0,0,0 heading=0 speed=0 phase=1'),
                    good.replace('stage=1 frame=20 position=0,0,0 heading=0',
                                 'stage=1 frame=20 position=0,0,0 heading=1'),
                    good.replace('behavior=1','behavior=0')):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):validate_crocodile_land(bad)

    def test_bunker_turn_requires_real_entry_shunts_and_forward_escape(self):
        rows=[]
        for i in range(16):
            phase=0 if i<5 else 1 if i<9 else 2 if i<15 else 3
            segment=0 if i<5 else i-5 if i<9 else 6
            depth=-1+i*7 if i<5 else 31.5 if i<9 else 29-(i-9)*6 if i<15 else -4.3
            speed=-.7 if phase==1 else 1.2
            rows.append(f"island-simulator: bunker turn stage={i} frame={(i+1)*50} phase={phase} segment={segment} "
                        f"across=0 depth={depth} speed={speed} floor-gap=0.05 walls=0 overturned=0 "
                        f"complete={int(phase==3)} blocked-samples=0 overturned-samples=0 max-floor-gap=0.12 camera-blocked=0 camera-occluded=0\n")
        good=''.join(rows)
        self.assertEqual(len(validate_bunker_quad_turn(good)["samples"]),16)
        for bad in ("",good+good,good.replace("frame=800", "frame=799"),
                    good.replace("depth=-4.3", "depth=1"),good.replace("speed=-0.7", "speed=0"),
                    good.replace("floor-gap=0.05", "floor-gap=nan"),
                    good.replace("walls=0", "walls=1"),good.replace("blocked-samples=0", "blocked-samples=1"),
                    good.replace("overturned-samples=0", "overturned-samples=1"),
                    good.replace("max-floor-gap=0.12", "max-floor-gap=6"),good.replace("complete=1", "complete=0"),good.replace("camera-blocked=0", "camera-blocked=1"),
                    good.replace("camera-occluded=0", "camera-occluded=1")):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):
                validate_bunker_quad_turn(bad)

    def test_bunker_quad_entry_rejects_original_airborne_false_pass(self):
        good = "island-simulator: quad QA shelter: bunker, site bunker, depth 2.69 m, floor 17.89, body 18.54, walls clear\n"
        self.assertAlmostEqual(validate_bunker_quad_entry(good)["body_floor_gap"], .65)
        original = "island-simulator: quad QA shelter: bunker, site open, depth -5.71518 m, floor 17.4266, body 24.1712, walls clear\n"
        for bad in (original, "", good+good, good.replace("site bunker", "site open"),
                    good.replace("depth 2.69", "depth -1"), good.replace("floor 17.89", "floor nan"),
                    good.replace("body 18.54", "body 24"), good.replace("body 18.54", "body 17.9"),
                    good.replace("walls clear", "walls touching")):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_bunker_quad_entry(bad)

    def test_deer_capture_rejects_frozen_throttled_or_invalid_motion(self):
        good = ''.join(f"island-simulator: deer motion stage={i} updates={(i+1)*8} time={(i+1)*.8:.1f} "
                       f"visible=1 position={i},2,3 speed=0.78 phase=0 grazing=0 tier=0\n" for i in range(8))
        samples = validate_deer_motion_stages(good)["samples"]
        self.assertEqual(len(samples), 8)
        self.assertEqual(samples[0]["grazing"], 0)
        for bad in ("", good+good, good.replace("stage=7", "stage=6"),
                    good.replace("updates=8 ", "updates=9 "), good.replace("time=0.8", "time=nan"),
                    good.replace("visible=1", "visible=5"), good.replace("1,2,3", "1,inf,3"),
                    good.replace("speed=0.78", "speed=3.2"), good.replace("phase=0", "phase=7"),
                    good.replace("grazing=0", "grazing=2"), good.replace("tier=0", "tier=1"),
                    re.sub(r"position=\d,2,3", "position=1,2,3", good)):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_deer_motion_stages(bad)

    def test_spider_bright_camera_selects_existing_habitat(self):
        rows = ''.join(f"spider habitat candidate={i} transmission={0.9 if i in (4, 7) else 0.1}\n"
                       for i in range(1000))
        good = rows + "spider habitat selected=4\n"
        self.assertEqual(validate_spider_habitat_selection(good)["subject"], 4)
        for bad in ("", good.replace("selected=4", "selected=7"),
                    good.replace("selected=4", "selected=1000"), good+"spider habitat selected=4\n",
                    good.replace("candidate=999", "candidate=998"),
                    good.replace("transmission=0.1", "transmission=nan", 1),
                    good.replace("transmission=0.1", "transmission=1.1", 1),
                    good.replace("transmission=0.1", "transmission=bad", 1)):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_spider_habitat_selection(bad)

    def test_spider_lighting_transfer_and_rejections(self):
        for transmission in (0, 0.5, 1):
            tint = ','.join(str(a + (1-a)*transmission) for a in (0.54, 0.60, 0.67))
            good = (f"spider lighting ground-transmission={transmission} "
                    f"body-transmission={transmission} body-tint={tint}\n")
            self.assertEqual(validate_spider_lighting(good)["bodyTransmission"], transmission)
        good = "spider lighting ground-transmission=0 body-transmission=0 body-tint=0.54,0.60,0.67\n"
        for bad in ("", good+good, good.replace("ground-transmission=0", "ground-transmission=nan"),
                    good.replace("body-transmission=0", "body-transmission=1.1"),
                    good.replace("0.54,0.60,0.67", "0.54,0.60"),
                    good.replace("0.54,0.60,0.67", "0.54,inf,0.67"),
                    good.replace("0.54,0.60,0.67", "0.54,0.60,0.32"),
                    good.replace("ground-transmission=0", "ground-transmission=bad")):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_spider_lighting(bad)

    def test_spider_render_requires_grounding(self):
        good = ("island-simulator: spider render proof: spiders 1/1000 visible, 2 draw; contact-shadows=1\n"
                "spider setae loaded Content/wildlife/spider-setae.png; 256x256, 9 mips, 349524 RGBA8 bytes\n"
                "near spiders use tiered fitted eyes with upper-forward catchlights\n"
                "near spider abdomens use a tapered muted dorsal lance in the existing setae draw\n")
        validate_spider_render_log(good)
        for bad in ("", good.replace("1/1000", "0/1000"), good.replace("1/1000", "1001/1000"),
                    good.replace("2 draw", "0 draw"), good.replace("contact-shadows=1", "contact-shadows=0"),
                    good.replace("contact-shadows=1", "contact-shadows=201"),
                    good.replace(" contact-shadows=1", ""), good.replace("spider setae loaded", "missing"),
                    good.replace("tiered fitted eyes", "uniform eyes"),
                    good.replace("tapered muted dorsal lance", "plain abdomen")):
            with self.assertRaises(RuntimeError):
                validate_spider_render_log(bad)

    def test_spider_web_render_requires_bounded_live_batch(self):
        good="island-simulator: spider-web render proof: webs 1/6 visible, strands 434, 1 draw\n"
        self.assertEqual(validate_spider_web_render_log(good),
                         {"visible":1,"total":6,"strands":434,"draws":1})
        self.assertEqual(validate_spider_web_render_log(good+good)["strands"],434)
        for bad in ("",good+good.replace("434","868"),good.replace("1/6","0/6"),good.replace("1/6","7/6"),
                    good.replace("434","433"),good.replace("1 draw","2 draw")):
            with self.subTest(bad=bad),self.assertRaises(RuntimeError):
                validate_spider_web_render_log(bad)

    def test_spider_live_motion_and_corruptions(self):
        def fixture(frozen=False):
            rows = []
            for i in range(8):
                x = 0 if frozen else i*.025
                feet = ';'.join(f"{x+(.08 if leg<4 else -.08)},2,{(leg%4-1.5)*.05},1" for leg in range(8))+';'
                rows.append(f"island-simulator: spider motion stage={i} updates={(i+1)*8} time={(i+1)*.8:.1f} "
                            f"visible=1 position={x},2,0 heading={i*.2:.1f} speed=0.035 feet={feet}")
            return '\n'.join(rows)
        good = fixture()
        self.assertEqual(len(validate_spider_motion_stages(good)["feet"]), 8)
        for bad in (fixture(True), good.split('\n', 1)[1], good+'\n'+good.split('\n')[0],
                    good.replace("stage=1", "stage=0"), good.replace("updates=8 ", "updates=9 "),
                    good.replace("time=0.8", "time=nan"), good.replace("visible=1", "visible=0"),
                    good.replace("speed=0.035", "speed=0.1"), good.replace("heading=0.2", "heading=2.0"),
                    good.replace("heading=0.2", "heading=nan"),
                    re.sub(r"position=\S+", "position=nan,2,0", good),
                    good.replace(",1;", ",0;"), good.replace(",1;", ",2;"),
                    re.sub(r"feet=.*", "feet=0,2,0,1;", good)):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_spider_motion_stages(bad)

    def test_goat_live_motion_stages_and_corruptions(self):
        rows = [f"island-simulator: goat motion stage={i} updates={(i+1)*8} time={(i+1)*.8:.1f} "
                f"visible=3 position={i*.2},10,20 speed=0.65 phase={i*.3} grazing={.8 if i==0 else 0}"
                for i in range(8)]
        good = "\n".join(rows)
        self.assertEqual(len(validate_goat_motion_stages(good)["positions"]), 8)
        for bad in ("\n".join(rows[:-1]), good+"\n"+rows[0], good.replace("stage=1", "stage=0"),
                    good.replace("updates=8 ", "updates=9 "), good.replace("visible=3", "visible=0"),
                    good.replace("speed=0.65", "speed=nan"), good.replace("phase=0.3", "phase=inf"),
                    good.replace("phase=0.3", "phase=9"), good.replace("position=0.2,10,20", "position=50,10,20"),
                    good.replace("grazing=0.8", "grazing=0"),
                    re.sub(r"position=\S+", "position=0,10,20", good),
                    re.sub(r"phase=\S+", "phase=0", good)):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_goat_motion_stages(bad)
    def test_runtime_log_uses_namespace_identity_not_filename_pid(self):
        evidence = Mock()
        matching, unrelated = Mock(), Mock()
        matching.read_text.return_value = "timestamp runtime-start pid=504958 main-tid=504958 namespace-pid=44 namespace-main-tid=44"
        unrelated.read_text.return_value = "timestamp runtime-start pid=504959 main-tid=504959 namespace-pid=441 namespace-main-tid=441"
        evidence.glob.return_value = [unrelated, matching]
        self.assertIs(owned_runtime_log(evidence, 44)[0], matching)
        for logs in ([], [unrelated], [matching, matching]):
            evidence.glob.return_value = logs
            with self.assertRaises(RuntimeError):
                owned_runtime_log(evidence, 44)

    def test_persistent_control_transitions_require_complete_finite_ordered_states(self):
        lines = [f"timestamp island-simulator: control-transition event={event} submitted-frames={i+1} "
                 f"camera=1,2,3 yaw=0 pitch=0 flight-active={int(event == 0)} "
                 f"flight-position=1,2,3 fullscreen-requested={int(event == 3)} finite=1"
                 for i, event in enumerate([3, 4, 3, 4, 0, 1])]
        valid = "\n".join(lines)
        self.assertEqual(len(validate_control_transitions(valid, True)), 6)
        self.assertEqual(len(validate_control_transitions("\n".join(lines[:4]))), 4)
        self.assertEqual(len(validate_control_transitions("\n".join(lines[:2]), False, 1)), 2)
        self.assertEqual(validate_control_transitions("", False, 0), [])
        with self.assertRaises(RuntimeError):
            validate_control_transitions("", False, 3)
        for invalid in ("", "\n".join(lines[:-1]), valid + "\n" + lines[-1],
                        "\n".join(reversed(lines)), valid.replace("camera=1,2,3", "camera=nan,2,3"),
                        valid.replace("camera=1,2,3", "camera=1,2"), valid.replace("finite=1", "finite=0"),
                        valid.replace("flight-active=1", "flight-active=0"),
                        valid.replace("submitted-frames=6", "submitted-frames=1"),
                        valid.replace("flight-position=1,2,3", "flight-position=2,2,3")):
            with self.assertRaises(RuntimeError):
                validate_control_transitions(invalid, True)

    def test_snake_motion_requires_connected_live_sequence(self):
        def fixture(moving=True):
            return '\n'.join(
                f"island-simulator: snake motion stage={stage} updates={(stage+1)*8} time={(stage+1)*.8} visible=1 speed=0.38 phase=1 body="
                + ''.join(f"{(stage*.1 if moving else 0)-j*.055:.6f},2,0;" for j in range(41))
                for stage in range(4))
        good = fixture()
        self.assertEqual(len(validate_snake_motion_stages(good)), 4)
        for invalid in (fixture(False), good.replace('stage=2', 'stage=1'),
                        good.replace('updates=16', 'updates=17'), good.replace('time=0.8', 'time=nan'),
                        good.replace('time=0.8', 'time=0.9'),
                        good+'\nisland-simulator: snake motion stage=broken',
                        good.replace('visible=1', 'visible=0'),
                        good.replace('speed=0.38', 'speed=nan'),
                        good.replace('0.000000,2,0;', 'nan,2,0;'),
                        good.replace('0.000000,2,0;', '10,2,0;'),
                        good.split('\n', 1)[1]):
            with self.assertRaises(RuntimeError):
                validate_snake_motion_stages(invalid)

    def test_boar_audio_trace_rejects_missing_playback_mute_and_budget_failures(self):
        valid = [(1, 0, 2, 5, 3)] * 5 + [(1, 1, 2, 4, 0)] * 24 + [(1, 0, 3, 5, 3)] * 5
        validate_boar_audio_trace(valid)
        for index, state in ((0, (0, 0, 2, 5, 3)), (5, (1, 1, 3, 4, 0)),
                             (5, (1, 1, 2, 5, 3)), (0, (1, 0, 2, 7, 3)),
                             (0, (1, 0, 2, 5, 10)),
                             (-1, (1, 0, 2, 4, 0))):
            invalid = list(valid)
            invalid[index] = state
            with self.assertRaises(RuntimeError):
                validate_boar_audio_trace(invalid)
        for invalid in ([], valid[:20], [(1, 0, 3, 4, 0)] * 40):
            with self.assertRaises(RuntimeError):
                validate_boar_audio_trace(invalid)

    def test_snake_audio_trace_rejects_missing_playback_mute_and_budget_failures(self):
        valid=[(1,0,0,5,4,1)]*4+[(1,1,0,4,0,1)]*24+[(1,0,0,5,4,2)]*4
        validate_snake_audio_trace(valid)
        for index,state in ((0,(0,0,0,5,4,1)),(4,(1,1,0,4,0,2)),
                            (4,(1,1,0,5,4,1)),(0,(1,0,0,7,4,1)),
                            (0,(1,0,0,5,10,1)),
                            (-1,(1,0,0,4,0,1))):
            invalid=list(valid);invalid[index]=state
            with self.assertRaises(RuntimeError):validate_snake_audio_trace(invalid)
        for invalid in ([],valid[:20],[(1,0,0,4,0,2)]*40):
            with self.assertRaises(RuntimeError):validate_snake_audio_trace(invalid)

    def test_goat_audio_trace_rejects_missing_playback_mute_and_budget_failures(self):
        valid=[(1,0,0,5,6,0,2)]*4+[(1,1,0,4,0,0,2)]*24+[(1,0,0,5,6,0,3)]*4
        validate_goat_audio_trace(valid)
        for index,state in ((0,(0,0,0,5,6,0,2)),(4,(1,1,0,4,0,0,3)),
                            (4,(1,1,0,5,6,0,2)),(0,(1,0,0,7,6,0,2)),
                            (0,(1,0,0,5,10,0,2)),
                            (-1,(1,0,0,4,0,0,2))):
            invalid=list(valid);invalid[index]=state
            with self.assertRaises(RuntimeError):validate_goat_audio_trace(invalid)
        for invalid in ([],valid[:20],[(1,0,0,4,0,0,3)]*40):
            with self.assertRaises(RuntimeError):validate_goat_audio_trace(invalid)

    def test_fullscreen_geometry_rejects_missing_or_invalid_dimensions(self):
        self.assertEqual(geometry("X=-20\nY=10\nWIDTH=960\nHEIGHT=540\n")["WIDTH"], 960)
        for text in ("", "X=0\nY=0\nWIDTH=1\n", "X=0\nY=0\nWIDTH=0\nHEIGHT=540\n"):
            with self.assertRaises(RuntimeError):
                geometry(text)

    def test_fullscreen_worker_refuses_unsupervised_launch_before_process_creation(self):
        with patch.dict("os.environ", {}, clear=True), patch("subprocess.Popen") as launch:
            with self.assertRaisesRegex(RuntimeError, "supervised"):
                fullscreen_worker(Path("/build"), Path("/evidence"), Path("/wm"))
            launch.assert_not_called()

    cache = "CNA_GRAPHICS_RENDERER:STRING=OPENGL33\nCNA_ROOT_DIR:PATH=/dependency\n"

    def test_climb_requires_whole_cycle_same_tree_ascent_descent_and_release(self):
        times = [0, 5, 10.1, 18.5, 24, 29.1, 38.9, 0]
        heights = [5, 5.2, 5.3, 7.3, 6.4, 5.3, 5.01, 5]
        rows = [f"monkey climb stage={i} subject=2 tree={12 if i<7 else -1} "
                f"time={t} active={int(i<7)} centre-y={heights[i]}" for i,t in enumerate(times)]
        valid = "\n".join(rows)
        self.assertEqual(len(validate_monkey_climb_stages(valid)), 8)
        for invalid in ["\n".join(rows[:-1]), valid.replace("stage=2 subject=2", "stage=2 subject=3"),
                        valid.replace("time=18.5", "time=11.0"), valid.replace("centre-y=7.3", "centre-y=5.3"),
                        valid.replace("centre-y=5.01", "centre-y=6.01"),
                        valid.replace("stage=7 subject=2 tree=-1", "stage=7 subject=2 tree=12")]:
            with self.assertRaises(RuntimeError):
                validate_monkey_climb_stages(invalid)

    def test_tree_sequence_requires_same_subject_tree_clock_and_applied_contacts(self):
        def ease(t):
            t = max(0, min(1, t/.75))
            return t*t*(3-2*t)
        rows = []
        for index, t in enumerate([.1,1.4,2.2,3.4,4.9,5.5,5.9]):
            rows.append(f"monkey tree stage={index} subject=4 tree=20 elapsed={t} clock={100+t} "
                        f"weights={ease(t-1)*ease(6-t-.75)},{ease(t-1.75)*ease(6-t)}")
        rows.append("monkey tree stage=7 subject=4 tree=-1 elapsed=0 clock=106.1 weights=0,0")
        valid = "\n".join(rows)
        self.assertEqual(len(validate_monkey_tree_stages(valid)), 8)
        for invalid in ("\n".join(rows[:-1]), valid+"\n"+rows[0],
                        "\n".join([rows[1], rows[0], *rows[2:]]),
                        valid.replace("stage=1 subject=4", "stage=1 subject=2"),
                        valid.replace("stage=1 subject=4 tree=20", "stage=1 subject=4 tree=21"),
                        valid.replace("clock=103.4", "clock=101.0"),
                        valid.replace("weights=1,1", "weights=0,0"),
                        valid.replace("clock=106.1", "clock=105.9"),
                        valid.replace("elapsed=1.4", "elapsed=1.8"),
                        valid.replace("clock=101.4", "clock=1e999"),
                        valid.replace("weights=1,1", "weights=1.1,1"),
                        valid.replace("stage=7 subject=4 tree=-1", "stage=7 subject=4 tree=20")):
            with self.subTest(invalid=invalid), self.assertRaises(RuntimeError):
                validate_monkey_tree_stages(invalid)

    def test_monkey_close_subject_must_still_be_submitted_at_capture(self):
        valid = ("island-simulator: monkey capture visible 2; near 1; draws 4; subject 55,19,17\n"
                 "monkey fur loaded Content/wildlife/monkey-fur.png; 512x512, 10 mips, 1398100 RGBA8 bytes\n"
                 "monkey fur uses one hidden underside/rear UV seam instead of mirrored crown and dorsal folds\n"
                 "near monkey tails keep a compact muscular hook and thirty-six raised fur clumps\n"
                 "near monkey faces use paired fur cheek ruffs around the skin mask\n"
                 "near monkey faces use integrated sculpted supraorbital relief and warmer modelled skin\n"
                 "near monkey faces use a fitted dark nose pad and raised readable corneas\n"
                 "near monkey eyes use compact recessed corneas, fitted short lids, and bilateral inner catchlights\n"
                 "monkey face skin uses muted brown palettes and a recessed perimeter shade\n"
                 "near monkeys use a compact forward-set cranium, reduced cheek ruffs, and fitted ears\n"
                 "near monkeys use an arched shoulder-rump trunk, broad blunt face, robust limbs, and compact tail\n"
                 "near monkeys use a lower tapered trunk, separated crown, and restrained furry tail root\n"
                 "near monkeys use a shallow waisted torso and shortened facial projection\n"
                 "monkey shoulder and hip mantles blend the trunk into all four animated limbs")
        validate_monkey_capture_log(valid)
        validate_monkey_capture_log(valid+"\nmonkey subject paired tree grips 1", True)
        for marker in ("", "monkey subject paired tree grips 0", "monkey subject paired tree grips 10"):
            with self.subTest(marker=marker), self.assertRaises(RuntimeError):
                validate_monkey_capture_log(valid+"\n"+marker, True)
        for invalid in ("", "monkeys 2/7 visible, 4 draws", valid.replace("near 1", "near 0"),
                        valid.replace("visible 2", "visible 0"), valid.replace("draws 4", "draws 0"),
                        valid.split("\n")[0], valid.replace("10 mips", "1 mips")):
            with self.subTest(invalid=invalid), self.assertRaises(RuntimeError):
                validate_monkey_capture_log(invalid)

    def test_missing_leaf_acceptance_is_narrow_and_opt_in(self):
        expected = ("island-simulator: leaf material fallback: Content/vegetation/leaf-lamina.png: "
                    "File not found; retaining procedural veins")
        validate_graphics_log(expected, missing_leaf=True)
        validate_graphics_log(expected + "\n" + expected, missing_leaf=True)
        with self.assertRaises(RuntimeError): validate_graphics_log(expected)
        for bad in ("", expected.replace("leaf-lamina.png", "other.png"),
                    expected.replace("retaining procedural veins", "ignored"),
                    expected + "\nleaf material loaded Content/vegetation/leaf-lamina.png",
                    expected + "\nforest-floor material fallback: missing",
                    expected + "\ngrass stock-effect fallback: shader failed",
                    expected.replace("File not found", "gl-error GL_INVALID_OPERATION")):
            with self.subTest(bad=bad), self.assertRaises(RuntimeError):
                validate_graphics_log(bad, missing_leaf=True)

    def test_directory_fixture_hides_only_child_view_and_preserves_host(self):
        with tempfile.TemporaryDirectory(prefix="island-leaf-mask-") as directory:
            root = Path(directory)
            empty, target = root / "empty", root / "material"
            empty.mkdir()
            target.mkdir()
            asset = target / "leaf.png"
            asset.write_bytes(b"original host asset")
            command = contained_command([sys.executable, "-c",
                "import pathlib,sys; p=pathlib.Path(sys.argv[1]); "
                "assert not p.exists(); assert not pathlib.Path('/dev/dri').exists()", str(asset)],
                private_devices=True, read_only_directories=[(empty,target)])
            result = subprocess.run(command, capture_output=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(asset.read_bytes(), b"original host asset")
            self.assertEqual(list(empty.iterdir()), [])

    def test_fixture_cannot_expose_devices_or_be_used_with_host_gpu_access(self):
        with tempfile.TemporaryDirectory(prefix="island-leaf-mask-") as directory:
            root = Path(directory)
            with self.assertRaises(MonitorError):
                contained_command(["fixture"], read_only_directories=[(root,root)])
            for source,target in ((Path("relative"),root), (root,Path("/")),
                                  (Path("/dev"),root), (root,Path("/proc")),
                                  (root / "absent",root)):
                with self.subTest(source=source,target=target), self.assertRaises(MonitorError):
                    contained_command(["fixture"], private_devices=True,
                                      read_only_directories=[(source,target)])

    def test_antialiasing_qualification_requires_applied_samples_not_just_request(self):
        for expected in (2, 4):
            marker = f"island-simulator: edge antialiasing requested {expected}x renderer-applied {expected}x"
            validate_antialiasing_log(marker, expected)
            validate_antialiasing_log(marker + "\n" + marker, expected)
            for bad in ("", f"edge antialiasing requested {expected}x", marker.replace(
                    f"renderer-applied {expected}x", "renderer-applied 0x"),
                    marker + "\nedge antialiasing requested 8x renderer-applied 8x"):
                with self.subTest(expected=expected, bad=bad), self.assertRaises(RuntimeError):
                    validate_antialiasing_log(bad, expected)

    def test_native_gl_only(self):
        self.assertEqual(check_configuration(self.cache), Path("/dependency"))

    def test_other_default_refused(self):
        with self.assertRaises(ValueError):
            check_configuration(self.cache.replace("OPENGL33", "VULKAN"))

    def test_missing_dependency_refused_with_actionable_error(self):
        for cache in ("CNA_GRAPHICS_RENDERER:STRING=OPENGL33\n",
                      "CNA_GRAPHICS_RENDERER:STRING=OPENGL33\nCNA_ROOT_DIR:PATH=\n"):
            with self.subTest(cache=cache), self.assertRaisesRegex(ValueError, "CNA_ROOT_DIR"):
                check_configuration(cache)

    def test_multi_renderer_refused(self):
        with self.assertRaises(ValueError):
            check_configuration(self.cache + "CNA_GRAPHICS_RENDERERS:STRING=OPENGL33;VULKAN\n")

    def test_boolean_extra_renderer_refused(self):
        with self.assertRaises(ValueError):
            check_configuration(self.cache + "CNA_RENDERER_VULKAN:BOOL=ON\n")

    def test_forced_software_and_worker_cap(self):
        environment = software_environment(Path("/evidence"))
        self.assertEqual(environment["LIBGL_ALWAYS_SOFTWARE"], "1")
        self.assertEqual(environment["GALLIUM_DRIVER"], "llvmpipe")
        self.assertEqual(environment["LP_NUM_THREADS"], "8")
        self.assertEqual(environment["SDL_VIDEO_DRIVER"], "x11")

    def test_material_and_stock_fallbacks_refused(self):
        for message in ("terrain material fallback: compile failed",
                        "shipwreck material fallback: invalid shader",
                        "grass stock-effect fallback: unavailable",
                        "island-simulator: windthrow sun shadow receiver unavailable: custom effects unavailable",
                        "island-simulator: lighthouse sun shadow receiver unavailable: shader failed",
                        "island-simulator: lighthouse sun shadow caster unavailable: shader failed",
                        "island-simulator: cottage masonry unavailable; retaining plain receiver: shader failed",
                        "gl-error GL_INVALID_OPERATION"):
            with self.subTest(message=message), self.assertRaises(RuntimeError):
                validate_graphics_log(message)

    def test_successful_material_diagnostic_accepted(self):
        validate_graphics_log("terrain close soil uses footprint-filtered clods and analytic relief")

    def test_diagnostic_only_rendering_refused(self):
        with self.assertRaisesRegex(RuntimeError, "cannot qualify"):
            validate_graphics_log("TEMPORARY SOFTWARE DIAGNOSTIC flat ocean normals")

    def test_inspection_arguments(self):
        self.assertEqual(inspection_arguments(None), [])
        self.assertEqual(inspection_arguments(["-87120", "-15369", "near"]),
                         ["--canopy-lod-inspection", "-87120", "-15369", "near"])
        self.assertEqual(inspection_arguments(["-87120", "-15369", "roots"]),
                         ["--canopy-lod-inspection", "-87120", "-15369", "roots"])
        for values in (["0", "0", "other"], ["1.5", "0", "near"], ["500001", "0", "near"],
                       ["0", "near"], ["x", "0", "far"]):
            with self.subTest(values=values), self.assertRaises(ValueError):
                inspection_arguments(values)

    def test_inspection_requires_exact_marker_and_rejects_other_diagnostics(self):
        values = ["-87120", "-15369", "near"]
        marker = ("TEMPORARY SOFTWARE DIAGNOSTIC canopy-lod target=-87120:-15369 "
                  "tier=near frozen-simulation=0 shader-time=1")
        validate_graphics_log(marker, values)
        for diagnostic in ("", marker.replace("tier=near", "tier=far"),
                           marker + "\nTEMPORARY SOFTWARE DIAGNOSTIC flat ocean normals",
                           marker + "\ngl-error GL_INVALID_OPERATION"):
            with self.subTest(diagnostic=diagnostic), self.assertRaises(RuntimeError):
                validate_graphics_log(diagnostic, values)


class CaptureArtifactTests(unittest.TestCase):
    def test_capture_records_before_launch_and_after_failed_workload(self):
        for mode in ("stable","replace","missing"):
            with self.subTest(mode=mode),tempfile.TemporaryDirectory() as directory:
                build=Path(directory)
                binary=build/"island-simulator"
                binary.write_bytes(b"old executable")
                (build/"CMakeCache.txt").write_text("fixture")
                journal=Mock()
                journal.latest.return_value={"_BOOT_ID":"boot","__CURSOR":"cursor"}
                journal.read.return_value=[]
                def workload(command,evidence,*args,**kwargs):
                    report=json.loads((evidence/"run.json").read_text())
                    self.assertEqual(report["artifact"],artifact_identity(binary))
                    if mode=="replace":binary.write_bytes(b"new executable")
                    elif mode=="missing":binary.unlink()
                    return 1  # Skip graphics validators; no process or GPU is launched.
                with patch("sys.argv",["capture",str(build),"jungle"]),patch("builtins.print"),patch.multiple(
                    "run_software_capture",check_configuration=Mock(return_value=build),
                    Journal=Mock(return_value=journal),run_monitored_capture=Mock(side_effect=workload),
                    compress_captures=Mock(),prune_sets=Mock(),cited_stamps=Mock(return_value=set())):
                    self.assertEqual(capture_main(),1)
                evidence=next((build/"software-captures").iterdir())
                report=json.loads((evidence/"run.json").read_text())
                self.assertEqual(report["failure"],"software capture exited 1")
                self.assertEqual(report["artifact_content_unchanged"],mode=="stable")
                if mode!="stable":self.assertIn("artifact_verification_error",report)
                if mode!="missing":self.assertIn("artifact_after",report)

    def test_fullscreen_cannot_pass_after_artifact_change(self):
        for mode in ("stable","replace","missing"):
            with self.subTest(mode=mode),tempfile.TemporaryDirectory() as directory:
                root=Path(directory);build=root/"build";build.mkdir()
                tools=root/"tools";tools.mkdir()
                harness=tools/"run_fullscreen_software.py";harness.write_bytes(b"fixture")
                (tools/"fullscreen_openbox.xml").write_bytes(b"config")
                binary=build/"island-simulator";binary.write_bytes(b"old executable")
                manager=build/"window-manager";manager.write_bytes(b"manager")
                (build/"CMakeCache.txt").write_text("fixture")
                journal=Mock()
                journal.latest.return_value={"_BOOT_ID":"boot","__CURSOR":"cursor"}
                journal.read.return_value=[]
                def workload(command,evidence,*args,**kwargs):
                    self.assertEqual(json.loads((evidence/"run.json").read_text())["artifact"],
                                     artifact_identity(binary))
                    (evidence/"input-result.json").write_text('{"passed":true}')
                    if mode=="replace":binary.write_bytes(b"new executable")
                    elif mode=="missing":binary.unlink()
                    return 0  # Fake worker success; no process or GPU is launched.
                with patch("sys.argv",["fullscreen",str(build),"--window-manager",str(manager)]),\
                     patch("run_fullscreen_software.__file__",str(harness)),\
                     patch("builtins.print"),patch("run_fullscreen_software.shutil.which",return_value="fixture"),\
                     patch("run_fullscreen_software.os.sched_setaffinity"),patch.multiple(
                         "run_fullscreen_software",check_configuration=Mock(return_value=build),
                         Journal=Mock(return_value=journal),run_monitored_capture=Mock(side_effect=workload)):
                    if mode=="stable":fullscreen_main()
                    else:
                        with self.assertRaises((OSError,RuntimeError)):fullscreen_main()
                report=json.loads((next((build/"software-fullscreen").iterdir())/"run.json").read_text())
                self.assertEqual(report["result"],"passed" if mode=="stable" else "failed")
                self.assertEqual(report["artifact_content_unchanged"],mode=="stable")

    def test_unchanged_bytes_pass_and_records_keep_file_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/"game"
            path.write_bytes(b"original executable")
            before=artifact_identity(path)
            verify_artifact_content_unchanged(before,artifact_identity(path))
            replacement=path.with_name("replacement")
            replacement.write_bytes(path.read_bytes())
            replacement.replace(path)
            after=artifact_identity(path)
            self.assertNotEqual(before["inode"],after["inode"])
            verify_artifact_content_unchanged(before,after)

    def test_changed_bytes_fail_even_at_same_size(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/"game"
            path.write_bytes(b"old executable")
            before=artifact_identity(path)
            for data in (b"new executable",b"a longer replacement"):
                path.write_bytes(data)
                with self.subTest(data=data),self.assertRaisesRegex(MonitorError,"content changed"):
                    verify_artifact_content_unchanged(before,artifact_identity(path))

    def test_missing_artifact_cannot_be_certified(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/"game"
            path.write_bytes(b"old executable")
            before=artifact_identity(path)
            path.unlink()
            with self.assertRaises(OSError):artifact_identity(path)


class CaptureMonitorTests(unittest.TestCase):
    """All child processes and journal reads are fake; no graphics or kernel fault injection."""

    baseline = {"__CURSOR": "start", "_BOOT_ID": "boot", "MESSAGE": "baseline"}

    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix="island-software-monitor-")
        self.addCleanup(self.directory.cleanup)
        self.evidence = Path(self.directory.name)
        self.journal = Mock()
        self.journal.latest.return_value = self.baseline
        self.journal.read.return_value = []
        self.process = Mock()
        self.process.poll.return_value = 0
        patches = ExitStack()  # TestCase.enterContext requires Python 3.11; retain 3.10 support.
        self.addCleanup(patches.close)
        self.spawn = patches.enter_context(patch("run_software_capture.subprocess.Popen",
                                                return_value=self.process))
        self.stop = patches.enter_context(patch("run_software_capture.stop_process_group"))
        self.sleep = patches.enter_context(patch("run_software_capture.time.sleep"))
        self.handlers = {number: signal.getsignal(number)
                         for number in (signal.SIGINT, signal.SIGTERM)}

    def tearDown(self):
        for number, handler in self.handlers.items():
            self.assertEqual(signal.getsignal(number), handler)

    def run_capture(self, **kwargs):
        return run_monitored_capture(["fake-game"], self.evidence, {}, self.evidence,
                                     self.journal, self.baseline, **kwargs)

    def record(self, message="ordinary kernel message", **fields):
        return {**self.baseline, "__CURSOR": "next", "MESSAGE": message, **fields}

    def test_success_checks_before_during_and_after_owned_cleanup(self):
        self.journal.read.side_effect = [[], [self.record()], []]
        self.assertEqual(self.run_capture(), 0)
        self.assertEqual(self.journal.latest.call_count, 3)
        self.assertEqual(self.journal.read.call_args_list[-1].args, ("--after-cursor", "next"))
        launch = self.spawn.call_args.args[0]
        self.assertIn("--unshare-pid", launch)
        self.assertIn("--die-with-parent", launch)
        self.assertIn("--dev", launch)
        self.assertNotIn("--dev-bind", launch)
        self.assertEqual(launch[-2:], ["--", "fake-game"])
        attribution = json.loads((self.evidence / "process-containment.json").read_text())
        self.assertEqual(attribution["command"], launch)
        self.assertTrue(attribution["private_devices"])
        self.stop.assert_called_once_with(self.process)
        self.assertTrue(self.spawn.call_args.kwargs["start_new_session"])
        live = (self.evidence / "kernel-live.jsonl").read_text().splitlines()
        self.assertEqual([json.loads(line) for line in live], [self.record()])

    def test_fault_before_launch_refuses_process(self):
        self.journal.read.return_value = [self.record("amdgpu: [gfxhub] page fault")]
        with self.assertRaisesRegex(MonitorError, "GPU fault"):
            self.run_capture()
        self.spawn.assert_not_called()
        self.stop.assert_not_called()

    def test_explicit_outer_device_bind_for_nested_android_sandbox(self):
        self.assertEqual(self.run_capture(private_devices=False), 0)
        launch = self.spawn.call_args.args[0]
        self.assertIn("--dev-bind", launch)
        self.assertNotIn("--dev", launch)
        attribution = json.loads((self.evidence / "process-containment.json").read_text())
        self.assertFalse(attribution["private_devices"])

    def test_first_live_fault_stops_before_waiting_for_process_exit(self):
        self.journal.read.side_effect = [[], [self.record("amdgpu: [gfxhub] page fault")], []]
        with self.assertRaisesRegex(MonitorError, "GPU fault"):
            self.run_capture()
        self.process.poll.assert_not_called()
        self.sleep.assert_not_called()
        self.stop.assert_called_once_with(self.process)
        self.assertIn("page fault", (self.evidence / "kernel-live.jsonl").read_text())

    def test_fault_during_cleanup_is_not_success(self):
        self.journal.read.side_effect = [[], [], [self.record("amdgpu: ring gfx_0.0.0 timeout")]]
        with self.assertRaisesRegex(MonitorError, "GPU fault"):
            self.run_capture()
        self.stop.assert_called_once_with(self.process)

    def test_journal_failure_after_launch_stops_child(self):
        self.journal.latest.side_effect = [self.baseline, MonitorError("journal access lost")]
        with self.assertRaisesRegex(MonitorError, "journal access lost"):
            self.run_capture()
        self.stop.assert_called_once_with(self.process)

    def test_boot_discontinuity_stops_child_even_with_empty_delta(self):
        self.journal.latest.side_effect = [self.baseline, self.record(_BOOT_ID="new-boot")]
        with self.assertRaisesRegex(MonitorError, "boot discontinuity"):
            self.run_capture()
        self.stop.assert_called_once_with(self.process)

    def test_boot_discontinuity_in_delta_is_preserved(self):
        self.journal.read.side_effect = [[], [self.record(_BOOT_ID="new-boot")]]
        with self.assertRaisesRegex(MonitorError, "boot discontinuity"):
            self.run_capture()
        self.assertIn("new-boot", (self.evidence / "kernel-live.jsonl").read_text())

    def test_timeout_stops_process(self):
        self.process.poll.return_value = None
        with self.assertRaisesRegex(MonitorError, "time limit"):
            self.run_capture(timeout=0.0)
        self.stop.assert_called_once_with(self.process)

    def test_signal_stops_process_and_restores_handlers(self):
        self.process.poll.return_value = None
        self.sleep.side_effect = lambda _: signal.getsignal(signal.SIGTERM)(signal.SIGTERM, None)
        with self.assertRaisesRegex(MonitorError, "interrupted by signal"):
            self.run_capture()
        self.stop.assert_called_once_with(self.process)

    def test_cleanup_failure_does_not_replace_primary_fault(self):
        self.journal.read.side_effect = [[], [self.record("GPU reset begin!")]]
        self.stop.side_effect = MonitorError("owned child did not terminate")
        with patch("run_software_capture.sys.stderr"), self.assertRaisesRegex(MonitorError, "GPU fault"):
            self.run_capture()

    def test_launch_failure_restores_handlers(self):
        self.spawn.side_effect = OSError("cannot launch")
        with self.assertRaisesRegex(OSError, "cannot launch"):
            self.run_capture()
        self.stop.assert_not_called()

    def test_nonzero_exit_is_returned_after_kernel_check(self):
        self.process.poll.return_value = 7
        self.assertEqual(self.run_capture(), 7)
        self.assertEqual(self.journal.read.call_count, 3)
        self.stop.assert_called_once_with(self.process)


class CaptureRetentionTests(unittest.TestCase):
    def test_ppm_becomes_an_identical_png(self):
        import struct, zlib
        from capture_retention import compress_captures
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            pixels = bytes((x * 7 + y * 3 + c) % 256 for y in range(5) for x in range(6) for c in range(3))
            (folder / "capture.ppm").write_bytes(b"P6\n6 5\n255\n" + pixels)
            (folder / "capture.ppm.fire-stage-0.ppm").write_bytes(b"P6\n6 5\n255\n" + pixels)
            self.assertEqual(compress_captures(folder), 2)
            self.assertFalse(list(folder.glob("*.ppm")))
            data = (folder / "capture.png").read_bytes()
            self.assertTrue(data.startswith(b"\x89PNG\r\n\x1a\n"))
            width, height = struct.unpack(">II", data[16:24])
            start = data.index(b"IDAT") + 4
            length = struct.unpack(">I", data[start - 8:start - 4])[0]
            raw = zlib.decompress(data[start:start + length])
            rows = [raw[row * 19 + 1:(row + 1) * 19] for row in range(5)]
            self.assertEqual((width, height), (6, 5))
            self.assertEqual(b"".join(rows), pixels)
            self.assertTrue((folder / "capture.ppm.fire-stage-0.png").is_file())

    def test_prune_keeps_the_newest_and_the_cited_sets(self):
        from capture_retention import prune_sets
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            names = [f"2026101{day}T0{hour}0000.000000Z-view" for day in range(3) for hour in range(3)]
            for name in names:
                (folder / name).mkdir()
            (folder / "state").mkdir()
            removed = prune_sets(folder, 4, {"20261010T000000"})
            left = sorted(entry.name for entry in folder.iterdir())
            self.assertEqual(left, sorted(names[-4:] + [names[0], "state"]))
            self.assertEqual(len(removed), 4)

    def test_left_over_browser_profiles_are_removed(self):
        from capture_retention import remove_browser_profiles
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / "chrome-profile-AbC123" / "Default").mkdir(parents=True)
            (folder / "island-browser-coast-water.png").write_bytes(b"x")
            self.assertEqual(remove_browser_profiles(folder), 1)
            self.assertEqual([entry.name for entry in folder.iterdir()], ["island-browser-coast-water.png"])

    def test_uppercase_handoff_and_linked_evidence_survive_pruning(self):
        from capture_retention import cited_stamps, prune_sets
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            captures = root / "build" / "software-captures"
            captures.mkdir(parents=True)
            names = [f"2026101{day}T000000.000000Z-view" for day in range(6)]
            for name in names:
                (captures / name).mkdir()
            (root / "HANDOFF.md").write_text(
                f"Direct: {names[0]}\nIndex: `build/comparison.json`\n", encoding="utf-8")
            (root / "handoff.md").write_text(names[1], encoding="utf-8")
            (root / "build" / "comparison.json").write_text(
                json.dumps({"before": names[2], "after": names[3]}), encoding="utf-8")
            removed = prune_sets(captures, 1, cited_stamps(root))
            self.assertEqual([path.name for path in removed], [names[4]])
            self.assertEqual(sorted(path.name for path in captures.iterdir()), names[:4] + names[5:])

    def test_evidence_index_cannot_escape_build_or_read_unbounded_files(self):
        from capture_retention import cited_stamps, MAX_EVIDENCE_INDEX_BYTES
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "build").mkdir()
            outside = root / "outside.json"
            outside.write_text('"20261010T000000"', encoding="utf-8")
            (root / "build" / "escape.json").symlink_to(outside)
            (root / "build" / "invalid.json").write_bytes(b"\xff20261011T000000")
            with (root / "build" / "large.json").open("wb") as stream:
                stream.write(b"20261012T000000")
                stream.truncate(MAX_EVIDENCE_INDEX_BYTES + 1)
            (root / "HANDOFF.md").write_text(
                "build/../outside.json build/escape.json build/invalid.json "
                "build/large.json build/missing.json", encoding="utf-8")
            self.assertEqual(cited_stamps(root), set())


if __name__ == "__main__":
    unittest.main()
