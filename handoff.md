# island-simulator handoff

## Backlog (agent-actionable, 2026-10-19)

Distilled from `PLAN.md` "Latest operator priorities — 2026-10-12" and "Current significant
unfinished work — 2026-10-15". Remove an item only when its evidence is recorded in `PLAN.md`.
Done 2026-10-19 (evidence in the matching `PLAN.md` items): cave overhead mantle (turf re-hue,
hashed 401 tufts, `cave-top` in the Web cave scope); spider discoverability on ordinary jungle
views plus current Chrome evidence; permanent eye-height `helicopter-approach` viewpoint with
native/Chrome/Android evidence; `IslandCloseObserverTurningTests` multi-species turning audit,
which fixed double per-frame turns in `DeerHerd` and `CrocodileGroup`; `shipwreck-wide` matched
landmark comparison and debris integration review (sand scour/drift mound against the hull
deferred as terrain-replacement polish); ground-plant per-placement proportion variation for the
jungle-floor repetition critique.

Also done: moving-edge review (Chrome `grass-motion` rerun passes after the F7 east-clearing
teleport was moved 6 m west of the campfire pit); final Web/Android rebuilds and the full native
suite pass on the committed sources.

Operator follow-ups 2026-10-19 (in `PLAN.md` under the 2026-10-12 priorities): animals now stop
and face an approaching player and retreat obliquely instead of turning their backs (gated by
`IslandCloseObserverTurningTests`; goat/boar gait capture clocks ignore the QA camera); the Web
build scales mouse look 2.5x and requests unadjusted pointer-lock motion. Both need the operator's
in-play confirmation (desktop facing feel, browser mouse travel) before `[x]`.

## Operator backlog 2026-10-19 (evening) — work in this order, then loop

Operator instruction: implement all twenty items below, then continue indefinitely in a loop of
"propose an island improvement, implement it" until the operator stops the session. Mark items
here as done with their `PLAN.md` evidence reference.

1. [x] Directional sun shadows (`SunShadowMap`, PLAN "Latest lighting pass — directional sun shadow
   map"); the wreck, hut, props, jetty, bridge, lookout and helicopter receive it since loop
   passes 216-218, a with/without comparison is pass 205, the animals read a CPU sun-shade field,
   and water receiving was measured and dropped (pass 219). The frame inventory names it "large-object sun shadows N" since loop pass 519 (hut 832 casters).
2. [x] Mid-distance hillside grain (PLAN "Latest hillside pass — mid-distance tussock grain").
3. [x] Wreck scour bar (PLAN "Latest wreck-grounding pass — sand scour bar against the hull").
4. [x] Sand streak audit (PLAN "Latest beach-streak audit — seabed ripples confined below the waterline").
5. [x] Cumulus layers and sea cloud shadows (PLAN "Latest sky pass — fringed cumulus layers and cloud shadows on the sea").
6. [x] Stream cascade and plunge pool (PLAN "Latest stream pass — a small cascade and plunge pool", `cascade` viewpoint).
7. [x] Bird identity: dove coos, pigeon walk, grey dove (PLAN "Latest bird-identity pass").
8. [x] Ecological links (PLAN "Latest ecology pass"); the goats drink at a rain-filled basin on their slope since loop pass 203.
9. [x] Herd cohesion and sentinels (PLAN "Latest herd pass — cohesion and sentinels").
10. [x] Tracks (PLAN "Latest ground-sign pass — deer and boar track lines", `tracks` viewpoint).
11. [x] Sea from within (PLAN "Latest sea-from-within pass — leaping dolphins, a surface fin and parting schools").
12. [x] Signs of the resident at the hut (PLAN "Latest occupation pass"); the canoe moves to item 13.
13. [x] Jetty and dugout canoe (PLAN "Latest landing pass — the resident's jetty and dugout canoe", `jetty` viewpoint).
14. [x] Rope bridge over the stream (PLAN "Latest crossing pass", `bridge` viewpoint); the walkable lookout tower followed in the loop (08a7a42, `lookout` viewpoint).
15. [x] Lianas and aerial roots (PLAN "Latest canopy-interior pass").
16. [x] Flowering shrubs and banana stools (PLAN "Latest understory-colour pass").
17. [x] Fruit in crowns and fallen coconuts (PLAN "Latest harvest pass", `fruit` viewpoint).
18. [x] Fungi on logs and moss on rocks (PLAN "Latest decay pass", `log` viewpoint).
19. [x] Player shadow and surface-dependent footsteps (PLAN "Latest player-presence pass").
20. [x] Diving details (PLAN "Latest diving pass — light shafts, bubbles and seabed caustics").
- Evidence re-run 2026-10-22 (loop pass 484, build 3e3a0e86): every item above re-captured natively and all 20 views passed validation. All 26 item diagnostics are present in the runs' workload logs, among them "sun shadow map 1 renders, 832 casters" at the hut (item 1), "walker's own shadow 1, sun figure 1184 triangles" (item 19), the wreck scour bar, cumulus fringe and sea cloud shadows, cascade and plunge pool, parrots and fruit doves, the goats' basin, the herd sentinels, the deer print line, dolphins leaping, the shark fin and parting schools, the hut yard, the jetty with its dugout canoe, the rope bridge and lookout, lianas, banana stools with hibiscus and plumeria, fallen fruit and coconuts, bracket fungi and rock moss, footstep grounds, and light shafts, bubbles and caustics. Sets: 1 `20261022T054404` hut; 2 `20261022T054413` east; 3 `20261022T054427` shipwreck; 4 `20261022T054435` shipwreck-wide; 5 `20261022T054444` distant; 6 `20261022T054452` cascade; 7 `20261022T054501` birds; 8 `20261022T054509` goats; 9 `20261022T054529` deer; 10 `20261022T054537` tracks; 11 `20261022T054545` dolphins; 12 `20261022T054404` hut; 13 `20261022T054553` jetty; 14 `20261022T054602` bridge; 14 `20261022T054613` lookout; 15 `20261022T054621` jungle; 16 `20261022T054631` bush; 17 `20261022T054642` fruit; 18 `20261022T054652` log; 19 `20261022T054703` walker-shadow; 20 `20261022T054713` fish.
- Evidence re-run 2026-10-22 (loop pass 512, build ab4d78b7, after the mine, the volcano and the receiver change): all 20 item views re-captured natively and passed validation (`20261022T112450` hut .. `20261022T112728` fish); the diagnostics are present again, among them "sun shadow map 1 renders, 832 casters" (1), "open inland ground keeps footprint-filtered metre-scale tussock grain" (2), "wreck scour bar rises on the lee side of the hull" (3), "shoreline-raked ripples" (4), "sky cumulus carry a frayed translucent fringe layer" and "open sea carries drifting cumulus shadow patches" (5), the cascade's plunge pool (6), "parrots and compact tropical fruit doves" (7), the goats' "rain-filled rock basin" (8), "a sentinel lifts" (9), "a deer print line" (10), "a dolphin's leap" (11), the hut yard (12), the jetty "with the dugout canoe" (13), the rope bridge and the lookout's ramp (14), "lianas ... aerial roots" (15), "banana stools ... hibiscus and plumeria" (16), "158 fallen coconuts" and the fruit-bearing crown (17), "bracket fungi ... moss" (18), the walker's own shadow and the footstep ground probe (19), "light shafts ... bubbles ... caustics" (20).

Operator request 2026-10-20 (added to the task, takes priority in the loop): a day/night cycle
with the moon, 24 game hours in about 24 real minutes; then weather: sometimes rain, a storm, or
other weather. Snow was asked about and judged physically impossible on a tropical island at sea
level, so it is left out (at most a hidden joke switch, off by default, if the operator asks).
- [x] Day/night clock (24 h per 24 min), sun arc east to west with a short tropical twilight,
      moon with phases over the days, stars at night (PLAN "Day/night pass 1"; T moves an hour on,
      `--time-of-day` for captures, QA views held at 14:00).
- [x] Time-of-day light: golden hours, moonlit blue night, shadows fading at low sun and at
      night, campfire glowing at night (done, Day/night pass 1; lighting the ground through the
      grade, Loop pass 225); the hut's windows and veranda lantern at night (Loop pass 228).
- [x] The moving sun in the real-time shaders and the sun shadow map; geometry with lighting
      baked at build time keeps the 14:00 sun (listed in PLAN "Day/night pass 1").
- [x] Weather: clear, partly cloudy, overcast, showers, tropical thunderstorm (lightning and
      thunder), world-anchored rain around the camera, wet darkened ground, rain and thunder
      sounds (done, PLAN "Weather pass 1"; Y steps through them, `--weather` for captures);
      rain rings on the sea (Loop pass 226), puddles (Loop pass 233), storm gusts (Loop pass
      234), morning mist (Loop pass 235). The weather request is complete.

Operator request 2026-10-20 (the next task after the weather): ride a quad bike (ATV).
- [x] F switches between walking and the ATV and back; a CNA avatar rides it (native desktop;
      Web and Android draw a stand-in rider: the avatar's bone uniforms exceed GLES 3's guarantee).
- [x] As realistic a model as possible (steel frame, moulded fenders, hood and rear deck,
      floorboards, racks, brush guard, handlebars under a pod, knobby tyres on spoked rims,
      A-arms, coil-overs and a swingarm that follow the suspension), exhaust smoke, a cast
      shadow, headlights by day and a pool of light on the ground at night (PLAN "Quad bike pass 1").
- [x] Drives over the island's terrain with suspension, can roll over; Backspace rights it.
- [x] Engine sound following the revs; a top speed of a real ATV (80 km/h).
- [x] Tyre tracks in sand and soil, dust off dry ground, spray in the shallows, a knock sound
      for trees, rocks, hard landings and rollovers (PLAN "Quad bike pass 2").
- [x] The engine idles on after you step off (lamps and exhaust with it) and is switched off
      90 s later or when the quad lies on its side; tyres crunch louder with the speed and the
      sliding (PLAN "Quad bike pass 3").

Operator request 2026-10-22 (future task, taken next in the loop): a whale skeleton somewhere
on a beach.
- [x] A stranded whale's bleached skeleton on a quiet stretch of beach, half sunk in the sand (PLAN "Loop pass 493", `whale-skeleton` view, `20261022T074431`).
      Skull with its curved jaws, vertebral column tapering to the tail, rib arcs (some fallen),
      shoulder blades and the flipper bones, a few vertebrae scattered. Weathered ivory-grey bone,
      receiving and casting sun shadows, with a QA view.

Operator requests 2026-10-22 (future tasks, taken in this order after the whale skeleton and its
realism pass):
- [x] A statue of some god somewhere on the island: a basalt tiki idol on an ahu platform at
      (76, -26) (PLAN "Loop pass 495", `idol` view, `20261022T075641`).
- [x] On a beach, HELP ME (or similar) laid out in stones, as a castaway would: pass 497 made it 7 m letters across the south beach at (0, -231), 580 stones, readable from aircraft; (PLAN "Loop pass 496", `help-sign` view, `20261022T080350`).
- [x] A lighthouse: whitewashed with red bands on the east coast's rise at (225, 73)  (PLAN "Loop pass 498"; night beams pass 499; `lighthouse` view, `20261022T082109`); its night light next.
- [x] A Second World War aircraft wreck: a fighter nose-down in a clearing at (86, 122), tail broken off, a wing torn away (PLAN "Loop pass 500", `plane-wreck` view, `20261022T083752`).
- [x] Washed-up plastic PET bottles now and then on the beaches: 39 along the strandline, see-through plastic, caps, faded labels (PLAN "Loop pass 501", `beach-litter` view, `20261022T085855`).
- [x] A real large ship, about 30 m long, near the island: enterable and walkable, the hold
      below deck too, with several sailors' skeletons aboard: a 30 m coaster driven up the
      south-west beach, walkable up its ladder, on deck, in the deckhouse and down in the hold
      (PLAN "Loop pass 503"), five of its crew dead aboard (PLAN "Loop pass 504"; `freighter*`
      views, `20261022T094410`..`094443`).
- [x] A mine entrance somewhere on the island, a tunnel several tens of metres long or more,
      lit inside by lanterns or something else that glows: an old mine under the main mountain
      at (1, 7), a 48 m timbered adit lit by 8 lanterns to a chamber of glowing crystals,
      walkable (PLAN "Loop pass 506", `mine*` views, `20261022T101820`..`102409`).
- [x] A small volcano somewhere on the island, with lava visible inside its crater: a 25 m cone on
      the eastern foothills at (174, 0), a churning lava lake in its crater, climbable to the rim,
      glowing at night (PLAN "Loop pass 507", `volcano*` views, `20261022T105022`..`105039`).

Loop log (propose → implement, continues until the operator stops the session):
- Pass 1: dappled leaf-gap sun flecks on the shaded forest floor; `ground` view reclassified
  (PLAN "Loop pass 1").
- Pass 2: butterflies make nectar excursions to the flowering bushes near their habitats
  (PLAN "Loop pass 2").
- Pass 3: seabirds leave their high orbits to circle low over the fish school of the moment
  (PLAN "Loop pass 3").
- Pass 4: a broken high-tide wrack line of weed, twigs and shell on the upper beach
  (PLAN "Loop pass 4").
- Pass 5: gust fronts sweeping the grass, crowns and fronds downwind together (PLAN "Loop
  pass 5").
- Pass 6: boars wallow at the stream bank and carry the mud on legs and belly (PLAN "Loop
  pass 6").
- Pass 7: circling seabirds plunge-dive and each strike raises a foam ring (PLAN "Loop pass 7").
- Pass 8: a walkable lookout tower beside the hut (PLAN "Loop pass 8", `lookout` viewpoint);
  closes the deferred half of backlog item 14.
- Pass 9: a positional splash sound for the seabird strikes (PLAN "Loop pass 9").
- Pass 10: fruit knocked loose by picking monkeys drops and lies under the tree (PLAN "Loop
  pass 10").
- Pass 11: foam collars at the jetty posts and a bird perch on the lookout rail (PLAN "Loop
  pass 11").
- Pass 12: the resident's tour calls at the jetty foot (PLAN "Loop pass 12").
- Pass 13: the resident's bare footprints along the beach from the jetty (PLAN "Loop pass 13").
- Pass 14: gust fronts shake coconuts from the nearer palms (PLAN "Loop pass 14").
- Pass 15: a positional thud when a coconut lands (PLAN "Loop pass 15").
- Pass 16: one shared wrack line that the beach crabs work and feed along (PLAN "Loop pass 16").
- Pass 17: the monkeys walk to the fallen fruit and eat it (PLAN "Loop pass 17").
- Pass 18: the boars forage for the fallen fruit and coconuts before the trunk bases (PLAN "Loop pass 18").
- Pass 19: a coconut's thud startles the deer and goats within 30 m (PLAN "Loop pass 19").
- Pass 20: perched birds flush from a close walker or a nearby thud (PLAN "Loop pass 20").
- Pass 21: butterflies come down to feed on the fermenting fruit (PLAN "Loop pass 21").
- Pass 22: the resident's walk along the beach scatters the crabs (PLAN "Loop pass 22").
- Pass 23: the gust front swells the breeze and canopy beds as it passes the listener (PLAN "Loop pass 23").
- Pass 24: a fallen fruit bounces off its strike and rolls downhill to rest (PLAN "Loop pass 24").
- Pass 25: the fruit doves cross to the fallen fruit and forage around it (PLAN "Loop pass 25").
- Pass 26: the gust front lays the campfire plume over and tears it (PLAN "Loop pass 26").
- Pass 27: gust fronts race over the sea as darker cat's-paw bands (PLAN "Loop pass 27").
- Pass 28: leaves drift down the stream on the current, over the cascade and out to sea (PLAN "Loop pass 28").
- Pass 29: gust fronts drive pale sand streamers along the dry beach (PLAN "Loop pass 29").
- Pass 30: small fish hold station in the plunge pool, dart and scatter from the walker (PLAN "Loop pass 30").
- Pass 31: the resident turns to look out to sea during the jetty-foot pause (PLAN "Loop pass 31").
- Pass 32: a leaf drifting over a pool fish draws it up in a dart (PLAN "Loop pass 32").
- Pass 33: turtle hatchlings break out of the nests and scramble to the surf (PLAN "Loop pass 33").
- Pass 34: the hut's hung fish and net swing in the breeze and the gust fronts (PLAN "Loop pass 34").
- Pass 35: flying birds cast soft moving shadows on the ground (PLAN "Loop pass 35").
- Pass 36: coconuts that strike the shallows float and drift ashore (PLAN "Loop pass 36").
- Pass 37: the cumulus shadows drift over the land as well as the sea (PLAN "Loop pass 37").
- Pass 38: a mist of spray hangs at the cascade's foot and drifts on the breeze (PLAN "Loop pass 38").
- Pass 39: sun shafts under the canopy, gone in the open and under a cloud (PLAN "Loop pass 39").
- Pass 40: the walker's bare feet leave a fading trail of prints in the sand (PLAN "Loop pass 40").
- Pass 41: rooting boars throw up puffs of soil at the snout (PLAN "Loop pass 41").
- Pass 42: the hermit crabs drag thin fading lines in the sand (PLAN "Loop pass 42").
- Pass 43: now and then a fish breaks the surface over a school (PLAN "Loop pass 43").
- Pass 44: breakers throw spray up the offshore rocks in time with their foam (PLAN "Loop pass 44").
- Pass 45: a surfacing dolphin blows a puff of spray from the blowhole (PLAN "Loop pass 45").
- Pass 46: coconut crabs walk to lying coconuts and work them with their claws (PLAN "Loop pass 46").
- Pass 47: the resident stands facing the fire at the campfire pause (PLAN "Loop pass 47").
- Pass 48: an ant trail bends to a fruit lying near its food end (PLAN "Loop pass 48").
- Pass 49: three dragonflies patrol the cascade's plunge pool (PLAN "Loop pass 49").
- Pass 50: two frogs keep the banks of the plunge pool (PLAN "Loop pass 50").
- Pass 51: a settled stingray bursts out of the sand when the swimmer nears (PLAN "Loop pass 51").
- Pass 52: the grass bends under the nearest animals and the resident as they pass (PLAN "Loop pass 52").
- Pass 53: the bushes and understory are brushed aside by passing bodies (PLAN "Loop pass 53").
- Pass 54: the butterflies scatter from passing animals and the resident (PLAN "Loop pass 54").
- Pass 55: a dove on the ground flushes from a passing animal (PLAN "Loop pass 55").
- Pass 56: a fish breaking the surface makes a small plip (PLAN "Loop pass 56").
- Pass 57: mice, rats, geckos and skinks scatter from passing animals (PLAN "Loop pass 57").
- Pass 58: the boars' wallows show as dark churned mud at the bank (PLAN "Loop pass 58").
- Pass 59: fallen twigs lie scattered on the forest floor under the crowns (PLAN "Loop pass 59").
- Pass 60: an alert deer swings both ears toward the watcher (PLAN "Loop pass 60").
- Pass 61: the resident raises a hand to a player who stops close by (PLAN "Loop pass 61").
- Pass 62: a startled octopus jets for its den behind a cloud of ink (PLAN "Loop pass 62").
- Pass 63: a crocodile sliding off the bank goes under with a splash and a ring (PLAN "Loop pass 63").
- Pass 64: the deer keep seven metres clear of the campfire (PLAN "Loop pass 64").
- Pass 65: the bear walks to lying fruit and coconuts and eats them (PLAN "Loop pass 65").
- Pass 66: a surfacing sea turtle breathes out a small puff of spray (PLAN "Loop pass 66").
- Pass 67: leaves out of the stream mouth drift on across the lagoon (PLAN "Loop pass 67").
- Pass 68: the boars stand off from the bear and trot away when it comes close (PLAN "Loop pass 68").
- Pass 69: the hut's roof ridge is a perch for the birds (PLAN "Loop pass 69").
- Pass 70: seabirds done hunting raft on the sea with folded wings before climbing back to their routes (PLAN "Loop pass 70").
- Pass 71: fruit doves come down to the plunge pool's bank to bathe in wing-flicking, head-dipping bouts (PLAN "Loop pass 71").
- Pass 72: goats square up to a neighbour and spar nose to nose, taking turns lowering their heads (PLAN "Loop pass 72").
- Pass 73: a frightened beach crab near its colony's burrow bolts down the hole and comes up once the beach is clear (PLAN "Loop pass 73").
- Pass 74: a parrot perched on a fruiting lobe feeds there in bouts and stays twice as long (PLAN "Loop pass 74").
- Pass 75: a hermit crab still pressed after its retreat pulls into its shell until the beach is clear (PLAN "Loop pass 75").
- Pass 76: at a gust front the crowns let go leaves that spiral down, drift downwind and lie on the floor (PLAN "Loop pass 76").
- Pass 77: a bird settling on a branch and a monkey moving through a crown shake a leaf loose (PLAN "Loop pass 77").
- Pass 78: a fish school passing the jetty turns in and holds off the end of the deck in its shade a while (PLAN "Loop pass 78").
- Pass 79: a roosting fruit bat's stir in its crown lets a fruit drop and a leaf come loose (PLAN "Loop pass 79").
- Pass 80: a mouse or rat setting out to forage goes to fruit lying within six metres and nibbles there (PLAN "Loop pass 80").
- Pass 81: a fly swarm with fermenting fruit within ten metres shifts over it and hangs low there (PLAN "Loop pass 81").
- Pass 82: a resting gecko stalks a low-hanging fly and lunges, and the flies dart from the snap (PLAN "Loop pass 82").
- Pass 83: a basking skink a cloud's shadow covers gets up and forages, and basks only in the sun (PLAN "Loop pass 83").
- Pass 84: a cicada falls silent when the walker or an animal comes within seven metres and starts up again once they have gone; spiders-bright capture restored (PLAN "Loop pass 84").
- Pass 85: every fourth forage without a coconut a crab climbs a living palm trunk, pauses near three metres up and comes down (PLAN "Loop pass 85").
- Pass 86: a monkey the walker closes on gives an alarm call as it breaks into flight (PLAN "Loop pass 86").
- Pass 87: an octopus lying still takes the sand's tones and flashes back to its own colours when it moves or is startled (PLAN "Loop pass 87").
- Pass 88: a bolting deer flags its tail up and a grazing or resting deer flicks it every few seconds (PLAN "Loop pass 88").
- Pass 89: the resident's pause at the campfire tends the fire, its tongues and plume standing taller while they stay (PLAN "Loop pass 89").
- Pass 90: a snake's head within four metres of a monkey sends it off with the alarm (PLAN "Loop pass 90").
- Pass 91: a snake's head come within three metres of a perched bird flushes it (PLAN "Loop pass 91").
- Pass 92: a pool frog fleeing the walker leaps into the pool, swims under, surfaces and hops back to its bank (PLAN "Loop pass 92").
- Pass 93: a frog's dive scatters the fish holding in the plunge pool (PLAN "Loop pass 93").
- Pass 94: one dragonfly patrol in three ends on a perch with the wings held still (PLAN "Loop pass 94").
- Pass 95: a school parts around the shark passing through it, as around the swimmer (PLAN "Loop pass 95").
- Pass 96: the shark passing within nine metres turns the rays away and bursts a settled one from the sand (PLAN "Loop pass 96").
- Pass 97: a dolphin within twenty-eight metres of the shark leaves its route to keep its berth (PLAN "Loop pass 97").
- Portable evidence: Web package of 2026-10-20 08:29 (pass 86); twelve Chrome scopes passed (PLAN "Portable evidence for loop passes 39–86").
- Pass 98: the shark within eighteen metres sends a surfacing turtle under and the rest away from it (PLAN "Loop pass 98").
- Pass 99: a fish breaking the surface within sixty metres draws the present shark that way for a few seconds (PLAN "Loop pass 99").
- Pass 100: a seabird's strike brings the others within a hundred and twenty metres in to hunt the same school (PLAN "Loop pass 100").
- Pass 101: the goat sentinel coming alert at the walker gives a loud alarm bleat ahead of the herd's other calls (PLAN "Loop pass 101").
- Pass 102: a boar the bear sends off gives a loud alarm grunt ahead of the herd's other calls (PLAN "Loop pass 102").
- Pass 103: the deer sentinel coming alert at the walker snorts an original alarm heard within fifty metres (PLAN "Loop pass 103").
- Pass 104: the resident's footfalls sound on the ground under them, heard within twenty-four metres (PLAN "Loop pass 104").
- Pass 105: on the walk the resident whistles an original short phrase every half minute (PLAN "Loop pass 105").
- Pass 106: a bird flushing from a perch or the ground claps its wings, heard within twenty metres (PLAN "Loop pass 106").
- Pass 107: a flushing bird lets a pale feather go that rocks down slower than a leaf (PLAN "Loop pass 107").
- Pass 108: a leaf or feather that comes down on the stream is taken by the current and drifts away (PLAN "Loop pass 108").
- Pass 109: a stream leaf drifting onto rising sand washes up and lies on the strand (PLAN "Loop pass 109").
- Pass 110: a crocodile coming alert on its bank hisses at the walker and gapes (PLAN "Loop pass 110").
- Pass 111: a dolphin's leap brings the seabirds within a hundred and sixty metres in to hunt the school of the moment (PLAN "Loop pass 111").
- Pass 112: the resident's tour calls at the lookout's ramp foot and looks up the ramp at the platform (PLAN "Loop pass 112").
- Pass 113: a perched bird flushes from the resident or an animal walking up under its perch (PLAN "Loop pass 113").
- Pass 114: animals darken in the sun shadows of snags, understory, bushes and structures through a CPU `SunShadeField` read by the canopy receiver sample (PLAN "Loop pass 114").
- Pass 115: a deer whose rest comes on sunlit ground walks to the nearest shade within ten metres and lies up there; the capture tool rejoins diagnostics split by the asset logger (PLAN "Loop pass 115").
- Pass 116: a deer resting in the shade beds down onto folded legs (`bedAmount`, `deer-bedded` QA viewpoint) and is up within half a second of an alarm (PLAN "Loop pass 116").
- Pass 117: a perched bird preens, the head turned back into one wing for 2.6 s every 11 s or so (`preenAmount`, `bird-preen` QA viewpoint) (PLAN "Loop pass 117").
- Pass 118: a basking crocodile gapes, holding its jaws open for 14 s a few seconds into each lie-up on the bank (PLAN "Loop pass 118").
- Pass 119: a resting boar lies down onto folded legs a second and a half into an undisturbed rest and is up within half a second of an alarm (`lieAmount`, `boar-lying` QA viewpoint) (PLAN "Loop pass 119").
- Pass 120: a settled stingray flicks sand over its back two seconds in and lies half-buried in the sand's tones until it moves (`buryAmount`, `stingray-buried` QA viewpoint) (PLAN "Loop pass 120").
- Pass 121: the deer and the boars press cloven hoofprints into wet sand, the stream's mud and the drinking bank as they walk (`FootprintTrail` kinds and strides) (PLAN "Loop pass 121").
- Pass 122: a gust front passing the windward beaches streams pale sand off the dry crests downwind (`BlownSandField`, 147 sources) (PLAN "Loop pass 122").
- Pass 123: a bird flushed by a snake mobs it, circling low over its head for 8 s and chattering the alarm (`GenerateBirdChatter`, `WildlifeCallType::Chatter`) (PLAN "Loop pass 123").
- Pass 124: a snake's head within 3.5 m sends a mouse or rat bolting and to ground for 6 s or more (`RodentPopulation::SetSnakes`) (PLAN "Loop pass 124").
- Pass 125: snakes flick a forked tongue every few seconds for half a second (`SnakeState::tongue`, drawn by the renderer; the snake-head view holds it out) (PLAN "Loop pass 125").
- Pass 126: a resting goat lies down onto folded legs two seconds into an undisturbed rest, the pose re-grounded at the soles (`lieAmount`, `goat-lying` QA viewpoint) (PLAN "Loop pass 126").
- Pass 127: a monkey idle on the ground scratches its flank with a forelimb for 1.4 s every 14 s or so (`scratchAmount`, `monkey-scratch` QA viewpoint) (PLAN "Loop pass 127").
- Pass 128: a low mist hangs over the seasonal wetland, breathing, drifting with the trade wind and wrapping back in at the windward edge (`WetlandMistField`) (PLAN "Loop pass 128").
- Pass 129: a running boar, deer or goat on dry sand or bare soil kicks up puffs of earth behind its hooves (`HoofDustField`, drawn by the rooting-dust renderer) (PLAN "Loop pass 129").
- Pass 130: the bear lies down onto folded legs two seconds into an undisturbed rest, the paws re-grounded with the bones kept (`lieAmount`, `bear-lying` QA viewpoint) (PLAN "Loop pass 130").
- Pass 131: prints on the wet sand at the water's edge are taken back by the swash within 45 s, those higher up over four minutes (`Footprint::life`) (PLAN "Loop pass 131").
- Pass 132: at the campfire the resident crouches to tend the fire, hands to the flames (`ResidentMotion::SetCrouch`, `resident-tending` QA viewpoint) (PLAN "Loop pass 132").
- Pass 133: a fish rolling through a turn flashes silver along its flanks, brightest parting from the shark (`FishState::flash`) (PLAN "Loop pass 133").
- Pass 134: a dolphin's leap comes down in a splash: the sea's foam ring and the positional splash at the re-entry (`DolphinPod::Splash`) (PLAN "Loop pass 134").
- Pass 135: the shark's dorsal fin riding just under the surface cuts a foam wake that spreads and fades behind it (`FinWakeField`) (PLAN "Loop pass 135").
- Pass 136: a butterfly at rest in the sun basks with open wings and closes them under a cloud's shadow (`ButterflyMeadow::SetCloudClock`, `BaskingCount`) (PLAN "Loop pass 136").
- Pass 137: a beach crab paused in the open waves a claw, raised and swept for a couple of seconds (`CrabState::waveAmount`) (PLAN "Loop pass 137").
- Pass 138: a calling frog's throat swells and pulses under its chin through the call (`Soundscape::ActiveFrogCall`, `FrogPopulation::NoteCall`, `FrogState::throat`) (PLAN "Loop pass 138").
- Pass 139: a basking skink in the sun displays with bouts of push-ups, the front body lifted about the hips (`SkinkState::pushUp`) (PLAN "Loop pass 139").
- Pass 140: a stalking gecko wags its tail, the tip swaying wide and fast until the lunge (`GeckoState::tailWag`) (PLAN "Loop pass 140").
- Pass 141: the fish, stingrays, sea turtles and the shark cast soft shadows on the sandy seabed (`SeabedShadowProjection`, `SeabedShadowRenderer`) (PLAN "Loop pass 141").
- Pass 142: at the jetty-foot pause the resident casts a line, rod in the right hand and a float past the deck's end (`ResidentMotion::SetFishingSpot`, `ResidentState::fishing`) (PLAN "Loop pass 142").
- Pass 143: the fishing float rides the swell and the rod is lifted every dozen seconds (`floatBob`, `rodLift`, `resident-fishing` QA viewpoint) (PLAN "Loop pass 143").
- Pass 144: a boar trotting off or a deer bounding away drums its hoofbeats (`GenerateHoofbeats`, `WildlifeCallType::Hoofbeats`, herd retreat/bolt counts) (PLAN "Loop pass 144").
- Pass 145: a turtle foraging low over the bottom dips its head to graze, a puff of sand at each bite (`SeaTurtleState::graze`, `TurtleGrazeField`) (PLAN "Loop pass 145").
- Pass 146: a patrolling dragonfly dips to touch the water every few seconds, a fleck of spray at the touch (`DragonflyState::dip`, `DragonflyDipField`) (PLAN "Loop pass 146").
- Pass 147: a hanging fruit bat fans its half-opened wings for a few seconds every quarter minute to cool (`FruitBatState::fan`, `FruitBatColony::FanCount`) (PLAN "Loop pass 147").
- Pass 148: a crocodile under way at the surface draws a faint wake of foam behind its snout (`CrocodileWakeField`, drawn by the fin-wake renderer) (PLAN "Loop pass 148").
- Pass 149: a boar stepping out of its wallow stands and shakes, mud flying from its flanks (`BoarState::shake`, `BoarHerd::LastShake`, `MudFleckField`) (PLAN "Loop pass 149").
- Pass 150: every few seconds a goat flicks its short tail side to side (`GoatState::tailSwing`, `BuildGoatMesh` tail swing) (PLAN "Loop pass 150").
- Pass 151: every few seconds a deer twitches one ear back and out at a fly, the ears taking turns (`DeerState::earTwitch`/`earSide`) (PLAN "Loop pass 151").
- Pass 152: the bear standing lifts its head to sniff the air, and holds it up facing the walker in its alert range (`BearState::sniff`, `BearMotion::SniffCount`) (PLAN "Loop pass 152").
- Pass 153: a perched bird stretches one wing out and down between its preens (`BirdState::stretchAmount`/`stretchSide`, `BirdFlock::StretchCount`) (PLAN "Loop pass 153").
- Pass 154: a ground snake lifts its head to look at the walker standing off within fourteen metres, low to slip away closer (`SnakeState::headLift`) (PLAN "Loop pass 154").
- Pass 155: a paused hermit crab feeds, its small claw picking at the sand and lifting to its mouth (`HermitCrabState::feedAmount`/`feedPhase`) (PLAN "Loop pass 155").
- Pass 156: a resting gecko licks an eye clean with its tongue, the eyes taking turns (`GeckoState::eyeLick`/`lickSide`) (PLAN "Loop pass 156").
- Pass 157: the resident's head turns to the walker within fourteen metres ahead of them (`ResidentState::lookYaw`) (PLAN "Loop pass 157").
- Pass 158: a resting mouse or rat sits up to groom, washing its face with both forepaws (`RodentState::groom`/`groomPhase`) (PLAN "Loop pass 158").
- Pass 159: a butterfly settled at its flower uncurls its proboscis to feed and rolls it up to leave (`ButterflyState::proboscis`, `ButterflyMeadow::ProbeCount`) (PLAN "Loop pass 159").
- Pass 160: a resting frog blinks, its eyes drawn down into its head for a moment (`FrogState::blink`, `FrogPopulation::BlinkCount`) (PLAN "Loop pass 160").
- Pass 161: a foraging skink flicks its tongue at the ground ahead every few seconds (`SkinkState::tongue`, `SkinkPopulation::FlickCount`) (PLAN "Loop pass 161").
- Pass 162: a ground spider the walker stands over within four metres rears, front legs raised in a threat display (`SpiderState::display`, `SpiderColony::DisplayCount`) (PLAN "Loop pass 162").
- Pass 163: a beetle at rest in the open lifts its wing cases a crack and holds them up until it moves on (`BeetleState::elytraLift`, `BeetlePopulation::FanCount`) (PLAN "Loop pass 163").
- Pass 164: a calling cicada's abdomen swells and drums with the call and its wings tremble (`CicadaState::call`/`tymbalPhase`) (PLAN "Loop pass 164").
- Pass 165: a perched bird shakes its feathers out as each preen ends (`BirdState::ruffleAmount`, `BirdFlock::RuffleCount`) (PLAN "Loop pass 165").
- Pass 166: the shark on patrol drops its jaw in a gape every several seconds, oftener investigating a breach (`SharkState::jawOpen`, `SharkPatrol::GapeCount`) (PLAN "Loop pass 166").
- Pass 167: every fourth surfacing after the leap's is a dolphin spy-hop, head-up out of the water (`DolphinBehavior::SpyHop`, `DolphinPod::SpyHopCount`) (PLAN "Loop pass 167").
- Pass 168: setting out to swim or startled for its den, an octopus jets, mantle drawn in and arms straightened (`OctopusState::jet`, `OctopusGroup::JetCount`) (PLAN "Loop pass 168").
- Pass 169: a goat flicks an ear back at a fly now and then, the ears taking turns (`GoatState::earFlick`, `BuildGoatMesh` ear flick) (PLAN "Loop pass 169").
- Pass 170: a crocodile blinks, its eyes shut for a moment every eight seconds or so (`CrocodileState::blink`, `CrocodileGroup::BlinkCount`) (PLAN "Loop pass 170").
- Pass 171: the bear shakes its head as a sniff ends, a quick swing side to side (`BearState::headShake`/`shakePhase`, `BearMotion::HeadShakeCount`) (PLAN "Loop pass 171").
- Pass 172: a turtle squeezes its eyes shut as its head breaks the surface for a breath, and blinks slowly under (`SeaTurtleState::blink`, `SeaTurtleGroup::BlinkCount`) (PLAN "Loop pass 172").
- Pass 173: a settled ray breathes with a slow wave running back along each wing's edge (`StingrayState::marginRipple`/`ripplePhase`) (PLAN "Loop pass 173").
- Pass 174: a hanging bat stretches one wing out and down now and then, the wings taking turns (`FruitBatState::stretch`/`stretchSide`, `FruitBatColony::StretchCount`) (PLAN "Loop pass 174").
- Pass 175: an ant pausing on the trail sweeps its antennae, feeling the way ahead (`AntState::antennaSweep`/`sweepPhase`, `AntTrails::SweepCount`) (PLAN "Loop pass 175").
- Pass 176: a deer standing alert at the walker stamps a forefoot now and then (`DeerState::stamp`, `DeerHerd::StampCount`) (PLAN "Loop pass 176").
- Pass 177: every third pause the resident stretches, both arms straight up for a couple of seconds (`ResidentState::stretch`, `ResidentMotion::StretchCount`) (PLAN "Loop pass 177").
- Pass 178: a fly at the fruit sets down on it every other hover, wings still, and rubs its forelegs (`FlyState::landed`/`rubPhase`, `FlySwarms::LandingCount`) (PLAN "Loop pass 178").
- Pass 179: a coconut crab's claws knock on the shell every couple of seconds, an original positional double click (`CoconutCrabGroup::LastKnock`, `Soundscape::NoteClawKnock`, `GenerateClawKnock`) (PLAN "Loop pass 179").
- Pass 180: a goat at rest, undisturbed, chews its cud, the jaw and beard swinging side to side (`GoatState::chew`, `BuildGoatMesh` chew) (PLAN "Loop pass 180").
- Pass 181: a deer at rest, undisturbed, chews its cud, the muzzle working side to side (`DeerState::chew`, `DeerHerd::ChewingCount`) (PLAN "Loop pass 181").
- Pass 182: the bear flicks an ear back at a fly every few seconds, the ears taking turns (`BearState::earFlick`/`earSide`, `BearMotion::EarFlickCount`) (PLAN "Loop pass 182").
- Pass 183: a boar rooting with its snout down works the snout side to side (`BoarState::snoutSway`, `BoarHerd::SnoutingCount`) (PLAN "Loop pass 183").
- Pass 184: a crab bolting for its burrow folds its eyestalks flat and raises them again as it comes up (`CrabState::eyeFold`, `CrabColony::EyeFoldedCount`) (PLAN "Loop pass 184").
- Pass 185: a crocodile under way in the water folds its legs back along its flanks and swims on its tail (`CrocodileState::legFold`, `CrocodileGroup::FoldedLegCount`) (PLAN "Loop pass 185").
- Pass 186: a mouse or rat sweeps its whiskers with each sniff, fully on the forage and a little at rest (`RodentState::whiskerTwitch`, `RodentPopulation::WhiskingCount`) (PLAN "Loop pass 186").
- Pass 187: every other surfacing a dolphin slaps its tail, the flukes raised and brought down with a splash (`DolphinState::tailSlap`, `DolphinPod::TailSlapCount`) (PLAN "Loop pass 187").
- Pass 188: a seabird lifting off its raft shakes the water from its feathers (`BirdFlock::WaterShakeCount`, the ruffle in flight) (PLAN "Loop pass 188").
- Pass 189: a school holding in the shade fans its pectoral fins to hover, folding them back as it moves on (`FishState::finFan`/`finPhase`, `FishSchools::FanningCount`) (PLAN "Loop pass 189").
- Pass 190: a foraging octopus settled on the bottom reaches one arm out into the crevices every few seconds, the arms taking turns (`OctopusState::probe`/`probeArm`, `OctopusGroup::ProbeCount`) (PLAN "Loop pass 190").
- Pass 191: a turtle moving slow draws a front flipper up to rub its eye, the flippers taking turns (`SeaTurtleState::scratch`/`scratchSide`, `SeaTurtleGroup::ScratchCount`) (PLAN "Loop pass 191").
- Pass 192: a skink basking in the sun flattens and spreads its body to soak the heat (`SkinkState::flatten`, `SkinkPopulation::FlattenedCount`) (PLAN "Loop pass 192").
- Pass 193: walking, a hermit crab lifts its shell clear of the sand and it rocks with the steps, set down at a pause (`HermitCrabState::shellCarry`, `HermitCrabGroup::CarryingCount`) (PLAN "Loop pass 193").
- Pass 194: a resting frog's throat rises and falls with its breathing, a small swell under the chin between calls (`FrogState::gular`, `FrogPopulation::PumpingCount`) (PLAN "Loop pass 194").
- Pass 195: a gecko licks its lips as a hunt ends (`GeckoState::lipLick`, `GeckoPopulation::LipLickCount`) (PLAN "Loop pass 195").
- Pass 196: a perched dragonfly raises its abdomen toward the sun, the obelisk, and levels it to lift off (`DragonflyState::obelisk`, `DragonflySwarm::ObeliskCount`) (PLAN "Loop pass 196").
- Pass 197: a crab feeding among the strandings blows a small bubble from its mouth every couple of seconds (`CrabState::feeding`, `CrabBubbleField`) (PLAN "Loop pass 197").
- Pass 198: two ants meeting head to head on the trail pause to touch antennae (`AntTrails::MeetCount`) (PLAN "Loop pass 198").
- Pass 199: a ground spider at rest draws a front leg in and works it through its mouthparts, the legs taking turns (`SpiderState::groom`/`groomLeg`, `SpiderColony::GroomCount`) (PLAN "Loop pass 199").
- Pass 200: coming up from a graze a deer tosses its head (`DeerState::headToss`, `DeerHerd::TossCount`) (PLAN "Loop pass 200").
- Fix (operator report): flight crash from the blown sand overflowing its 18-puff dust buffer (renderer capacity + bounded uploads); screen-space canopy sun shafts removed (they striped the view in flight); sun shadow map texel-snapped with receiver-plane depth bias (PLAN "Fix pass (2026-10-20)").
- Pass 201: a perched bird scratches its head with a foot now and then, the feet taking turns (`BirdState::scratchAmount`/`scratchSide`, `BirdFlock::HeadScratchCount`); the inventory's misleading "large-object sun shadows 0" label renamed (PLAN "Loop pass 201").
- Pass 202: a dolphin's blowhole opens as its back breaks the surface for the breath (`DolphinState::blowhole`, `DolphinPod::BreathCount`) (PLAN "Loop pass 202").
- Pass 203: the goats, 85-110 m from the stream, drink at a rain-filled rock basin on their slope; 4 goats reach it (`GoatHerd::WaterholeSite`, `WaterholeRenderer`), closing backlog item 8's "0 goats" (PLAN "Loop pass 203").
- Pass 204: evidence for backlog items 12 and 19: a core check of the four yard props at the hut, a startup footstep-ground probe (wood, shallows, sand, rock, soil) and the walker's own shadow in the inventory (1 in jetty, beach, ground) (PLAN "Loop pass 204").
- Pass 205: evidence for backlog item 1: a `--no-sun-shadows` QA switch and a native `hut` comparison, 444 casters and long tree, hut and tower shadows over the yard with the map, the contact-batch fallback and an evenly lit yard without (PLAN "Loop pass 205").
- Pass 206: the sun shadows fade on a rounded square around the viewer, 52-63 m in the sun's view, instead of on the shadow map's edge, which jumped 8 m at every re-centre in flight; core check 0.27 % largest change per 2 cm across 10 re-centres against 91 % before (PLAN "Loop pass 206").
- Pass 207: the wreck, hut and helicopter keep their projected mass shadows beyond the sun shadow map, cross-faded vertex by vertex with the map's own fade instead of switched off (`shipwreck-wide` 1 mass shadow, `hut` 0) (PLAN "Loop pass 207").
- Pass 208: the load-sensitive GPU qualification supervisor fixture failed because its stub processes read marker files half-written; all four markers are now written and renamed atomically (2/18 failures at six concurrent copies before, 0/66 after) (PLAN "Loop pass 208").
- Pass 209: grass mesh tiers are chosen per patch by its own distance and rank, morphing over 5 m, drawn by quarter-cell ranges, instead of per 37.5 m cell by the cell centre (which thinned patches 51 m away in square blocks); about +25-30 % grass vertex work (PLAN "Loop pass 209").
- Pass 210: jungle understory, bushes and ground plants (and standing snags) swap meshes at staggered per-plant distances (+-12 %) and shrink into the ground over the last 22 % of their range instead of popping on six fixed circles (PLAN "Loop pass 210").
- Pass 211: canopy trees and palms shrink to nothing over the last 12 % of their range instead of a hard cut, and palms are judged each by its own distance instead of by their 85 m cell's centre (PLAN "Loop pass 211").
- Pass 212: rocks, logs, driftwood, reef habitat, beach details, tracks, fallen fronds and nuts shrink away over the end of their range with staggered mesh swaps, and the palm frond shading fades with distance instead of switching with the mesh (PLAN "Loop pass 212").
- Pass 213: deer, goats, boars, the bear and the resident use the land's fog and shrink away over the last 12 % of their range instead of fogging to pale silhouettes from 55-93 m on native (PLAN "Loop pass 213").
- Pass 214: spiders (fully fogged from 18 m before), crabs and their burrows, crocodiles and the small-fauna batch use the land's fog; all but the tiny animals shrink away at the end of their range (PLAN "Loop pass 214").
- Pass 215: the abandoned helicopter carries grime low on the body, a damp-green film on upward paint and rust on low bare metal in its vertex colours (PLAN "Loop pass 215").
- Pass 216: the hut's material shader receives the sun shadow map and the cumulus shade (the trees' and its own eaves' shadows fall on it); `--no-sun-shadows` captures pass again (PLAN "Loop pass 216").
- Pass 217: the natural props (rocks, logs, driftwood, beach details) and the wreck receive the sun shadow map and the cumulus shade (a log in a palm's shadow is now shaded with the ground) (PLAN "Loop pass 217").
- Pass 218: one shared `StructureShadowReceiver` material lets the jetty, rope bridge, lookout and helicopter receive the sun shadows and cumulus shade, BasicEffect kept as the fallback (PLAN "Loop pass 218").
- Pass 219: water receiving the sun shadows was tried and dropped: 0.17 % of a dedicated view changed (the shallows already show the shaded seabed) for sixteen map samples per water pixel (PLAN "Loop pass 219").
- Pass 220: grass fades and tiers and every plant, tree and prop shrink measure the true distance including the camera's height, so from the air no full-size disc of vegetation follows the camera (PLAN "Loop pass 220").
- Pass 221: a second flight crash path closed: the leaf renderer (14-leaf buffer) also drew the canopy leaf fall, measured at up to 35 leaves in one view; it now holds 48 and stops at its buffer; surf spray, deer and contact-shadow capacities corrected, shared puff renderers checked at compile time (PLAN "Loop pass 221").
- Pass 222: the resident's footprints are rounded sand-toned hollows instead of near-black stamps in the hoof prints' mud colours (PLAN "Loop pass 222").
- Pass 223: a new `SlopeScrubLayout` grows 1262 low coastal scrub bushes on the steep banks, foredunes and beach backs every other plant rule leaves bare (the bank behind the jetty's beach), drawn as extra jungle bushes with their own count (PLAN "Loop pass 223").
- Pass 531: adit rock walls on a fine grid with per-corner normals (from the bumps) and tones, no more flat panels; `mine-adit` `20261022T132643` vs `132848` (PLAN "Loop pass 531").
- Pass 530: adit puddles (11 irregular ovals beside/between the track, wet margins on a fine floor grid) with view-dependent lantern reflections; QA `mine-puddle` (proof line "N lantern glints") `20261022T132536`; dim-interior profile floors mean 20 / spread 10 (PLAN "Loop pass 530").
- Pass 529: lava lake spatter (a bubble bursts every ~3.7 s: flash + 14 glowing gobbets arcing and cooling); volcano halo buffer 1024 -> 2048 with overflow guard (pass 521's night glow could exceed it); `volcano-crater` `20261022T131631` (PLAN "Loop pass 529").
- Pass 528: mine bats squeak (`GenerateBatSqueaks`, `WildlifeCallType::Bat`, `Soundscape::NoteBatSqueaks`): a burst on flushing, one flying bat every 2.3 s while any fly; PCM tested in CoreTests; playback not visible in software captures (no wildlife call plays in 31 updates) (PLAN "Loop pass 528").
- Pass 527: bird droppings under the perches: white streaks down the coaster's port bow under the rail perch, whitewashed mastheads, whitened lifeboat keel; `freighter-birds` `20261022T130516`, `lifeboat` `130550` (PLAN "Loop pass 527").
- Pass 526: coaster rust weeps from scuppers, rail fittings and hawse pipes (varied thin streaks, sun-faded black) on a fine topside grid instead of evenly spaced bars; ship 62,923 tris; `freighter` `20261022T130102`, `freighter-birds` `130221` (PLAN "Loop pass 526").
- Pass 525: `SculptMeshBuilder::tintPerVertex` (+ per-corner `StructureMeshBuilder::Tri`): idol ahu blocks and fighter skin weather smoothly (no triangle camouflage), lifeboat in three strakes with sharp paint edges and smooth peeling; `idol` `20261022T125211`, `plane-wreck` `125204`, `lifeboat` `125342` (PLAN "Loop pass 525").
- Pass 524: signal fire of bowed driftwood and barked branches (crossing past the peak, snapped stubs, husk tinder); burnt ring of lofted sooted stones, charcoal lumps, half-burnt sticks; `signal-fire` `20261022T124750` (PLAN "Loop pass 524").
- Pass 523: mine chamber hewn rough (64x16 dome with recesses and knobs, per-vertex normals and tones, no crown star; hummocky grid floor; crystals rooted in the real surface; ore spill as lumps); `mine-bats` in dim-interior profile; `mine-chamber` `20261022T124334`, `mine-bats` `124326` (PLAN "Loop pass 523").
- Pass 522: 12 small bats in the mine (`MineBatColony`): hang in 3 clusters under the chamber dome, flush when walked under, leave down the adit at dusk to hunt before the portal, home at dawn; QA `mine-bats` `20261022T123437`, `mine-bats-night --time-of-day 18.6` `123534` (PLAN "Loop pass 522").
- Pass 521: volcano bombs as lumpy rocks (not boxes), ragged sulphur crust patches round the fumaroles, softer wall sulphur, plume puffs lit from the lava at night; `volcano` `20261022T122201`, night `122222`, crater `122259` (PLAN "Loop pass 521").
- Pass 520: shore birds perch on the landmarks (coaster mastheads + port bow rail, lifeboat keel, lighthouse gallery rail, whale skull; 4 seated at load via `BirdFlock::SettlePerched`); QA `freighter-birds` `20261022T121321` (PLAN "Loop pass 520").
- Pass 519: frame inventory reads "large-object sun shadows N" (hut 832, jungle 1620, shipwreck 355, helicopter 1832, freighter 419, plane-wreck 813), capture gate fails on zero; hut with/without map `20261022T115858`/`115953` (PLAN "Loop pass 519").
- Pass 518: the fighter wreck overgrown: creepers with leaves over the fuselage and tail, moss on the upturned skin, ferns round the belly (PLAN "Loop pass 518").
- Pass 517: the castaway's signal fire stacked ready (driftwood poles, palm fronds) and a burnt-out fire ring by HELP ME; QA view `signal-fire` (PLAN "Loop pass 517").
- Pass 516: four sulphur-crusted fumaroles steaming on the volcano's slopes (PLAN "Loop pass 516").
- Pass 515: the coaster's name MARIA K on both bows and the transom with PANAMA (stroke font on the plating) (PLAN "Loop pass 515").
- Pass 514: mine props (crates, pickaxe and shovel, rubble with a fallen cap, a smashed lantern, a lit lantern on the chamber floor); crates and rubble in the walkable plan (PLAN "Loop pass 514").
- Pass 513: the coaster's lifeboat keel up on the beach at (-119, -199) with oars and a life ring (LifeboatLayout/Renderer, QA view `lifeboat`, `20261022T113208`) (PLAN "Loop pass 513").
- Pass 512: evidence re-run of operator backlog items 1-20 on build ab4d78b7 (20 native views, all diagnostics present; see the backlog section above).
- Pass 511: StructureShadowReceiver fades the sun-shadow tint on faces turned from the sun: no canopy dapples on ceilings (the coaster mess), no double-darkened lee faces (PLAN "Loop pass 511").
- Pass 510: README gallery adds the old mine (portal, adit, crystals) and the volcano (day, lava, night) rows (`20261022T111703`..`111807`) (PLAN "Loop pass 510").
- Pass 509: mine drips-and-echo loop and volcano rumble/bubbles loop in the regional loop slot (MineSite/VolcanoSite::SoundPresence; mine muffles the outdoors) (PLAN "Loop pass 509").
- Pass 508: steel deck footsteps aboard the coaster (FootstepSurface::Metal, GenerateMetalFootstep: heel knock + ringing plate), gravel in the mine, ash/rock on the volcano (PLAN "Loop pass 508").
- Pass 507: small volcano on the eastern foothills at (174, 0): analytic walkable cone (25 m, rounded rim, gullies, old flow, lava blocks), lava-lake shader, crater glow and night halo over the grade, steam plume; QA views `volcano`, `volcano-crater` (`20261022T105022`..`105039`) (PLAN "Loop pass 507").
- Pass 506: old mine under the main mountain at (1, 7): 48 m timbered adit, 8 lanterns, crystal chamber, ore cart; walkable (MineLayout, PlayerMotion::SetMine), terrain cut in the shader, forest reserved; two-pass baked light (lanterns over the night grade); QA views `mine`, `mine-adit`, `mine-chamber` (`20261022T101820`..`101840`; night `102355`/`102409`) (PLAN "Loop pass 506").
- Pass 505: README gallery adds the coaster row (grounded-coaster, coaster-hold, coaster-crew; captures `20261022T094410`/`094427`/`094435`) (PLAN "Loop pass 505").
- Pass 504: five sailors' skeletons aboard the coaster (HumanSkeleton: skull with jaw/sockets/teeth, 22 vertebrae, ribs, pelvis, long bones, hands/feet, trouser rags, boots, caps; lying/sitting/slumped/curled poses); QA views `freighter-crew`, `freighter-mess` (profile `dim-interior-scene`) (`20261022T094410`..`094443`) (PLAN "Loop pass 504").
- Pass 503: grounded 30 m coaster at (-107, -221), walkable (FreighterLayout/Renderer, PlayerMotion::SetFreighter): ladder, deck, deckhouse, stair into the hold; QA views `freighter`, `freighter-deck`, `freighter-hold` (`20261022T092758`); skeletons next (PLAN "Loop pass 503").
- Pass 502: README gallery additions: 15 new 960x540 frames (landmarks, night/sunset/storm, frigatebird/octopus/coconut crab) from captures `20261022T090923`..`20261022T091121`, hash-gated in the manifest, licensed, packaged (PLAN "Loop pass 502").
- Pass 501: washed-up PET bottles (BeachLitterLayout/Renderer: 39 on the strandline, four kinds, two-pass see-through plastic, caps, torn labels, sand shade, 17 culled stretches); QA view `beach-litter` (`20261022T085855`) (PLAN "Loop pass 501").
- Pass 500: WWII fighter wreck (PlaneWreckLayout/Renderer: fuselage loft, broken tail, radial engine, bent prop, framed canopy, crumpled and torn wings with hinomaru, camo worn to aluminium); SculptMeshBuilder tints before staining; QA view `plane-wreck` (`20261022T083752`) (PLAN "Loop pass 500").
- Pass 499: lighthouse night light: lamp glow flaring as the two soft beams (140 m, 12 s a turn) sweep past (`20261022T082725` lighthouse at 21:30) (PLAN "Loop pass 499").
- Pass 498: lighthouse on the east coast rise (LighthouseLayout/Renderer: plinth, banded tower, door, windows, gallery, lantern, dome); QA view `lighthouse` (`20261022T082109`) (PLAN "Loop pass 498").
- Pass 497: HELP ME enlarged for aircraft: 7 m letters, ~40 m line, 580 stones in double rows on the south beach (`20261022T081423`) (PLAN "Loop pass 497").
- Pass 496: HELP ME in 149 stones on the north-east beach (StoneSignLayout/Renderer); QA view `help-sign` (`20261022T080350`) (PLAN "Loop pass 496").
- Pass 495: stone god (tiki idol on an ahu) on the highest clear inland rise (IdolStatueLayout/Renderer, SculptMeshBuilder); QA view `idol` (`20261022T075641`) (PLAN "Loop pass 495").
- Pass 494: whale skeleton detail: tapered neural spines/processes (Bones::Blade), jaw condyles and coronoid hooks, occipital condyles, rib heads, scattered small bones (`20261022T074841`) (PLAN "Loop pass 494").
- Pass 493: stranded whale skeleton on the north beach (WhaleSkeletonLayout/Renderer, 44 vertebrae, 24 ribs, rorqual skull, contact patches, sun map caster); QA view `whale-skeleton` (`20261022T074431`) (PLAN "Loop pass 493").
- Pass 492: two great frigatebirds (last two high birds) soar at 112-128 m, crooked wings, forked tail, never dive; QA view `frigatebird` (`20261022T070643`); full sweep 127/127 after 456-491 (PLAN "Loop pass 492").
- Pass 491: wet feet (25 s after the water) leave fading prints on the jetty/bridge/lookout planks, dry in 30 s (Footprint::y/strength, FootprintTrail deckY); core test only, no capture (PLAN "Loop pass 491").
- Pass 490: gusts clatter the nearest palm's fronds (GeneratePalmClatter, Soundscape::NotePalmClatter, every 2.5-5.5 s while gust > 0.5) (PLAN "Loop pass 490").
- Pass 489: octopus arms six-sided lit tubes (cross-section bug fixed), mottled, two sucker rows; mantle mottled; near budget 4326 (`20261022T062233` octopuses) (PLAN "Loop pass 489").
- Pass 488: turtle paddles smooth-lit per corner (faces from their grid neighbours), budget unchanged (`20261022T061749` turtles) (PLAN "Loop pass 488").
- Pass 487: turtle head smooth-lit with dark-centred, pale-rimmed scale plates and a round eye (pupil, iris, lid ring, highlight); near budget 5274 (`20261022T061343` turtles) (PLAN "Loop pass 487").
- Pass 486: star colours recoloured/uploaded only every 0.004 h or 0.001 rad of cumulus drift (~4/s), 0.23 ms + 326 KB per frame saved (PLAN "Loop pass 486").
- Pass 485: steam rises off the sunlit wet ground after rain (SunSteamField, own mist renderer, --wet-ground QA; `20261022T055602` east-clear-wet) (PLAN "Loop pass 485").
- Pass 484: evidence re-run of operator backlog items 1-20 on build 3e3a0e86 (20 native views, all diagnostics present; see the backlog section above).
- Pass 483: stars and Milky Way wheel about the celestial pole with the hour (DayNightCycle::SkyAt, full-sphere stars recoloured per frame) (PLAN "Loop pass 483").
- Pass 482: capture evidence as PNG, newest 400 native / 100 browser sets kept plus handoff-cited ones, left-over Chrome profiles removed (tools/capture_retention.py); captures now give capture.png (PLAN "Loop pass 482").
- Pass 481: LocalLightRenderer::DrawMesh caches the shared-corner map (resident by the fire 0.80 -> 0.14 ms, bit-identical) (PLAN "Loop pass 481").
- Pass 480: hut house geckos chirp at night (hut_geckos::ChirpBetween, GenerateGeckoChirp, Soundscape::NoteGeckoChirp) (PLAN "Loop pass 480").
- Pass 479: muddy boars on soaked ground (BoarHerd::SetWetness) (PLAN "Loop pass 479").
- Pass 478: deer shelter under crowns in a downpour (DeerHerd::SetRain, RainShelterCount) (PLAN "Loop pass 478").
- Pass 477: rain drums on the helicopter cabin (RainUnderShelter metal). Note: Chrome coast scope timed out at load ~20 after pass 473 (water/shipwreck views rendered); retry when the machine is quieter (PLAN "Loop pass 477").
- Pass 476: morning valley mist (new ValleyMistField; dew/wetness, 05:30-09:30) (PLAN "Loop pass 476").
- Pass 475: wet coats (wildlife_render::CoatWetness/WetCoatFactor; deer, goats, boars, bear) (PLAN "Loop pass 475").
- Pass 474: wet jetty/bridge/lookout planks and wet rocks/logs/driftwood (uWetness) (PLAN "Loop pass 474").
- Pass 473: silt plume off the stream mouth after rain (ocean uPlume, weather_now::Silt shared) (PLAN "Loop pass 473").
- Pass 472: frog chorus x(1 + 1.5 night + 1.5 wet), up to 4 (FrogCallScheduler::SetChorus) (PLAN "Loop pass 472").
- Pass 471: silty stream after rain (uSilt from wetness) (PLAN "Loop pass 471").
- Pass 470: rain erodes tyre ruts, shell drags, crocodile grooves (SetRain) (PLAN "Loop pass 470").
- Pass 469: rain erodes footprints (FootprintTrail::SetRain, x6 at full rain) (PLAN "Loop pass 469").
- Pass 468: mud footstep squelch (GenerateMudFootstep*, SetFootstepMud, NoteResidentStep mud flag) (PLAN "Loop pass 468").
- Pass 467: figures on the seabed refracted at the surface (uSea in the figure shader) (PLAN "Loop pass 467").
- Pass 466: walker/resident prints in mud (stream banks, soil at wetness >= 0.45; footprint_ground::Muddy) (PLAN "Loop pass 466").
- Pass 465: frame profile splits rain and local-lights out of time-of-day (read the next operator log before optimizing) (PLAN "Loop pass 465").
- Pass 464: static contact shadows in a GPU buffer (867 shadows / 71,820 vertices no longer restaged per camera move; fade + structure share in the vertex shader) (PLAN "Loop pass 464").
- Pass 463: beetle figures on level ground (PLAN "Loop pass 463").
- Pass 462: figure reaches x contactShadowDistanceScale (full 1, mobile .72, native .5); small figures 8/6/4 (PLAN "Loop pass 462").
- Pass 461: skink, frog, rat/mouse figures (nearest-eight small list) (PLAN "Loop pass 461").
- Pass 460: ground dove (body 0.20·scale over feet) and reef heron figures (PLAN "Loop pass 460").
- Pass 459: crab figures (GhostCrabLimbs/HermitCrabLimbs/CoconutCrabLimbs), nearest 8 within 18 m (PLAN "Loop pass 459").
- Pass 458: goats (GoatHerd::SetGroundPatches) and boars (BoarRenderer::SetGroundPatches) fit their feet to the patched drawn ground (PLAN "Loop pass 458").
- Pass 457: deer stand on the drawn ground (DeerHerd::SetGround; banks up to 0.26 m off at the drinking points) (PLAN "Loop pass 457").
- Pass 456: crocodiles lie on the drawn (stream-carved) ground via CrocodileGroup::SetGround (they hovered 0.45 m); CrocodileLimbs figures (PLAN "Loop pass 456").
- Pass 455: sweep 127/127; walker figure only from its own eye; figures on decks (support height), deck-edge cells skipped, reach grows with a drop (PLAN "Loop pass 455").
- Pass 454: bird/seabed/monkey shadows along the hour's light (seabed refracted), darkness x shadow strength (PLAN "Loop pass 454").
- Pass 453: land rocks, logs, driftwood cast into the sun shadow map (small visible effect: 0.45 m bias) (PLAN "Loop pass 453").
- Pass 452: underwater shafts lean to the hour's shading light, strength = shadow strength (PLAN "Loop pass 452").
- Pass 451: bear sun shadow (BearLimbs on BearPose; head by forage and sniff) (PLAN "Loop pass 451").
- Pass 450: goat and boar sun shadows (GoatLimbs on GoatGroundPose, BoarLimbs on FitBoarGroundPose) (PLAN "Loop pass 450").
- Pass 449: deer sun shadows (DeerLimbs from DeerRenderer joints; grid extent from the capsules) (PLAN "Loop pass 449").
- Pass 448: resident sun shadow = same renderer, limbs from the posed skeleton (walker_shadow::Lay/ResidentLimbs) (PLAN "Loop pass 448").
- Pass 447: walker sun shadow = 17-capsule figure along the hour's light (WalkerShadow/WalkerShadowRenderer, multiply by crown-shade tint only where sunlit); QA view walker-shadow (PLAN "Loop pass 447").
- Pass 446: far hermit crab shell = coarse coil (4x5 per turn, 360 vertices) (PLAN "Loop pass 446").
- Pass 445: AnemoneRenderer (new files) + ReefAnemone in ReefFishField (1 head in 6), clownfish pairs (palette 6 with white bars); QA view anemone (PLAN "Loop pass 445").
- Pass 444: ReefFishField (new files): chromis/tang clouds over the 12 nearest coral heads, hide within 1.8 m; FishRenderer palettes 4/5, size-scaled near LOD, separate draw (PLAN "Loop pass 444").
- Pass 443: reef habitat in patches: 2-5 satellite colonies per site (1425 total), reef budget 700-1800 in CoreTests + WorldReport (PLAN "Loop pass 443").
- Pass 442: CrocodileDragTrail grooves on sand/soil out of water; DragSegment.halfWidth; drag renderer 3-strip groove (PLAN "Loop pass 442").
- Pass 441: goat cloven prints in the basin's wet soil ring (goatPrints_, forefeet 0.4 m ahead) (PLAN "Loop pass 441").
- Pass 440: resident bare-foot FootprintTrail on sand/shallows (residentPrints_) (PLAN "Loop pass 440").
- Pass 439: ground spiders: by day a quarter drawn (AwakeSpiders, within 40 m), all at night; close-ups keep all (PLAN "Loop pass 439").
- Pass 438: dragonflies perch in rain > 0.25 (DragonflySwarm::SetRain, not with a held clock); dragonflies+showers QA view shows none (roost rule, pre-existing) (PLAN "Loop pass 438").
- Pass 437: butterflies settle with folded wings in rain > 0.25 (ButterflyMeadow::SetRain, RestingCount) (PLAN "Loop pass 437").
- Pass 436: rain sound by cover: RainUnderShelter (canopy louder/deeper, cave/bunker muffled), Soundscape::SetRainShelter (PLAN "Loop pass 436").
- Pass 435: deer legs slimmer (knee 0.044, shank 0.040-0.030, pastern 0.031-0.024, smaller hooves) (PLAN "Loop pass 435").
- Pass 434: turtle front flippers curl back to a narrow tip; upper face two strips (scales+seams leading, paler skin trailing); NearVertexCount 5142 (PLAN "Loop pass 434").
- Pass 433: wreck faces relit by the hour (LitColor rule ratio in the wreck shader) (PLAN "Loop pass 433").
- Pass 432: StructureShadowReceiver bakedAmbient (0.46) relights jetty/bridge/lookout faces by the hour; helicopter unchanged (PLAN "Loop pass 432").
- Pass 431: hut faces relit by the hour (derivative face normal, hour/fixed light ratio in HutMaterial) (PLAN "Loop pass 431").
- Pass 430: terrain relit by the hour's sun in its vertex shader (PackTerrainLight bearing in data.z; exact at 14:00) (PLAN "Loop pass 430").
- Pass 429: quad engine floods in water over the airbox (QuadBike::AirboxFlooded, quadFlooded_, 2 s restart); QA view quad-sea (PLAN "Loop pass 429").
- Pass 428: stream-bank stones darkened (wet) in the props rock shader via StreamRoute in GLSL (PLAN "Loop pass 428").
- Pass 427: rain rings on the sea and the stream (rainRingsAt, uRain from weather_now) (PLAN "Loop pass 427").
- Pass 426: coconut crab body 0.27 x 0.36 (was 0.34 x 0.29), long fanned legs (toes 1.06), front parts forward, abdomen back (PLAN "Loop pass 426").
- Pass 425: full sweep 124/124 ok; fish QA camera >= 0.45 m over still water (near plane cut wave crests) (PLAN "Loop pass 425").
- Pass 424: hermit crab near shell = 3-turn logarithmic whorl tube (30x9), aperture at the front; budget 4008 (PLAN "Loop pass 424").
- Pass 423: damp tyre prints: quadTreadWater_ per wheel -> WheelOnGround::wetTread, TyreTrackField prints firm ground while wet (75 s) (PLAN "Loop pass 423").
- Pass 422: monkey tail radius .065 -> .048 (was .085 -> .040), within the tail tests (PLAN "Loop pass 422").
- Pass 421: frogs: 14-sided body/head lofts, round eyes on top of the head with bronze iris + horizontal pupil, blotches (AppendGeckoLoft<N, Sides>) (PLAN "Loop pass 421").
- Pass 420: ground doves stand on visible legs, breast up 0.20 rad (stanceLift 0.075), longer bill (PLAN "Loop pass 420").
- Pass 419: reef heron slimmer body, folded wings (coverts/primaries), back and breast plumes, tapered neck (PLAN "Loop pass 419").
- Pass 418: near beach crab carapace 0.42 x 0.31 (was 0.515 x 0.198), legs/claws/eyes/mouth/marks moved with it (PLAN "Loop pass 418").
- Pass 417: beach morning glory (BeachVinePlacement, CreateBeachVineGeometry, 141 crowns, laid along the sand, seaward +X); QA view beach-vine (PLAN "Loop pass 417").
- Pass 416: fallen logs: girth/axis (butt flare, taper, bow, knots), onTrunk for moss/fungi/stubs, layered bracket fungi, lichen colonies (PLAN "Loop pass 416").
- Pass 415: StreamRingField (16 rings) noted by quad wheels, waders, frog dives, landing leaves; stream shader uRings[16]; quad-stream captured mid-ford (wait 13 updates) (PLAN "Loop pass 415").
- Pass 414: quad water sounds: SetTyres(wading) fades the crunch, GenerateQuadWadeLoop churning loop, GenerateQuadSplash on running in > 2.5 m/s; runtime test ExerciseQuadWade (PLAN "Loop pass 414").
- Pass 413: walker wades the stream: FootstepGroundAt shallows in stream water > 4 cm, soundscape depth = WalkerWaterDepth (sea or stream); StreamWaterAt (was QuadStreamBed); footstep probe checks the stream (PLAN "Loop pass 413").
- Pass 412: RenderedTerrainSurface::SetLocalPatches (stream bed + wreck scour) on drawnGround_ and quadSurface_: marks, shadows and quad ruts lie in the carved channel (PLAN "Loop pass 412").
- Pass 411: quad fords the stream: StreamBedPatch bed as ground in the channel, StreamCourse::WaterLevel for the water depth (spray, no ruts), QuadBike wading drag per wetted tyre; QA view quad-stream (PLAN "Loop pass 411").
- Pass 410: quad collides with the bear, boars, goats, deer, crocodiles and the resident (circles); QA view quad-bear (PLAN "Loop pass 410").
- Pass 409: quad shadow pools fade and the sun silhouette is skipped where they would hang off the surface (jetty edge) (PLAN "Loop pass 409").
- Pass 408: quad blocked by the lookout ramp where it is under 1.9 m above the ground; QA view quad-lookout (PLAN "Loop pass 408").
- Pass 407: quad blocked by the helicopter and the rope bridge, carried on the jetty deck (no ruts/spray there); QA views quad-helicopter, quad-jetty (PLAN "Loop pass 407").
- Pass 406: TyreTrackRenderer caches each rut piece's ground points per slot (PLAN "Loop pass 406").
- Pass 405: boar canopy tint from three body stations per boar instead of per vertex (operator saw boars 7.7 ms) (PLAN "Loop pass 405").
- Pass 404: raining closed cloud deck gets noise rolls/billows on the cloud plane, darker base, pale under-edge band at the horizon (PLAN "Loop pass 404").
- Pass 403: terrain puddles reflect the canopy (dark leaves, 25% sky) by canopy transmission, softer edge, troubled Fresnel (PLAN "Loop pass 403").
- Pass 402: deer Ellipsoid/LoftTube light each corner once via a scratch grid (bit-identical) (PLAN "Loop pass 402").
- Pass 401: resident HumanMeshBuilder lights each grid corner once (bit-identical; pass ~2x cheaper) (PLAN "Loop pass 401").
- Pass 400: quad drives into the cave and the bunker on their floors and against their walls (QuadSite, QuadWallBlocked, chase camera kept under the bunker roof); QA views quad-bunker, quad-cave (PLAN "Loop pass 400").
- Pass 399: monkey limb radii slimmer from the elbow/knee down (mantles unchanged: the gait test pins them broad) (PLAN "Loop pass 399").
- Pass 398: monkeys capture uses the QA clock (UsesMonkeyCaptureClock); deterministic, no leaf in frame (PLAN "Loop pass 398").
- Pass 397: fish near LOD 72 -> 28 m, deer near 48 -> 30 m; full sweep after 369-396 passed (115 views) (PLAN "Loop pass 397").
- Pass 396: turtle skin palette olive-grey; flippers banded scales with pale seams and a pale leading margin (PLAN "Loop pass 396").
- Pass 395: ContactShadowRenderer drapes the moving ellipses on drawnGround_ (DrawnOrAnalyticHeight); static patches unchanged (PLAN "Loop pass 395").
- Pass 394: bird/seabed shadows, shell drags and footprint trails use DrawnOrAnalyticHeight via IslandGame::drawnGround_ (PLAN "Loop pass 394").
- Pass 393: tyre ruts use DrawnOrAnalyticHeight (drawn mesh, ~10 ns) instead of IslandTerrain::Height per vertex per frame (operator saw contact-shadows 20-66 ms while riding) (PLAN "Loop pass 393").
- Pass 392: turtle nests as soft churned pits with clods; crawls and nests laid on RenderedTerrainSurface heights (PLAN "Loop pass 392").
- Pass 391: TurtleTrackRenderer draws the nesting crawls as soft blended ribbons (trough, tail drag, irregular paired gouges); NaturalPropRenderer skips turtleTrace && !turtleNest (PLAN "Loop pass 391").
- Pass 390: driftwood tapers from a flared root to a splintered broken tip, underside kept on the sand line; near 8x9, far 3x5 (PLAN "Loop pass 390").
- Pass 389: driftwood material bleached silver-tan, damp underside, sand dust, long grain streaks, footprint-faded checks, knots (PLAN "Loop pass 389"). Next idea: driftwood shape (taper, broken ends, partly buried; watch web-smoke grounding).
- Pass 388: crab palm is a 3-ring ellipsoid; shore-crab palm flattened/elongated (budgets 2112/3120) (PLAN "Loop pass 388").
- Pass 387: crab palms 12-sided and smooth-lit, octahedral joints lit per corner (SmoothLit); budgets 1968/2760 (PLAN "Loop pass 387"). Next idea: shore-crab claw palms are squat discs in shape.
- Pass 386: crab limb segments are 6-sided smooth-lit elliptical tubes (budgets 1872/2520/3000) (PLAN "Loop pass 386").
- Pass 385: hermit crab shell 16-sided, smooth one-sided lighting per vertex with crisp per-whorl bands (budget 2232) (PLAN "Loop pass 385").
- Pass 384: hermit crabs past 18 m are a six-sided shell alone (216 vertices) (PLAN "Loop pass 384").
- Pass 383: fruit bats 30-110 m use coarse ellipsoids/bones without eyes or claws (FruitBatRenderer DetailDistance) (PLAN "Loop pass 383").
- Pass 382: QuadBikeRenderer dynamic parts cached per rider slot (hubs/lean/detail key), suspension only within 30 m (PLAN "Loop pass 382").
- Pass 381: birds 35-255 m use a six-sided coarse near model without eyes/feet (BirdRenderer DetailLodDistance) (PLAN "Loop pass 381").
- Pass 380: IslandTerrain::IsInlandBy (MinimumShoreRadius 138 m) for snakes/goats; quad grip surface memo per 25 cm cell (PLAN "Loop pass 380").
- Pass 379: goat body-frame terrain normal memoized (identical poses), spiders full to 14 m / asleep past 22 m, snake grounding by heights alone (PLAN "Loop pass 379").
- Pass 378: bear mesh built from shared lit grid corners (bit-identical, ~4.4x fewer lit vertices); snakes past 10 m / 30 m drop sides, stations and face detail (PLAN "Loop pass 378").
- Pass 377: spiders drawn within 14 m (max 16, detail within 3 m); contact shadows split into a camera-only static part and a movement-driven dynamic tail (PLAN "Loop pass 377").
- Pass 376: fallen-log bark weathered, furrowed, peeled, mossed and lichen-freckled (PLAN "Loop pass 376").
- Pass 375: roosting fruit bat wrapped in a wing cloak, smooth-lit ellipsoids (near budget 2628) (PLAN "Loop pass 375").
- Pass 374: frame-profile marks inside blocks (boar herd, walker, spiders/snakes/monkeys...); goats draw 5.8 ms after 373; dolphin cost was llvmpipe JIT (PLAN "Loop pass 374").
- Pass 373 (operator request: why 12-40 fps): variable time step for interactive play (the fixed step's catch-up ran 4.8 Updates per frame at 12 fps), frame-profile report (Update sections + render-pass CPU ms) in the runtime log every 5 s and in captures, quad sleep, goat/spider/snake canopy sampling, resident LOD, small-fauna reach by size. Next: read the operator's next runtime log's `frame-profile` lines for the real per-pass split on the Radeon 780M (PLAN "Loop pass 373").
- Web smoke (tools): `grounding` scope fixed - its contact-shadow regex predated the walker's-shadow wording (8c7c88b, 2026-10-20), the full-run prop totals were stale (185 rocks / 11 logs, now 209 / 12), and the `reef` view has shown 0 contact shadows since 2026-10-13.
- Pass 372: goat limb top station kept inside the barrel (no pale cap lines at shoulder/haunch) (PLAN "Loop pass 372").
- Pass 371: natural props take the canopy sky occlusion (transmission packed in instance M14) (PLAN "Loop pass 371").
- Pass 370: stream gravel stones sun-lit domes, packed, shadowed grit (PLAN "Loop pass 370").
- Pass 369: removed the opaque green contact disk under every rock (a green ring on soil/sand); rock moss in patches (PLAN "Loop pass 369").
- Pass 368: grass blade midrib fold (GrassFoldStrength); full sweep after 354-367: 116/117 (PLAN "Loop pass 368").
- Pass 367: bear and resident contact shadows premultiplied (were pale discs on dark soil) (PLAN "Loop pass 367"). Chrome `campfire` passed for 366.
- Pass 366: campfire ring stones smooth and rounded, fire facing baked per vertex; campfire now 2712 solid triangles (PLAN "Loop pass 366"). Android debug build passed after 365.
- Pass 365: deer shanks keep the coat (no near-black posts) (PLAN "Loop pass 365").
- Pass 364: shoreline foam lace irregular (rim weight, beads, coarse net, filled patches) (PLAN "Loop pass 364").
- Pass 363: shipwreck timbers sun-bleached above the tide, algae band at it (PLAN "Loop pass 363"). Chrome `rodents` passed for 361.
- Pass 362: shared AppendShadedLoft lights the gecko/skink/frog and beetle/cicada lofts too (PLAN "Loop pass 362").
- Pass 361: rodent bodies per-corner lit and graded (no flat bands); `mice` joins the close-natural-detail profile (PLAN "Loop pass 361").
- Pass 360: gravel stream bed in the terrain shader, clearer stream water (PLAN "Loop pass 360"). Chrome `fish` passed for 359.
- Pass 359: near-eye value-noise wind ripples in the ocean shader (glitter sparkles, no grating; plane ripple waves in the spectrum were tried and reverted) (PLAN "Loop pass 359").
- Pass 358: ocean reflection uses the azimuth-dependent horizon too (PLAN "Loop pass 358"). Chrome `sky` scope passed for 357.
- Pass 357: sky horizon colour by azimuth at golden hour/dusk (warm toward the sun, blue-grey / rose opposite) (PLAN "Loop pass 357").
- Pass 356: rain streaks ~1 px wide (near ones fade), rain curtains fade out of the cloud deck (PLAN "Loop pass 356").
- Pass 355: monkey shader hemisphere ambient (form in shade) (PLAN "Loop pass 355").
- Pass 354: `monkeys` QA camera picks a clear spot; full sweep after 342-353: 116/117 (canopy needs its flag) (PLAN "Loop pass 354").
- Pass 353: flying birds' wings profiled (thick leading edge, camber, gull bend) instead of flat slabs (PLAN "Loop pass 353").
- Pass 352: tide pools worn into limestone benches in the terrain shader (PLAN "Loop pass 352"). Timing note: gpu-pass averages include frame 0's Mesa JIT compile of new shader text; compare frames 1-3.
- Pass 351: crocodiles float low / rest on the bed in the shallows, below-surface vertices water-tinted, banded tail (PLAN "Loop pass 351"). Chrome `campfire` scope passed for pass 350.
- Pass 350: campfire plume as one continuous widening column (tent-weighted strips, height-only sway/drift, paler); `east` camera moved out of the fire pit (PLAN "Loop pass 350").
- Pass 349: fallen palms laid flat on the drawn ground with collapsed, dried crowns (per-palm meshes) (PLAN "Loop pass 349"). Chrome `coast` scope: still past 900 s under load ~15 - uniform 2x slowdown across views (shipwreck too), so load not a regression; rerun with a longer limit when quiet.
- Pass 348: palm middle LOD (half the leaflet straps, twice as wide, 36-44 m out); `water` palm pass 20 -> 11-14 ms (steady frames) (PLAN "Loop pass 348").
- Pass 347: beach details seated on the rendered ground (were floating up to 0.23 m); prop grounding audit otherwise clean (PLAN "Loop pass 347").
- Pass 346: authored clearing landmark logs turned along the contour (were floating up to 1.9 m at their low ends) (PLAN "Loop pass 346").
- Pass 345: resident-fishing QA held at the jetty-foot pause facing the sea; helicopter-approach camera inside the clearing (PLAN "Loop pass 345").
- Pass 344: monkey-scratch QA framing: clearest ground subject, camera ring scored by sight-line clearance (`QaSightClearance`) (PLAN "Loop pass 344").
- Pass 343: bunker computer room lamps, cables, hazard stripes, exit sign; `bunker-computers` now passes (72 colour bins), the sweep's only known exception is `canopy` (PLAN "Loop pass 343").
- Pass 342: heron alarm croak on flush (GenerateHeronCroak, NoteHeronCroak, WildlifeCallType::Heron); untested by ear (PLAN "Loop pass 342").
- Pass 341: herons roost at night in shore palm crowns / coastal perches, back to wading at dawn (PLAN "Loop pass 341").
- Pass 340: herons flush from the resident, the quad, the bear, boars and moving crocodiles too (PLAN "Loop pass 340").
- Pass 339: crocodile body/skull/tail lit per corner (smooth), pigment per quad (PLAN "Loop pass 339").
- Pass 338: trodden-sand hollows lit as relief over the whole dry beach (PLAN "Loop pass 338").
- Pass 337: heron strikes splash (shared small ring + plip), wading legs tinted below the waterline (PLAN "Loop pass 337").
- Pass 336: four reef herons (dark and white morphs) wade the lagoon edge: stalk, freeze, strike, flush, rest at night; `herons` viewpoint (PLAN "Loop pass 336").
- Pass 335: turtle carapace lit per corner (smooth), mottle per corner; no checkerboard (PLAN "Loop pass 335").
- Pass 334: Snell's window: ocean surface from below mirrors the water outside 48°, sky takes the water colour outside the window; fog 0.06/m; `underwater-scene` validation profile for fish/reef-habitat/dolphins (PLAN "Loop pass 334").
- Pass 333: underwater distance fog (UnderwaterFog.hpp): terrain/seagrass/props shaders + sea-fauna BasicEffects, ramped over the first 0.8 m of depth (PLAN "Loop pass 333").
- Pass 332: wrack line thin, clumped, brown strands instead of near-black wedges (PLAN "Loop pass 332").
- Pass 331: coastal sand palette dry by 0.6 m (the shader owns the wet band) (PLAN "Loop pass 331").
- Pass 330: campfire light falls off nearly inverse-square; ring stones/ash/firewood lit by facing (PLAN "Loop pass 330"). Note: `resident --time-of-day 21.5` fails its walking check by design (resident sits at the fire at night).
- Pass 329: the crocodile tail is laterally compressed into a paddle (PLAN "Loop pass 329").
- Pass 328: sky-dome cumulus fringe is the core lightened, no shade band; no more ink outline (PLAN "Loop pass 328").
- Pass 327: birds get smooth-shaded seven-ring bodies, four-ring heads and split primaries (PLAN "Loop pass 327").
- Pass 326: coconut fronds of 34 long hanging leaflets a side, widened to about a pixel at distance in the shader (PLAN "Loop pass 326").
- Pass 325: steep vegetated banks take the turf hue instead of the bare green vertex palette (closes pass 323's open item; PLAN "Loop pass 325").
- Pass 324: deer coats carry the shared goat strand texture (mirrored, per-part UVs); deer now 2 draws (PLAN "Loop pass 324").
- Pass 323: cave mouth terrain: bare steep shoulders get soil instead of the raw green palette, ragged rock apron edges (PLAN "Loop pass 323"). Open: steep non-rock faces elsewhere (summit, stream banks) still show the vertex palette.
- Pass 322: the cascade spray mist thinned (density, growth, near-eye fade); the haze over the stream bank is gone (PLAN "Loop pass 322").
- Pass 321: lobed tide-pool basins; the pool water fades by its depth to the true waterline (PLAN "Loop pass 321").
- Pass 320: the ocean colour carriers get their own footprint filter and a 30-90 m reach; the far-sea checkerboard is gone (a first, looser filter only weakened it; PLAN "Loop pass 320").
- Pass 319: seagrass grown per 8 m bed as static meshes with a sway shader, planted into a carpet; the terrain darkens the meadow bed on the same curve (PLAN "Loop pass 319").
- Pass 318: touch tap buttons for riding the quad and the hand torch (PLAN "Loop pass 318").
- Pass 317: the Web/Android stand-in quad rider leans into the corners like the CNA avatar (PLAN "Loop pass 317").
- Pass 316: the quad's CNA rider leans into the corners, hands kept on the grips (PLAN "Loop pass 316").
- Pass 315: H switches a hand torch on foot: lights the night ground, the rain in its beam and the animals' eyeshine (PLAN "Loop pass 315").
- Pass 314: at night the monkeys climb a tree and sleep up it until morning (PLAN "Loop pass 314"; closes the deferred monkeys-in-trees item).
- Pass 313: the quad throws spray through the rain puddles (CPU mirror of the shader's puddle mask, `TerrainShaderNoise.hpp`); the headlight-beam haze cones were tried and dropped (PLAN "Loop pass 313").
- Pass 312: the quad's brake light flares red when braking, by day and night (PLAN "Loop pass 312").
- Pass 311: the crocodiles' eyes glow red in the quad's beam (PLAN "Loop pass 311").
- Pass 310: animals' eyes shine back in the quad's headlights at night (PLAN "Loop pass 310").
- Pass 309: the swash foam rides a dark wet sheet of water instead of lying on dry sand (the pale strip beyond it in `jetty` is clear shallow water over the lagoon sand; darkening it was tried and dropped) (PLAN "Loop pass 309").
- Pass 308: the spider webs bead with the morning dew and after rain (PLAN "Loop pass 308").
- Pass 307: a before/after audit of 106 views; the ground-dove close-up no longer has the resident's leg filling a third of the frame (PLAN "Loop pass 307").
- Pass 306: the sunbeam motes no longer make a false star against the daytime sky (only drawn below the eye, against the forest) (PLAN "Loop pass 306").
- Pass 305: the cave's rock no longer streaks toward the mouth (its triplanar weights now use the normal in the cave's own frame) (PLAN "Loop pass 305").
- Pass 304: on dark nights a swimmer stirs plankton into blue light (PLAN "Loop pass 304").
- Pass 303: seagrass clumps sway on the lagoon floor's meadow beds (`SeagrassRenderer`; tufts, not yet a full carpet) (PLAN "Loop pass 303").
- Pass 302: the stream's margins are wet mud and its shallows nearly clear, not a green film over the bank (PLAN "Loop pass 302").
- Pass 301: the deer and boar prints are rounded ovals, not little square tiles (PLAN "Loop pass 301").
- Pass 300: the sea turtle's carapace is a low flared dome, and its nest and tracks lie along the beach's slope instead of being cut off by it (PLAN "Loop pass 300").
- Pass 299: in a storm a waterspout now and then stands offshore for a few minutes (PLAN "Loop pass 299").
- Pass 298: satellites glide across the twilight sky after dusk and before dawn (PLAN "Loop pass 298").
- Pass 297: the morning dew wets the forest's leaves too (shared `weather_now::Dew`) (PLAN "Loop pass 297").
- Pass 296: the grass darkens and takes a sky sheen when wet, after rain and with the morning dew (PLAN "Loop pass 296"; `grass` in showers fails the luminance floor with or without it).
- Pass 295: the forest drips from its crowns through and long after the rain (`CanopyDripRenderer`) (PLAN "Loop pass 295").
- Pass 294: a cloud cap forms on the summit in moist air (`SummitCloudField`) (PLAN "Loop pass 294").
- Capture sweep 2026-10-21 (after passes 326-341): 115 of 117 views pass (the new `herons` included); `bunker-computers` and `canopy` as before.
- Capture sweep 2026-10-21 (after passes 294-325, three of them terrain colour): 114 of 116 views pass; `bunker-computers` fails its colour-bin floor as before, `canopy` needs `--canopy-lod-inspection`.
- Capture sweep 2026-10-21 (after passes 268-293): 114 of 116 views pass; `bunker-computers` fails its colour-bin floor as before (a grey interior), `canopy` needs `--canopy-lod-inspection X_MM Z_MM near|far`.
- Pass 293: at night the rain glitters in the quad's beams, the lantern's and the fire's light (`RainRenderer::DrawLit`) (PLAN "Loop pass 293").
- Pass 292: the quad gathers mud and dust as it is ridden, washed off in the sea and rinsed by rain (PLAN "Loop pass 292").
- Pass 291: a yacht sails past well offshore every nine minutes, its navigation lights burning at night (PLAN "Loop pass 291").
- Pass 290: most mice and rats stay hidden by day and come out at night (PLAN "Loop pass 290").
- Pass 289: the day geckos and skinks go in at dusk; the frogs hide by day and come out at night or in the rain (PLAN "Loop pass 289").
- Pass 288: the puddles, the stream and the wet sand mirror the hour's sky (PLAN "Loop pass 288").
- Pass 287: the sea mirrors the hour's sky (pink at dusk, deep blue at night) via the shared `sun_light::SkyScalesOf` (PLAN "Loop pass 287").
- Pass 286: the seabed caustics fade as the sun sinks (gone at sunset) (PLAN "Loop pass 286").
- Pass 285: the resident tends the campfire from its edge (a route spur to 1.9 m) and is lit warm by it at night (`LocalLightRenderer::DrawMesh`) (PLAN "Loop pass 285").
- Pass 284: the lantern lights the hut's front wall, gable and veranda floor at night (`LocalLightRenderer::DrawPlane`) (PLAN "Loop pass 284").
- Pass 283: two house geckos come out on the hut wall by the lantern at night and hunt its moths (PLAN "Loop pass 283").
- Pass 282: the Milky Way arches over the island on dark moonless nights (PLAN "Loop pass 282").
- Pass 281: the shore's breaking foam is broken into irregular runs instead of a dashed line seen from the air (PLAN "Loop pass 281").
- Pass 280: a storm's wind tears leaves loose, carries them far downwind and shakes the coconuts down (PLAN "Loop pass 280").
- Pass 279: heavier rain splashes on the ground round the walker, small crowns of droplets (PLAN "Loop pass 279").
- Pass 278: in a storm's wind the forest crowns lean and toss and the grass lies over downwind (PLAN "Loop pass 278"; `grass` in a thunderstorm fails the luminance floor with or without it).
- Pass 277: in a storm's wind the palm fronds blow downwind and thrash (PLAN "Loop pass 277").
- Pass 276: the quad bike wet in the rain: darker tyres and plastic, a water film mirroring the sky (PLAN "Loop pass 276").
- Pass 275: the cloud deck closes under overcast and in a storm: no blue-teal holes or bluish cumulus over a storm (PLAN "Loop pass 275").
- Pass 274: the hut's thatch and boards and the palms' bark and fronds darken as they soak in the rain (PLAN "Loop pass 274").
- Pass 273: no rain falls under the hut's roof (`RainRenderer::SetShelter`) (PLAN "Loop pass 273").
- Pass 272: the hut's eaves drip in the rain, drops falling from the thatch tips to splash in the drip line (PLAN "Loop pass 272").
- Pass 271: the day fliers shelter from the rain, the birds and monkeys sit out a downpour, the fruit bats stay in during a storm (PLAN "Loop pass 271").
- Pass 270: a downpour beats the campfire down: low sputtering flames, no sparks, steam, a dimmer firelight and a quieter crackle (PLAN "Loop pass 270").
- Pass 269: the resident's clothes and hair darken as they soak in the rain (PLAN "Loop pass 269").
- Pass 268: dust and pollen glint in the forest's sunbeams round the walker (PLAN "Loop pass 268").
- Pass 267: whitecaps break the open sea in a strong wind (PLAN "Loop pass 267").
- Capture sweep 2026-10-21 (after passes 225-266): 113 of 116 views pass. `rats` sat at 63-65 colour bins against the scene floor of 64 and was reviewed into the close-natural-detail profile like `ants` and `beetles`; `bunker-computers` fails the colour-bin floor as it already did on 2026-10-14 (a grey interior: a correct render, left as a known failure); `canopy` needs `--canopy-lod-inspection`.
- Pass 266: a bright moon casts soft shadows (PLAN "Loop pass 266").
- Pass 265: a storm's surf breaks harder along the shore: fuller crests and a whiter, wider wash (PLAN "Loop pass 265").
- Pass 264: humpback whales pass far offshore: a blow, a rolling back and now and then raised flukes (PLAN "Loop pass 264").
- Pass 263: the hermit crabs sit out the day and wander at night (PLAN "Loop pass 263").
- Pass 262: the resident waits out a downpour where they have stopped, and the night chorus swells on soaked ground after rain (PLAN "Loop pass 262").
- Pass 261: the beach crabs go home down their burrows at night (or dig in where they stand) and come up in the morning (PLAN "Loop pass 261").
- Pass 260: raindrops ring the puddles while it rains (PLAN "Loop pass 260").
- Pass 259: distant rain curtains: grey shafts of rain hang from far showers over the sea round the horizon in showery and overcast weather (PLAN "Loop pass 259").
- Pass 258: moths flutter round the hut's lantern at night (PLAN "Loop pass 258").
- Pass 257: butterflies, dragonflies and flies roost one by one at dusk and come out again at dawn (PLAN "Loop pass 257").
- Pass 256: the stars behind the passing clouds are hidden too (PLAN "Loop pass 256").
- Pass 255: the dawn chorus: at first light the birds sing every few seconds until mid-morning (PLAN "Loop pass 255"; not yet heard by a person).
- Pass 254: the moon hides behind passing clouds (and an overcast), and two tide pools no longer glow cyan at night (a pass-243 fault) (PLAN "Loop pass 254").
- Pass 253: the coconut crabs keep to the night: long rests by day, longer forage rounds after dark (PLAN "Loop pass 253").
- Pass 252: the surf pounds louder as a storm's wind raises the sea (PLAN "Loop pass 252"; not yet heard by a person).
- Pass 251: no sunlight after sunset: the seabed caustics need the direct sun (gone at night and under overcast), and the sea's glint is the sun's while it is out and a faint cool moon glint at night (PLAN "Loop pass 251").
- Pass 250: a visible lightning bolt with its flash, at the strike's bearing and distance; and the puddles' sun glint is held to the direct sun (a stormy flight view showed them white) (PLAN "Loop pass 250").
- Pass 249: rain-wet trees: bark darkens and leaves deepen when the weather has wet them, with a glossy sun sheen where the sun gets through (PLAN "Loop pass 249").
- Pass 248: a scops owl calls from the trees round the listener every half minute or so through the night (PLAN "Loop pass 248"; not yet heard by a person).
- Pass 247: on soaked ground the walker's steps on bare soil squelch and splash like the shallows' edge (PLAN "Loop pass 247"; not yet heard by a person).
- Pass 246: a rainbow when the sun breaks through a shower easing off: the primary bow at 42 degrees opposite the sun and a fainter reversed secondary at 51 (PLAN "Loop pass 246").
- Pass 245: the sea runs higher in a storm (up to 1.7 times the trade wind's waves) and the breeze in the trees swells with the wind (PLAN "Loop pass 245").
- Pass 244: turtle hatchlings break out of their nests after dark, as they do, a clutch due by day waiting for the night (PLAN "Loop pass 244").
- Pass 243: bioluminescence: on dark nights the breaking surf glitters blue in specks where the plankton are stirred (PLAN "Loop pass 243").
- Pass 242: shooting stars: about one a minute a brief bright streak slides down the starry sky (PLAN "Loop pass 242").
- Pass 241: after dark the bear lies up until morning too (PLAN "Loop pass 241").
- Pass 240: after dark the deer bed down, the goats lie down and the boars rest until morning (PLAN "Loop pass 240").
- Pass 239: after dark the monkey troop settles to rest instead of wandering until morning (PLAN "Loop pass 239").
- Pass 238: the resident keeps the fire through the night: arriving at the pit after dark they stay crouched tending it until dawn, then walk on (PLAN "Loop pass 238").
- Pass 237: the wildlife keeps the clock: at dusk the birds go to roost a perch apiece until dawn and the fruit bats leave their roost to forage on wider rounds (PLAN "Loop pass 237").
- Pass 236: the soft pinkish smears on the hillsides end at crisp outcrop lips, and leaf humus and moss gather under the crowns rather than staining the open turf (PLAN "Loop pass 236").
- Pass 235: morning mist lies in banks over the stream's valley, the wetland and the coastal flats at first light and burns off by mid-morning (PLAN "Loop pass 235").
- Pass 234: the gust fronts follow the weather's wind: in a storm they sweep the island about every eight seconds, faster and fuller, on a live clock that never jumps (PLAN "Loop pass 234").
- Pass 233: puddles after rain: flat hollows of the paths, clearings and forest floor hold water that mirrors the sky and catches the sun, spreading in a soaking and drying back (PLAN "Loop pass 233").
- Pass 232: the island sounds like night after dark: a chorus of crickets and tree frogs rises while the birds, monkeys and cicadas fall quiet and the jungle's day bed softens (PLAN "Loop pass 232"; not yet heard by a person).
- Pass 231: fireflies blink low over the stream's banks and round the wetland on dry nights, part of each cluster flashing in step (PLAN "Loop pass 231").
- Pass 230: the open hillsides read as grass, not paint: a world-anchored sward texture (two-metre clumps, half-metre tussocks, dry and fresh patches) from where the grass blades thin out, and sparse shrubs on open inland slopes (PLAN "Loop pass 230").
- Pass 229: the wildlife hears the quad: a rider on it alarms the deer, goats and boars from up to two and a half times a walker's distance, and perched birds flush as it races past (PLAN "Loop pass 229").
- Pass 228: the hut is lit at night: its front windows glow warm through their mullions and a hurricane lantern on the veranda lights the steps (PLAN "Loop pass 228").
- Pass 227: the straight seam across the sea from the air is gone: the water closes over the seabed where the eye's ray meets it near the terrain mesh's edge (PLAN "Loop pass 227").
- Pass 226: rain rings on the sea, world-anchored, only over water deeper than 25 cm, and a `jetty-end` QA viewpoint (PLAN "Loop pass 226").
- Pass 225: the campfire lights the night ground through the grade, warm orange instead of capped at the night's dark yellow-green; the quad's beam moved into the shared `LocalLightRenderer` (PLAN "Loop pass 225").
- Pass 224: a `flight` QA viewpoint 40 m up; the sea's diamond lattice seen from the air (the ripple cells fading only from 28-85 m up) now fades by 10-32 m, and the seabed caustics are an irregular three-family net instead of an axis-aligned grid (PLAN "Loop pass 224").

The earlier agent-actionable backlog (above this section's predecessor) is empty. The open
mid-distance hillside that read as a smooth painted green surface got a sward texture and sparse
hillside shrubs (Loop pass 230); in the native-performance profile vegetation draws to 55 %
(bushes to 56 m), so a hill 60-100 m off still shows no shrubs there. Its soft pinkish
blotches were weathered-rock outcrops whose detail filters out at distance; they now end at
crisp lips (Loop pass 236).

Not agent-actionable (stay open in `PLAN.md`; need the operator or physical hardware): human
speaker/headphone listening (goat, boar, fly, snake hiss, audio-focus lifecycle), the physical-GPU
59--60 FPS comparison and AMD hardware qualification (the AMD latch forbids automated host-GPU
runs), and Android runtime on an emulator or physical device (emulator/KVM setup is off-limits).

## Current checkpoint — high campfire smoke and live animal-mix audit

The narrow goal for this checkpoint is complete in source: finish the campfire smoke, verify goat
bleats and boar grunts in the actual final game mix rather than only inspecting generated PCM,
refresh this handoff, then commit/push and stop. Do not resume unrelated backlog work in the same
turn after the push. The broader `PLAN.md` backlog remains active for a future agent; it is not
finished and must not be represented as complete.

The inland campfire now has twelve additional camera-facing upper-smoke quads arranged as six
overlapping altitude bands. They widen from 1.65m to 4.55m, reach about 15m, cool from warm low
smoke to neutral grey, curl slowly and bend west in world space with the established easterly trade
wind. This reuses `CampfireRenderer`'s existing moving vertex buffer, shader, alpha blend state and
single moving draw: the whole flame/smoke/spark pass changes from 42 to only 66 triangles, with no
new texture, allocation, emitter, draw call or subsystem. The expanded finite bounds keep the high
plume culled when the clearing is out of view. Keep the plume translucent; do not respond to future
visibility feedback by turning it into an opaque dark column.

Current inspected native evidence is
`build-consumer/software-captures/20261017T160648.389643Z-campfire` (close) and
`20261017T160612.920244Z-campfire-wide` (landmark distance). Both pass the strict 1280x720 capture
gate and show a continuous readable plume above the flame. Current inspected WebGL evidence is
`build/browser-software/20261017T160912.831387Z-chrome-campfire`; Chrome 155/SwiftShader passes 60
positive intervals, two distinct frames, GL error zero, a live context and empty kernel delta. The
Web package is 14,457,108 bytes uncompressed / 5,819,665 gzip. Android ARM64 assemble and lint pass
without an emulator; `android/app/build/outputs/apk/debug/app-debug.apk` is 24,837,763 bytes,
SHA-256 `34a176ff1c19c8aed40e20f2250a496ee598485fd7e3b99a67df73b1535143a0`, with a 19,795,192-byte
`libmain.so`. A matched physical-GPU 59--60 FPS comparison remains for `[x]`; renderer cost is
bounded to 24 extra triangles in an existing draw, but that fact is not a physical benchmark.

The current goat and boar fixes were then exercised through the real `Soundscape` mix and SDL disk
audio backend. Goat evidence
`build-consumer/software-goat-audio/20261017T161258.499491Z` passes three actual calls, moving
positional playback, five of six maximum sources, exact-zero mute and 204,800 nonzero bytes after
resume. Direct 250ms-window analysis of the captured 44.1-kHz stereo `game-audio.raw` finds the two
main bleats about 8--10dB above the ambient bed, with 88--93% energy in 80--500Hz. This is final-mix
audibility evidence, not merely standalone clip generation or a `Playing` flag. The revised
151/166Hz-to-118/132Hz falling low-harmonic synthesis remains recognisably separated from the
high narrow-band fly buzz that the operator previously heard.

Boar evidence `build-consumer/software-boar-audio/20261017T161709.073086Z` passes three calls,
positional playback, five of six maximum sources, exact-zero mute and 221,184 nonzero resume bytes.
The strongest 250ms grunt window is 6.7dB above the non-silent final-mix median and has a clear
45--420Hz energy rise. The first attempted current run was rejected only because
`validate_boar_audio_trace` still treated wildlife enum values above Boar as invalid; frogs can now
legitimately occupy type 7. `run_fullscreen_software.py` now accepts the complete current
`WildlifeCallType` range 0--9 for boar, snake and goat traces while retaining the source-count,
mute, monotonic-counter and expected-species checks. The corrected run passes with no new kernel
GPU fault. Human speaker/headphone identity/direction review and Web/Android runtime listening
remain open in `PLAN.md`; do not claim those physical/perceptual checks from disk evidence.

Validation/build constraints for the next agent:

- Never use the host AMD GPU for automated work and do not touch Android emulator/KVM setup.
- Compile with at most three CPU jobs (`--parallel 3` / `-j 3`); the Android wrapper already caps
  Gradle/native work to two workers.
- Use the one shared ccache: `CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv` (writable again as of
  2026-10-19); never create a private cache under `/tmp`.
- Android uses the pinned local runtime at
  `build/dependencies/sharp-runtime-pinned` (`-PsharpRuntimeRoot=...`) and needs no network.
- Two report-only CTests, `IslandGameStartupReportValidation` and
  `IslandGameCaptureValidation`, depend on deliberately disabled producer tests; exclude them when
  running the active standalone suite. They are not game regressions.
- The final current native run passes all 72 enabled tests (108 registered, 36 display/benchmark
  producers deliberately disabled) in 45.58 seconds with `ctest --test-dir build-consumer
  --output-on-failure -j 3 -E '^(IslandGameStartupReportValidation|IslandGameCaptureValidation)$'`.
- The immediate next broad-backlog work after this checkpoint should be chosen from current
  `PLAN.md`, not from stale older sections below. High-value open reviews include physical audio
  listening, shipwreck integration/polish, and remaining portable/ordinary-player audits.

## Current work — cave ceiling breakup inspected

Goal remains active; continue the whole plan. Six small ceiling pendants now break the smooth cave
arch. A first single-ring version passed but was rejected as cone-like; final forms use irregular
seven-sided roots, narrower rotated middle rings and offset tips. Every tip remains above 2.3m, so
the existing playable corridor/collision/layout are unchanged. The cave rises 1,384→1,510 rendered
triangles in the same draw. Strict native `20261013T212901.927564Z-cave` PASS273bins/std48.99/
spatial226.19 and `20261013T212749.826507Z-cave-inside` PASS65/std24.57/spatial116.01, both
inspected/kernel[]. Chrome/SwiftShader `20261013T211636.896127Z-chrome-cave` PASS both views/
60 intervals each/four inspected frames/GL0/contexts intact/kernel[]. Release+Debug cave/asset,
33 browser-runner tests and ARM64 assemble/lint pass. Free-walk moving review, broader exterior
geology and Android runtime remain; PLAN stays `[~]`. No emulator/KVM or AMD GPU. Max four cores.

## Current work — broken stern deckhouse inspected

Goal remains active; continue the whole plan. The shipwreck stern no longer carries one roof slab
over solid box walls. Ten separated uneven longitudinal roof-board lanes include one real fracture;
lower/upper wall bands and four uprights form three side window bays, with a true open middle bay
and recessed surviving panes. Cost rises 5,004→5,532 triangles in the same one draw/buffer with
unchanged texture, collision, layout and 12 debris pieces. Final strict native
`20261013T204318.064303Z-shipwreck` PASS193bins/std38.55/spatial161.29/kernel[] and inspected.
Chrome/SwiftShader `20261013T204536.386851Z-chrome-shipwreck` PASS60 intervals/two inspected
frames/GL0/context intact/kernel[]. Release+Debug core/asset,33 browser-runner tests and ARM64
assemble/lint pass. End grain, debris sand/water integration, wide comparison and Android runtime
remain; PLAN stays `[~]`. No emulator/KVM or AMD GPU. Max four compile cores.

## Current work — helicopter cockpit controls inspected

Goal remains active; continue the whole plan. Each pilot seat now has a curved tubular cyclic,
left-mounted collective and two rudder pedals. Four recessed round gauges have metal bezels,
individual needles and six lower annunciators. The interior QA camera now looks down the centre
aisle so controls are visible. The model rises 1,788→2,104 triangles, still two draws and with no
texture/shader/collision/flight-state change. First rear-cabin capture201141.581953Z passed but
was rejected because seat backs hid the controls. Final native201600.553835Z-helicopter
PASS301bins/std55.12/spatial235.22 and201518.621138Z-helicopter-inside PASS167/std25.15/
spatial110.19, both inspected/kernel[]. Chrome/SwiftShader201903.851542Z-chrome-helicopter PASS
both views/60 intervals each/four inspected frames/GL0/contexts intact/kernel[]. Release+Debug
layout/asset,33 browser-runner tests and ARM64 assemble/lint pass. Free-walk clearance, Android
runtime, sitting/interaction and richer mechanics remain; PLAN stays `[~]`. No emulator/KVM or
AMD GPU. Max four compile cores.

## Current work — monkey nose and corneas inspected

Goal remains active; continue the whole plan. The latest near face now moves/enlarges each cornea
and adds a fitted 76mm-wide shallow dark nose pad on the existing head bone. A negative-U feature
tag reuses the face shader/draw; cost is +78 vertices/+120 triangles per near face with no new
texture, bone, material, draw, population or simulation change. Native
`20261013T194439.335771Z-monkeys` passed technically but was rejected as facial evidence because
the view was rear/occluded. The complete llvmpipe
`20261013T194613.603137Z-monkey-tree` PASS all nine frames, 204--211 bins/std44.21--44.94/
spatial192.17--196.26/kernel[]; montage inspection shows the nose, cornea and mouth in a clear
side pose while the hooked furry tail and tree contacts remain attached. Chrome/SwiftShader
`20261013T195104.174891Z-chrome-monkeys` PASS60 intervals/six inspected frames/GL0/context intact/
kernel[]. Release+Debug monkey tests3/3, both asset gates,33 browser-runner tests and Android
ARM64 assemble/lint pass. Procedural joint/gait/facial micro-anatomy and Android runtime remain;
PLAN stays unchecked. No emulator/KVM or AMD GPU. Max four compile cores.

## Current work — helicopter closed main-rotor blades inspected

Goal remains active; continue the whole plan. The four stationary 5.65m main-rotor blades no longer
collapse into wire-like one-quad sheets: each is a closed tapered 68mm-deep volume with separate
upper/lower and leading/trailing/root/tip faces. Cost is +40 triangles, 1,788 total, still two
opaque/glass draws and no collision/traversal/texture/simulation change. Inspected native
`20261013T191917.508759Z-helicopter` PASS 304 bins/std55.11/spatial234.93/kernel[]. Focused
Chrome/SwiftShader `20261013T192258.296430Z-chrome-helicopter` PASS exterior+interior, 60 finite
intervals each, GL0/contexts intact/four distinct images/kernel[]. Release+Debug layout/asset,
33 browser-runner tests and ARM64 assemble/lint pass. No emulator/KVM or AMD GPU. Richer mechanics,
lighting, sitting/free-walk and Android runtime remain; PLAN stays `[~]`. Max four compile cores.

## Current work — shipwreck missing-plank thickness inspected

Goal remains active; continue the whole plan. All four genuine beach-facing missing-plank bays now
have 11cm inward geometry around all four cut boundaries: weathered transverse fracture faces and
darker upper/lower inner wood. Sixteen quads add 32 triangles inside the existing buffer and draw;
the wreck is now 5,004 triangles with unchanged collision, layout and 12 debris pieces. Inspected
native `20261013T190835.072388Z-shipwreck` PASS 214 bins/std38.30/spatial160.11/kernel[]. Focused
Chrome/SwiftShader `20261013T191119.148542Z-chrome-shipwreck` PASS 60 intervals/two distinct
inspected frames/GL0/context intact/kernel[]. Release+Debug asset validation, 32 browser-runner
tests and ARM64 assemble/lint pass. No emulator/KVM or AMD GPU. Transom/cabin and beach/water
integration remain; keep PLAN `[~]`. Max four compile cores.

## Current work — shipwreck raked bow/stern pass inspected

Goal remains active; continue the whole plan and do not stop early. The wreck's lower bow/stern no
longer end as vertical box walls: a bounded 0.90/0.65m rake below the deck edge is shared by hull,
deck ends, seams, caps and the sternpost. The exact 20m top endpoints, collision, materials, debris,
4,972 triangles and one draw remain unchanged. An initial 0.46/0.34m attempt was inspected and
rejected as too subtle. Final native `20261013T183155.768012Z-shipwreck` PASS 194 bins/std38.50/
spatial161.09/kernel[]. Broad Web `20261013T183440.535268Z-chrome-coast` passed shipwreck/water/west
but timed out in reef, so is not a suite pass. New focused Web
`20261013T185915.467977Z-chrome-shipwreck` PASS 60 finite intervals/two distinct inspected frames/
GL0/context intact/kernel[]. Release+Debug core/asset tests, 32 browser-runner tests and ARM64
assemble/lint pass. No emulator/KVM or AMD GPU. Further transom/cabin and beach/water
integration remain; keep PLAN `[~]`. Max four compile cores.

## Current work — dolphin silhouette and countershading pass inspected

Goal remains active; continue the whole plan. Fresh current baseline
`20261013T175140.727008Z-dolphins` exposed a flat dorsal slab, bulky trunk, abrupt melon/beak and
oversized wedge fins. The close dolphin now uses13 longitudinal stations ×16 sides, a raised
forehead over the short rostrum, narrower girth, compact five-point closed dorsal/pectorals and
symmetric lower-contrast grey-blue countershading. The old fish-like lateral tail bend is corrected
to a vertical tail-stock/up-down horizontal-fluke stroke in both LODs. Other motion, LOD,
population and one draw stay unchanged. Budget rises708→1476 near vertices (492 triangles), at most94,464 bytes for all four;
far remains15 vertices. No texture/shader/world/simulation change.

Final inspected native `20261013T181811.002065Z-dolphins` PASS235bins/std28.33/
spatial146.81/kernel[]. Final Chrome/SwiftShader
`20261013T182032.131304Z-chrome-dolphins` PASS60intervals/two distinct inspected poses/GL0/
context intact/kernel[]. ARM64 assemble/lint and Release+Debug pod tests pass; no emulator/KVM or
AMD GPU. Android runtime, skin microtexture and physical cost remain. Max4 build cores. Next
continue highest-value plan work; do not stop or complete the goal.

## Current work — monkey trunk and grasping-extremity pass inspected

Goal remains active. Image review of the latest monkey still showed a barrel torso and
thin parallel finger/toe rods despite the already-live fur, quadrupedal gait and tree
excursions. The13-section trunk now has separate fuller rump and shoulder/chest masses
with a narrower waist; cranium depth/width is slightly reduced. Near palms/soles are
wider, their four forward digits shorter/thicker, and a lateral opposable digit gives a
readable grasp. All vertices retain the existing sole-frame transform,14 bones, texture,
three near draws and population. New diagnostic measures2017/797 vertices and3100/1208
triangles near/far.

Gait/shader/sole tests3/3 PASS. Actual `20261012T221727.352629Z-monkey-tree` nine-stage
sequence and final current-binary `20261012T221906.711225Z-monkeys` PASS; images inspected,
kernel deltas empty. The result is more recognisably primate, but face scale, manifold
shoulder/hip transitions, continuous gait naturalness and fresh Web/Android review remain;
PLAN stays unchecked. Corrected stale overhaul documentation that still called climbing
unimplemented. Max4 build cores; no AMD GPU, Android emulator, KVM or dependency changes.

## Current work — live goat calls and native disk playback verified

Goal remains active. Six original 1.2-second procedural clips now provide paired goat
breath, bleat and hoof detail. `GoatCallScheduler` reads live Rest/Graze/Walk/Retreat
state, starts eligible animals with a bleat, uses close quiet detail only when audible,
rotates fairly with per-animal and 2.8--5.0s herd cooldowns, and varies pitch/gain without
adding a voice. Soundscape priority is bear/snake/goat/boar after bird/monkey arbitration;
the active emitter follows the goat and mute/distance/removal stop it. F1/input diagnostics
expose GOAT count and wildlife type6. The 38-clip report/export passes at5,893,964 PCM
bytes; goat peaks are .48/.60/.54 for breath/bleat/hoof, exact endpoints and distinct
fingerprints. Core and real-CNA dummy runtime pass.

New supervised SDL-disk gate `build-consumer/software-goat-audio/
20261012T220810.661476Z` PASS: three live goat calls, max5/6 sources,991232 exact-zero
muted bytes,262144 nonzero resumed bytes, llvmpipe and kernel-final[]. Diagnostic image
inspected; it is audio plumbing evidence, not visual acceptance. SoftwareCaptureTests
52/52 pass. `PLAN.md`, README, provenance and audio review updated. Subjective listening,
spatial turn/distance, mixed-herd overlap and Web/Android lifecycle remain open. Max4
build cores; no AMD GPU, Android emulator, KVM or dependency changes. Continue goal.

## Current work — sea-turtle head and shell-detail pass inspected

Goal remains active. New side-on QA camera first exposed the existing detailed1728-vertex turtle.
The shell/paddle silhouette was retained; head rings/neck/skull/brow/beak are fuller, eye triangles
move to the surface, paired nostrils and mouth creases add4triangles, and cage-like scute seam strips
narrow5.5→3.5mm while lightening70→82% shell tone. Near budget1740vertices/turtle, fixed five-agent
capacity139200bytes, far LOD/population/simulation/one-draw unchanged. Actual contained llvmpipe
`20261012T211746.659012Z-turtles` PASS/kernel[], one near turtle/one draw, image inspected. Natural
interlocking scutes, skin-scale material, further flipper/head art, Web/Android and performance
remain; PLAN aquatic umbrella stays[~]. See docs/sea-turtle-form.md. Next crab visual audit. Max4
build cores; no AMD/Android/KVM changes.

## Current work — offshore shark volumetric form inspected

Goal remains active. Baseline `20261012T210613.753528Z-shark` exposed the old51-vertex diamond
body/symmetric fish tail. The close shark is now375vertices: capped blunt snout, seven eight-sided
stations, forward girth, narrow caudal peduncle, progressive rear bend, longer heterocercal upper
tail lobe, two-facet primary+small secondary dorsal, paired pectoral/pelvic fins, eyes, mouth and
three gill slits per side. Existing patrol/presence/habitat/pitch/bank/activity tint,15-vertex far
LOD and one draw unchanged; buffer6000bytes and exact runtime budget assertion. Final contained
`20261012T211055.205953Z-shark` PASS/kernel[], byte-identical to preceding successful run, image
inspected. A closer side view was also inspected but correctly remains failed evidence because
the underwater-only frame had63/64 required colour bins. Web diagnostic pins updated; actual
Web/Android/performance and teeth/skin/fin polish remain. See docs/shark-form.md. Next continue
turtle/crab visual audit. Max4 build cores; no AMD/Android/KVM changes.

## Current work — shallow-water fish volumetric form inspected

Goal remains active. The prior51-vertex close fish visibly read as intersecting flat wedges in
baseline llvmpipe `20261012T205724.527481Z-fish`. It is now a276-vertex six-station/eight-facet
loft with rounded snout, deep forward body, narrow caudal peduncle, dark back/pale belly,
two-facet dorsal, separately rooted pectorals and surface-fitted eye/gill/mouth cues. Rear four
stations progressively follow existing bodyWave; distant12-vertex LOD,48 population, routes,
one-draw batch and simulation unchanged. Fixed QA camera is now side-on. Final contained
`20261012T210406.805334Z-fish` PASS/kernel[], six near fish/one draw, image inspected. Maximum
near buffer13,248vertices (~212KiB), runtime exact-budget assertion. Web diagnostic pins updated;
actual Web/Android/performance and texture/fin polish remain. See docs/fish-form.md. Next continue
shark/turtle/crab visual audit. Max4 build cores; no AMD/Android/KVM changes.

## Current work — offshore dolphin first pass inspected

Goal remains active. Four peaceful dolphins are now live beyond the western reef. `DolphinPod`
uses deterministic 17--21m home ranges in the 34--105m offshore band and cycles cruise/surface/
dive with smoothed pitch, bank and tail beat; full/reduced/dormant distances are190/520m. The
original procedural renderer has fusiform body, pale underside, rostrum, eyes, dorsal/pectoral
fins and horizontal flukes, three depth-lit palettes and one dynamic draw; geometry is498/30
near/far vertices per visible animal. `IslandDolphinPodTests` covers600s deterministically,
including habitat, surface/deep travel, motion extremes and tiers. WorldReport now validates and
hashes all four states/homes, fingerprint049597fca36b232e. Actual contained llvmpipe evidence
`20261012T205513.019794Z-dolphins` PASS/kernel[], one close dolphin/one draw; image inspected after
rounding the forehead, lowering/sweeping the dorsal fin and broadening the horizontal flukes.
Still faceted; melon/mouth/blowhole/fin polish, breach/wake/social behaviour, Web/Android and
integrated performance remain. PLAN dolphins and aquatic polish remain[~]. Continue whole plan,
max4 build cores, no AMD/Android/KVM changes.

## Current work — monkey face/body and hooked-tail second pass inspected

Goal remains active. Monkey trunk now has13 instead of9 loft sections; cranium and rounded
ears are larger; dark corneas have generated skin-coloured upper/lower eyelid rims. Existing
three-bone tail anchors now curl upward then forward, retaining the full fur-bearing profile
instead of the previous straight dog/rat-like rod. No new draw, texture, shader or behavior.
Release profile/gait/shader/sole tests3/3 and Debug gait PASS; Release/Debug games build.
Actual llvmpipe `20261012T202913.511429Z-monkey-tree` nine-stage sequence and
`20261012T202959.340747Z-monkeys` PASS, inspected, kernel[]. Sequence shows quadrupedal
approach/arm reach/return and stable hooked tail. Still procedural; face remains small at
gameplay distance and limb/torso realism plus Web/Android review stay open. PLAN monkey remains
unchecked. Continue whole plan, max4 build cores, no AMD/Android/KVM change.

## Current work — inland hut relocation and reference facade inspected

Goal remains active; continue the whole plan. Hut moved from the coastal strip to
(-137.765,-81.232), 57.061m from the stream. Deterministic 5m clearing removes only low
non-solid foliage; tree/rock/prop/terrain constraints remain. Dry normal-spawn route PASS:
314.243m,4800 continuous walking steps,142 waypoints. Collision radius3.25m and contact
shadow now include the0.60m veranda. Facade adds covered deck, split railing/six balusters,
three steps, closed plank door, two framed dark window recesses and exposed gable frame;
9988triangles/29964vertices, still one draw and existing palm-fibre material. Actual contained
llvmpipe `20261012T201625.232865Z-hut-material` and `201645.805217Z-hut` PASS, images inspected,
kernel deltas empty. Focused rebuilt hut/resident/bear/goat/spider/world tests6/6 PASS.
WorldReport fingerprint e744bc4e0f3093f6,807 solids/421 cells. PLAN hut remains[~]: portable
visuals, full route walk and realism polish open. Max4 CPU builds; no AMD/Android/KVM changes.
Next continue the highest-value remaining plan work; helicopter is explicitly last.

## Current work — bear native first pass live, audible and inspected

Goal remains active; continue the whole plan. Exactly one non-hostile adult bear is now in
IslandGame and WorldReport. Original procedural near/far model: heavy brown-bear torso/hump,
short tail, muzzle/nose/eyes/ears, four articulated legs, broad paws and claws, generated128
directional-fur atlas (87,380-byte mip chain), one body draw plus18-triangle shadow. No external
asset. BearMotion finds safe interior habitat and keeps a34m home range; idle/walk/trot/turn/
forage/rest/withdraw, max1.65m/s, max two swinging feet, at least two supports, fixed.48/.46m
IK, exact rendered-ground soles, world-locked stance feet, non-hostile12m player withdrawal.
Release+Debug600s tests PASS:219.719m travel, all behaviors/feet, max update.16559m, deterministic,
actual obstacles and blocked-world refusal. Final inspected llvmpipe/pixman/kernel-clean evidence
`20261012T195558.674888Z-bear` PASS: eight stages,5904/18 body/shadow triangles, natural walk
instead of forced QA withdrawal. Earlier dark hump/boots/large tail were visually corrected.

Original two-variant breath/snuffle/huff clips use existing single wildlife voice; six-source
limit unchanged. Audio inventory32clips/5,576,444PCM bytes. Release+Debug exporter and real-CNA
dummy playback/mute/range tests PASS; first positional peaceful bear call enters playback.
WorldReport PASS fingerprint d0edd20a977a3eaf. Runtime texture bytes9,305,216/9,325,432.
PLAN bear is only[~]: still procedural rather than photoreal, subjective listening and actual
Web/Android/physical performance open. Next continue helicopter, hut relocation/upgrade and
remaining fauna/portable work. Do not mark goal complete or stop early.

## Previous work — resident native first pass live and inspected

Goal active. Exactly one resident is now wired into IslandGame and WorldReport. New
ResidentRenderer streams one original3638-triangle articulated adult, driven directly by the
authoritative planted-foot pose. It has face/ears/nose/eyes/lips, close-cropped hair, hands,
woven sleeveless clothes/belt and plant-fibre sandals; generated128-square atlas has skin,
hair, cloth and fibre regions (87,380-byte full mip chain). Two terrain-fitted sole shadows
bring the visible path to2bounded draws. No downloaded character/cultural asset. Native
`--viewpoint resident` fixed clock captures8stages/64updates. Final inspected evidence
20261012T191806.756957Z-resident PASS/kernel clean: one visible actor,3638/24body/shadow
triangles,1.979m body and foot motion. First frame was too cartoonish; second pass reduces
eyes, side-hair lumps, sleeve/arm/hand bulk. It is still visibly procedural/angular—face,
cloth drape and skin lighting need polish; actual Web/Android and hardware performance remain open.
WorldReport Release/Debug PASS and now exports count1,25points,688.2493m; fingerprint
26c8147e841cd6a6. Release Core/Resident PASS2/2, Debug Resident/WorldReport PASS2/2,
software-runner51/51. Emscripten Release target compiles/links with ResidentRenderer; this is
source portability only, not an actual browser visual result. Next: commit, then continue
bear/helicopter/fauna/portable work.
Preserve all human requirements; PLAN resident[~], whole goal active.

The actual-world tour has25points/688.249m through5 landmarks: fire clearing, north corridor,
west clearing, south corridor, central trail. Deterministic A* with cached3m nodes/edges and
verified line simplification; refuses disconnected tours, no vegetation deletion/teleports.
Release constructor setup~135ms. Terrain/collision cache copied, immutable terrain ref must
outlive controller. Cave and stream modified terrain excluded. Usual pace.95m/s, hill-aware
shorter steps, boundedturns,6–10s landmark pauses, close-player yield/resume (sidestep pending).
Two planted world-space soles, one swinging foot, exact rendered-ground sole probes,
fixed.43/.44m legIK, armcounter-swing tied to actualfoot phase. Delta clamped.1/substeps.02.
Native tests use actual full obstacle set inclhut/fire/wreck, repeated1200s simulation reaches
847.907m/6pauses/atleast1lap, exactstate equality, bothlegsteps, nofoot sliding/floating/ground
penetration, fixedleg/arm lengths, fullfoot discs clear obstacles, yield/resume and blocked
world refusal. Final ReleasePASS1.19s/DebugPASS6.67s incl full foot/body obstacle assertions.
Initial deadlock at body rejection fixed by allowing airborne foot to finish;
final8 rejectedmicrosteps recover. Do not weaken these tests or claim human visuals complete.
See docs/resident.md. Do not mark resident complete until sidestep/art refinement and portable QA.

## Latest implementation — campfire functional native pass

Goal active. Campfire at46,24.7219,-16 in east clearing,11 stored logs ~6m away at51.5,-18.5.
12 stones/5 burning logs/10 embers;1992 solid triangles plus42 moving flame/smoke/spark
triangles in2 culled draws, no frame vertex uploads or new textures. Local6m flicker shared
by terrain/grass/jungle. World collision proxies1.25/1.4m, shared vegetation/grass reservation.
Original19s fire PCM adds837900bytes (current32clips/5576444bytes), shares fourth persistent voice
with stream/player-water; full3D24m attenuation. FirstApply3D precedesPlay. Real-CNA dummy
fire/stream/swim/high-flight/mute tests pass, actualfirePlaying and4–6source budget checked.
Release Core/Campfire/World/AudioReport/AudioPreviewExport/SoundscapeRuntime PASS6/6 in17.07s;
supervisor Python49/49. Final Release physical approach PASS0.28s; both native executables
refreshed. Debug Cave/Campfire/SoundscapeRuntime PASS3/3 in7.34s.
Inspected llvmpipe near180909.580398Z andwide180918.134168Z PASS, four exact animation stages
show flame/spark movement, stored wood grounded and distinct. First180516false gate failure
was an audio diagnostic demanded from deliberately silent capture mode; corrected scope.
No new kernel faults. Worldfingerprint97cc56c53b547613 now includes cave+fire geometry/proxies.
Still first-pass/stylized; smoke subtle, rock/wood need polish, live encounter/listening and
portable/performance qualification open. PLAN campfire[~], whole plan nowhere near complete.
LIVE QA nowPASS `software-campfire/20261012T182016.847082Z`: F1+O/W protected approach,
actualloopgain,1032192exactzero mute PCM,204800nonzero resume bytes, walking beyond24m
stops firevoice. Closeimage inspected; no new kernel GPU faults. Debug audiotestPASS2.92s,
both native binaries refreshed, Python50/50. New O debug teleport and opt-in audio diagnostics.
Next: resident/bear/helicopter and remaining fauna visuals,
plus consolidated portable qualification. Keep separate F11 private-window restore finding open.

Shared Sharp Runtime advanced to7805c5dc during work. Gitdiff fromlockedc20373ba contains ONLY
AGENTS.md andCLAUDE.md, parent exactlyc20373ba; all library/build code identical. Updated
island dependencies.lock deliberately, retaining strict revision gate; no dependency edits,
reset, new clone or export. Native requalification above. CNA/EasyGL/MetaGL pins unchanged.
See docs/campfire.md. Compile CPUs0–3/-j4; no AMD/Android/KVM workload/settings.

## Current work — cave live integration and visual revision

Goal remains active. Operator confirms audio fixed and reiterates continuing the whole plan,
roughly100 productive hours, not a stop deadline. Do not close the campaign after one feature.
New CaveLayout/CaveGroundPatch and IslandCaveLayoutTests implement a real20m cavity at
(184.04,7.63779,115.384),1424 shell triangles,2793 terrain vertices/5376 terrain triangles.
PlayerMotion/FlightMotion hooks now wired into IslandGame; TerrainRenderer removes coarse
quads and draws the2793-vertex replacement. CaveRenderer draws1384 shell triangles, skipping
the40 floor triangles to avoid coplanar duplication. Lightweight CaveSite reservation keeps
plants/props/grass out of the footprint. Survey before:5 palms,0 canopy,5 understory,1 bush,
7 plants,1 rock,0 logs,1 frond; after all0. Core fixture bushes574/ordinary ground plants2193.
Both actual-controller walking/flight and Core tests PASS2/2 in24.36s; runner tests47/47.
Flight/spider-habitat/snake-population/WorldReport PASS4/4 in18.40s, fingerprint8d45b4d307bfeff3.
First outside llvmpipe capture172702 passes mechanical gate but visually rejected as too
manufactured, patterned floor. Broader irregular1286.82m³ shell, projecting lip, soil shoulder
and noise floor now implemented. Inspected revised llvmpipe captures173120cave and173129inside
both PASS; floor aliasing gone, no sealing terrain, sea visible from inside. Still stylized/
tunnel-like, especially strata and rear blend; do not claim finished realism.
NEW live keyboard QA PASS `build-consumer/software-cave/20261012T173950.239540Z`: F1+L
debug teleport8m outside, W todepth19.6597 and held againstbackwall, Tab/Space ceiling,
Shift floor, Tab land, S exitdepth-6.527.88samples, all capsule-solid0. Back/exit PNGs inspected.
`tools/run_fullscreen_software.py --cave` is separate scope with no F11; parser tests48/48.
Wildlife integration follow-up: CaveSite half-width8m now covers all emitted shell vertices
(analytic bound7.62m). Boar/Goat/Snake/Spider now consult it during placement/movement;
spider live pose and snake connecting segments also guarded.18s simulation tests all bodies
and spider feet, counts6/6/50/200 retained. Ordinary bushes/ground plants573/2191.
WorldReport now fingerprints cave shell+ground geometry and exports counts/entrance/length.
Core/cave/spider-habitat/snake/WorldReport PASS5/5 in36.64s. Source/core current; native
binaries will be refreshed with next integrated build. Next: campfire functional integration,
then further natural-shape polish, portable checks and remaining whole-plan items.
No AMD/Android/KVM activity. Builds CPUs0-3/-j4. See docs/cave.md.
Both native Release and Debug executables are rebuilt; Debug CaveLayoutTests PASS6.06s.

Separate unresolved QA: private F11 gate failed173501 (originalposition561,326→2,36) and
173646 (same even after35s). Exact original size/position gate retained, now waits forasync
configure before judging. No crash or new kernelfault; no evidence cave caused it. CNA SDL
fullscreen source read only, no dependency edits. Cause may involve exclusive displaymode
1920x1080→1280x720 and Openbox; do NOT call it fixed or weaken the assertion. Continue safe
in-scope work, no goal-block/goal-complete transition. New live cave gate does not test F11.

## Latest operator feedback — FPS and audio dropout resolved

Operator confirms Release59.3FPS; runtime3558619 corroborates ~59FPS walking and flight. Close
the FPS incident; preserve current profile. Cave planning started but NO cave code was changed.
Priority switched to all-sound dropout, reported twice. New real-CNA dummy-audio CTest reproduces
the cause: stream Play happens before its first Apply3D, throwing InvalidOperationException and
DisableAfterRuntimeFailure tears down all audio. Fixed: prepare stream spatial mode BEFORE Play at both
near-stream startup and first approach; preserve four ambient/six total source budget. Runtime
start/mute/fatal diagnostics now persist; frame windows include actual audio playing count.
No AMD/Android/KVM activity or device/volume changes. Host audio state was read-only inspected.
Native Release/Debug executables rebuilt with CPUs0-3/-j4. New `IslandSoundscapeRuntimeTests`
fails before fix (`build-consumer/audio-stream-before.log`), passes after in both trees
(`audio-stream-after.log`). Python software-runner tests47/47. The runtime test links real CNA,
forces dummy audio and opens no graphics context. Actual Playing-state counts stay4..6 through
far/near startup, repeated stream/swim switches and mute/unmute. Operator subsequently confirms
audio fixed and resumes the goal. Portable runtime remains; continue cave/remaining plan work.

## Historical native FPS regression evidence

Latest finding: user's log3538060 explicitly starts `./island-simulator` in `build/`, which is
unoptimized Debug (`-g`, no -O). Existing native Release `build-consumer/` was stale Oct8; refreshed
current sources without changing quality/content. Recommend `./build-consumer/island-simulator`
from repo root for ordinary play; do not replace Debug. Both now log build configuration, and
software capture validates it against CMakeCache. Python runner 47/47; Release core/spider/snake/
rendered-terrain 4/4 PASS36.33s. Serial Release/Debug/Release CPU reports show ground snake update
0.4035/2.8509/0.8606ms, all-species ground sum1.0749/6.0692/2.1728ms. Host variation, NOT FPS.
Files `build-consumer/wildlife-cpu-release-{a,b}.txt`, `build/wildlife-cpu-debug-comparison.txt`.
Inspected Release llvmpipe jungle `20261012T164826.322843Z-jungle` PASS, no shader artifacts.
Release private fullscreen/flight/ceiling/landing/frame-window gate
`build-consumer/software-flight/20261012T164855.811504Z` PASS, no new kernel GPU faults.
Native Release cache now also enables tools/tests for qualification; preset still defaults them
off. Build CPUs0-3/-j4. No AMD/Android/KVM launched or changed. Actual Release walking FPS remains
unverified; prioritize that distinction before further reducing graphics quality.

Follow-up CPU attribution: `island-wildlife-cpu-report` (new CMake host tool, no graphics) found all
200 dormant spiders still replayed up to 15 gait/terrain steps each. Flight mean spider update
3.372ms before -> 0.0211ms after true suspension. Ground example 4.7281 -> 0.9731ms, but other
unchanged species show variable host scheduling/clocks, so only claim the clear dormant-work
removal. Reports `build/wildlife-cpu-all-before.txt` and `build/wildlife-cpu-final.txt`. Full-tier
activation discards pending reduced time to avoid catch-up jumps; all 200 wake-up paths tested.
Also cache authored snake radii and replace binary-search terrain cell lookup with an arithmetic
candidate corrected against exact rounded vertex coordinates. New representable-boundary tests
compare exact heights/normals with an independent binary search. Interactive runs now emit
`frame-window` every5s (FPS, updates/frame, update/draw ms/frame, spider/goat/snake/monkey ms/update).
Operator subsequently reported about 60 FPS high flight / 30 FPS walking. Read-only runtime
`island-runtime-3538060.log` already contains the new instrumentation: high-flight windows
54.751/56.374 FPS, walking windows 17.512–44.708 FPS. At 29.443 FPS walking, update/draw
submission cost 16.532/16.563ms per frame, 2.047 updates/frame; snakes 4.188ms/update,
spiders 0.590ms/update. This is an uncontrolled moving run, not a matched historical comparison;
do not claim the original 60 FPS baseline or that walking performance is solved. Next priority:
snake update cost and dense near-ground rendering, using fixed comparable views.
Focused spider/snake/rendered-terrain tests PASS 3/3; software-runner tests PASS 46/46.
Private llvmpipe flight/fullscreen/landing and frame-window validation PASS in
`build/software-flight/20261012T163818.420440Z`, no new kernel GPU faults. No AMD workload launched.

User reported about 8 FPS on an otherwise idle host. Read-only AMD runtime evidence
`island-runtime-3494639.log` confirms about 21 FPS before flight and about 10 FPS in flight, real
Radeon 780M/Wayland, former full profile and requested/applied 4x MSAA. Later operator feedback
above confirms partial recovery, not uniform 60 FPS. Current corrective pass removes snake-body/grass vertex coupling, staggers
reduced-tier snake updates, invalidates snake contact shadows only for nearby motion, and makes
native Linux default to `native-performance` (grass72m, vegetation55%, shadows50%, wildlife70%,
ocean96x96, MSAA0). All 50 snakes, 200 spiders and requested features remain. Web stays full;
Android stays mobile. Core, snake and 45 software-runner tests PASS. Isolated llvmpipe jungle and
overview captures PASS; overview first draw 200.735ms versus 311.281ms at the last same-world full
capture, but this is directional software evidence only. Builds used CPUs0-3/-j4; no AMD,
Android or KVM workload was launched.

## Latest implementation — turtle nesting traces

One western dry beach now has 2 deterministic traces, each 7 paired flipper sweep marks ending in
one disturbed-sand nest. Existing beach-detail renderer/batch, no collision/simulation. Core PASS.
Inspected llvmpipe `20261012T155227.429400Z-turtle-nest` PASS and visibly links nest to sea; first
pass remains intentionally stylized under the 100h budget. Dedicated fixed camera/runner included.
Current world fingerprint `b35db8900977389a`, beach details 233.

## Latest implementation — bounded bird landing and real perches

Existing shore/canopy birds now use Fly/Approach/Perched/Depart with at most two non-flying;
high birds remain airborne. Eight deterministic real supports: 5 broadleaf branch vertices,
2 solid rock caps, 1 wreck cross-tree beam. Near birds fold wings and add feet/toes in the same
batch; far birds fold wings. Core PASS covers layout/counts/all states/terrain/bank/two-agent cap/
exact contact. Inspected llvmpipe `20261012T153950.288712Z-birds` PASS222bins/std42.51/spatial206.86
and visibly places the bird on the wreck beam. Moving sequence, branch wind tracking and actual
Web/Android remain; item13 `[~]`. Builds CPUs0-3/-j2; no Android/KVM/AMD changes.

## Latest implementation — positional stream audio and wet local mix

New deterministic 13s procedural stream PCM follows the nearest point of the authoritative
`StreamRoute` through a finite 4–34m `StreamSoundField`. Near the channel it adds <=.024 to
jungle ambience and softens breeze <=12%. It shares the otherwise silent land-side player-water
playback slot: four ambient sources plus at most two transients remain, so wildlife/footsteps are
not starved. Report inventory is 25 unique clips / 4,315,184 PCM bytes; five loop lengths realign
after 290,290s. Full build, Core and independent WAV export PASS; actual spatial listening,
swim/mute switching and Web/Android lifecycle remain, so ecosystem item12 is `[~]`. Build used
CPUs0-3/-j2; no Android/KVM/AMD changes.

## Latest implementation — wind-aware coast and ambience

New `CoastalWindResponse` is neutral away from shore, ~.25 in the western lee and 1 on
the exposed east. Ocean displacement/slopes use .78–1.22, ocean+shoreline foam .75–1.25,
and surf+breeze gains .82–1.18; tide pools, jungle and player-water audio stay unchanged.
Core PASS. Inspected isolated llvmpipe east/bay/headland captures
`20261012T145730.795098Z-east`, `20261012T145824.100449Z-bay` and
`20261012T145932.019541Z-headland` all PASS without shader artifacts. East camera faces
mostly inland, so matched comparative motion/audio and actual Web/Android remain; item11
is `[~]`. Builds CPUs0-3/-j2; no AMD/KVM changes.

## Latest implementation — slow prevailing-wind cloud drift

Existing cumulus and high wisps now advance west at 0.00035rad/s through one portable
shader uniform; the phase wraps after a complete periodic turn, with no new geometry,
textures, draws, weather or day/night state. `SkyCloudDrift` CPU checks pin start,
ten-minute 0.21rad travel, wrap and invalid clocks. Full Core PASS after also correcting
the 50-snake hiss fairness fixture. Inspected isolated llvmpipe
`20261012T144704.173888Z-overview` PASS377bins/std39.79/spatial207.94 and runtime logs
the new uniform path. Item10 remains `[~]` pending separated-time motion comparison and
actual Web/Android. Builds CPUs0-3/-j2; no AMD/KVM changes.

## Current operator target — roughly 100 productive hours

The earlier broad 250–450/roughly-400-hour implementation outlook is superseded. The
operator set approximately 100 productive hours as the target for implementing the whole
remaining functional scope. It is not a stop condition: continue until the operator stops
the campaign, accept added hours if offered, and use any time saved by early functional
completion to deepen visual/motion/material quality. `PLAN.md` allocates the target across stability/controls,
wildlife, reduced-fidelity animal repair, ecosystem links, hut/cave/campfire, resident/
bear, grounded helicopter and final integration. Simplify initial model/material/animation
variety before cutting required behavior, but do not stop merely because 100 hours elapsed.
Builds remain limited to CPUs0-3 and `-j2`;
do not touch Android emulator/KVM or AMD GPU state.

## Latest implementation — 50 snakes and 200 spiders

The exact requested live populations are now implemented. Fifty snake homes are chosen
deterministically on understory margins with >=14m separation; 200 spider homes use the
existing qualified forest habitat with >=4.5m separation. Spiders now use distance-tiered,
staggered simulation. GPU work is independently bounded: nearest 16 snakes, 24 spiders,
and 16 contact shadows per species. The later FPS repair removes snake-driven grass deformation;
snakes still move, render, hiss and cast nearby shadows without making dense-grass vertices search
animated body segments.
WorldReport fingerprint `b0f0ae75a88ce3c3` exports 50/200. Focused population/motion,
WorldReport validators and portable preflight tests pass 11/11 in 241.75s. Inspected
llvmpipe `20261012T143710.846753Z-snakes` and `20261012T143753.699776Z-spiders` each
show one live grounded subject and report 1/50 and 1/200. Both retain their known dark
close-view generic-contrast failures, so visual polish, long-walk encounter/performance
review and actual Web/Android runtime remain open. No AMD/KVM changes.

## Latest implementation — four coastal identities, first composition pass

New `SampleCoastalExperience` provides finite shared weights for the broad south beach,
sheltered west bay, exposed rocky shore and localized stream/mangrove mouth. Natural
props use it without new draws/assets: exposed sectors gain rock density, the main
beach selects shell/pebble clusters without wrack, and bay/mouth sites bias toward the
existing wrack form. Core default counts are 36 rocky-shore rocks including ten
tide-pool rim accents, 41 main-beach
shell/pebble clusters, zero main-beach wrack, three bay wrack and two mouth wrack.
Current fingerprint `18ba6c1fe51b097f`; Core, WorldReport and capture preflight pass.
Inspected llvmpipe bay `113137`, headland `113202` and bay-ground `113228` pass and
show macro contrast. The technically passing `113111` beach image was rejected because
it points inland. Item 7 remains `[~]`: close prop proof, water/audio contrast and
portable qualification remain. No AMD/KVM changes; builds CPUs0-3 and `-j2`.

## Latest implementation — deep closed-canopy monkey homes

All seven production monkey homes now use scale-specific stages beside closed-canopy
trees at least 90m inland and at least 7m apart. The stages were found offline by
running the existing 1502-sample attachment and 1951-sample full-excursion clearance
validators, then are installed only when their tree indices, headings, terrain and
closed-canopy predicates still match. Runtime tree choices continue to exclude
mangroves and isolated canopy. Core retains >900m ground travel plus natural climb,
summit and interruption coverage. WorldReport now calls `SetCanopy` in runtime order.
Current authoritative world fingerprint is `f6d038aec99ebb57`.
Inspected isolated llvmpipe `20261012T111900.787421Z-monkeys` PASS 272bins/std20.03/
spatial69.07 and clearly shows dense interior undergrowth. It also clearly shows that
the long-tailed model still reads as rat-like: habitat accepted, monkey appearance
rejected and still planned. No AMD/KVM changes; builds remain CPUs0-3 and `-j2`.

## Earlier implementation — ecological wildlife placement, first pass

Ecosystem-stage item 6 is now `[~]`, not complete. Existing crab sand habitat,
butterfly clearings and canopy-adjacent spider homes were audited; fish already use
shared reef/seagrass suitability. Boars now require shared-field moisture>=.16,
drainage<=.74 and salt<=.12 and their three paired regions follow the vegetated western
stream banks. Goats reject moisture>.18, drainage>.24 and salt>.08. Four production
snake homes are deterministically selected from sampled dense-understory margins,
with >=52m separation and fail-closed construction; legacy no-jungle constructors
remain for isolated locomotion tests. Three of five turtle bases now occupy the
sheltered western bay, while all five retain reef/seagrass routing. The fingerprint
at that earlier stage was `7362757530e472d9`. Inspected llvmpipe `20261012T102822.874894Z-boars` PASS
274bins/std18.04/spatial88.19 and `20261012T102941.088316Z-turtles` PASS
268bins/std26.31/spatial104.02. Inspected `103005.681421Z-snakes` shows the animal
between understory leaves but FAILS the generic scene threshold (std13.74), so is not
accepted evidence. Monkey tree targets now reject mangroves/isolated trees and require
>=2 natural neighbours within20m, with a cached eligibility table. The initially
attempted direct relocation was rejected; its replacement is documented in the newer
section above. Generic capture `104842.384627Z-monkeys` faces out of the grove and is
not accepted evidence; `104937.753599Z-monkey-tree` failed its paired-grip gate.
Portable/performance review and the monkey visual redesign remain.
No AMD/KVM changes. Builds remain bounded to CPUs0-3 and `-j2`.

Crab follow-up: runtime and WorldReport now build habitat cues from every non-solid
driftwood placement plus beach-detail variant3 dry wrack. `SetDebrisHabitats` selects
six >=28m-separated sandy anchors transactionally and rehomes five crabs per anchor;
all 30 homes are <=3.25m from real debris and retain existing bounded sand routes.
Core and WorldReport passed at fingerprint `7362757530e472d9`. Inspected llvmpipe
`20261012T110201.712430Z-crabs` PASS 402bins/std24.02/spatial145.92 and visibly shows
the foreground colony surrounding a small piece of beach debris.

## Latest implementation — spatial reef plus habitat-routed fish/turtles

NaturalPropLayout now has314 non-solid seabed-fitted reef instances selected by new
shared ReefSuitability: staghorn/compound-round/stacked-plate/underwater-rock forms.
NaturalPropRenderer reuses cells, instancing and shader; all4 forms have near/far LOD,
138m cull and underwater material. Total prop meshes3904vertices/109312bytes. New
SeagrassSuitability + combined AquaticHabitatSuitability anchor all8 fish schools and
all5 turtle home/range checks; Core long routes remain deterministic/in habitat.
First100513 image rejected sparse/simple;100813 rejected rock-focused;100923 improved
branching; final isolated llvmpipe101426.680059Z-reef-habitat PASS165bins/std24.98/
spatial128.65/kernel[], VIEWED:51/314 reef habitat,3fish,1turtle submitted. World
fingerprint4c0ea30b6c5aaac1. Native perf/motion and Web/Android remain; stage [~].
Goal active; no AMD/KVM/dependency changes.

## Latest implementation — localized mangrove mouth habitat

Exactly15 deterministic mangroves now occupy only the lower shared stream/mouth.
Jungle canopy variant8 adds low broad foliage, 3 secondary stems and 8 curved stilt
roots near (2/5 far), lighter grey bark and existing instanced material/wind/LOD.
Placement requires moisture>=.30, salt>=.06, drainage<=.76 plus shore/height/slope/
rock/route/canopy clearance; Core additionally proves no palm collision overlap and
root-radius coverage. World fingerprint32d95c2a1819702e:327palms,272canopy including
15mangroves,1781understory,588bushes,3026groundplants(37bank),16943grass,11logs(2bank).
First actual094932 capture reproduced the user's fatal startup error: curved support
frame used reversed cross-product, so procedural outward-normal validation aborted.
Frame fixed; second095137 passed but visual inspection rejected nearly black bark/
inside-colony camera. Final isolated llvmpipe095538.769586Z-mangroves PASS410bins/
std49.05/spatial204.42/kernel[]; image inspected and shows readable stilt roots,
multiple stems, low crowns, mouth water and inland context. Core PASS after added
palm/root assertions. Web/Android/performance/motion review remains; goal active;
no AMD/KVM/dependency changes. See docs/mangroves.md.

## Latest implementation — one shared meandering stream route

New StreamRoute is shared by StreamCourse, replacement bed, water mesh, CPU/GLSL
moisture and wet-bank plant/log placement. Middle course bends -2.1626..+1.4484m
from the old line but returns exactly to source/mouth; actual polyline101.879m.
Bed now projects onto nearby real segments. First X-only attempt correctly aborted
with "Stream water intersects bed"; fixed +/-6 local search passes all sampled
water/bed clearances. Final patch8696vertices/17282triangles, deepest cut.429558m;
water3429/6080. Fingerprint583c5a9ba1150779:327palms,1781understory,3028groundplants
(39bank),12logs(3bank), grass16943. StreamSurvey+Core+RenderedTerrain+
TerrainMaterial4/4 PASS100.29s.
Actual final isolated llvmpipe092831.265962Z-stream PASS391bins/std50.48/spatial233.38/
kernel[]; build/stream-meander-final.png and before/after inspected: visible bend,
readable damp log, no separation artifact. Motion, richer bank relief and portable/
performance QA remain. All jobs terminal; goal active; no AMD/KVM/dependency changes.

## Latest user addition — boar and goat sounds

PLAN now explicitly requires convincing positional sounds for both species: audible
boar grunts/snorts across rest/forage/wander and varied goat bleats with restrained
close breathing/hoof detail. Calls must follow behaviour, vary without repetition,
attenuate with distance, respect the shared voice budget and remain audible against
the ambience. Acceptance requires recorded/listened runtime output plus overlap,
mute/unmute and native/Web/Android lifecycle checks. This intake note is superseded by
the current implementation entry at the top: goat vocalisations are now live and
backend-verified, but neither species has subjective or portable acoustic acceptance.

## Latest implementation — irregular stream-bank microhabitat

Added marked wet-bank plants/logs without a new renderer: 39 mixed existing
fern/broadleaf plants in deterministic irregular one-bank colonies with gaps, plus
two supported fallen logs using the darker existing bark. The first 47-plant
paired-station version was actually captured and rejected because it formed
obvious double rows. Current world fingerprint965c50c3c8a40e47:3023groundplants,
723forestcompanions,11logs; base populations remain2201/576. Final
StreamSurvey+CoreTests2/2 PASS51.31s. Actual isolated llvmpipe090622.678487Z-stream PASS392bins/std50.81/
spatial238.05/kernel[]; build/stream-bank-irregular.png inspected and rows are gone.
This is only the first microhabitat composition: stream form still straight/simple,
logs not prominent, no motion/Web/Android/performance acceptance. All jobs terminal;
goal active; no AMD/KVM/dependency changes.

## Latest user addition — one realistic large bear

PLAN now explicitly requires exactly one large realistic bear in a suitable island
habitat. Prefer a high-quality rigged internet model only with verified inclusion
and redistribution rights for mesh+textures and retained attribution/licence;
otherwise make an original asset. Required: authentic bear anatomy/layered fur,
weight/ground contact and quadrupedal idle/walk/trot/turn/forage motion, plus
restrained positional vocalisations and breathing/snuffling tied to behaviour.
Keep it non-hostile under the existing peaceful scope and within portable LOD,
animation/audio/geometry budgets. Require actual close/front/side/moving visual
and audible runtime evidence. Not implemented or sourced yet; no licence claim.

## Latest implementation — shared wet corridor and cleared live stream

IslandEnvironmentField now has normalized CPU SampleInland moisture/drainage/
saltExposure plus matching terrain GLSL, derived from weststream/southvalley/coast.
GroundCover Density takes moisture/drainage; Grass uses it and rejects drainage
>=.82. Palms reject>.68; canopy>.68; understory>.82; bushes>.78; groundplants>.84.
Moisture modestly boosts non-channel jungle layers; fallenlogs use moisture and
reject>.72. Terrain gets damp earth/moss/sparse stones. Stream mesh centre, width,
opacity vary inside bed; water color/alpha retuned. Camera now(-110,ground+3.5,-37)
to target(-122,terrain-.18,-26.94). Final corrected south-valley field measured
world fingerprint0c6b31374962d7d1:326palms/16943grass/257canopy/1780understory/
588bushes/2986groundplants/9logs. Core base populations now576bushes/2201groundplants.
Final corrected build23367exit0; StreamSurvey/Core/TerrainMaterial99575exit0 PASS43.07s.
Actual final capture80000exit0 evidence084525.578398Z-stream PASS388bins/std50.99/
spatial239.36/kernel[]. Viewed build/stream-wet-biome-final2.png: channel open,
readable and connected to bay; less harsh after first083733 image, but still
linear/simple, NOT final art acceptance. No Web/Android/perf/motion proof.
All handles terminal. Full goal active; no AMD/KVM/dependency changes.

## Latest — first stream water live, visual polish still open

Added StreamWaterMesh/Renderer to IslandGame+CMake after shoreline pass. Static
3429vertices/6080triangles,16bit, one culled transparent draw, GLSL330/GLES300
downstream ripple normals/Fresnel/glint. Mesh inside.8*bedhalfwidth, alpha ends/edges
fade; survey checks winding and interior bed clearance. Fixed missing GL clip-depth
conversion after real captures showed invisible water. First-visible diagnostic.
Stream QA camera now(-137,ground+3.5,-28),target(-145,ground-.25,-21.65).
Native build65259exit0, StreamSurvey/CoreTests76083exit0 PASS61.32s before final
shader-only fix. Final capture95028exit0 evidence081415.552173Z-stream PASS348bins/
std23.82/spatial114.83/kernel[]. Viewed build/stream-water-depth-fixed.png:
water visible to bay but unnaturally straight/soft green strip, NOTartacceptance.
Earlier081023closecamera failedcontrast;080826/081215passedtechnicalgatebutwater
invisible (depthbug), retained. Need wetbanks/substrate/lessuniformedges, shared
placement clearance and authoritative grounding, lighting +motionsequence review.
NoWeb/Android/fullsuite/performanceclaim. Allhandles terminal at this note.
Added finite/alpha/bounds/index guards to survey after finalcapture; finalbuild
and focusedtest64140exit0 PASS.14s (stream-water-final-{build,tests}.log).
Fullgoal remainsactive, noAMD/KVM/dependencychanges. Latest aquatic request below
retained; no species task marked complete. Prior sections chronological.

## Latest user addition — aquatic/coastal wildlife appearance

Added explicit unchecked PLAN task: improve fish, shark, dolphins, turtles and
crabs visually (anatomy/material/details/motion and actual view audit). Dolphins
remain unimplemented, so appearance work does not replace their population task.
Keep ecosystem stage priorities and all earlier requirements.

## Latest — stream bed LIVE in terrain, no stream water yet

Previous119a265 progress. TerrainRenderer owns StreamBedPatch, excludes original
coarsequads by centre-inside alignedrectangle, percell indexcounts now actual.
Separate16bit VB/IB avoidglobal58081+8510>65535; newfrustumbounds/existingmaterial,
oneextra draw ifvisible; getters totalcounts. Patch normals/slope/light sampled.
PlayerMotion SetGroundPatch borrowed after renderer installed; walking footprint
height/slope and uphill checks usepatch. StreamSurvey adds actual-player support
tests alongroute. Build91393exit0 +StreamSurvey/Core PASS45.58s. New --viewpoint
stream enum/parser/camera/toolchoice; finalcamera build28423exit0. taskset0-3/-j2.
Actual60227exit0 software-captures/20261012T075212.867607Z-stream PASS350bins/std41.25/
spatial210.68/kernel[]. Actualbuild/stream-bed-live.png VIEWED: subtle drycut with
plants/tree obstructing route, NOTvisualcompletion. No water yet. NEXT watermesh/
flowshader and wetbanks + sharedplacementclearance. Existinggrass/props/wildlife/
contactshadows/packedRenderedTerrainHeightMap and diving/flight surfacecache still
oldsurface: must reconcile all, notclaimauthoritativeworldcomplete. Onlyplayerwalking
wiredthisturn. NoWeb/Android/fullsuite/performanceclaim. Allhandles terminal;
noAMD/KVM/dependencychanges; fullgoal active.

## Latest — adaptive stream-bed geometry reduction

Previous253a32a progress. StreamBedPatch marks coarsecells containing ANYmodified
gridvertex (bothneighbors onedges), keeps8x there. Unmodifiedcells usecenterfans
onoriginalTR-BLdiagonal, splittingonlyedgesadjacentfinecells; compactunusedvertices.
Current8510verts/16910tris vs36273/71680 (~76%reduction),samecut.433233m.
SampleHeight keeps36273scalarheights145092bytes, samefinegridinterpolation; coarse
fanvalues matchoriginalplanes withinexisting1e-4 checks. New edgeincidence map
requiresallinterioredges exactlytwice/oppositedirection; onlyouterboundarysingle.
Tests retainallvertex/trianglecentroid/nonraising/boundary/depth/outside checks,
also require>50%countsreduction. Build/test19661exit0PASS.10s, taskset0-3/-j2/cache.
Logs build/stream-adaptive-{build,tests,summary}.log and-profile.csv.
No live stream/renderer/terrain replacement/collision integration yet. NEXT remove
coveredcoarseterrainindices, uploadboundedpatch withsame terrainmaterial, and share
surface withmovement/grounding beforewater mesh. Do not overlay old terrain.
No graphics/AMD/KVM/dependencychanges;allhandles terminal; fullgoal/planretained.

## Latest — CPU local stream-bed replacement geometry, not live

Previous226f5fd progress. New StreamBedPatch.hpp subdivides coarse-grid-aligned
route rectangle8x (~.354m), samples exactoriginalRenderedTerrainSurface then lowers
parabolic wetbed with2mbanks/source2mfade/outlet4mfade. Outerboundary unchanged;
indices upward,16bit. SampleHeight uses same triangle interpolation, inclusive
boundary tolerance. NEVER layer this over old terrain: replace covered triangles
and route collision/grounding through same surface when integrating.
Current fullrectangle36273verts/71680tris,maxcut.433233: TOOcostly; next adaptive
refinement only cutregion/stitched coarse surround before live renderer integration.
Survey validates everyvertex+trianglecentroid, boundary, winding, cutbound, station
wetdepth>=.1 aftersource, outsidequery rejection. Finalbuild/test52905exit0PASS.10s;
initial72941exit0. Logs build/stream-bed-{build,tests}.log and -summary.log/rawprofile.
taskset0-3/-j2/sharedcache; nofullsuite/graphics/AMD/KVM/dependencychanges. No stream
in game yet. Allhandles terminal; new ecosystem ordering/fullpriorrequirements remain.

## Latest — western stream route surveyed and layout foundation

Previousb475e33 was progress. New StreamCourse.hpp and tools/StreamSurvey.cpp plus
CMake host target/test. Actual westvalley z=-55-.23x sampled x=-65..-163 reaches
western bay;99stations/100.559m,ground9.82824->-.00209, no sampled uphill,rawcutmax.4m.
Layout water=ground-.15/minfall.002 then clamp>=0 for sea-connected mouth;bed-.25,
halfwidth.45..1.5 smooth. Guard>1m incision throws. No terrain/render integration.
Native survey71623exit0; finalbuild/survey/test72056exit0, testPASS.07s. taskset0-3/
-j2/cache. Logs build/stream-{survey,course}-build.log, stream-course-tests.log,
stream-west-profile.csv and -summary.log. CSV is RAWunclampeddiagnostic, layout
is sea-clamped. docs/stream-course.md describes exactscope/remainingwork.
NEXT local channel/bank mesh + coherent terrain/collision carve; coarse2.83mgrid
cannot faithfully represent.9m upper wetwidth by itself. Avoid overlaying uncut
terrain withwater. Then render/flow/wetmaterials/placement/environment/audio.
No graphics/AMD/KVM/dependency changes;allhandles terminal;goalactive/fullplanretained.

## NEW PRIORITY — user's complete 17-point ecosystem stage

Read docs/ecosystem-stage.md: every numbered request preserved as unchecked work,
including constraints and order. Highest next major stage is stream+moisture/
drainage+wet habitat+localized mangroves+spatial reef/habitat-driven existing fauna.
No broad redesign/enlargement/weather/survival. Helicopter retained LAST; prior
species deferred, not silently cancelled. Other previous requirements retained.
User asked to record these tasks; no ecosystem implementation claimed this turn.

## Current diving audit — failed before underwater evidence

Added --diving to supervised fullscreen worker: F12 offshore, Shift dive, Space
resurface, screenshots + trace validator; new swim-position diagnostic in game.
Build84875exit0;44CPUpreflight PASS0.064s (including8invalid dive traces), taskset0-3/
-j2/cache. Actual74576terminal exit1, evidence software-fullscreen/20261012T073057.137680Z,
kernel-final[]. workload.log: "timeout waiting for fullscreen request" during
the harness fullscreen preamble, BEFORE diving. input-result.json absent. No
underwater screenshot/success yet. Inspect focus/input in owned logs before retrying;
process terminal, not pending. Do not claim runtime
diving proved. New priority request arrived after failure; leave evidence intact.

## Latest — diving movement integrated, runtime appearance NOT verified

PlayerMotion opt-in SetSwimInput(RenderedTerrainSurface*,up,down) with caller-owned
lifetime, SwimDepth state. Submerged horizontal substeps reject swept rectangle
HeightRange maximum+clearance above eye; outside mesh rejects. Conservative whole
quad-vertex bound, not exact contact distance. Final vertical uses footprint support.
Teleport/land reset. IslandGame lazy232324-byte rendered surface before PlayerMotion
member; first Space/Shift in swim allocates, keys refreshed each frame, Shift swimming
doesn't change ground pace, underwater bob suppressed. Flight reset retained.
Core regression actual-water descent/held input/inward seabed block/swept support/
surface/teleport. First run11088 failed ONLY habitat premise (25m shelf not3mdeep);
changed to actual38moffshore support<-1.3 and .5s descent below-.2, not a relaxed
collision assertion. Build+Core handle30161 terminal exit0, CorePASS40.77s. Logs
build/diving-motion-{build,tests}.log. taskset0-3/-j2/cache. No graphics yet.
Next actual input/runtime underwater audit, underwater side of water/fog/audio,
deeper routes/shore/safety-correction and depenetration/focus tests. Entire plan
retained, diving not complete; docs/diving.md has current contracts.

## Latest — diving vertical controller foundation, NOT live diving

Previousf8c366d was progress. SwimDepth.hpp introduced: surface.58m, floor support+
.45m,1.6m/s,dt<=.1, Space/Shift booleans cancel, hold/reset, reject invalid support.
No game wiring/render/audio yet. CoreTests add floor/surface/opposed/idle/invalid/
stall/risingground/dry/reset cases. Build+Core handle4485 terminal exit0; CoreTests
PASS41.96s. Build taskset0-3/-j2/sharedcache; logs build/swim-depth-{build,tests}.log.
Next integrate into PlayerMotion authoritative state with swept RENDERED seabed
footprint collision (current height is analytic/always surface), then underwater
render/audio and real-input images. docs/diving.md details remaining scope.
No graphics, AMD/KVM/dependency changes. Full animal/hut/stream/cave/helicopter/
campfire and platform/audit backlog retained; goal active.

## New user requirement — realistic campfire and gathered firewood

Added unchecked PLAN task: suitable location, burning stone-ringed campfire,
realistic animated flames/embers/light/sound, separate gathered-log pile with safe
clearance. Evaluate portable CNA shaders/particles, verify actual visual result.
Not implemented; no fuel interaction/crafting requirement inferred.

## Latest — flight remapping implemented and native tested

Tab toggles, Space ascends, either Shift descends; opposed keys neutral,12m/s no
Shift speed boost. Ground pace and F3 debug Q/E/Shift controls retained. Updated
overlay/transition messages/docs/Web shell and native/browser QA drivers. Browser
Tab/Space default prevented only for focused or pointer-locked canvas; CPU test
covers page navigation preserved. Native build44320exit0 taskset0-3/-j2/cache;
FlightMotion/BrowserFullscreen/SoftwareCapturePreflight3PASS1.20s, JS/Python syntax
passed. Actual73145exit0 software-flight/20261012T071541.812529Z PASS/kernel-final[];
heldTab single activation, Space ceiling, bothShift descent, simultaneousSpace+
rightShift neutral, safe return. Notice screenshot ACTUALLY viewed/readable.
No Web/Android runtime/fullsuite/gameplay focus-loss qualification. All handles
terminal; AMD stop unchanged. Latest diving/campfire requests remain unimplemented.

## New user requirement — underwater diving

Implement diving below the water surface: Space ascends, Shift descends. Added
unchecked to PLAN; not implemented. Use real underwater camera/motion with seabed
collision, surface transitions, underwater view/audio; do not substitute current
near-surface haze. Separate from flight, preserve existing horizontal swim controls.

## Latest — finer boar source v2 integrated

Previous94391a7 was progress. Used imagegen skill/built-in tool, no references;
new assets/art-studies/boar-bristle-albedo-v2.png plus exact sibling prompt, both
manifest/license recorded. Source1254square4247297bytes preserved; v1 retained.
SourceSHA79490e2046ba87f0e5260e39981f0e2e29909a2b2ed32326a4b2462b5cd50c15.
Actual prepared256square153886bytes SHA0b292105904b1337de14a8fdb2323a934aef61f8bf2d3b7066a991fac3fcf880.
Source and reduced texture viewed; native/Web CMake + Android source selection v2.
Same runtime349524bytes/9mips and two draws. Build80418exit0 taskset0-3/-j2/cache;
AssetValidation/PreparedMaterial two testsPASS34.01s. Actual3535exit0 capture
071024.005936Z-boars PASS320bins/std46.80/spatial221.52/kernel[]. PoseJSON exactly
matches070439.874400Z; actual stage0crop build/boar-v2-after.png inspected: finer
less contrasted body fur, head still stretched/streaked and anatomy simplified.
No full or portable qualification; do not claim boar task finished. All handles
terminal. Full priorities retained; newest flight remap below is planned only.

## Earlier user request — flight key remapping (native implementation above)

Add to tasks: Tab toggles flight, Space ascends during flight, Shift descends.
Supersedes Space toggle and Q/E vertical movement. Update handling/help/tests;
retain grounded controls and collision/100m ceiling/island boundary constraints.
Native implementation tested above; portable/focus qualification remains.

## Operator build limit — 2026-10-12

All future compilations must use at most four CPU cores, per explicit user request.
Use `-j4` / `--parallel 4` or fewer workers; never unbounded parallelism or `-j$(nproc)`.
Also bound nested compiler/tool workers (Emscripten, Binaryen, Gradle/native builds)
and serialize builds so their combined compilation load stays within this limit.
Use CPU affinity to at most four allowed logical CPUs when tools spawn internal
workers. Existing `-j2` builds remain appropriate. This overrides older uncapped
build guidance; do not change Android/emulator/KVM configuration.

## Latest — shorter boar muzzle and denser short-hair nasal mapping

Next-turn follow-up: repeated unmodified capture18407exit0 at070112.556340Z-boars
passed, kernel-after[]. Both boar-motion.json AND finalcapture.ppm byte-identical
to065903.868384Z (cmp exit0), proving these two runs repeat exactly.
Near ears now cut original eight outline corners at20/80percent into16points;
cup/back/root frame unchanged,64triangles/ear vs32 (+192vertices/near animal),
far8 unchanged, same GPU capacity/two draws. Closed-edge/winding/mirror/unit-normal/
pose-envelope CPU tests passed0.64s. Native build90540exit0, taskset0-3/-j2/cache.
Geometry-only capture45686exit0 at070252.098123Z-boars passed/kernel[], same pose
JSON as baseline. Actual enlarged matched stage0 images boar-ear-{before,after}.png
inspected: modestly rounder silhouette, still plain pasted-looking colour patches.
Then mapped existing fur onto outer rim/back (inner cup remains solid), no new
texture or draws, UVs bounded. Build27156exit0, taskset0-3/-j2. Final fur capture
97507exit0 at070439.874400Z-boars passed318bins/std46.82/spatial221.77/kernel[].
PoseJSON still byte-identical to baseline. Actual enlarged stage0
build/boar-ear-fur-after.png inspected: outer ears now fur rather than flat patches,
modest improvement; inner cup/face still simplified, coat over-streaked and cheek
elongated. Not overall artistic completion. All handles terminal, no full/portable
run for ear changes. Next tackle coarse directional coat/cheek shape, then retained
monkey/hut/stream/cave/helicopter priorities; do not abandon full plan.

Follow-up: boar capture now advances by0.1s/update with fixed-step catch-up disabled,
uses that clock for renderer animation, follows subject translation only, and saves
eight stages at8..64frames. Normal gameplay/audio inspection unchanged. New capture
validator checks ordered clocks/finite states/ranges and all eight image files;
43 preflight tests PASS0.081s, native incremental build25018exit0 with taskset0-3/-j2.
Actual contained capture63302exit0:20261012T065903.868384Z-boars PASS318bins/std46.82/
spatial221.77/kernel[]. Stages0/2/4/7 montage build/boar-clock-stages.png ACTUALLY
viewed: forage-to-walk/rooting.96->0, speed.11->.85; coarse fur streaks/angular ears,
rear partly obscured by existing vegetation. No artistic acceptance. Repeat capture
and compare boar-motion.json to verify reproducibility next; only one deterministic
run so far. No Web/Android qualification. All owned process handles terminal.

Previousa33958f was progress. BoarMuzzleZ inBoarPose.hpp compresses authoredz>.78
by.70; tip1.060->.976m. Body profile, fitted mouth/outereye rim, nose/nostrils/tusks
sharetransform; depth radii .7 too. Feet/limbs/ears/vertical rooting unchanged.
BoarFurAlong continuous atbridge, reverses into denser sourcewindow towardnose,
>6x body longitudinal density. No newtexture/buffer/draw change. Regression adds
tip/identity/UVcontinuity/range/density, originalface-groundcontact tests retained.
Build14253exit0 and focusedpose/herd/coat PASS11.19s. Actual contained97120exit0
064822.203943Z-boars PASS332bins/std44.06/spatial195.44/kernel[]. Actual
build/boar-muzzle-after.png inspected: finer nosefur, stillcoarsecheekstreaks/ears/
limbjoins. Pose differsfromlastview duewall-timecapture; NOTstrictbeforeafterproof.
NEXT deterministic boarcaptureclock/multi-poseinspection orremainingrealism fixes;
fullotherpriorities hut/stream/cave/helicopter etc remainopen. Noportablebuild/runtime.
Full CPU handle41527 completed with exit0: all55 groups PASS207.54s.
Logs build/boar-muzzle-{build,tests,full-tests}.log. Reusedbuild/-j2/cache.
NoAMD/KVM/Android/dependencychanges. Goalactive/no push.

## Latest — generated boar coat LIVE, muzzle mapping still needs refinement

Previous565ddda was progress. BoarRenderer now requires256squareRGB
Content/wildlife/boar-bristle.png,9mips349524bytes, no procedural fallback. Prepared
hash7dc6d99d... matches preview. Body+upperlegs use near-neutral variant tint and
MonkeyFurUv Along/Around interior mirrored windows; all other anatomy solid marker
(-1,-1). Validate homogeneous triangle material before stable_partition, one bounded
36000vertex upload/two opaque BasicEffect draws. Partition may allocate bounded CPU
temporary, no new permanentGPUvertexbuffer. Old proceduralcoat code/tests historical.
Generated-only4586636bytes; runtime9130456/caps9150672 (delta+283988). CMake native
install/Webpreload andAndroid materialprep updated; exactChrome/Firefox/Androidsmoke
budget strings updated. Boar capture requires newmaterial diagnostic and2draws.
RGB UV regression and actualsource host/C++pixelcomparison added. Final nativebuild
andfull55CPU PASS136.47s (2257exit0); logs build/boar-material-{final-build,full-tests}.log.
JS/shellsyntaxPASS. No Web/Android build/runtime. Final unusedinclude cleanup rebuild
PASS (52536exit0), build/boar-material-cleanup-build.log; no executable semantics change.
Actual first063826.490440Z (24976exit0) inspected build/boar-bristle-live-first.png:
clearfur/plainupperlegs/headstreaks. Addedupperlegfur. Final064239.018898Z (23306exit0)
PASS324bins/std44.05/spatial195.52/kernel[], actual build/boar-bristle-live-final.png
inspected: upperlegsnowfur, stilllongmuzzlestreaks/coarselimbs. NOTfinalrealismacceptance.
NEXT correct muzzle UV scale/stretch and head anatomy; furtherboar/monkeyrefinement,
hut inland/reference rebuild, stream,cave,enterablegroundedhelicopter/fullplan remain.
Reused build/-j2/sharedcache. NoAMD/AndroidKVM/dependencychanges. Goalactive/no push.
All owned handles terminal; stop latch unchanged.
Disk build8.8G/build-probe430M/android/app/build412M; no artifacts removed.

## Latest — original boar bristle source prepared, runtime integration NEXT

Previous12274a6 was progress. Used imagegen skill (read SKILL and both shared prompt
refs), built-in tool, no references. New1254square opaque original4,135,795bytes at
assets/art-studies/boar-bristle-albedo-v1.png; exact prompt sibling-prompt.txt.
Image SHA26576df7..., prompta0086021..., recorded in manifest/license table.
Tool source .../generated_images/01a129bd-2c19-7192-a43c-adcfe79c0fb9/
exec-ad125c01-bfe7-436f-a8bb-755f957f65f7.png copied unchanged, original retained.
Source ACTUALLY inspected: coarse short grey/brown/tan overlapping bristles, slight
oblique clumps. Existing prepare_material_texture.py --size256 produced actual
build/boar-bristle-preview-256.png,154913bytes/hash7dc6d99d..., also inspected.
No seamless or final art acceptance claim; source contrast needs live assessment.
NO runtime code/render/material/packaging change yet. Integration NEXT: load required
file-backed RGB coat+mips, neutral body tint instead of old dark-brown multiplier,
separate solid anatomy from textured body (newsource has no oldwhiteatlasstrip),
interior directional UVs, update budgets/native/Web/Android content, actual capture.
See docs/boar-coat-material.md. Skill used built-in, noCLI/API key fallback.
All other user tasks remain; goalactive/no push. No GPU/Android/dependency changes.
AssetValidation+PreparedMaterialTests PASS9.06s,33318terminal0;
build/boar-bristle-source-tests.log. No compile/fullsuite or renderer for source-only
change. All owned handles terminal; no artifact deletion.

## Latest — boar shoulder/bristle silhouette, visual task still open

Previousf86b2b4 was progress. MakeBoarBodyProfile rear/abdomen anchors reduced,
same shoulder/head/limb anchors; shoulder now17cm above rear versus13cm. New
BoarBristle.hpp shared135-strand/three-row shape, roots seated6mm in loft, variable
adult34–67mm/juvenile20–53mm raised tips and rearward lean. Replaces45triangles,
+270near vertices/animal, no extra draw/texture/buffer allocation. CPU tests cover
all rooting/bob/age variants, finite bounded tips, attached roots, shoulder/rump.
Build90480terminal0; pose/herd/coat PASS4.59s. Actual contained software67202exit0
062718.488953Z-boars PASS317bins/std43.88/spatial195.58/kernel[]. Image ACTUALLY
inspected build/boar-silhouette-after.png versus previous build/boar-current-review.png.
Modest leaner rear/fine broken dorsal edge; smooth flank, long muzzle/angular legs
still obvious. No complete boar visual acceptance, motion or portable qualification.
Full CPU handle89368 terminal0: all55groups PASS94.79s.
Logs build/boar-silhouette-{build,tests,full-tests}.log. Reusebuild/-j2/sharedcache.
NEXT substantial boar coat/head/limb improvement; all other user priorities remain.
Goal active/no push. No AMD/AndroidKVM/dependency changes; stop latch unchanged.
All owned handles terminal. Disk build8.8G/build-probe430M/android/app/build412M;
no artifacts removed. Stop latch SHA c0bd1704 unchanged.

## Latest — quiet boars now grunt; actual disk output/mute verified

Previous1be0bd4 was progress. Current actual boar image062020.501241Z (29191exit0)
PASS/kernel[], inspected build/boar-current-review.png: smooth barrel-like body and
long snout remain visual defects. No model/material change this turn.
Found Rest only breath/Forage only snuffle, never grunt; rest7m gain~.0069. This is
a source-level limitation, not proven explanation of all silence on user's device.
BoarCallScheduler per-animal phase now grunt/detail/detail, initialized/reset0; if
detail gain<.03 substitute grunt. Same behaviour cooldowns, herd gap,42m range, clips,
voice budget and bird/monkey arbitration. Phase advances only when request emitted.
CPU tests isolate Rest/Forage at2/7/15/30m for120s, repeated grunts/first<11s given
available slot, retained2m details; original deterministic/range/budget tests retained.
Build/full55CPU PASS87.77s (57830exit0), logs build/boar-grunts-{build,full-tests}.log.
Actual contained software/disk audio062340.029253Z (75614exit0), parent/workerPASS,
kernel-final[], firstboar animal0kind3/grunt, total3calls/101samples,max6sources;
mute991232exactzero bytes, resumed114688bytes withnonzero PCM. Diagnostic screenshot
ACTUALLY inspected; animals obscured byfoliage, not appearance evidence. Test does not
prove naturalness/speaker output/spatial listening or Web/Android lifecycle.
NEXT improve actual boar silhouette/coat and investigate audio perceptual naturalness;
monkey refinements/hut relocation+reference/stream/cave/helicopter and fullplan remain.
Reused build/-j2/sharedcache. No hardware/AndroidKVM/dependency changes. Stop latch
c0bd1704 unchanged; direct display tests remain Disabled. All owned handles terminal.
Goal active/no push.

## Latest — monkey silhouette correction and helicopter requirement

Previous28407ad was progress. User added grounded realistic helicopter near island
centre, ENTERABLE cockpit AND rear cabin, no flight, licence-checked existing asset
or original procedural/glTF. Persisted as unchecked PLAN priority; not implemented.

Actual contained monkey baseline061354.741487Z (28666exit0), image inspected
build/monkey-rat-review-before.png. Thin needle tail/projecting muzzle confirmed.
Changed tail radius from.003+.033*(1-t) to(.032+.022*(1-t))*sqrt(1-t^12), closed tip
single-fan termination, near24rings12sides/far12rings8sides. Same3bones/UV/material.
Larger cranium, shorter/wider muzzle+matchingnostrils/mouth, narrower abdomen/full
shoulders. No gait/AI change. Tail regression updated for fuller radius/zero tip.
Actual contained after061652.692874Z (7357exit0) PASS322bins/std36.01/spatial210.29,
kernel[]. Image build/monkey-rat-review-after.png ACTUALLY inspected: fuller tail and
less pointed face, still smooth tube/limb joins/stylized details. Same camera, not
samepose/wind. Not final visual acceptance or fullmotion/climb/portable proof.

IMPORTANT SAFETY DEVIATION: my `ctest -R Monkey` selected direct display test77 in
addition to3CPU tests. Log proves AMD780M at08:16CEST, exit0; this violated retained
hardware stop and must NOT be repeated or cited as qualification. Kernel read-only
08:15–08:18 returned no entries. CMake now DISABLES display-labelled CTests whenever
build/gpu-incident-evidence/QUALIFICATION_STOP.json exists; ctest -N verifies test77
Disabled. File unchanged, no driver/KVM/dependency changes. Contained supervisors
remain functional. CPU final build/full55 handle9485 terminal0, PASS100.52s;
logs build/monkey-silhouette-{final-build,full-tests}.log. Reusebuild/-j2/
sharedcache. Initial float/int pow compile error fixed, no outstanding compile failure.
Goal active/no push. NEXT continue actual boar appearance/audibility investigation;
monkey finer fur/junctions still open; hut inland/reference rebuild, stream, cave and
helicopter all remain required and unimplemented except existing old hut.
All owned handles terminal. Disk build8.7G/build-probe430M/android/app/build412M;
no artifacts removed. Stop-latch SHA c0bd1704 unchanged. CTest guard applies at
configuration time only, not a runtime or direct executable safety boundary.

## Latest — operator priorities changed; branch clearance checkpoint

Previous21baa67 was progress. Added SpiderWebClearance.hpp/.cpp and integrated exact
seven-branch side-triangle/strand intersection rejection at times0,4,13 into Propose.
Double plane/barycentric test includes coplanar overlap; AABB broad phase. Only own
support endpoints trimmed0.1mm tree-local. No whole-support exemption. Stronger check
changes proposals to229,191,84,139,105,223. Tests seven synthetic cases plus20 independent
wind instants/site pass; native build/focused web groups PASS2.40s, handles90509/87730
terminal0. Logs build/web-clearance-final-{build,tests}.log. No new full55 suite, GPU,
Web/Android or actual web rendering; leaf/trunk/vine/nearby-plant/end-volume clearance
still open. Reused build/, shared ccache/-j2; no dependency or hardware/Android changes.

NEW USER MESSAGES reprioritize work: pigs look poor and no grunts heard; monkey looks
like a rat with a rat tail; hut must move farther inland and resemble supplied photo;
add stream/river; add cave with terrain adaptation. All persisted in PLAN top section.
Reference downloaded to build/hut-user-reference.jpg and ACTUALLY inspected: timber
house on supports, veranda/railing, plank walls, windows/door, broad pitched roof,
exposed framing/braces, jungle backdrop. Not a game texture/license grant.
NEXT prioritize these requests over further web integration. Inspect actual boar/monkey
views before changes. Sound sources/scheduler exist but audible naturalness NOT proven:
BoarCallVolume breath9m/base.14, snuffle23m/.29, grunt42m/.40 with squared fade;
rest emits only breath. Shared wildlife slot gives due bird/monkey calls first refusal,
also blocked by wave/water voices. Global sound distance scale24. No cause confirmed
for user's silence; do not assert missing playback or a fix from source alone.
MonkeyTailProfile radius currently .003+.033*(1-t); AddTail uses existing fur UVs.
Hut Find currently576candidates only18..38m inland; new farther-inland constraint not
implemented. Stream/cave wholly unimplemented. All remain open, goal active/no push.

## Latest — connected web strand graph, still CPU-only

Previous9f6822e was progress. SpiderWebGeometry now constructs a triangle-supported
24-spoke/eight-turn spiral graph: default220nodes/434strands per proposed site.
Frame edges split at radial attachments; radial/spiral intersections share nodes;
corner hits reuse support vertices. Rest spiral is circular in the actual triangle
plane, increasing radius0.12..0.88 of bounded radial clearance. Stored barycentric
nodes follow moving anchors affinely. Taut scaffold, not silk physics or final art.
Tests: graph connectedness/unique/nonzero edges, exact supports, convex containment,
finite independent-double interpolation, rest spiral radii, six sites*40wind samples,
24extra oriented/asymmetric frames and invalid rest/moving frame rejection.
Initial focused two web groups PASS0.82s; all six sites220nodes/434strands confirmed.
Build initially failed read-only shared ccache, then approved same incremental build
with required cache access succeeded (63913exit0). No cache bypass/duplicate build.
Final build/full CPU handle97949 terminal exit0: all55 groups PASS110.25s, geometry
test0.56s, no build warnings. Logs build/web-geometry-{final-build,full-tests}.log.
Reused build/, -j2/shared ccache. No graphics launched or new live web/report flags.
NEXT vegetation clearance, strand GPU geometry/material/visibility and live occupants,
then actual close/moving visual audits. Do not call this rendered/accepted wildlife.
Goal active; full remaining plan including AMD root cause still open. No hardware,
Android/KVM/AVD or dependency edits, no push.
Stop latch c0bd1704 unchanged. Disk build8.7G/build-probe430M/android/app/build412M;
no artifacts removed. All owned handles terminal.

## Latest — bounded world-space web site proposals, not live webs

Previous turn answered an ETA question only (no implementation progress). Resumed
owned build/test handle53872 to authoritative exit0: all54 CPU groups PASS89.68s.
Current SpiderWebLayout.hpp/.cpp and test/CMake changes complete the previously
unfinished proposal step. At most six sites, deterministic hash-ranked tree order
instead of sequential southern concentration; default trees1,229,138,23,184,189.
Anchors store three distinct actual branch triangles/barycentric coordinates and
sample shared wind, rendered-terrain grounding, instance scale/yaw/translation.
Bounds: area0.08–1.5m², edges0.3–2.5m, centre>=2m above terrain, centres>=12m apart.
Three wind-time interior grids exclude trunk/coarse crown cores only. NOT individual
leaf/other-branch/nearby-plant or continuous-wind clearance. No forced count/fallback
and no vegetation removal. Twenty-time world-transform/area/span checks, determinism,
spacing/height/motion and four invalid cases pass; focused0.36s/full54CPU89.68s.
Logs build/web-layout-{ranked-tests,final-build,full-tests}.log. Build reused build/
with shared ccache and -j2. No renderer launched: proposals NOT integrated in game
or report, websImplemented=false remains correct. No new web population or visuals.
NEXT bounded strand geometry/stronger clearance, then actual live rendering and
occupants plus close/moving visual inspection. Full plan remains open, goal active.
No AMD hardware, Android/KVM/AVD or dependency changes; latch SHA c0bd1704 unchanged.
Disk check: build8.7G/build-probe430M/android/app/build412M; no artifacts removed.
No push. The earlier 250–450-hour estimate is superseded by the operator's fixed
approximately 100-hour remaining budget and reduced visual-fidelity target above.

## Latest — actual branch geometry shared for future web attachment

Previousacd23c1nearlegs wasprogress. Added CanopyBranchProfile.hpp/.cpp, consumed by
JungleRenderer inbothcanopytiers:7branches/form,three6-sidedrings/18vertices/24outward
triangles, samebend/taper/woodcoords/windweights/phase. Renderer-privatecopyremoved;
sharedindicesalreadyoutward so FinishWoodNormals no longerreversesthem. No newGPU
geometry/draws/webpopulation. This is attachmentfoundation, NOTlivewebimplementation.
SampleCanopyBranchAnchor usestriangleid+barycentricu/v, deforms3actualvertices with
matchinglocalwind beforeinterpolation. Stilltree-local; futureworldsite/transform,
windtime/clearance/strands/occupancy/visualproof required. WorldReportwebs=false remains.
BranchAnchorTests all8forms*7branches*24triangles*5times=6720anchors versusindependent
doublewindreference, outwardtriangles/restvertex and7badinputs. Build62885terminal0,
focusedbranch/light/visibilityPASS3.17s, no warnings. Actualnativejungle054755.576838Z
22938terminal0 PASS356bins/luminancedeviation47.10/spatial214.05/kernel[]. Actualimage
inspected build/web-branch-anchor-jungle.png; supports/foliagepresent,noobviousgross
break. Overviewdoesnotproveexactbeforeafter,webattachmentormaturejunglevisualquality.
FullCPU53groups PASS163.64s (18068terminal0), build/web-branch-anchor-full-tests.log.
NoWeb/Androidbuild/runtime, noAMDhardware/KVM/AVD/dependencychanges;latchc0bd1704unchanged.
Goalactive/no push. NEXT world-spaceboundedwebsitesusingtheseactualtriangles, then
wind-stablewebgeometry/liveoccupants/renderaudit; fullremainingplan/AMDrootcause open.

## Latest — shaped near spider limbs within existing collision/buffer limits

Previous0487722habitatcomparison wasprogress. Nearleg/palpTube now8sides+midring,
radiuslineartaper*(.72+.28sin(pi*t)), analyticprofilenormals. Exactendpoints/lengths/
anchors/zeroradiusfoot retained; no collision/colonymotionchange. Near9840vertices
vs7920(+24.2%),far1632unchanged. Existing10000cap hasonly160headroom; GPU/CPUbuffer
capacities/texture/2draws unchanged. Notportableperformancequalification.
Testchecks eachposedlegvertex againstoriginalbonetaperunion plusoldUV/winding/bounds.
Finalnativebuild+mesh/colonyPASS9.59s (57527terminal0); initial84098terminal0 PASS8.95s.
Firstcapture053956.140126Z-bright73154terminal1, comparedsame-stagebeforeafter in
build/spider-limbs-comparison.png. Thenexactendpointarithmeticpreserved; finalactual
054117.864733Z-spiders-bright37493terminal1 all8imagesinspected in
build/spider-limbs-final-montage.png. Slightlyrounder/slimmerarticulations, stillsimple
separatebones/jointseams/foldedbodypattern.1/12visible/2draws/contact1,habitat+motion+
lightingpass/kernel[], outergenericcontrastFAILunchanged. No newfull52suite orportable
build/runtime; nohardware/AndroidKVM/dependencychanges; latchc0bd1704unchanged.
Logsbuild/spider-limbs-*.log. Goalactive/no push. NEXT requiredplant-attachedwebs or
bodypatternrefinement,fullremainingspecies/resident/environment/AMDrootcause remainopen.

## Latest — compared twelve habitats; actual fully-lit spider camera

Previous Androidaudit4de4859 wasprogress. AddedFixedViewpoint::SpiderBrightDetail,
CLI spiders-bright/help, UsesSpiderViewpoint and retainedsubjectindex. Selectsfirst
maxbodytransmission among12actualinitialspiders; cameraonly, noanimalrelocation,
material/lightchange. SameFOV/offset/normalretreat/follow/100msclock/eightstages.
Logs12candidates+selectedindex; independentharness verifiesorderedfinitecomplete
rows/tiefirstmax and saves spider-habitats.json.42harnessCPUchecks+8badcomparisoncases.
Nativebuild44689exit0; full52CPU29925exit0 PASS80.86s.
Actual053333.460326Z-spiders-bright35470exit1: index1 selected; transmission
[.00512093,1,.0309411,1,1,1,1,.408079,1,1,.0575958,.722916]. Sevenfullysunlitinmodel.
Body+groundtransmission/RGBtint1. All8actualimages inspected build/spider-bright-montage.png:
readablebody,stillangularlegs/foldedpattern. Motion/material/lightingpass/kernel[].
OutercontrastFAIL(deviation11.6962,bins48,spatial19.7302,mean53.2637), notacceptance.
Originalviewregression053432.291661Z-spiders46274exit1 all8+finalbyteidentical
to051626.813603Z/kernel[]; unchangedoutercontrastFAIL. Do not brightenwholeisland
solelyforclosecropthresholds. No thresholdexceptions/hardware/AndroidKVM/dependencychanges.
Latchc0bd1704unchanged; noWeb/Androidbuildforthiscamerachange. Logsbuild/spider-bright-*.
Goalactive/no push. NEXT actualvisualimprovementofangularlegs/foldedpattern orrequired
plant-attachedwebs, remainingfullplan/AMDrootcause stillopen. Allownedhandles terminal.

## Latest — actual Android spider image inspected, contrast gate failed

Previous turn Webaudit76fb141 wasprogress. AddedspidersAndroidviewpoint/actualproof
requirements;23CPUpreflightPASS0.731s including6badspiderlogs,shellsyntaxPASS.
x86assembleDebug+lintDebugPASS10s (90696terminal0). ExactAPKpreparedmaterialbytes
verified including141381byte spiderPNGhash06b52958 (sameasnative/Web).
Actual052659.737424Z-spiders (11685terminal1): mobileGLES3/SwiftShader,1/12visible/
2draws/contact1,material+lightingcorrect. Retainedlogindependentlypassesvalidator.
Intermediateandfinalactual1280x720PNGinspected: roundtexturedspider,darkscene,
angularlegs. OuterimageFAILED17attempts (deviation9.4266<18,bins28<64,contrast23.9878<30).
Parentfailedunchanged, kernel-final[],ownedemulatorexit0. No thresholdexceptions.
Oneimage is notmotion/lifecycle/mobileperformancequalification. Worker removed1stale
ownedAVDlock; no KVM/AVDconfiguration/AMDhardwarechanges.
TestAPK build/android-software/island-spider-setae-x86_64.apk SHA
8c8e006bbaebaba7cd9d8d161afab74ed55d772d20c77dc6d9ee697b95f83ec7 retained.
NormalARM64assemble+lintrestoredPASS7s (87921terminal0), ARMlib+spiderassetverified;
android/app/build/outputs/apk/debug/app-debug.apk SHA
6dda4d1761f9acde07d9c9d8a29e11d2a22ef5c517b53ee62a7a2bf8927f5d99.
Logs build/spider-android-{preflight,x86-build,arm64-build}.log. Latchc0bd1704unchanged.
Goalactive/no push. NEXT realhabitatlightingcomparison/visualrefinement orrequired
plant-attachedwebs; fullremainingspecies/resident/environment/AMDrootcausestillopen.

## Latest — actual WebGL2 spider material/pose audit

Previousturn progressed nearbody (77b520c). AddedspidersfocusedChrome scope/page,
siximages(initial+four8rAFposes+final),60intervals. Parent independentlychecks live
SwiftShader/gl0/context,finitepositiveintervals,material/population/shadows,canopy
transfer,uniquePNGlocalpathsandfiles.21BrowserSoftwareTests+41SoftwareCaptureTests
PASS, JS/shellsyntaxPASS. Nativegameunchanged thisturn.
WebGL2incrementalbuild21576exit0,package11803579bytes/gzip4677410. Imagehashmatches
native06b52958. Firstactual052041.937861Z-chrome-spiders35035exit1: outdatedtexture
budgetexpectations8496944/8517160. Correctedfocused/fullChrome+Firefox+Androidsmoke
to8846468/8866684, exactCPU-pinnedtotals; failedreportretained, no thresholdweakening.
NoFirefox/Androidruntime fromthosesourceedits.
Repeatactual052259.613764Z-chrome-spiders33829exit0/parentpassed/kernel-final[].
1/12visible/2draws/contact1,validmaterial+lighting,gl0/contextretained. Allsiximages
actuallyinspectedvia build/spider-web-montage.png; roundedtexturedbody/movinglegs,
stillangularlimbs/foldedbodypattern/darkscene. rAF60samplesmean510.258ms,
min133.4/max1016.6ms: softwareonly, NOT smoothmotion/targetGPUperformanceproof.
NativecontrastFAILunchanged. Logsbuild/spider-web-{build,package,preflight,native-preflight}.log.
NoAMDhardware/AndroidKVM/dependencychanges; latchc0bd1704unchanged. Goalactive/no push.
NEXT Androidspideraudit,realhabitatlightingcomparisons,plant-attachedwebs andfullremaining
plan; AMDflightrootcause stillopen. Allownedprocesshandles above terminal.

## Latest — rounder near spider bodies, unchanged far LOD

Previous turn measuredlighting (019c2b9), concrete progress. Near ellipsoids now24sides/
12rows versus12/8, giving7920/1632 near/far vertices. More actual-vertex-rootedbristles;
eyes still fittedtogeometry. Limbs/farLOD/texture/drawcount/capacities unchanged.
Near upload+meshwork +86.4%, not a measured portableperformancepass.
New geometricregression maxfacecentreradialerror.512275mm near/3.14511mmfar,
requiresnear<.7mm and<40%far, farcount1632. ExistingUV/bounds/windingchecks retained.
Build+mesh/colonyPASS8.90s (8725 terminal0); finalmeshregressionPASS0.06s.
Actualnative051626.813603Z-spiders (92828 terminal1): all8frames inspected via
build/spider-silhouette-montage.png; enlargedmatchingbeforeafter inspected via
build/spider-silhouette-comparison.png. Bodyoutline rounder, limbsstillangular,
foldedtexturepatternvisible, viewdark.1/12visible/2draws/contact1,motion+lightingchecks,
kernel[]; genericimagecontrastFAIL unchanged. No newfull52suite/portablebuild/runtime.
Logs build/spider-silhouette-*.log. No AMDhardware/AndroidKVM/dependencieschanged.
Goalactive/no push. NEXT lightinghabitatcomparisons, portablecostaudit, webhabitats,
remaining fullplan; do not mark visualrealism or AMDflightincident resolved.

## Latest — measured spider close-view lighting, no visual change or acceptance

Previous turn progressed via generatedmaterial+UVcorrection (6b7465e). This turn
investigated darkcapture and added actual first-frame ground/bodytransmission/tint
diagnostic plus independent harness transfer validation/save spider-lighting.json.
41CPUpreflight PASS0.038s including8badlightingrecords; nativeincrementalbuildPASS.
Actual contained run051411.829809Z-spiders (70387 terminal1) measures direct
transmission ground.00488045858/body.00512093166,RGBtint.462765306/.532406807/.621945977.
Near-totaldirectocclusion, ambientretained. Spider/goat light expressions match;
BasicEffectlightingdisabled/defaultdiffuseunity, no additional spideronlyfactorfound.
This is not proof of correct exposure/colourspace/commonlighting or habitatquality.
All8stageimages+final are byte-identical to051053.143139Z already visuallyinspected.
FinalPPM SHAe119fde805467132cb976d0da520c987915260a39fb12855315bc4e20f568899.
Kernel[]; outercontraststillFAIL. No gateexceptions/threshold/renderingchanges.
Logs build/spider-lighting-{preflight,build}.log; build49352 terminal0.
NEXT: compare other realhabitats and open-daylightcontrol before anysharedlightchange;
portableaudits, webs, remaining fullplan andAMDrootcause stillopen. No hardware,
Android/KVM/dependencychanges, latchSHAc0bd1704 unchanged. Goalactive/no push.

## Latest — spider setae material integrated and inspected, visual gate still fails

The preceding response merely confirmed the existing AMD task (no implementation
progress). This turn resumed the existing uncommitted material work and completed
native verification/visual iteration. Built-in ImageGen original1254square and exact
prompt are in assets/art-studies/spider-setae-albedo-v1* with manifest/provenance.
Prepared256image SHA06b5295893a201a4f83523bf5194c69089af7b723ad186eb4d22a59f6ace1f89.
Skin uses required opaque9mip texture349524bytes; eyes/bristles remain solid. Two
near draws, unchanged4248/1632geometry. VPCT120000capacity2880000 GPUbytes; two
CPU vectors each reserve thatpayload. RuntimeRGBA8total8846468; qualitycapsupdated.
CMake install/Webpreload/Androidpreparation included, not portable runtime evidence.

Old build handle62346 missing; incremental resume exit0/no work. Full52CPU PASS74.94s
(30507 exit0). Added pose-invariantUV regression; final curvedUV focusedtest PASS0.05s
andnativebuildPASS (29298 exit0). First actual050946.511021Z-spiders capture
showed top-projection stretching at rear. Latitude/foldedlongitude mapping corrected
this; final actual051053.143139Z-spiders all8crops inspected in
build/spider-setae-curved-montage.png. Finebodydetail/reducedrearstreaking, stilldark,
faceted and stiff-looking limbs. 1/12visible/2draws/contact1/motiondatapass/kernel[].
Both capture supervisors terminalexit1 (17149,80645): generic luminance/colour/spatial
contrast gate FAILED. No threshold/profile exceptions. Do not call these green or
portable qualification. docs/spiders.md has full evidence and limitations.

No hardware/Android/KVM/dependency changes; GPU latch SHA c0bd1704 unchanged.
Goal remains active, no push. NEXT: shaded-scene lighting/acceptance investigation,
portable spider audit and required plant-attached webs; all other remaining requested
animals/resident/environment work and AMD incident rootcause remain open.

## Latest — live spider grounding shadows, inspected before/after; material still open

The previous turn made progress by fixing measured foot-reach stalls. This turn adds
SpiderContactShadow.hpp/.cpp: one body ellipse (60/29mm radii, local z=-12mm, opacity
0.18) and 3.5mm/0.30 foot ellipses for stance feet only. ContactPatchDetail::SmallAnimal
uses 1..200mm radius bounds and 0.3mm bias; old Compact/Coastal bounds/bias are unchanged.
Clipping follows actual terrain triangles. Optional RenderedTerrainSurface cache avoids
procedural corner resampling. This is ambient grounding, not directional leg silhouettes.

Integrated into the existing ContactShadowRenderer transparent batch, with no extra
draw call. Quality-scaled 8m range; 4096 vertices/spider reserve adds 786432 bytes each
to GPU and CPU staging capacities. Population/upload guards remain. Exact nearby
spider state/stance changes refresh the cache; distant changes do not force it.
HUD and mirrored actual-render proof expose spider shadow count. Native harness requires
positive body and grounding counts; 40 CPU preflight tests include seven bad proof cases.
Colony tests additionally check shadow vertices/triangle interiors against rendered
ground and removal of raised-foot footprints. Focused core+colony PASS39.89s; inclusive
build/full52CPU PASS88.30s. Logs build/spider-shadow-{build,focused-tests,tests}.log.
World fingerprint remains4c38d1adbddc9c9e.

Actual llvmpipe sequence:
build/software-captures/20261012T045511.802447Z-spiders,1/12visible,1bodydraw,
contact-shadows=1, motion-data pass, kernel-after[]. All8 images actually inspected in
build/spider-shadow-montage.png; matching stage0 raw crops from before/after inspected
in build/spider-shadow-comparison.png. Under-body darkening is subtle in deep shade.
Outer image-contrast/color gate still FAILED; no thresholds/profile changes or clean
visual/platform qualification. Smooth plain body/sparse hairs remain the main deficit.

Owned handles47732build0,13492focused0,14099capture1,34862full0 are terminal.
No Web/Android build/runtime, AMD hardware, KVM/emulator or dependency changes.
Latch SHA c0bd1704 unchanged. Goal active; no push. Full remaining requested species,
web habitats, resident, environment, goat refinements and AMD flight root-cause work
remain open. NEXT: tangible spider body/hair material detail, then Web/Android and
plant-attached webs. Do not just repeat the same unchanged dark-camera audit.

## Latest — spider reach-wait pauses fixed on measured routes, actual8stage post-fix inspected

Previous turn was progress (actualmotionfoundpauses). Extended SpiderColonyTests with
8s/.02 directretreat andclosecameraoffsetretreat audits. Eligible onlyafter10steps,
awayangle<1.03rad,home<.55m excludesintentionalturn/boundarystops. Baseline389eligible/
165paused/165nexttranslationIKlimited direct;298/117/113 turning. Full oldlog
build/spider-pause-baseline.log. Confirmed realreachwaits.

AdvanceFeet now scorespositivebackwardlag duringmovement pluslateralerror for turns;
retains newlyplantedforwardfeet instead of immediatelyliftingagain. Rest symmetric.
Lead.004→.012m reducedpausecounts66/56 (spider-pause-directed.log), then swing.12→.08s
producedzero/zero(spider-pause-fast.log). Max2swing/>=6supports,8mmlift,stanceanchors,
fixedbones/Validchecks/envelope/terrain tolerances unchanged. Added paused==0 regression
forboth reference routes,notallfutureterrain.8stravel .518799/.390786.
Quiet30stravel .643225–.984048m,111499fixedstancepairs/26020swings,
minsampledmeshclearance .0000991821m. Currentinclusivebuild/full52CPU PASS58.03s:
build/spider-pause-current-{build,tests}.log. Fingerprint4c38d1adbddc9c9e unchanged.

Actualpostfixllvmpipe:
build/software-captures/20261012T044909.500088Z-spiders, motiondatagatePASS,
1of12visible/6supports allstages, displacement .272804163m. Stage0/1intentionalturn
speed0;stage2..7 .064899.. .065013m/s. All8actualimages inspected in
build/spider-pause-fixed-montage.png. Bodyturn/crawl/footposechangesvisible, but
plainbody/sparsehair/missingcontactshadowsunchanged. OuterFAILEDgenericimagecontrast/
color gates, kernel-after[]. Noimagegate/profilechanges; sparseviewsnotcontinuous
naturalness/platformqualification. Priorfailedrunsretained.

Handles terminal:41612baseline0,44459build0,55464directed0,76333build0,
46215fast0,59247full0,9040capture1. Finalbuildterminal0. NoWeb/Androidbuild/run,
nohardware/KVM/emulator/dependencychanges. Latchc0bd1704 unchanged. Goalactive/nopush;
fullpriorother14species/webs/resident/environment/goat/AMDflightbacklogretained.

NEXT: tangible spider material/hair/contact refinement, then actualWeb/Android and
plant-attachedwebs(stillabsent). Don't keep reauditing unchanged dark material while
neglecting these actual visual deficits; capture-image acceptance stillneeds explicit
appropriate review without retroactivelylabeling failedruns green.

## Latest — actual8stage spider motion inspected; outer dark-image gate still FAILED

Previous turn was progress (WorldReport). Added UsesSpiderCaptureClock/counter:
capturepath+SpiderDetail only,100ms/update, matching render time, fixedaccumulationoff.
64frames, images8/16/...64=0.8..6.4s, realcolony/closeobserverretreat, translation-
followingcamera, noforcedposes/teleport. Logs9digit positions/headings/speed/eight
worldfootanchors+stance. validate_spider_motion_stages requires8orderedfinite rows,
exactclock, visible1..12, speed<=.069, sampletravel<=.055/turn<=.481,8distinctfeet
within.1401horizontal,>=6supports, bodytravel>=.02+foottravel>=.01. SavesmotionJSON,
requires8imagefiles. Notcontinuousfootlock/terrain/naturalnessproof.
SoftwareCaptureTests39PASS including14badspiderfixtures. Build/full52CPU PASS59.42s,
logs build/spider-motion-{build,tests}.log.

Actualisolatedllvmpipe build/software-captures/20261012T044137.963028Z-spiders:
motiongatePASS, all8actualimagesinspected in build/spider-motion-montage.png.
1/12visiblethroughout,6supportsatallstages, first→lastbody.169829786m,
maxstep.033670739m, headingchange-1.76553741rad. Turningandcrawlingvisible, plain
smoothbody/sparsebristles/missingcontactshadowstillweak. Sampledspeed0atstages0/1
(initialturn),4/6/7;bodymovesbetweenstages. NEXTmeasurefinerbodypauses/footstep
scheduling ratherthanassumegaitnatural. Max2swing/120ms/shorttargets maylimitstride
throughput; causalfixnotprovenyet. Keepworldstanceanchors/clearancechecksstrict.

OuterFAILEDluminancevariation/colorbins/spatialcontrast, kernel-after[]. No image
gate/profilechanges, no cleanplatform/visualacceptanceclaim. All ownedhandles terminal:
15268build0,76458capture1(retainedfailure),18340full0. NoWeb/Androidbuild/run, AMD
latchc0bd1704 unchanged/nohardware/KVM/emulator/dependencychanges. Goalactive/nopush.
Fullscope/AMDflightrootcause retained. Currentworldfingerprint4c38d1adbddc9c9e unchanged.

NEXT: tangiblematerial/hair/contactrefinement plus pauseaudit/fix; review appropriate
shaded-small-animalimageacceptance explicitly withoutretroactivelymarkingfailedruns
green. ThenWeb/Android andplant-attachedwebhabitats(stillabsent). Other14species,
resident/environment/goatetcstayopen. Do not repeatonlysameunchangeddarkcamera.

## Latest — live spider WorldReport coverage, visual/motion work remains open

Previous turn was progress (12groundspiders live+actualfailedcontrastcapture).
WorldReport now constructs same SpiderColony afterfullgrid/hut. wildlife.groundSpiders=12;
groundSpiderPopulation initial snapshot contains positions/homes/canopyindices,
heading/speed/behavior enum,3basisvectors,8worldfootanchors/stance/reachflags and
fourlocaljoints perleg. Seven decimalplaces onlyinnewspidersection. websImplemented=false.
Validity rechecks accepted habitats/homeposition, finite state, stance/reach/contact
and worldreconstruction. Fingerprint adds entireinitialspiderstate/home/basis/feet/
chains: reference4c38d1adbddc9c9e replaces0a98cb638125508f. NOliveplacement/model/motion
change. Actual report build/spider-world-report.json valid; all othercountpins retained.

Independent tests/WorldSpiderReportTests.py runsrealreport, checks12homes96feet,
spacing/canopyindices/orthonormalbasis/18,50,65mmbones/reconstructedanchors and11bad
cases(count,missing,nonfinite,overlap,falsewebclaim,missingfoot,anchor,bone,basis,stance,
canopy). FocusedPASS5.15s; inclusivebuild+52CPU PASS64.55s.
Logs build/spider-report-{build,tests}.log. Ownedhandles92785build/55244focused/
72856report/33965full allterminal0. No new GPU/display/Web/Androidbuild/run.
AMDlatchSHA c0bd1704 unchanged; nohardware/KVM/emulator/dependencychange.
Goalactive/nopush, fullremainingbacklog/highpriorityAMDflightrootcause retained.

NEXT: deterministicmulti-stageactualspidermotionview and actualimageinspection,
bodymaterial/hairs/contactshadowrefinement, carefuldarksmallanimalcaptureacceptance.
Prior043243.618050Z-spiders remainsFAILEDimagegate, notretroactivelyqualified.
ThenWeb/Android. Webhabitats/pavučiny stillABSENT/required; other14species/resident/
environment/goatrefinementetcstayopen. Do not stop afterreporting or callwholegoal done.

## Latest —12 ground spiders LIVE, first actual native view inspected but image gate FAILED

Previous turn was progress (CPUcolony). IslandGame now owns SpiderColony after full
worldsolidgrid/hut, updates real observer, renders authoritative pose via new
SpiderRenderer.hpp/.cpp. One opaque VertexPositionColor BasicEffect batch, CPU normals/
sun/sky plus shared canopy transmission, no customshader/texture; near<4m/far<=18m,
frustumsphere.16m atground+.04. Capacity120000vertices×16=1920000GPUbytes plus equal
CPUreservedpayload; population/uploadguards. No spider contactshadows yet.
New --viewpoint spiders/FOV.5, camera firstspider+(.36,.28,.45), target+.035,
translationfollowsnormal simulation; closeobserver triggers ordinary retreat.
Nativeharness accepts spiders and requires mirroredactualrenderproof1..12of12/1draw.
HUD+benchmarkvisiblecounts anddrawsum added; Mainhelpupdated. WorldReport coverage
NOTyetadded: oldfingerprint0a98cb638125508f is incomplete for newlivepopulation.

Actual contained llvmpipe capture:
build/software-captures/20261012T043243.618050Z-spiders,1/12visible1draw/kernel-after[].
Actual1280x720PNG inspected (build/spider-live-first.png): recognizable eightlegs,
rounded2body/eyes onforestfloor, plain/smoothsurface+sparsebristles, missingcontact
shadows, deepcanopyshade. OuterFAILED genericluminance/colorbins/spatialcontrast
gates. Noimage thresholds/profiles changed to forcepass. NOTqualifiedruntime/
realism/motionacceptance. Usesordinary4framecapture, nodeterministicmotionstages.
CapturepredatesHUD/help/benchmark-onlyedits; renderssameworld/model/motion.

Currentinclusivebuild+51CPU PASS59.58s:
build/spider-live-current-{build,tests}.log. Firstbuildspider-live-build.log.
Allhandles terminal:51415build0,17575capture1(retainedfailure),80399build0,
57143tests0. NoWeb/Androidbuild/runtime. AMDlatchSHA c0bd1704 unchanged; noAMD
hardware/KVM/emulator/dependencychanges. Goalactive/nopush/fullpriorbacklogretained,
includinghighpriorityunresolvedAMDflightcrash.

NEXT: WorldReportlivepopulationcoverage; deterministicmulti-stageactualspidermotion
views; bodymaterial/hairs/contactshadowrefinement and explicitreviewofdarksmall-animal
captureacceptance withoutcallingfailedgatesgreen. ThenactualWeb/Android. Plant-attached
webs are stillABSENT/required. Other14species/resident/environment/goatetcstayopen.

## Latest — grounded spider colony simulation, NOT yet live in IslandGame

Previous turn was progress (mesh+inspected CPU preview). Added SpiderColony.hpp/.cpp
and tests.12CPUagents use accepted SpiderHabitat homes/fullworld+hut obstacle set;
reserved.60m patrol+.14m envelope. Authoritative SpiderGroundPose carries rendered
triangle basis/local8legIK/worldfeet/stance flags with ToWorld/ToLocal. Static stance
anchors, atmost2swing(>=6supports),120ms smoothstep8mm sin² lift; largest foot error
>3.5mm triggers replant, moving target4mm ahead/rest neutral. Ground contact+.1mm.
Valid samples5points/legsegment with taper radii, refuses reachlimit/envelope/ground
penetration. Not continuous whole-mesh/self-contact proof. State rest/crawl/retreat,
deterministic intents; .035/.065m/s horizontal speeds,.6rad/s turn; 3Dobserver<2.5m
retreat waits for headingwithin60degaway. At patrolboundary translationstops but
turnretained; no inventedcolliders/teleport. dtcap.1/substeps.02. No GPU changes.

Focused6.05s PASS: two equal colonies30s/20ms, per-agenttravel.625706–.940732m,
110443consecutive stancepairs exactanchorzerochange,28773swings, >=6supports, actual
reconstructed endpoints,1s detailedmesh samples minclearance.0000991821m; another
10s fixedobserver test nondecreasingdistance+actualretreat; high/invalidobserver and
invaliddt checks. Current inclusivebuild+51CPU PASS55.93s, logs
build/spider-colony-current-{build,tests}.log. Older initial stats in tool output are
pre-rest/boundary/retreat refinements, not current. All handles terminal0:88751,39272,
41237,85714,15246,90805. No actual display/browser/Androidrun or portablebuild.
Latchc0bd1704 unchanged/no hardware/KVM/emulator/dependency changes. Goalactive/nopush.

NEXT: actually wire SpiderColony and SpiderMesh into IslandGame/renderer, visibility
budget/counts/worldreport, close camera and contained runtime inspection. Spiders
remain ABSENT in live world until that integration; webs remainabsent andrequired.
Body acceleration/supportpolygon/normaljumps/selfcontact/longerroutes remainopen;
CPU anchors do not prove natural-lookinggait. All prior species/resident/environment/
goat work and high-priority unresolved AMD flight crash retained.

## Latest — first spider mesh plus inspected CPU shape preview, NOT live spiders

Previous turn was progress (eight-leg kinematics). Added SpiderMesh.hpp/.cpp and
SpiderMeshTests: two smooth-normal ellipsoids, eight tapered3segment legs driven by
SpiderPose, point-contact feet, two2segment palps. Near12sides/8rows bodies+6sidedlimbs;
far8sides/6rows+4sidedlimbs. Near8eyeellipsoids with centre-ray fit to actual front-body
triangles and0.6mm embedding, plus2.4mm vertex-rooted body bristles0.18mmrootradius.
Cuticle/marking/eye/hair tags, no texture/shader yet. Near/far4248/1632 vertices under
10000hardcap. Tests2LODs×9raised/longitudinal foot sets, finite/unit normals, winding,
nondegenerate triangles, flatground clearance,140mm horizontal envelope, parts/LOD and
invalid pose. Not roughground/selfintersection/foot anchoring proof.

Initial inclusive build+50CPU PASS69.78s beforeeye-fit; current eye-fit inclusivebuild
and50CPU PASS61.84s (build/spider-mesh-current-{build,tests}.log). Then added CPUonly
island-spider-mesh-preview tools/SpiderMeshPreview.cpp, inclusivebuildPASS plus3spider
groupsPASS1.10s. build/spider-mesh-preview.svg rasterized withMagick; actualPNG inspected:
recognizable spider from elevatedfront/side, but angular facets/sparsebristles remain.
Preview uses painter sorting/flatface light/democolors, NOTlivegame/realismacceptance.
No GPU/display/browser/Androidrun; no hardware/KVM/emulator/dependency changes.
Latch SHA c0bd1704 unchanged. All owned handles terminal0 (98637,11981,86083,16960,
88638,13403). Full remaining scope/AMD flight incident unresolved, goalactive/nopush.

NEXT: bounded ground locomotion retaining world-space feet, ground fitting and retreat,
then live IslandGame population/renderer/counts/worldreport and contained actual views.
No live spiders or webs yet; plant-attached webs remain required. Do not stop at mesh
tests or mistake offline preview for actual island visuals. Full prior backlog retained.

## Latest — eight-leg spider kinematics foundation, NOT live spiders

Previous goal turn was progress (12 deterministic ground homes); this turn adds
SpiderPose.hpp/.cpp and SpiderPoseTests. Two body ellipsoid dimensions, eight front-body
attachments, fixed18mm coxa/50mm upper/65mm lower chains; arbitrary3D foot targets,
upward knee bend with vertical fallback, explicit reachable-shell clamping/flags.
Tests eight neutral poses/mirrors/140mm joint-centre envelope,216 local landing targets,
32 coincident/vertical/far targets and6 invalid cases. No gait/speed-scaled stride:
future crawling must retain world stance anchors and use this solver for the resulting
local targets. No mesh, fine hairs, live animation/population/renderer, world-report
change or webs yet. Next: actual two-part mesh and eight tapered articulated limbs,
then bounded ground crawling with planted feet and live integration; plant-attached
webs remain an explicit separate requirement. Do not claim species/realism acceptance.

Inclusive native build and49/49 CPU tests PASS68.36s; logs
build/spider-pose-{build,tests}.log. Initial build hit sandbox read-only shared ccache;
narrow escalated build succeeded without changing cache configuration. Build62770 and
test26759 terminal0. No visual/browser/Android run or portable build this turn.
AMD latch SHA c0bd170489321c8d497eae0f1211e1553e30aef9ee02ac01774269300e6ee6f1
unchanged; no hardware/KVM/emulator/dependency changes. Goal active, no push, full
remaining species/resident/environment/goat refinement scope retained; AMD flight
crash root cause remains high-priority unresolved.

## Latest — ground-spider habitat foundation, NOT live spiders

Moved forward the explicitly requested missing spider species while retaining all
remaining goat issues. New SpiderHabitat.hpp/.cpp + SpiderHabitatTests in island_core:
12 ground homes,16candidates/canopy (2.7/4.2scale radii×8directions), hashed stable
order seed0x73706964,8m spacing. .60m patrol+.14m body disk excludes full solids/hut
plus groundplant.18scale/understory.25scale/bush.30scale root bases.9rendered-ground
samples requireheight>=2,shore>=30,normalY>=.95,heightdelta<=.25. Canopy association
is not shadow/humidity proof; sampled patch is not full route/leaf-volume proof.
No fallback/vegetation clearing/invisible collisions. FindGround returns nullopt if
no complete set. Tests determinism, independent solid/plant clearances, terrain height,
spacing, invalid coordinates/index, inserted local obstacle and fully blocked world.
Reference coordinates and full remaining ground+web requirements in docs/spiders.md.

Inclusive native build/full48CPU PASS97.63s (spider1.58s), logs
build/spider-habitat-current-{build,tests}.log; initial focused1.17s earlier. Native
build config regenerated normally for new core source/test; existing build/cache used.
No IslandGame population/renderer integration or WorldReport spider count: spiders and
webs are STILL ABSENT in the live game. No visual/portable acceptance, GPU/emulator
run, Android/KVM/dependency changes. Corrected stale goat PLAN header saying untextured;
current coat/eyes/ears/planted feet are already live, other goat issues stay open.
Handles86383 and79236 terminal0. Goalactive/nopush; full prior backlog retained,
AMD flight crash remains high-priority unresolved. NEXT: spider articulated anatomy/
ground locomotion and live renderer, plus genuine plant-attached web habitats; do not
mark species done based on placement tests. Goat rotational feet/body smoothness and
all other requested species/environment/resident tasks remain open.

## Latest — post-anchor Web and Android actual runtime qualification

No code changes this turn; qualified29c23a0 live anchor code plus current coat/ears/eyes.
Web build/package11589087 raw/4512594 gzip PASS; browser preflight20PASS/JS syntax pass.
Actual build/browser-software/20261012T035115.028629Z-chrome-goats outerPASS on
Chrome155.0.8059.39 SwiftShader60intervals/glError0/contextLostfalse/3of6goats2draws,
kernel-final[]. All6images actually inspected (initial/final +4pose montage
build/goat-anchors-web-montage.png). Walking/turning continue; rear hair bands and
angular legs persist. Sparse samples do not establish continuous naturalness.

Android x86 assembly/lint7s,23preflight tests pass. Actual
build/android-software/20261012T035719.719637Z-goats outerPASS OPENGLES3SwiftShader,
3of6goats2draws/kernel-final[]/emulator0,1280×720 screenshot actually inspected.
Current grazing pose/coat/ears/eyes/feet visible; one view, not Android motion/lifecycle.
Archive build/android-software/island-goat-anchors-x86_64.apk SHA
6b3f1133e0b0dfa1bda3580ded05a00c139d2391b1d7a8c39975aca161e0b64c.
DefaultARM64 assembly/lint restored9s; APK SHA
d1b4b25ca0b9ef3c2e6cee0a0dbbce2f2586d4d89b1bd5f7a1d1f3b5e98ef98f (ARM64 libs verified).
Logs build/goat-anchors-web-{build,package}.log and goat-anchors-android-{x86,arm64}-build.log.
All owned handles terminal0:83081Webbuild,35314browser,48184x86build,93251Android,
53108ARM64restore. No hardware/Android/KVM/dependency configuration changes; latch
SHA c0bd1704 unchanged. Goalactive/nopush, full prior scope/AMD crash unresolved.
Next: hoof rotational locking/body smoothness/self-contact and wider animal/environment
backlog; do not replace remaining requirements with these successful platform views.

## Latest — live planted-foot anchors integrated, sampled slide fixed

GoatHerd private Feet retain world sole position/start/target/progress/swinging.
AdvanceFeet uses180ms smoothstep +.09scale sin² arch; at most2swing/1new per20ms.
Four-beat phase triggers walking; neutral error>.08m or>.025m near-stationary triggers
replant. Landing forward bias.18*speed/max; fallback neutral if reach-limited. Candidate
body moves/turns use ContactPose with RetargetGoatLeg; unreachable progress not adopted.
GoatState.pose is authoritative (default equality includes pose); renderer, herd tests,
stance auditor all use it. Added ToLocal/MakeGoatBodyFrame to avoid redundant terrain-leg
solves; removed renderer's unused terrain ref/height cache. Mesh/assets/GPU budgets same.

Initial regression failed head-on avoidance: foot-reach wait wrongly triggered obstacle
rerouting. TryMove now sets waitingForStep flag; body pauses but route intent stays and
feet keep stepping. Smaller stationary-replant threshold enables turn support.0.1mm
ContactSeparation remains INSIDE unchanged0.15mm sole/1mm mesh tolerances. Intermediate
CPU failures retained build/goat-anchors-{tests,recovery-tests,contact-tests,probe-tests}.log.
Final inclusive build+47CPU PASS111.75s: goat-anchors-final-{build,tests}.log. New checks
stance motion<.1mm,>=2supports, all poses reachable, inverse transform, real mesh1.25m
clearance envelope.90s travel8.50997–27.7957m, observer7.57935m/head-on7.01376m,
min near-hoof clearance-.000240326m, initial fingerprint0a98cb638125508f unchanged.
goat-anchors-final-audit.jsonl quiet25563/observer23569 stance pairs,0over1mm, measured
step/drift0 at float precision; minimum30s travel2.87275/3.09094m. Auditor now rejects
immobilized herd. Not proof of all future routes or rotational hoof locking.

Actual contained native034322.249615Z-goats8-stage PASS/kernel[], full montage
build/goat-anchors-montage.png inspected: graze/turn/walk preserved, changed leg poses.
Still sparse images, not continuous naturalness acceptance. Body stop/acceleration,
hoof rotation about fixed centre, self-contact and extended routes remain open.
No new Web/Android build/runtime; latestAndroid predates 3Dsolver/liveanchors. All owned
handles terminal (31273full0,70858capture0; earlier targeted failures preserved).
Goalactive/nopush; no AMD hardware/emulator/KVM/dependency changes. Full prior scope,
other species/environment tasks and high-priority unresolved AMD flight crash retained.

## Latest — 3D fixed-length goat leg solver, anchor integration still pending

GoatPose.hpp/.cpp adds RetargetGoatLeg(reference,target,leg). Arbitrary lateral target,
fixed.39/.38m bones, reachable shell[upper-lower+.0001,upper+lower-.0001], clamps+flags
unreachable; preserves root/stance/soleNormal/reference reachLimited. Fore/hind bend
hint ±z projected perpendicular to root→hoof, fallback projected up for axial goals.
EvaluateGoatLeg delegates after existing ground-height clamp; old sagittal knee shape
preserved within1e-5. No persistent world anchors yet, so NOT a foot-sliding fix.
GoatHabitatTests180lateral targets, bone/metadata checks,16singular/far/axial cases,
128legacy knee samples, nonfinite target; existing all-pose/herd/mesh tests retained.

Inclusive native build+47CPU PASS64.07s, build/goat-retarget-{build,tests}.log.
build/goat-retarget-stance.jsonl equals prior goat-stance-current.jsonl byte-for-byte:
quiet/observer maxstep.122717/.132466m still unfixed. Worldfingerprint0a98cb638125508f
and sampledhoofclearance-.000736237m unchanged. Handle20634 terminal0. No actual
visual/browser/Android run or fresh portable build; latestAndroid predates solver.
Goalactive/nopush; latchc0bd1704 unchanged/no hardware/KVM/dependency changes.
NEXT: per-foot world anchor state, transform anchors into current body frame and use
RetargetGoatLeg, bounded swing/replant at turns/stops. Share pose between simulation
validity and rendering. Keep stance audit strict, don't mistake solver foundation for
the requested natural gait. All prior species/environment/AMD-crash scope retained.

## Latest — CPU stance audit proves goat sliding; priority is a real foot-locking fix

New tools/GoatStanceAudit.cpp built as build/island-goat-stance-audit (native tools
only). Twenty-ms samples, real full world+hut obstacles, six goats,30s quiet and30s
fixed near observer. Track consecutive stance sole centres in world x/z; reset anchor
on swing. Calibration10mm slip/reset plus flat constant.65m/s cancellation checks.
Repeat build/goat-stance-current.jsonl and goat-stance-repeat.jsonl byte-identical.
Quiet30342stance pairs/5588>1mm; maxstep.122717m/maxanchordrift.948285m.
Retreat29133pairs/5609>1mm; maxstep.132466m/maxanchordrift.873171m.
Flat261pairs/0>1mm, maxstep1.19209e-7m/maxdrift7.15256e-7m.
Worst quiet15.16s goat3/leg2 speed.65→0 headingdelta0; retreat15.44s goat0/leg2
speed.698→0 headingdelta-.036. Source GoatHerd::Step hard-zeros speed on TryMove
failure, EvaluateGoatLeg stride scales by speed; blocked heading may rotate feet
while phase advances only with accepted translation. These explain genuine errors;
prior height tests/sparse images are NOT stance acceptance. Full evidence docs/goats.md.

NEXT: persistent per-foot world stance anchors, bounded release/swing/replant during
turns and stops, pose shared by simulation validity/rendering. Preserve fixed IK lengths,
surface normals/clearance, obstacle safety and determinism. Do not mask metrics or
relax limits. New auditor is diagnostic: exit0 means valid measurement, NOT good gait.
Current inclusive build +47CPU PASS71.12s; logs goat-stance-current-{build,tests}.log.
Native rebuild also includes prior Android one-time render-proof marker. No game motion
or mesh edits this turn; no actual GPU/browser/Android run. Handles97647 and7849 terminal0.
Goal active/no push; full prior species/environment scope retained. AMD crash still
high-priority unresolved; no hardware/emulator/KVM/dependency configuration changes.

## Latest — actual Android goat view passed after logcat proof repair

run_android_software.py accepts goats, requires exact coat/mips + actual1–6/6visible,
two draws; shell waits for goat render proof. Initial030243.275234Z-goats FAIL outer:
image rendered/inspected but ordinary stdout totals absent from logcat. Preserved
evidence/kernel[]/emulator0, not a qualified run. IslandGame now mirrors a one-time
actual count after first GoatDetail draw, no per-frame stream/hardcoded counts.
AndroidSoftwareTests23PASS incl5negative goat variants, shell syntax/diff pass.

Actual repeat build/android-software/20261012T030631.137902Z-goats outerPASS,
OPENGLES3 SwiftShader3/6visible2draw, actual1280×720 screenshot inspected, kernel-final[],
emulator0. Current height-map/ears/eyes/hooves render; coat bands/angular legs remain.
Single view only, no motion/lifecycle/physical-phone qualification. x86 APK archived
build/android-software/island-goat-eyes-x86_64.apk SHA
b4b4db56ae51573093187ddac86881a9e8d5a00a54114c40207d1cb6300bb88c.
x86 assembly/lint13s then proof rebuild3s; defaultARM64 assembly/lint restored7s.
Default APK SHA ebfb084604af1286500e7fc8e6c1ca209554c9cc789ad90ee00189def8681fb0.
Logs build/goat-android-{x86,proof,arm64}-build.log. No fresh native/Web rebuild for
the log-only marker; previous native47CPU source mechanics unchanged. Latest Web
runtime predates height-map/ear/eye changes. All handles terminal:41209/67203build0,
50101initialgate1,29333repeat0,83698ARM64restore0. No emulator/KVM/driver configuration
change; launcher cleared one stale owned lock after no-live-emulator guard. Hardware
latch c0bd1704 unchanged. Goal active/no push; full prior species/visual/stability
scope retained. AMD flight crash remains high-priority unresolved. Next: continuous
goat gait/stance audit, remaining anatomy/coat and broad missing species backlog.

## Latest — surface-fitted goat eyes, native visual comparison inspected

Builder::Eye replaces near black-dot loft:52×28mm dark oval border,44×22mm ochre
Iris,30×5mm horizontal pupil. Side-ray fit against actual transformed head triangles,
interpolated head normals, contour lifts1.5/3.5/4.5mm pluscentre5.5mm. No eye in far LOD.
Iris appended to GoatMaterial; renderer/CPU preview palettes now7, same bare draw.
Near/far7140/3996, six near42840,8192/animal49152/batch/layout/assets unchanged.
Tests near-eye/iris LOD and all eye triangle vertices+centroids at11graze amounts
against head,0.1–7mm clearance; not exhaustive continuous triangle-interior proof.

Inclusive build+full47CPU PASS66.81s, build/goat-eye-{build,tests}.log; mesh1.12s.
Build/tests completed BEFORE capture (avoids previous concurrent fixture load).
Native025612.436707Z-goats PASS8stages/kernel[], actual same-stage1 comparison
build/goat-eye-comparison.png and8-stage goat-eye-montage.png inspected: iris/rim
more readable than dot, pupil detail limited at this scale. Orbital/muzzle shape,
coat direction/joints/continuous stance remain open. Hoof clearance-0.000736237m and
fingerprint0a98cb638125508f unchanged. Handles77211 and17853 terminal0.
No new Web/Android build/runtime; latest Web predates body-height UV/ears/eyes.
No AMD hardware/Android/KVM/dependency changes; latch c0bd1704 unchanged. Goal active,
no push, full previous backlog and high-priority unresolved AMD flight crash retained.

## Latest — thin cupped goat ears with visible bare interiors

GoatMesh Builder::Ear replaces circular tubes: seven longitudinal rows,5/3 transverse
near/far samples, recessed front, furred back, closed12mm-separated rim; Skin occupies
actual front surface, not overlay. Existing head deformation/bob and two draw passes.
Near/far7068/3996 (+432/+240), six near42408,8192/animal49152/batch unchanged. No new
asset/shader/animation/layout allocation. Ear tests paired Skin, forward normals and
attachment/depth bounds; existing all-pose finite/winding/normal/terrain tests retained.

Native024656.529510Z-goats PASS8stages/kernel[]. Enlarged same-stage1 before/after
build/goat-ear-comparison.png and all8 stages in goat-ear-montage.png actually viewed:
thinner/readable front inset and furred back; still angular, eyes/joints/coat/gait open.
Initial full47 run failed supervisor CPU fixture startup marker after4s (not GPU fault),
retained build/goat-ear-tests.log. Capture was concurrent. Subsequent build explicitly
compiles newly added GoatMeshTests assertions (goat-ear-current-build.log), rerun without
capture full47PASS80.77s (goat-ear-current-tests.log); supervisor1.78s, limit unchanged.
No evidence proving exact startup-delay cause; do not call initial run green.
Current sampled hoof clearance-0.000736237m/fingerprint0a98cb638125508f unchanged.
All handles terminal:28125 failure8,28788capture0,22339currentbuild/tests0.
No new Web/Android build/runtime; latest Web precedes body-height UV and ear changes.
No hardware/Android/KVM/dependency changes; latch c0bd1704 unchanged. Goal active,
no push. Next: eye/head/joint/coat/stance detail, actual Android goat runtime; retain
full prior species/environment backlog and unresolved high-priority AMD flight crash.

## Latest — physical-height goat body coat, native rear comparison inspected

GoatMesh Builder::Vertex authors Body u=(z+.58)/1.12 and v=(y-.52)/.56 before bob,
including cap centres. GoatCoatUv uses body v directly; head sin/sweep cos unchanged.
This prevents full fur-window squeeze on shrinking rump rings. Geometry/animation/
source assets/draws/layout/memory unchanged. Actual body-vertex tests cover both LODs,
sampled poses, physical z/y and bob compensation; body no longer angular seam contract.
Inclusive native build +47/47 CPU PASS94.68s (build/goat-height-uv-{build,tests}.log).
Only subsequent source edit was comment clarification, no executable change.

Contained capture20261012T024124.340743Z-goats PASS8stages/kernel-after[]. Actual
build/goat-height-uv-comparison.png (same stage7 old left/new right) and full8-stage
goat-height-uv-montage.png inspected. Rear star pattern removed; replacement rear
hair too horizontal, so directional rear projection/finer coat still open. Head/ears/
eyes/joints/continuous stance remain unaccepted. No current Web/Android build/runtime;
previous023517 Chrome audit predates this UV change. Build73052/capture87827 terminal0.
Goal active/no push, no AMD hardware/Android/KVM/dependency changes. Full prior scope
and unresolved high-priority flight crash retained. Next: rear hair direction/anatomy
and actual Android goat runtime; do not treat this partial visual correction as done.

## Latest — actual Chrome software goat audit, six images inspected

Added supervised scope goats in run_browser_software.py/run_web_smoke.mjs and real
--viewpoint goats page mode. Requires coat marker,1–6/6 visible goats,two draws,
SwiftShader/live GL0, four distinct intermediate captures; outer report validator
rejects invalid population/material/context/images. BrowserSoftwareTests20PASS,
JS syntax and diff checks pass. Not deterministic native8-stage clock: real-time
eight-rAF gaps/four images then60intervals plus final image, not stance-locking proof.

Current Web rebuild/package PASS11559073 raw/4499299 gzip bytes. Actual
build/browser-software/20261012T023517.624374Z-chrome-goats outer PASS, Chrome155.0.8059.39
SwiftShader60intervals/glError0/contextLostfalse/kernel-final[]. Initial/final images
and all four pose images (build/goat-web-motion-montage.png) actually inspected.
Coat/smaller hooves render; goat turns/moves. Rear taper star-pattern fur remains
conspicuous, ears crude/eyes barely readable/legs angular. No realism acceptance.
Build handle9136 and browser97353 terminal0; earlier sandbox build67065 terminal1
due shared-cache write restriction, resolved by scoped escalation, not cache bypass.
No fresh native C++ change, Android build/runtime, AMD hardware or KVM changes.
Hardware latch SHA c0bd1704 unchanged. Goal active, no push. Next: coat taper/anatomy,
continuous stance and actual Android goat runtime; all prior species/stability scope
retained, AMD crash remains high-priority unresolved.

## Latest — goat limb volumes and lower hooves verified

GoatMesh now retains six longitudinal limb stations in both LODs, distinct front/rear
upper volume and tapered lower legs. Near/far6636/3756 (+384/+240), six near39816;
8192/animal and49152/batch guards unchanged. Hoof wall110→70mm, pair width114→96mm,
minimum toe gap8mm; flat sole, terrain-normal rotation, IK chain and gait unchanged.
Actual-geometry tests cover upper ring radii in both LODs and hoof height/width.
Current inclusive native build/linux-core47/47 PASS54.47s, logs
build/goat-leg-hoof-{build,tests}.log; LastTest.log minimum actual near-hoof terrain
clearance-0.000736237m (unchanged1mm tolerance), world fingerprint0a98cb638125508f.

Final native contained capture20261012T022306.275860Z-goats PASS8stages/kernel[].
build/goat-leg-hoof-comparison.png and goat-leg-hoof-motion-montage.png actually
inspected after recovering the previously truncated image result. Smaller hooves
improve the silhouette; upper-leg improvement modest. Angular joints, crude ears/eyes,
coat taper distortion and continuous stance quality remain open, not accepted realism.
No fresh Web/Android build/runtime, no AMD/Android/KVM/dependency changes. Stop latch
SHA c0bd170489321c8d497eae0f1211e1553e30aef9ee02ac01774269300e6ee6f1 unchanged.
All owned jobs terminal. Goal active, no push. Latest user crash request was confirmed
already high-priority OPEN in PLAN/docs/flight-amdgpu-incident-2026-10-11.md; previous
status-only turn was no implementation progress. Root cause/fix still unknown.
Next: actual portable goat runtime, anatomy/stance refinement, and full prior backlog.

## Latest — flank coat fold reduced; stale native-test preset repaired

GoatCoatUv now uses sin(angle) for body/head across coordinate, matching loft ring
height; old cos folded fur through visible flank centres. Sweep UVs/source/geometry/
draw counts/memory unchanged. Added cardinal-height and bilateral-height UV checks.
Native llvmpipe020419.131486Z-goats8stage PASS/kernel[]; actual montage and enlarged
same-stage comparison inspected (build/goat-flank-comparison.png, old left/new right).
Side flow improved, but rump taper/star pattern and coarse groups remain; anatomy open.

IMPORTANT verification correction: old linux-debug build preset omitted goat tests.
Source04:03CEST vs binary03:55CEST proved the first current-turn46-test green run used
a stale GoatMeshTests executable. It is NOT evidence of new flank tests. Older
preset-only passes likewise cannot alone prove their latest goat edits were compiled;
explicit goat-target builds/actual runtime captures remain separate valid evidence.
Fixed debug/sanitize build presets to configured all, CNA EXCLUDE_FROM_ALL retained.
linux-core test preset now excludes display only (old allowlist omitted many CPU tests).
New IslandPresetCoverageTests guards4narrow/unsafe mutations. Corrected build log
build/goat-flank-current-build.log explicitly compiles GoatMeshTests.cpp and preview;
actual ctest --preset linux-core47/47 PASS87.05s. Includes newest support-plane/flank
checks; hoof route minimum remains-0.000804901m. See docs/build-linux.md/docs/goats.md.
No sanitizer run, Web/Android build/runtime or emulator/KVM/dependency changes this
turn. All owned handles terminal; hardware latch c0bd1704 unchanged. Goal active,
no push. Next: goat anatomical detail/stance and actual portable runtime; full prior
species/environment/stability backlog remains open, including unresolved AMD crash.

## Latest — eight-stage native goat motion audit and per-hoof support rotation

UsesGoatCaptureClock only for nonempty capturePath + GoatDetail; fixed timestep disabled
there, exactly.1s/update, goatCaptureUpdates counter and matching vegetation/render time.
Capture64frames, images at8/16/...64 with position/speed/phase/grazing/updates/time.
Normal herd decisions/terrain/observers retained; no forced poses or teleport. Validator
requires8ordered finite/live samples, exact clock,<=.961m per.8s, phase0..Tau, walking
and grazing transition;12corrupt cases tested. Normal gameplay/snake QA unchanged.

Actual contained native runs015104.568755Z-goats pre-fix and015711.270483Z-goats post-fix
PASS/kernel[]. Both8-stage montages actually inspected; post stage3 enlarged leg detail
also viewed. First goat grazes/lifts head/turns/walks,2.44m net displacement, all stages
visible; sparse stills do not qualify smooth continuous foot locking. Logs/images under
build/software-captures/20261012T...; build/goat-contact-motion-montage.png and
build/goat-contact-stage3-detail.png. Final actual log revalidated with latest bounds.

New near-hoof actual vertex check along90s six-goat CPU route/1s samples found minimum
-0.00175476m (retained build/goat-hoof-audit.log; CPU test abort, NOT GPU crash).
GoatLegPose.soleNormal now rendered support normal in body frame; BuildGoatMesh rotates
hooves and normals about sole-centre pivot. IK lengths/pivot/gait/root placement unchanged.
Worst measured clearance now-0.000804901m, under UNCHANGED1mm test tolerance; not zero
penetration or all triangle-interior proof. Both LODs analytical tilted planes/unit
normals/invalid-normal tests added. Full native46 groups PASS106.71s; latest capture
preflight PASS0.19s. All owned handles terminal.
Visible bigger issues: angular uniform legs/blocky hooves, tiny eyes, obvious rump fur
swirl. Prioritize anatomy/mapping/stance transitions; don't call goats completed.
No Web/Android build/runtime for this latest change (prior62a2bd5 packages remain),
no emulator/KVM/dependency changes, hardware latch c0bd1704 unchanged. Goal active,
AMD flight crash and all prior species/visual backlog remain open; no push.

## Latest — generated goat coat live, native images inspected; portable builds only

Used imagegen skill/built-in tool; original1254square goat-fur-albedo-v1.png and exact
prompt preserved in assets/art-studies, manifest hashes/ASSET_LICENSES record added.
Original SHA90ad4035...,3,986,448bytes;256 prepared139,062bytes/9mips349,524RGBA8.
Coat-only textured stock BasicEffect pass, bare horns/eyes/nose/hooves second opaque
pass. One dynamic upload, VPColorTexture now24bytes×49,152capacity; two staging vectors.
No shader/uniform/geometry-count change. GoatCoatUv mirrored6–94% window avoids source
edge wrapping. First actual image revealed station-index stretching; loft UV now
physical z, sweep UV arc length, regression tests plus seam/direction/range guards.
Runtime texture total8,496,944/policy8,517,160 (both policies add exact349,524bytes).
Chrome/Firefox/Android expected diagnostics updated, APK gate requires goat PNG.

Native captures20261012T014053.532859Z-goats and014251.605524Z-goats PASS; actual1280×720
images inspected, kernel-after[]. Visible3/6,2draws, coat-load marker confirmed.
Visible hair improves plain coat, but neck streaks/swirl too coarse; tiny eyes/crude
ears/angular joints and live continuous gait/hoof-grounding remain unaccepted.
Native full46 CPU PASS63.57s (initial68.52s); prepared artwork tested pixel-for-pixel
against C++ area filter. Tools preflight2/2 PASS2.29s, JS/shell syntax pass.
Web build/package11,551,789raw/4,497,328gzip PASS; NO actual new goat browser runtime.
ARM64 Android assemble/lint PASS10s, default APK SHA
e2363bdbb5ceeb7063f63f4a75ff6bc40fdb10f50cf3f148489cfe2be40a791f;
goatPNG present, native/Android prepared SHA66ecf380... equal. No x86/Android runtime
this turn, no emulator/KVM/dependency changes. All owned jobs terminal. Hardware latch
unchanged c0bd1704. Goal active, no push. Next: continuous live goat gait/contact audit,
anatomy/mapping refinement and actual Web/Android qualification; all prior scope open.

## Latest — six goats LIVE in IslandGame, first native image inspected, NOT accepted realism

IslandGame now constructs GoatHerd after full obstacles/hut and updates all six normal
world agents. GoatRenderer uses fitted poses/CPU smooth daylight+canopy tint, stock
untextured BasicEffect/fog, frustum/range culling, near/far30m, quality-scaled120m range.
One draw,49,152 reserved position/colour vertices; current six near meshes37,512.
Zero added textures. Provisional beige material, not fur; profiling/throttling pending.
Existing shadow batch includes terrain-conforming goat body ellipses with capacity,
cache and totals; not hoof-specific/directional shadows. HUD/console/benchmark include
goats. --viewpoint goats follows first animal translation from initial8.5m offset.
WorldReport matches full herd construction, six initial positions/headings/scales and
new fingerprint0a98cb638125508f. Independent WorldGoatReportTests checks JSON and five
corrupt variants. Procedural provenance:26 records/89 sources/12 project files/0external.

Native contained llvmpipe build/software-captures/20261012T013223.609582Z-goats PASS,
3/6 visible one draw, kernel-after[]. Converted capture.ppm→capture.png and actually
inspected: central goat clear, another partly grass-occluded. Smooth body with readable
horns but plain beige coat, point eyes, crude ears/angular legs. This is first runtime
baseline, not accepted realism or continuous movement/hoof-contact proof.
Native full46 non-display groups PASS61.98s; final current-tree rebuild/full46 PASS70.70s
(build/goat-live-verified-{build,tests}.log). All owned jobs terminal. Hardware latch
SHA c0bd1704 unchanged; no AMD/Android/KVM/dependency changes. No new Web/Android build
or runtime qualification. Goal active, no push; AMD crash remains high-priority open.
Next: readable goat coat/anatomy and actual continuous gait/contact audit, performance
budget, Web/Android build/runtime; keep all other requested species/backlog open.
Historical foundation entries below saying no live goats are superseded by this entry.

## Latest — goat mesh CPU foundation and inspected shape preview, NOT live goats

GoatMesh.hpp/.cpp builds local triangles with normals/UV/material/anatomy tags:
near6252/far3516 vertices, budget8192. Body, deforming grazing neck/head, horns,
ears, beard, tail, fitted limbs and split hooves. Native CPU SVG preview tool added;
build/goat-mesh-preview.png actually inspected. Initial detached foreleg caps led
to chest extension/buried upper limbs and actual torso-triangle root regression.
Revised image shows connected forelegs but still coarse ears, tiny eyes and angular
limbs/hooves; no coat texture or runtime renderer. Do not call this visual acceptance.
Mesh tests both LODs×16 phases×11 grazing amounts: finite/unit/UV/winding/flat ground,
required anatomy, root coverage and sampled tilt envelope max1.17903m. BodyClearance
1.1→1.25m and analytic centre normalY>=.90 now align placement with sampled geometry;
fifth home changed (docs/goats.md), raw habitat/herd tests still agree. Current90s
travel17.36–32.09m, observer8.69m/head-on7.95m;13,824 poses/43,776 contacts pass.
Complete native build and45/45 non-display tests PASS59.51s. Logs:
build/goat-mesh-complete-{build,tests}.log. All owned jobs terminal. Hardware latch
SHA c0bd1704 unchanged; no GPU/emulator/KVM/dependency change. Goal remains active.
Next: renderer/live goat integration and readable coat/anatomy, then actual platform
audits including motion and whole-hoof/slope clearance. No live goat counts yet;
entire prior backlog and HIGH PRIORITY unresolved AMD flight crash remain open.

## Latest — deterministic goat herd CPU simulation, NOT live game integration

GoatHerd.hpp/.cpp in core, six agents from existing homes (one .75scale juvenile).
Graze/walk/rest and non-hostile retreat for finite observer<7m in3D; high/invalid
observers do not trigger retreat. Timed deterministic decisions, speed<=1.2, bounded
acceleration/turn1.8rad/s, update<=.1s/5substeps. GoatHabitat terrain predicate extracted
without changing placement; indexed solids+same canopy/understory/bush exclusion discs.
Steps check endpoint/midpoint ground, containing swept disc, other goats,14m individual
home/40m herd region and fitted-pose reach. Phase only advances with accepted3D travel.
Blocked steps stop/turn in place if pose valid;1.5s avoidance prevents immediate
observer steering override. No teleport/attack/unsafe fallback. Rendering absent.
New GoatHerdTests: duplicate90s deterministic run; all goats move4.75–48.47m, no overlaps,
raw placement oracle agrees; near observer clearance8.44m/head-on-obstacle7.74m; high/
invalid observer, dt clamp/no-op, turn bound and moving-route sole contacts checked.
Details/limits in docs/goats.md. Abrupt blocked gait transitions, full hoof/model
envelope/world-foot locking, distance throttling/performance and actual visual audits
remain open. Next: goat model/renderer + live integration; full prior backlog intact.
Native build/full44 CPU groups PASS73.92s (herd3.90s); WorldReport valid/fingerprint
b3fde347f77eb5f8 unchanged. All owned jobs terminal; no renderer/Android/KVM/dependency
changes, hardware stop latch SHA c0bd1704 unchanged. Goal active, no push.

## Latest — goat fixed-length walking/contact foundation, still NOT live

GoatPose.hpp/.cpp in core, used only by foundation tests. Four-beat68% stance,
1.2m/s walk reference, stride<=.25m/lift<=.12m; provisional upper/lower .39/.38m,
hoofheight .055/soleforward .025m. Front/rear bends differ, no bone stretch; unreachable
targets flagged. Smooth analytic body basis/rendered root height; each sole target
uses18-step body-up bisection against rendered triangles. Invalid inputs rejected.
GoatPhaseAdvance(distance,speed,scale) modulo cycle calibrates flat constant-speed
stance; NOT persistent world-space foot locking/turning/acceleration acceptance.
Foundation tests now13,824 poses at six homes/eight headings/32phases/three speeds/
three scales;43,776 stance centres within0.15mm, no reach limits. Continuity/wrap,
flat no-slip across scales, invalid input/unreachable ground tests added. Whole-hoof
footprint/fetlock anatomy/model envelope/live routes/visual gait remain unqualified.
No runtime population/renderer or count change. Next: goat model and live motion/
renderer integration, then actual platform audits; full prior backlog unchanged.
Native build/full43 CPU groups PASS71.25s. WorldReport valid/fingerprint b3fde347f77eb5f8
unchanged. All owned jobs terminal; no renderer/emulator/KVM/dependency changes,
hardware stop latch SHA c0bd1704 unchanged. Goal active, no push.

## Latest — goat open-slope placement foundation, NOT live goats

New GoatHabitat.hpp/.cpp in island_core; currently used only by new GoatHabitatTests.
3025 candidates, bounded deterministic ±2m jitter seed0x6f617431, prefers higher sites.
Six homes: altitude>=6m/interior>=35m, centre normalY .90.. .997, centre/eight footprint
samples normalY>=.90/height delta<=.55; proposed1.1m body disc; canopy9m*scale and
understory/bush envelopes excluded. Homes>=8m apart and<=40m from first. No unsafe
fallback, terrain/vegetation edits or invisible collisions. Six reference homes near
ridge x-64..-81,z46..81,y33..38.4m. Actual coordinates/criteria in docs/goats.md.
Tests use full palms/canopy/props/wreck/hut obstacles, check determinism and independent
distances, occupied home/blocked island/nonfinite/offshore rejection. Initial test
container-type compile error fixed with explicit spans; habitat criteria not relaxed.
Goats marked in-progress in PLAN, explicitly NOT in running game. No count/fingerprint
or live behavior change. Next: grounded anatomy/coat, locomotion/path validation,
runtime renderer/population and actual platform audits. Initial homes do not prove
route connectivity or final mesh clearance. Full existing backlog remains intact.
Native build/full43 non-display groups PASS70.22s; report valid/unchanged fingerprint
b3fde347f77eb5f8/live counts. All owned jobs terminal, no renderer/Android/KVM/dependency
changes, hardware stop latch unchanged, no push. Overall goal active.

## Latest — WorldReport now includes the live ground snakes

tools/WorldReport.cpp reconstructs SnakePopulation after complete obstacle grid/hut,
matching runtime. Exports wildlife.groundSnakes=4 and groundSnakePopulation with
initial homes/headings/all41 ordered body points, spacing .055, treeHabitatImplemented
false. Count/finite/ValidBody validation; fingerprint covers full initial state incl
phase/speed/behavior/tier. Host fingerprint b3fde347f77eb5f8 (was a590b7e4fb787269);
change is audit coverage, not live placement/behavior. Additive schemaVersion1 fields.
New independent tests/WorldSnakeReportTests.py parses real stdout and rejects five
corrupt variants. Registered CTest; full42/42 non-display PASS68.19s. Existing native
build reused, only report target built; no renderer/emulator/KVM/dependency changes.
Docs asset-pipeline.md/ground-snakes.md updated. Current monkey shader Web runtime
still pending; missing species/resident/visual/audio/AMD backlog intact. Goal active.
All owned jobs terminal; hardware stop latch SHA c0bd1704 unchanged, no push.

## Latest — monkey orbital relief and restrained socket ambient shading

MonkeyFaceProfile adds compact bilateral orbit/brow/nose/cheek relief to existing
face grid; perimeter/head envelope retained. Eye centre Z .645→.635 seats eyes in
new sockets. Reserved negative skin V encodes0.8..1 ambient retention; shader affects
ambient only, ordinary positive fur UV/direct sun/specular unchanged. No extra mesh/
draw/texture/attribute/uniform,14bones/92vectors unchanged. Full41/41 CPU PASS56.96s.
Native20261012T003001.503299Z-monkeys PASS/full image+enlarged comparison viewed/kernel[].
Comparison uses prior Web/current native and differing poses, not pixel equivalence.
Android003122.519072Z-monkeys actual SwiftShader PASS/image viewed/parent passed/kernel[].
Archive build/android-software/island-monkey-orbits-x86_64.apk SHA256
61502971ca76ad607c78ddf578bfc06534de5218042b1baf3c841456c45de34a.
x86_64/restored ARM64 build/lint PASS3s each, asset audit84sources/12project/0external.
Modest visible orbital shading, still simplified face. Eyelids/skin-fur boundary/ears/
limb transitions and continuous gait/climb remain OPEN. Latest Web runtime pending.
No hardware/KVM/dependency changes; AMD incident unresolved; overall goal active.
Web build/package PASS11,358,303raw/4,335,409gzip; all owned jobs terminal. Hardware
stop latch SHA c0bd1704 unchanged, no push.

## Latest — actual Web wildlife/foliage audit, current boar appearance verified

After4593dc8, existing serial Chrome SwiftShader scope wildlife-foliage completed:
build/browser-software/20261012T001634.994190Z-chrome-wildlife-foliage.
Chrome155.0.8059.39: jungle/boars/monkeys/east each60 intervals PASS, GL0/context live;
parent run.json passed, kernel-final[]. All six initial/post-simulation PNGs actually
viewed. This closes compile-only Web evidence for current boar body/ears/eyes/cheeks.
Not full realism: coat reads as broad streaks; boar walks off to left behind plants,
so fixed view cannot qualify continuous gait. Monkey stays tracked but has flat face,
point eyes/angular limbs. Jungle crowns/leaves repetitive; eastern steep slope material
blurry/smeared. Details and follow-up checklist in docs/wildlife-visual-audit-2026-10-12.md.
NEXT meaningful work: monkey face relief/lids, boar fine-coat readability/tracked gait
audit, eastern steep-slope material diagnosis. Missing species/resident/audio and AMD
incident remain OPEN. No game/build/dependency/Android/KVM changes this audit. All jobs
terminal; hardware stop latch unchanged; overall goal active, no push.

## Latest — fuller boar cheeks and fitted closed-mouth crease

Broader cheek/narrower raised nasal bridge in existing21-ring profile; nose anchor,
body envelope/limb roots/dip coefficients unchanged. Near crease21paired stations,
40triangles/side (+240vertices/animal), existing batch/material/no extra draw.
Initial6station strip had buried face interiors despite vertex tests/image PASS;
new sampled-face test caught it. Four subdivisions/span fixed it, test unchanged.
Failed build/boar-mouth-suite.log retained. Final41/41 CPU PASS61.02s.
Native001239.781186Z-boars PASS/full image viewed/kernel[]. Actual Android SwiftShader
001308.822363Z-boars PASS/image viewed/kernel-final[] also verifies previous fitted
eyes. Broader face readable, mouth subtle, feet still obscured; not full realism/gait.
Archived build/android-software/island-boar-face-x86_64.apk SHA256
58f81d0d116d21fb263f595ffbf7fee9d1cc81f6754277f97eded70d0a90885a.
x86_64 and restored ARM64 build/lint PASS3s each; asset audit84sources/12project/0external.
No hardware renderer/KVM config/dependency changes. AMD incident unresolved, stop
latch SHA c0bd1704 unchanged. NEXT coat readability/unobscured moving feet, actual Web
current boars and remaining wildlife/resident/audio backlog. Overall goal active.
Web build/package PASS11,356,327raw/4,334,892gzip; actual browser current boars pending.
All owned jobs terminal; local-only work, no push.

## Latest — surface-fitted boar eyes, modest visual improvement

BoarEyeGeometry.hpp replaces near floating ellipsoids with shallow cornea/8-segment
lid, exact16-sided head triangle intersections for31 tested rooting poses and both
bob extremes. Cached circular directions; outer/inner/centre X lifts1.5/4/9mm.
Exact mirrored sides, outward/nondegenerate triangles, independent triangle-centroid
and invalid-input tests. Existing BasicEffect/material/batch/guards; far/nose unchanged.
Full41/41 non-display tests PASS68.62s; native build PASS; ARM64 build/lint PASS5s.
Actual native000031 comparison and final20261012T000617.039228Z-boars PASS, full PNGs
and comparison actually viewed, kernel[]. Eye flatter, difference modest, lids still
poorly readable. Stylized head/coat and grass-obscured feet remain; NOT complete anatomy.
Android/Web actual eye runtime still pending. No hardware/KVM/dependency changes.
NEXT muzzle/cheek/lid readability, coat and moving-foot review; full species/resident/
audio/AMD incident backlog stays OPEN. Goal active; no push.
Web build/package PASS11,354,610raw/4,333,324gzip; asset audit PASS83sources/12project/
0external. All owned jobs terminal, hardware stop latch SHA c0bd1704 unchanged.

## Latest — cupped boar ears, actual native + Android audit PASS

New BoarEarGeometry.hpp gives8point curvedoutline, near32tri/ear rim+inner+closedback,
far8trianglefan sameoutline. Cached4side/LODrestmeshes witharea-weightedfrontnormals
acrossrim/cup, separateback; dip/bobtranslateposedcopy. Continuouscupcolour inexisting
SmoothVertex/BasicEffect, noextratexture/shader/draw. +174nearverts/+58tri or30farverts/
10tri peranimal; existing36kbatch/6kguard unchanged. Manifest addsprocedural source.
CPU bound/mirror/winding/nondegenerate/unitnormal/closededge tests. Initialroot1.5mm
inwardovershoot testFAIL retained; correctedgeometry,notthreshold. Full41/41PASS59.05s.
Native234702.122104Z-boars technicalPASS butflatcup contrasttoo harsh(imageviewed).
Smoothnormals/tone pass234925.212313Z-boars PASS/comparisonimageviewed/kernel[].
ActualAndroid235223.099135Z-boars PASS/imageviewed/GLES SwiftShader/kernel[]; qualifies
currentQAtelephoto/body/ears stillview, NOTnaturalgait/completeanatomy. SavedAPK
build/android-software/island-boar-ears-x86_64.apk SHA
cd1ba83ea64346b53eaccf3144217b4d0fe0ef4be413511e2cdfbfe74cf831ff.
x86buildlintPASS5s; defaultARM64restored/buildlintPASS2s. AssetauditPASS82sources/
12projectassets/0external. NoKVM/emulator/dependencychange. NEXT muzzle/eyes/coat,
unobscuredmovingfeet andactualWebcurrentboar; wildlife/resident/audio/AMDbacklogOPEN.
Webbuild/packagePASS11,350,607raw/4,331,592gzip; actualWebnotrun. Allownedjobs terminal,
hardwarestoplatchunchanged; no push. Overallgoalactive.

## Latest — readable boar inspection view and rounded near-body profile

InitialcloseQA233518 technicallyPASS butrejectedvisually: headblockedand<6mobserver
triggerednaturalretreat. FinalBoarDetail cameraheadingrelativeforward2.8/side-6.6,
heightmax(ground+.4,boar+2.4), FOV.50rad onlyQA. NormalFOV/AI/worldunchanged; observer
stillaffectsnaturalbehavior.233703baseline actuallyviewed: head/torso readable, grass
partlycoverslegs. No animal/plantmovedorhidden. Nextanatomyworkhasusablecomparison.
BoarBodyProfile now21rings from11anchors+10nonuniformHermitehalfstations, component
clampedtoadjacentanchors (noovershoot), preservedrootingdip/extrema. Renderernearuses
all21,+960vertices/+320tri peranimal; farusesoriginal11anchors. Batch36kunchanged,
addedperanimal6kguard. Fixedleg/contact/AI/collisionunchanged. Testsfinite/order/bounds
added alongsidefullpose/contacttests. Actual233914roundedcomparisonimageviewed;
modestshoulder/rumpsmoothing, notrealismcompletion. Finalafterfardecimation234145.503731Z
boarsPASS/imageactuallyviewed/kernel[]. Currentboarimage includesforegroundsnake.
Full41/41non-displayPASS63.03s; AndroidARM64build/lintPASS7s.
Webbuild/packagePASS11,343,447raw/4,332,469gzip; allownedjobs terminal/latchunchanged.
NoactualAndroid/Webnewcamera/profile yet. NEXT cuppedeargeometry, muzzle/eyes/coat andunobscuredmovingfeet;
monkey/fullspecies/resident/audio/AMDbacklogOPEN. NoKVM/dependencychanges/push.

## Latest — actual textured hut Web PASS; boar/monkey audit exposes remaining defects

No game/source changes thisturn. Actual Web232507.842729Z-chrome-hut parent/report
PASSboth60intervals/gl0/contextLostfalse/requirednewtexture+materialmarkers/7-6contacts,
kernel-final[]. Initial/finaloverview/detailimages actuallyviewed, finalmontageretained.
Currentroof portablematerial works; temporal/longrange/route input/realism stillopen.
Then serialnativecaptures boars233059.789662Z andmonkeys233132.293700Z PASStechnical
gates/imagesactuallyviewed/kernel[]. Boar anatomy/legs mostlyobscuredbyforeground
shrubs/leaf; visibleheadwedge/angular. Monkeyrecognizablefur/horizontalbody present,
butflatfacemask/tinyeyes/coarsejoints/boxytrunk/stifftail/grassobscuredfeet remain.
Full findings in docs/wildlife-visual-audit-2026-10-12.md; doNOTequatecapturePASSwith
visualquality ormotionacceptance. No newbuildsneeded; same5fc2a94artifacts reused.
NEXT returnto explicitwildliferealism: unobscuredcloseboarQAview withoutmovingworld
vegetation, thenhead/shoulder/muzzle/coat improvement. Monkeyface/tail andfullnew-species/
resident/audio/AMDincidentbacklog remainOPEN. Allownedjobs terminal/latchunchanged,
nohardware/KVM/dependencychange/push. Goalactive.

## Latest — generated palm fibre on hut roof; native/Android actual PASS

Used imagegen skill/built-in ImageGen for new opaque dry-palm microtexture. Original
assets/art-studies/hut-palm-fibre-v1.png and exactprompt sibling preserved inrepo,
hashes/provenance inmanifest/ASSET_LICENSES. Source2932608bytes; prepared256PNG110298
bytes viaexistinghostpipeline,9runtime mips349524RGBA8bytes. InteriorclampUVwindows
perleaf (noseamlessassumption), boundedmean-normalizedcolour/strawbase; timbersame.
Oneextrasampler/texture; no geometry/drawincrease. Missingtexturefatal; fallbacklogged.
CMakeprepare/install/Webpreload andAndroidGradle stagingdone; APKvalidationexpectsPNG.
Native/Android/Webhut gatesrequire newtexturemarker; negativefixturesupdated.
Budget auditfound EXISTINGmonkeyfur1,398,100bytes missing; addedwithhut349524. Runtime
now8147420/policy8167636 (notallnewallocation), diagnosticpins/CPUexpectationsupdated.
Native231908.276787Z-hut-material PASS/imageactuallyviewed/kernel[]: palefibrousroof
distinctfromwood, surfaceimprovedbutnotfinalrealism. ActualAndroid232044.759725Z-
hut-material PASS/imageviewed/GLES SwiftShader/texturemarker/nofallback/kernel[].
Archived build/android-software/island-hut-fibre-x86_64.apk SHA
e84dab206a5b57096b347faff5862fc9b119f6503a609281092705b937802d73.
x86buildlintPASS14s, defaultARM64restored/buildlintPASS11s. No KVM/emulatorchanges.
Full41/41non-displayPASS71.49s; Android23/browser19/native-supervisor37 testsPASS.
AssetauditPASS12projectfiles/0external; JS/shellsyntax/diffchecksPASS. Webbuild/package
PASS11,342,167raw/4,331,803gzip; noactualWebnewshadercheckyet. Allownedjobs terminal;
hardwarestoplatchSHA unchanged.
NEXT actualWebnewmaterial, temporal/longrange andvisualroute; wildlife/residentbacklog
andAMDincidentstillOPEN. Shader/texture additionisnotAMDfix. No dependencychange/push.

## Latest — hut world inventory fixed; continuous spawn-to-hut route verified

HutLayout::FindInWorld nowshares the actual solid+foliage exclusion between livegame
andWorldReport, identicalplace/pose/footings. Report previouslyomittedhut/collider;
nowreports hutcount/position/floor/yaw/radius/non-enterable/sixfootings andhashes them.
Validatesfinitegroundedsupports/matchingproxy/repeatfingerprint. Actualreportvalid,
fingerprint a590b7e4fb787269,790solids/417cells. Groundsnakesreport stillmissing/open.
New tests/HutReachability.hpp bounded2mcardinalA* suggestsroute; authoritative proof
is uninterrupted PlayerMotion replayfromnormalspawn(0,-225),1/60steps, nofly/teleport.
459.108m/6548steps/225waypoints reachesfrontapproach; walkingonly/noobstaclepenetration/
safetyclamp/depenetration/overspeed/nonfiniteposition. Perwaypoint240stepdeadline,
fullyblockedworldnegativefixture. No non-solidfoliagevisibility/platforminputclaim.
GlobaldryrouteCPU acceptance achieved; visualwalkstillopen. Sharedplacementtest
independently reconstructs foliageexclusions; eightlocalapproaches retained.
NativebuildPASS,41/41non-displayPASS69.89s; stricter finalroutefixture rebuilt/rerunPASS.
AndroidARM64build/lintPASS6s; Webbuild/packagePASS11,228,439raw/4,221,482gzip.
No new graphicalrun (refactor/report/tests only), noactualportable leaf-tip check yet.
NEXT visuals/materials/portablehutroute QA, groundsnakereport andfullspecies/resident
backlog. AMDincidentstillHIGHPRIORITYOPEN; nofixclaim. Allownedjobs terminal, hardware
stoplatchunchanged, noKVM/emulator/dependencychange/push. Goal remainsactive.

## Latest — hut tapered/drooping leaf tips, native detail audited

HutThatchBlade nowfourstations instead ofthree: broad body through82%, narrow
deterministically varied tips, eavedroop, strongerfold/reducedcourselift. Renderer
threequadsegments instead oftwo; now9492triangles/28476vertices/1draw, +2688triangles
and8064vertices, stillbelow30kguard. No newshader/texture. CPU bounds/determinism/
overlap retained; tipwidth<4.1cm; combined projectedcoverage at1cm slopeintervals
onbothsides checksnointernalholes overx0..2.55,z±2.20. Outerfringe intentionallyragged.
FocusedCPU PASS0.65s; full41/41non-display PASS72.75s. NativebuildPASS.
Webbuild/packagePASS11,228,998raw/4,221,248gzip. Allownedjobs terminal; hardware
stoplatchSHA unchanged. No actual Web runtime thispass.
Actual `20261011T230406.778103Z-hut-material` PASS/imageactuallyviewed/kernel-after[]:
frayedeave betterthan squarestripends, surface STILLtoo regular/wood-like, notdone.
AndroidARM64build/lintPASS3s; no portable actualruntime ofnewtips yet. Previous Web
andAndroidscreenshots predategeometrychange; do not treat ascurrentacceptance.
NEXT naturalroofmaterial/remainingrealism, globalroute/worldreport. AMDincidentand
fullwildlife/residentbacklog stillOPEN; nohardware/KVM/dependencychange orpush.

## Latest — actual Web hut overview/detail audit PASS; realism still open

Added Chrome software `--scope hut` with two sequential views, retained overlay/image
checks, before/after captures and60 measured intervals each. Material marker/no
fallback/one draw/zeroGLerror/livecontext required. Exact visible shadows7overview,
6close (frustum-culled count, matching earlier native close evidence).
Actual `build/browser-software/20261011T225804.399610Z-chrome-hut` parent/report PASS,
both60intervals/gl0/contextLostfalse, kernel-final[]. Both final overview/detail
images actually viewed. Wood grain/contact shading present, but roof still reads as
wooden slats, not realistic palm-leaf thatch; this is NOT complete hut realism.
Earlier failed224903 missingoverlayargument and225033 wrong7patchcloseexpectation
retained and explained in docs/island-hut.md; no failed evidence relabeled.
Browser19/software37 CPU tests PASS; JS syntax/diff checks PASS. No game code changed;
existing Web payload reused, build/package no work. No fresh Android/native run.
NEXT natural thatch/wood realism, global route and world-report integration; temporal
aliasing remains unqualified. User flight AMD incident still HIGH PRIORITY OPEN;
software audit is no evidence of its fix. Full wildlife/resident backlog staysopen.
All owned jobs terminal; hardware stop latch SHA unchanged, no KVM/emulator config,
dependencies or push. Persistent overall goal remains active.

## Latest — hut grain/fibre shader, actual native + Android close views PASS

HutMaterial.hpp portable330/ES300 shader: longitudinal beam-aligned wood coordinates,
transverse endgrain rings, slope-aligned thatch fibres, bounded weather/desaturation,
basal darkening, derivative/distance filtering. Geometry unchanged6804tri/20412verts;
VertexPositionColorTexture adds2floats/vertex (163296bytes), no texture/extra draw.
FiniteUVguard; logged BasicEffect fallback on unsupported/failed customshader.
New HutMaterialDetail/CLI hut-material/native supervisor; nearQA camera preserves
normal hut view. Android accepts hut/hut-material and requiresmaterialmarker (CPU
positive/negative checks). Android23/software37 testsPASS.
Native223611.972091Z-hut and223822.323931Z-hut-material bothPASS/imagesactuallyviewed/
kernel-after[]. Grain follows beams; overallstillstylized/plain, notfinalrealism.
ActualAndroid224038.084741Z-hut-material PASS/imageactuallyinspected, GLES SwiftShader,
materialmarker/nofallback/kernel-final[]/ownedexit0. ExactAPK archived
build/android-software/island-hut-material-x86_64.apk SHA
c04e08f64a718f814a9e1b2f521cff562a46827f00d611a71d3f14916e581541.
x86buildlint5sPASS; defaultARM64restored/buildlint4sPASS. NoKVM/emulatorconfigchange.
NEXT WebQA routing/actualshaderaudit, materialtemporal/longrange andremainingrealism,
hutworldreport/globalroute. Fullwildlifebacklog andAMDincident remainOPEN.
Full41/41non-displaytestsPASS54.76s. Webbuild/packagePASS (sizes in
build-probe/hut-material-package.log); no actualWebshadercheck yet. Allownedjobs
terminal, hardwarestoplatchSHAunchanged, no push/dependencychanges.

## Latest — hut roof box bundles replaced with folded irregular strips

New CPU-only HutThatch.hpp generates672 deterministic3-station/3-across leaf strips
with middle crease, varied length/curvature/course starts/pigment/uneven eave tips.
Renderer replaces448box bundles with672×24vertices = same16128roofvertices; complete
hut still6804triangles/20412vertices/1draw, no newtexture/shader, structureunchanged.
CPU allstrip bounds/determinism/downhillorder/courseoverlap/adjacentcoverage/length
variation tests. Firstvariant failed gaptest; reduced lateraldrift, testnotrelaxed.
FinalhutCPU PASS0.58s. Actual222902.709084Z-hut capturePASS/imageactuallyinspected:
rooflessgridlike,uneveneave, butstillplainwithoutfinefibres; NOTfinalrealism.
kernel-after[], nativebuildPASS, AndroidARM64build/lintPASS6s. Portableactualaudit
stillpending. NEXT wood/thatchmaterialdetail andAndroid/WebQA; globalroute/reporting
andfullwildlifeplan remainopen. NoAMDfix/hardware/KVM/dependencychange.
Full41/41non-displaytestsPASS92.60s. Webbuild/packagePASS11,214,600raw/4,215,808gzip.
Allownedjobsterminal; hardwarestoplatchSHAunchanged. No push.

## Latest — hut contact shadows added and actual image reviewed

HutContactFootprints returns one yaw-oriented soft floor ellipse(alpha.28) + six
25cm footing ellipses(alpha.34), all passed through existing rendered-terrain-clipped
BuildContactPatch. ContactShadowRenderer optional hut pointer, new counted Hut kind,
90m quality-scaled fade; existing single batch/no extra texture/shader/draw. IslandGame
supplies placement and logs hut draw/contact count.3300vertices/1100triangles cached.
Tests: yaw/footingcenters/finitealpha/vertex AND triangle-centroid ground conformity,
12000vertex cap; eight player approach tests retained. FocusedPASS0.65s.
Actual222202.355026Z-hut capturePASS/capture.png actuallyinspected: subtle underdeck/
postbase darkening improvesgroundconnection. Kernel-after[]. Loghut1draw/7contacts,
allcontactshadows43patches/1704triangles/1draw. This is NOT directionalroof/selfshadow.
NativebuildPASS, AndroidARM64build/lintPASS4s. NoAndroid/Webactualvisualrun yet.
NEXT naturalroof/woodmaterials andportableQA; globalroute/worldreport/fullbacklog
stillopen. AMDtimeoutunresolved; nohardware/KVM/dependencychanges. Goalactive.
Full suite initially40/41PASS71.31s: supervisor SIGKILL fixture failed to start within
4s. No gate relaxed; isolated rerun of entire supervisor group PASS1.82s (retained
hut-shadow-supervisor-recheck.log). Do not rewrite first run as41/41. Webbuild/package
PASS11,208,839raw/4,213,502gzip. Allownedjobs terminal, stoplatchSHAunchanged.

## Latest — first hut visible in live island, native image audited

HutRenderer added and linked into ordinary IslandGame creation/draw. Placement now
reserves actual solid obstacles plus understory1.8×scale/bush1.2×/groundplant0.5×.
Same site2.58166,204.115/floor2.51744. Shared collision grid gets radius2.83 disc
covering deck/posts; player and wildlife receive it. Eight radial player approaches
test no penetration and actual approach. Decorative/non-enterable; no indoor system.
6804triangles/20412vertices, one static buffer/draw, meshAABB/frustum/300m cutoff,
CPU fixed-sun vertex color+BasicEffect fog. Six supports/braces/deck/slats/rafters/
448narrow layered roof bundles. Finite/degenerate/30000vertex upload guards added.
Native --viewpoint hut, CLI help, native software supervisor and frame draw accounting
updated. Actual software221344.265945Z-hut capturePASS; capture.png actuallyinspected:
recognizable raisedhut, no obvious plantpenetration, butrooflooksregular/slatlike,
woodplain and missingcontactshadows makesgroundconnection weak. These stayOPEN.
kernel-after[]. Renderer no new texture/customshader. Nativebuild/HutCPU0.73sPASS;
Android ARM64assemble/lintPASS9s. NoAndroid/Webactualrun yet. No hardware/KVM edits.
NEXT hut realisticroof/wood/contactshadows, worldreport, portableQArouting+actual
images/globalroute audit. Fullgoal andAMDincidentremainopen. See docs/island-hut.md.
Full41/41 non-display testsPASS78.11s; software supervisor37PASS. Webbuild/package
PASS (payload sizes in build-probe/hut-model-package.log). All owned jobs terminal,
hardwarestoplatchSHA unchanged. No push/dependency changes.

## Latest — hut placement foundation added, NO live hut yet

Previous goal turn progressed with camera/image evidence. Moved to the explicit hut
backlog; source inspection found no island hut (only wreck cabin geometry).
New HutLayout CPU search considers 576 bounded coastal candidates, 5m clearance from
supplied occupied discs, dry locally walkable approach ring, exact rendered roof-AABB
height range <=0.45m relief. Floor is 0.45m above max; six supports sample exact rendered
ground. Front faces coast. Invalid occupancy throws; no valid site returns nullopt.
Current actual palm/jungle/prop/wreck solid fixture candidate x2.58166,z204.115,
floorY2.51744,yaw0.0126473. Tests check determinism, clearance, footing/floor grounding,
distinct posts, alternate reserved site and blocked world. Both native test presets
include island_hut_tests. Initial test was NOT RUN until explicit target was built;
then passed. No runtime integration, collision, mesh, textures or shadows yet: do not
claim a hut exists. No graphical run this turn. Android ARM64 build/lint PASS3s.
See docs/island-hut.md for contract and remaining work. NEXT integrate vegetation
reservation + hut geometry/renderer before installing any collision; do real QA.
Snake camera contrast/trunk occlusion, Web verification and AMD incident remain open.
Native build PASS; all41 non-display tests PASS52.09s. Expanded hut fixture rerun
PASS0.38s after adding distinct-post/reserved-site checks. All owned jobs terminal;
no emulator/KVM/hardware changes, dependency edits or push. Web build not run for
this currently unused CPU placement component; required before live integration.

## Latest — wide snake QA camera improved; Android image gate still FAILS

SnakeDetail initial camera now uses actual41-station centroid+(0,2,.35), aiming at
centroid, instead of oblique head-relative offset/target. Existing centroid translation
tracking retained. SnakeHeadDetail unchanged. No vegetation hidden/moved and no
simulation logic changed; observer position changes the natural retreat route.
NativebuildPASS. Actual llvmpipe215642.556056Z-snakes4-stagegrid inspected: wholebody
visible across4poses, evolving lateralwaves; priorlargeleafocclusion removed in these
poses. MotionvalidatorPASS/kernel-after[]. Genericluminance/contrastFAIL unchanged.
Androidx86build/lintPASS5s; actual215811.492358Z-snakesFAILED image qualification:
fourruntime motion samplesPASS9.372m, actual screenshotinspected: mostbodyvisible,
headpartlyhiddenbytrunkatupperedge. Savedimage luminanceSD12.6251<18, spatialcontrast
38.3623>30,21attempts, noacceptedimagewithin60s. Forcedownedteardown-9/kernel-final[].
DoNOTcallthisAndroidPASS or completewholebodyvisibility. Existinggatesunchanged.
Android23/software37CPUtestsPASS. DefaultARM64restored/buildlintPASS (see log).
NEXT improvevisualreadability/remainingtrunkocclusion, retainnativeheadview and
earlierqualifiedAndroidAPK; Webcamera/runtimeverificationstillpending. NoAMDfix,
noKVM/AVD/dependencychanges; fullwildlife/hut/residentplan remainsOPEN.

## Latest — Android head/body functional motion PASS, wide image still occluded

Prior turn progressed with exact segment rejection/native image equivalence. Added
one QA-only post-first-draw Update marker and extended CPU breadcrumbs from first
to first TWO snake QA frames; no simulation/render content changes. Actual corrected
APK SHA045a1121b58ea5bf2a5a57bc3c02354e132445d1e6ad5ec0886ebebb60a0cc44:
-215120.905025Z-snake-head PASS;4runtime samples frames8/16/24/32, displacement11.304m.
-215307.497535Z-snakes PASS; sameAPK,4samples, displacement11.320m.
Bothkernel-final[], ownedemulatorexit0, screenshotacceptedfirstattempt. BOTH actual
game/island-android.png images inspected: head/scales/eyes clear in close view;
wideview haslargeforegroundleaf coveringhead/upperbody, S-curvedbodypartvisible.
Therefore functionalmotion/GLES check PASS, not wholebodyvisual/naturalgaitacceptance.
These runs followed segment rejection but are NOT a controlledA/B proof of prior
timeoutcause; earlierfailuresremainretained. No nativeAMDfix/performanceclaim.
ExactAPKarchived build/android-software/island-snakes-qualified-x86_64.apk (old
island-snakes-x86_64.apk retained unchanged). Android23/browser18/software37testsPASS;
x86build/lintPASS8s. NoKVM/AVD/dependency/hardwareconfigurationchange.
NativebuildPASS; defaultARM64restored,assemble/lintPASS4s. Allownedjobsterminal;
hardwarestoplatchunchanged. Webnotrebuilt foradditionalQA-onlymarkers thisturn.
NEXT fix wideQA camera leafocclusion without hidingplants; portableWeb check of
segmentrejection stillpending, grass3Dcontact/normal/relaxation andfullbacklogopen.

## Latest — exact segment-level grass rejection, native pixels unchanged

Prior turn provided diagnostic evidence, not a stall fix. SnakeGrassContact CPU+GLSL
now conservatively rejects separated swept-blade/segment envelopes before four
distance calculations, expanded by endpoint maxradius+Margin+Fade+0.1mm floatguard.
No geometry/contact fade/stations/uniform capacity removed. Independent unculled
double capsule oracle:24000 deterministic queries (points+sweeps,straight/actual4body)
plus246fadeboundary cases, pressure tolerance.0002. Prior movementtests retained.
SnakeCPU PASS10.08s; nativebuildPASS, ARM64assemble/lintPASS5s. Actual contained
llvmpipe214716.834158Z-snake-head motion validatorPASS/kernel-after[]. Fourstage
motion-grid.png actuallyinspected, headretreat/scales visible with priorgrassocclusion.
Every fullresolutionstage pixelIDENTICAL to205902.050693Z (ImageMagickAE=0 each).
Genericluminance/contrast gates stillFAIL unchanged. Performancegainunmeasured;
this is NOT an Android/AMD fix. No new Android emulation or hardware run this turn.
Full wildlife/hut/resident/treehabitat/visual backlog and GPU incidents remainOPEN.
Full40/40non-display testsPASS95.27s. Webbuild/packagePASS11,178,925raw/4,203,549gzip.
Allownedjobs terminal; hardwarestoplatch SHAunchanged. Next: portable actual runtime
of segment rejection, remaininggrass3Dcontact/normal/relaxation andwidecameraocclusion;
Androidwait stillunresolved, avoid identical retries withoutnewobservability.

## Latest — finer Android grass trace contradicts a deterministic first-grass stall

Previous turn progressed by localizing CPU wait. Added nested scopes in grass effect
Apply, uniform upload and instanced bind/draw (existing opt-in diagnostics; CPU-only
markers remain limited to first snake QA frame). NO shader/geometry/contact changes.
Actual214147.707723Z-snake-head FAILED readiness again, but grass now returned:
23:42:36.164 begin, Apply same-ms, uniforms complete36.167, all draws return, grass
end36.576 (412ms CPU interval). All subsequent draw passes return, startup log reports
first draw5746.146ms/total17743.021ms. No frame8 motion sample before timeout.
Thus213726's final grass-entry marker is only an observed wait location, NOT proof
of a deterministic grass shader defect or execution completion. Presentation/queued
driver work and later frames remain unresolved. No screenshot; screencap10s timeout,
owned teardown-9, kernel-final[]. Do not weaken gates or claim Android qualified.
Read-only GDB attach to this owned emulator PID751700 captured emulator-stacks.txt,
detached successfully within seconds, no inferior function/memory changes. All stacks
lack useful symbols; no compiler/driver causation established. Attach perturbs timing
and is recorded as diagnostic, not a performance run. No KVM/AVD configuration edits.
Android23/software37 CPU tests PASS; x86_64 build/lint PASS3s. Hardware stop unchanged.
Native build PASS; default ARM64 restored, assemble/lint PASS3s. All owned jobs
terminal. Web not rebuilt/rerun for these additional diagnostic scopes.
Next: distinguish first presentation/later-frame wait with available persistent
diagnostics; avoid repeating identical full timeouts without new observability.
Original AMD incident and complete wildlife/visual/hut/resident backlog remain OPEN.

## Latest — Android first-frame stall localized to grass pass, NOT repaired

Previous goal turn made progress (gates/diagnostic fix/failure evidence). Re-ran the
corrected archived APK: 20261011T213346.432797Z-snake-head FAILED again; no motion
samples/qualified first frame, screencap timeout and owned emulator forced exit-9.
Thus missing platform logging alone does not explain the failure. kernel-final[].
Added optional CPU-only breadcrumbs to existing GpuDiagnosticScope, enabled ONLY for
frame0 of SnakeDetail/SnakeHeadDetail. No GL calls, fences, sleeps or content changes
are added by this flag; existing opt-in GPU diagnostics retain their behavior.
Actual instrumented Android213726.011849Z-snake-head also FAILED, but now proves:
clear/sky/terrain/distant-scenery/contact-shadows/natural-props/shipwreck/jungle all
return; grass begins at23:38:19.439 guest timestamp and has no end record before
timeout. No later passes, motion samples or image. kernel-final[], forced owned
teardown-9. This localizes the CPU-side wait to GrassRenderer::Draw, NOT a particular
shader operation, and is NOT proof of the cause of the native AMD timeout.
Next inspect/narrow grass Apply/uniform upload/instanced draws and compiler/runtime
cost of snake-contact shader; do not disable contact or weaken the motion gate to
manufacture a pass. No KVM/emulator configuration/dependency/hardware changes.
Android23/browser18/software37 CPU tests PASS; instrumented x86_64 build/lint PASS4s.
Native build PASS; default ARM64 APK restored, assemble/lint PASS4s. All owned jobs
terminal, hardware stop-latch SHA unchanged. Web build/runtime not rerun this turn.
Archived island-snakes-x86_64.apk remains the earlier logging-fixed candidate (no
CPU breadcrumbs); exact instrumented APK identity is retained in run.json.
All wildlife/hut/resident/full plan still open; goal remains active.

## Latest — Android snake qualification attempted, FAILED and remains open

Android supervisor now accepts snakes/snake-head and requires the shared sparse
runtime-motion validator (same finite/ordered/visible/displacement gate as Web).
Shell waits for frame32 before qualification; Android CPU23, browser18 and software
capture37 tests PASS, shell syntax PASS. Actual contained run
build/android-software/20261011T212904.738068Z-snake-head FAILED. Guest boot confirmed
SwiftShader, game installed/launched, but readiness timed out; failure screencap also
timed out10s and owned emulator required forced teardown (exit-9). kernel-final[].
No screenshot obtained or inspected; no Android appearance/motion acceptance.
Source review found snake samples lacked Android platform-log mirroring. Fixed using
the existing MirrorRuntimeDiagnosticToPlatformLog and stdout flush (QA-only four
samples). This diagnostic fix does NOT establish the cause of the unresponsive guest.
x86_64 assemble/lint PASS4s after fix; archived candidate
build/android-software/island-snakes-x86_64.apk; default ARM64 restored, build/lint
PASS4s. Corrected candidate NOT yet runtime-tested. No emulator/KVM configuration
changes, hardware stop latch unchanged. Next: investigate retained Android failure
and verify corrected candidate safely; full wildlife/visual backlog and AMD incident
remain open. Previous turn was status-only; this turn adds gates and actual failure
evidence, not a claim of goal completion.

## Latest — actual WebGL retreat repair PASS with numeric motion gate

IslandGame nowprints4sparse runtimehead/speed/visible samples frames8/16/24/32 ONLY
interactive SnakeDetail/SnakeHeadDetail, notgameplay/nativedeterministiccapture.
Parent browser validatesbothviews, exactordered4samples, finitehead/speed0..1.151,
visible1..4, >=.1m displacement. BrowserCPU18PASS inclfrozen/missing/nonfinite/reordered
rejections; softwaresupervisor37/assets25/81/10/0PASS.
Actual build/browser-software/20261011T210926.570528Z-chrome-snakes finalPASS,
Chrome155.0.8059.39SwiftShader: body5.892m/head5.614m sampledisplacement; each60intervals,
glError0/no contextloss/kernel-final[]. Both body-motion-grid.png andhead-motion-grid.png
actuallyinspected. Headnowcontinuesretreat acrosschangingterrain/inframe; materialvisible.
Widerbodyposesstillleafoccluded(onefullyhidden), notwholegaitacceptance. NoAMDfixclaim.
Native/Web buildsPASS, package11,167,232raw/4,200,881gzip; ARM64assemble/lint4sPASS.
NoAndroidactualruntime. Allownedjobs terminal, hardwarestoplatchuntouched, noKVM/deps/push.
NEXT Androidcontainedactualsnakeviews ifappropriate, wideQAleafocclusion/continuousmotion,
3Dgrasscontact/normal/relaxation/treehabitat; allotherwildlifebacklog/hut/residentopen.
Goalactive; originalAMDtimeout remainsunresolved, nohardware reproduction authorized.

## Latest — close-frontal retreat repaired in native CPU + actual image audit

SnakePopulation adds turnAside fallback onlywhenordinaryretreatsubstep rejected.
Sidechosenaway, halfspeedhead, quadraticbendfront12stations/66cm, remaininghalfmovement
budget restores55mm jointlength gradually across40joints. Explicitperjointdisplacement
guard + originalValidBody/obstacle/selfclearance/home/ground + nonapproach beforecommit.
Acceptedturnreseed43trailpoints; normalretainedheadtrail unchanged/512cap unchanged.
Earlyvariants rejected: onejointpivot travel.233m/waves0; distributedno-restoration
2.499m/waves0. Finaloriginalcoverage7.902m/2340waveframes, alloldtestsretained.
Closeobservertest nowUNCONDITIONAL indefault (oldCLIaliaskept),4offsets900×1/60:
(.35,.3)7.553m,(.35,-.3)7.760m,(.5,0)7.900m,(3,0)5.176m, eachstepbodymotionand
nonapproachchecked. FocusedPASS9.30s; all40non-displaygroupsPASS84.37s.
HeadQA nowfollowsheadtranslation notcentroid. Actualcontainedllvmpipe
build/software-captures/20261011T205902.050693Z-snake-head/motion-grid.png inspected:
headturn/retreatvisibleall4, bendpropagates, somegrassocclusion. MotiongateNOWPASS,
2.394m first/lasthead displacement, graphicsvalid/kernel[]. Genericluminance+large
contraststillFAIL unchanged. Notfinalnaturalness/portable/runtimeorallroutesacceptance.
Native/Web builds/package11,166,542raw/4,200,268gzipPASS; ARM64assemble/lint7sPASS.
Allownedjobs terminal; noportableactualrun yetforrepair. Stoplatchunchanged. NoAMD
hardware/AndroidKVM/dependency/push. Goalactive, GPUtimeoutcauseunknown.
NEXT portableactualretreat review, bodywideview andcontinuousnaturalness/grasscontact
remaining; then treesnakes/otherwildlife/hut/resident/fullbacklog stillopen.

## Latest — native matched head UV correction; retreat still fails

SnakeHeadProfile.hpp shared SkinU/V density helpers; headV now matchesbody1.5/55mm,
nosefan pole duplicatedpersector withmidU andnoseV instead ofsame(0,0) stretching4wraps.
No geometry/triangle/textureallocation/simulationchange. CPU seam/density/localUVspan
tests, normalsnake suitePASS9.79s; knownoptincloseretreatstillopen (notnormalgreen).
Native before build/software-captures/20261011T204120.519708Z-snake-head andafter
20261011T204412.087633Z-snake-head:4 timedbodylogsidentical, graphicsvalid/kernel[].
Bothstage0PNG+afterhead-before-after.png cropsactuallyinspected; moreregularheadscales
andbetterdensitycontinuity. BOTH captureoverallFAIL motiongate dueconfirmedstationary
closeretreat. Validatorunchanged; pairedimagesproveonlylocalizedmaterialimprovement.
Native/Web builds, package11,162,882raw/4,197,939gzipPASS; ARM64assemble/lint3sPASS.
Supervisor37/assets25/81/10/0PASS. Nonewportableactualruntime. Allownedjobs terminal.
Stoplatchunchanged; nohardware/AndroidKVM/dependency/push. Goalactive, AMDcauseunknown.
NEXT close-frontalretreat stillpriority; boundedneck/turnsolver mustretainalltests,
alsoheadQAcamera centroidtracking/leafocclusion andremainingwildlifebacklog stayopen.

## Latest — close-frontal snake retreat deadlock reproduced, NOT fixed

New opt-in CPUreproducer `build/island_snake_tests --close-observer-regression` uses
actualterrain/vegetation/jungle/propobstacles, firsthead+(0.35,0.25,0.30) fixedobserver,
900×1/60s. Wholebodyvalid, displacement0m, exit1KNOWNOPENDEFECT (normalreturn,noabort).
Default snakeCTestPASS9.48s DOESNOTcover thisknownfailure. Logs snake-close-retreat-
reproducer.log/restored-tests.log; initialfailingtest log snake-close-retreat-tests.log.
Mechanismfromsource: nonapproachheadconstraint + arc-lengthresample/chord55mm±3%
rejectsharpretreatturn; headingrotation doesn'tbendneck/trail. Triedcircle/path exit
sampler; existingtest caughtstationteleport, thenwithmovementguard trail512overflow.
FullyREVERTED experimentalSnakePopulation.cpp edits viaapply_patch (diffempty), rebuilt
CPUtest againstrestoredsim. No game/renderer/portablebuild usedexperiment; noGPU run.
Only testreproducer/docs changed. Alljobs terminal, goalactive, no push/dependency/KVM.
NEXT: boundedcontinuousneck/turn/pathrepair; retainnonapproach/obstacle/selfclearance,
spacing/motion/512trailconstraints. DoNOTrelaxoldtests orclaimnormalgreen meansfix.
Prior WebGLheadstationaryfinding isconfirmed simulationdefect. HeadUV andQAcamera
tracking stillopen, AMDtimeoutunresolved. This is not a goalblock; moreworkpossible.

## Latest — actual WebGL snake body/head audit, new defects retained

New tools/run_browser_software.py --scope snakes routes ordinaryinteractive snakes +
snake-head via web_smoke.html.in, sequentialcontexts,4intermediates/60intervalseach,
SwiftShader/GL0/contextalive/visible1..4snakes andno materialfallback required. CPU
BrowserSoftwareTests17PASS, SoftwareCaptureTests37PASS,node syntaxPASS. No gamebuild.
Actual build/browser-software/20261011T202147.595070Z-chrome-snakes finalPASS, bothviews
60intervals/glError0/no loss, kernel-final[]. Allownedjobs terminal, no hardwareGPU.
Initialbody/head images AND body-motion-grid.png/head-motion-grid.png actuallyinspected.
BodyS-bends/scalesvisible butlargeforegroundleaves completelyocclude oneintermediate.
Headeyes/mouthreadable butheadscaleUVsize/shape doesn'tmatchbody. Headappearsstationary
through4poses whilegrassmoves: doNOTclaimretreat/gaitpass. Theseare realtimeviewsNOT
nativefixedclock. SourceheadQAcamera followscentroid nothead. NEXT investigate close
observerretreat (possibleblockedheading), headmaterialcontinuity andheadcameratracking;
do not removeactualvegetation justto makeimagespass. Androidruntime/treesnakesopen.
No dependency/AndroidKVM/push; stoplatchunchanged. AMDincidentcauseunknown. Goalactive.

## Latest — deterministic snake capture timing verified by exact repeated pixels

UsesSnakeCaptureClock scopes ONLY capturePath+SnakeDetail/SnakeHeadDetail. Framework
fixed-stepcatch-up disabled there; Update uses0.1s/countedupdates and Draw wind/water
time/monkeyabsolute time samecounter. Gameplay/manual viewpoints/othercaptures unchanged.
Stage logs nowrequire updates8/16/24/32 andtime0.8/1.6/2.4/3.2. Supervisor rejects
missing/extra/malformed records/wrongtime/nonfinite/extra updates;37testsPASS.
Actual containedllvmpipe runs build/software-captures/20261011T201403.216063Z-snakes
and20261011T201453.821080Z-snakes: all4 bodytraces EXACT andall4 stagePPMs BYTEIDENTICAL
(hashes docs/ground-snakes.md). Firstmotion-grid.png actuallyinspected; snakevisibleall4,
headbend propagatesinto initiallystraightbody, flattenedgrass visible. Bothgeneric
luminance ANDlargecontrastgatesFAIL unchanged; bothgraphics/timedmotionPASS/kernel[].
Native/Web builds andWebpackage11,162,865raw/4,197,939gzip PASS, ARM64assemble/lint4sPASS,
assets25/81/10/0PASS. Noportableactualruntime. Allownedjobs terminal. Stoplatchunchanged.
NEXT: use these reproducibleposes for contact/normal/relaxation improvements; no longer
compare real-time oldframes asmatchedimages. Crossdriveridentity notclaimed. Full3D
curvedtriangleclearance, continuousmotion, portableviews, treesnakes andotherbacklogopen.
AMDtimeout stillunresolved, nohardware/AndroidKVM/dependencywrite/push. Goalactive.

## Latest — swept blade-root contact, visual audit timing gap identified

SnakeGrassContact::SweptPressure now tests root-to-vertex segment vs snake segments:
crossing returns1, otherwise min4point/segment squared distances → radius/margin/fade;
early fullpressure return and cellmask preserved. Pressure(point) delegates zero-length
segment. GLSL mirrors CPU. Grass customVB48bytes (was40), newvec2 root atlocation5,
instances shifted6..9 (unchanged44bytes). Allblade/turf/seed vertices get localroot;
length/finite guards. +7,712staticVBbytes, no extra draw/texture/uniformarrays.
Focused CPU2/2PASS9.91s covers endpoint-outside crossing/reversal/parallel/collinear/
nonfinite/interior oracle/mask; supervisor37/assets25/81/10/0PASS. Native/Web builds,
Webpackage11,163,384raw/4,198,207gzip, ARM64 assembleDebug/lintDebug4sPASS.
Actual software capture build/software-captures/20261011T200740.707227Z-snakes:
motion-grid.png actuallyinspected, snakevisible0/2/3, largeleaves obscurestage1.
Graphics/motionreplayPASS/kernel-after[], genericluminanceFAIL unchanged. Real-time
updates produced differentposes frompreviouscapture, so NOT matchedbeforeafterproof.
NEXT: deterministic capture-only simulation timing needed before image regressions;
gameplay stays real-time. Still no curvedtriangle/3D collisionguarantee, normals and
temporalrelaxation/performance/portableactualviews open. AMDcauseunknown, nofixclaim.
Allownedjobs terminal; no GPUhardware/AndroidKVM/dependencies/push. Stoplatchunchanged.

## Latest — snake-grass contact with conservative whole-cell rejection

Finished initial pending contact implementation plus CPU CellMask over full grass
cell.bounds horizontal envelope (mesh/slope/wind/player motion), not placementBounds.
Shader uniform vec4 mask early returns for all-zero cells and skips nonoverlappingsnakes;
no new draw/texture. Pressure still evaluated at vertices afterwind; NOT continuous
blade/triangle collision. CPU movingbody grid masked/unmasked equivalence/boundary/
invalidinput tests added. Native focused snake+grassmotion PASS2/2 6.31s; supervisor37,
assets25/81/10/0 PASS. Native/Web builds, package11,160,494raw/4,195,534gzip PASS;
ARM64 assembleDebug/lintDebug PASS5s. No portable runtime or GPU speedup qualification.
Actual containedllvmpipe build/software-captures/20261011T200218.123460Z-snakes:
motion-grid.png generated and actuallyinspected; S-bends/body visible, obliquegrass
intersections andleafocclusion remain. GenericluminanceFAIL unchanged, graphics/motion
replayPASS/kernel-after[]. Compared old185758 stagePPMs are notbyteidentical; no
pixel-equivalence claim. Allownedjobs terminal. No AMDGPU/Android/KVM/dependency/push.
Originalincident candidate+pendingpatch/header preserved; timeoutcauseunknown and
stoplatch unchanged. Source-level loop rejection is NOT evidenceof crashfix.
NEXT: whole-blade sweptcontact/normaldeformation/relaxation and portableactualviews
stillopen, plus incidentoffline resource/lifetime/workloadanalysis. Goalactive.

## Latest — actual persistent flight/fullscreen diagnostic verification

Added strict scripted transition validator to tools/run_fullscreen_software.py, reads
owned persistent log via startup namespace PID (filename deliberately uses outerPID).
37 CPU supervisor tests PASS, including ambiguous/missing identity and bad state/order.
Actual contained llvmpipe flight run build/software-flight/20261011T195901.992700Z PASS:
two F11 cycles; held Space, ascent100m/descent/safe return; six persistent events at
frames3/8/10/14/22/51. Kernel-final[]; stoplatch SHA unchanged. Ceiling/landed PNGs
actually inspected: sparse slopes/grass transitions still open. NOT AMD crash fix.
Two earlier runs195708.323678Z/195829.747964Z completed scenario but failed loglookup
(outerPID mismatch, then matcher omitted main-tid); retained as failed. Alljobs terminal.
No build, hardware GPU, dependencies, Android/KVM or push. Pending snakegrass edits
remain intact/uncommitted; only supervisor/tests/docs changed this turn. Goalactive.
NEXT: actual timeout cause still unknown. Continue offline resource/shader/workload
investigation and planned visual work; software persistence success does not authorize
hardware reproduction or mark incident resolved.

## Latest — offline grass CPU upload audit, no found bounds violation

Extended core-review.py to strict retained CPU upload bytes; no GetData/inferiorcalls.
After newbuild, select preservedcandidate via GDB `file .../island-simulator.candidate`
before sourcing helper; helper reloads temporarycore to restore sharedlibrarysymbols
and validates original4 buildIDs. First oldexe mismatch warnings are expected BEFORE
explicit candidate selection; do not interpret symbols from newexe. No gamebuild thisturn.

Final core-grass-all-cells-review.txt: array256, occupied114, patches17225 matches
renderer total; all instance44byte strides/counts/finitefloats valid. Meshes near862v/
1920indices/max861; middle60/120/max59; far42/84/max41. Allstride40; shadowsize/counts,
finitepositions/indexlimits/primitivecount checks pass. Earlier64cell partial and
typedef-unwrapping failedattempt retained separately, not fullaudit evidence.
Shaderreview: snake s<4,j<41 maxindex163 of164vec3; typed upload164×3=492 floats.
Existing grass draw offset/count guard present. NO proven bounds bug; GPUdescriptors/
resourcebinding/compiledshader/performance stillunproven. Snapshot not faultingGPUframe.
Docs updated, allownedjobs terminal, stoplatchunchanged, no GPU/dependencywrite/push.
Pending grass changes remain uncommitted and intact. Goal/incident remain open.

## Latest — sparse persistent control transitions, NOT GPU crash fix

Added pure ControlTransitionDiagnostic.hpp (event enum + message + snapshot formatter),
IslandGame::ReportControlTransition mirrors existing console notice and separate detailed
state into timestamped/flushed persistent sink on native fullscreen request and flight
on/off/landing-refused. Snapshot: submitted frames, camera/flight xyz, yaw/pitch,
flight-active/fullscreen-requested, finite flag; classiclocale/max_digits10. No perframe
logging/GPU synchronization. noexcept best-effort. Webfullscreen eventpath unchanged.
Focused native build/tests PASS2/2(.72s), supervisor35/assets25/81/10/0PASS. No graphics
or new interactive persistence test after incident; no crashfixclaim. Original candidate
is safely copied and SHAunchanged before rebuild. New native executable includes pending
grasswork; the diagnostic commit must exclude that unrelated grass-call hunk.
Logs build/control-transition-{build,tests}.log. Allownedjobs terminal. Goalactive.
NEXT: actual fault/state/resource investigation remainspriority; runtime diagnostic
persistence verification only via containedsoftware whenappropriate; never hardware.

## Latest — offline core build IDs verified, finite in-bounds flight snapshot recovered

Preserved22,437,288byte game candidate in build/gpu-incident-evidence/20261011T190021Z-user-flight/
island-simulator.candidate, SHA6e509871e84a624a0eb61dbbfa478eb4ac05b0616869ed29290d682bb822bf4a.
Batch coredumpctl/GDB used nx/nh, auto-loadoff/debuginfodoff, never ran/resumed inferior.
Temporary core expansion cleaned byhelper; original compressedcore retained. Read PT_LOAD
pages directly, no file-backed fallback: game e37224286aeef8884283b5f2231b5983fc2231cd,
Gallium f34ab849e878d8ffbbb15533a597ca31ebd583ec, CNA f8dc807d0df965e4dfb6881e3cae4b1269f70acf,
SDL3 c25c94cd74bd8f6413a1ab65fa4f5e662ab16689 all match currentfiles.
Thus current gamecandidate includes pending snakegrasschange; NOT proof of causation.
Preserved trackedpatch +untrackedSnakeGrassContactheader in sameevidencedir.

Gameobject0x7ffcef5dc0c0, CPUdrawcounter2290; flightactive and camera/rendercamera/flight
all(-15.9166985,49.44701,-118.114937). Radius119.18255m, retainedground4.66093m,
aboveground44.78608m; yaw-.417899817/pitch-.296599835, matricesfinite. Grass2838patches
(776/470/1592),24draws/customshader1; monkeys4/4draws; shadows20/240tris/1draw.
CPUabortsnapshot != faulting GPUframe; no NaN/outofbounds evidence in thissnapshot only.
Raw compactcore-state-review.txt retained. Versioned offlinehelper
docs/incidents/evidence/2026-10-11/core-review.py, reportupdated. No newGPU/build/push.
Alljobs terminal. Stoplatch unchanged. Uncommitted grasswork preserved; goalactive.
NEXT: offline renderresource/shader/state bounds inspection with matching identities;
controlled software coverage later only after reviewing freshincident. Don't claimcrashfixed.

## Latest — read-only flight incident triage, no new GPU workload

Recovered actual runtime423089 log from ~/.local/state/island-simulator/runtime; exact
copy in build/gpu-incident-evidence/20261011T190021Z-user-flight/runtime-423089.log,
SHA bfac8da5e9c35b767d80aa1511c4003da0ecde765c0029368caadce187627092.
Started18:59:24.835UTC, firstframe18:59:33.012, lastmonkey18:59:58.852, kernel19:00:21.
Confirms real AMD780M/Mesa25.0.7 nativeWayland4xMSAA; GPUdiagnostics unset, no pass markers.
Lastmonkey is22s beforetimeout; not culpritproof. Flight/fullscreen console messages not
mirrored inpersistentlog. Archivedmaps truncated4024bytes/57lines, no full libraryidentity.
Exe inode37231455 matches current22,437,288byte build, mtime20:56:52.391CEST precedingrun;
buildID e37224286aeef8884283b5f2231b5983fc2231cd, SHA6e509871e84a624a0eb61dbbfa478eb4ac05b0616869ed29290d682bb822bf4a.
DO NOT rebuild over candidate before preserving/resolving loaded-imageidentity.

Cleanlocal Mesa25.0.7 source maps exacttext to -ENODATA/guiltyreset and abort for context
notallowingloss. InstalledGallium disassembly corroborates core returnPCs0xa66824/0xa6a5f2
and string0x18e05e0. Terminationmechanism established, original GPUtimeout cause NOT.
Olderincident had X11/GLX GPUVM0x17000; newWayland/EGL record has timeoutonly. Notproven
samecause. Full evidence/limits in docs/flight-amdgpu-incident-2026-10-11.md.
No coreextraction, driverchanges, rendererlaunch, dependencywrite orpush thisturn.
NEXT: loadedcore build-ID/candidate preservation and offline stateanalysis; sparse persistent
controltransitiondiagnostics are a gap (do not mistake telemetry improvement for fix).
Uncommitted snake-grass work below preserved intact. All ownedjobs terminal. Goal active.

## PRIORITY — operator GPU timeout/abort while flying, 2026-10-11 21:00 CEST

User requests analysis task after fullscreen/repeated flight toggles/traversal freeze.
Read-only kernel confirms21:00:21 gfx_0.0.0 timeout/soft recovery; coredumpctl PID423089
confirms21:00:22 SIGABRT. Abort in Mesa/Gallium thread423277; mainthread in EGL/SDL/CNA
presentation. Rootcause unknown; last monkeypalette log is NOT culprit proof. Exact
loaded executable/library identity not established (file has been rebuilt during work).
See docs/flight-amdgpu-incident-2026-10-11.md and new HIGH PRIORITY unchecked PLAN task.
Raw core metadata preserved in build/gpu-incident-evidence/20261011T190021Z-user-flight/
coredump-info.txt; core remains in systemstorage, no extraction/deletion/upload.
No renderer launched after report; no hardware reproduction authorized; stop latch unchanged.
Investigate fresh evidence read-only first. Do not misfile as confirmed framework bug.

Interrupted work, intentionally still UNCOMMITTED: SnakeGrassContact.hpp header with
packed164 vec3 points +4bounds (all4 groundsnakes), per-segment radius+15mm core /140mm
softedge pressure, bounds rejection; GrassRenderer uploads it and presses contacted
vertices to<=2mm terrain clearance after wind, keepsroots. Single shader/batch/no texture.
CPU tests pathcoverage/softedge/far/empty/invalidsamples PASS8.87s, supervisor35PASS,
assets25/81/10/0. Actual native185758.816335Z-snakes four-stagegrid +stage1 inspected:
lessupright grass throughbody, some obliqueblades/leafocclusion remain. Graphics/motion
valid, kernelempty for that PRE-INCIDENT run; generic luminancegate FAIL unchanged.
Web build/package PASS11,154,560raw/4,194,144gzip; ARM64 build/lint PASS5s. All jobs
terminal (last97461). No portable runtime qualification for grass shader yet. Uniform
budget/performance and triangle-interior contact/normal deformation remain unqualified.
Preserve these edits; don't falsely attribute operator fault to them from chronology.
Grass logs build/snake-grass-{build,tests,arm64}.log and build-probe/snake-grass-{build,package}.log.


## Latest — retained head trail improves snake slithering

Replaced leader projection with arc-length sampling of accepted head path, newest-first
deque per snake, min6mm commit spacing, prune just beyond tail, defensive512record cap.
Body55mm arc spacing (existing3% chord tolerance), grounded perstation; rejected movement
does not enter trail. Head swing0.8rad/phase6rad per metre accepted travel. No newtextures.
Tests require >1rad both bend directions beyond neck in >10frames, allstations no teleport,
and fine1ms/coarse10ms agreement<25mm. Full40 groups PASS58.03s; final addedfine-step
focused PASS7.55s. Supervisor35/assets25/80/10/0 PASS. Nativebuild passed.

Actual184024.136306Z four-stagegrid inspected, improvedbend but travel1.693m short.
Extended capture to frames8/16/24/32 (still4samples). FINAL
`20261011T184233.069497Z-snakes/motion-grid.png` inspected: S bends visible along body,
head displacement3.573m. Laststage leafocclusion, grassintersections persist. Both runs
graphics/motionreplay PASS, kernelempty, genericimagegate FAIL (final luminance), unchanged.
Not finalnaturalness/continuouscollisionproof. No hardware/emulator/KVM changes.
Native/Web final builds and Web package PASS; ARM64 assembleDebug/lintDebug PASS7s.
First ARM64 failed -Wdangling-gsl in the prior QA camera range-for over temporaryspan;
fixed by retaining the span locally (no population lifetime change). Final native/Web
rebuilt after this fix. Logs build/snake-trail-{build,all-tests,fine-tests,arm64-final,
final-build}.log and build-probe/snake-trail-final-{build,package}.log. Softwarecaptures
precede only this lifetime-warning fix, not a simulationchange. Allownedjobs terminal;
GPUstop SHA unchanged. Builds not portable runtime evidence. Goal active; no push.

NEXT: grass/body intersections, clear subject-aware image QA, portable actualsnake views,
longerturning/obstacle/stall audit (fourposes not comprehensive motionacceptance), then
tree snakes and all other animals/hut/resident/boar/wreck/control/audio backlog intact.

## Latest — actual live snake motion audit finds rope-like motion

SnakeDetail/SnakeHeadDetail QA camera now follows mean-body translation, fixedorientation;
ordinary live simulation/observer retreat unchanged. Captures at frames4/8/12/16 plus
41 station positions/speed/phase; finalcapture16. Software frames variable time, not video.
Validator requires ordered4 finite connected41-point samples, visible count and displacement;
does not prove image visibility or natural motion. Negative fixtures added, supervisor35PASS.
Native build + focused snake tests PASS6.27s; portable builds not rerun for this QAchange.

Actual183434.634965Z-snakes four images inspected separately: head follow clippedtail.
Final `20261011T183616.764190Z-snakes/motion-grid.png` four stages inspected after meanbody
tracking: body remains inframe, head travels2.171m. Both graphicslogs valid, kernelempty;
both FAIL unchanged generic luminance/spatialcontrast gate. All ownedjobs terminal.

NEXT CONCRETE DEFECT: visible motion is rope-like, mostlystraight before a broad bend,
not propagated slithering. Current follow-leader projection smooths tiny headoscillation;
investigate retained head trail sampled at arc length with boundedcurvature, preserving
wholebody habitat/obstacle/self/grounding invariants. Grass stillcrossesbody. Fourposes
not continuousqualityproof. See docs/ground-snakes.md. Tree snakes/allremainingplan open.
Goal active, no push/hardware/KVMchange. Previous shadow commit8b55653 remains valid.

## Latest — continuous ground snake contact shadows

Added SnakeContactShadow CPU geometry: soft continuous body ribbon/head cap/tail fade,
clipped to exact rendered grid triangles, 4 mm offset, max alpha .23. Reuses population
height cache, existing single shadow batch; reserves 12k vertices per snake, no texture.
Any station moving >8 mm invalidates cache. Existing profile scales shadow distance.
F1 snake visible/total/shadows/draws added (layout not yet visually reviewed).
Tests cover vertices/interior triangle points over 2400-update scenario and invalid
NaN/stretched/degenerate fixtures. Asset inventory now25/80/10/0.

Actually inspected `20261011T182707.559480Z-snake-head/capture.png`: subtle body-ground
darkening, substantial grass occlusion and head facing away. Generic luminance gate
FAILS; independent graphics-log replay PASS, kernel delta empty. Startup1/4 snakes,
1 snake shadow, single shared shadow draw. Not final appearance/motion acceptance.
See docs/ground-snakes.md. Full 40 non-display groups PASS87.01s; supervisor34PASS,
assets25/80/10/0. Web Release build/package PASS11,138,018 raw/4,188,113 gzip bytes;
ARM64 assembleDebug/lintDebug PASS7s. Logs build/snake-shadow-all-tests.log,
build-probe/snake-shadow-{build,package}.log and build/snake-shadow-arm64.log.
All owned jobs terminal; GPU stop latch SHA unchanged. Builds are not runtime images.
Next: live continuous motion/grass interactions, subject-aware close QA,
portable images and tree snakes. Keep whole goal active, no hardware/KVM changes/push.

## Latest — readable snake head refinement, image gate still not passed

SnakeHeadProfile header + connected muzzle sections replace conical nose; surface
mouth seam and convex amber eyes/dark pupil caps replace flat eye fans. Shared profile
tests check 360-angle full spherical clearance (50 mm) and eye spheres/neck join/lower
head bound. Found slight rotation-envelope excess, shortened front section1mm before
final capture/test. Final mesh18,240/20,000 vertices; no new textures. Asset25/78/10/0.
Added fixed `snake-head` to Main/IslandGame/contained capture list. Ordinary live snakes,
not replacement/frozen pose. Native build and focused tests PASS8.36s; supervisor34PASS.

Actual images181314,181427 and FINAL `20261011T181556.624816Z-snake-head` inspected.
Near eye/pupil, mouth seam and muzzle now recognizable; grass still obscures parts,
face stylized not final realism. All three fail unchanged generic luminance variation
gate (do NOT claim passing captures). Final graphics-log replay passes, kernel empty.
Final PNG beside PPM in build/software-captures/run directory. Docs/ground-snakes.md
records scope. Logs build/snake-head-{build,tests}.log.
Web Release build/package and ARM64 assembleDebug/lintDebug PASS (Android6s);
logs build-probe/snake-head-{build,package}.log and build/snake-head-arm64.log.
All owned jobs terminal; GPU stop latch unchanged. These builds are not portable
runtime image qualification; generic close image gate remains failed as above.

NEXT: continuous body motion/grounding audit, clear subject-aware close QA without
accepting blank/missing animals, grass interaction/contact shadows and portable
runtime snake views; then tree snakes (still absent). F1/WorldReport snake metadata,
profile culling/LOD and all other full-plan work also remain. Goal active; no push.

## Latest — ground snakes live/rendered, close image gate still open

Added SnakeRenderer and ordinary IslandGame instantiation after world obstacles,
update/draw and draw-count/startup-visible diagnostics; Main/FixedViewpoint snakes QA.
Connected 41×16 tube with tapered tail/capped nose/eyes, shared canopy tint, 70 m/frustum
cull, one 20k-vertex dynamic batch. Original repeating 64x64 scale texture +16,384 bytes;
generated total4,652,172/runtime6,399,796/allowance6,420,012. Exact tests and Web/Android
log expectations updated. Manifest/provenance gate passes25/77/10/0.

Native build +40 CPU groups PASS73.86s; supervisor34 PASS. Four actual native captures
inspected, all kernel deltas empty. First180203 shows snake but oversized scales;
softened borders and increased UV frequency. Wider180405 and opposite180523 pass
generic image metrics BUT hide snake with foliage, rejected as appearance evidence.
FINAL `20261011T180722.242708Z-snakes` restores close view and visibly shows fine-scale
tapered snake on ground. Generic scene contrast gate FAILS (luminance11.8835<18,
spatial17.8797<30). Do NOT claim successful final capture! No thresholds changed.
Screenshot reviewed; head/eyes still weak, grass crosses body, no motion audit yet.
Details docs/ground-snakes.md. Final PNG is in capture directory beside PPM.
Final graphics-log replay PASS (image gate still FAIL). Web Release build/package
PASS11,118,476 raw/4,183,185 gzip; ARM64 assembleDebug/lintDebug PASS10s. Logs:
`build/snake-render-build.log`, `build/snake-render-tests.log`,
`build-probe/snake-render-build.log`, `build-probe/snake-render-package.log`,
`build/snake-render-arm64.log`. All owned jobs terminal; GPU stop latch unchanged.

NEXT: improve head readability/continuous motion, validate final mesh clearance and
snake-specific close-view acceptance without accepting missing/blank subjects; contact
shadows/grass overlap, profile-scaled culling/LOD, F1 and WorldReport snake metadata,
portable actual snake images. Tree snakes not implemented. Keep whole task unchecked.
No hardware GPU or emulator/KVM changes, no push; full visual/wildlife goal remains active.

## Latest — ground snake simulation started, not yet visible

Added SnakePopulation header/source: four ground snakes, 41 stations spaced .055 m
in XZ, tapered .004–.048 m radius, head follows bounded sinusoidal steering and body
follows preceding station (not a rigid waving stick). Rest/explore/retreat, .38/1.15 m/s,
dt <=.1 with movement substeps <=.012 m, 18 m home radius; retreat cannot move toward
nearby observer. Dormant at >120 m; reduced tier currently uses same bounded stepping
as full tier. One heap RenderedTerrainSurface cache and WorldCollisionGrid per population.
Spawn searches whole-body safe habitat; rejects impossible layout. Finite mesh/dryshore/
rendered slope restrictions; containing-disc checks cover horizontal body/solid segments,
nonadjacent station checks reject self-intersection. Vertical support uses local rendered
samples, not a continuous final triangle-mesh penetration proof. Renderer still absent.

Dedicated IslandSnakePopulationTests / island_snake_tests added to CMake and both
explicit native test build presets, linux-core regex. Initial test fixture ctor corrected;
initial run exposed negative floating-point sin(pi) at tail causing fractional-power NaN,
fixed with nonnegative clamp and radius regression. Focused test passed 7.97 s before
the additional independent segment/obstacle oracle; final rerun recorded below.
Tests use actual vegetation/jungle/prop obstacles, 2400 updates with repeated simulation,
all behaviors, retreat monotonicity, ground samples between stations, invalid dt/observer,
dormancy and blocked habitat. Logs build/snake-population-{build,tests}.log.
Final independent-oracle rerun PASS (10.31 s), diff check clean; all owned jobs terminal.

NEXT: implement SnakeRenderer and actual IslandGame population/update/draw integration,
with continuous tapered mesh, recognizable head/eyes and scale material (avoid a chain
of geometric lumps), QA viewpoint + actual image audit. Do not claim snakes are present
until rendered/seen. Ground feature remains partial; tree snakes still entirely open.
All other tasks, audio listening and full goal remain open. No renderer/emulator launched.

## Latest — actual native boar playback/mute verified with disk audio

Native `build/software-boar-audio/20261011T174853.374413Z` passed parent and worker,
empty kernel delta. Existing fullscreen/focus/mouse tests also rerun. Added optional
`--boar-audio` to contained fullscreen launcher: forces SDL disk backend (no host
audio settings), captures actual S16LE stereo 44100 Hz stream, uses F1/K to herd,
requires two boar calls, real M mute, 24 muted diagnostic intervals, M unmute/thirdcall.
130 audio samples, max5/6 sources; muted counter constant/no transients, 1,048,576
exact-zero PCM bytes, then 884,736 nonzero-containing resumed bytes. Wildlife count
now increments only after Play() actually enters Playing. Native build passed.
34 supervisor/trace CPU tests pass; strengthened final validator replays actual trace.

K with overlay visible cycles six live boars; controls/README/F1 help updated. New
audio diagnostics are separate lines, preserving existing input parsers. Screenshot
actually inspected: readable overlay, animals hidden by vegetation; NOT visual boar
acceptance. `game-audio.wav` is a lossless wrapper of mixed raw disk stream, available
for listening, not an isolated boar sound. No listening capability available here;
naturalness, spatial hearing, hardware output and Web/Android lifecycle remain open.
Prior native display tests had unavailable ALSA and did not qualify actual audio.
See docs/audio-review.md. All original visual/wildlife tasks and full goal stay open.
Final Web Release build/package and ARM64 assembleDebug/lintDebug PASS (Android 6 s).
Logs: `build-probe/boar-audio-device-build.log`, `boar-audio-device-package.log`,
`build/boar-audio-device-arm64.log`. All owned processes terminal; latch unchanged.
NEXT: keep remaining listening/portable audio checks explicit, and move to missing
wildlife (snakes are still absent from game sources; ground AND tree snakes remain
unchecked). Do not spend the whole continuation on repeated green audio/build gates.

## Latest — boar clips and live audio integration, listening still open

Six original synthesized mono 22.05 kHz / 1.2 s clips in GenerateBoarCall(kind,variant),
two breath/snuffle/grunt variants. Filtered deterministic noise and low irregular
harmonics; not animal recordings. Live Soundscape creates effects one at a time,
feeds BoarHerd to scheduler and uses existing single wildlife voice after due bird/
monkey calls (six-source budget unchanged). Boar emitter follows stable animal index,
and gain follows listener distance every update; invalid/dormant/removed source stops.
BoarCallVolume shared by scheduler/live playback: breath 9 m, snuffle 23 m, grunt 42 m;
the earlier wander 38 m choice is superseded. Mute uses existing transient teardown.
F1 BOAR counter and first-playback diagnostic added. No third-party assets/dependency
changes; original source provenance updated in ASSET_LICENSES.md.

Native build and 39/39 non-display groups PASS (83.36 s). Audio report/export validates
22 distinct clips / 3,653,684 PCM bytes, +317,520 for boars. Exact silent endpoints;
new clips peak .48/.62, RMS .0623–.1303, absolute mean <=.00456. Previews retained at
`build/boar-audio-preview/`, report `build/boar-audio-report.json`. Build/tests logs
`build/boar-audio-runtime-build.log` and `build/boar-audio-runtime-tests.log`.
Web Release build/package PASS: 11,089,781 raw / 4,172,122 gzip bytes; logs
`build-probe/boar-audio-runtime-build.log` and `boar-audio-runtime-package.log`.
ARM64 assembleDebug/lintDebug PASS (6 s), `build/boar-audio-arm64-build.log`.
All owned jobs terminal; no render/emulator launch, GPU latch unchanged (c0bd1704…).
No listening-capable tool was available; do NOT infer naturalness/audible game behavior
from these numerical passes. NEXT: contained actual runtime playback/arbitration/mute
test near herd, spatial listening to clips/game, Web/Android lifecycle qualification.
Task remains unchecked and full goal active. See docs/audio-review.md for details.

## Latest — boar sound scheduler implementation started (not audible yet)

Added BoarCallScheduler/BoarCallRequest/BoarCallKind in SoundscapeModel header/source.
Consumes stable BoarHerd span order; Rest breath (9 m), Forage snuffle (23 m),
Wander grunt (38 m), Retreat grunt (42 m). Squared distance fade, bounded randomized
gain/pitch/variant, individual cooldowns and 2.5–4.5 s herd gap; round-robin fairness.
Dormant, invalid and inaudible candidates skipped. Caller must pass available shared
wildlife slot (false while muted/busy); no new voice budget or playback yet. Zero or
invalid time cannot trigger even an overdue call; no missed-call backlog replay.

CoreTests exercise ten-minute deterministic multi-animal scheduling/behavior/fairness,
cooldowns, variants/limits, muted/busy slot, distance attenuation, invalid data, dormant
and empty population, pause and delayed-slot return. Build/test logs:
`build/boar-audio-scheduler-build.log`, `build/boar-audio-scheduler-tests.log`.
Native core build and IslandCoreTests pass (63.26 s); diff whitespace check passes.
No renderer/emulator was launched. This does not qualify clips or runtime audio.
NEXT: create distinct original mono breath/snuffle/grunt clips with variants, connect
to the existing pooled Soundscape wildlife voice without starving birds/monkeys,
track the moving emitting animal, expose diagnostics and export/listen to previews,
then actually qualify gameplay audio/mute/lifecycle. Do not claim audible boars or
natural sound quality from scheduler tests. PLAN task stays unchecked.

## Latest — actual Web vertical flight verified

Contained Chrome 155 / SwiftShader `20261011T172804.191069Z-chrome-flight` reports
passed in parent run.json and browser report; kernel-final.json is empty. Actual
Space/Shift-E/mouse-pointer-lock/Q/Space sequence records 47 flight samples: 1.7521 m
activation, 99.999 m ceiling held for four additional samples, 59.7995 m descent,
safe ground return at camera Y 2.62923. Inspected actual ceiling, landed and
post-60-interval screenshots: correct aerial/grounded views, sparse upper slopes and
grass-density transitions still visible. GL 0/context intact; not hardware FPS QA.

Added contained `flight` browser scope, real input controller and independent CPU
telemetry validator with negative fixtures, integrated in BrowserSoftwareTests (16).
Final review additionally rejects non-finite/below-sea landed camera heights; this
only strengthens evidence checks, not gameplay. Existing recorded run is revalidated.
No new build or renderer needed for that validator-only change.

NEXT: actual horizontal ridge/radius/offshore-return flight and Android runtime,
focus-loss/audio checks, then visible island/wildlife work. Full goal remains active;
all prior animal, hut, resident, boar sounds and wreck tasks remain open as recorded.
No Android/KVM/dependency changes or push. Details: docs/flight-controls.md.

## Latest — bounded Space flight implemented and native/portable builds verified

Added FlightMotion and live Space toggle, WASD horizontal/Q-E vertical/held Shift fast.
12/36 m/s, normalized 3D direction, 0.1 s dt clamp, 330 m radial boundary (nominal 250+80),
eye >=1.72 m above rendered terrain and sea, <=100 m above rendered terrain including
seabed. Segment clipping visits every grid and diagonal crossing (piecewise-affine gap),
with 1 mm numerical margin, not endpoint-only collision. Lazy height cache on heap.
Landing uses PlayerMotion placement/depenetration plus dry ground, slope/obstacle/rendered
surface checks; bounded 10 m fallback grid from offshore; refuse disabling if all blocked.
F3 remains separate, debug teleports cancel flight, walking pace persistence unchanged.
Camera bob, underwater overlay/ripple source and movement audio suppressed while flying;
listener/shore ambience follow camera. No new touch flight button; keyboard Space/Q/E.

39/39 non-display groups passed (80.93 s), initial native live run
`20261011T171103.599867Z` passed ascent/ceiling/descent/landing; aerial/land PNGs inspected.
Second `20261011T171521.041577Z` also passes, with landed WALK notice visible. Its early
flight-notice capture was too early (old frame), so QA now waits three diagnostic samples.
Both kernel deltas empty. Native flight path includes existing fullscreen/focus regression.
Input telemetry format preserved; flight position/above-ground is a separate diagnostic line.

Review found per-axis input clipping bent rotated diagonals; changed to double-intermediate
vector normalization and added heading regression. Focused CPU tests also cover invalid
input, cap/radius, 48x120 paths with 33 interior probes and actual world landing layouts.
Explicit build preset includes the new test. Final direction-corrected native run
`20261011T171850.676505Z` passes, with all three actual PNGs inspected: readable flight
notice, aerial ceiling view and grounded WALK notice. Empty final kernel delta. Focused
CPU rerun passes in 1.08 s. Final Web Release build/package and ARM64 assembleDebug/lintDebug
pass (Android 4 s); all owned jobs are terminal. No Web/Android runtime flight claim.
Logs: `build/flight-final-build.log`, `build/flight-final-tests.log`, then current native
evidence, `build-probe/flight-final-build.log`, `build-probe/flight-final-package.log`,
`build/flight-final-arm64-build.log`. Asset gate 24/75/10/0 and 33 software supervisor
tests pass; stop latch unchanged. No Android/KVM configuration or dependency edits.

NEXT: portable runtime flight and continuous horizontal ridge/boundary review remain open, as do audio,
focus-loss cases and existing F11 edge cases. Aerial audit reveals conspicuous grass-density
transitions and sparse upper slopes; not final island appearance. See docs/flight-controls.md.
Full wildlife/monkey/boar/hut/resident/wreck backlog and boar vocalizations remain intact.

## Latest — native F11 verified in an owned window manager

Added `tools/run_fullscreen_software.py` plus minimal `fullscreen_openbox.xml`, reusing
the existing software/kernel/PID/private-device supervisor and GPU lock. Existing
user-space Openbox only inside private Xwayland/Weston pixman; no installation or host
desktop/KVM/dependency changes. Worker refuses unsupervised launch. Geometry/launch
CPU regressions added: 33/33 capture/supervisor tests pass.

Final `build/software-fullscreen/20261011T165736.522833Z` passes two F11 on/off cycles,
actual EWMH state + whole private-display coverage, held repeat suppression, original
1280x720/(561,326) restoration, and background-key isolation during focus transfer to
an owned dialog and back. W captures and Escape releases mouse afterward. Fullscreen
and windowed PNGs actually inspected; no new rendering loss. Empty final kernel delta,
stop latch unchanged, asset gate 24/75/10/0. All owned jobs terminal. Prior setup failures
and narrower successful run retained/described in `docs/fullscreen-controls.md`.

The native window is non-resizable; forced 960x540 resize was correctly refused by WM
hints, so arbitrary resizable-window sizes are not qualified. Neither held-F11 focus
loss nor fullscreen while already captured is tested. NEXT: finish those input edge
cases and Web Escape/pointer-lock/focus checks, then bounded Space flight and the
larger visual/wildlife backlog. F11 remains partial; overall goal stays active.

## Earlier — F11 fullscreen and monkey debug shortcut remap

F11 now requests a native desktop graphics fullscreen toggle once per press; Android
keeps immersive mode. Web uses shared linked `tools/browser_fullscreen.js` directly in
trusted key events, rejecting repeat/pending duplicates, observing actual DOM state,
and logging policy/permission errors. Both release shell and QA use the same pre-JS.
Monkey debug teleport is J instead of F11; docs and overlay updated. Added CPU event,
held-key, pending, blur, exit and denial tests plus contained Web `fullscreen` scope.
Native build and 38/38 non-display groups pass (71.33 s), browser supervisor tests 15/15.
Web Release build/package passes (11,069,478 raw / 4,163,284 gzip bytes). Desktop-only
member needed a platform guard to pass strict Web compilation. First runtime
`20261011T164526.828216Z-chrome-fullscreen` failed on a diagnostic-event race after
actual DOM transitions; final `20261011T164625.592602Z-chrome-fullscreen` passes after
requiring both actual state and its asynchronous confirmation (same four transitions).
Two on/off cycles, held repeat suppression, 60 intervals, GL 0/context intact, empty
kernel delta. Fullscreen and restored-window 1280x720 PNGs actually inspected, no new
blank/cropped scene. Fixed emulated viewport, not arbitrary resolution or hardware QA.
Asset gate 24/75/10/0, latch unchanged, all owned jobs terminal. See
`docs/fullscreen-controls.md`; no Android/KVM/dependency changes or remote push.
Native window restoration/focus/pointer capture still need actual contained input QA.
NEXT: complete native actual F11/window-size restoration/focus/capture verification and
Web Escape/pointer-lock/focus interaction, then bounded Space flight and visual backlog.
F11 task remains partial; Space flight and all prior backlog remain open.

## New user task — pig/boar sounds (2026-10-11)

Added an unchecked PLAN.md task for natural pig/wild-boar grunts, snuffles and breathing
matched to behavior, positional/distance-aware audio, varied timing/cooldowns and bounded
voices/volume. Require asset provenance, mute/lifecycle compatibility and actual in-game
listening on supported platforms. This is a plan addition, not implemented audio.

## User priorities — fullscreen and bounded flight (2026-10-11)

Added unchecked PLAN.md tasks: F11 toggles fullscreen; Space toggles fly mode.
Resolve the existing F11 monkey debug-teleport binding. Flight must never enter terrain,
must stay at most 100 metres above local terrain (working interpretation), and must
remain within a reasonable finite offshore boundary; exact allowance is still to be
chosen. Require safe return to walking, input/help updates and actual input validation.
These are plan additions only, not implemented controls.

## Latest — attachment preparation also split into bounded chunks

Live preparation now checks all 1,502 attachment samples before the existing 1,951
excursion samples, at most 16 per candidate and 32 across the troop per update.
PrepareRange returns a provisional plan only; activation requires every check. Full
Prepare/IsReachable remain complete wrappers for offline callers. Tests compare ranged
and full decisions across 64 natural candidates (same four accepted), exact contact
coordinates, all sample coverage, and at least 215 pending updates before activation.
Measured Debug chunk maxima: attachment 0.651717 ms, excursion 1.61227 ms; these are
shared-host observations, not production frame-time guarantees.

37/37 non-display groups pass in 85.80 s (`build/monkey-site-incremental-final-tests.log`).
Native `20261011T162645.838705Z-monkey-climb` passes all eight stages; summit and sequence
were inspected, with no new visible regression and far contacts still occluded.
Web Release build/package succeeds (11,068,385 raw / 4,163,056 gzip bytes); default ARM64
assembleDebug/lintDebug succeeds. Logs: `build-probe/monkey-preflight-portable-build.log`,
`build-probe/monkey-preflight-portable-package.log`, `build/monkey-preflight-arm64-build.log`.
Current Android runtime has not been rechecked after the scheduling/cache changes.
Web runtime `20261011T163407.734425Z-chrome-monkey-climb` passes all eight cycle stages,
held summit and 60 simulation intervals, GL 0/context intact, empty kernel delta. Both
actual summit PNGs inspected: connected fur-covered body and near-side bark grips remain;
coarse joints, stiff tail and occluded far contacts are not accepted as finished realism.
Added invalid range regressions (negative, empty, overflowing); focused Core passes
65.54 s (`build/monkey-site-range-bounds-tests.log`) after correcting a test-only strict
compiler warning about copied structured bindings. Asset gate 24/75/10/0 passes,
stop-latch SHA unchanged, all owned jobs finished. No Android/KVM/dependency changes.
Continue portable runtime verification, continuous/all-angle motion and face/joint/tail
realism, then wider island backlog. Goal remains active; no overall visual acceptance.

## Earlier — preflight hitch measured; exact cache and bounded full-cycle chunks

CPU Core fixture now prints attachment/whole-clearance mean/max and first-fixture chunk
maximum. Live-simulation fixture accepts 4/64 sites consistently before/after (older
21/64 was pre-live behavior). Baseline Debug whole clearance 249.233/255.921 ms mean/max,
attachment 1.54248/24.5363 ms: `build/monkey-preflight-baseline-tests.log`.
CanopyProfile surface query now retains ring tables and lazily caches exact local vertices
by variant/LOD/optional wind time, thread-local, no quantization or geometry changes.
Cache-only whole mean/max 139.585/155.632 ms, attachment .931476/16.0059 ms:
`build/monkey-preflight-cache-tests.log`. Shared-host Debug timings vary; no Release/FPS claim.

Full excursion clearance now exposes ValidateRange; Validate still covers all 1,951
samples. Agent holds an immutable pending plan and planted inspection while validating
16 poses per update, with troop-wide maximum 32. Cannot activate until all samples pass.
Player proximity cancels preparation on the ground. EnterState/world replacement clears
pending state. Near supporting-tree LOD retained while preparing. No worker threads or
wall-clock-dependent decisions. Multi-candidate queues can lengthen inspection dwell.
Tests assert complete range coverage, <=32 samples/update, no early activation and >=122
preparation updates before climb. Measured 16-pose chunk max 1.42561 ms; remaining
synchronous attachment prep max 21.8729 ms (`build/monkey-preflight-incremental-tests.log`).

Final 37/37 non-display groups pass (69.89 s), `build/monkey-preflight-final-tests.log`.
Native `20261011T161831.733794Z-monkey-climb` passes all eight gates; summit/full sequence
inspected, complete cycle retained, no visible body/contact regression, far contacts
occluded. Kernel delta empty, stop latch unchanged, asset gate 24/75/10/0, all jobs finished.
Current Web/Android evidence below predates this cache/scheduling change.

NEXT: split the remaining synchronous MonkeyAttachmentSite preparation (up to ~22 ms
Debug) without discarding its 1,001 reach and 501 fixed-wind clearance samples; preserve
full-cycle 1,951 dynamic-wind checks too. Then portable cache/scheduling build/runtime
rechecks and Release profiling. Continue motion/all-angle/interruption visuals and
face/joints/tail realism plus wider island backlog. Goal remains active, not complete.

## Earlier — current WebGL climbing qualified and unobscured images inspected

Added contained Chrome `monkey-climb` scope, QA HTML mapping and parent-side full-cycle
validation using the shared native/Android validator. Wait for eight stages/held summit,
capture, then unchanged 60 intervals / GL 0 / intact context / material gates and final
capture. Ordinary `monkeys` walking scope unchanged. 14 browser CPU tests + Node syntax pass.

Existing Release Web build/package refreshed: 11,062,826 raw / 4,161,779 gzip bytes.
Logs `build-probe/monkey-climb-web-build.log`, `build-probe/monkey-climb-web-package.log`.
Prepared fur SHA dc65039f7c3875ec2f33d930afc895b47f0443dc82b84c75fa7a056b8f0293ed.
First `20261011T160143.966968Z-chrome-monkey-climb` failed after seven healthy stages at
40-s wait; terminal failure retained, bounded wait extended to 80 s (no gate removal).
Second `20261011T160312.460229Z-chrome-monkey-climb` passed functional checks but actual
image inspection found the debug panel obscured the monkey: NOT visually accepted.
QA launch now omits overlay, regression guards that mapping.

FINAL `20261011T160632.181230Z-chrome-monkey-climb`: Chrome/155.0.8059.39, host PID 239924,
contained SwiftShader, full cycle + held summit passes, 60 intervals, GL 0, context intact,
empty final kernel delta. Both actual 1280x720 PNGs inspected before/after intervals:
connected body, fur, head correction and near-side grips match native/Android. Far-side
contacts occluded; these are held-summit images, NOT continuous/all-angle motion proof.
Evidence directory contains report, run metadata with eight parsed stages, both PNGs,
artifact/harness attribution and kernel logs. Stop latch unchanged, all jobs finished,
asset gate 24/75/10/0. No dependencies, KVM or emulator configuration changed.

NEXT: profile synchronous attachment/excursion preflight cost (possible main-thread
spikes), audit continuous/all-angle climbing and interruptions, improve face/joint/tail
realism, and continue wider island backlog (including F11/fly, wildlife, hut, resident,
boars/wreck). The goal and full visual-quality requirements remain open.

## Earlier — Android GLES climbing qualified, default ARM64 restored

Stage diagnostics now emit without a desktop capture filename and mirror to Android.
At frame 36, a held-summit marker follows all eight cycle stages. For `monkey-climb`,
Android readiness must wait for this marker; Python validates the full native stage
invariants plus held summit. Negative fixtures reject missing cycle, missing summit
and wrong phase. Unchanged pixel thresholds and gates for other views.

Android x86_64 build/lint passes (6 s). Existing isolated KVM/SwiftShader emulator run
`20261011T155424.149773Z-monkey-climb` passes, PID 1874 / render TID 1915, GLES 3.
All eight stage log checks pass; actual 1280x720 summit PNG inspected, first presentation
attempt. Matches native fur/body/head orientation and near-side bark contacts; far-side
contacts occluded. Touch controls visible but untested. This is staged diagnostic plus
one inspected summit evidence, NOT eight Android images/continuous motion/lifecycle/
physical performance. Startup first draw 5601.262 ms / total 22765.587 ms includes QA.
Final kernel delta `kernel-final.json` is empty; all owned jobs/emulator finished.
No shared adb, KVM, emulator configuration or dependency edits.

Retained `build/android-software/island-live-climb-x86_64.apk`, 21,251,403 bytes,
SHA fc3c575c2b8d3461bf55faefcc7ffa44d6310ff3fd02847d05be4c4e577a8ed4.
Default ARM64 build/lint restored (6 s), 21,313,652 bytes,
SHA 3434ee4d48a3db55f83a0b59a0ad30fce10fe78812ef2b11ac56959247b327d5;
three ARM64 libraries + prepared fur verified, runtime untested.
Logs `build/monkey-live-climb-android-build.log`, `build/monkey-live-climb-arm64-build.log`.
Native diagnostic-change rebuild + 37/37 non-display groups pass (62.97 s),
`build/monkey-climb-platform-markers-tests.log`. 22 Android/31 capture unit tests pass.
Asset gate 24/75/10/0, GPU stop latch unchanged, all jobs finished.

NEXT: current Web climbing build and actual-browser qualification (new `monkey-climb`
scope needs harness support), then continuous/all-angle/interruption visuals, synchronous
preflight frame-cost profiling, monkey face/joints/tail realism and wider island backlog.
Goal remains active. Do not mark visual quality or the full plan complete.

## Earlier — LIVE climbing integrated and actual native whole-cycle images inspected

Ordinary troop now attempts a validated excursion once after 3.2 s of natural inspection
at full simulation distance. Immutable shared plan + reversible clock, original ground
origin retained. Player proximity requests descent/reversal; after return, 2.8 s of
ordinary torso lowering/hand release precedes wandering/fleeing. Canopy/obstacle setters
reject replacement while any monkey is attached/releasing rather than delete supports.
Renderer uses wind-retargeted limbs/normals, torso rotation AND hip translation; culling/
lighting and ground-shadow cache/position follow body centre, ground shadow fades aloft.
Supporting tree stays near LOD through release. Added bounded head counter-pitch after
actual first-capture audit found skyward gaze. Shared terrain cache: 232,324 bytes/troop
(copies share it); preflight is synchronous and needs frame-cost profiling.

`monkey-climb` is a new native CLI/QA viewpoint: naturally reached subject, eight stages
of one replayed excursion, held summit final reference. Existing `monkey-tree` explicitly
disables climb to retain its independent inspection-only gate; normal gameplay enables it.
New harness requires expected phase times, same subject/tree, >1.5 m ascent, returned
height and completed release, plus unchanged pixel-content gates for every frame.
31 capture CPU tests pass, including six negative full-cycle cases. Live Core checks
require actual climb/summit/determinism, safe player interruption and support-removal refusal.
Full 37/37 non-display groups pass (82.01 s), `build/monkey-live-climb-final-tests.log`.
Final renderer-only head correction rebuild succeeds: `build/monkey-live-climb-head-build.log`.

First native evidence `20261011T154734.814785Z-monkey-climb`: passes/inspected, head too
skyward, retained as pre-correction. FINAL `20261011T154923.604121Z-monkey-climb`: passes
all eight stage gates; original-resolution summit and whole sequence inspected. Subject 3,
tree 187, centre rises 12.7403 -> 15.046 and returns 12.7416 before release. Body connected,
visible near-side grips meet bark; far-side contacts partially occluded. Corrected muzzle
looks toward upcoming grips. Not continuous-motion/all-angle proof or final anatomy
acceptance: mask-like face, obvious joint transitions and stiff tail still need work.
Images `peak.png` and `sequence.png` in both evidence directories. Both kernel deltas empty.
Asset gate 24/75/10/0, stop latch unchanged, all jobs finished; no Android/KVM/dependency edits.

NEXT: current Web/Android build/runtime climbing qualification, continuous/all-angle and
interruption visuals, preflight frame-cost profiling (avoid long main-thread spikes),
then remaining monkey realism and wider island backlog. Do not call the whole goal done.
This is the first live climb implementation, not merely another CPU-only foundation.

## Earlier — whole-cycle clearance, ready for live integration work

`MonkeyExcursionClearance.hpp` samples the existing 39-s wind-retargeted excursion every
.02 s. Checks sole centres/toe corners, four points per upper/lower limb with .055/.035
model-unit radii, and three torso/head spheres. Uses actual rendered terrain, bent-centre
wind-deformed trunk triangle queries and nearby world circular obstacle proxies. Exempts
only the selected tree's matching canopy proxy; returns failure category/time/probe.
All 21 accepted natural attachment sites pass initial wind 3. Negative tests reject an
added obstacle on the bark path, pose/tree shifted .02 m into terrain, and zero frame scale.
This remains finite shape/time sampling, not exact skin/tail or swept collision proof.

Final incremental build and 37/37 non-display groups pass (54.09 s), logs
`build/monkey-excursion-clearance-final-build.log`,
`build/monkey-excursion-clearance-final-tests.log`. Initial focused Core: 34.44 s,
`build/monkey-excursion-clearance-tests.log` reports 21/21 full-cycle clearance.
Asset validation 24/75/10/0; GPU stop latch unchanged. All jobs finished, no graphics run.

NEXT: wire the prepared excursion/clock into actual troop and renderer, not more isolated
pose-only milestones. Preserve grounded origin for frame continuity; update visible
body centre/culling and contact-shadow position/cache while elevated. Keep supporting
tree geometry LOD coherent (JungleRenderer builds canopy instances near line 1657 and
draws near/far later; blindly switching mesh can reject grips). Retain tree identity
and reverse/descend safely on interruption, then finish ordinary grip release before
walking. Actual native whole-cycle captures and visual inspection are required, followed
by Web/Android qualification. Live monkeys still only inspect, not climb. Goal stays active.

## Earlier — wind-aware excursion contacts and unreachable LOD rejection, CPU only

`MonkeyTreeExcursion::Retarget` refreshes initial/raised forehands, final climb grips
and body hip at current wind/mesh, preserving original grounded hind feet and staging.
The four-argument `Sample(plan, seconds, windTime, farMesh)` uses retargeting for attachment
and detachment, current wind directly for climb/rest. No expensive site search per pose.
This is not a collision preflight or runtime integration.

First regression failed three assertions: coarse trunk LOD moved initial palms beyond
reach and the solver silently clamped the requested grip. Retained failed evidence:
`build/monkey-excursion-wind-tests.log`. Retarget now rejects unreachable initial/
raised-torso or climb-entry poses explicitly. Tests independently check whether the
requested initial grips are reachable before expecting retarget acceptance/refusal.
Three wind times/both mesh queries test ground-contact invariance, current bark targets,
actual nonzero wind movement and boundary continuity for valid targets. All 21 accepted
natural sites also check reach/bone lengths over 39 seconds of advancing wind, near mesh.
Sampling is not an all-wind, continuous-collision or cross-LOD transfer proof.

Final build and 37/37 non-display groups pass (38.52 s):
`build/monkey-excursion-wind-final-build.log`,
`build/monkey-excursion-wind-final-tests.log`. Asset gate 24/74/10/0; stop latch unchanged.
All jobs finished; no renderer/emulator launched, no new visual claim.
Next: whole-ascent collision/clearance, then live state/renderer integration. Keep the
supporting tree mesh coherent while attached or qualify a continuous LOD transfer; do
not accept a rejected retarget, freeze hands in space or drop the monkey to a walking
pose. Preserve safe interruption/tree identity and update culling/shadows consistently.
Actual whole-cycle visual/platform audit still required. Goal remains active.

## Earlier — whole excursion and safe-return clock, CPU only

`MonkeyTreeExcursion.hpp` composes attach 10 s / ascend 8 s / rest 3 s / descend 8 s /
detach 10 s, all in the original ground monkey frame. Eight gait cycles travel 2.2*scale
world metres upward with zero endpoint velocity; four bark supports at summit. Descent
retraces the same contacts and detachment returns to the original .22-radian planted
inspection pose, which still needs ordinary torso lowering/hand release before walking.
All 21 accepted natural attachment sites pass full-cycle 0.01-s reach sampling.
Independent tests check stage-boundary roots/joints/soles, bone lengths, support counts,
contact retracing and seven interruption times. The latched return clock reverses
attachment/ascent, skips only identical summit rest, or finishes descent/detachment.
It never directly clears an airborne animal's tree identity. Reversal velocity currently
changes sign immediately; smoothing remains needed.

Final incremental build and 37/37 non-display groups pass (58.77 s), logs
`build/monkey-excursion-final-build.log`, `build/monkey-excursion-final-tests.log`.
Initial focused Core run: 35.69 s, `build/monkey-excursion-tests.log`, 21/21 reachable.
Asset gate 24/74/10/0. GPU stop latch unchanged; all jobs finished; no graphics launched.
Live troop/renderer NOT wired yet. Next: retarget contacts with changing wind/LOD,
validate whole-ascent clearance (current full-cycle check is reach only), then integrate
state/skin/sole/shadow/culling and safe interruptions/tree replacement together. Perform
actual whole-cycle visual/platform qualification, not merely held inspection captures.
Goal and full island backlog remain active.

## Earlier — actual-island attachment-site preparation, NOT live climbing

`MonkeyAttachmentSite.hpp` constructs the attachment trajectory from naturally reached
tree sites, actual rendered terrain heights/normals, regrounded trunk geometry and
obstacles. Checks heading, slope, height, fixed-length reach, sampled contact centres,
sole/toe corners and three torso/head probes. Exempts only the selected canopy proxy,
whose actual bark is checked separately. 21/64 natural arrivals pass; 16 reject initial
reach, 2 height and 25 trajectory reach. Added obstacle at a staged foot rejects entry;
stale layout tree Y does not affect grounded output. Tests also cover heading/scale guards.

Final revision including the sole-frame 1e-8 fallback threshold rebuilt successfully;
37/37 non-display tests pass (57.77 s), logs
`build/monkey-attachment-site-verified-build.log` and
`build/monkey-attachment-site-verified-tests.log`. Asset validation 24/73/10/0;
GPU stop latch unchanged. All launched jobs finished; no renderer/emulator launched.
This is fixed-wind near-mesh CPU preparation, not continuous/full mesh collision or
visual qualification. Next wire safe attachment/ascent/rest/descent/detach into troop
and renderer together, preserving tree identity, coherent wind contacts and safe
interruption. Live monkeys still only inspect trees. Wider tasks and goal stay open.

## User priorities — boar and shipwreck visual rework (2026-10-11)

Added explicit unchecked tasks near the top-level wildlife backlog in PLAN.md for better
pig/wild-boar anatomy and bristly coat/skin/snout/hoof textures, and for a more realistic
shipwreck with convincing construction, broken planks, weathered wood and rusty fittings.
The user permits original generated textures where useful. These are planning additions,
not implemented improvements. Require actual close/wide in-game visual review on native,
Android and Web; existing build/material diagnostics do not close either task. Keep the
previous monkey, additional wildlife, hut and resident work open.

## Earlier — complete attachment trajectory mathematics, NOT live yet

New `MonkeyTreeAttachment.hpp` bridges live .22-radian torso loading to the existing
vertical `MonkeyClimbPose` frame. Ten segments: move torso nearer, left/right higher grip,
left/right hind-foot ground staging, vertical torso, left/right hind-foot bark transfer,
left/right final climbing grip. Three or four stationary supports, one moving contact at
most, smooth endpoint velocities, .08-model-unit swing clearance, fixed-length spatial IK.
Reverse time traverses the same path for dismount. Caller supplies actual surface points.
`IsReachable` preflights 1,001 samples; rejects clamped/non-finite/invalid-normal poses.
It is not collision/continuous-wind proof. Not yet included by troop or live renderer.

Real-tree fixture failed the original hand-first order at conservative stand-off; retained
`build/monkey-attachment-fixtures.log`. Move torso first now passes all form/LOD coverage:
208 accepted / 80 rejected of 288 tree/monkey-scale/wind cases. Independent trajectory
tests check fixed contacts, at least three supports and lengths; end roots/contacts match
climbing solver, entry roots/joints/contacts match the current live torso-loading pose.
Real-tree staging ground is flat in these fixtures; yaw/slope/obstacles/body clearance
are not yet qualified. Full 37/37 groups pass (48.43 s) plus final targeted entry-continuity
test. Asset gate 24/72/10/0, no runtime/graphics change or new capture claimed; stop latch
unchanged. All jobs finished, no dependency/host/emulator edits.

Next integration must sample/validate actual staging terrain and collision, preserve tree
identity through attach/ascent/rest/descent/detach, handle interruptions without dropping
the animal, apply body frame to skin/culling/shadows and track wind-aware grips coherently.
Then visually audit the real whole cycle, not only CPU reach or a held pose. Live monkeys
still stop at the earlier inspection. Goal and wider island tasks remain active.

## Earlier — current WebGL monkey model/material qualification

Existing Release Web build at f7f2801 succeeds and deployment package is refreshed:
11,010,318 raw / 4,139,727 gzip bytes. Prepared fur SHA matches native
`dc65039f7c3875ec2f33d930afc895b47f0443dc82b84c75fa7a056b8f0293ed`.
Contained Chrome/SwiftShader `20261011T143701.925390Z-chrome-monkeys` passes unchanged
focused 60-interval gate, GL error 0, context intact, kernel final delta empty. All six
images (initial, four poses, final) inspected. Fur/collars present, no disintegrated limb
mesh in sampled motion; grass hides feet in some frames. Mask-like face, visible shoulder
junction and stiff tail remain. This qualifies ordinary current WebGL geometry/material,
not every foot contact/frame, full climbing, tree-transfer poses, Firefox or real GPU.
Metadata retains package/harness hashes. All jobs finished, stop latch unchanged; no
threshold/dependency/host/emulator changes. See `docs/monkey-limb-skin.md`.
Goal remains active; next implementation must address remaining monkey behavior/realism
and the wider wildlife/hut/resident/boar/wreck plan, not treat this gate as completion.

## Earlier — narrower body-anchored limb collars, native inspected

`MonkeyLimbProfile.hpp` defines ten sleeve rings, proximal radius .040 embedded inward
and upward, maximum .083 further down the limb. First ring follows Spine/Pelvis, next
blends to upper bone; distal rings retain upper/lower weights. At most two influences,
14 bones, unchanged sole contacts/shader/draw count. +44 vertices/80 triangles near,
+28 vertices/48 triangles far per silhouette. Taper-aware normals and orthonormal rings.

First capture `20261011T142749.956942Z-monkey-tree` passed gates but exposed an angular
collar rim; corrected after actual image inspection, retained as rejected evidence.
Final `20261011T143018.362367Z-monkey-tree` passes all eight gates; grounded/peak images
inspected, exposed rim hidden, upper arm less bulky. Walking
`20261011T143134.265841Z-monkeys` also passes/inspected. All kernel deltas empty.
Final native build and 37/37 non-display tests pass (60.06 s), asset gate 24/71/10/0;
stop latch unchanged, all jobs finished, no dependency/host/emulator edits.
This remains overlapping geometry with a visible shoulder junction, not completed
anatomical realism. Android/Web of these new collars unqualified; last Android below
still qualifies only the previous geometry. Full climbing, all-angle and continuous
motion checks remain open. See `docs/monkey-limb-skin.md`. Goal remains active.

## Earlier — shorter fur source and angular side mapping, native + Android inspected

Built-in ImageGen source v2 is preserved with exact prompt and provenance under
`assets/art-studies/monkey-fur-albedo-v2*`; v1 untouched. First replacement attempt was
rejected for long strands. CMake/Android now prepare v2 at the existing 512-square path.
`MonkeyFurUv` replaces collapsing cosine side projection with angular mirrored interior
UVs on body/head/limbs/tail; no texture repetition or shader/budget change. CPU density,
closure and range checks added. Native build and 37/37 non-display groups pass (48.27 s).
Asset gate passes 24 records / 70 generators / 10 project files / zero external assets.

Native `20261011T141731.945186Z-monkey-tree` and `20261011T141831.862271Z-monkeys`
pass, peak/walking images inspected; shorter finer coat instead of broad wood-like streaks.
Android x86_64 build/lint passes in 10 s; isolated GLES
`20261011T142013.159547Z-monkeys` passes first presentation attempt and image inspected.
All three kernel deltas empty. Tested APK retained as
`build/android-software/island-monkey-fur-v2-x86_64.apk` (21,158,731 bytes), SHA-256
`fa80bf806c7540f9fafc71a010b38909d5730ab4e7f5ba92d09eeef7f0abde2b`.
Default ARM64 build/lint restored (9 s), 21,233,236-byte APK SHA-256
`d6078c5ef061eb3127e2c7193758f8ef835a230ba37a05bcc7740970c12cb0aa`; ARM64 runtime untested.
All launched jobs finished, stop latch unchanged, no dependency/host/KVM configuration edits.
This qualifies current ordinary model/material on software GLES, not tree-transfer poses,
lifecycle, physical-device performance or continuous animation. Web remains outstanding.
Coarse shoulders, simple face/silhouette and stiff tail remain; hind-foot bark transfer and
ascent/rest/descent/dismount are still missing. Goal and all wider island tasks stay active.

## Earlier — initial torso lift while four contacts remain planted

Live inspection now raises the torso .22 radians about the hind hips only while both
forehands grip bark, returning before hand release. Fixed-target IK preserves all four
contacts; full-excursion reach is prechecked. Hind feet stay on terrain: full attachment,
ascent/rest/descent/dismount are still missing. No shader/bone budget change.
Peak capture stage moved from >=2.7 to >=3.3 seconds. Final native evidence
`20261011T141011.681705Z-monkey-tree` passes all eight image/contact gates, kernel delta
empty; peak and return images inspected. Peak diagnostic angle .22, left shoulder Y
.702676 and palm Y .604693. Earlier partial-lift capture `20261011T140816.894255Z-monkey-tree`
also passes/inspected, empty kernel delta. Fur/shoulder/face/tail defects remain.
See contact-foundation doc for CPU fixtures and the corrected float-boundary test failure.
Final native build and 37/37 non-display groups pass (60.44 s,
`build/monkey-torso-loading-final-tests.log`); direct 30 capture-unit tests pass. All jobs
finished, GPU stop latch hash unchanged, no dependency or emulator configuration edits.
Android/Web remain unqualified for these changes. Goal remains active.

## Earlier — eight-stage native tree-transfer visual audit

The source changes add eight sampled states from the same naturally reached
inspection to native `monkey-tree` capture, plus strict sequence/contact-weight validation.
See `docs/monkey-climb-contact-foundation.md` for capture semantics and limitations.
`20261011T135905.681863Z-monkey-tree` passes; all eight stage images and final reference
were inspected, kernel delta empty. Native build and 37/37 non-display tests pass (78.35 s).
Sequential hand transfer/return is visible; hind feet remain grounded. This is not full
climbing or continuous-frame qualification. Bulky shoulders, wooden-looking fur, flat
face and stiff tail remain. Android/Web qualification is still outstanding. Next implement
body attachment with mixed ground/bark supports, then ascent/rest/descent/dismount; retain
the larger wildlife, hut, resident, boar and wreck tasks. Goal remains active.

## Earlier — reaching elbow pole fixed after actual-pose diagnosis

`MonkeyTreeReach::BendHint(weight)` continuously changes the ground rearward bend direction
to downward during a forehand transfer. Retaining the ground pole had pitched elbows above
the shoulders when palms moved forward. Added a bounded first-contact root/joint/palm
diagnostic to MonkeyRenderer; no shader/bone-budget changes or dependency edits.
Native pre-fix `20261011T134622.185496Z-monkey-tree` records left root Y .567373,
elbow Y .779067, palm Y .604692. Final `20261011T135100.198476Z-monkey-tree` has elbow
Y .391113 with identical root/palm coordinates; image compared, backwards angular forearm
protrusion gone and hands retain bark contacts. Both kernel deltas empty; latch unchanged.
Shoulder blend remains bulky; wood-like fur and flat face still defective.

Native build and all 37 non-display groups pass (60.58 s). New 202-pose transfer regression
requires reachable, downward-bending elbows through the blend. All jobs finished. No
Android/Web runtime qualification, no full moving-sequence audit claimed: these are held
before/after poses. Next: capture the actual sequential hand transfers (same naturally
reached subject/tree, including approach/return), then raise/rotate the body with mixed
ground/bark supports and implement ascent/rest/descent/dismount. Full goal remains active.

## Latest — live forehand-to-bark transfer, native held contact inspected

`MonkeyTreeReach.hpp` provides closer stand-off, sequential forehand transfer weights and
model-space bark targets. `MonkeyTroop` prechecks nominal forehand reach, approaches within
4 cm, stores selected tree and inspection timers. Six-second `InspectTree`: settle, left
then right hand onto trunk, hold, sequentially return. Hind feet remain on terrain. Live
renderer checks actual terrain-fitted reach and applies spatial IK/palm normals only if
both full targets are reachable. No body raising, attachment/climbing/descent yet.

IMPORTANT discovered correction: JungleRenderer uses `GroundJunglePlacement`, overriding
layout Y with rendered-triangle terrain height. Hand queries now use that same grounding,
same draw-time wind and shared ordinary canopy LOD threshold. Special forced canopy-LOD
override is not contact-qualified. `MonkeyClimbPose` full-body model remains CPU-only.

New native `monkey-tree` QA viewpoint warms ordinary simulation (bounded 12000 steps),
selects a naturally inspecting troop member at >=2.7 seconds into inspection, then holds
troop state while wind stays live. Captures must show the selected subject with reachable
paired grips, not just another visible animal. First `20261011T133719.762250Z-monkey-tree`
failed because member zero never reached a suitable tree in the bounded warm-up; retained,
no image, empty kernel delta. Selection of any naturally inspecting member fixed QA setup.
Final `20261011T133928.741268Z-monkey-tree` passes/inspected: both forepalms meet bark,
hind feet remain on terrain (some grass occlusion), head outside trunk. Shoulder/elbow
blockiness, wood-like fur and flat face remain defects. This is a HELD pose, not a verified
moving transfer or full climbing sequence. Regular `monkeys` view remains moving.

Native build and all 37 non-display groups pass (57.42 s), plus direct 29 capture-unit tests.
Asset gate: 24 records / 69 generators / 8 project assets. All jobs finished, stop latch
unchanged; no dependency/host/emulator configuration changes. Android/Web NOT requalified.
Next: visual transfer sequence and body attachment with mixed ground/bark support, then
actual bounded ascent/rest/descent/dismount; improve exposed limb joins and fur/face too.
Full goal remains active; wildlife/hut/resident and entire visual audit remain unfinished.

## Latest — spatial limb solver and CPU bark-grip pose model

`MonkeyGroundGait::SolveLimb` handles arbitrary 3D targets, fixed .31/.33 bone lengths,
bend hints and explicit reach clamping. Ground fitting uses it. Renderer segment rotations
now use a general rigid shortest arc (`MonkeySoleFrame.hpp`), not X-axis-only rotations;
palm frames handle forward-parallel bark normals. Bone/uniform budgets are unchanged.

New `MonkeyClimbPose.hpp` (asset manifest registered) produces local limb poses plus world
contacts/normals/origin/frame from actual tree transforms and wind-deformed trunk triangles.
Model +Z points upward, +Y outward, +X circumferential. Root height offset -.08; stride .22,
support fraction .8, phase advances with signed vertical travel / (.275 * monkeyScale).
At least three supports; supporting tree coordinates stay fixed while root moves. Reach
clamping is diagnostic, NOT contact success. This model is only tested, NOT used by live
monkeys yet. Current live behavior remains ground approach/inspection then wandering.

Native build and all 37 non-display groups pass (60.58 s). 46,080 climbing fixtures cover
two heights (straight/bent), eight forms, both LODs, yaw, independent tree/monkey scales;
fixed lengths, target reconstruction, bark clearance and support locking pass. Segment
opposition/palm singularity tests pass. Native `20261011T132740.781674Z-monkeys` passes and
was inspected: retained ground quadruped geometry, grass partly masks contacts, existing
wood-like fur/flat face/shoulder defects remain. Empty kernel delta, stop latch unchanged.
No current Android/Web qualification; all jobs finished, no dependency/host/emulator changes.

Next must solve attachment: the present staging stand-off is too far for a forehand grip.
Use a collision-checked closer stance, forehands on bark while hind feet stay on terrain,
then raise/rotate torso and transfer hind contacts. Do not simply elevate the whole animal.
Need reachable mixed-support IK, body/root-flare clearance, reverse dismount and actual
visual motion evidence before claiming climbing. See `docs/monkey-climb-contact-foundation.md`.
Full goal/plan stays active and incomplete.

## Latest — actual canopy selection and ground approach (not attachment/climbing)

`MonkeyTroop::SetCanopy` now receives actual canopy placements from `IslandGame` after
obstacles. Periodic wander decisions select a suitable nearby tree and a near-side staging
point outside its flared root plus body clearance. Full segment/proxy clearance and sampled
terrain qualify candidate paths. Every actual movement step now also checks swept circular
collision, without exempting the selected tree. `ApproachTree` slows while turning and
stops within 12 cm; `InspectTree` faces the tree, pauses, then returns to wandering for now.
Timeouts/blocked steps abandon attempts; player proximity triggers escape. Replacing canopy
or obstacles cancels stale tree selection. No attachment/ascent/descent or limb grip exists.

The two repeated real-island test troops now use canopy placements and require actual tree
arrival, matching identities, stand-off, interruption and replacement cancellation. Existing
terrain/home/behavior coverage remains. Native build passes. First approach-only core test
passed (35.49 s); see `docs/monkey-climb-contact-foundation.md` for final evidence.
Native `20261011T131911.555963Z-monkeys` passes/inspected, empty kernel delta; this shows
early ground locomotion, NOT visual approach/arrival. Coarse fur/face/shoulder defects remain.
Android/Web current qualification and complete climbing remain open. Next: attachment/body
rotation and reachable bark grips, then bounded climbing/return with visual motion evidence.
Final build and all 37 non-display groups pass (85.59 s); all jobs finished. Stop latch
unchanged, no dependency/host/emulator configuration changes. Full goal remains active.

## Latest — shared trunk contact geometry, including wind (climbing not connected yet)

`docs/monkey-climb-contact-foundation.md` records the new shared near/far trunk vertices,
ring layout and barycentric triangle contact/normal query. Renderer consumes the same
rest vertices. Optional wind time deforms vertices before interpolation with the rendering
wind equation. Coordinates are mesh-local; tree instance transforms and actual LOD must
be supplied by future callers. Branches/buttresses are not covered. No monkey uses the
query yet: actual approach, attachment, reachable limb poses, ascent/descent and arboreal
movement remain unimplemented, as do moving ground stance locks.

Native build passes. Rest-surface suite passed 37/37 (58.79 s); expanded wind tests cover
all eight variants/both LODs, both triangles in each cell, three wind times, normals,
periodicity, invalid inputs and independent shader-equation fixtures. Native jungle
captures `20261011T130518.614818Z-jungle` and `20261011T131221.540034Z-jungle` pass and
were inspected; empty kernel deltas. These are renderer integration checks, not climbing
evidence. No dependency/host/emulator changes. Android/Web qualification remains open.
Final wind-enabled build and all 37 non-display groups pass (102.14 s). All jobs finished;
hardware stop latch unchanged. The full plan/goal remains active and incomplete.
Next integration: `MonkeyTroop` currently receives only circular collision proxies via
`SetObstacles` (`IslandGame.cpp`), not tree identities/transforms. Supply actual canopy
placements before selecting approach targets; do not infer climbable trees from generic
obstacle circles. `MonkeyBehavior` still has only ground states. Preserve the existing
distance-driven ground gait while adding explicit approach/attach/climb/dismount states.

## Latest — connected monkey face replaces the two pale oval patches (native first pass)

`docs/monkey-face-profile.md` records scope/evidence. One twelve-row cheek/muzzle/chin surface
replaces separate orbital/muzzle ellipsoids; smaller eyes/nostrils, attached curved mouth
strip and muted skin palettes. Same head bone/draw structure. New profile included in asset
manifest (67 sources), CPU profile regression added. Native build and all 37 non-display
groups pass (93.53 s). `20261011T125510.820617Z-monkeys` passes/inspected, empty kernel delta.
Two bright oval patches no longer dominate the face, but it is still simple/low-relief and
needs sockets/lids/cheek transitions. Coarse fur and shoulder attachment remain defects.
All jobs finished, no dependency/host/emulator changes. Android/Web have NOT qualified this
face revision; Web also still lacks current contact/tail/camera qualification. Continue those
checks and actual stance locking/tree gripping/climbing. Whole goal/plan remains open.

## Latest — close side QA camera and Android contact shader verified

The `monkeys` camera now views from forward 0.6, side -2.1, height 1.1, target height 0.52.
No vegetation/behaviour hidden or altered. Candidate `123555.508886Z` passed image checks but
was visually rejected because a shrub masked feet; `123834.120032Z` failed image variance.
Both retained. Final native `20261011T124224.778948Z-monkeys` passes/inspected: front palms
clearly visible along slope, rear partly masked. Empty kernel deltas, unchanged image gates.
Native build and four targeted monkey/CLI groups pass. Prior full sole-frame suite was 37/37.

Android x86_64 assembly/lint and `20261011T124529.157229Z-monkeys` PASS: new `uSoles[4]`
shader packaged, side-view image inspected with visible front palms/rear foot. First image
attempt accepted, no shader/GL/reach-limit errors, empty kernel delta. Tested APK retained;
ARM64 assembly/lint restored and ZIP checked. Hashes/scope in `docs/monkey-sole-alignment.md`.
All jobs finished. No dependency/host/KVM/emulator changes. The intermittent earlier black
presentation remains unresolved. Web has NOT qualified current contact shader/tail/camera.
NEXT: focused Web sequence with 600-second bound; face-mask/shoulder/coarse fur refinement,
turning stance locks and actual tree-contact/climbing. Whole goal and wildlife/hut/resident
backlog remain open. The new close camera makes the toy-like face and coarse/wavy fur obvious.

## Latest — independent palm/sole frames (native first pass, portable recheck required)

See `docs/monkey-sole-alignment.md`. Four contact matrices now orient hand/foot mesh to each
rendered ground normal rather than inheriting lower-limb rotation. Blend-index W tags 1–4
select these matrices; zero remains ordinary skeleton skinning. Fourteen skeleton bones
unchanged; conservative vertex uniform budget now 92 vectors. Sole-height compensation
preserves tangent-plane underside clearance. No extra draws/textures/dependency changes.
Native build and all 37 non-display groups pass (81.08 s), including new CNA-math-only sole
frame tests and shader dialect/budget contracts. Native `20261011T123046.817709Z-monkeys`
passes/inspected: visible geometry, but grass masks some digits. Kernel only perf throttle,
no GPU fault. Asset manifest passes with 66 generator sources. All jobs finished.
NEXT: requalify this new contact shader on Android/Web and use unobscured close/moving
contact views; continue stance locking, face/shoulder anatomy and real tree gripping/climbing.
Prior Android/Web passes do NOT qualify these new contact matrices. Whole goal/plan open.

## Latest — current monkey model qualified on Android; first black presentation unresolved

See `docs/monkey-android-current-model.md` for hashes, both runs and scope. Current ca6b042
x86_64 assembly/lint passes. `20261011T121437.574103Z-monkeys` FAILED: six all-black images
in 30 seconds despite fur-load/first-draw markers. Fresh failure-log retention was added and
CLI presentation wait extended to 60 seconds without changing image/GL gates. Identical APK
`20261011T121912.364377Z-monkeys` PASSES on its FIRST screenshot attempt, image inspected:
current quadruped/fur/terrain-target/tail visible. Empty kernel deltas in both. No claim that
the longer wait fixed the failure; cause remains unknown/intermittent. Both runs retained.
Shell and Python startup gates require the fur-load marker; APK hashes all three textures.
22 Android CPU tests and shell syntax pass. Tested x86 APK retained separately; default
ARM64 assembly/lint restored, three ARM64 libraries and three textures verified. No physical
ARM64/lifecycle qualification. All jobs finished; no emulator/KVM/dependency changes.

NEXT: improve toy-mask face/shoulder form, independent palm/sole alignment and stance locks,
then actual tree-contact/climbing; Web current-tail verification is still pending (its last
passing motion series used the prior tail). Keep whole-plan audit/wildlife/hut/resident open.

## Latest — Web anatomy/fur/contact qualified; continuous tail native first pass

`20261011T120413.407382Z-chrome-monkeys` PASSES with a 600-second bound, all sixty
intervals retained: GL error zero, intact context, no app error or reach warning. Start,
four intermediate and final images inspected. Ten unrelated AppArmor kernel audits, no
GPU fault. This qualifies the existing 05c3824 anatomy/fur/contact renderer, NOT the tail
change authored while that browser was running. Older timed-out evidence remains intact.

New `MonkeyTailProfile.hpp` and `AddTail` replace three disconnected rigid tubes with one
smoothly weighted tapered mesh, eighteen near/nine far longitudinal spans, same three tail
bones and same fourteen-bone palette. No new draw calls/textures. Profile/gait CPU test
passes; native build and `20261011T121030.039628Z-monkeys` pass. Image inspected with
continuous taper/silhouette, empty kernel delta. This static native image does not qualify
tail motion or Android/Web. Asset validator passes with 65 generator sources.
All 36 non-display groups pass (79.24 s); all browser/build/capture/test jobs finished.
NEXT: finish current-tail portable qualification, continue face/shoulder realism and full
palm/sole contact, then actual tree-contact/climbing. Android is still the older shader-only
APK, not these anatomy/material/contact revisions. Do not label the monkeys or whole plan
complete. No dependencies, host graphics settings, KVM or emulator configuration changed.

## Latest — Web moving-view audit captured, runtime deadline NOT passed

Current anatomy/fur/contact Web build succeeds; package refreshed (10,927,110 raw /
4,083,870 gzip bytes). Focused monkey runner adds four intermediate images after eight
frames each, plus original endpoints and sixty measured intervals. Syntax and 13 launcher
CPU tests pass. `20261011T115524.501474Z-chrome-monkeys` TERMINATED at the 360-second
supervisor deadline; do NOT call it qualified. Start + four intermediate images inspected:
visible textured quadruped changes position/heading/limb pose; toy-like face mask, angular
tail and assembled shoulder remain explicit defects. No final GL/interval report exists.
Final kernel delta only keyboard registration/perf throttling, no GPU fault. All jobs finished.
No current job to poll/restart. Keep failed evidence unchanged. Newly added partial checkpoint
writing will preserve page state during the next attempt; it always says complete:false.
NEXT: fresh bounded 600-second focused Web run for the longer sequence, retaining all sixty
intervals and graphics gates; then Android current-model qualification and contact/climbing.
See `docs/monkey-quadruped-overhaul.md`. Goal and whole-plan scope remain unchanged/open.

## Latest — individual rendered-terrain targets for monkey limbs (native first pass)

`MonkeyGroundGait::FitGround` now refits all four fixed-length chains to individual
rendered ground heights, preserving swing clearance and lifting/lowering the trunk within
the common reachable interval. Impossible terrain reports reach limitation and preserves
bones instead of stretching. `MonkeyRenderer` samples its cached RenderedTerrainSurface
at each yaw/scale-transformed sole and applies the body offset to the pelvis hierarchy.
Native build and all 36 non-display groups pass (82.60 s). The added real-island test
checks all seven spawn footprints at eight yaws and forty gait phases; separate gait/shader
recheck passes 2/2. Synthetic tests cover slopes and an impossible cliff. Asset gate passes.
Capture `20261011T115107.984389Z-monkeys` passes and was inspected: visible quadruped/fur,
no reach-limit diagnostic, empty kernel delta. Static frame only, not motion acceptance.
All jobs finished. No dependency/host/emulator changes, GPU stop latch unchanged.

Remaining contact work is explicit in `docs/monkey-quadruped-overhaul.md`: hands/feet still
inherit lower-limb orientation; independent palm/sole alignment, turning stance locks,
trunk pitch/roll and silhouette/contact-shadow refinements remain. Actual tree climbing is
still absent. NEXT: moving-sequence and Android/Web qualification of current anatomy/fur/
contact, then continue full monkey contact/climbing work. Whole plan/goal remains open.

## Latest — first real monkey fur material integrated (native inspected)

See `docs/monkey-fur-material.md`. Built-in ImageGen source and exact prompt preserved
in `assets/art-studies/monkey-fur-albedo-v1*`, hashed in manifest and attributed.
512-square build preparation wired into native/Web/Android packaging; runtime ten mips,
required asset, rest-mesh UVs, body-only mean-normalized diffuse modulation. Face/eyes
excluded. Native build and `20261011T114648.821520Z-monkeys` pass; image inspected,
fur variation visible, but face/limb junctions still stylized. Empty kernel delta.
Native gate now requires fur-load marker as well as a close subject at readback;
focused Web gate requires the marker, Android APK attribution includes the third texture.
Native non-display suite passes 36/36 (69.53 s); direct Android/launcher CPU tests pass
21/29 respectively, asset validation passes and Web runner syntax passes. The actual-fur
pixel-oracle test was added after the full suite started; separate material/shader recheck
passes 2/2 (9.98 s), including the actual fur source. All build/test/capture jobs finished.
Portable runtime qualification and movement review remain pending. No emulator/KVM or
dependency changes. Keep the entire goal active; monkey realism, tree climbing, full
visual audit, remaining wildlife, hut and human resident are NOT complete.

## Latest — quadrupedal mesh and distance-driven ground pose are live (native first pass)

`docs/monkey-quadruped-overhaul.md` records the actual scope and deficiencies. Horizontal
torso/neck loft, continuous dual-weight limb skins and smaller face replace the upright model.
`MonkeyState.gaitPhase/gaitAmount` now drive the ground IK from actual travel, not a clock.
Native build and contained `20261011T113355.740100Z-monkeys` pass; image inspected with a
visibly horizontal quadrupedal animal, empty kernel delta. This is NOT complete monkey realism:
fur texture and climbing are still absent; flat-plane sole supports/turning/contact need work.
Native test run: 35/36 pass; the CPU supervisor fixture missed its four-second startup marker.
Unchanged standalone supervisor recheck passes (1.95 s), recorded in
`build/monkey-quadruped-supervisor-recheck.log`; all jobs finished.
No safety gate was weakened. No native/Android/browser renderer remains running.

NEXT: actual documented fur texture and stable UVs/filtering, then per-limb rendered-terrain
support and tree-contact/climbing. Portable anatomy verification is pending; do not reuse
the older Android/Web shader-fix images as qualification for this changed model. The full
island visual audit, wildlife backlog, hut and resident are all still open.

## Latest — monkey overhaul started, NOT yet integrated into the live model

Operator explicitly rejected the toy-like brown primitives and human-like upright walk.
Requirements now recorded in PLAN: coherent primate anatomy, actual fur texture, quadrupedal
locomotion and climbing actual trees with gripping/contact. Do not substitute a colour change
or shader success for these requirements. Full-island visual audit and all unimplemented
wildlife (including snakes, spiders and goats), hut and newly requested human resident remain
open. The active goal is to implement the whole plan, then repeatedly audit and improve.

First implementation foundation: `MonkeyGroundGait.hpp` provides four phased support/return
trajectories and two-link sagittal IK with fixed segment lengths; idle-to-walk amplitude and
support-phase root-travel cancellation. `IslandMonkeyGroundGaitTests` passes (601 phase samples
at three amplitudes, periodicity, two-or-more supports, sole heights and segment lengths).
Native incremental build passes. This helper is NOT connected to MonkeyRenderer yet: the live
model/animation/images are unchanged. No graphics job is running. No new image was generated.

NEXT: replace the upright rest rig and disconnected primitive body with a coherent horizontal
quadrupedal body, attach this pose solver with speed synchronization and terrain support, then
qualify real captures. Integrate a documented fur texture, then implement approach/grip/climb/
rest/descent on actual tree geometry; verify full cycles, not merely a vertical translation.
ImageGen skill was inspected in the preceding conversation but generation has not started;
read/use the skill when working on the raster fur asset. Dependencies remain read-only.

## Latest — 14-bone monkey shader verified on native, Android and Web

Android x86_64 assembly/lint and `20261011T110417.653056Z-monkeys` pass strict graphics
gates; close monkey visible, image inspected, no shader/guest GL errors, empty final kernel.
Tested APK retained separately and default ARM64 assembly/lint restored; exact hashes in
`docs/monkey-palette-workaround.md`. Native also qualified below. Physical ARM64/lifecycle
remain unqualified. Focused Chrome `20261011T110957.242647Z-chrome-monkeys` passes
60 intervals, zero GL error, intact context; before/after images inspected with visible
close geometry and changed pose/location, empty final kernel. Web package refreshed:
10,366,754 raw / 3,534,595 gzip bytes. 13 browser-launcher tests and Node syntax pass.
ALL jobs finished, including Web sessions 51546/70348; do not poll or restart them.
No temporary diagnostic shader/depth/draw substitutions remain. This is a rendering fix,
NOT a realism milestone: the old model is still visibly toy-like. Next: continue existing
content realism (monkey form/material/contact or terrain backlog), retaining the same safety
boundaries. Full browser input/lifecycle, Firefox and Android lifecycle remain separate open work.

## Latest — native monkey visibility restored; Android/Web still pending

Read the latest section of `docs/monkey-palette-workaround.md`, which supersedes earlier
visibility hypotheses below. QA camera now follows subject translation; replacement shader
now converts XNA clip depth to GL, matching terrain. Normal native close monkey is visible
in `20261011T110027.440307Z-monkeys` (inspected, empty kernel delta). Native 35/35 non-display
tests pass in 74.01 s. Temporary magenta/bind-pose/depth-disabled/screen diagnostics removed.
Native harness additionally requires a close-model submission at capture time; final
`20261011T110247.359170Z-monkeys` passes (2 visible / 1 near / 4 draws), inspected,
empty kernel. 29 software-launcher and 21 Android device-free tests pass.
No dependency/host/emulator settings changed, hardware latch unchanged.
Current Android default APK is still the older ARM64 diagnostic build. Next: x86 build/lint,
strict software close capture, retain tested APK and restore ARM64; then Web verification.

## Update — WIP 14-bone shader; native visibility still unresolved

Uncommitted implementation in `MonkeySkinningShader.hpp`, MonkeyRenderer, shader contract test,
CMake/presets and launcher marker updates. Read `docs/monkey-palette-workaround.md` before
continuing. Native build/first 35/35 CPU groups pass, but replacement captures show no monkeys.
Crucially temporary native STOCK comparison is also empty; its bypass was removed. Do not
claim shader acceptance or infer that the replacement alone caused invisibility.
Latest contained probe `20261011T103724.288807Z-monkeys` finished (session `77749`), CPU
skinned pelvis is correctly near subject, camera above terrain. Other build/test/capture
sessions also finished. No jobs live. Current native binary is replacement + bounded probes.
NEXT: investigate vertex/palette binding or occlusion on native before Android qualification.
Android default remains the restored ARM64 diagnostic build, not the new monkey shader.

## Update — confirmed Android missing-monkey shader failure

Diagnostic monkey run `20261011T101527.286817Z-monkeys` FINISHED (session `51286`).
New stderr mirror captured stock `skinned (vertex-lit) link failed`, active vertex uniforms
exceed 256; the inspected image has no monkeys. Old run.json says passed, but that is a
documented FALSE POSITIVE, not acceptance; preserve it unchanged. Empty kernel delta.
`CNA-ISLAND-002` is now confirmed in `BUGS.md`. Dependencies remain read-only.
Harness now rejects stock VS/FS/link and nonzero guest GL errors at all four checkpoints;
21 Android CPU tests and shell syntax pass. Existing swimming fixture delimiter updated.
Diagnostic x86 APK preserved separately at `build/android-software/island-diagnostic-x86_64.apk`.
Default ARM64 assembly/lint restored (20,533,364 bytes, SHA-256
`931349af0a37149cee31154c6b4e8409a562eb725abf48963f9d599d94d6e6f2`), physical runtime untested.
No jobs remain running. NEXT: implement consumer-only 14-bone monkey shader workaround,
preserving animation, material, canopy shade and fog; qualify close geometry/motion on all
three platforms. Current stock shader declares 72 mat4 bones; game rig has only 14.
Do not hide monkeys, change dependency pins or claim earlier generic image gates prove visibility.

## Update — Web surface-frame materials complete; Android stderr mirror prepared

Browser `20261011T100522.314132Z-chrome-materials` passes all four views, 60 intervals
each, GL error zero. All four images inspected; final kernel 12 AppArmor audit + one
audit-suppression notice, no GPU fault, latch unchanged. Session `21871` FINISHED exit zero.
See `docs/terrain-surface-frame.md`. No full input/lifecycle/Firefox qualification implied.

Added consumer-only Android stderr mirror before game construction: complete lines/chunks,
2 KiB buffer, at most 128 chunks plus a limit notice, original stderr always preserved.
No dependency/rendering changes. Native build and 34/34 non-display tests pass (52.34 s),
including chunking, fragmented unitbuf lines, trailing text, stream restoration and log cap.
New test added to explicit debug/sanitizer preset targets. Android build/runtime verification
is next; capture the actual shader/link message for suspected `CNA-ISLAND-002`.
Android x86_64 assembly/lint passes. Monkey close-view run is ACTIVE in session `51286`;
poll the same handle. Launcher log: `build/diagnostic-mirror-android-monkeys.log`.
After inspection, preserve the diagnostic x86 APK separately and restore default ARM64
assembly/lint. The rolling tested APK still holds the earlier material-qualified build.

## Update — Android guest uniform diagnostics investigated read-only

See suspected `CNA-ISLAND-002` in `BUGS.md`: stock CompileAndLink logs failure but continues
uniform queries; the vertex-lit skinned program has 24+3 queries, matching the 27-message
burst. This is a candidate, NOT confirmed program attribution or a driver/uniform-limit defect.
Next targeted evidence: contained monkey close view and actual link diagnostic, after Web ends.
Browser materials session `21871` is still live; litter and grass screenshots are now present.
Litter image inspected and consistent with native. Do not restart or run concurrent graphics.

## Update — surface-frame Android visual checks; Web material scope prepared

Game revision `3642ac1` / capture harness `90fc6a4`: x86_64 assembly/lint and litter
`20261011T095903.886091Z-litter`, wet-sand `20261011T100107.688712Z-wet-sand` gates pass.
Both images inspected, first screenshot attempt; empty final kernel deltas. ARM64 build/lint
restored, tested x86 APK retained; exact hashes in `docs/terrain-surface-frame.md`.
CAVEAT: litter has 27 guest gfxstream uniform-query 0x502 diagnostics, also seen in the
older `20261011T092539.275219Z-interactive`; wet-sand has none. Root cause/ownership unknown;
passed image/startup gates do not mean zero GL errors. Android lifecycle remains open.
Added Chrome `materials` scope: litter, grass, bay-ground, wet-sand, 60 intervals each;
12 browser-launcher tests and Node syntax check pass. Web runtime qualification pending.
Web build/package complete: 10,347,083 raw / 3,528,841 gzip bytes. Browser materials run
is ACTIVE in session `21871`; poll the same handle, do not restart on observation timeout.
Launcher output: `build/surface-frame-web-materials.log`. Build session `8078` is complete.


## Update — explicit shaded-floor capture profile verified

The litter image discrepancy below is resolved by an explicit `shaded-detail` profile,
not a lighting change or global gate reduction. See `docs/terrain-surface-frame.md`.
Fresh native `20261011T095340.833878Z-litter` passes and was inspected; empty kernel delta,
latch unchanged. The old failed run remains failed and untouched. Six validator and twenty
Android CPU-only tests pass; native/Android launchers select the profile from camera arguments.
Full non-display CTest also passes 33/33 (49.94 s); no jobs remain running.
NEXT: Android/Web material qualification of game revision `3642ac1` remains pending.
No emulator/KVM configuration change, hardware rendering or push is authorized.

## Update — surface normal fix built; litter image gate remains open

`docs/terrain-surface-frame.md`: scale-first CPU/GLSL normal helper now drives sand,
soil clods and wet sheen; new `wet-sand` camera and mappings. Native build and 33/33
non-display tests pass (42.70 s), including 1e-30..1e30 scale/winding invariance.
Fingerprint still `5d0b62ad43a25895`, grass 17,225. Native wet-sand
`20261011T094128.539261Z-wet-sand`, grass `20261011T094154.489156Z-grass`, bay-ground
`20261011T094225.864744Z-bay-ground`, water `20261011T094616.083800Z-water` pass and
were inspected; all kernel deltas empty, latch unchanged. No jobs running.

NEXT: resolve retained litter capture `20261011T094405.569362Z-litter`, which FAILED
the generic image gate: luminance deviation 16.8656 < 18, mean 58.8703, 240 bins,
spatial contrast 37.2447; other gates pass. Inspected image is complete textured shaded
floor, not blank. Do not call it passed. Validator unchanged. Evaluate appropriate
image criteria/reference evidence without adjusting lighting just to satisfy a threshold.
Then Android/Web material qualification for this fix; prior `9f1ee59` results do not
qualify it. Android lifecycle remains open and hardware latch must remain set.

## Update — sand/turf Android and Web qualification complete

Current game revision `9f1ee59`. x86_64 assembly/lint passes. Bay-ground
`20261011T092336.875730Z-bay-ground` passes the image gate and matches native visually.
Interactive `20261011T092539.275219Z-interactive` passes sensitivity/footsteps/swimming
and transition audio after one gesture; image inspected. Both kernel deltas empty,
latch unchanged. Lifecycle deliberately excluded, still degraded. Tested x86 APK
preserved, default ARM64 build/lint restored. Exact sizes/hashes in `docs/dry-sand-relief.md`.

Web build/package: 10,346,158 raw / 3,528,706 gzip bytes. Contained Chrome terrain
`20261011T092826.584773Z-chrome-terrain` passes seven views, 60 intervals each, GL error zero.
Bay-ground before/after and jungle images inspected. No GPU faults; final kernel delta
contains AppArmor audit and audit-suppression notices. Full input/lifecycle/Firefox not rerun.
Browser session `28597` and build `34267` completed; do not poll/restart them.
This qualifies game revision `9f1ee59`, not newer uncommitted normal/camera changes above.
These are software checks, not host AMD or physical-device acceptance.

## Update — dry sand relief native stage complete

`docs/dry-sand-relief.md`: wind-aligned warped ripples plus footprint-filtered grain
shading. Native build succeeds, non-display 33/33 pass (50.10 s); final shader refinements
pass material/CLI/browser-preflight checks. Initial bright foreground rejected and fixed
by correcting the sand normal-length floor. Final bay-ground
`20261011T092129.366965Z-bay-ground` and bay `20261011T092157.241700Z-bay` pass and
were inspected; empty kernel deltas, latch unchanged. No geometry/population changes.
Web terrain suite now has seven views, including `bay-ground`. No jobs running.
NEXT: qualify BOTH this shader and preceding coastal turf on Android (bay-ground and
focused touch/traversal, restore ARM64) and Web (build/package, seven-view terrain matrix).
Earlier beach qualification does not qualify these newer stages. Static native images
do not prove temporal stability. Soil/wet-normal length floors deserve separate review;
they were not changed here. Full environment/geology and Android lifecycle remain open.

## Update — coastal turf native stage complete

`docs/coastal-turf.md`: shared CPU/generated-GLSL sand/rock suppression for grass and
turf, plus hard sandy-apron exclusion before the density floor/route boost (parents and
companions). New `bay-ground` eye-height camera and platform mappings. Native build and
final 33/33 non-display groups pass (52.01 s); CLI help test updated for the new camera.
Captures bay-ground `20261011T091417.768738Z-bay-ground`, grass
`20261011T091436.406649Z-grass`, bay `20261011T091521.122820Z-bay` pass and were
inspected; all kernel deltas empty, latch unchanged. Fingerprint `5d0b62ad43a25895`,
17,225 grass patches (two fewer), other populations unchanged, rendered root gaps zero.
Browser grass count pin updated. No jobs remain running.

NEXT: Android/Web qualification of the new turf/camera revision. The previous beach
results below do not qualify it. Ground-level bay review also identified overly smooth
dry sand as the next material target (filtered wind ripples/grain); that is not implemented.
Full environment migration/geology still open. Android lifecycle remains degraded.

## Update — Web wider-beach validation complete

Web build and package for committed `53a2442` complete: 10,343,092 raw / 3,528,055
gzip bytes. Contained Chrome terrain suite `20261011T085703.088366Z-chrome-terrain`
finished with all six views passing, 60 intervals each. Overview/bay/headland/east/jungle
images inspected; no new GPU faults, Wi-Fi bandwidth/EXT4 unmount/USB disconnect notices.
Full Web input/lifecycle
and Firefox not rerun. No browser job remains running; session `38693` completed.

Do not attribute this completed Web test to the newer coastal-turf revision above.

## Update — dominant bay beach native stage complete

`docs/dominant-bay-beach.md`: shared approximately 36 m sheltered bay / 6.5 m eastern
headland width now controls physical profile and sand material. Final 33/33 non-display
tests pass (53.06 s). Native bay `20261011T084410.221147Z-bay`, headland
`20261011T084654.624120Z-headland`, beach `20261011T084740.315381Z-beach` pass and
were inspected, all empty kernel deltas. Existing beach camera looks inland through
vegetation, so it is not a close sand review. Fingerprint `08ced4994845ae68`; measured
inventory pins updated in core and browser tests. Hardware latch unchanged.
Android x86_64 build/lint and interactive `20261011T085027.124944Z-interactive` pass
touch/footsteps/swimming after two gestures, image inspected. Initial bay run had a black
PNG despite old log-gate success. Added `tools/android_capture.py`: bounded 30-second
image-content validation for initial/hot/cold screenshots; 19 CPU-only Android tests pass.
Repeat `20261011T085244.623709Z-bay` passes strengthened gate and visually matches native.
No new GPU faults (Wi-Fi-only kernel deltas). Tested x86 APK preserved; ARM64 build/lint
restored. See `docs/dominant-bay-beach.md` for hashes. Web work is tracked above.
Earlier rocky-coast runs do not
qualify this newer code. A dedicated ground-level bay view is useful follow-up work.
Android lifecycle remains degraded; do not close the known issue from focused visual runs.
No Android/KVM system configuration changes are needed or requested. No push.

## Update — rocky coast Web qualification complete

Web build/package refreshed: 10,343,035 raw / 3,528,104 gzip bytes. Contained Chrome
terrain suite `20261011T083120.118782Z-chrome-terrain` passes all six views: overview,
bay, headland, beach, east, jungle, 60 intervals each and GL error zero. Bay/headland/east/
jungle images inspected. No new GPU faults; kernel delta contains USB/storage, PCIe and
perf notices. Full Web input/lifecycle and Firefox not rerun. This qualifies `2a660b7`.

Historical source preparation (now completed in the native stage above): changes in
`IslandEnvironmentField.hpp`, `IslandTerrain.cpp`, `TerrainRenderer.cpp` and `CoreTests.cpp`
add variable beach width, approximately 36 m in the sheltered bay and 6.5 m at the eastern
headland, shared by geometry and sand material. The completed browser suite used the immutable
rocky-coast build from commit `2a660b7`; it does NOT qualify this newer source preparation.
Native build/tests/review are now complete; affected Android/Web platforms still need requalification.

## Update 2026-10-11 — integrated rocky coast (native and focused Android qualified)

See `docs/coastal-environment.md`. The first `IslandEnvironmentField` coastal subset drives
actual up-to-7.5 m headland relief, RockyGround classification, palm/canopy/understory/grass
exclusions and shader rock/sand controls. Reuses canopy texture blue/alpha; red/green and
texture/mesh/draw budgets unchanged. This is not the full environment model.
Crab starts now find nearby valid sandy habitat; tests preserve their population/determinism.
Rocky faces intentionally block climbing, with separately tested adjacent beach access.
Final 33/33 non-display groups pass (55.12 s). Bay `20261011T082154.597630Z-bay` and final
headland `20261011T082316.930694Z-headland` pass/inspected, empty kernel deltas.
Fingerprint `b0f79ae14adeb8cd`; inventory pins updated from report. `headland` camera is
available in native/Android launchers and Web template; terrain suite now has six views.
Android x86_64 build/lint and headland `20261011T082539.891561Z-headland` pass, image
inspected. Interactive `20261011T082658.311623Z-interactive` passes touch/footsteps/swimming
after two gestures; lifecycle not rerun and still degraded. Empty kernel deltas. ARM64
assembly/lint restored; physical-device runtime untested. NEXT: finish the running Web
visual/runtime qualification for this new terrain/shader, then
extend environmental controls/dominant beach. Prior coast-shape qualification below is not
current evidence. Android lifecycle remains degraded. No push or hardware rendering authorized.

## Update 2026-10-11 — Web coast qualification complete

Coast Web build and release package refreshed (10,340,739 raw / 3,525,638 gzip bytes).
Contained Chrome terrain suite `20261011T080318.835613Z-chrome-terrain` passes all five
views: overview, bay, beach, east, jungle (60 frame intervals each). Overview/bay/east/jungle
screenshots inspected. No new GPU faults; kernel delta contains perf and Bluetooth/USB
messages. No game changes since coast commit `623c314`. Web input/lifecycle and Firefox
were not rerun. Android lifecycle remains degraded, separately from the passed visual checks.
Read-only SDL inspection found explicit pending pause/resume coalescing, which can explain
missing AAudio calls during rapid Home/resume; see `BUGS.md`, not a confirmed root cause.
Next implementation: `docs/environment-field-design.md` (design only), integrating shared
coastal rockiness/beach suitability through relief, vegetation placement and terrain materials.

## Update 2026-10-11 — bay/headland silhouette (native qualified)

See `docs/coastal-silhouette.md`. Western bay between two headlands plus eastern promontory;
shore radii 158.994–258.680 m. New `bay` QA camera. The first shallow design was rejected
visually; final geometry has world-space recession checks. New coast exposed turtle bank
steering at the home centre, now fixed without weakening its movement regression.
Final 33/33 non-display groups pass (37.13 s). Native final bay
`20261011T074941.874456Z-bay` and turtles `20261011T075118.375540Z-turtles` pass/inspected.
Fingerprint `9dc988ef0ffd9f67`; population and shadow-count pins updated from the report.
Android x86_64 build/lint and bay `20261011T075311.442314Z-bay` pass, image inspected.
Interactive first fails a too-short fixed swimming gesture; bounded repeated-touch gate
added with a device-free shell regression. Repeat `20261011T075739.370693Z-interactive`
passes touch/footsteps/swimming after two gestures but lacks AAudio pause/start evidence
on Home/resume, despite restored surface/focus/same PID. Lifecycle remains degraded;
see the distinct observation under `CNA-ISLAND-001`. ARM64 assembly/lint restored;
physical-device runtime untested. Relevant harness regressions and syntax checks pass.
Web qualification is recorded above. Next is shared coastal
environment/material placement to replace the continuous sand band with rocky headlands
and a dominant beach. Macro-geography items 1–3 remain incomplete. Android lifecycle issue
`CNA-ISLAND-001` remains open. No hardware rendering or push is authorized.

## Update 2026-10-11 — ridge/valley interior

See `docs/ridge-and-valleys.md`. Two offset ridges now reach 40.0192 / 29.0938 m, with a
13.7758 m saddle station and southern/western drainage. Steep faces reveal filtered rock
grain/fractures. Bird clearance now anticipates terrain and clamps the complete altitude.
Final 33/33 non-display tests pass (49.78 s), including summit/valley, trail slope, roots,
wildlife contact and bird clearance. Native final overview `20261011T072739.811196Z-overview`
and east `20261011T072605.084747Z-east` pass and were inspected.
Android x86_64 build/lint and east `20261011T072932.613664Z-east` pass, image inspected.
Interactive `20261011T073115.062445Z-interactive` fails Home/resume with `EGL_BAD_SURFACE`;
identical-APK repeat `20261011T073355.598497Z-interactive` passes. Suspected lifecycle race
is open as `CNA-ISLAND-001` in `BUGS.md`; do not call Android lifecycle reliably qualified.
ARM64 assembly/lint restored. Web build/package refreshed; Chrome terrain suite
`20261011T073720.044111Z-chrome-terrain` passes all four views (60 frame intervals each).
Overview/east/jungle screenshots inspected; no new kernel GPU faults.
The world fingerprint and new inventory pins are in the ridge document.
Next is the shoreline/headland/bay stage, followed by the shared environment field. The full
macro-geography rewrite remains open. No hardware rendering or push is authorized.

## Update 2026-10-11 — narrow footpath

See `docs/natural-footpath.md`: vegetation corridor 12.4 m -> 2.8 m, narrower dirt shading,
full trunk/rock footprint rejection and three-lane collision regression. Native overview/jungle
inspected; all non-display test groups pass (core rerun after the rock fix).
Android x86_64 touch/traversal/lifecycle run `20261011T071027.731091Z-interactive` passes;
ARM64 build/lint restored. Web build and package refreshed. First grass-motion run failed a
15 s wall wait at 5.99/6 m; bounded game-sample wait added. Retry
`20261011T071524.062886Z-chrome-grass-motion` passes seven stages, 93 distinct frames,
GL error zero and no new GPU faults. Full Web lifecycle was not rerun.
New inventory and world fingerprint are in the footpath document; browser pins updated.
The next substantial realism target is the asymmetric ridge/valley terrain
and coast from macro-geography items 1–3. Local path forks/detours remain open too.

## Update 2026-10-11 — reef/seabed material pass

Continued the suggested items 5/16 in `TerrainRenderer.cpp`: depth-limited olive seagrass,
mottled ochre reef patches, pale radial sand channels and a broken outer material crest.
See `docs/reef-seabed.md` for the design, limitations, captures and package identities.
This is a material-only first pass; geometric reef relief and the larger terrain rewrite
remain open. No dependency pins or population/mesh counts changed.

Linux build and 33/33 non-display tests pass. Native before/after overview and Android
SwiftShader reef/overview captures pass and were visually inspected. ARM64 assembly/lint
restored; physical runtime untested. Chrome/SwiftShader terrain suite
`20261011T065909.248161Z-chrome-terrain` passes all four views and the web release is refreshed.
Nothing from this
session has been pushed. The AMD hardware stop latch remains unchanged.

The reef pass is complete as a first material milestone. The trail step selected after it is
recorded above.

The following is the historical 2026-10-10 handoff; use the update above and `PLAN.md` for
the current state.

Written 2026-10-10 (last update at the end of that day's session) for the next agent
(expected: Codex Astra High) starting on a fresh context. The working tree is clean and
everything below is committed and pushed to `origin/develop`. Everything you need to continue is here or linked; `PLAN.md` is the detailed
ledger (newest sections at the top, under "2026-10-10"). All repository text is English.

## 1. Read first: hardware safety boundary (unchanged, still in force)

**Do not run this game on the host AMD GPU. Do not clear the hardware stop latch.**
The 2026-10-08 AMDGPU GPUVM faults caused a MODE2 reset, VRAM loss and a GNOME logout; the
root cause is not identified or fixed. Clean software-renderer runs are not evidence that the
hardware workload is safe.

- Latch: `build/gpu-incident-evidence/QUALIFICATION_STOP.json`, SHA-256
  `c0bd170489321c8d497eae0f1211e1553e30aef9ee02ac01774269300e6ee6f1` (ignored local state; a
  fresh clone lacking it is not hardware authorization).
- Only run the game through the contained software launchers listed in section 5. Never run
  unrestricted display CTest, never replay GL traces on AMD, never launch the binary directly.
- Incident documentation: `docs/incidents/2026-10-08-amdgpu-gpuvm-reset.md`,
  `capture-evidence-inventory.md`, `mesa-recorder-readiness.md`, `process-containment.md`,
  `maintainer-sharing-summary.md` and `upstream-triage-draft.md` (drafts only; nothing has
  been submitted upstream and pushing this repository is not permission to do so).
- Stop on any new GPU fault, including unrelated host activity. Do not restart a running
  tool job just because an output poll timed out.

## 2. Operator direction (2026-10-10)

1. **The island must look noticeably closer to reality**, for existing and all future
   content. Stylized or crude assets are defects. The operator named the sea turtle as
   "really terrible"; a first pass is done, more is expected (see section 4).
2. Product boundaries are unchanged: peaceful first-person exploration of a ~500 m tropical
   island, fixed sunny daytime, no combat, quests, survival, crafting, inventory, weather or
   day/night. Deterministic simulation. Portable GLSL 330 / ES 300. Measured budgets.
3. Backlogs the operator asked for are in `PLAN.md` (section "2026-10-10 — operator
   direction"): a 16-species nonhostile wildlife list (snakes, spiders, parrots, pigeons,
   crocodiles, frogs, bats, mice, rats, deer, goats, ants, flies, stingrays, dolphins,
   octopuses), a decorative island hut, and a 20-item macro-geography realism list from the
   operator-supplied analysis (Czech original kept outside the repository at
   `../island_improvements.md`). **Monkeys and wild boars stay on the island** by operator
   decision.
4. Order: visual realism of existing content before new species; a new species is not done
   until silhouette, material, motion and contact look believable in close software captures.

## 3. What was done on 2026-10-10 (all committed on `develop`)

- Dependency pins advanced twice to the clean shared checkouts (`dependencies.lock`): Sharp
  Runtime `next` c20373ba, EasyGL f4d6afaf, MetaGL dd3b3005, CNA 97783d1e (unchanged). The
  strict check in `cmake/VerifyDependencies.cmake` refuses to configure when a shared
  checkout moves; **other agents commit to those checkouts during the day**, so if a build
  refuses, review the delta, update the lock deliberately and requalify. Never switch or
  edit the shared worktrees.
- `docs/ground-cover-and-trunks.md`: `GroundCoverField` + `GroundCover::Density` shared by
  grass placement and the terrain turf undercoat (patch field in the canopy-light texture's
  green channel), 20,523 grass clusters; grey-brown canopy trunks with root flare, rounded
  buttresses, moss/lichen, contact occlusion and canopy-shaded wood; slender flared palm
  stems with smooth normals.
- `docs/sea-turtle-form.md`: domed scute-aligned carapace, plastron, beaked head, bending
  paddle strokes (`SeaTurtleProfile.hpp`, tested); still blocky head and tonal-only seams.
- `docs/sky-atmosphere.md`: shared-haze horizon, warm sun glow, eleven fixed distant cumulus
  masses; `environment::FogColor*` constants back the C++ fog sites (GLSL literals are still
  hand-kept at `vec3(0.53, 0.78, 0.88)`).
- `docs/forest-stands.md`: closed canopy stands with a sparse scatter and 22 emergents
  (277 trees). Counts that changed downstream: 579 bushes (14 clearing), 3,116 ground plants
  (64 clearing, 2,225 island-wide), 199 colonies, 159 land rocks, 12 logs.
- Evening, after the operator chose to continue the backlog: sea turtle second pass (ten-segment
  four-ring head, geometric scute seams, wet sheen; `docs/sea-turtle-form.md`) and the first
  prevailing-wind pass (`docs/prevailing-wind.md`: shared east trade wind, coastal windward
  exposure, palms lean up to ten degrees; ocean/foam/crowns/wisps/audio still to do) and
  depth on the horizon (`docs/distant-islets.md`: nearer wooded islet, sandbar, hazed far
  silhouettes), and coconut palm groves with young palms (`docs/palm-groves.md`, 370 palms).
- Harness fixes (`docs/browser-input-ownership.md`): GLSL ES reserves `patch` (a CPU test now
  scans generated GLSL for ES-reserved identifiers); the Web harness holds keys until the game
  logs a new acknowledgement and waits for the release to be observed; the swim walk and
  resume waits are paced by game output because frames on SwiftShader exceed 1 s and
  `PlayerMotion` clamps each step to 0.1 s.

## 4. Open work, in the intended order

1. **Keep going on visual realism** per the `PLAN.md` 2026-10-10 backlog. Highest-impact open
   items: terrain macro-geography rewrite (item 1), varied coast types (2), environment
   field replacing shore-distance bands (3), prevailing wind (4), readable reef (5), natural
   footpath (6), palm distribution (9), directional shadows (11), coastal micro-topography
   (15), underwater habitat (16), small fauna (17), geology (19), wet biome (20).
   Partially done: wind (4), stands (7), emergents (8), palm groves (9), trunks (10), sky (12),
   atmosphere (13); done: islets (14).
2. Sea turtle third pass: hexagonal scute layout, surface reflections, underwater tint.
   Suggested next realism step (designed, not started): readable reef/seabed (items 5/16) in the
   terrain fragment shader only. Measured profile: the bed is 0.7 m deep 20 m offshore, 3 m at
   70 m, 5 m at 110 m, 16 m at 250 m; water alpha is `0.42/m` extinction, so seabed patterns
   read to about 3 m depth. Plan: seagrass meadows (0.5-2.5 m depth, dark olive, broad/medium
   noise), patch reefs 28-78 m offshore (ochre-brown mottled blobs) cut by pale radial sand
   channels (noise along arc length), and a broken reef crest band 70-84 m offshore. Keep it
   straight-line GLSL (see the sky lesson below) and verify on Android.
3. Then species from the wildlife backlog and the island hut, each with deterministic
   simulation, budget, capture viewpoint, regression tests and close-capture review.
4. Requalify Web input/lifecycle and Android touch/lifecycle after relevant changes. ARM64
   physical-device runtime and Linux AMD hardware acceptance remain open. Never promote
   pre-incident results to current qualification.
5. Incident work is a separate approval-gated track (section 1).

**Resolved 2026-10-10 evening:** the Android emulator crash (qemu host RenderThread SIGSEGV
in `jungle`/`east`/`turtles`) was caused by the sky's cumulus loop over a constant array, not
by the forest stands. Bisected file by file and fixed by unrolling (`docs/sky-atmosphere.md`).
Lesson for future shaders: avoid constant arrays indexed by loop counters (and `continue`)
in fragment shaders; the Android emulator's gfxstream/SwiftShader host can crash on them even
when Mesa and ANGLE accept them. Many earlier "colony" experiments in `PLAN.md` are void.

Known soft spots to watch: distant turf undercoat reads as flat olive in open clearings;
cumulus masses are smooth blobs; `lifecycleResumed` in the full Web suite is false (reported
only, not a gate); `IslandGpuQualificationSupervisorTests` has a 4 s fixture timeout and can
fail under `ctest -j4` load while passing at `-j2` or alone.

## 5. Build, test and capture commands (run from the repository root)

```sh
export CCACHE_DIR=/rv/cnaccache CCACHE_BASEDIR=/rv     # mandatory, see ../AGENTS.md
cmake --build --preset linux-debug -j2                 # existing build/ tree, do not reconfigure
ctest --test-dir build -LE display --output-on-failure -j2

# Contained native llvmpipe captures (viewpoints: overview beach jungle east clearing-edge
# west grass ground ground-broadleaf bush litter contact movement palms water current distant
# props driftwood reef fronds birds fish shark monkeys shipwreck crabs butterflies turtles)
sg systemd-journal -c 'python3 tools/run_software_capture.py build grass'

# Web (existing build-probe/ tree)
source /home/robertvokac/emsdk/emsdk_env.sh
export EMCC_CORES=2 BINARYEN_CORES=2
cmake --build --preset web-webgl2 -j2
sg systemd-journal -c 'python3 tools/run_browser_software.py build-probe --node /home/robertvokac/emsdk/node/24.19.0_64bit/bin/node --browser chrome --scope litter --timeout 300'
#   scopes: full terrain wildlife-foliage coast grounding vegetation grass-motion litter
#   (the turtle view exists only in --scope full --timeout 1700, about 25 minutes)
tools/package_web_release.sh build-probe

# Android (x86_64 emulator run, then restore the default ARM64 package)
export ANDROID_SDK_ROOT=/home/robertvokac/Android/Sdk
tools/run_android_gradle.sh :app:assembleDebug :app:lintDebug -PislandAbi=x86_64
sg systemd-journal -c 'python3 tools/run_android_software.py --sdk /home/robertvokac/Android/Sdk --boot --apk android/app/build/outputs/apk/debug/app-debug.apk --viewpoint grass'
cp android/app/build/outputs/apk/debug/app-debug.apk build/android-software/island-x86_64.apk
tools/run_android_gradle.sh :app:assembleDebug :app:lintDebug
```

Rules that bit this session:

- Every launcher writes `run.json` with `"result"`; a `tail` of its stdout can look like
  success while the gate failed. Always read `run.json` and keep failed-run directories.
- The owned emulator crashed once (exit -11) on 2026-10-10 and its stale AVD lock files made
  the next launch fail before boot; the harness now removes such locks when no emulator
  process exists. Browser scopes with eight views need `--timeout 1700` at current frame cost.
- Harnesses pin exact diagnostics and counts (`tools/run_software_capture.py`,
  `tools/run_web_smoke.mjs`, `tools/run_firefox_web_smoke.mjs`,
  `tools/run_android_runtime_smoke.sh`). When you change a mesh inventory, population count or
  a diagnostic string, update the pins in the same change, otherwise runs fail with
  "check for a stale executable" or silently on a regex.
- The full Web suite verifies 28 sections; if a run fails, test the harness' `includes`
  literals and regexes against the retained `island-browser-smoke.json` outputs to find the
  stale one quickly (see the PLAN entry for the 2026-10-10 turtle requalification).
- Do not overlap heavy jobs (Web build, browser suite, emulator, Gradle). Two build workers
  for Sharp-Runtime-governed work; `-j4` is fine for the island build itself.
- Never build in `/tmp`; reuse `build/`, `build-probe/`; keep `build-consumer/` and
  `build/gpu-incident-evidence/` untouched.

## 6. Evidence locations (ignored local data, not in Git)

- Native: `build/software-captures/<timestamp>-<viewpoint>/` (PPM, logs, `run.json`,
  kernel deltas). Latest (palm groves): `20261010T174808.840978Z-overview`,
  `174754.658042Z-palms`, `174801.794546Z-beach`; before: `20261010T172806.329415Z-overview`,
  `172812.566297Z-distant`, `172642.513830Z-palms`, `172648.540756Z-jungle`. Earlier:
  `20261010T150916.889020Z-jungle`, `150924.874906Z-overview`,
  `150932.482781Z-east` (forest stands); `145219Z-beach`, `145227Z-overview`,
  `145234Z-palms` (sky); `143756Z-turtles`; `130435Z-grass`, `131112Z-litter` (ground cover).
- Web: `build/browser-software/`. Full suite `20261010T180146.003280Z-chrome-full` (28/28,
  final tree with palm groves; earlier `172921.618192Z`, `142621.752717Z`);
  vegetation scope for the forest stands `152203.104225Z-chrome-vegetation`; litter scopes `145543Z`
  (sky) and `131626Z` (ground cover); grass-motion `132225Z`. Failed runs from the same day
  are retained on purpose (reserved word, key pulses, stale pins).
- Android: `build/android-software/`. Final tree (palm groves), all pass:
  `20261010T175715.080427Z-palms`, `175828.715762Z-jungle`, `175941.257408Z-east`; before the
  groves: `173721.178620Z-jungle`, `173833.723258Z-east`, `174128.122762Z-turtles`. Earlier:
  `145908Z-palms` (sky), `144750Z-turtles`, `133014Z-grass`; the many failed `jungle`/`east`
  runs of the evening are the sky-shader crash bisection. Rolling tested x86_64 APK:
  `build/android-software/island-x86_64.apk`.
- Package sizes and hashes are in the `docs/*.md` file of each pass. ARM64 runtime untested.
- Incident evidence: `build/gpu-incident-evidence/` and historical `build-consumer/`.
  Never clean these as disposable build output.

## 7. Repository and dependency boundaries

- CNA `next` export at `build/dependencies/cna-next` and Sharp Runtime `next`, EasyGL and
  MetaGL `develop` under `../` are strictly read-only. Record actual framework defects in
  `BUGS.md`; do not patch them.
- Original artwork lives in `assets/art-studies/` with provenance in `ASSET_LICENSES.md`
  and `assets/manifest.json`; prepared PNGs are generated at build time (Python 3.10+ and
  Pillow on the host only).
- Commit on `develop`; push only when the operator asks. The push on 2026-10-10 included all
  work up to the forest-stands pass.

# Operator additions — 2026-10-12 (snake/spider density and snake behaviour)

- Increase the island's production populations beyond the current four snakes and
  twelve ground spiders, with habitat-aware separation and measured performance/encounters.
- Snakes must visibly crawl rather than appear static, including convincing propagated
  S-curves, terrain contact and smooth bounded turns/retreats.
- Add restrained positional snake hissing with behavioural triggers, cooldowns and actual
  listened native/Web/Android evidence. These requirements were first recorded unchecked
  in `PLAN.md`; documentation by itself was not treated as implementation or acceptance.

Implementation follow-up: ground snakes are now 8 and ground spiders 24. Snake homes
remain >=34m apart and spider homes >=8m apart. All retain deterministic simulation,
render/shadow bounds; the grass shader selects the nearest four snakes to preserve its
portable uniform budget. Snake startup phases now crawl immediately with a five-second
rest window later in the cycle. Focused CPU/WorldReport tests pass, fingerprint
`02d44fab27a0b823`. Inspected llvmpipe evidence `121151.377464Z-snakes` and
`121332.031522Z-spiders` shows both subjects moving, but both generic scene gates fail,
so portable/performance/encounter and final appearance acceptance remain open. Hissing
now has a tested procedural implementation, but still lacks actual disk-device/listened
and Web/Android acoustic acceptance.

Snake-hiss follow-up: two original one-second variants pass the 24-clip/3,741,884-byte
audio report and preview export. Only active retreating snakes inside 16m are eligible;
per-animal and population cooldowns prevent chorus/repetition. The moving head remains
the positional emitter and playback reuses the one wildlife voice, preserving the
six-source cap. Core scheduler tests pass. Supervised private-llvmpipe/SDL disk evidence
`build/software-snake-audio/20261012T133332.612520Z` PASS: live wildlife type4, hiss count
0->1 before mute and 1->2 after resume, 1,482,752 exact-zero muted bytes, 2,277,376 nonzero
resumed bytes, max5/6 sources, kernel-final[]. Root cause found/fixed: soundscape advance
and M mute were nested in interactive UpdateCamera, so fixed QA views never advanced
cooldowns or muted. Both now run in the common update path. Screenshot inspected: close
snake/head and F1 state visible; it is functional evidence, not visual/acoustic realism.
No subjective listening, Web or Android claim has been made.

# Latest — beach-crab close-form pass

The fifth requested aquatic/coastal animal now has a first native visual pass. A new low
`crabs` QA camera exposed the old pointed fan bodies, hairline single-triangle legs and
arrow claws. `CrabRenderer` now emits an exact594-vertex/198-triangle near animal: four-ring
ten-sector rounded carapace, varied upper panels/dark side plates, crossed two-segment walking
legs, thick claw arms, octahedral palms, split fingers, shortened volumetric eye stalks,
mouthparts and lateral shell teeth. Far remains12 vertices/4 triangles; population30, habitat,
simulation,38m LOD and one batch/draw are unchanged. Runtime guards verify both exact budgets;
worst-case buffer17820 vertices/285120 bytes. All native/Web/Firefox/Android diagnostic pins
were updated. Final isolated pixman/llvmpipe `build-consumer/software-captures/
20261012T212449.173818Z-crabs` PASS:166 bins/std28.25/spatial175.93, five near/one draw,
kernel-live/final empty; image inspected. No AMD GPU. Release and Debug builds pass. The first
parallel62-group non-display run passed59 groups; the known load-sensitive supervisor fixture,
a stale post-hut ordinary-bush expectation (573 instead of measured572), and the missing dolphin
manifest record failed. The expectation and manifest were corrected, then all three groups passed
serially in58.64s; asset gate is27 procedural records/92 generator sources/20 project files/zero
external. Web/Android/performance/material review remains, so aquatic polish stays `[~]`. See
`docs/crab-form.md`.
# Latest — prevailing-wind debris accumulation native pass

Ecosystem item9 progressed to `[~]`. `IslandEnvironmentField::SampleCoastalDebris`
combines existing `PrevailingWind` exposure with bay/mouth/rock identities; driftwood,
beach detail and coastal fallen-frond density share it. NaturalPropLayout adds a bounded
deterministic western-bay belt of10 supported/tree+route-cleared driftwood and24 spaced
shell/wrack clusters, reusing all existing meshes/draw paths. Initial removal-biased
attempts correctly failed because one of six crab debris habitats disappeared; final
additive sorting preserves every old cue and all crab habitats. Core PASS105.8s; measured
sheltered/exposed wood/detail/fronds11/4,47/26,2/0; totals33/217/35. WorldReport valid,
fingerprintec61dbf7e9ab7adb. Native game build max4cores PASS. Actual isolated llvmpipe
`20261012T140703.574060Z-bay-ground` and `140806.665876Z-headland` PASS/kernel[]; both
images inspected, bay visibly richer and headland sparse/rocky. Web/Android, interactive
walk and performance remain, so no completion claim. Next ecosystem item10 cloud drift.
No AMD/KVM/dependency changes.

# Latest — grounded enterable helicopter native first pass

The retained centre-island helicopter requirement is now `[~]`, not complete. One original
project-owned procedural utility helicopter is deterministically placed at
(-50.2571,40.0224,48.7260), clear of the stream and every static solid. Its 5.65m main rotor
owns a 6.20m vegetation/grass reservation; this intentionally changes the inventory to
322palms/270canopy/1770understory/582bushes/3008groundplants/16809grass. Four skid contacts
sample the rendered terrain exactly. WorldReport exports the placement, entry flags and skid
contacts; fingerprint `76af8e15925526d1`,805 player-static/806 wildlife-avoidance obstacles.

The 1006-triangle one-draw vertex-colour mesh has a sealed faceted glazed nose, centre mullion,
olive fuselage, tail/fin/stabilizer, stationary main/tail rotors, terrain-following skids, cockpit
panel/lights, two seats and cyclics, rear bench, floor and two-level thresholds. Both pilot and
rear door bands are real openings on both sides. A collision defect discovered during review
(one full left shell rectangle sealing both left doors) was removed. `PlayerMotion` now uses
detailed shell/seat sweep collision and helicopter floor support; its CPU test actually walks all
four door paths. Wildlife gets a separate broad proxy, so animals avoid the airframe without
sealing player entry. There is deliberately no helicopter flight state.

Final isolated llvmpipe/pixman captures were inspected:
`build-consumer/software-captures/20261012T215230.210248Z-helicopter` (275 bins/std54.49/
spatial230.02) and `20261012T215239.544205Z-helicopter-inside` (216/32.48/132.84). Both pass,
show one helicopter draw and triangles=1006 with empty new-kernel-fault evidence. Exterior reads
clearly as a helicopter and interior exposes seats/panel/open sides; it remains a faceted reduced-
detail first pass, not photorealism. Texture/material, door/seat interaction, live free-walk,
Web/Android and performance gates remain open. Debug/Release builds pass max4cores. Focused
14 groups pass; broad enabled run passed60 groups, with only the known load-sensitive supervisor
fixture and two orphan validators of disabled display prerequisites failing. Supervisor passed
serially; the two display validators lack their intentionally disabled report/capture inputs.
Asset gate passes with the new original procedural record. No AMD GPU, KVM, Android emulator or
dependency tree was touched. See `docs/helicopter.md`; continue the active full-plan goal.

# Latest — campfire ash and charcoal readability pass

Active goal continues. The existing campfire solid mesh now renders a pale, two-frequency
mottled ash centre with a scorched perimeter instead of one brown disk. Its unchanged ten low
pieces are slightly larger/wider and divided into four bright embers and six dark charcoal
chunks with sparse pulsing fissures. This adds one material tag but keeps5976 vertices/1992
solid triangles,42 moving effect triangles, two buffers/two culled draws, no texture and no
new light/audio/physics state. `CampfireTests` now requires all seven material groups while
retaining exact geometry, deterministic terrain contact and collision checks.

Release build max4 cores PASS. Core+campfire2/2 PASS30.41s; BrowserSoftwareTests+
SoftwareCaptureTests88/88 PASS2.009s. Native llvmpipe four-stage
`20261013T214043.021426Z-campfire` PASS365bins/std39.05/spatial172.00/kernel[]; montage and
close crop inspected, ash visibly separates from soil and mixed coal is less uniformly red.
WebGL2 build max4 cores PASS; Chrome/SwiftShader
`20261013T214342.603885Z-chrome-campfire` PASS60 finite intervals,GL0/context intact/kernel[],
final image inspected. WorldReport valid fingerprint`0ecd98ad6f56b15b`. Android ARM64 and Debug
assembleDebug+lintDebug PASS10s without emulator/KVM. Debug build PASS and Core+Campfire2/2
PASS67.95s. No AMD GPU/KVM/emulator/dependency change. Remaining
campfire speaker listening, continuous moving-view/performance and broader plan stay open.

# Latest — resident woven-clothing silhouette pass

Goal remains active. Current resident audit showed the face readable but the top/shorts still
forming a boxy figurine silhouette. `ResidentRenderer` now narrows the shoulder ring and cap sleeves,
adds a continuous four-segment plant-fibre-bound V neckline, and overlays the fitted shorts with
two front/two rear cloth panels. Outer hem corners follow the authoritative thigh positions while
inner corners stay near the pelvis, so the centre split opens through the existing gait without a
cloth simulator. Exactly one peaceful resident, route, collision, planted feet, atlas, draw count
and shadow geometry are unchanged. Body cost5090→5154 triangles/15462 streamed vertices, still one
body draw plus24 shadow triangles and below the40000-vertex capacity.

Release build and ResidentTests PASS1.14s; focused Python gates88/88 PASS1.688s. First accepted
native221102.938781Z-resident PASS213bins/std35.47/spatial134.29/kernel[]; all eight stages and a
close crop inspected, with continuous neckline and stride-opening wrap. Chrome/SwiftShader
221420.747958Z-chrome-resident PASS two distinct inspected frames/60 finite intervals/GL0/context
intact/kernel[]. ARM64 assembleDebug+lintDebug PASS13s without emulator/KVM. Debug build and
ResidentTests PASS5.99s; final Python gates88/88 PASS1.137s. No AMD GPU/KVM/emulator/dependency
change. Skin response, sidestepping, continuous gait, Android runtime/performance and the wider
plan remain open; do not mark resident or goal complete.

# Latest — boar continuous lower-leg pass

The active goal continues. Near boars no longer use dark spherical knee caps between the
furred upper limb and lower tube. Each lower leg is now two exactly connected tapering
sections from knee through cannon to the existing cloven hoof; hoof colour is a slightly
lighter dark brown. Authoritative foot targets, terrain contact, gait, far geometry, herd,
material partition, buffers, shadows and two draws are unchanged. Net near cost is
-32 triangles/-96 streamed vertices per animal.

Release build and herd/coat/pose tests pass; focused Python software gates88/88 pass.
Native llvmpipe `20261013T222749.259239Z-boars` PASS214bins/std19.18/spatial94.22/kernel[];
the eight-stage montage and matched old/new stage-two crop were inspected and the former
black joint ball is gone. WebGL2 build passes max4 cores. Chrome/SwiftShader
`20261013T223134.897234Z-chrome-boars` PASS60 finite intervals/two distinct frames/GL0/
context intact/kernel[]; final image inspected. ARM64 assembleDebug+lintDebug PASS3s without
emulator/KVM. Debug build and herd/coat/pose tests PASS7.22s. This does not complete boar
realism, subjective speaker listening or the larger plan. No AMD GPU, KVM, emulator or
dependency change.

# Latest — bear continuous anatomy pass

The goal remains active. A current close audit exposed the bear as three visibly intersecting
body spheres plus tube/ball/tube legs. `BearRenderer` now emits one thirteen-station elliptical
loft from rump through raised shoulder and neck, and one nine-station loft from each shoulder/
haunch through its authoritative IK joint to the wrist. Near paws use five claws instead of
three. Motion, planted targets, fixed bone lengths, behavior, material, texture, buffers, one
body draw and one18-triangle shadow draw remain unchanged. Replacing1848 body-shell triangles
with528 while adding8 limb and112 claw triangles changes the near body6778→5578 (-17.7%).

The first Web loft review correctly exposed stretched horizontal fur and then a flank atlas seam;
both were visually rejected. Final UVs run source guard hairs along the trunk and place the
non-tileable seam under the belly. Final native llvmpipe
`20261013T232603.134747Z-bear` PASS189bins/std36.58/spatial168.50/kernel[]; all eight poses and
old/new comparisons inspected. Final Chrome/SwiftShader
`20261013T233028.007588Z-chrome-bear` PASS60 finite intervals/two distinct inspected frames/
GL0/context intact/kernel[]. WebAssembly, ARM64 assembleDebug+lintDebug PASS8s without emulator,
Release/Debug BearMotionTests and89 focused Python gates pass. This removes the worst body/joint
assembly seams but not remaining head/neck topology, gait-weight, Android runtime, physical
performance, subjective listening or the larger plan. No AMD GPU, KVM, emulator or dependency
change.
