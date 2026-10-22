#include "IslandSimulator/BoarGroundPose.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/QuadBike.hpp"
#include "IslandSimulator/WindGust.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/SlopeScrubLayout.hpp"
#include "IslandSimulator/IslandGame.hpp"
#include "IslandSimulator/BridgeLayout.hpp"
#include "IslandSimulator/LookoutLayout.hpp"
#include "IslandSimulator/JettyLayout.hpp"
#include "IslandSimulator/StreamBedPatch.hpp"
#include "IslandSimulator/StreamCourse.hpp"
#include "IslandSimulator/CloudShadow.hpp"
#include "IslandSimulator/CanopyShafts.hpp"
#include "IslandSimulator/WreckScourPatch.hpp"
#include "IslandSimulator/CanopyLighting.hpp"
#include "IslandSimulator/CampfireLighting.hpp"
#include "IslandSimulator/HutGeckos.hpp"
#include "IslandSimulator/OceanWaveSpectrum.hpp"
#include "IslandSimulator/TerrainShaderNoise.hpp"
#include "IslandSimulator/CoastalMaterial.hpp"
#include "IslandSimulator/CoastalWindResponse.hpp"

#include "IslandSimulator/FirstPersonInput.hpp"
#include "IslandSimulator/GeneratedTextureBudget.hpp"
#include "IslandSimulator/GpuDiagnostics.hpp"
#include "IslandSimulator/HelicopterSite.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/PerformanceStatistics.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include "IslandSimulator/TidePoolLayout.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"
#include "IslandSimulator/WorldIdentity.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <limits>
#include <numbers>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsProfile.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "CNA/Input/Clipboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;

namespace island
{
    namespace
    {
#if defined(__EMSCRIPTEN__)
        volatile int WebMovementKeyMaskValue = 0;

        EM_JS(void, InstallWebMovementKeyBridge, (), {
            if (globalThis.islandMovementKeyBridgeInstalled) return;
            globalThis.islandMovementKeyBridgeInstalled = true;
            globalThis.islandMovementKeyBridgeEvents = [];
            const record = (code, value) => {
                const events = globalThis.islandMovementKeyBridgeEvents;
                if (events.length < 32) events.push({code, value, milliseconds: performance.now()});
            };
            const clear = () => {
                _IslandSetWebMovementKeyState(15, 0);
                record("clear", 0);
            };
            const update = (event, value) => {
                let bit = 0;
                switch (event.code) {
                case "KeyW": case "ArrowUp": bit = 1; break;
                case "KeyS": case "ArrowDown": bit = 2; break;
                case "KeyA": case "ArrowLeft": bit = 4; break;
                case "KeyD": case "ArrowRight": bit = 8; break;
                default: return;
                }
                _IslandSetWebMovementKeyState(bit, value);
                record(event.code, value);
            };
            addEventListener("keydown", event => update(event, 1), {capture: true, passive: true});
            addEventListener("keyup", event => update(event, 0), {capture: true, passive: true});
            addEventListener("blur", clear, {capture: true, passive: true});
            document.addEventListener("visibilitychange", () => {
                if (document.visibilityState !== "visible") clear();
            }, {capture: true, passive: true});
        });
#endif

        constexpr float MouseSensitivity = 0.0021f;
#if defined(__EMSCRIPTEN__)
        // Browser pointer-lock motion arrives as OS-accelerated CSS pixels (further divided
        // by the device pixel ratio), far fewer units per centimetre of mouse travel than
        // the raw relative counts SDL delivers on the desktop. Scale the Web look so one
        // physical mouse sweep turns a comparable angle on both.
        constexpr float MouseLookScale = 2.5f;
#else
        constexpr float MouseLookScale = 1.0f;
#endif
        constexpr float TouchLookSensitivity = 0.0032f;
        constexpr std::uint32_t SmokeTestFrames = 4U;
        constexpr std::uint32_t MonkeyCaptureFrames = 24U;
        constexpr std::size_t BenchmarkWarmupIntervals = 120U;
        constexpr std::size_t BenchmarkSampleIntervals = 600U;
        constexpr float LocomotionModeNoticeDuration = 1.4f;

        [[nodiscard]] const char* ViewpointName(const FixedViewpoint viewpoint) noexcept
        {
            switch (viewpoint) {
            case FixedViewpoint::Overview: return "overview";
            case FixedViewpoint::BayOverview: return "bay";
            case FixedViewpoint::BayBeachDetail: return "bay-ground";
            case FixedViewpoint::HeadlandDetail: return "headland";
            case FixedViewpoint::TidePoolDetail: return "tide-pools";
            case FixedViewpoint::TurtleNestDetail: return "turtle-nest";
            case FixedViewpoint::CaveExterior: return "cave";
            case FixedViewpoint::CaveOverhead: return "cave-top";
            case FixedViewpoint::CaveInterior: return "cave-inside";
            case FixedViewpoint::CaveBack: return "cave-back";
            case FixedViewpoint::BunkerExterior: return "bunker";
            case FixedViewpoint::BunkerCorridor: return "bunker-corridor";
            case FixedViewpoint::BunkerComputerRoom: return "bunker-computers";
            case FixedViewpoint::BunkerReactorRoom: return "bunker-reactor";
            case FixedViewpoint::BunkerServiceRoom: return "bunker-service";
            case FixedViewpoint::BunkerEndBulkhead: return "bunker-end";
            case FixedViewpoint::CampfireDetail: return "campfire";
            case FixedViewpoint::CampfireWide: return "campfire-wide";
            case FixedViewpoint::ResidentDetail: return "resident";
            case FixedViewpoint::ResidentTendDetail: return "resident-tending";
            case FixedViewpoint::ResidentFishDetail: return "resident-fishing";
            case FixedViewpoint::BearDetail: return "bear";
            case FixedViewpoint::BearLyingDetail: return "bear-lying";
            case FixedViewpoint::BeachPath: return "beach";
            case FixedViewpoint::JungleTrail: return "jungle";
            case FixedViewpoint::StandingSnagDetail: return "snags";
            case FixedViewpoint::SaplingDetail: return "sapling";
            case FixedViewpoint::EastClearingDetail: return "east";
            case FixedViewpoint::ClearingEdgeDetail: return "clearing-edge";
            case FixedViewpoint::CanopyInspection: return "canopy";
            case FixedViewpoint::WestClearingDetail: return "west";
            case FixedViewpoint::StreamDetail: return "stream";
            case FixedViewpoint::SeasonalWetlandDetail: return "wetland";
            case FixedViewpoint::MangroveDetail: return "mangroves";
            case FixedViewpoint::GrassDetail: return "grass";
            case FixedViewpoint::GroundPlantDetail: return "ground";
            case FixedViewpoint::GroundBroadleafDetail: return "ground-broadleaf";
            case FixedViewpoint::BushDetail: return "bush";
            case FixedViewpoint::FallenLeafDetail: return "litter";
            case FixedViewpoint::GroundContactDetail: return "contact";
            case FixedViewpoint::MovementTraceDetail: return "movement";
            case FixedViewpoint::PalmDetail: return "palms";
            case FixedViewpoint::FallenPalmDetail: return "fallen-palm";
            case FixedViewpoint::BrokenPalmDetail: return "broken-palm";
            case FixedViewpoint::WaterlineDetail: return "water";
            case FixedViewpoint::WetSandDetail: return "wet-sand";
            case FixedViewpoint::CurrentBoundaryDetail: return "current";
            case FixedViewpoint::DistantSceneryDetail: return "distant";
            case FixedViewpoint::PropDetail: return "props";
            case FixedViewpoint::DriftwoodDetail: return "driftwood";
            case FixedViewpoint::ReefDetail: return "reef";
            case FixedViewpoint::ReefHabitatDetail: return "reef-habitat";
            case FixedViewpoint::AnemoneDetail: return "anemone";
            case FixedViewpoint::WalkerShadowDetail: return "walker-shadow";
            case FixedViewpoint::FallenFrondDetail: return "fronds";
            case FixedViewpoint::BirdDetail: return "birds";
            case FixedViewpoint::BirdPreenDetail: return "bird-preen";
            case FixedViewpoint::GroundDoveDetail: return "ground-doves";
            case FixedViewpoint::FruitBatDetail: return "bats";
            case FixedViewpoint::FruitBatRoostDetail: return "bat-roost";
            case FixedViewpoint::FishDetail: return "fish";
            case FixedViewpoint::StingrayDetail: return "stingrays";
            case FixedViewpoint::ReefHeronDetail: return "herons";
            case FixedViewpoint::StingrayBuriedDetail: return "stingray-buried";
            case FixedViewpoint::OctopusDetail: return "octopuses";
            case FixedViewpoint::SharkDetail: return "shark";
            case FixedViewpoint::DolphinDetail: return "dolphins";
            case FixedViewpoint::MonkeyDetail: return "monkeys";
            case FixedViewpoint::MonkeyScratchDetail: return "monkey-scratch";
            case FixedViewpoint::MonkeyTreeDetail: return "monkey-tree";
            case FixedViewpoint::MonkeyClimbDetail: return "monkey-climb";
            case FixedViewpoint::ShipwreckDetail: return "shipwreck";
            case FixedViewpoint::ShipwreckWide: return "shipwreck-wide";
            case FixedViewpoint::AnimalTrackDetail: return "tracks";
            case FixedViewpoint::FruitTreeDetail: return "fruit";
            case FixedViewpoint::StreamCascadeDetail: return "cascade";
            case FixedViewpoint::JettyDetail: return "jetty";
            case FixedViewpoint::JettyEndDetail: return "jetty-end";
            case FixedViewpoint::FlightOverview: return "flight";
            case FixedViewpoint::BridgeDetail: return "bridge";
            case FixedViewpoint::LookoutDetail: return "lookout";
            case FixedViewpoint::FallenLogDetail: return "log";
            case FixedViewpoint::BeachVineDetail: return "beach-vine";
            case FixedViewpoint::HutDetail: return "hut";
            case FixedViewpoint::HutMaterialDetail: return "hut-material";
            case FixedViewpoint::HelicopterDetail: return "helicopter";
            case FixedViewpoint::HelicopterInteriorDetail: return "helicopter-inside";
            case FixedViewpoint::HelicopterApproach: return "helicopter-approach";
            case FixedViewpoint::QuadDetail: return "quad";
            case FixedViewpoint::QuadRideDetail: return "quad-ride";
            case FixedViewpoint::QuadChaseDetail: return "quad-chase";
            case FixedViewpoint::QuadDriveDetail: return "quad-drive";
            case FixedViewpoint::QuadBunkerDetail: return "quad-bunker";
            case FixedViewpoint::QuadCaveDetail: return "quad-cave";
            case FixedViewpoint::QuadJettyDetail: return "quad-jetty";
            case FixedViewpoint::QuadHelicopterDetail: return "quad-helicopter";
            case FixedViewpoint::QuadLookoutDetail: return "quad-lookout";
            case FixedViewpoint::QuadStreamDetail: return "quad-stream";
            case FixedViewpoint::QuadSeaDetail: return "quad-sea";
            case FixedViewpoint::QuadBearDetail: return "quad-bear";
            case FixedViewpoint::CrabDetail: return "crabs";
            case FixedViewpoint::HermitCrabDetail: return "hermit-crabs";
            case FixedViewpoint::CoconutCrabDetail: return "coconut-crabs";
            case FixedViewpoint::CrocodileDetail: return "crocodiles";
            case FixedViewpoint::BoarDetail: return "boars";
            case FixedViewpoint::BoarLyingDetail: return "boar-lying";
            case FixedViewpoint::GoatDetail: return "goats";
            case FixedViewpoint::GoatLyingDetail: return "goat-lying";
            case FixedViewpoint::DeerDetail: return "deer";
            case FixedViewpoint::DeerBeddedDetail: return "deer-bedded";
            case FixedViewpoint::SpiderDetail: return "spiders";
            case FixedViewpoint::SpiderBrightDetail: return "spiders-bright";
            case FixedViewpoint::SpiderWebDetail: return "spider-web";
            case FixedViewpoint::SnakeDetail: return "snakes";
            case FixedViewpoint::SnakeHeadDetail: return "snake-head";
            case FixedViewpoint::TreeSnakeDetail: return "tree-snakes";
            case FixedViewpoint::ButterflyDetail: return "butterflies";
            case FixedViewpoint::GeckoDetail: return "geckos";
            case FixedViewpoint::SkinkDetail: return "skinks";
            case FixedViewpoint::DragonflyDetail: return "dragonflies";
            case FixedViewpoint::FrogDetail: return "frogs";
            case FixedViewpoint::MouseDetail: return "mice";
            case FixedViewpoint::RatDetail: return "rats";
            case FixedViewpoint::FlyDetail: return "flies";
            case FixedViewpoint::AntDetail: return "ants";
            case FixedViewpoint::CicadaDetail: return "cicadas";
            case FixedViewpoint::BeetleDetail: return "beetles";
            case FixedViewpoint::SeaTurtleDetail: return "turtles";
            case FixedViewpoint::BenchmarkTraversal: return "traversal";
            case FixedViewpoint::None: return "interactive";
            }
            return "unknown";
        }

        [[nodiscard]] const char* TouchSensitivityName(
            const TouchLookSensitivityLevel sensitivity) noexcept
        {
            switch (sensitivity) {
            case TouchLookSensitivityLevel::Low: return "low";
            case TouchLookSensitivityLevel::Standard: return "standard";
            case TouchLookSensitivityLevel::High: return "high";
            }
            return "standard";
        }
    }

#if defined(__EMSCRIPTEN__)
    extern "C" EMSCRIPTEN_KEEPALIVE void IslandSetWebMovementKeyState(
        const int bit, const int down) noexcept
    {
        if (bit <= 0 || (bit & ~15) != 0) return;
        if (down != 0) WebMovementKeyMaskValue |= bit;
        else WebMovementKeyMaskValue &= ~bit;
    }
#endif

    IslandGame::IslandGame(const bool smokeTest, std::string capturePath,
                           FixedViewpoint viewpoint, const bool showDebugOverlay,
                           const bool inputDiagnostics, const bool showWorldDebug,
                           std::string benchmarkPath, std::string startupReportPath,
                           const bool userSettingsEnabled,
                           std::optional<CanopyLodInspection> canopyLodInspection)
        : graphics_(this),
          birdFlock_(terrain_),
          butterflyMeadow_(terrain_),
          geckoPopulation_(terrain_),
          skinkPopulation_(terrain_),
          dragonflySwarm_(terrain_),
          frogPopulation_(terrain_),
          rodentPopulation_(terrain_),
          flySwarms_(terrain_),
          crabColony_(terrain_),
          hermitCrabGroup_(terrain_),
          coconutCrabGroup_(terrain_),
          crocodileGroup_(terrain_),
          boarHerd_(terrain_),
          fishSchools_(terrain_),
          stingrayGroup_(terrain_),
          reefHeronGroup_(terrain_),
          octopusGroup_(terrain_),
          seaTurtleGroup_(terrain_),
          sharkPatrol_(terrain_),
          dolphinPod_(terrain_),
          monkeyTroop_(terrain_),
          vegetationLayout_(terrain_),
          jungleLayout_(terrain_),
          antTrails_(terrain_,jungleLayout_),
          cicadaColony_(jungleLayout_),
          beetlePopulation_(terrain_),
          fruitBatColony_(terrain_, jungleLayout_.Canopy()),
          spiderWebSites_(SpiderWebLayout::Propose(jungleLayout_,RenderedTerrainSurface(terrain_))),
          canopyLightField_(terrain_, MakeCanopyOccluders(
              jungleLayout_.Canopy(), vegetationLayout_.Palms())),
          grassLayout_(terrain_, canopyLightField_, groundCoverField_),
          naturalPropLayout_(terrain_, vegetationLayout_, jungleLayout_),
          shipwreckLayout_(terrain_),
          birdPerchLayout_(terrain_, jungleLayout_, naturalPropLayout_, shipwreckLayout_),
          sunShade_(terrain_,jungleLayout_),
          cameraPosition_(VegetationLayout::PlayerSpawnX, 3.0f,
                          VegetationLayout::PlayerSpawnZ),
          renderCameraPosition_(cameraPosition_),
          smokeTest_(smokeTest),
          viewpoint_(viewpoint),
          capturePath_(std::move(capturePath)),
          canopyLodInspection_(canopyLodInspection),
          debugOverlayVisible_(showDebugOverlay),
          worldDebugVisible_(showWorldDebug),
          userSettingsEnabled_(userSettingsEnabled),
          inputDiagnostics_(inputDiagnostics),
          benchmarkPath_(std::move(benchmarkPath)),
          startupReportPath_(std::move(startupReportPath))
    {
        std::vector<CrabHabitatCue> crabDebrisCues;
        crabDebrisCues.reserve(naturalPropLayout_.Driftwood().size()
                               + naturalPropLayout_.BeachDetails().size());
        for (const auto& wood : naturalPropLayout_.Driftwood())
            crabDebrisCues.push_back({wood.x, wood.z});
        for (const auto& detail : naturalPropLayout_.BeachDetails())
            if (detail.variant == 3U) crabDebrisCues.push_back({detail.x, detail.z});
        crabColony_.SetDebrisHabitats(crabDebrisCues);
        std::vector<CircularObstacle> worldObstacles;
        worldObstacles.reserve(vegetationLayout_.Obstacles().size()
                               + jungleLayout_.Obstacles().size()
                               + naturalPropLayout_.Obstacles().size()
                               + shipwreckLayout_.Obstacles().size());
        worldObstacles.insert(worldObstacles.end(), vegetationLayout_.Obstacles().begin(),
                              vegetationLayout_.Obstacles().end());
        worldObstacles.insert(worldObstacles.end(), jungleLayout_.Obstacles().begin(),
                              jungleLayout_.Obstacles().end());
        worldObstacles.insert(worldObstacles.end(), naturalPropLayout_.Obstacles().begin(),
                              naturalPropLayout_.Obstacles().end());
        worldObstacles.insert(worldObstacles.end(), shipwreckLayout_.Obstacles().begin(),
                              shipwreckLayout_.Obstacles().end());
        crocodileGroup_.SetObstacles(worldObstacles);
        const auto fireObstacles=campfireLayout_.Obstacles();
        worldObstacles.insert(worldObstacles.end(),fireObstacles.begin(),fireObstacles.end());
        hutPlacement_=HutLayout::FindInWorld(terrain_,jungleLayout_,worldObstacles);
        if(hutPlacement_) worldObstacles.push_back({hutPlacement_->x,hutPlacement_->z,HutLayout::CollisionRadius});
        if(hutPlacement_) jettyPlacement_=JettyLayout::FindInWorld(terrain_,*hutPlacement_);
        if(jettyPlacement_) {
            for(const auto& obstacle:jettyPlacement_->CanoeObstacles()) worldObstacles.push_back(obstacle);
            birdPerchLayout_.AddSite(jettyPlacement_->MooringPerch());
            // The water under the deck is too shallow for a school; the first point out along
            // the jetty's line deep enough is the shade off its end a passing school turns into.
            for(float along=5.0f;along<=40.0f;along+=1.0f) {
                const float x=jettyPlacement_->rootX+jettyPlacement_->dirX*along;
                const float z=jettyPlacement_->rootZ+jettyPlacement_->dirZ*along;
                if(-terrain_.Height(x,z)>=FishSchools::ShelterDepth) {fishSchools_.SetShelter(x,z,true);break;}
            }
        }
        if(hutPlacement_) birdPerchLayout_.AddSite(HutLayout::RidgePerch(*hutPlacement_));
        if(hutPlacement_) lookoutPlacement_=LookoutLayout::FindInWorld(terrain_,*hutPlacement_,worldObstacles);
        if(lookoutPlacement_) {
            for(const auto& obstacle:lookoutPlacement_->PostObstacles()) worldObstacles.push_back(obstacle);
            birdPerchLayout_.AddSite(lookoutPlacement_->RailPerch());
        }
        birdFlock_.SetPerchSites(birdPerchLayout_.Sites());
        {
            // The reef herons roost on the coast: in the crowns of the shore's palms (the crown
            // anchor the canopy light field uses: the curved stem and the wind's lean) and on
            // the perches near the water, the wreck, the rocks, the jetty's post.
            std::vector<ReefHeronGroup::Roost> roosts;
            for (const PalmPlacement& palm : vegetationLayout_.Palms()) {
                if (palm.condition != PalmCondition::Living || palm.shoreDistance > 35.0f) continue;
                const float variant = static_cast<float>(palm.variant);
                const float height = 9.4f + variant * 0.55f;
                const float curveAngle = 0.72f + variant * 1.37f;
                const float internalLean = 0.55f + static_cast<float>((palm.variant * 5U) % 3U) * 0.25f;
                const float localX = std::cos(curveAngle) * internalLean, localZ = std::sin(curveAngle) * internalLean;
                const float cosine = std::cos(palm.yawRadians), sine = std::sin(palm.yawRadians);
                const float windLean = wind::PalmLeanRadians(wind::Exposure(palm.x, palm.z, palm.shoreDistance));
                roosts.push_back({palm.x + (localX * cosine + localZ * sine) * palm.scale
                                      + wind::DirectionX * std::sin(windLean) * height * palm.scale,
                                  palm.y + std::cos(windLean) * height * palm.scale + 0.35f * palm.scale,
                                  palm.z + (-localX * sine + localZ * cosine) * palm.scale
                                      + wind::DirectionZ * std::sin(windLean) * height * palm.scale});
            }
            for (const BirdPerchSite& site : birdPerchLayout_.Sites())
                if (terrain_.SignedShoreDistance(site.x, site.z) < 40.0f) roosts.push_back({site.x, site.y, site.z});
            reefHeronGroup_.SetRoosts(roosts);
        }
        {
            // The plunge pool's lower bank, a little outside the water's edge, is where the
            // fruit doves come down to bathe.
            const StreamCourse course(terrain_);
            const StreamStation& pool=course.PoolStation();
            const float reach=pool.halfWidth+0.7f;
            const float side=terrain_.Height(pool.x,pool.z+reach)<=terrain_.Height(pool.x,pool.z-reach)?1.0f:-1.0f;
            birdFlock_.SetBathSite(pool.x,pool.z+side*reach,true);
        }
        {
            // The turtle nests among the beach details are where the hatchlings break out.
            std::vector<HatchlingNest> nests;
            for (const auto& detail : naturalPropLayout_.BeachDetails())
                if (detail.turtleNest) nests.push_back({detail.x, detail.z});
            hatchlings_.SetNests(nests);
        }
        surfSpray_.SetRocks(naturalPropLayout_.Rocks());
        butterflyMeadow_.SetFlowerBushes(jungleLayout_.Bushes());
        canopyLeafFall_.SetCrowns(jungleLayout_.Canopy());
        bridgePlacement_=BridgeLayout::FindInWorld(terrain_,StreamCourse(terrain_));
        helicopterPlacement_=HelicopterLayout::FindInWorld(terrain_,jungleLayout_,worldObstacles);
        if(!helicopterPlacement_)throw std::runtime_error("No valid central helicopter site");
        // The animals' lighting reads the solid casters the sun shadow map draws: the jungle's
        // snags, understory and bushes (baked with the field) and the structures, so a goat
        // under the hut's eave or a boar behind a trunk darkens as the ground there does.
        sunShade_.AddDisc(hutPlacement_->x,hutPlacement_->floorY+1.6f,hutPlacement_->z,2.7f,SunShadeField::StructureShade);
        if(lookoutPlacement_) sunShade_.AddDisc(lookoutPlacement_->x,lookoutPlacement_->platformY+0.3f,lookoutPlacement_->z,
            LookoutPlacement::Half*1.4f,SunShadeField::StructureShade);
        sunShade_.AddDisc(helicopterPlacement_->x,helicopterPlacement_->floorY+1.0f,helicopterPlacement_->z,2.2f,SunShadeField::StructureShade);
        canopyLightField_.SetSunShade(&sunShade_);
        std::cout<<"island-simulator: the animals' lighting reads the sun's solid casters: "<<sunShade_.CasterCount()
                 <<" snag, understory, bush and structure shadows darken a body standing in them by up to "
                 <<static_cast<int>(CanopyLightField::SunShadeDepth*100.0f+0.5f)<<" percent; the ground keeps its porous canopy reading\n";
        worldCollisionGrid_.Reset(worldObstacles);
        playerMotion_.SetHelicopter(&*helicopterPlacement_);
        if(jettyPlacement_) playerMotion_.SetJetty(&*jettyPlacement_);
        if(bridgePlacement_) playerMotion_.SetBridge(&*bridgePlacement_);
        if(lookoutPlacement_) playerMotion_.SetLookout(&*lookoutPlacement_);
        std::vector<CircularObstacle> wildlifeObstacles(worldObstacles);
        // Keep complete animal bodies out of the authored rotor/clearing footprint,
        // not merely outside the fuselage collision proxy. Goat habitat selection
        // previously accepted a home under an open cabin door because 3.2 m only
        // represented the airframe itself.
        wildlifeObstacles.push_back({helicopterPlacement_->x,helicopterPlacement_->z,
                                     HelicopterSite::ClearingRadius});
        // The resident's tour also calls at the jetty foot, three metres up the sand from the
        // deck's root, when the route search can reach it.
        std::vector<ResidentPoint> residentLandmarks,residentFacings;
        if(jettyPlacement_) {
            residentLandmarks.push_back({jettyPlacement_->rootX-jettyPlacement_->dirX*3.0f,0.0f,
                jettyPlacement_->rootZ-jettyPlacement_->dirZ*3.0f});
            residentFacings.push_back({jettyPlacement_->dirX,0.0f,jettyPlacement_->dirZ});
        }
        // The tour also calls at the lookout's ramp foot, two metres beyond the ramp's end,
        // where the resident turns to look up the ramp at the platform.
        if(lookoutPlacement_) {
            const float footAlong=LookoutPlacement::RampLength+2.0f;
            residentLandmarks.push_back({lookoutPlacement_->x+lookoutPlacement_->dirX*footAlong,0.0f,
                lookoutPlacement_->z+lookoutPlacement_->dirZ*footAlong});
            residentFacings.push_back({-lookoutPlacement_->dirX,0.0f,-lookoutPlacement_->dirZ});
        }
        resident_=std::make_unique<ResidentMotion>(terrain_,wildlifeObstacles,residentLandmarks,residentFacings);
        // The jetty-foot call is a fishing pause: the rod comes out there.
        if(jettyPlacement_) resident_->SetFishingSpot(jettyPlacement_->rootX-jettyPlacement_->dirX*3.0f,
            jettyPlacement_->rootZ-jettyPlacement_->dirZ*3.0f);
        std::cout<<"island-simulator: resident tour visits "<<resident_->Route().LandmarkCount()
                 <<" landmark(s): the jetty foot and the lookout's ramp foot join the campfire, path and clearing calls; "
                 <<"at the jetty foot the resident turns to look out along the deck to sea; at the lookout's ramp foot the resident looks up the ramp at the platform; "
                 <<"at the campfire the resident stands facing the fire; "
                 <<"the resident raises a hand to a player who stops close by\n";
        bear_=std::make_unique<BearMotion>(terrain_,wildlifeObstacles);
        monkeyTroop_.SetObstacles(wildlifeObstacles);
        monkeyTroop_.SetCanopy(jungleLayout_.Canopy());
        boarHerd_.SetObstacles(wildlifeObstacles);
        snakes_ = std::make_unique<SnakePopulation>(
            terrain_,jungleLayout_,wildlifeObstacles);
        goats_ = std::make_unique<GoatHerd>(terrain_,jungleLayout_,wildlifeObstacles);
        {
            // Coastal scrub on the banks every other plant rule leaves bare, kept off the
            // goats' basin, the lookout's footprint and the jetty's root.
            std::vector<SlopeScrubLayout::ClearCircle> clear;
            if (goats_->WaterholeSite().valid)
                clear.push_back({goats_->WaterholeSite().x, goats_->WaterholeSite().z, GoatHerd::WaterholeRadius + 2.5f});
            if (lookoutPlacement_)
                clear.push_back({lookoutPlacement_->x, lookoutPlacement_->z, LookoutPlacement::RampLength + 2.0f});
            if (jettyPlacement_)
                clear.push_back({jettyPlacement_->rootX, jettyPlacement_->rootZ, 7.0f});
            slopeScrub_ = SlopeScrubLayout::Generate(terrain_, clear);
        }
        deer_ = std::make_unique<DeerHerd>(terrain_,wildlifeObstacles);
        deer_->SetLight(&canopyLightField_);
        for(auto& trail:deerPrints_) { trail.SetKind(1U); trail.SetStride(0.9f); }
        for(auto& trail:boarPrints_) { trail.SetKind(1U); trail.SetStride(0.55f); }
        for(auto& trail:goatPrints_) { trail.SetKind(1U); trail.SetStride(0.45f); }
        spiders_ = std::make_unique<SpiderColony>(terrain_,jungleLayout_,wildlifeObstacles);
        if (smokeTest_ && viewpoint_ == FixedViewpoint::None) {
            viewpoint_ = FixedViewpoint::Overview;
        }
        if (!benchmarkPath_.empty() && viewpoint_ == FixedViewpoint::None) {
            viewpoint_ = FixedViewpoint::Overview;
        }
        if (viewpoint_ == FixedViewpoint::BenchmarkTraversal) {
            const BenchmarkRouteSample start = benchmarkTraversalRoute_.Sample(0.0f);
            cameraPosition_ = Vector3(start.x, terrain_.Height(start.x, start.z)
                                                  + PlayerMotion::EyeHeight,
                                      start.z);
            yaw_ = std::atan2(start.forwardX, start.forwardZ);
            pitch_ = -0.025f;
        } else if (viewpoint_ == FixedViewpoint::Overview) {
            cameraPosition_ = Vector3(0.0f, 68.0f, -365.0f);
            pitch_ = -0.16f;
        } else if (viewpoint_ == FixedViewpoint::BayOverview) {
            cameraPosition_ = Vector3(-365.0f, 115.0f, 0.0f);
            yaw_ = std::numbers::pi_v<float> * 0.5f;
            pitch_ = -0.38f;
        } else if (viewpoint_ == FixedViewpoint::BayBeachDetail) {
            constexpr float x = -147.0f, z = -8.0f;
            cameraPosition_ = Vector3(x, terrain_.Height(x, z) + PlayerMotion::EyeHeight, z);
            yaw_ = 0.12f;
            pitch_ = -0.07f;
        } else if (viewpoint_ == FixedViewpoint::WalkerShadowDetail) {
            // Stand on the open bay beach with the sun at the back and look down along one's own
            // shadow, aimed at its middle (the shading sun stands at least 15 degrees high).
            constexpr float x = -147.0f, z = -8.0f;
            cameraPosition_ = Vector3(x, terrain_.Height(x, z) + PlayerMotion::EyeHeight, z);
            const float hour = day_night_options::RequestedHour() >= 0.0f ? day_night_options::RequestedHour()
                                                                         : DayNightCycle::DefaultHour;
            const DayVec3 toSun = DayNightCycle::SunDirectionAt(hour);
            const float horizontal = std::max(std::hypot(toSun.x, toSun.z), 1e-3f);
            yaw_ = std::atan2(-toSun.x, -toSun.z);
            const float height = std::max(toSun.y, std::sin(sun_light::MinimumShadingElevationDegrees
                                                             * std::numbers::pi_v<float> / 180.0f));
            pitch_ = -std::atan2(PlayerMotion::EyeHeight, 0.5f * 1.75f * horizontal / height + 0.3f);
            std::cout << "island-simulator: walker-shadow QA camera looks down along the walker's own shadow on the bay beach\n";
        } else if (viewpoint_ == FixedViewpoint::HeadlandDetail) {
            cameraPosition_ = Vector3(300.0f, 28.0f, 110.0f);
            yaw_ = std::atan2(-63.0f, -23.0f);
            pitch_ = -0.33f;
        } else if(viewpoint_==FixedViewpoint::CampfireDetail||viewpoint_==FixedViewpoint::CampfireWide) {
            const auto fire=campfireLayout_.Centre();
            const bool wide=viewpoint_==FixedViewpoint::CampfireWide;
            const float x=fire.x-(wide?12.0f:3.8f),z=fire.z+(wide?15.0f:4.8f);
            cameraPosition_=Vector3(x,RenderedTerrainSurface(terrain_).Sample(x,z).height+(wide?6.0f:2.4f),z);
            const Vector3 target(fire.x+(wide?2.0f:0),fire.y+.65f,fire.z-1);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: campfire QA camera "<<(wide?"wide":"near")<<'\n';
        } else if(viewpoint_==FixedViewpoint::ResidentDetail||viewpoint_==FixedViewpoint::ResidentTendDetail
                  ||viewpoint_==FixedViewpoint::ResidentFishDetail) {
            // The tending view holds the resident crouched as at the fire, framed lower; the
            // fishing view holds the rod out, framed from ahead and to the right so the rod
            // and the line read.
            const bool tending=viewpoint_==FixedViewpoint::ResidentTendDetail;
            const bool fishing=viewpoint_==FixedViewpoint::ResidentFishDetail;
            if(tending) resident_->PreviewTending();
            if(fishing) resident_->PreviewFishing();
            const auto& subject=resident_->State();
            const Vector3 forward(std::sin(subject.heading),0,std::cos(subject.heading));
            const Vector3 right(std::cos(subject.heading),0,-std::sin(subject.heading));
            // (tending, they face the fire at the pit's edge: framed from the side, the
            // resident and the flames together)
            cameraPosition_=Vector3(subject.x,subject.y+(tending?1.25f:fishing?1.45f:1.58f),subject.z)
                +forward*(fishing?3.4f:tending?0.8f:2.65f)+right*(fishing?2.6f:tending?2.9f:2.00f);
            const float lead=fishing?.9f:tending?.85f:0.0f;
            const Vector3 target(subject.x+forward.X*lead,subject.y+(tending?.72f:fishing?1.0f:1.02f),subject.z+forward.Z*lead);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<(tending?"island-simulator: resident QA camera frames the resident crouched to tend the fire\n"
                        :fishing?"island-simulator: resident QA camera frames the resident fishing, rod out and line to the float\n"
                               :"island-simulator: resident QA camera frames and follows one peaceful adult\n");
        } else if(viewpoint_==FixedViewpoint::BearDetail||viewpoint_==FixedViewpoint::BearLyingDetail) {
            // The lying view holds the bear down at rest, framed lower and nearer.
            const bool lying=viewpoint_==FixedViewpoint::BearLyingDetail;
            if(lying) bear_->PreviewLie();
            const auto& subject=bear_->State();
            const Vector3 forward(std::sin(subject.heading),0,std::cos(subject.heading));
            const Vector3 right(std::cos(subject.heading),0,-std::sin(subject.heading));
            cameraPosition_=Vector3(subject.x,subject.y+(lying?1.45f:2.05f),subject.z)+forward*(lying?4.6f:5.3f)+right*(lying?2.4f:2.35f);
            const Vector3 target(subject.x,subject.y+(lying?.55f:1.03f),subject.z+.22f);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<(lying?"island-simulator: bear QA camera frames the bear lying at rest on folded legs\n"
                             :"island-simulator: bear QA camera frames and follows the single peaceful adult\n");
        } else if(viewpoint_==FixedViewpoint::HelicopterApproach) {
            // An ordinary first-person sight of the aircraft: eye height on the real ground
            // just inside the clearing off its front-left quarter, not an elevated QA frame.
            // The vegetation clearing is only 6.2 m: from the old eight metres the plants at
            // its edge filled half the frame.
            const auto foot=helicopterPlacement_->World(-4.9f,0.0f,3.3f);
            cameraPosition_=Vector3(foot.x,terrain_.Height(foot.x,foot.z)+PlayerMotion::EyeHeight,foot.z);
            const auto target=helicopterPlacement_->World(0.0f,1.35f,0.0f);
            const Vector3 direction=Vector3(target.x,target.y,target.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: helicopter QA camera frames an ordinary eye-height approach from the front-left quarter\n";
        } else if(viewpoint_==FixedViewpoint::HelicopterDetail
                  ||viewpoint_==FixedViewpoint::HelicopterInteriorDetail) {
            const bool inside=viewpoint_==FixedViewpoint::HelicopterInteriorDetail;
            const auto camera=inside
                ?helicopterPlacement_->World(0.0f,1.64f,-.03f)
                :helicopterPlacement_->World(-6.1f,2.8f,4.0f);
            const auto target=inside?helicopterPlacement_->World(0.0f,.72f,1.08f)
                :helicopterPlacement_->World(0.0f,1.40f,-.25f);
            cameraPosition_=Vector3(camera.x,camera.y,camera.z);
            const Vector3 direction=Vector3(target.x,target.y,target.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: helicopter QA camera frames "
                <<(inside?"centre aisle across cockpit controls and accessible footwell":"grounded exterior and open doors")<<'\n';
        } else if (viewpoint_ == FixedViewpoint::CaveExterior
                   || viewpoint_ == FixedViewpoint::CaveOverhead
                   || viewpoint_ == FixedViewpoint::CaveInterior
                   || viewpoint_ == FixedViewpoint::CaveBack) {
            const CaveLayout cave(terrain_);
            const bool inside=viewpoint_==FixedViewpoint::CaveInterior;
            const bool overhead=viewpoint_==FixedViewpoint::CaveOverhead;
            const bool back=viewpoint_==FixedViewpoint::CaveBack;
            const auto p=back?cave.World(.25f,CaveLayout::Floor(15)+PlayerMotion::EyeHeight,15)
                :inside?cave.World(.6f,CaveLayout::Floor(12)+PlayerMotion::EyeHeight,12)
                :overhead?cave.World(7.0f,18.0f,-6.0f):cave.World(12,8,-24);
            const auto target=back?cave.World(.15f,CaveLayout::Floor(20)+1.08f,21.0f)
                :inside?cave.World(0,1.6f,-3)
                :overhead?cave.World(0,2.0f,8.0f):cave.World(0,2.1f,7);
            cameraPosition_=Vector3(p.x,p.y,p.z);
            yaw_=std::atan2(target.x-p.x,target.z-p.z);
            pitch_=std::atan2(target.y-p.y,std::hypot(target.x-p.x,target.z-p.z));
            std::cout<<"island-simulator: cave QA camera "
                <<(back?"back":inside?"inside":overhead?"overhead":"outside")<<'\n';
        } else if(viewpoint_==FixedViewpoint::BunkerExterior
                  ||viewpoint_==FixedViewpoint::BunkerCorridor
                  ||viewpoint_==FixedViewpoint::BunkerComputerRoom
                  ||viewpoint_==FixedViewpoint::BunkerReactorRoom
                  ||viewpoint_==FixedViewpoint::BunkerServiceRoom
                  ||viewpoint_==FixedViewpoint::BunkerEndBulkhead) {
            BunkerPoint camera,target;
            if(viewpoint_==FixedViewpoint::BunkerExterior) {
                camera=bunkerLayout_.World(-7.2f,4.1f,-10.5f);
                target=bunkerLayout_.World(0,1.35f,.5f);
            } else if(viewpoint_==FixedViewpoint::BunkerCorridor) {
                camera=bunkerLayout_.World(0,bunkerLayout_.FloorRelative(5)+1.72f,5);
                target=bunkerLayout_.World(0,bunkerLayout_.FloorRelative(19)+1.45f,19);
            } else if(viewpoint_==FixedViewpoint::BunkerComputerRoom) {
                camera=bunkerLayout_.World(2.45f,bunkerLayout_.FloorRelative(17)+1.72f,17);
                target=bunkerLayout_.World(6.4f,bunkerLayout_.FloorRelative(17)+1.18f,17);
            } else if(viewpoint_==FixedViewpoint::BunkerReactorRoom) {
                camera=bunkerLayout_.World(-2.3f,bunkerLayout_.FloorRelative(29.5f)+1.72f,29.5f);
                target=bunkerLayout_.World(-5.55f,bunkerLayout_.FloorRelative(29.5f)+1.28f,29.5f);
            } else if(viewpoint_==FixedViewpoint::BunkerServiceRoom) {
                camera=bunkerLayout_.World(2.05f,bunkerLayout_.FloorRelative(31.2f)+1.72f,31.2f);
                target=bunkerLayout_.World(6.55f,bunkerLayout_.FloorRelative(31.2f)+1.25f,31.2f);
            } else {
                camera=bunkerLayout_.World(0,bunkerLayout_.FloorRelative(31.4f)+1.72f,31.4f);
                target=bunkerLayout_.World(0,bunkerLayout_.FloorRelative(35.9f)+1.30f,35.9f);
            }
            cameraPosition_={camera.x,camera.y,camera.z};
            const Vector3 direction=Vector3(target.x,target.y,target.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: bunker QA camera "<<ViewpointName(viewpoint_)<<'\n';
        } else if (viewpoint_ == FixedViewpoint::TidePoolDetail) {
            const auto [poolX,poolZ]=TidePoolLayout::Centre(terrain_,0U);
            const float radialLength=std::hypot(poolX,poolZ);
            const float outwardX=poolX/radialLength,outwardZ=poolZ/radialLength;
            const float cameraX=poolX-outwardX*8.0f-outwardZ*6.0f;
            const float cameraZ=poolZ-outwardZ*8.0f+outwardX*6.0f;
            cameraPosition_=Vector3(cameraX,terrain_.Height(cameraX,cameraZ)+4.5f,cameraZ);
            const Vector3 target(poolX,TidePoolLayout::WaterLevel(terrain_,0U),poolZ);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout << "island-simulator: tide-pool QA camera targets rocky basin water\n";
        } else if (viewpoint_ == FixedViewpoint::TurtleNestDetail) {
            const auto nest = std::ranges::find_if(
                naturalPropLayout_.BeachDetails(),
                [](const BeachDetailPlacement& detail) { return detail.turtleNest; });
            if (nest == naturalPropLayout_.BeachDetails().end())
                throw std::runtime_error("Turtle-nest QA camera found no target");
            const float radialLength = std::hypot(nest->x, nest->z);
            const Vector3 outward(nest->x / radialLength, 0.0f, nest->z / radialLength);
            const Vector3 tangent(-outward.Z, 0.0f, outward.X);
            const float cameraX = nest->x - outward.X * 3.2f + tangent.X * 2.2f;
            const float cameraZ = nest->z - outward.Z * 3.2f + tangent.Z * 2.2f;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ) + 2.4f,
                                      cameraZ);
            const Vector3 target = Vector3(nest->x, nest->y, nest->z)
                + outward * 3.4f + Vector3::Up * 0.05f;
            const Vector3 direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: turtle-nest QA camera follows paired tracks from "
                         "disturbed dry sand toward the sea\n";
        } else if (viewpoint_ == FixedViewpoint::BeachPath) {
            constexpr float pathZ = -190.0f;
            const float pathX = VegetationLayout::PrimaryPathCenterX(pathZ);
            cameraPosition_ = Vector3(pathX, terrain_.Height(pathX, pathZ)
                                                 + PlayerMotion::EyeHeight,
                                      pathZ);
            pitch_ = -0.035f;
        } else if (viewpoint_ == FixedViewpoint::JungleTrail) {
            constexpr float pathZ = 45.0f;
            const float pathX = VegetationLayout::PrimaryPathCenterX(pathZ);
            cameraPosition_ = Vector3(pathX, terrain_.Height(pathX, pathZ)
                                                 + PlayerMotion::EyeHeight,
                                      pathZ);
            yaw_ = std::numbers::pi_v<float> * 0.5f;
            pitch_ = -0.065f;
        } else if (viewpoint_ == FixedViewpoint::StandingSnagDetail) {
            if (jungleLayout_.StandingSnags().empty())
                throw std::runtime_error("Standing-snag QA camera found no target");
            const auto& snag = jungleLayout_.StandingSnags().front();
            const Vector3 forward(std::sin(snag.yawRadians), 0.0f,
                                  std::cos(snag.yawRadians));
            const Vector3 right(forward.Z, 0.0f, -forward.X);
            const float cameraX = snag.x + forward.X * 7.2f + right.X * 2.8f;
            const float cameraZ = snag.z + forward.Z * 7.2f + right.Z * 2.8f;
            cameraPosition_ = Vector3(cameraX,
                terrain_.Height(cameraX, cameraZ) + 2.5f, cameraZ);
            const Vector3 target(snag.x, snag.y + 3.7f * snag.scale, snag.z);
            const Vector3 direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: standing-snag QA target variant="
                      << static_cast<int>(snag.variant) << " scale=" << snag.scale
                      << " position=" << snag.x << ',' << snag.y << ',' << snag.z << '\n';
        } else if (viewpoint_ == FixedViewpoint::SaplingDetail) {
            const auto selected = std::ranges::find_if(jungleLayout_.Canopy(), [](const auto& tree) {
                return tree.variant != MangroveCanopyVariant
                    && tree.scale < JungleLayout::CanopyMinimumScale;
            });
            if (selected == jungleLayout_.Canopy().end())
                throw std::runtime_error("Sapling QA camera found no target");
            const Vector3 forward(std::sin(selected->yawRadians), 0.0f,
                                  std::cos(selected->yawRadians));
            const Vector3 right(forward.Z, 0.0f, -forward.X);
            const float cameraX = selected->x + forward.X * 5.0f + right.X * 1.5f;
            const float cameraZ = selected->z + forward.Z * 5.0f + right.Z * 1.5f;
            cameraPosition_ = Vector3(cameraX,
                terrain_.Height(cameraX, cameraZ) + 2.2f, cameraZ);
            const float stature = CanopyNominalHeight(selected->variant) * selected->scale;
            const Vector3 target(selected->x, selected->y + stature * .52f, selected->z);
            const Vector3 direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: sapling QA target variant="
                      << static_cast<int>(selected->variant) << " scale=" << selected->scale
                      << " nominal-height=" << stature << " position=" << selected->x << ','
                      << selected->y << ',' << selected->z << '\n';
        } else if (viewpoint_ == FixedViewpoint::EastClearingDetail) {
            // The campfire pit sits at the clearing's authored centre; the eye stands off to
            // the east of it (past the stored wood) and looks west across the clearing.
            const ClearingDefinition& clearing = VegetationLayout::EastClearing;
            const float cameraX = clearing.x + 8.0f, cameraZ = clearing.z - 3.0f;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ)
                                                   + PlayerMotion::EyeHeight,
                                      cameraZ);
            yaw_ = -std::numbers::pi_v<float> * 0.5f;
            pitch_ = -0.085f;
            std::cout << "island-simulator: east clearing QA camera east of the campfire at the authored centre\n";
        } else if (viewpoint_ == FixedViewpoint::ClearingEdgeDetail) {
            // Fixed world coordinates keep comparisons stable if colony placement changes.
            const auto& clearing = VegetationLayout::EastClearing;
            cameraPosition_ = Vector3(clearing.x + 26, terrain_.Height(clearing.x + 26, clearing.z + 6)
                + PlayerMotion::EyeHeight, clearing.z + 6);
            const Vector3 target(clearing.x + 40, terrain_.Height(clearing.x + 40, clearing.z + 10) + 0.4f,
                clearing.z + 10);
            const auto direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
        } else if (viewpoint_ == FixedViewpoint::StreamDetail) {
            // Review the bank from the same eye height as ordinary walking; the former
            // elevated overview hid the shallow but physical cross-section.
            cameraPosition_=Vector3(-110,terrain_.Height(-110,-37)+PlayerMotion::EyeHeight,-37);
            const Vector3 target(-122,terrain_.Height(-122,-26.94f)-.18f,-26.94f);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        } else if (viewpoint_ == FixedViewpoint::SeasonalWetlandDetail) {
            const float cameraX=IslandEnvironmentField::SeasonalWetlandX;
            const float cameraZ=IslandEnvironmentField::SeasonalWetlandZ+20.0f;
            cameraPosition_=Vector3(cameraX,terrain_.Height(cameraX,cameraZ)
                +PlayerMotion::EyeHeight,cameraZ);
            const float targetX=IslandEnvironmentField::SeasonalWetlandX;
            const float targetZ=IslandEnvironmentField::SeasonalWetlandZ;
            const Vector3 target(targetX,terrain_.Height(targetX,targetZ)+.35f,targetZ);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        } else if (viewpoint_ == FixedViewpoint::MangroveDetail) {
            const auto selected = std::max_element(jungleLayout_.Canopy().begin(),
                jungleLayout_.Canopy().end(), [](const CanopyPlacement& left,
                                                  const CanopyPlacement& right) {
                    const float leftRadius = left.variant == MangroveCanopyVariant
                        ? std::hypot(left.x, left.z) : -1.0f;
                    const float rightRadius = right.variant == MangroveCanopyVariant
                        ? std::hypot(right.x, right.z) : -1.0f;
                    return leftRadius < rightRadius;
                });
            if (selected == jungleLayout_.Canopy().end()
                || selected->variant != MangroveCanopyVariant)
                throw std::runtime_error("Mangrove QA camera found no target");
            const float radius = std::hypot(selected->x, selected->z);
            const float outwardX = selected->x / radius;
            const float outwardZ = selected->z / radius;
            const float cameraX = selected->x + outwardX * 8.5f - outwardZ * 2.2f;
            const float cameraZ = selected->z + outwardZ * 8.5f + outwardX * 2.2f;
            cameraPosition_ = Vector3(cameraX, selected->y + PlayerMotion::EyeHeight, cameraZ);
            const Vector3 target(selected->x, selected->y + 3.4f * selected->scale, selected->z);
            const auto direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: mangrove QA target exposes stilt roots and low crown\n";
        } else if (viewpoint_ == FixedViewpoint::WestClearingDetail) {
            const ClearingDefinition& clearing = VegetationLayout::WestClearing;
            cameraPosition_ = Vector3(clearing.x, terrain_.Height(clearing.x, clearing.z)
                                                   + PlayerMotion::EyeHeight,
                                      clearing.z);
            yaw_ = std::numbers::pi_v<float> * 0.25f;
            pitch_ = -0.085f;
            std::cout << "island-simulator: west clearing QA camera at authored centre\n";
        } else if (viewpoint_ == FixedViewpoint::GroundContactDetail) {
            constexpr float contactZ = 45.0f;
            const float contactX = VegetationLayout::PrimaryPathCenterX(contactZ);
            const Vector3 target(contactX, terrain_.Height(contactX, contactZ) + 0.42f,
                                 contactZ);
            const float cameraX = contactX - 4.2f;
            const float cameraZ = contactZ - 5.0f;
            cameraPosition_ = Vector3(
                cameraX, terrain_.Height(cameraX, cameraZ) + 3.2f, cameraZ);
            const Vector3 toContact = target - cameraPosition_;
            yaw_ = std::atan2(toContact.X, toContact.Z);
            pitch_ = std::atan2(toContact.Y, std::hypot(toContact.X, toContact.Z));
        } else if (viewpoint_ == FixedViewpoint::MovementTraceDetail
                   && !vegetationLayout_.Obstacles().empty()) {
            const CircularObstacle& obstacle = vegetationLayout_.Obstacles().front();
            const float cameraX = obstacle.x - 2.2f;
            const float cameraZ = obstacle.z + 4.8f;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ) + 2.5f,
                                      cameraZ);
            const Vector3 target(obstacle.x, terrain_.Height(obstacle.x, obstacle.z) + 0.7f,
                                 obstacle.z);
            const Vector3 toTarget = target - cameraPosition_;
            yaw_ = std::atan2(toTarget.X, toTarget.Z);
            pitch_ = std::atan2(toTarget.Y, std::hypot(toTarget.X, toTarget.Z));
        } else if (viewpoint_ == FixedViewpoint::BushDetail) {
            const BushPlacement* selected = nullptr;
            float bestScore = std::numeric_limits<float>::max();
            float selectedCameraX=0,selectedCameraZ=0;
            // The flowering hibiscus form now stands for the bush family in this QA frame.
            for (const auto& bush : jungleLayout_.Bushes()) {
                if (bush.variant != 1) continue;
                const float pathDistance = std::abs(bush.x - VegetationLayout::PrimaryPathCenterX(bush.z));
                for (int heading=0;heading<8;++heading) {
                    const float angle=heading*std::numbers::pi_v<float>/4;
                    const float offsetX=std::cos(angle)*3.8f,offsetZ=std::sin(angle)*3.8f;
                    const auto distanceToSightline=[&](float x,float z) {
                        const float t=std::clamp(((x-bush.x)*offsetX+(z-bush.z)*offsetZ)/(3.8f*3.8f),0.0f,1.0f);
                        return std::hypot(x-bush.x-offsetX*t,z-bush.z-offsetZ*t);
                    };
                    bool obstructed=false;
                    for (const auto& plant:jungleLayout_.Understory()) {
                        if (distanceToSightline(plant.x,plant.z)<1.8f) { obstructed=true; break; }
                    }
                    if (obstructed) continue;
                    for (const auto& obstacle:worldCollisionGrid_.Obstacles()) {
                        if (distanceToSightline(obstacle.x,obstacle.z)<obstacle.radius+0.65f) {
                            obstructed=true; break;
                        }
                    }
                    if (obstructed) continue;
                    const float score=std::abs(terrain_.SignedShoreDistance(bush.x,bush.z)-70.0f)
                        + std::abs(pathDistance-5.0f)*0.4f - bush.scale*2.0f;
                    if (score<bestScore) {
                        bestScore=score; selected=&bush;
                        selectedCameraX=bush.x+offsetX; selectedCameraZ=bush.z+offsetZ;
                    }
                }
            }
            if (!selected) throw std::runtime_error("Bush QA camera found no unobstructed variant 0 target");
            cameraPosition_=Vector3(selectedCameraX,terrain_.Height(selectedCameraX,selectedCameraZ)
                +PlayerMotion::EyeHeight,selectedCameraZ);
            const Vector3 target(selected->x,selected->y+0.6f*selected->scale,selected->z);
            const Vector3 delta=target-cameraPosition_;
            yaw_=std::atan2(delta.X,delta.Z);
            pitch_=std::atan2(delta.Y,std::hypot(delta.X,delta.Z));
            std::cout << "island-simulator: bush QA target variant 0 at player-eye detail distance\n";
        } else if ((viewpoint_ == FixedViewpoint::GroundPlantDetail
                    || viewpoint_ == FixedViewpoint::GroundBroadleafDetail
                    || viewpoint_ == FixedViewpoint::FallenLeafDetail)
                   && !jungleLayout_.GroundPlants().empty()) {
            const std::uint8_t targetVariant = viewpoint_ == FixedViewpoint::FallenLeafDetail
                ? 0U : (viewpoint_ == FixedViewpoint::GroundBroadleafDetail ? 1U : 2U);
            const GroundPlantPlacement* selected = nullptr;
            float bestScore = std::numeric_limits<float>::max();
            for (const GroundPlantPlacement& plant : jungleLayout_.GroundPlants()) {
                if (plant.variant != targetVariant) continue;
                const float pathX = VegetationLayout::PrimaryPathCenterX(plant.z);
                const float pathDistance = std::fabs(plant.x - pathX);
                if (pathDistance < 3.2f || pathDistance > 6.5f) continue;
                const auto hasNearby = [&plant](const auto& placements,
                                                const float radius) {
                    const float radiusSquared = radius * radius;
                    for (const auto& placement : placements) {
                        const float deltaX = plant.x - placement.x;
                        const float deltaZ = plant.z - placement.z;
                        const float distanceSquared = deltaX * deltaX + deltaZ * deltaZ;
                        if (distanceSquared > 0.01f && distanceSquared < radiusSquared) {
                            return true;
                        }
                    }
                    return false;
                };
                if (hasNearby(jungleLayout_.Understory(), 4.5f)
                    || hasNearby(jungleLayout_.Bushes(), 3.2f)
                    || hasNearby(jungleLayout_.GroundPlants(), 1.5f)) {
                    continue;
                }
                bool obstructed = false;
                for (const CircularObstacle& obstacle : worldCollisionGrid_.Obstacles()) {
                    const float deltaX = plant.x - obstacle.x;
                    const float deltaZ = plant.z - obstacle.z;
                    const float clearance = obstacle.radius + 3.0f;
                    if (deltaX * deltaX + deltaZ * deltaZ < clearance * clearance) {
                        obstructed = true;
                        break;
                    }
                }
                if (obstructed) continue;
                const float shoreDistance = terrain_.SignedShoreDistance(plant.x, plant.z);
                const float score = std::fabs(shoreDistance - 58.0f) * 1.4f
                    + std::fabs(pathDistance - 4.2f) * 5.0f
                    - plant.scale * 2.0f;
                if (score < bestScore) {
                    bestScore = score;
                    selected = &plant;
                }
            }
            if (selected == nullptr) {
                for (const GroundPlantPlacement& plant : jungleLayout_.GroundPlants()) {
                    if (plant.variant == targetVariant) {
                        selected = &plant;
                        break;
                    }
                }
            }
            if (selected == nullptr) selected = &jungleLayout_.GroundPlants().front();
            const float pathX = VegetationLayout::PrimaryPathCenterX(selected->z);
            const float detailDistance = viewpoint_ == FixedViewpoint::GroundBroadleafDetail
                ? 2.15f : 2.8f;
            const float cameraOffset = pathX < selected->x ? -detailDistance : detailDistance;
            const float cameraX = selected->x + cameraOffset;
            const float cameraZ = selected->z;
            const float detailEyeHeight = viewpoint_ == FixedViewpoint::GroundBroadleafDetail
                ? 1.25f : PlayerMotion::EyeHeight;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ)
                                                   + detailEyeHeight,
                                      cameraZ);
            const Vector3 target(selected->x, selected->y + 0.42f * selected->scale,
                                 selected->z);
            const Vector3 toPlant = target - cameraPosition_;
            yaw_ = std::atan2(toPlant.X, toPlant.Z);
            pitch_ = std::atan2(toPlant.Y, std::hypot(toPlant.X, toPlant.Z));
            std::cout << "island-simulator: "
                      << (viewpoint_ == FixedViewpoint::FallenLeafDetail
                              ? "ground-litter" : (viewpoint_ == FixedViewpoint::GroundBroadleafDetail
                                  ? "ground-broadleaf" : "ground"))
                      << " QA target variant "
                      << static_cast<int>(selected->variant)
                      << " at player-eye detail distance\n";
        } else if ((viewpoint_ == FixedViewpoint::FallenPalmDetail
                    || viewpoint_ == FixedViewpoint::BrokenPalmDetail)
                   && !vegetationLayout_.Palms().empty()) {
            const PalmCondition wanted = viewpoint_ == FixedViewpoint::FallenPalmDetail
                ? PalmCondition::Fallen : PalmCondition::Broken;
            const PalmPlacement* selected = nullptr;
            for (const PalmPlacement& palm : vegetationLayout_.Palms()) {
                if (palm.condition == wanted) {
                    selected = &palm;
                    break;
                }
            }
            if (selected == nullptr) selected = &vegetationLayout_.Palms().front();
            Vector3 target(selected->x, selected->y + 2.5f, selected->z);
            Vector3 cameraDirection(0.8f, 0.0f, 0.6f);
            float distance = 8.0f;
            if (wanted == PalmCondition::Fallen) {
                const float side = selected->variant % 2U == 0U ? 1.0f : -1.0f;
                const Matrix transform = Matrix::CreateScale(selected->scale)
                    * Matrix::CreateRotationZ(side * 1.43f)
                    * Matrix::CreateRotationY(selected->damageYawRadians
                                              + side * std::numbers::pi_v<float> * 0.5f)
                    * Matrix::CreateTranslation(selected->x,
                                                selected->y + 0.24f * selected->scale,
                                                selected->z);
                const Vector3 base = Vector3::Transform(Vector3::Zero, transform);
                const Vector3 crown = Vector3::Transform(Vector3(0.0f, 9.8f, 0.0f), transform);
                target = (base + crown) * 0.5f + Vector3(0.0f, 0.35f, 0.0f);
                const Vector3 stem = crown - base;
                const float horizontalLength = std::max(0.1f, std::hypot(stem.X, stem.Z));
                cameraDirection = Vector3(-stem.Z / horizontalLength, 0.0f,
                                          stem.X / horizontalLength);
                distance = 10.5f;
            } else {
                target.Y = selected->y
                    + (2.4f + 0.20f * static_cast<float>(selected->variant)) * selected->scale;
                cameraDirection = Vector3(std::cos(selected->damageYawRadians), 0.0f,
                                          std::sin(selected->damageYawRadians));
            }
            const float cameraX = target.X + cameraDirection.X * distance;
            const float cameraZ = target.Z + cameraDirection.Z * distance;
            cameraPosition_ = Vector3(cameraX,
                                      terrain_.Height(cameraX, cameraZ) + PlayerMotion::EyeHeight,
                                      cameraZ);
            const Vector3 toPalm = target - cameraPosition_;
            yaw_ = std::atan2(toPalm.X, toPalm.Z);
            pitch_ = std::atan2(toPalm.Y, std::hypot(toPalm.X, toPalm.Z));
            std::cout << "island-simulator: storm palm QA condition "
                      << (wanted == PalmCondition::Fallen ? "fallen" : "broken")
                      << " at " << selected->x << ',' << selected->y << ',' << selected->z
                      << '\n';
        } else if (viewpoint_ == FixedViewpoint::PalmDetail
                   && !vegetationLayout_.Palms().empty()) {
            constexpr float targetX = -120.0f;
            constexpr float targetZ = -135.0f;
            const PalmPlacement* selected = &vegetationLayout_.Palms().front();
            float bestScore = std::numeric_limits<float>::max();
            for (const PalmPlacement& palm : vegetationLayout_.Palms()) {
                const float shoreDistance = terrain_.SignedShoreDistance(palm.x, palm.z);
                if (shoreDistance < 8.0f || shoreDistance > 24.0f) continue;
                const float deltaX = palm.x - targetX;
                const float deltaZ = palm.z - targetZ;
                const float score = deltaX * deltaX + deltaZ * deltaZ
                    - palm.scale * 120.0f
                    - ((palm.variant == 1U || palm.variant == 3U) ? 600.0f : 0.0f);
                if (score < bestScore) {
                    bestScore = score;
                    selected = &palm;
                }
            }
            const Vector3 radial = Vector3::Normalize(
                Vector3(selected->x, 0.0f, selected->z));
            const Vector3 tangent(-radial.Z, 0.0f, radial.X);
            const float cameraX = selected->x + radial.X * 14.0f + tangent.X * 2.0f;
            const float cameraZ = selected->z + radial.Z * 14.0f + tangent.Z * 2.0f;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ)
                                                   + PlayerMotion::EyeHeight,
                                      cameraZ);
            const Vector3 target(selected->x, selected->y + 5.2f * selected->scale,
                                 selected->z);
            const Vector3 toPalm = target - cameraPosition_;
            yaw_ = std::atan2(toPalm.X, toPalm.Z);
            pitch_ = std::atan2(toPalm.Y, std::hypot(toPalm.X, toPalm.Z));
        } else if (viewpoint_ == FixedViewpoint::GrassDetail
                   && !grassLayout_.Patches().empty()) {
            constexpr float targetX = 35.0f;
            constexpr float targetZ = -170.0f;
            const GrassPlacement* selected = &grassLayout_.Patches().front();
            float bestScore = std::numeric_limits<float>::max();
            for (const GrassPlacement& patch : grassLayout_.Patches()) {
                if (patch.pathScale) continue;
                // Inspect a representative sward, not an isolated tuft on exposed humus.
                const TerrainSample coverSample = terrain_.Sample(patch.x, patch.z);
                if (GroundCover::Density(std::clamp(canopyLightField_.Sample(patch.x, patch.z), 0.0f, 1.0f),
                        std::clamp(groundCoverField_.SamplePatch(patch.x, patch.z), 0.0f, 1.0f),
                        coverSample.signedShoreDistance, coverSample.slopeRadians) < 0.6f) continue;
                bool clear = true;
                for (const CircularObstacle& obstacle : worldCollisionGrid_.Obstacles()) {
                    const float deltaX = patch.x - obstacle.x;
                    const float deltaZ = patch.z - obstacle.z;
                    const float clearance = obstacle.radius + 5.0f;
                    if (deltaX * deltaX + deltaZ * deltaZ < clearance * clearance) {
                        clear = false;
                        break;
                    }
                }
                if (!clear) continue;
                const auto foliageIsClear = [&patch](const auto& placements,
                                                     const float radius) {
                    for (const auto& placement : placements) {
                        const float deltaX = patch.x - placement.x;
                        const float deltaZ = patch.z - placement.z;
                        if (deltaX * deltaX + deltaZ * deltaZ < radius * radius) {
                            return false;
                        }
                    }
                    return true;
                };
                if (!foliageIsClear(jungleLayout_.Understory(), 4.0f)
                    || !foliageIsClear(jungleLayout_.Bushes(), 4.0f)
                    || !foliageIsClear(jungleLayout_.GroundPlants(), 3.5f)) {
                    continue;
                }
                const float deltaX = patch.x - targetX;
                const float deltaZ = patch.z - targetZ;
                const float score = deltaX * deltaX + deltaZ * deltaZ;
                if (score < bestScore) {
                    bestScore = score;
                    selected = &patch;
                }
            }
            cameraPosition_ = Vector3(selected->x, selected->y + PlayerMotion::EyeHeight,
                                      selected->z);
            yaw_ = std::atan2(-selected->x, -selected->z);
            pitch_ = -0.38f;
        } else if (viewpoint_ == FixedViewpoint::WetSandDetail) {
            const float x = -(terrain_.ShoreRadius(std::numbers::pi_v<float>) - 1.0f);
            cameraPosition_ = Vector3(x, terrain_.Height(x, 0.0f) + PlayerMotion::EyeHeight, 0.0f);
            yaw_ = 0.0f;
            pitch_ = -0.40f;
        } else if (viewpoint_ == FixedViewpoint::WaterlineDetail) {
            constexpr float angle = -std::numbers::pi_v<float> * 0.5f;
            const float radius = terrain_.ShoreRadius(angle) + 18.0f;
            cameraPosition_ = Vector3(std::cos(angle) * radius, 0.58f,
                                      std::sin(angle) * radius);
            yaw_ = std::atan2(-cameraPosition_.X, -cameraPosition_.Z);
            pitch_ = -0.025f;
        } else if (viewpoint_ == FixedViewpoint::CurrentBoundaryDetail) {
            constexpr float angle = -std::numbers::pi_v<float> * 0.5f;
            const float radius = terrain_.ShoreRadius(angle) + 62.0f;
            cameraPosition_ = Vector3(std::cos(angle) * radius, 0.58f,
                                      std::sin(angle) * radius);
            yaw_ = std::atan2(-cameraPosition_.X, -cameraPosition_.Z);
            pitch_ = -0.015f;
        } else if (viewpoint_ == FixedViewpoint::DistantSceneryDetail) {
            constexpr float cameraZ = -214.0f;
            cameraPosition_ = Vector3(0.0f, terrain_.Height(0.0f, cameraZ)
                                                + PlayerMotion::EyeHeight,
                                      cameraZ);
            yaw_ = std::numbers::pi_v<float>;
            pitch_ = -0.005f;
        } else if (viewpoint_ == FixedViewpoint::PropDetail
                   && !naturalPropLayout_.FallenLogs().empty()) {
            const FallenLogPlacement& log = naturalPropLayout_.FallenLogs().front();
            const float offsetX = -std::sin(log.yawRadians) * 4.8f;
            const float offsetZ = std::cos(log.yawRadians) * 4.8f;
            const float cameraX = log.x + offsetX;
            const float cameraZ = log.z + offsetZ;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ)
                                                   + PlayerMotion::EyeHeight,
                                      cameraZ);
            yaw_ = std::atan2(log.x - cameraX, log.z - cameraZ);
            pitch_ = -0.14f;
        } else if (viewpoint_ == FixedViewpoint::DriftwoodDetail
                   && !naturalPropLayout_.Driftwood().empty()) {
            const auto selected = std::find_if(
                naturalPropLayout_.Driftwood().begin(), naturalPropLayout_.Driftwood().end(),
                [](const DriftwoodPlacement& wood) {
                    return wood.coveLandmark && wood.variant == 2U;
                });
            const DriftwoodPlacement& wood = selected != naturalPropLayout_.Driftwood().end()
                ? *selected : naturalPropLayout_.Driftwood().front();
            float targetX = 0.0f;
            float targetY = 0.0f;
            float targetZ = 0.0f;
            int landmarkCount = 0;
            for (const DriftwoodPlacement& candidate : naturalPropLayout_.Driftwood()) {
                if (!candidate.coveLandmark) continue;
                targetX += candidate.x;
                targetY += candidate.y + candidate.radius;
                targetZ += candidate.z;
                ++landmarkCount;
            }
            if (landmarkCount > 0) {
                targetX /= static_cast<float>(landmarkCount);
                targetY /= static_cast<float>(landmarkCount);
                targetZ /= static_cast<float>(landmarkCount);
            } else {
                targetX = wood.x;
                targetY = wood.y + wood.radius;
                targetZ = wood.z;
            }
            const float targetRadius = std::hypot(targetX, targetZ);
            const float outwardX = targetRadius > 0.001f ? targetX / targetRadius : 0.0f;
            const float outwardZ = targetRadius > 0.001f ? targetZ / targetRadius : -1.0f;
            // The landmark arc is 7--10.5 m inland. Review it from the lower beach side;
            // the former 12 m inward offset put the camera behind the rising cove shoulder.
            const float cameraX = targetX + outwardX * 10.0f - outwardZ * 1.0f;
            const float cameraZ = targetZ + outwardZ * 10.0f + outwardX * 1.0f;
            cameraPosition_ = Vector3(cameraX,
                std::max(terrain_.Height(cameraX, cameraZ) + PlayerMotion::EyeHeight,
                         targetY + 3.8f), cameraZ);
            yaw_ = std::atan2(targetX - cameraX, targetZ - cameraZ);
            pitch_ = std::atan2(targetY - cameraPosition_.Y,
                                std::hypot(targetX - cameraX, targetZ - cameraZ));
            std::cout << "island-simulator: driftwood QA views the three-piece cove arc from the lower beach\n";
        } else if (viewpoint_ == FixedViewpoint::ReefDetail) {
            float targetAngle = 0.0f;
            float targetOffshore = 74.0f;
            float targetWeight = -1.0f;
            for (int angleSample = 0; angleSample < 360; ++angleSample) {
                const float angle = static_cast<float>(angleSample)
                    * std::numbers::pi_v<float> / 180.0f;
                const float shoreRadius = terrain_.ShoreRadius(angle);
                for (float offshore = 62.0f; offshore <= 86.0f; offshore += 1.0f) {
                    const float x = std::cos(angle) * (shoreRadius + offshore);
                    const float z = std::sin(angle) * (shoreRadius + offshore);
                    const float weight = ReefMacroAppearance::BreakerWeight(
                        x, z, offshore, -terrain_.Height(x, z));
                    if (weight > targetWeight) {
                        targetWeight = weight;
                        targetAngle = angle;
                        targetOffshore = offshore;
                    }
                }
            }
            if (targetWeight >= 0.0f) {
                const Vector3 radial(std::cos(targetAngle), 0.0f, std::sin(targetAngle));
                const Vector3 tangent(-radial.Z, 0.0f, radial.X);
                const float shoreRadius = terrain_.ShoreRadius(targetAngle);
                const Vector3 target(radial.X * (shoreRadius + targetOffshore), 0.15f,
                                     radial.Z * (shoreRadius + targetOffshore));
                const float cameraRadius = shoreRadius + targetOffshore + 31.0f;
                const float cameraX = radial.X * cameraRadius + tangent.X * 6.0f;
                const float cameraZ = radial.Z * cameraRadius + tangent.Z * 6.0f;
                cameraPosition_ = Vector3(cameraX, 8.5f, cameraZ);
                const Vector3 toReef = target - cameraPosition_;
                yaw_ = std::atan2(toReef.X, toReef.Z);
                pitch_ = std::atan2(toReef.Y, std::hypot(toReef.X, toReef.Z));
                std::cout << "island-simulator: reef QA camera uses an elevated wide substrate view\n";
                std::cout << "island-simulator: reef QA camera targets peak broken crest weight "
                          << targetWeight << " at " << targetOffshore << "m offshore\n";
            }
        } else if (viewpoint_ == FixedViewpoint::ReefHabitatDetail) {
            const ReefHabitatPlacement* selected = nullptr;
            int selectedNeighbours = -1;
            for (const auto& candidate : naturalPropLayout_.ReefHabitat()) {
                if (candidate.variant >= 3U) continue;
                int neighbours = 0;
                for (const auto& other : naturalPropLayout_.ReefHabitat()) {
                    if (&candidate != &other && other.variant < 3U
                        && std::hypot(candidate.x - other.x, candidate.z - other.z) < 16.0f) {
                        ++neighbours;
                    }
                }
                if (neighbours > selectedNeighbours
                    || (neighbours == selectedNeighbours
                        && (!selected || candidate.scale > selected->scale))) {
                    selected = &candidate;
                    selectedNeighbours = neighbours;
                }
            }
            if (!selected) throw std::runtime_error("Reef-habitat QA camera found no target");
            const Vector3 radial = Vector3::Normalize(Vector3(selected->x, 0, selected->z));
            const Vector3 tangent(-radial.Z, 0, radial.X);
            cameraPosition_ = Vector3(selected->x - radial.X * 1.4f + tangent.X * 4.3f,
                                      std::min(-.35f, selected->y + 1.05f),
                                      selected->z - radial.Z * 1.4f + tangent.Z * 4.3f);
            const Vector3 target(selected->x, selected->y + .42f * selected->scale,
                                 selected->z);
            const auto direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: reef habitat QA camera underwater with "
                      << selectedNeighbours << " neighbours\n";
        } else if (viewpoint_ == FixedViewpoint::AnemoneDetail) {
            // Hover a metre off the reef's first anemone, a little above it, looking down on its
            // tentacles and the pair of clownfish in them.
            const ReefFishField reef(naturalPropLayout_.ReefHabitat());
            if (reef.Anemones().empty()) throw std::runtime_error("Anemone QA camera found no anemone");
            const ReefAnemone& anemone = reef.Anemones().front();
            cameraPosition_ = Vector3(anemone.x + 1.40f, std::min(-0.3f, anemone.y + 0.65f), anemone.z + 0.50f);
            const Vector3 direction = Vector3(anemone.x, anemone.y + 0.12f, anemone.z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: anemone QA camera hovers by a sea anemone at depth " << -anemone.y << " m\n";
        } else if (viewpoint_ == FixedViewpoint::FallenFrondDetail
                   && !naturalPropLayout_.FallenFronds().empty()) {
            const auto closestToShore = std::min_element(
                naturalPropLayout_.FallenFronds().begin(),
                naturalPropLayout_.FallenFronds().end(),
                [this](const FallenFrondPlacement& first,
                       const FallenFrondPlacement& second) {
                    return terrain_.SignedShoreDistance(first.x, first.z)
                        < terrain_.SignedShoreDistance(second.x, second.z);
                });
            const FallenFrondPlacement& frond = *closestToShore;
            const Vector3 direction(std::cos(frond.yawRadians), 0.0f,
                                    std::sin(frond.yawRadians));
            const Vector3 side(-direction.Z, 0.0f, direction.X);
            const float cameraX = frond.x - direction.X * 0.8f + side.X * 3.1f;
            const float cameraZ = frond.z - direction.Z * 0.8f + side.Z * 3.1f;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ)
                                                   + PlayerMotion::EyeHeight,
                                      cameraZ);
            const Vector3 toFrond = Vector3(frond.x, frond.y + 0.10f, frond.z)
                - cameraPosition_;
            yaw_ = std::atan2(toFrond.X, toFrond.Z);
            pitch_ = std::atan2(toFrond.Y, std::hypot(toFrond.X, toFrond.Z));
        } else if (viewpoint_ == FixedViewpoint::BirdDetail
                   && !birdFlock_.Birds().empty()) {
            // Agent 11 is a flying parrot. Agent 10 is intentionally seeded onto a canopy
            // branch, where the close camera can sit inside foliage and hide the bird entirely.
            const BirdState& bird = birdFlock_.Birds().size() > 11U
                ? birdFlock_.Birds()[11U] : birdFlock_.Birds().front();
            const Vector3 flightForward(std::cos(bird.headingRadians), 0.0f,
                                        std::sin(bird.headingRadians));
            const Vector3 flightRight(-flightForward.Z, 0.0f, flightForward.X);
            cameraPosition_ = Vector3(bird.x, bird.y, bird.z)
                + flightForward * 1.55f + flightRight * 1.10f + Vector3::Up * 0.30f;
            const Vector3 toBird = Vector3(bird.x, bird.y + bird.scale*.06f, bird.z)
                - cameraPosition_;
            yaw_ = std::atan2(toBird.X, toBird.Z);
            pitch_ = std::atan2(toBird.Y, std::hypot(toBird.X, toBird.Z));
            std::cout << "island-simulator: bird QA camera frames one flying hooked-beak parrot\n";
        } else if (viewpoint_ == FixedViewpoint::BirdPreenDetail
                   && !birdFlock_.Birds().empty()) {
            // Agent 0 is seeded perched on the wreck; held mid-preen, framed from its
            // right front so the head turned back into the right wing reads.
            birdFlock_.PreviewPreen(0U);
            const BirdState& bird = birdFlock_.Birds().front();
            const Vector3 forward(std::cos(bird.headingRadians), 0.0f,
                                  std::sin(bird.headingRadians));
            const Vector3 right(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(bird.x, bird.y, bird.z)
                + forward * 0.95f + right * 1.15f + Vector3::Up * 0.42f;
            const Vector3 toBird = Vector3(bird.x, bird.y + bird.scale * .05f, bird.z)
                - cameraPosition_;
            yaw_ = std::atan2(toBird.X, toBird.Z);
            pitch_ = std::atan2(toBird.Y, std::hypot(toBird.X, toBird.Z));
            std::cout << "island-simulator: bird QA camera frames one perched bird preening, its head turned back into a wing\n";
        } else if (viewpoint_ == FixedViewpoint::GroundDoveDetail
                   && birdFlock_.Birds().size() > 14U) {
            const BirdState& dove = birdFlock_.Birds()[14U];
            const Vector3 radial = Vector3::Normalize(Vector3(dove.x, 0.0f, dove.z));
            Vector3 tangent(-radial.Z, 0.0f, radial.X);
            // Of the two sides, the one away from the resident (who tends the fire beside this
            // clearing and otherwise crouches right by the camera).
            if (resident_) {
                const auto& r = resident_->State();
                const Vector3 at(dove.x, dove.y, dove.z), them(r.x, r.y, r.z);
                if (Vector3::DistanceSquared(at - radial * 1.55f + tangent * 0.62f, them)
                    < Vector3::DistanceSquared(at - radial * 1.55f - tangent * 0.62f, them))
                    tangent = -tangent;
            }
            cameraPosition_ = Vector3(dove.x, dove.y, dove.z)
                - radial * 1.55f + tangent * 0.62f + Vector3::Up * 0.45f;
            const Vector3 toDove = Vector3(dove.x, dove.y + dove.scale * .06f, dove.z)
                - cameraPosition_;
            yaw_ = std::atan2(toDove.X, toDove.Z);
            pitch_ = std::atan2(toDove.Y, std::hypot(toDove.X, toDove.Z));
            std::cout << "island-simulator: ground-dove QA camera frames one walking "
                         "fruit dove on dry interior terrain\n";
        } else if (viewpoint_ == FixedViewpoint::FruitBatDetail
                   && !fruitBatColony_.Bats().empty()) {
            const FruitBatState& bat = fruitBatColony_.Bats().front();
            const Vector3 forward(std::cos(bat.headingRadians), 0.0f,
                                  std::sin(bat.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(bat.x, bat.y, bat.z)
                + forward * 2.55f + side * 1.75f + Vector3::Up * .36f;
            const Vector3 toBat = Vector3(bat.x, bat.y, bat.z) - cameraPosition_;
            yaw_ = std::atan2(toBat.X, toBat.Z);
            pitch_ = std::atan2(toBat.Y, std::hypot(toBat.X, toBat.Z));
            std::cout << "island-simulator: fruit-bat QA camera frames one bounded daytime flight\n";
        } else if (viewpoint_ == FixedViewpoint::FruitBatRoostDetail
                   && fruitBatColony_.Bats().size() > 2U) {
            const FruitBatState& bat = fruitBatColony_.Bats()[2U];
            const Vector3 roostForward(std::cos(bat.headingRadians),0.0f,
                                       std::sin(bat.headingRadians));
            // A roost is offset from its host trunk roughly opposite its authored
            // heading. Approach from that real crown-outward direction rather than
            // from the island centre, which can put the camera behind the trunk.
            const Vector3 outward=-roostForward;
            const Vector3 side(-outward.Z,0.0f,outward.X);
            cameraPosition_ = Vector3(bat.x, bat.y, bat.z)
                + outward*1.42f+side*.38f-Vector3::Up*1.64f;
            const Vector3 toBat = Vector3(bat.x, bat.y - .04f, bat.z) - cameraPosition_;
            yaw_ = std::atan2(toBat.X, toBat.Z);
            pitch_ = std::atan2(toBat.Y, std::hypot(toBat.X, toBat.Z));
            std::cout << "island-simulator: fruit-bat roost QA camera frames one inverted canopy sleeper\n";
        } else if (viewpoint_ == FixedViewpoint::FishDetail
                   && !fishSchools_.Fish().empty()) {
            const FishState& fish = fishSchools_.Fish().front();
            const Vector3 forward(std::cos(fish.headingRadians), 0.0f,
                                  std::sin(fish.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(fish.x, fish.y, fish.z)
                + side * 2.55f - forward * 0.35f + Vector3::Up * 0.68f;
            // clear of the waves: a camera a few centimetres over the still water had the
            // passing crests cut by its near plane, the sea missing from a band at its feet
            cameraPosition_.Y = std::max(cameraPosition_.Y, 0.45f);
            const Vector3 toFish = Vector3(fish.x, fish.y, fish.z) - cameraPosition_;
            yaw_ = std::atan2(toFish.X, toFish.Z);
            pitch_ = std::atan2(toFish.Y, std::hypot(toFish.X, toFish.Z));
        } else if (viewpoint_ == FixedViewpoint::StingrayDetail
                   && !stingrayGroup_.Stingrays().empty()) {
            const StingrayState& ray = stingrayGroup_.Stingrays().front();
            const Vector3 forward(std::cos(ray.headingRadians),0.0f,
                                  std::sin(ray.headingRadians));
            const Vector3 side(-forward.Z,0.0f,forward.X);
            cameraPosition_=Vector3(ray.x,ray.y,ray.z)
                +side*2.35f+forward*1.70f+Vector3::Up*1.02f;
            const Vector3 toRay=Vector3(ray.x,ray.y,ray.z)-cameraPosition_;
            yaw_=std::atan2(toRay.X,toRay.Z);
            pitch_=std::atan2(toRay.Y,std::hypot(toRay.X,toRay.Z));
            std::cout<<"island-simulator: stingray QA camera follows one shallow-sand glide\n";
        } else if (viewpoint_ == FixedViewpoint::StingrayBuriedDetail
                   && stingrayGroup_.Stingrays().size() > 2U) {
            // The third ray is seeded settled; held buried, framed from its quarter.
            stingrayGroup_.PreviewBury(2U);
            const StingrayState& ray = stingrayGroup_.Stingrays()[2U];
            const Vector3 forward(std::cos(ray.headingRadians),0.0f,
                                  std::sin(ray.headingRadians));
            const Vector3 side(-forward.Z,0.0f,forward.X);
            cameraPosition_=Vector3(ray.x,ray.y,ray.z)
                +side*2.0f+forward*1.5f+Vector3::Up*1.1f;
            const Vector3 toRay=Vector3(ray.x,ray.y,ray.z)-cameraPosition_;
            yaw_=std::atan2(toRay.X,toRay.Z);
            pitch_=std::atan2(toRay.Y,std::hypot(toRay.X,toRay.Z));
            std::cout<<"island-simulator: stingray QA camera frames one settled ray lying half-buried in the sand\n";
        } else if(viewpoint_==FixedViewpoint::OctopusDetail&&!octopusGroup_.Octopuses().empty()){
            const auto& o=octopusGroup_.Octopuses().front();Vector3 forward(std::cos(o.headingRadians),0,std::sin(o.headingRadians)),side(-forward.Z,0,forward.X);
            cameraPosition_=Vector3(o.x,o.y+.28f,o.z)+forward*1.65f+side*.85f+Vector3::Up*.36f;
            const Vector3 target(o.x,o.y+.24f,o.z);const auto d=target-cameraPosition_;yaw_=std::atan2(d.X,d.Z);pitch_=std::atan2(d.Y,std::hypot(d.X,d.Z));
            std::cout<<"island-simulator: octopus QA camera frames one exposed reef forager and all eight arms\n";
        } else if (viewpoint_ == FixedViewpoint::SeaTurtleDetail
                   && !seaTurtleGroup_.Turtles().empty()) {
            const SeaTurtleState& turtle = seaTurtleGroup_.Turtles()[2U];
            const Vector3 radial = Vector3::Normalize(Vector3(turtle.x, 0.0f, turtle.z));
            const Vector3 forward(std::cos(turtle.headingRadians), 0.0f,
                                  std::sin(turtle.headingRadians));
            Vector3 side(-forward.Z, 0.0f, forward.X);
            if (Vector3::Dot(side, radial) < 0.0f) side = -side;
            cameraPosition_ = Vector3(turtle.x, turtle.y, turtle.z)
                + side * 2.1f - forward * 0.15f + Vector3::Up * 0.75f;
            const Vector3 toTurtle = Vector3(turtle.x, turtle.y, turtle.z) - cameraPosition_;
            yaw_ = std::atan2(toTurtle.X, toTurtle.Z);
            pitch_ = std::atan2(toTurtle.Y, std::hypot(toTurtle.X, toTurtle.Z));
            std::cout << "island-simulator: sea-turtle QA camera views a surfacing route\n";
        } else if (viewpoint_ == FixedViewpoint::SharkDetail) {
            const SharkState& shark = sharkPatrol_.State();
            const Vector3 radial = Vector3::Normalize(Vector3(shark.x, 0.0f, shark.z));
            const Vector3 tangent(-radial.Z, 0.0f, radial.X);
            cameraPosition_ = Vector3(shark.x, shark.y, shark.z)
                + radial * 7.2f + tangent * 1.4f + Vector3::Up * 2.7f;
            const Vector3 toShark = Vector3(shark.x, shark.y, shark.z) - cameraPosition_;
            yaw_ = std::atan2(toShark.X, toShark.Z);
            pitch_ = std::atan2(toShark.Y, std::hypot(toShark.X, toShark.Z));
        } else if (viewpoint_ == FixedViewpoint::DolphinDetail
                   && !dolphinPod_.Dolphins().empty()) {
            // Agent zero begins in its ordinary cruise depth. Agent one is the
            // surfacing animal and the water plane used to bisect its head in QA.
            const DolphinState& dolphin = dolphinPod_.Dolphins().front();
            const Vector3 forward(std::cos(dolphin.headingRadians), 0.0f,
                                  std::sin(dolphin.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(dolphin.x, dolphin.y, dolphin.z)
                + side * 3.7f - forward * .45f + Vector3::Up * .35f;
            const Vector3 toDolphin = Vector3(dolphin.x, dolphin.y, dolphin.z)
                + forward * .12f - cameraPosition_;
            yaw_ = std::atan2(toDolphin.X, toDolphin.Z);
            pitch_ = std::atan2(toDolphin.Y, std::hypot(toDolphin.X, toDolphin.Z));
            std::cout << "island-simulator: dolphin QA uses a low moving broadside view\n";
        } else if (viewpoint_ == FixedViewpoint::MonkeyDetail || viewpoint_ == FixedViewpoint::MonkeyTreeDetail
            || viewpoint_ == FixedViewpoint::MonkeyClimbDetail || viewpoint_ == FixedViewpoint::MonkeyScratchDetail) {
            if (viewpoint_ == FixedViewpoint::MonkeyTreeDetail) {
                monkeyTroop_.SetClimbingEnabled(false); // Preserve the separate inspection-only regression.
                const MonkeyTroop initialTroop(monkeyTroop_);
                bool ready = false;
                for (int step = 0; step < 12000 && !ready; ++step) {
                    const auto subject = monkeyTroop_.Monkeys().front();
                    monkeyTroop_.Update(1.0f/30.0f,subject.x+12,subject.y,subject.z);
                    for (std::size_t index = 0; index < monkeyTroop_.Monkeys().size() && !ready; ++index) {
                        const auto& current = monkeyTroop_.Monkeys()[index];
                        ready = current.behavior == MonkeyBehavior::InspectTree
                            && current.treeInspectionSeconds >= 2.7f;
                        if (ready) monkeyQaSubjectIndex_ = index;
                    }
                }
                if (!ready) throw std::runtime_error("monkey-tree inspection could not reach a tree in bounded simulation");
                std::cout << "island-simulator: monkey-tree QA holds naturally reached inspection; wind remains live\n";
                if (!capturePath_.empty()) {
                    const auto selected = monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
                    const float startClock = selected.animationSeconds-selected.treeInspectionSeconds;
                    MonkeyTroop replay(initialTroop);
                    constexpr std::array<float,7> phaseTimes{.05f,1.35f,2.1f,3.3f,4.8f,5.4f,5.8f};
                    bool entered = false;
                    for (int step = 0; step < 12240 && monkeyTreeQaPoses_.size() < 8; ++step) {
                        const auto observer = replay.Monkeys().front();
                        replay.Update(1.0f/30.0f,observer.x+12,observer.y,observer.z);
                        const auto& current = replay.Monkeys()[monkeyQaSubjectIndex_];
                        const bool sameInspection = current.behavior == MonkeyBehavior::InspectTree
                            && current.treeIndex == selected.treeIndex
                            && std::abs(current.animationSeconds-current.treeInspectionSeconds-startClock) < .01f;
                        if (sameInspection) {
                            entered = true;
                            if (monkeyTreeQaPoses_.size() < phaseTimes.size()
                                && current.treeInspectionSeconds >= phaseTimes[monkeyTreeQaPoses_.size()])
                                monkeyTreeQaPoses_.emplace_back(replay.Monkeys().begin(),replay.Monkeys().end());
                        } else if (entered) {
                            if (monkeyTreeQaPoses_.size() != phaseTimes.size())
                                throw std::runtime_error("monkey-tree transfer interrupted before all phases");
                            monkeyTreeQaPoses_.emplace_back(replay.Monkeys().begin(),replay.Monkeys().end());
                        }
                    }
                    if (monkeyTreeQaPoses_.size() != 8)
                        throw std::runtime_error("monkey-tree replay did not retain all eight transfer stages");
                }
            }
            if (viewpoint_ == FixedViewpoint::MonkeyClimbDetail) {
                const MonkeyTroop initial(monkeyTroop_);
                bool ready=false;
                for (int step=0;step<18000 && !ready;++step) {
                    const auto observer=monkeyTroop_.Monkeys().front();
                    monkeyTroop_.Update(1.0f/30,observer.x+12,observer.y,observer.z);
                    for (std::size_t i=0;i<monkeyTroop_.Monkeys().size();++i) {
                        const auto& current=monkeyTroop_.Monkeys()[i];
                        if (current.treeExcursion && !current.treeClock.returning && current.treeClock.seconds>=18.5f) {
                            monkeyQaSubjectIndex_=i; ready=true; break;
                        }
                    }
                }
                if (!ready) throw std::runtime_error("No natural monkey climb reached summit in bounded QA warmup");
                const auto selected=monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
                const float start=selected.animationSeconds-selected.treeClock.seconds;
                MonkeyTroop replay(initial);
                constexpr std::array<float,7> times{0,5,10.1f,18.5f,24,29.1f,38.9f};
                bool entered=false;
                for (int step=0;step<19500 && monkeyTreeQaPoses_.size()<8;++step) {
                    const auto observer=replay.Monkeys().front();
                    replay.Update(1.0f/30,observer.x+12,observer.y,observer.z);
                    const auto& current=replay.Monkeys()[monkeyQaSubjectIndex_];
                    const bool same=current.treeExcursion && current.treeIndex==selected.treeIndex
                        && std::abs(current.animationSeconds-current.treeClock.seconds-start)<.02f;
                    if (same) {
                        entered=true;
                        if (monkeyTreeQaPoses_.size()<times.size()
                            && current.treeClock.seconds>=times[monkeyTreeQaPoses_.size()])
                            monkeyTreeQaPoses_.emplace_back(replay.Monkeys().begin(),replay.Monkeys().end());
                    } else if (entered && !current.treeReleasing) {
                        if (monkeyTreeQaPoses_.size()!=7) throw std::runtime_error("Natural monkey climb interrupted during QA replay");
                        monkeyTreeQaPoses_.emplace_back(replay.Monkeys().begin(),replay.Monkeys().end());
                    }
                }
                if (monkeyTreeQaPoses_.size()!=8) throw std::runtime_error("Monkey climb QA missing cycle stages");
            }
            if (viewpoint_ == FixedViewpoint::MonkeyScratchDetail) {
                // The scratch is shown on the troop member standing furthest from any trunk on
                // the ground: the first one stood at the foot of a tree with its head to the
                // bark, hidden from every side.
                float clearest = -1.0f;
                for (std::size_t index = 0; index < monkeyTroop_.Monkeys().size(); ++index) {
                    const MonkeyState& candidate = monkeyTroop_.Monkeys()[index];
                    if (candidate.treeIndex >= 0) continue;
                    float nearest = 1.0e6f;
                    for (const auto& tree : jungleLayout_.Canopy())
                        nearest = std::min(nearest, std::hypot(tree.x - candidate.x, tree.z - candidate.z) - 0.7f * tree.scale);
                    if (nearest > clearest) { clearest = nearest; monkeyQaSubjectIndex_ = index; }
                }
            }
            const MonkeyState& monkey = monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
            const Vector3 forward(std::cos(monkey.headingRadians), 0.0f,
                                  std::sin(monkey.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            if (viewpoint_ == FixedViewpoint::MonkeyScratchDetail) {
                // The scratch view holds the subject idle on the ground mid-scratch, framed
                // from its left so the raised forelimb reads.
                monkeyTroop_.PreviewScratch(monkeyQaSubjectIndex_);
                std::cout << "island-simulator: monkey QA camera frames one idle monkey scratching its flank\n";
            }
            if (viewpoint_ == FixedViewpoint::MonkeyScratchDetail) {
                // Out on the subject's left so the whole body and the raised forelimb read, at
                // whichever of a few places round that side sees it clearest past the ground
                // plants and trunks (a broad leaf once filled a third of the frame); the right
                // side only if the left is screened everywhere.
                const Vector3 base(monkey.x, monkey.y, monkey.z);
                const Vector3 target = base + Vector3::Up * 0.6f;
                // the head a little ahead of where it stands now: it settles half a pace on
                const Vector3 head = base + forward * 0.75f + Vector3::Up * 0.55f;
                // A monkey often scratches at the foot of a tree: stand on the far side of the
                // nearest trunk so the tree falls behind it, not across its head.
                Vector3 away = forward;
                float trunkDistance = 1.0e6f;
                for (const auto& tree : jungleLayout_.Canopy()) {
                    const float distance = std::hypot(tree.x - monkey.x, tree.z - monkey.z);
                    if (distance < trunkDistance) {
                        trunkDistance = distance;
                        away = Vector3(monkey.x - tree.x, 0.0f, monkey.z - tree.z);
                    }
                }
                const bool besideTrunk = trunkDistance < 3.0f && away.LengthSquared() > 1.0e-6f;
                if (besideTrunk) away.Normalize();
                Vector3 frontLeft = forward + side;
                frontLeft.Normalize();
                float best = -1.0e6f;
                for (int bearing = 0; bearing < 12; ++bearing) {
                    const float angle = static_cast<float>(bearing) * (2.0f * std::numbers::pi_v<float> / 12.0f);
                    const Vector3 direction = forward * std::cos(angle) + side * std::sin(angle);
                    // eye height over the tall grass and the ground plants' outer leaves
                    const Vector3 eye = base + direction * 2.3f + Vector3::Up * 1.45f;
                    const float clear = std::min(QaSightClearance(eye, target), QaSightClearance(eye, head));
                    const float score = std::min(clear, 1.2f) + 0.3f * Vector3::Dot(direction, frontLeft)
                        + (besideTrunk ? 0.6f * Vector3::Dot(direction, away) : 0.0f);
                    if (score > best) { best = score; cameraPosition_ = eye; }
                }
            } else if (viewpoint_ == FixedViewpoint::MonkeyDetail) {
                // View across the shoulder from the open side of the deterministic route.
                // This exposes the face and the full quadrupedal silhouette without putting
                // the camera between the subject and its selected tree. Of a few such places
                // the first that sees the body and head half a metre clear of the ground plants
                // (a broad leaf once crossed half the frame in front of it), else the clearest.
                const Vector3 base(monkey.x, monkey.y, monkey.z);
                const Vector3 target = base + Vector3::Up * 0.82f;
                const Vector3 head = base + forward * 0.45f + Vector3::Up * 0.75f;
                float best = -1.0e6f;
                for (const float along : {0.35f, 0.0f, 0.75f}) {
                    for (const float out : {1.15f, 1.5f}) {
                        const Vector3 eye = base + forward * along + side * out + Vector3::Up * 0.94f;
                        const float clear = std::min(QaSightClearance(eye, target), QaSightClearance(eye, head));
                        const float score = std::min(clear, 0.5f)
                            - 0.05f * std::abs(along - 0.35f) - 0.04f * (out - 1.15f);
                        if (score > best) { best = score; cameraPosition_ = eye; }
                    }
                }
            } else {
                cameraPosition_ = Vector3(monkey.x, monkey.y, monkey.z)
                    + forward * 0.6f - side * 2.1f + Vector3::Up * 1.10f;
            }
            if (viewpoint_==FixedViewpoint::MonkeyClimbDetail)
                cameraPosition_=Vector3(monkey.x,monkey.y,monkey.z)+forward*.65f-side*5.0f+Vector3::Up*2.2f;
            std::ostringstream monkeyCameraReport;
            monkeyCameraReport << "island-simulator: monkey QA subject " << monkey.x << ','
                << monkey.y << ',' << monkey.z << "; camera " << cameraPosition_.X << ','
                << cameraPosition_.Y << ',' << cameraPosition_.Z << "; camera terrain "
                << terrain_.Height(cameraPosition_.X, cameraPosition_.Z);
            std::cout << monkeyCameraReport.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(monkeyCameraReport.str());
            const Vector3 monkeyTarget(monkey.x, monkey.y
                + (viewpoint_==FixedViewpoint::MonkeyClimbDetail ? 1.8f
                    : viewpoint_==FixedViewpoint::MonkeyDetail ? .82f : viewpoint_==FixedViewpoint::MonkeyScratchDetail ? .6f : .52f), monkey.z);
            const Vector3 toMonkey = monkeyTarget-cameraPosition_;
            yaw_ = std::atan2(toMonkey.X, toMonkey.Z);
            pitch_ = std::atan2(toMonkey.Y, std::hypot(toMonkey.X, toMonkey.Z));
        } else if (viewpoint_ == FixedViewpoint::FallenLogDetail) {
            // Stand three metres off the flank of the first fallen log, looking at its side.
            const auto& logs = naturalPropLayout_.FallenLogs();
            if (logs.empty()) throw std::runtime_error("No fallen log to frame");
            const auto& log = logs.front();
            const float acrossX = -std::sin(log.yawRadians), acrossZ = std::cos(log.yawRadians);
            cameraPosition_ = Vector3(log.x + acrossX * 3.1f, log.y + 1.35f, log.z + acrossZ * 3.1f);
            const Vector3 direction = Vector3(log.x, log.y + 0.15f, log.z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: log QA camera frames the flank of a fallen log\n";
        } else if (viewpoint_ == FixedViewpoint::BeachVineDetail) {
            // Stand four metres up the beach from a crown of beach morning glory, looking down
            // over it and its runners toward the sea.
            const auto& vines = naturalPropLayout_.BeachVines();
            if (vines.empty()) throw std::runtime_error("No beach morning glory to frame");
            const auto& vine = vines[vines.size() / 2];
            const float seaX = std::cos(vine.yawRadians), seaZ = -std::sin(vine.yawRadians);
            cameraPosition_ = Vector3(vine.x - seaX * 4.0f, vine.y + 1.7f, vine.z - seaZ * 4.0f);
            const Vector3 direction = Vector3(vine.x + seaX * 0.8f, vine.y, vine.z + seaZ * 0.8f) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: beach-vine QA camera frames a crown of beach morning glory\n";
        } else if (viewpoint_ == FixedViewpoint::FruitTreeDetail) {
            // Frame the crown of the first fruit-bearing canopy form from eleven metres.
            const auto& canopy = jungleLayout_.Canopy();
            const auto tree = std::find_if(canopy.begin(), canopy.end(),
                [](const CanopyPlacement& candidate) { return candidate.variant == 2U; });
            if (tree == canopy.end()) throw std::runtime_error("No fruit-bearing canopy tree");
            const float crownY = tree->y + 7.2f * tree->scale;
            cameraPosition_ = Vector3(tree->x + 8.5f, crownY - 4.2f, tree->z + 7.0f);
            const Vector3 direction = Vector3(tree->x, crownY, tree->z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: fruit QA camera frames a fruit-bearing canopy crown\n";
        } else if (viewpoint_ == FixedViewpoint::LookoutDetail) {
            // Stand beside the ramp foot and look up it to the railed platform.
            if(!lookoutPlacement_) throw std::runtime_error("Lookout QA requires a valid site");
            const auto& l=*lookoutPlacement_;
            const float cameraX=l.x+l.dirX*(LookoutPlacement::Half+LookoutPlacement::RampLength+3.0f)+l.AcrossX()*4.0f;
            const float cameraZ=l.z+l.dirZ*(LookoutPlacement::Half+LookoutPlacement::RampLength+3.0f)+l.AcrossZ()*4.0f;
            cameraPosition_=Vector3(cameraX,terrain_.Height(cameraX,cameraZ)+PlayerMotion::EyeHeight,cameraZ);
            const Vector3 target(l.x,l.platformY+0.6f,l.z);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: lookout QA camera looks up the ramp at the platform\n";
        } else if (viewpoint_ == FixedViewpoint::BridgeDetail) {
            // Stand on the left bank a little downstream and look across the span.
            if(!bridgePlacement_) throw std::runtime_error("Bridge QA requires a valid site");
            const auto& b=*bridgePlacement_;
            const float cameraX=b.ax-b.dirX*2.2f-b.AcrossX()*4.6f;
            const float cameraZ=b.az-b.dirZ*2.2f-b.AcrossZ()*4.6f;
            cameraPosition_=Vector3(cameraX,terrain_.Height(cameraX,cameraZ)+PlayerMotion::EyeHeight,cameraZ);
            const Vector3 target((b.ax+b.bx)*0.5f,b.DeckHeight(b.span*0.5f)+0.5f,(b.az+b.bz)*0.5f);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: bridge QA camera looks across the rope bridge from the bank\n";
        } else if (viewpoint_ == FixedViewpoint::FlightOverview) {
            // The operator's usual view: flying about forty metres over the forest behind the
            // hut, looking out across the trees, the hut's clearing and the beach to the sea.
            if(!hutPlacement_||!jettyPlacement_) throw std::runtime_error("Flight QA requires the hut and the jetty");
            const auto& h=*hutPlacement_;
            const auto& j=*jettyPlacement_;
            const float towardX=j.rootX-h.x,towardZ=j.rootZ-h.z;
            const float length=std::max(std::hypot(towardX,towardZ),1.0f);
            const float cameraX=h.x-towardX/length*45.0f,cameraZ=h.z-towardZ/length*45.0f;
            cameraPosition_=Vector3(cameraX,terrain_.Height(cameraX,cameraZ)+40.0f,cameraZ);
            const Vector3 target(j.rootX+towardX/length*30.0f,0.0f,j.rootZ+towardZ/length*30.0f);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: flight QA camera looks out from forty metres over the forest behind the hut to the sea\n";
        } else if (viewpoint_ == FixedViewpoint::JettyEndDetail) {
            // Standing at the end of the jetty, looking out over the water (the rain's rings on
            // the sea, the swell under the deck).
            if(!jettyPlacement_) throw std::runtime_error("Jetty QA requires a valid site");
            const auto& j=*jettyPlacement_;
            const float along=JettyPlacement::Length-1.0f;
            cameraPosition_=Vector3(j.rootX+j.dirX*along,j.DeckHeight(along)+PlayerMotion::EyeHeight,j.rootZ+j.dirZ*along);
            yaw_=std::atan2(j.dirX+j.AcrossX()*0.35f,j.dirZ+j.AcrossZ()*0.35f);
            pitch_=-0.32f;
            std::cout<<"island-simulator: jetty-end QA camera looks out over the water from the end of the deck\n";
        } else if (viewpoint_ == FixedViewpoint::JettyDetail) {
            // Stand on the sand at the far end of the resident's footprints and look back along
            // them past the canoe to the deck and the mooring post.
            if(!jettyPlacement_) throw std::runtime_error("Jetty QA requires a valid site");
            const auto& j=*jettyPlacement_;
            const auto prints=j.FootprintLines(terrain_,RenderedTerrainSurface(terrain_));
            const auto& last=prints.back();
            const float backX=last.x-(last.x-j.rootX)*0.25f, backZ=last.z-(last.z-j.rootZ)*0.25f;
            const float cameraX=last.x+(last.x-backX)*1.2f-j.dirX*1.5f;
            const float cameraZ=last.z+(last.z-backZ)*1.2f-j.dirZ*1.5f;
            cameraPosition_=Vector3(cameraX,terrain_.Height(cameraX,cameraZ)+PlayerMotion::EyeHeight,cameraZ);
            const Vector3 target(j.rootX+j.dirX*6.0f,j.deckY+0.2f,j.rootZ+j.dirZ*6.0f);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: jetty QA camera looks along the deck from the canoe\n";
        } else if (viewpoint_ == FixedViewpoint::StreamCascadeDetail) {
            // Stand on the bank beside the plunge pool and look back up at the lip.
            const StreamCourse course(terrain_);
            const auto& lip = course.Station(course.CascadeIndex());
            const auto& pool = course.PoolStation();
            const float flowX = pool.x - lip.x, flowZ = pool.z - lip.z;
            const float flowLength = std::max(std::hypot(flowX, flowZ), 0.001f);
            const float sideX = flowZ / flowLength, sideZ = -flowX / flowLength;
            const float cameraX = pool.x + flowX / flowLength * 3.0f - sideX * 2.6f;
            const float cameraZ = pool.z + flowZ / flowLength * 3.0f - sideZ * 2.6f;
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ) + 1.35f, cameraZ);
            const Vector3 direction = Vector3(lip.x, lip.water, lip.z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: cascade QA camera looks up the plunge pool at the stream lip\n";
        } else if (viewpoint_ == FixedViewpoint::AnimalTrackDetail) {
            // Stand beside the first stream-bank deer print line and look down along it.
            const auto& tracks = naturalPropLayout_.AnimalTracks();
            const auto track = std::find_if(tracks.begin(), tracks.end(),
                [](const AnimalTrackPlacement& candidate) {
                    return candidate.streamBank && candidate.variant == 0U; });
            if (track == tracks.end()) throw std::runtime_error("No stream-bank deer track line");
            const float acrossX = std::cos(track->yawRadians), acrossZ = -std::sin(track->yawRadians);
            cameraPosition_ = Vector3(track->x + acrossX * 1.25f, track->y + 1.15f,
                                      track->z + acrossZ * 1.25f);
            const Vector3 direction = Vector3(track->x, track->y, track->z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: tracks QA camera looks down on a deer print line beside the stream\n";
        } else if (viewpoint_ == FixedViewpoint::ShipwreckDetail
                   || viewpoint_ == FixedViewpoint::ShipwreckWide) {
            const ShipwreckPlacement& ship = shipwreckLayout_.Ship();
            const float radius = std::hypot(ship.x, ship.z);
            const float outwardX = ship.x / radius;
            const float outwardZ = ship.z / radius;
            const float tangentX = -outwardZ;
            const float tangentZ = outwardX;
            // The wide stance is the matched landmark-distance comparison: the same bearing
            // from the beach, but fifty metres along the shore at ordinary eye height.
            const bool wide = viewpoint_ == FixedViewpoint::ShipwreckWide;
            const float cameraX = ship.x - outwardX * (wide ? 14.0f : 17.5f)
                + tangentX * (wide ? 50.0f : 4.8f);
            const float cameraZ = ship.z - outwardZ * (wide ? 14.0f : 17.5f)
                + tangentZ * (wide ? 50.0f : 4.8f);
            cameraPosition_ = Vector3(cameraX, terrain_.Height(cameraX, cameraZ)
                                                   + PlayerMotion::EyeHeight,
                                      cameraZ);
            const Vector3 toShip = Vector3(ship.x, 1.2f, ship.z) - cameraPosition_;
            yaw_ = std::atan2(toShip.X, toShip.Z);
            pitch_ = std::atan2(toShip.Y, std::hypot(toShip.X, toShip.Z));
            std::cout << (wide ? "island-simulator: shipwreck QA camera views the 20m wreck "
                                 "from fifty metres along the beach\n"
                               : "island-simulator: shipwreck QA camera views the 20m wreck "
                                 "from the beach\n");
        } else if (viewpoint_ == FixedViewpoint::HutDetail || viewpoint_ == FixedViewpoint::HutMaterialDetail) {
            if(!hutPlacement_) throw std::runtime_error("Hut QA requires a valid site");
            const auto& h=*hutPlacement_;
            const float c=std::cos(h.yawRadians),s=std::sin(h.yawRadians);
            const bool close=viewpoint_==FixedViewpoint::HutMaterialDetail;
            // The two routes jointly expose all four walls: overview from the
            // front/right, material review from the formerly unchecked rear/left.
            const float x=close?-4.2f:6.0f,z=close?-5.0f:9.0f;
            cameraPosition_=Vector3(h.x+c*x+s*z,h.floorY+3.2f,h.z-s*x+c*z);
            const Vector3 direction=Vector3(h.x,h.floorY+(close?2.0f:1.5f),h.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        } else if (viewpoint_ == FixedViewpoint::SnakeDetail || viewpoint_ == FixedViewpoint::SnakeHeadDetail
                || viewpoint_==FixedViewpoint::TreeSnakeDetail) {
            const auto& snake = viewpoint_==FixedViewpoint::TreeSnakeDetail
                ? snakes_->TreeSnakes().front() : snakes_->Snakes().front();
            const auto head = snake.body.front();
            Vector3 centre = Vector3::Zero;
            for (const auto& p : snake.body) centre += Vector3(p.x,p.y,p.z);
            centre /= static_cast<float>(SnakeState::StationCount);
            // Frame the entire body from above; the old oblique head-relative
            // camera crossed nearby broad leaves and lost the retreating head.
            cameraPosition_ = centre + Vector3(0.0f,2.0f,0.35f);
            if(viewpoint_==FixedViewpoint::TreeSnakeDetail) {
                Vector3 outward(head.x-snake.homeX,0,head.z-snake.homeZ);
                if(outward.LengthSquared()<.001f)outward=Vector3::Forward;
                outward.Normalize();
                cameraPosition_=Vector3(head.x,head.y,head.z)+outward*1.8f+Vector3::Up*.10f;
                std::cout<<"island-simulator: tree-snake QA frames a live climber on the sampled trunk surface\n";
            }
            if(viewpoint_ == FixedViewpoint::SnakeHeadDetail) {
                const Vector3 headPosition(head.x,head.y,head.z);
                const auto neck=snake.body[1];
                const Vector3 forward=Vector3::Normalize(headPosition-Vector3(neck.x,neck.y,neck.z));
                const Vector3 right=Vector3::Normalize(Vector3::Cross(Vector3::Up,forward));
                cameraPosition_=headPosition+forward*.38f+right*.25f+Vector3::Up*.10f;
                std::cout << "island-simulator: snake-head QA isolates facial geometry while the companion body view retains habitat grass\n";
            }
            const Vector3 target = viewpoint_ == FixedViewpoint::SnakeHeadDetail
                ? Vector3(head.x,head.y+.012f,head.z)
                : viewpoint_==FixedViewpoint::TreeSnakeDetail
                    ? Vector3(head.x,head.y,head.z)*.60f+centre*.40f : centre;
            const Vector3 direction = target-cameraPosition_;
            yaw_ = std::atan2(direction.X,direction.Z);
            pitch_ = std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        } else if(viewpoint_==FixedViewpoint::SpiderWebDetail) {
            if(spiderWebSites_.empty())throw std::runtime_error("Spider-web QA found no qualified site");
            const RenderedTerrainSurface surface(terrain_);
            std::size_t webViewIndex=0;float bestWebScore=-1;
            for(std::size_t index=0;index<spiderWebSites_.size();++index) {
                const auto candidate=SpiderWebLayout::Sample(spiderWebSites_[index],jungleLayout_,surface,1.0f);
                const Vector3 p0(candidate[0][0],candidate[0][1],candidate[0][2]);
                const Vector3 p1(candidate[1][0],candidate[1][1],candidate[1][2]);
                const Vector3 p2(candidate[2][0],candidate[2][1],candidate[2][2]);
                const float area=Vector3::Cross(p1-p0,p2-p0).Length()*.5f;
                const float squaredEdges=(p1-p0).LengthSquared()+(p2-p1).LengthSquared()
                    +(p0-p2).LengthSquared();
                const auto& candidateTree=jungleLayout_.Canopy()[spiderWebSites_[index].canopyIndex];
                const Vector3 candidateCentre=(p0+p1+p2)/3.0f;
                const float radial=std::hypot(candidateCentre.X-candidateTree.x,
                    candidateCentre.Z-candidateTree.z);
                const float score=4.0f*std::sqrt(3.0f)*area/squaredEdges+.02f*radial;
                if(score>bestWebScore){bestWebScore=score;webViewIndex=index;}
            }
            const auto anchors=SpiderWebLayout::Sample(spiderWebSites_[webViewIndex],jungleLayout_,surface,1.0f);
            const Vector3 a(anchors[0][0],anchors[0][1],anchors[0][2]);
            const Vector3 b(anchors[1][0],anchors[1][1],anchors[1][2]);
            const Vector3 c(anchors[2][0],anchors[2][1],anchors[2][2]);
            const Vector3 centre=(a+b+c)/3.0f;
            Vector3 normal=Vector3::Cross(b-a,c-a);
            if(normal.LengthSquared()<1e-8f)throw std::runtime_error("Spider-web QA site is degenerate");
            normal.Normalize();
            const auto& tree=jungleLayout_.Canopy()[spiderWebSites_[webViewIndex].canopyIndex];
            const Vector3 outward(centre.X-tree.x,0,centre.Z-tree.z);
            if(Vector3::Dot(normal,outward)<0)normal=-normal;
            cameraPosition_=centre+normal*1.05f+Vector3::Up*.06f;
            const Vector3 direction=centre-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: spider-web QA camera frames qualified canopy site "
                <<webViewIndex<<" regularity-offset score "<<bestWebScore<<'\n';
        } else if (UsesSpiderViewpoint()) {
            // Both spider views frame the sunlit subject: since the sun shade and the leaf-gap
            // flecks (2026-10-20) a shaded spot falls below the close-detail luminance bar.
            {
                float brightest=-1;
                for(std::size_t i=0;i<spiders_->Spiders().size();++i) {
                    const auto& candidate=spiders_->Spiders()[i];
                    // The sunlit score: the canopy's transmission to the body, dimmed by the
                    // cumulus shadow over the spot at the capture's start and by every crown
                    // or palm head that lies on the spot's line to the sun (the sun shadow
                    // map's casters), so the bright view is framed on sunlit ground.
                    const float cover=CloudShadow::Cover(candidate.x,candidate.z,0.0f);
                    int casters=0;
                    const auto shades=[&](const float cx,const float cz,const float crownY,const float crownRadius) {
                        const float s=(crownY-candidate.y)/environment::SunDirectionY;
                        if(s<=0.0f) return false;
                        const float rx=candidate.x+environment::SunDirectionX*s,rz=candidate.z+environment::SunDirectionZ*s;
                        return std::hypot(rx-cx,rz-cz)<crownRadius;
                    };
                    // Every layer the shadow pass draws: crowns, palm heads, standing snags,
                    // the understory and the bushes, each as a disc at its body's height.
                    for(const auto& tree:jungleLayout_.Canopy())
                        if(shades(tree.x,tree.z,tree.y+7.0f*tree.scale,3.2f*tree.scale)) ++casters;
                    for(const auto& palm:vegetationLayout_.Palms())
                        if(shades(palm.x,palm.z,palm.y+9.0f*palm.scale,2.6f*palm.scale)) ++casters;
                    for(const auto& snag:jungleLayout_.StandingSnags())
                        if(shades(snag.x,snag.z,snag.y+3.0f*snag.scale,0.6f*snag.scale)) ++casters;
                    for(const auto& plant:jungleLayout_.Understory())
                        if(shades(plant.x,plant.z,plant.y+2.2f*plant.scale,1.5f*plant.scale)) ++casters;
                    for(const auto& bush:jungleLayout_.Bushes())
                        if(shades(bush.x,bush.z,bush.y+0.9f*bush.scale,1.1f*bush.scale)) ++casters;
                    const float transmission=canopyLightField_.SampleReceiver(
                        candidate.x,candidate.y+.04f,candidate.z,candidate.y)*(1.0f-.85f*cover)
                        /(1.0f+static_cast<float>(casters));
                    std::ostringstream row;row.precision(9);
                    row << "island-simulator: spider habitat candidate=" << i
                        << " transmission=" << transmission << " cover=" << cover << " casters=" << casters;
                    std::cout << row.str() << std::endl;MirrorRuntimeDiagnosticToPlatformLog(row.str());
                    if(transmission>brightest) {brightest=transmission;spiderViewSubject_=i;}
                }
                const auto selected="island-simulator: spider habitat selected="+std::to_string(spiderViewSubject_);
                std::cout << selected << std::endl;MirrorRuntimeDiagnosticToPlatformLog(selected);
            }
            const auto& spider=spiders_->Spiders()[spiderViewSubject_];
            const Vector3 forward(std::cos(spider.heading),0,std::sin(spider.heading));
            const Vector3 right(forward.Z,0,-forward.X);
            cameraPosition_=Vector3(spider.x,spider.y+.24f,spider.z)+forward*.38f+right*.30f;
            const auto direction=Vector3(spider.x,spider.y+.035f,spider.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        } else if (viewpoint_ == FixedViewpoint::GoatDetail || viewpoint_ == FixedViewpoint::GoatLyingDetail) {
            // The lying view holds the second goat down at rest, framed nearer and lower.
            const bool lying=viewpoint_==FixedViewpoint::GoatLyingDetail;
            if(lying) goats_->PreviewLie(1U);
            const auto& goat=lying?goats_->Goats()[1U]:goats_->Goats().front();
            const float x=goat.x+(lying?5.0f:7.5f),z=goat.z+(lying?2.6f:4.0f);
            cameraPosition_=Vector3(x,std::max(terrain_.Height(x,z)+.4f,goat.y+(lying?1.6f:2.1f)),z);
            const auto direction=Vector3(goat.x,goat.y+(lying?.35f:.75f),goat.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            if(lying) std::cout<<"island-simulator: goat QA camera frames one resting goat lying on folded legs\n";
        } else if (viewpoint_ == FixedViewpoint::DeerDetail || viewpoint_ == FixedViewpoint::DeerBeddedDetail) {
            const bool bedded=viewpoint_==FixedViewpoint::DeerBeddedDetail;
            // The bedded view sets the antlered deer down as a shade rest does and frames it
            // a little lower and closer.
            if(bedded) deer_->PreviewBed(0);
            const auto& deer=deer_->Deer().front();
            const Vector3 forward(std::cos(deer.headingRadians),0,std::sin(deer.headingRadians));
            const Vector3 side(-forward.Z,0,forward.X);
            const float reach=bedded?5.6f:7.0f;
            const float x=deer.x-forward.X*2.4f+side.X*reach;
            const float z=deer.z-forward.Z*2.4f+side.Z*reach;
            cameraPosition_=Vector3(x,std::max(terrain_.Height(x,z)+.45f,deer.y+(bedded?1.5f:2.1f)),z);
            const auto direction=Vector3(deer.x,deer.y+(bedded?.55f:1.05f),deer.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<(bedded?"island-simulator: deer QA camera frames one antlered clearing animal bedded down broadside\n"
                              :"island-simulator: deer QA camera frames one antlered clearing animal broadside\n");
        } else if (viewpoint_ == FixedViewpoint::BoarDetail || viewpoint_ == FixedViewpoint::BoarLyingDetail) {
            // The lying view holds the second boar down at rest, framed lower and nearer.
            const bool lying = viewpoint_ == FixedViewpoint::BoarLyingDetail;
            if (lying) boarHerd_.PreviewLie(1U);
            const auto& boar = lying ? boarHerd_.Boars()[1U] : boarHerd_.Boars().front();
            // Telephoto inspection outside the 6m retreat trigger; habitat stays unchanged.
            const float c=std::cos(boar.headingRadians),s=std::sin(boar.headingRadians);
            // The lying body sits low in the grass, so that view looks down from nearer and higher.
            const float reach = lying ? 4.2f : 6.6f;
            // (the lying boar is viewed from its other flank, clear of the understory there).
            const float side = lying ? -1.0f : 1.0f;
            const float x = boar.x + c*(lying?1.6f:2.8f) + s*reach*side;
            const float z = boar.z + s*(lying?1.6f:2.8f) - c*reach*side;
            cameraPosition_ = Vector3(x, std::max(terrain_.Height(x,z)+0.4f,boar.y+(lying?2.3f:2.4f)), z);
            const Vector3 direction = Vector3(boar.x, boar.y + (lying?0.3f:0.5f), boar.z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << (lying ? "island-simulator: boar QA camera frames one resting boar lying on folded legs\n"
                                : "island-simulator: boar QA camera frames and follows non-hostile inland wildlife\n");
        } else if (viewpoint_ == FixedViewpoint::CrabDetail
                   && crabColony_.Crabs().size() > 2U) {
            const auto crabs=crabColony_.Crabs();
            const CrabBurrow& burrow=crabColony_.Burrows().front();
            float nearestBurrow=std::numeric_limits<float>::infinity();
            for(std::size_t candidate=0;candidate<5U;++candidate) {
                const float distance=std::hypot(crabs[candidate].x-burrow.x,
                    crabs[candidate].z-burrow.z);
                if(distance<nearestBurrow) {nearestBurrow=distance;crabViewSubject_=candidate;}
            }
            const float radius = std::hypot(burrow.x, burrow.z);
            const float outwardX = burrow.x / radius;
            const float outwardZ = burrow.z / radius;
            const float tangentX = -outwardZ;
            const float tangentZ = outwardX;
            // An elevated oblique view keeps the debris cue, burrow mouth and several
            // palm-sized animals readable without placing the lens inside the driftwood.
            const float cameraX = burrow.x - outwardX * 2.2f + tangentX * 2.8f;
            const float cameraZ = burrow.z - outwardZ * 2.2f + tangentZ * 2.8f;
            cameraPosition_ = Vector3(cameraX,
                std::max(terrain_.Height(cameraX, cameraZ)+1.45f,burrow.y+2.15f),cameraZ);
            const Vector3 toCrab = Vector3(burrow.x, burrow.y + 0.08f, burrow.z)
                - cameraPosition_;
            yaw_ = std::atan2(toCrab.X, toCrab.Z);
            pitch_ = std::atan2(toCrab.Y, std::hypot(toCrab.X, toCrab.Z));
            std::cout << "island-simulator: crab QA camera frames a debris-linked burrow and its foraging colony\n";
            std::cout << "island-simulator: crab QA subject " << crabViewSubject_
                      << " is " << nearestBurrow << "m from the burrow\n";
        } else if (viewpoint_ == FixedViewpoint::ReefHeronDetail) {
            // Beside the first heron on the watch, broadside, from a wader's eye height.
            const ReefHeronState& heron = reefHeronGroup_.Herons().front();
            const Vector3 forward(std::cos(heron.headingRadians), 0.0f, std::sin(heron.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            // from the landward side, broadside to the bird, the water behind it
            const Vector3 seaward(heron.x, 0.0f, heron.z);
            const float landSide = Vector3::Dot(side, seaward) > 0.0f ? -1.0f : 1.0f;
            const float cameraX = heron.x + side.X * 1.45f * landSide + forward.X * 0.25f;
            const float cameraZ = heron.z + side.Z * 1.45f * landSide + forward.Z * 0.25f;
            cameraPosition_ = Vector3(cameraX, std::max(terrain_.Height(cameraX, cameraZ), 0.0f) + 0.55f, cameraZ);
            const Vector3 target(heron.x + forward.X * 0.12f, heron.y + 0.42f * heron.scale, heron.z + forward.Z * 0.12f);
            const Vector3 direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: reef-heron QA camera frames one bird on the watch in the shallows\n";
        } else if (viewpoint_ == FixedViewpoint::HermitCrabDetail
                   && !hermitCrabGroup_.Crabs().empty()) {
            const HermitCrabState& crab = hermitCrabGroup_.Crabs()[3U];
            const Vector3 forward(std::cos(crab.headingRadians), 0.0f,
                                  std::sin(crab.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            const float cameraX = crab.x + forward.X * .28f + side.X * .20f;
            const float cameraZ = crab.z + forward.Z * .28f + side.Z * .20f;
            cameraPosition_ = Vector3(cameraX,
                std::max(terrain_.Height(cameraX, cameraZ) + .14f, crab.y + .17f), cameraZ);
            const Vector3 target(crab.x, crab.y + .055f, crab.z);
            const Vector3 direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: hermit-crab QA camera frames one shell-bearing animal on sheltered sand\n";
        } else if (viewpoint_ == FixedViewpoint::CoconutCrabDetail
                   && coconutCrabGroup_.Crabs().size() > 2U) {
            const CoconutCrabState& crab = coconutCrabGroup_.Crabs()[2U];
            const Vector3 forward(std::cos(crab.headingRadians), 0.0f,
                                  std::sin(crab.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            const float cameraX = crab.x + forward.X * 2.15f + side.X * .48f;
            const float cameraZ = crab.z + forward.Z * 2.15f + side.Z * .48f;
            cameraPosition_ = Vector3(cameraX,
                std::max(terrain_.Height(cameraX, cameraZ) + .76f, crab.y + .92f), cameraZ);
            const Vector3 target(crab.x, crab.y + .18f, crab.z);
            const Vector3 direction = target - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: coconut-crab QA camera frames one large terrestrial animal in coastal vegetation\n";
        } else if (viewpoint_ == FixedViewpoint::CrocodileDetail
                   && !crocodileGroup_.Crocodiles().empty()) {
            const CrocodileState& crocodile=crocodileGroup_.Crocodiles()[2U];
            const Vector3 forward(std::cos(crocodile.headingRadians),0,
                                  std::sin(crocodile.headingRadians));
            Vector3 side(-forward.Z,0,forward.X);
            const float firstHeight=terrain_.Height(crocodile.x+side.X*5.5f,
                                                    crocodile.z+side.Z*5.5f);
            const float secondHeight=terrain_.Height(crocodile.x-side.X*5.5f,
                                                     crocodile.z-side.Z*5.5f);
            // Review from the clearer waterward side. The landward view crosses the dense
            // mangrove trunks that deliberately shelter this estuary habitat.
            if(secondHeight<firstHeight)side=-side;
            const float cameraX=crocodile.x+side.X*5.5f-forward.X*2.0f;
            const float cameraZ=crocodile.z+side.Z*5.5f-forward.Z*2.0f;
            cameraPosition_=Vector3(cameraX,std::max(terrain_.Height(cameraX,cameraZ)+1.35f,
                                                     crocodile.y+1.65f),cameraZ);
            const Vector3 target(crocodile.x,crocodile.y+.35f,crocodile.z);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: crocodile QA camera frames an estuary animal broadside\n";
        } else if (viewpoint_ == FixedViewpoint::ButterflyDetail
                   && butterflyMeadow_.Butterflies().size() > 2U) {
            const ButterflyState& butterfly=butterflyMeadow_.Butterflies()[2U];
            const Vector3 forward(std::cos(butterfly.headingRadians),0,
                                  std::sin(butterfly.headingRadians));
            const Vector3 side(-forward.Z,0,forward.X);
            const float cameraX=butterfly.x-forward.X*.62f+side.X*.30f;
            const float cameraZ=butterfly.z-forward.Z*.62f+side.Z*.30f;
            cameraPosition_=Vector3(cameraX,
                std::max(terrain_.Height(cameraX,cameraZ)+.28f,butterfly.y+.26f),cameraZ);
            const Vector3 toButterfly=Vector3(
                butterfly.x,butterfly.y+.015f,butterfly.z)-cameraPosition_;
            yaw_ = std::atan2(toButterfly.X, toButterfly.Z);
            pitch_ = std::atan2(
                toButterfly.Y, std::hypot(toButterfly.X, toButterfly.Z));
            std::cout << "island-simulator: butterfly QA camera follows one live near animal at its nectar patch\n";
        } else if (viewpoint_ == FixedViewpoint::GeckoDetail
                   && geckoPopulation_.Geckos().size() > 2U) {
            // The subject is the gecko on the best-lit ground, so the close view keeps its
            // contrast under the tree shadows and cloud shade the forest floor now carries.
            float bestLight=-1.0f;
            for(std::size_t i=0;i<geckoPopulation_.Geckos().size();++i) {
                const GeckoState& candidate=geckoPopulation_.Geckos()[i];
                const float light=canopyLightField_.Sample(candidate.x,candidate.z);
                if(light>bestLight) {bestLight=light;geckoQaSubjectIndex_=i;}
            }
            const GeckoState& gecko=geckoPopulation_.Geckos()[geckoQaSubjectIndex_];
            std::cout<<"island-simulator: gecko QA subject "<<geckoQaSubjectIndex_<<" on ground with canopy light "
                <<bestLight<<" and cloud cover "<<CloudShadow::Cover(gecko.x,gecko.z,0.0f)<<"\n";
            const Vector3 forward(std::cos(gecko.headingRadians),0.0f,
                                  std::sin(gecko.headingRadians));
            const Vector3 side(-forward.Z,0.0f,forward.X);
            // Low and a little further back than before: the frame keeps the ground beyond
            // the animal and the stand behind it, not only the bare floor under the camera.
            const float cameraX=gecko.x-forward.X*.85f+side.X*.45f;
            const float cameraZ=gecko.z-forward.Z*.85f+side.Z*.45f;
            cameraPosition_=Vector3(cameraX,
                std::max(terrain_.Height(cameraX,cameraZ)+.26f,gecko.y+.24f),cameraZ);
            const Vector3 target(gecko.x,gecko.y+.080f,gecko.z);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout << "island-simulator: gecko QA camera frames a resting inland lizard at ground level\n";
        } else if (viewpoint_ == FixedViewpoint::SkinkDetail
                   && skinkPopulation_.Skinks().size() > 1U) {
            const SkinkState& skink = skinkPopulation_.Skinks()[1U];
            const Vector3 forward(std::cos(skink.headingRadians), 0.0f,
                                  std::sin(skink.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            const float cameraX = skink.x - forward.X * .62f + side.X * .38f;
            const float cameraZ = skink.z - forward.Z * .62f + side.Z * .38f;
            cameraPosition_ = Vector3(cameraX,
                std::max(terrain_.Height(cameraX, cameraZ) + .38f, skink.y + .35f), cameraZ);
            const Vector3 direction = Vector3(skink.x, skink.y + .065f, skink.z)
                - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: skink QA camera frames one striped basking lizard on open inland ground\n";
        } else if(viewpoint_==FixedViewpoint::DragonflyDetail&&!dragonflySwarm_.Dragonflies().empty()) {
            const auto& fly=dragonflySwarm_.Dragonflies()[1U];
            const Vector3 forward(std::cos(fly.headingRadians),0,std::sin(fly.headingRadians));
            const Vector3 side(-forward.Z,0,forward.X);
            // Inspect the long abdomen, paired wings and compound eyes broadside;
            // a rear-quarter view visually collapsed them into an aircraft shape.
            cameraPosition_=Vector3(fly.x-forward.X*.025f+side.X*.205f,fly.y+.100f,
                                    fly.z-forward.Z*.025f+side.Z*.205f);
            const Vector3 direction=Vector3(fly.x,fly.y,fly.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: dragonfly QA camera frames one visible wet-habitat insect broadside\n";
        } else if (viewpoint_ == FixedViewpoint::FrogDetail
                   && frogPopulation_.Frogs().size() > 1U) {
            const FrogState& frog = frogPopulation_.Frogs()[1U];
            const Vector3 forward(std::cos(frog.headingRadians), 0.0f,
                                  std::sin(frog.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            const float cameraX = frog.x + forward.X * .46f + side.X * .34f;
            const float cameraZ = frog.z + forward.Z * .46f + side.Z * .34f;
            cameraPosition_ = Vector3(cameraX,
                std::max(terrain_.Height(cameraX, cameraZ) + .34f, frog.y + .31f), cameraZ);
            const Vector3 direction = Vector3(frog.x, frog.y + .075f, frog.z)
                - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: frog QA camera frames one resting stream-bank animal\n";
        } else if (viewpoint_ == FixedViewpoint::MouseDetail) {
            const RodentState& mouse = rodentPopulation_.Rodents().front();
            const Vector3 forward(std::cos(mouse.headingRadians),0,std::sin(mouse.headingRadians));
            const Vector3 side(-forward.Z,0,forward.X);
            cameraPosition_=Vector3(mouse.x,mouse.y+.34f,mouse.z)
                +forward*.88f+side*.62f;
            const Vector3 direction=Vector3(mouse.x,mouse.y+.055f,mouse.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: mouse QA camera frames one field mouse at ground level\n";
        } else if (viewpoint_ == FixedViewpoint::RatDetail) {
            const RodentState& rat = rodentPopulation_.Rodents()[RodentPopulation::MouseCount + 6U];
            const Vector3 forward(std::cos(rat.headingRadians),0,std::sin(rat.headingRadians));
            const Vector3 side(-forward.Z,0,forward.X);
            cameraPosition_=Vector3(rat.x,rat.y+.43f,rat.z)+forward*1.18f-side*.80f;
            const Vector3 direction=Vector3(rat.x,rat.y+.075f,rat.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: rat QA camera frames one larger inland rat at ground level\n";
        } else if (viewpoint_ == FixedViewpoint::FlyDetail) {
            const FlyState& fly = flySwarms_.Flies()[FlySwarms::PerSwarm];
            const Vector3 forward(std::cos(fly.headingRadians), 0.0f,
                                  std::sin(fly.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(fly.x, fly.y + .045f, fly.z)
                + forward * .16f + side * .12f;
            const Vector3 direction = Vector3(fly.x, fly.y, fly.z) - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: fly QA camera frames one close insect within its inland swarm\n";
        } else if(viewpoint_==FixedViewpoint::AntDetail) {
            const AntState& ant=antTrails_.Ants()[5U];
            const Vector3 forward(ant.forwardX,ant.forwardY,ant.forwardZ);
            const Vector3 up(ant.normalX,ant.normalY,ant.normalZ);
            const Vector3 side=Vector3::Normalize(Vector3::Cross(up,forward));
            cameraPosition_=Vector3(ant.x,ant.y,ant.z)+up*.045f+side*.15f-forward*.03f;
            const Vector3 direction=Vector3(ant.x,ant.y,ant.z)+up*.018f-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
            std::cout<<"island-simulator: ant QA camera frames a visible worker on one inland soil trail\n";
        } else if (viewpoint_ == FixedViewpoint::CicadaDetail) {
            const CicadaState& cicada = cicadaColony_.Cicadas()[1U];
            const Vector3 outward(cicada.normalX, cicada.normalY, cicada.normalZ);
            const Vector3 forward(cicada.forwardX, cicada.forwardY, cicada.forwardZ);
            const Vector3 side = Vector3::Normalize(Vector3::Cross(outward, forward));
            cameraPosition_ = Vector3(cicada.x, cicada.y, cicada.z)
                + outward * .35f + side * .10f + forward * .02f;
            const Vector3 direction = Vector3(cicada.x, cicada.y, cicada.z)
                + outward * .025f - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: cicada QA camera frames one trunk-resting animal broadside\n";
        } else if (viewpoint_ == FixedViewpoint::BeetleDetail) {
            const BeetleState& beetle = beetlePopulation_.Beetles()[2U];
            const Vector3 forward(std::cos(beetle.headingRadians), 0.0f,
                                  std::sin(beetle.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(beetle.x, beetle.y + .095f, beetle.z)
                + forward * .310f - side * .250f;
            const Vector3 direction = Vector3(beetle.x, beetle.y + .068f, beetle.z)
                - cameraPosition_;
            yaw_ = std::atan2(direction.X, direction.Z);
            pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
            std::cout << "island-simulator: beetle QA camera frames one foraging animal in inland leaf litter\n";
        }
        graphics_.setGraphicsProfileProperty(GraphicsProfile::HiDef);
        graphics_.setPreferMultiSamplingProperty(qualityProfile_.preferredBackBufferSamples > 0);
        // PreferMultiSampling alone requests up to eight samples in current CNA. Bound the
        // request through its supported preparation event, including future device resets.
        // Capture only the immutable policy value; this callback does not depend on game lifetime.
        graphics_.PreparingDeviceSettings.Add(
            [samples = qualityProfile_.preferredBackBufferSamples](
                System::Object*, const PreparingDeviceSettingsEventArgs& args) {
                args.getGraphicsDeviceInformationEXT().getPresentationParametersProperty()
                    .setMultiSampleCountProperty(samples);
            });
        graphics_.setPreferredBackBufferWidthProperty(1280);
        graphics_.setPreferredBackBufferHeightProperty(720);
        if (!benchmarkPath_.empty()) {
            graphics_.setSynchronizeWithVerticalRetraceProperty(false);
        }
#if defined(__ANDROID__)
        graphics_.setIsFullScreenProperty(true);
        touchOverlayVisible_ = true;
#endif
        getWindowProperty().setTitleProperty("island-simulator");
        setIsMouseVisibleProperty(true);
        // Interactive play steps the world once per drawn frame by the real elapsed time (every
        // system clamps its own step to 0.1 s; the walker's slide and the quad substep inside it).
        // The fixed 60 Hz step made a slow frame run several catch-up Updates before the next
        // Draw, each making it slower still: measured on 2026-10-21 on a Radeon 780M, 4.8 Updates
        // per drawn frame and 12 fps in the jungle, where one Update per frame would have given 18.
        // QA captures, smoke runs and startup reports keep the deterministic fixed step.
        const bool interactivePlay = viewpoint_ == FixedViewpoint::None && capturePath_.empty()
            && !smokeTest_ && startupReportPath_.empty();
        // Capture-only clock: no wall-time catch-up updates between recorded frames.
        setIsFixedTimeStepProperty(!interactivePlay && benchmarkPath_.empty() && !UsesSnakeCaptureClock() && !UsesGoatCaptureClock() && !UsesSpiderCaptureClock() && !UsesBoarCaptureClock()&&!UsesCampfireCaptureClock()&&!UsesResidentCaptureClock()&&!UsesBearCaptureClock()&&!UsesMonkeyCaptureClock()&&!(QuadDriveView()&&!capturePath_.empty()));
        InitializeUserSettings();
#if defined(__EMSCRIPTEN__)
        soundscapeNeedsGesture_ = true;
#endif
        constructionFinished_ = std::chrono::steady_clock::now();
        worldGenerationMilliseconds_ = std::chrono::duration<double, std::milli>(
            constructionFinished_ - constructionStarted_).count();
    }

    void IslandGame::InitializeUserSettings()
    {
        if (!userSettingsEnabled_) {
            std::cout << "island-simulator: user settings disabled for isolated QA run\n";
            return;
        }

        std::string error;
        userSettingsStore_ = UserSettingsStore::Open(error);
        if (!userSettingsStore_) {
            const std::string diagnostic =
                "island-simulator: user settings unavailable; using defaults: " + error;
            std::cerr << diagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
            return;
        }

        const UserSettingsLoadResult loaded = userSettingsStore_->Load();
        userSettings_ = loaded.settings;
        locomotionPaceToggle_.SetRunningSelected(userSettings_.runningSelected);
        cameraMotion_.SetEnabled(userSettings_.cameraMotionEnabled);
        touchControls_.SetLookSensitivity(userSettings_.touchLookSensitivity);
        if (!loaded.warning.empty()) {
            const std::string warning = "island-simulator: " + loaded.warning
                + "; using " + (loaded.usedBackup ? "backup" : "defaults");
            std::cerr << warning << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(warning);
        }
        std::ostringstream diagnostic;
        diagnostic << "island-simulator: user settings "
                   << (loaded.loaded ? (loaded.usedBackup ? "recovered" : "loaded")
                                     : "defaults")
                   << " run " << (userSettings_.runningSelected ? "enabled" : "disabled")
                   << " camera motion "
                   << (userSettings_.cameraMotionEnabled ? "enabled" : "disabled")
                   << " audio " << (userSettings_.audioMuted ? "muted" : "enabled")
                   << " touch " << TouchSensitivityName(userSettings_.touchLookSensitivity)
                   << " at " << userSettingsStore_->Location();
        const std::string diagnosticText = diagnostic.str();
        std::cout << diagnosticText << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(diagnosticText);
    }

    void IslandGame::PersistUserSettings()
    {
        if (!userSettingsStore_) return;
        std::string error;
        if (!userSettingsStore_->Save(userSettings_, error)) {
            const std::string diagnostic =
                "island-simulator: user settings could not be saved: " + error;
            std::cerr << diagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
        } else {
            std::cout << "island-simulator: user settings saved\n";
            MirrorRuntimeDiagnosticToPlatformLog("island-simulator: user settings saved");
        }
    }

    void IslandGame::LoadContent()
    {
        const auto contentStarted = std::chrono::steady_clock::now();
        frameworkInitializationMilliseconds_ = std::chrono::duration<double, std::milli>(
            contentStarted - constructionFinished_).count();
        if (!qualityProfile_.IsValid()) {
            throw std::logic_error("The selected runtime quality profile is invalid");
        }
#if defined(__EMSCRIPTEN__)
        InstallWebMovementKeyBridge();
        constexpr auto webMovementDiagnostic =
            "island-simulator: Web movement bridge mirrors trusted WASD/arrow state and clears on focus loss";
        std::cout << webMovementDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(webMovementDiagnostic);
#endif
        if (GeneratedTextureBudget::TotalRuntimeRgba8Bytes
                > qualityProfile_.textureByteBudget
            || Soundscape::MaximumPlayingSourceCount
                > qualityProfile_.maximumPlayingAudioSources) {
            throw std::logic_error(
                "Runtime texture or audio inventory exceeds the selected quality profile");
        }
        std::ostringstream qualityDiagnostic;
        const std::string buildDiagnostic = "island-simulator: build configuration "
            ISLAND_BUILD_CONFIGURATION;
        std::cout << buildDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(buildDiagnostic);
        qualityDiagnostic << "island-simulator: quality profile " << qualityProfile_.name
                          << " grass "
                          << static_cast<int>(qualityProfile_.grassMaximumDistance)
                          << "m vegetation "
                          << static_cast<int>(qualityProfile_.vegetationMaximumDistanceScale * 100.0f)
                          << "% shadows "
                          << static_cast<int>(qualityProfile_.contactShadowDistanceScale * 100.0f)
                          << "% wildlife "
                          << static_cast<int>(qualityProfile_.wildlifeMaximumDistanceScale * 100.0f)
                          << "% ocean " << qualityProfile_.oceanQuadsPerAxis << 'x'
                          << qualityProfile_.oceanQuadsPerAxis << " textures "
                          << GeneratedTextureBudget::TotalRuntimeRgba8Bytes << '/'
                          << qualityProfile_.textureByteBudget << " bytes audio "
                          << Soundscape::MaximumPlayingSourceCount << '/'
                          << qualityProfile_.maximumPlayingAudioSources << " sources";
        std::cout << qualityDiagnostic.str() << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(qualityDiagnostic.str());
        auto& device = getGraphicsDeviceProperty();
        const int appliedSamples = device.getPresentationParametersProperty().getMultiSampleCountProperty();
        std::ostringstream antialiasingDiagnostic;
        antialiasingDiagnostic << "island-simulator: edge antialiasing requested "
            << qualityProfile_.preferredBackBufferSamples << "x renderer-applied " << appliedSamples << 'x';
        if (appliedSamples < qualityProfile_.preferredBackBufferSamples) {
            antialiasingDiagnostic << " (device-limited; geometric edge quality reduced)";
        }
        std::cout << antialiasingDiagnostic.str() << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(antialiasingDiagnostic.str());
        RecordGraphicsRuntimeDiagnostics(device);
        InitializeGpuDiagnostics();
        // Build consumers against a temporary owner. Commit them before replacing the old
        // owner so a repeated LoadContent never leaves old renderers referencing freed data.
        auto canopyLighting = std::make_unique<CanopyLighting>(device, canopyLightField_, groundCoverField_, terrain_);
        auto terrainHeightMap = std::make_unique<RenderedTerrainHeightMap>(device, terrain_);
        auto terrainRenderer = std::make_unique<TerrainRenderer>(
            device, terrain_, *canopyLighting, bunkerLayout_);
        distantSceneryRenderer_ = std::make_unique<DistantSceneryRenderer>(
            device, distantSceneryLayout_);
        // The per-frame ground marks and shadows lie on the drawn mesh: its height costs ~10 ns,
        // the analytic terrain's microseconds a vertex.
        drawnGround_=std::make_unique<RenderedTerrainSurface>(terrain_);
        contactShadowRenderer_ = std::make_unique<ContactShadowRenderer>(
            device, terrain_, vegetationLayout_, jungleLayout_, naturalPropLayout_,
            shipwreckLayout_, qualityProfile_.contactShadowDistanceScale,
            hutPlacement_?&*hutPlacement_:nullptr,
            helicopterPlacement_?&*helicopterPlacement_:nullptr, drawnGround_.get());
        walkerShadowRenderer_ = std::make_unique<WalkerShadowRenderer>(device);
        constexpr auto coastalShadowDiagnostic =
            "island-simulator: coastal contact patches follow rendered seabed triangles in the existing batch";
        std::cout << coastalShadowDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(coastalShadowDiagnostic);
        constexpr auto landShadowDiagnostic =
            "island-simulator: land prop shadows match model yaw and rendered terrain triangles";
        std::cout << landShadowDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(landShadowDiagnostic);
        constexpr auto structureShadowDiagnostic =
            "island-simulator: wreck, hut, and helicopter project bounded fixed-sun mass shadows in the shared contact batch";
        std::cout << structureShadowDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(structureShadowDiagnostic);
        birdRenderer_ = std::make_unique<BirdRenderer>(
            device, 690.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        fruitBatRenderer_ = std::make_unique<FruitBatRenderer>(
            device, 300.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        butterflyRenderer_ = std::make_unique<ButterflyRenderer>(
            device, 72.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        crabRenderer_ = std::make_unique<CrabRenderer>(
            device, 92.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        crocodileRenderer_ = std::make_unique<CrocodileRenderer>(
            device, 220.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        auto boarRenderer = std::make_unique<BoarRenderer>(
            device, terrain_, canopyLighting->Field(), 135.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        auto snakeRenderer = std::make_unique<SnakeRenderer>(device,canopyLighting->Field());
        auto goatRenderer = std::make_unique<GoatRenderer>(device,canopyLighting->Field(),
            120.0f*qualityProfile_.wildlifeMaximumDistanceScale);
        auto deerRenderer = std::make_unique<DeerRenderer>(device,canopyLighting->Field(),
            190.0f*qualityProfile_.wildlifeMaximumDistanceScale,goatRenderer->CoatTexture(),goatRenderer->CoatMean());
        auto spiderRenderer = std::make_unique<SpiderRenderer>(device,canopyLighting->Field());
        auto spiderWebRenderer=std::make_unique<SpiderWebRenderer>(device,terrain_,jungleLayout_,spiderWebSites_);
        fishRenderer_ = std::make_unique<FishRenderer>(
            device, 215.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        stingrayRenderer_ = std::make_unique<StingrayRenderer>(
            device, 220.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        anemoneRenderer_ = std::make_unique<AnemoneRenderer>(device);
        reefHeronRenderer_ = std::make_unique<ReefHeronRenderer>(
            device, 170.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        octopusRenderer_=std::make_unique<OctopusRenderer>(device,180.0f*qualityProfile_.wildlifeMaximumDistanceScale);
        seaTurtleRenderer_ = std::make_unique<SeaTurtleRenderer>(
            device, 190.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        sharkRenderer_ = std::make_unique<SharkRenderer>(
            device, 710.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        dolphinRenderer_ = std::make_unique<DolphinRenderer>(
            device, 580.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        auto monkeyRenderer = std::make_unique<MonkeyRenderer>(
            device, terrain_, canopyLighting->Field(), 235.0f * qualityProfile_.wildlifeMaximumDistanceScale);
        auto residentRenderer=std::make_unique<ResidentRenderer>(device,canopyLighting->Field(),
            180.0f*qualityProfile_.wildlifeMaximumDistanceScale);
        auto bearRenderer=std::make_unique<BearRenderer>(device,canopyLighting->Field(),
            190.0f*qualityProfile_.wildlifeMaximumDistanceScale);
        auto grassRenderer = std::make_unique<GrassRenderer>(
            device, terrain_, grassLayout_, *canopyLighting, *terrainHeightMap, qualityProfile_.grassMaximumDistance);
        auto jungleRenderer = std::make_unique<JungleRenderer>(device, terrain_, jungleLayout_,
                                                              *canopyLighting, *terrainHeightMap,
                                                              qualityProfile_.vegetationMaximumDistanceScale,
                                                              slopeScrub_);
        if (canopyLodInspection_) {
            const auto camera = jungleRenderer->SetLodInspection(*canopyLodInspection_);
            if (viewpoint_ == FixedViewpoint::CanopyInspection) {
                cameraPosition_ = Vector3(camera.position[0], camera.position[1], camera.position[2]);
                const Vector3 direction = Vector3(camera.target[0], camera.target[1], camera.target[2]) - cameraPosition_;
                yaw_ = std::atan2(direction.X, direction.Z);
                pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
                std::cout << "island-simulator: elevated canopy inspection camera position="
                          << cameraPosition_.X << ',' << cameraPosition_.Y << ',' << cameraPosition_.Z
                          << " target=" << camera.target[0] << ',' << camera.target[1] << ',' << camera.target[2]
                          << " full-scene=1\n";
            }
        }
        terrainRenderer_ = std::move(terrainRenderer);
        playerMotion_.SetGroundPatches(&terrainRenderer_->StreamBed(),&terrainRenderer_->WreckScour());
        // the marks and shadows laid on the ground follow the channel and the scour too
        drawnGround_->SetLocalPatches({&terrainRenderer_->StreamBed(), &terrainRenderer_->WreckScour()});
        playerMotion_.SetCave(&terrainRenderer_->Cave(),&terrainRenderer_->CaveGround());
        playerMotion_.SetBunker(&bunkerLayout_);
        flightMotion_.SetCave(&terrainRenderer_->Cave(),&terrainRenderer_->CaveGround());
        caveRenderer_=std::make_unique<CaveRenderer>(device,terrainRenderer_->Cave(),
                                                      terrainRenderer_->CaveGround());
        bunkerRenderer_=std::make_unique<BunkerRenderer>(device,bunkerLayout_);
        campfireRenderer_=std::make_unique<CampfireRenderer>(device,campfireLayout_);
        streamWaterRenderer_=std::make_unique<StreamWaterRenderer>(device,terrain_,terrainRenderer_->StreamBed());
        grassRenderer_ = std::move(grassRenderer);
        jungleRenderer_ = std::move(jungleRenderer);
        boarRenderer_ = std::move(boarRenderer);
        snakeRenderer_ = std::move(snakeRenderer);
        goatRenderer_ = std::move(goatRenderer);
        deerRenderer_ = std::move(deerRenderer);
        spiderRenderer_ = std::move(spiderRenderer);
        spiderWebRenderer_=std::move(spiderWebRenderer);
        std::cout << "island-simulator: ground spiders " << spiders_->Spiders().size()
                  << " agents with retained stance anchors; canopy webs "
                  << spiderWebRenderer_->SiteCount()<<" qualified sites in one bounded translucent batch\n";
        std::cout << "island-simulator: a ground spider the walker stands over within four metres rears, its front legs raised in a threat display, until they step closer and it retreats or they leave\n";
        std::cout << "island-simulator: a ground spider at rest draws a front leg in and works it through its mouthparts every dozen seconds or so, the legs taking turns\n";
        std::cout << "island-simulator: goats " << goats_->Goats().size()
                  << " agents on shared-field dry open slopes, terrain-fitted limbs, directional fur material; "
                  << "a rest two seconds in, undisturbed, settles a goat onto folded legs for a longer lie, and the walker's approach has it up at once; "
                  << "every few seconds a goat flicks its short tail side to side, two beats and still again; "
                  << "and flicks an ear back at a fly now and then, the ears taking turns; "
                  << "a goat at rest, undisturbed, chews its cud, the jaw and beard swinging side to side\n";
        std::cout << "island-simulator: deer " << deer_->Deer().size()
                  << " agents graze and walk in two inland clearings and the stream valley and retreat non-aggressively; "
                  << "every few seconds a deer twitches one ear back and out at a fly, the ears taking turns; "
                  << "a deer standing alert at the walker stamps a forefoot now and then; "
                  << "a deer at rest, undisturbed, chews its cud, the muzzle working side to side; "
                  << "coming up from a graze a deer tosses its head\n";
        std::cout << "island-simulator: boars " << boarHerd_.Boars().size()
                  << " agents in moist non-channel interior habitat; a rest a second and a half in settles a boar onto folded legs, body on the ground, for a longer lie, and an alarm has it up within half a second\n";
        {
            // The walker's footstep ground, resolved at real places: the jetty's deck, the
            // beach at its root, the shallows off its end, the first rock and the first
            // inland ground on a walk from the hut out to the north-east.
            const auto name = [](const FootstepSurface surface) {
                switch (surface) {
                case FootstepSurface::Sand: return "sand";
                case FootstepSurface::Soil: return "soil";
                case FootstepSurface::Grass: return "grass";
                case FootstepSurface::Rock: return "rock";
                case FootstepSurface::Shallows: return "shallows";
                case FootstepSurface::Wood: return "wood";
                default: return "none";
                }
            };
            std::ostringstream probe;
            probe << "island-simulator: footstep ground probe:";
            if (jettyPlacement_) {
                const auto& j = *jettyPlacement_;
                const float deckX = j.rootX + j.dirX * 7.0f, deckZ = j.rootZ + j.dirZ * 7.0f;
                probe << " jetty deck " << name(FootstepGroundAt(deckX, deckZ))
                      << ", water beside its end " << name(FootstepGroundAt(j.rootX + j.dirX * 12.0f + j.AcrossX() * 3.0f,
                                                                             j.rootZ + j.dirZ * 12.0f + j.AcrossZ() * 3.0f));
            }
            {
                const auto& reach = streamDrift_.Course().Station(45);
                probe << ", the stream's middle reach " << name(FootstepGroundAt(reach.x, reach.z))
                      << ", beside it " << name(FootstepGroundAt(reach.x, reach.z - 3.0f * reach.halfWidth - 1.0f));
            }
            const char* rock = "none";
            const char* inland = "none";
            const char* sand = "none";
            for (float r = 0.0f; r < 220.0f && (std::string(rock) == "none" || std::string(inland) == "none" || std::string(sand) == "none"); r += 0.5f) {
                for (float a = 0.0f; a < 6.2831853f; a += 0.19634954f) {
                    const float x = std::cos(a) * r, z = std::sin(a) * r;
                    const FootstepSurface surface = FootstepGroundAt(x, z);
                    if (std::string(rock) == "none" && surface == FootstepSurface::Rock) rock = "rock";
                    if (std::string(sand) == "none" && surface == FootstepSurface::Sand) sand = "sand";
                    if (std::string(inland) == "none" && (surface == FootstepSurface::Grass || surface == FootstepSurface::Soil)) inland = name(surface);
                }
            }
            probe << ", nearest beach " << sand << ", nearest rock " << rock << ", nearest inland ground " << inland;
            std::cout << probe.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(probe.str());
        }
        std::cout << "island-simulator: deer, goats, boars, the bear and the resident share the land's fog and shrink away "
                     "over the last 12 % of their range instead of paling to the fog colour before they vanish; "
                     "so do spiders, crabs, hermit and coconut crabs, their burrows, crocodiles and flower patches, "
                     "and the small animals keep the land's fog\n";
        std::cout << "island-simulator: the rocks, logs, driftwood, beach details and the wreck receive the sun shadows "
                     "and the cumulus shade with the ground around them\n";
        std::cout << "island-simulator: the jetty, the rope bridge, the lookout and the helicopter receive them through one "
                     "shared structure material\n";
        std::cout << "island-simulator: ecological links: " << deer_->DrinkerCount() << " deer walk to the stream bank to drink and "
                  << goats_->DrinkerCount() << " goats to the rain-filled basin on their slope; "
                  << boarHerd_.WallowerCount() << " boars wallow at the bank and carry the mud on their legs and bellies; "
                  << "a boar stepping out of its wallow stands and shakes, the mud flying from its flanks; "
                  << "a boar rooting with its snout down works the snout side to side; "
                  << "boars root at the nearest trunk base, monkeys favour fruiting crowns and linger to pick\n";
        std::cout << "island-simulator: herd behaviour: goats share their separated ranges and the "
                     "East deer pair walks back together; a sentinel lifts its head first and the others follow after 0.7 s; "
                     "a coconut's thud startles the deer and goats within 30 m, who stand facing the fall for 6 s; "
                     "the deer keep seven metres clear of the campfire; "
                     "a deer whose rest comes on sunlit ground walks to the nearest shade within ten metres and lies up there\n";
        std::cout << "island-simulator: goats square up to a neighbour for a bout of head-to-head shoving, standing nose to nose and taking turns lowering their heads\n";
        std::cout << "island-simulator: a deer that lies up in the shade beds down onto folded legs and is on its feet within half a second of an alarm\n";
        std::cout << "island-simulator: the goat sentinel coming alert at the walker gives a loud alarm bleat ahead of the herd's other calls\n";
        std::cout << "island-simulator: a boar the bear sends off gives a loud alarm grunt ahead of the herd's other calls\n";
        std::cout << "island-simulator: snakes " << snakes_->Snakes().size()
                  << " ground agents with separated dense-understory-margin homes; "
                  << snakes_->TreeSnakes().size()<<" arboreal agents on sampled broadleaf trunks\n";
        std::cout << "island-simulator: snakes flick a forked tongue every few seconds, a half-second taste of the air at rest and on the move\n";
        std::cout << "island-simulator: a ground snake lifts its head to look at the walker standing off within fourteen metres, and keeps it low to slip away closer\n";
        monkeyRenderer_ = std::move(monkeyRenderer);
        residentRenderer_=std::move(residentRenderer);
        bearRenderer_=std::move(bearRenderer);
        canopyLighting_ = std::move(canopyLighting);
        terrainHeightMap_ = std::move(terrainHeightMap);
        std::vector<AnimalTrackPlacement> residentFootprints;
        if (jettyPlacement_)
            for (const auto& line : jettyPlacement_->FootprintLines(terrain_, RenderedTerrainSurface(terrain_)))
                residentFootprints.push_back(line);
        turtleTrackRenderer_ = std::make_unique<TurtleTrackRenderer>(device, naturalPropLayout_.BeachDetails(),
                                                                     RenderedTerrainSurface(terrain_));
        naturalPropRenderer_ = std::make_unique<NaturalPropRenderer>(device, naturalPropLayout_, residentFootprints,
                                                                     &canopyLightField_);
        shipwreckRenderer_ = std::make_unique<ShipwreckRenderer>(device, shipwreckLayout_);
        if(hutPlacement_) hutRenderer_=std::make_unique<HutRenderer>(device,*hutPlacement_);
        if(hutPlacement_) eaveDrips_=std::make_unique<EaveDripRenderer>(device,*hutPlacement_,terrain_);
        if(jettyPlacement_) jettyRenderer_=std::make_unique<JettyRenderer>(device,*jettyPlacement_);
        if(bridgePlacement_) bridgeRenderer_=std::make_unique<BridgeRenderer>(device,*bridgePlacement_);
        if(lookoutPlacement_) lookoutRenderer_=std::make_unique<LookoutRenderer>(device,*lookoutPlacement_);
        fallingFruitRenderer_=std::make_unique<FallingFruitRenderer>(device);
        streamDriftRenderer_=std::make_unique<StreamDriftRenderer>(device);
        streamFishRenderer_=std::make_unique<StreamFishRenderer>(device);
        hatchlingRenderer_=std::make_unique<HatchlingRenderer>(device);
        birdShadowRenderer_=std::make_unique<BirdShadowRenderer>(device,terrain_,*drawnGround_);
        seabedShadowRenderer_=std::make_unique<SeabedShadowRenderer>(device,terrain_,*drawnGround_);
        cascadeMistRenderer_=std::make_unique<CascadeMistRenderer>(device);
        // a small stream's step throws a thin spray, not a fog: ten overlapping puffs at full
        // density smeared the bank beside the pool into a pale haze
        cascadeMistRenderer_->SetRange(40.0f,20.0f,0.45f);
        wetlandMistRenderer_=std::make_unique<CascadeMistRenderer>(device,
            "island-simulator: a low mist hangs over the seasonal wetland, breathing slowly, drifting with the trade wind and leaning in the gust fronts");
        fallingFruit_.SetPalms(vegetationLayout_.Palms());
        coconutCrabGroup_.SetPalms(vegetationLayout_.Palms());
        helicopterRenderer_=std::make_unique<HelicopterRenderer>(device,*helicopterPlacement_);
        {
            // the fallen palms' crowns are laid on the drawn ground
            const RenderedTerrainSurface palmGround(terrain_);
            palmRenderer_ = std::make_unique<PalmRenderer>(
                device, vegetationLayout_, qualityProfile_.vegetationMaximumDistanceScale, &palmGround);
        }
        try {
            if (!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects)
                || !device.SupportsCapability(CNA::GraphicsCapability::Instancing))
                throw std::runtime_error("custom effects or instancing unavailable");
            if (SunShadowMap::QaDisabled())
                throw std::runtime_error("switched off for a QA comparison (--no-sun-shadows)");
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
            constexpr int sunShadowResolution = 1024;
#else
            constexpr int sunShadowResolution = 2048;
#endif
            sunShadowMap_ = std::make_unique<SunShadowMap>(device, sunShadowResolution);
            contactShadowRenderer_->SetSunShadowMapActive(true);
            std::cout << "island-simulator: sun shadow map " << sunShadowResolution
                      << " px covers " << static_cast<int>(2.0f * SunShadowMap::Radius)
                      << " m around the camera; trees, palms, snags, bushes, hut, wreck and "
                         "helicopter cast directional shadows onto terrain, grass, plants and palms; "
                         "they fade out " << static_cast<int>(sun_shadow_fade::FarFadeStart) << "-"
                      << static_cast<int>(sun_shadow_fade::FarFadeEnd)
                      << " m from the viewer in the sun's view, moving with the camera rather than "
                         "with the map's 8 m re-centring; beyond that the wreck, hut and helicopter keep "
                         "their projected mass shadows, cross-faded vertex by vertex with the map's\n";
            MirrorRuntimeDiagnosticToPlatformLog(
                "island-simulator: sun shadow map casts directional tree and structure shadows");
        } catch (const std::exception& error) {
            sunShadowMap_ = nullptr;
            std::cout << "island-simulator: sun shadow map unavailable: " << error.what()
                      << "; projected footprint shadows retained\n";
        }
        oceanRenderer_ = std::make_unique<OceanRenderer>(
            device, terrain_, qualityProfile_.oceanQuadsPerAxis);
        shorelineRenderer_ = std::make_unique<ShorelineRenderer>(device, terrain_);
        skyRenderer_ = std::make_unique<SkyRenderer>(device);
        debugOverlay_ = std::make_unique<DebugOverlay>(device);
        touchOverlay_ = std::make_unique<TouchOverlay>(device);
        waterViewOverlay_ = std::make_unique<WaterViewOverlay>(device);
        underwaterEffect_ = std::make_unique<UnderwaterEffectRenderer>(device);
        timeOfDay_ = std::make_unique<TimeOfDayRenderer>(device);
        rain_ = std::make_unique<RainRenderer>(device);
        // no rain under the hut's thatch (its leaf tips reach about 2.73 m out and 2.3 m along
        // the ridge, 3.6 m above the floor at the ridge and 2.1 m at the eaves)
        if (hutPlacement_) rain_->SetShelter(hutPlacement_->x, hutPlacement_->z, hutPlacement_->yawRadians, 2.73f, 2.32f,
                                             hutPlacement_->floorY + 2.08f, hutPlacement_->floorY + 3.62f);
        rainRings_ = std::make_unique<RainRingRenderer>(device, terrain_);
        rainSplashes_ = std::make_unique<RainSplashRenderer>(device, terrain_);
        canopyDrips_ = std::make_unique<CanopyDripRenderer>(device, terrain_, canopyLightField_);
        fireflies_ = std::make_unique<FireflySwarm>(terrain_);
        fireflyRenderer_ = std::make_unique<FireflyRenderer>(device);
        morningMist_ = std::make_unique<MorningMistField>(terrain_);
        whaleRenderer_ = std::make_unique<WhaleRenderer>(device);
        sailboatRenderer_ = std::make_unique<SailboatRenderer>(device);
        waterspoutRenderer_ = std::make_unique<WaterspoutRenderer>(device);
        seagrassRenderer_ = std::make_unique<SeagrassRenderer>(device, terrain_);
        swimGlowRenderer_ = std::make_unique<SwimGlowRenderer>(device);
        eyeshineRenderer_ = std::make_unique<EyeshineRenderer>(device);
        sunbeamMotes_ = std::make_unique<SunbeamMoteRenderer>(device, terrain_, canopyLightField_);
        whaleBlowRenderer_ = std::make_unique<CascadeMistRenderer>(device,
            "island-simulator: a humpback's blow stands up five metres far offshore, drifts downwind and thins away", WhalePod::PuffCount);
        whaleBlowRenderer_->SetRange(700.0f, 200.0f, 2.2f);
        mistBankRenderer_ = std::make_unique<MistBankRenderer>(device, MorningMistField::MaximumSites);
        summitCloudRenderer_ = std::make_unique<MistBankRenderer>(device, SummitCloudField::Count, 1000.0f, 0.6f, 215.0f,
            "island-simulator: a cloud cap forms on the summit in moist air, hugging the top and dissolving in its lee");
        {
            // the cap sits on the island's highest ground
            float bestX = 0.0f, bestZ = 0.0f, best = -1.0e9f;
            for (float z = -250.0f; z <= 250.0f; z += 2.0f)
                for (float x = -250.0f; x <= 250.0f; x += 2.0f) {
                    const float h = terrain_.Height(x, z);
                    if (h > best) { best = h; bestX = x; bestZ = z; }
                }
            summitCloud_.SetSummit(bestX, best, bestZ);
        }
        std::cout << "island-simulator: morning mist: " << morningMist_->SiteCount() << " sites on the low damp ground\n";
        try {
            localLights_ = std::make_unique<LocalLightRenderer>(device);
            std::cout << "island-simulator: local lights at night (the campfire, the quad's headlights) lift the night grade "
                         "where they reach the ground, keeping its colours\n";
        } catch (const std::exception& error) {
            std::cout << "island-simulator: local lights unavailable: " << error.what() << '\n';
        }
        try {
            quadRenderer_ = std::make_unique<QuadBikeRenderer>(device);
            quadRider_ = std::make_unique<QuadRider>(*this);
            exhaustRenderer_ = std::make_unique<RootingDustRenderer>(device,
                "island-simulator: the quad's exhaust puffs thicker with the throttle, carried with its speed and drifting downwind",
                112, 112, 118, ExhaustSmokeField::Capacity);
            quadSurface_ = std::make_unique<RenderedTerrainSurface>(terrain_);
            if (terrainRenderer_)
                quadSurface_->SetLocalPatches({&terrainRenderer_->StreamBed(), &terrainRenderer_->WreckScour()});
            tyreTrackRenderer_ = std::make_unique<TyreTrackRenderer>(device, terrain_, *quadSurface_);
            wheelDustRenderer_ = std::make_unique<RootingDustRenderer>(device,
                "island-simulator: the quad's wheels throw dust off dry sand and soil, more with the speed, settling downwind",
                214, 200, 172, WheelThrowField::Capacity);
            wheelSprayRenderer_ = std::make_unique<RootingDustRenderer>(device,
                "island-simulator: driven through the shallows the quad's wheels fling spray up and back",
                226, 234, 238, WheelThrowField::Capacity);
            // Parked in a clear, level spot beside the hut, facing away from it.
            if (hutPlacement_) {
                const auto& h = *hutPlacement_;
                bool placed = false;
                for (float radius = 7.0f; radius < 30.0f && !placed; radius += 1.5f) {
                    for (float angle = 0.0f; angle < 6.2831853f && !placed; angle += 0.26f) {
                        const float x = h.x + std::cos(angle) * radius, z = h.z + std::sin(angle) * radius;
                        const auto sample = terrain_.Sample(x, z);
                        if (sample.height < 0.8f || sample.slopeRadians > 0.14f) continue;
                        if (worldCollisionGrid_.IntersectsCircle(x, z, 1.6f)) continue;
                        bool yardClear = true;
                        for (const auto& prop : h.yard) yardClear = yardClear && std::hypot(prop.x - x, prop.z - z) > 2.4f;
                        if (!yardClear) continue;
                        PlaceQuadNear(x, z, std::atan2(x - h.x, z - h.z));
                        placed = true;
                    }
                }
            }
            if (!quadPlaced_) PlaceQuadNear(cameraPosition_.X + 3.0f, cameraPosition_.Z, 0.0f);
            if (QuadDriveView()) {
                // Driven: set off up the beach path at a run before the capture (on a fixed
                // clock), the chase camera following, to show its ruts and its dust; or in from
                // six metres before the bunker's portal or the cave's mouth, on their floors.
                if (viewpoint_ == FixedViewpoint::QuadBunkerDetail) {
                    const auto from = bunkerLayout_.World(0.0f, 0.0f, -6.0f), to = bunkerLayout_.World(0.0f, 0.0f, 0.0f);
                    PlaceQuadNear(from.x, from.z, std::atan2(to.x - from.x, to.z - from.z));
                } else if (viewpoint_ == FixedViewpoint::QuadCaveDetail) {
                    const auto from = terrainRenderer_->Cave().World(0.0f, 0.0f, -6.0f);
                    const auto to = terrainRenderer_->Cave().World(0.0f, 0.0f, 0.0f);
                    PlaceQuadNear(from.x, from.z, std::atan2(to.x - from.x, to.z - from.z));
                } else if (viewpoint_ == FixedViewpoint::QuadJettyDetail && jettyPlacement_) {
                    // from the deck's ramp out along it (the jetty's root lies under a steep bank)
                    const auto& jetty = *jettyPlacement_;
                    PlaceQuadNear(jetty.rootX + jetty.dirX * 1.0f, jetty.rootZ + jetty.dirZ * 1.0f,
                                  std::atan2(jetty.dirX, jetty.dirZ));
                } else if (viewpoint_ == FixedViewpoint::QuadHelicopterDetail && helicopterPlacement_) {
                    // straight at the helicopter's flank from ten metres off
                    const auto side = helicopterPlacement_->World(10.0f, 0.0f, 0.0f);
                    PlaceQuadNear(side.x, side.z, std::atan2(helicopterPlacement_->x - side.x, helicopterPlacement_->z - side.z));
                } else if (viewpoint_ == FixedViewpoint::QuadLookoutDetail && lookoutPlacement_) {
                    // at the lookout's ramp from its side, four and a half metres off, near its foot
                    const auto& lookout = *lookoutPlacement_;
                    const float u = LookoutPlacement::Half + LookoutPlacement::RampLength * 0.8f;
                    const float rampX = lookout.x + lookout.dirX * u, rampZ = lookout.z + lookout.dirZ * u;
                    const float fromX = rampX + lookout.AcrossX() * 4.5f, fromZ = rampZ + lookout.AcrossZ() * 4.5f;
                    PlaceQuadNear(fromX, fromZ, std::atan2(rampX - fromX, rampZ - fromZ));
                } else if (viewpoint_ == FixedViewpoint::QuadStreamDetail) {
                    // across the stream's middle reach, square to it from nine metres off its
                    // south bank
                    const StreamCourse& course = streamDrift_.Course();
                    const auto& a = course.Station(44);
                    const auto& b = course.Station(46);
                    const auto& centre = course.Station(45);
                    const float length = std::hypot(b.x - a.x, b.z - a.z);
                    float acrossX = -(b.z - a.z) / length, acrossZ = (b.x - a.x) / length;
                    if (acrossZ > 0.0f) { acrossX = -acrossX; acrossZ = -acrossZ; }
                    const float fromX = centre.x + acrossX * 9.0f, fromZ = centre.z + acrossZ * 9.0f;
                    PlaceQuadNear(fromX, fromZ, std::atan2(centre.x - fromX, centre.z - fromZ));
                    std::cout << "island-simulator: quad QA stream start: " << std::hypot(quad_.Position().x - centre.x,
                        quad_.Position().z - centre.z) << " m from the centreline\n";
                } else if (viewpoint_ == FixedViewpoint::QuadSeaDetail) {
                    // set down in water over its seat off the beach path's foot (it can not wade there: the water
                    // holds it to a crawl past some 0.7 m), its airbox flooded
                    constexpr float startZ = -266.0f;
                    const float x0 = VegetationLayout::PrimaryPathCenterX(startZ);
                    PlaceQuadNear(x0, startZ, std::atan2(0.0f, -1.0f));
                } else if (viewpoint_ == FixedViewpoint::QuadBearDetail && bear_) {
                    // straight at the bear from eight metres off
                    const auto& bear = bear_->State();
                    const float fromX = bear.x + 8.0f, fromZ = bear.z;
                    PlaceQuadNear(fromX, fromZ, std::atan2(bear.x - fromX, bear.z - fromZ));
                } else {
                    // up the beach path from the south shore, which runs clear of the trees
                    constexpr float startZ = -222.0f;
                    const float x0 = VegetationLayout::PrimaryPathCenterX(startZ), x1 = VegetationLayout::PrimaryPathCenterX(startZ + 6.0f);
                    PlaceQuadNear(x0, startZ, std::atan2(x1 - x0, 6.0f));
                }
                riding_ = true;
                quadControls_ = {viewpoint_ == FixedViewpoint::QuadDriveDetail ? 0.8f
                    : viewpoint_ == FixedViewpoint::QuadJettyDetail ? 0.4f
                    : 1.0f, 0.0f, false};
                quadCameraYaw_ = quad_.YawRadians();
                const auto start = quad_.Position();
                quadCamera_ = Vector3(start.x - std::sin(quadCameraYaw_) * 4.8f, start.y + 1.9f, start.z - std::cos(quadCameraYaw_) * 4.8f);
                std::cout << "island-simulator: quad QA drive sets off with the chase camera\n";
            } else if (viewpoint_ == FixedViewpoint::QuadChaseDetail) {
                // Ridden, from the rider's chase camera: behind and above, over the rider's
                // shoulders to the ground the lamps light ahead.
                riding_ = true;
                quadControls_ = {0.0f, 0.0f, true};
                const auto q = quad_.Position();
                const float yaw = quad_.YawRadians();
                const float cameraX = q.x - std::sin(yaw) * 4.8f, cameraZ = q.z - std::cos(yaw) * 4.8f;
                cameraPosition_ = Vector3(cameraX, std::max(q.y + 1.9f, terrain_.Height(cameraX, cameraZ) + 0.6f), cameraZ);
                const Vector3 direction = Vector3(q.x, q.y + 0.7f, q.z) - cameraPosition_;
                yaw_ = std::atan2(direction.X, direction.Z);
                pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
                std::cout << "island-simulator: quad QA camera rides behind the quad\n";
            } else if (viewpoint_ == FixedViewpoint::QuadDetail || viewpoint_ == FixedViewpoint::QuadRideDetail) {
                // The parked quad from three quarters in front on its left; ridden, from in
                // front on its right with the rider on the seat and the handbrake on.
                const bool ride = viewpoint_ == FixedViewpoint::QuadRideDetail;
                riding_ = ride;
                quadControls_ = {0.0f, ride ? 0.7f : 0.0f, true};   // ridden: the bars turned to the right
                const auto q = quad_.Position();
                const BikeVec f = quad_.Forward(), r = quad_.Right();
                const float along = ride ? 3.0f : 2.6f, across = ride ? 2.4f : -2.9f;
                const float cameraX = q.x + f.x * along + r.x * across, cameraZ = q.z + f.z * along + r.z * across;
                cameraPosition_ = Vector3(cameraX, std::max(terrain_.Height(cameraX, cameraZ), 0.0f) + (ride ? 1.45f : 1.55f), cameraZ);
                const Vector3 target(q.x, q.y + (ride ? 0.75f : 0.25f), q.z);
                const Vector3 direction = target - cameraPosition_;
                yaw_ = std::atan2(direction.X, direction.Z);
                pitch_ = std::atan2(direction.Y, std::hypot(direction.X, direction.Z));
                std::cout << "island-simulator: quad QA camera looks at the " << (ride ? "ridden" : "parked") << " quad bike\n";
            }
            const auto p = quad_.Position();
            std::cout << "island-simulator: quad bike parked at (" << static_cast<int>(p.x) << ", " << static_cast<int>(p.z)
                      << "): F to ride it or step off, W/S throttle and brake, A/D steer, Space the handbrake, Backspace "
                         "sets it back on its wheels; top speed " << static_cast<int>(QuadBike::TopSpeed * 3.6f + 0.5f)
                      << " km/h, headlights by day and night\n";
        } catch (const std::exception& error) {
            quadRenderer_.reset();
            std::cout << "island-simulator: quad bike unavailable: " << error.what() << '\n';
        }
        {
            // The clock: 24 game hours in 24 real minutes from 14:00 (the former fixed sun's
            // hour), or from --time-of-day; held still for the fixed QA views and smoke runs so
            // their frames stay comparable.
            const float requested = day_night_options::RequestedHour();
            const float requestedDay = day_night_options::RequestedDay();
            if (requestedDay >= 0.0f)
                dayNight_ = DayNightCycle(requested >= 0.0f ? requested : DayNightCycle::DefaultHour, requestedDay);
            else if (requested >= 0.0f) dayNight_.SetHour(requested);
            dayNight_.SetFrozen(viewpoint_ != FixedViewpoint::None || smokeTest_);
            // The weather: the fixed QA views keep the former partly cloudy sky unless one is
            // asked for; play starts partly cloudy and lets the spells run.
            if (const auto requestedWeather = weather_now::Requested()) weather_.Force(*requestedWeather);
            else if (dayNight_.Frozen()) weather_.Force(WeatherKind::PartlyCloudy);
            weather_now::Set(weather_.State());
            {
                auto light = dayNight_.Lighting();
                WeatherSystem::ApplyTo(light, weather_.State());
                weather_now::AddMoonShadows(light);
                sun_light::Set(light);
            }
            std::cout << "island-simulator: weather: spells of 1.5-5 game hours of clear, partly cloudy, overcast, showers "
                         "and thunderstorms (no snow on a tropical island); now " << WeatherName(weather_.State().kind)
                      << (weather_.Forced() ? ", held" : "") << "; press Y for the next weather\n";
            const auto shading = sun_light::Current().shading;
            wildlife_render::FixedSunDirection = Vector3(shading.x, shading.y, shading.z);
            const auto clockText = [](const float hour) {
                const int minutes = static_cast<int>(std::lround(hour * 60.0f)) % (24 * 60);
                char text[16];
                std::snprintf(text, sizeof text, "%02d:%02d", minutes / 60, minutes % 60);
                return std::string(text);
            };
            std::cout << "island-simulator: day and night: 24 game hours pass in "
                      << static_cast<int>(DayNightCycle::SecondsPerDay / 60.0f) << " real minutes from "
                      << clockText(dayNight_.Hour()) << "; the sun rises in the east at 06:00 and sets at 18:00 "
                         "over a short tropical dusk, the moon turns through its phases every "
                      << static_cast<int>(DayNightCycle::LunarDays) << " days; press T to move the clock an hour on"
                      << (dayNight_.Frozen() ? "; held still for this QA view" : "") << '\n';
        }
        std::cout << "island-simulator: screen-space canopy sun shafts are off; the light under the crowns comes from the shade field and the drifting leaf-gap flecks\n";
        footprintRenderer_ = std::make_unique<FootprintTrailRenderer>(device, terrain_, *drawnGround_);
        if (goats_->WaterholeSite().valid)
            waterholeRenderer_ = std::make_unique<WaterholeRenderer>(device, terrain_,
                goats_->WaterholeSite().x, goats_->WaterholeSite().z, GoatHerd::WaterholeRadius);
        rootingDustRenderer_ = std::make_unique<RootingDustRenderer>(device);
        shellDragRenderer_ = std::make_unique<ShellDragRenderer>(device, terrain_, *drawnGround_);
        {
            const std::vector<BoarHerd::WallowSite> wallowSites = boarHerd_.WallowSites();
            wallowScarRenderer_ = std::make_unique<WallowScarRenderer>(device, terrain_, wallowSites);
        }
        forestTwigRenderer_ = std::make_unique<ForestTwigRenderer>(device, terrain_, forestTwigs_.Twigs());
        // Every field drawn through a shared puff renderer must fit that renderer's buffer; an
        // overflow is the fatal "data is not the correct size for this VertexBuffer".
        static_assert(WetlandMistField::Capacity <= CascadeMistField::Capacity
                      && DolphinBlowField::Capacity <= CascadeMistField::Capacity
                      && DragonflyDipField::Capacity <= CascadeMistField::Capacity
                      && CrabBubbleField::Capacity <= CascadeMistField::Capacity
                      && TurtleBreathField::Capacity <= CascadeMistField::Capacity,
                      "a mist field outgrew the shared mist renderer");
        static_assert(MudFleckField::Capacity <= RootingDustField::Capacity
                      && HoofDustField::Capacity <= RootingDustField::Capacity
                      && StingraySandField::Capacity <= RootingDustField::Capacity
                      && TurtleGrazeField::Capacity <= RootingDustField::Capacity
                      && FinWakeField::Capacity <= RootingDustField::Capacity
                      && CrocodileWakeField::Capacity <= RootingDustField::Capacity
                      && OctopusInkField::Capacity <= RootingDustField::Capacity,
                      "a puff field outgrew the shared dust renderer");
        surfSprayRenderer_ = std::make_unique<CascadeMistRenderer>(device,
            "island-simulator: breakers throw spray up the seaward faces of the offshore rocks in time with their foam",
            SurfSprayField::Capacity);
        dolphinBlowRenderer_ = std::make_unique<CascadeMistRenderer>(device,
            "island-simulator: a dolphin breaking the surface blows a puff of spray from the blowhole");
        stingraySandRenderer_ = std::make_unique<RootingDustRenderer>(device,
            "island-simulator: a settled stingray bursts out of the sand in a puff when the swimmer comes near", 196, 178, 140);
        // Blown sand holds up to BlownSandField::Capacity puffs, more than the dust default.
        blownSandRenderer_ = std::make_unique<RootingDustRenderer>(device,
            "island-simulator: a gust front passing the windward beaches streams pale sand off the dry crests downwind, a hand's height and gone within two seconds", 221, 208, 172,BlownSandField::Capacity);
        finWakeRenderer_ = std::make_unique<RootingDustRenderer>(device,
            "island-simulator: the shark's dorsal fin riding just under the surface cuts a foam wake that spreads and fades on the water behind it within three seconds", 236, 244, 248);
        std::cout << "island-simulator: blown sand: " << blownSand_.Sources().size()
                  << " exposed dry-sand sources around the shore blow while a gust front passes them\n";
        octopusInkRenderer_ = std::make_unique<RootingDustRenderer>(device,
            "island-simulator: an octopus startled from the open jets for its den behind a cloud of ink", 22, 18, 34);
        turtleBreathRenderer_ = std::make_unique<CascadeMistRenderer>(device,
            "island-simulator: a sea turtle lifting its head through the surface breathes out a small puff of spray");
        worldDebugRenderer_ = std::make_unique<WorldDebugRenderer>(device, terrain_);
        if (viewpoint_ == FixedViewpoint::GroundContactDetail) {
            constexpr float contactZ = 45.0f;
            playerMotion_.SetPosition(
                terrain_, VegetationLayout::PrimaryPathCenterX(contactZ), contactZ);
        } else if (viewpoint_ == FixedViewpoint::MovementTraceDetail
                   && !vegetationLayout_.Obstacles().empty()) {
            const CircularObstacle& obstacle = vegetationLayout_.Obstacles().front();
            playerMotion_.SetPosition(terrain_, obstacle.x - 1.8f, obstacle.z);
            for (int step = 0; step < 90; ++step) {
                playerMotion_.Update(terrain_, 1.0f, 0.0f, 1.0f / 60.0f,
                                     worldCollisionGrid_);
            }
        } else {
            playerMotion_.SetPosition(terrain_, cameraPosition_.X, cameraPosition_.Z);
        }
        const bool waterViewAllowed = viewpoint_ == FixedViewpoint::None
            || viewpoint_ == FixedViewpoint::WaterlineDetail
            || viewpoint_ == FixedViewpoint::CurrentBoundaryDetail
            || viewpoint_ == FixedViewpoint::ReefHabitatDetail || viewpoint_ == FixedViewpoint::AnemoneDetail
            || viewpoint_ == FixedViewpoint::BenchmarkTraversal;
        waterViewModel_.Reset(waterViewAllowed
                                  && playerMotion_.Mode() == LocomotionMode::Swimming,
                              playerMotion_.GroundHeight(),
                              playerMotion_.OceanCurrentStrength(terrain_));
        const float initialRippleSpeed = viewpoint_ == FixedViewpoint::WaterlineDetail
            ? 1.35f
            : std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ());
        waterRippleModel_.Reset(
            playerMotion_.Mode() == LocomotionMode::Swimming,
            playerMotion_.GroundHeight(), initialRippleSpeed);
        if (viewpoint_ == FixedViewpoint::WaterlineDetail) {
            waterRippleModel_.TriggerSplash(true, initialRippleSpeed);
        }
        const float initialGrassSpeed = viewpoint_ == FixedViewpoint::GrassDetail
            ? 3.2f
            : std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ());
        grassInteractionModel_.Reset(initialGrassSpeed);
        if (!smokeTest_ && viewpoint_ == FixedViewpoint::None) {
            const PlayerPosition& position = playerMotion_.Position();
            cameraPosition_ = Vector3(position.x, position.y, position.z);
        }
        soundscape_ = std::make_unique<Soundscape>();
        const ShipwreckPlacement& shipwreck = shipwreckLayout_.Ship();
        soundscape_->SetShipwreckSource(shipwreck.x, shipwreck.y + 0.35f, shipwreck.z);
        const StreamCourse streamCourse(terrain_);
        soundscape_->SetStreamCascade(streamCourse.CascadeX(), streamCourse.CascadeZ());
        soundscape_->SetBunkerSource(bunkerLayout_.EntranceGroundY());
        if (userSettings_.audioMuted) soundscape_->SetMuted(true);
        if (!smokeTest_ && !soundscapeNeedsGesture_) {
            TryStartSoundscape();
        }
        contentFinished_ = std::chrono::steady_clock::now();
        contentCreationMilliseconds_ = std::chrono::duration<double, std::milli>(
            contentFinished_ - contentStarted).count();

        std::string rendererDiagnostic = "island-simulator: renderer ";
        rendererDiagnostic.append(device.GetGraphicsRendererName());
        std::cout << rendererDiagnostic << '\n'
                  << "island-simulator: terrain " << terrainRenderer_->VertexCount()
                  << " vertices, " << terrainRenderer_->PrimitiveCount() << " triangles in "
                  << terrainRenderer_->TotalCellCount()
                  << " cull cells with shared buffers\n"
                  << "island-simulator: authoritative coast uses sub-metre berms, dunes, wash channels, rock steps, and tide-pool basins\n"
                  << "island-simulator: collision grid "
                  << worldCollisionGrid_.Obstacles().size() << " static obstacles in "
                  << worldCollisionGrid_.CellCount() << " occupied cells\n"
                  << "island-simulator: birds " << birdFlock_.Birds().size()
                  << " agents in a distance-throttled one-batch simulation\n"
                  << "island-simulator: birds use " << birdFlock_.PerchSites().size()
                  << " real branch, rock, and wreck perches with approach and departure; "
                     "a perched bird flushes when the walker comes within 9 m or a coconut thuds within 30 m\n"
                  << "island-simulator: bird flight uses slow radial drift, banking, and "
                     "intermittent glide\n"
                  << "island-simulator: seabirds break from their high orbits to circle low over the fish school of the moment\n"
                  << "island-simulator: circling seabirds plunge-dive every nine seconds and each strike raises a foam ring on the water and a positional splash\n"
                  << "island-simulator: a seabird done hunting settles onto the sea and rafts there with folded wings, head to wind, before climbing back to its route; lifting off, it shakes the water from its feathers\n"
                  << "island-simulator: a seabird's strike brings the others on their routes within a hundred and twenty metres in to hunt the same school\n"
                  << "island-simulator: a dolphin's leap brings the seabirds on their routes within a hundred and sixty metres in to hunt the school of the moment\n"
                  << "island-simulator: a perched bird flushes from the resident or an animal walking up under its perch as from the walker\n"
                  << "island-simulator: a perched bird preens between its looks around, the head turned back into one wing for a few seconds every eleven seconds or so\n"
                  << "island-simulator: a perched bird stretches one wing out and down for a second or two every seventeen seconds or so, between its preens\n"
                  << "island-simulator: a perched bird shakes its feathers out as each preen ends, the body puffed for a moment and smoothed again\n"
                  << "island-simulator: a perched bird scratches its head with a foot now and then, the foot brought up past the wing, the feet taking turns\n"
                  << "island-simulator: the deer and the boars press cloven prints into wet sand and the stream's mud as they walk, two toes a stride apart that the ground takes back over four minutes\n"
                  << "island-simulator: a bird flushed by a snake mobs it, circling low over its head for eight seconds and chattering the alarm, an original run of sharp chirps heard within twenty-five metres\n"
                  << "island-simulator: a snake's head within three and a half metres sends a mouse or rat bolting and to ground for six seconds or more\n"
                  << "island-simulator: a body standing in a snag's, a plant's or a structure's sun shadow is lit as the ground there is\n"
                  << "island-simulator: a fish school passing the jetty turns in and holds off the end of the deck, in its shade, a while before going on\n"
                  << "island-simulator: a school parts around the shark passing through it at seven metres, as around the swimmer at three\n"
                  << "island-simulator: a falling coconut lands with a positional thud heard within thirty metres\n"
                  << "island-simulator: a fish breaking the surface makes a small plip heard within twenty-five metres\n"
                  << "island-simulator: a crocodile sliding off the bank goes under with a heavy splash and a ring on the water\n"
                  << "island-simulator: passing gust fronts swell the breeze and canopy beds at the listener\n"
                  << "island-simulator: the cumulus shadows drift over the land as well as the sea, dimming every sun-shadow receiver together\n"
                  << "island-simulator: a fallen fruit bounces off where it strikes and rolls downhill to rest\n"
                  << "island-simulator: gust fronts sweep the grass, crowns and fronds downwind together every twenty-two seconds within their authored sway\n"
                  << "island-simulator: four fruit doves alternate canopy perches with bounded dry-ground walking and foraging; "
                     "a dove's ground visit crosses to the nearest fruit lying on dove-grade ground when there is any; "
                     "a dove on the ground flushes from a passing animal or the resident\n"
                  << "island-simulator: fruit doves coo in low slow phrases and thrust their heads as they walk; parrots keep their squawk and a pigeon-grey dove joins the green ones\n"
                  << "island-simulator: every fourth ground visit a fruit dove comes down to the plunge pool's bank to bathe, flicking its wings and dipping in bouts\n"
                  << "island-simulator: a parrot perched on a fruiting lobe of the crown feeds there in bouts, reaching to the fruit, and stays twice as long\n"
                  << "island-simulator: at a gust front the crowns let go leaves that spiral down through the shade, drift downwind and lie on the floor a while\n"
                  << "island-simulator: a bird settling on a branch and a monkey moving through a crown shake a leaf loose\n"
                  << "island-simulator: a monkey the walker closes on gives an alarm call as it breaks into flight, the troop's second phrase pitched up and heard within forty metres\n"
                  << "island-simulator: a snake's head within four metres of a monkey sends it off with the alarm as the walker does\n"
                  << "island-simulator: a snake's head come within three metres of a perched bird flushes it as the walker does\n"
                  << "island-simulator: fruit bats " << fruitBatColony_.Bats().size()
                  << " agents use six inland canopy roosts with four hanging and two bounded daytime flights; "
                     "a roosting bat's stir in its crown lets a fruit drop and a leaf come loose; "
                     "a hanging bat fans its half-opened wings for a few seconds every quarter minute to cool; "
                     "and stretches one wing out and down for a couple of seconds now and then, the wings taking turns\n"
                  << "island-simulator: clearing butterflies "
                  << butterflyMeadow_.Butterflies().size()
                  << " agents in deterministic low-altitude habitats; "
                  << butterflyMeadow_.FlowerBushCount()
                  << " flowering bushes within reach of the habitats draw alternate nectar excursions at blossom height; "
                     "fruit that has lain fermenting for 20 s draws them down to feed low over it; "
                     "a butterfly settled into its hover at a blossom, the patch or fruit uncurls its proboscis to feed and rolls it up again as it leaves; "
                     "they scatter from a passing animal or the resident as from the walker\n"
                  << "island-simulator: geckos " << geckoPopulation_.Geckos().size()
                  << " agents in deterministic inland rest, flee, and hide habitats; a resting gecko with a fly hanging low within reach stalks it and lunges, and the flies dart from the snap; "
                  << "a stalking gecko wags its tail, the tip swaying wide and fast until the lunge; "
                  << "a resting gecko licks an eye clean with its tongue every ten seconds or so, the eyes taking turns; "
                  << "and licks its lips as a hunt ends\n"
                  << "island-simulator: skinks " << skinkPopulation_.Skinks().size()
                  << " agents in deterministic open-ground bask, forage, and flee habitats; a basking skink a cloud's shadow covers gets up and forages, and lies down to bask only in the sun; "
                  << "a basking skink in the sun displays with a bout of push-ups, the front body lifted on straightened forelegs, a second and a half into the bask and every eight seconds or so after; "
                  << "a foraging skink flicks its tongue at the ground ahead every few seconds, tasting its way; "
                  << "a skink basking in the sun flattens and spreads its body to soak the heat, rounding again as it displays or gets up\n"
                  << "island-simulator: dragonflies " << dragonflySwarm_.Dragonflies().size()
                  << " agents in three stream and wetland habitats; one patrol in three ends on a perch, a stem's top at half a metre, the wings held still a few seconds; "
                  << "a patrolling dragonfly dips to touch the water every few seconds, a fleck of spray at the touch; "
                  << "a perched dragonfly raises its abdomen toward the sun a second into the perch, the obelisk, and levels it to lift off\n"
                  << "island-simulator: frogs " << frogPopulation_.Frogs().size()
                  << " agents rest, hop, and retreat beside the stream and wetland; two keep the banks of the cascade's plunge pool, "
                     "and one of those fleeing the walker leaps into the pool, swims under, surfaces and makes back for its bank; "
                     "its splash scatters the fish holding in the pool to the deep; "
                     "a calling frog's throat swells and pulses under its chin through the call; "
                     "a resting frog blinks, its eyes drawn down into its head for a moment every seven seconds or so; "
                     "a resting frog's throat rises and falls with its breathing, a small swell under the chin between calls\n"
                  << "island-simulator: field mice " << RodentPopulation::MouseCount
                  << " and rats " << RodentPopulation::RatCount
                  << " forage, scurry, and hide in bounded inland edge habitats\n"
                  << "island-simulator: flies " << FlySwarms::Count
                  << " agents in four close-range inland organic-ground swarms; a swarm with fermenting fruit lying within ten metres shifts over it and hangs low there; "
                  << "a fly at the fruit sets down on it every other hover, wings still, and rubs its forelegs\n"
                  << "island-simulator: ants " << AntTrails::Count
                  << " agents in three soil trails and one real broadleaf-trunk trail; an ant pausing on the trail sweeps its antennae, feeling the way ahead; "
                  << "two ants meeting head to head on the trail pause to touch antennae\n"
                  << "island-simulator: cicadas " << CicadaColony::Count
                  << " agents resting, calling, and making short flights from real broadleaf trunks; "
                     "a cicada falls silent when the walker or an animal comes within seven metres and starts up again once they have gone; "
                     "a calling cicada's abdomen swells and drums with the call and its wings tremble\n"
                  << "island-simulator: beetles " << BeetlePopulation::Count
                  << " agents resting, foraging, and hiding in three inland leaf-litter colonies; "
                  << "a beetle at rest in the open lifts its wing cases a crack and holds them up until it moves on\n"
                  << "island-simulator: beach crabs " << crabColony_.Crabs().size()
                  << " agents in deterministic sand-and-debris-constrained colonies; every other bout works the wrack line and feeds among the strandings; "
                     "the resident's walk along the beach scatters them as the player's does; "
                     "a crab near its colony's burrow bolts down the hole and comes up once the beach has been clear a few seconds; "
                     "a crab paused in the open waves a claw, raised and swept up and down for a couple of seconds, a second into each pause and every nine seconds or so after; "
                     "a crab bolting for its burrow folds its eyestalks flat and raises them again as it comes up; "
                     "a crab feeding among the strandings blows a small bubble from its mouth every couple of seconds\n"
                  << "island-simulator: hermit crabs " << hermitCrabGroup_.Crabs().size()
                  << " agents in deterministic sheltered-beach habitats; one still pressed after its retreat pulls into its shell until the beach is clear; "
                  << "one paused on the sand feeds, its small claw picking at the sand and lifting to its mouth; "
                  << "walking, a hermit crab lifts its shell clear of the sand and it rocks with the steps, set down again at a pause\n"
                  << "island-simulator: coconut crabs " << coconutCrabGroup_.Crabs().size()
                  << " agents in deterministic coastal palm-zone habitats; "
                  << "a foraging crab walks to a coconut lying in its range and works it with its claws until it is gone, the claws knocking on the shell every couple of seconds, an original double click heard within fifteen metres; "
                     "every fourth forage without one is a climb up a living palm trunk, a pause near three metres up, and the way down\n"
                  << "island-simulator: an ant trail whose food end lies near a fallen fruit bends to it and the carriers come back from the fruit\n"
                  << "island-simulator: the grass bends under the nearest six animals and the resident as they pass, not only under the walker\n"
                  << "island-simulator: the bushes and understory are brushed aside by the same bodies as they pass\n"
                  << "island-simulator: mice, rats, geckos and skinks scatter from a passing animal or the resident as from the walker\n"
                  << "island-simulator: a mouse or rat setting out to forage with fruit lying within six metres goes to it and nibbles there a while\n"
                  << "island-simulator: a resting mouse or rat sits up to groom, washing its face with both forepaws for a couple of seconds\n"
                  << "island-simulator: a mouse or rat sweeps its whiskers with each sniff, fully on the forage and a little at rest\n"
                  << "island-simulator: an alert deer swings both ears toward the watcher\n"
                  << "island-simulator: a bolting deer flags its tail up, and a grazing or resting deer flicks it every few seconds\n"
                  << "island-simulator: the deer sentinel coming alert at the walker snorts the alarm, an original short nasal blast heard within fifty metres\n"
                  << "island-simulator: the resident's pause at the campfire tends the fire, its tongues and plume standing a third taller while they stay\n"
                  << "island-simulator: the resident's footfalls sound on the ground under them, sand, soil, grass, rock, shallows or planks, heard within twenty-four metres\n"
                  << "island-simulator: the resident's head turns to the walker within fourteen metres ahead of them, and back to the front once they leave that\n"
                  << "island-simulator: every third pause the resident stretches, both arms straight up for a couple of seconds before standing easy\n"
                  << "island-simulator: on the walk the resident whistles an original short phrase every half minute, heard within thirty metres\n"
                  << "island-simulator: a bird flushing from a perch or the ground claps its wings, an original half-second whir heard within twenty metres\n"
                  << "island-simulator: a flushing bird lets a pale feather go that rocks down slower than a leaf and lies where it lands\n"
                  << "island-simulator: a leaf or feather that comes down on the stream is taken by the current and drifts away with it\n"
                  << "island-simulator: the bear walks to fruit and coconuts lying in its range and eats them where they lie\n"
                  << "island-simulator: a rest two seconds in, undisturbed, settles the bear onto folded legs for a longer lie, and the walker's approach has it up within a second\n"
                  << "island-simulator: the bear standing lifts its head to sniff the air for a couple of seconds, and holds it up facing the walker in its alert range\n"
                  << "island-simulator: the bear shakes its head as a sniff ends, a quick swing side to side\n"
                  << "island-simulator: the bear flicks an ear back at a fly every few seconds, the ears taking turns\n"
                  << "island-simulator: the boars stand off from the bear and trot away when it comes close, as from the walker\n"
                  << "island-simulator: the hut's roof ridge is a perch for the birds\n"
                  << "island-simulator: crocodiles " << crocodileGroup_.Crocodiles().size()
                  << " agents bask, crawl, swim, and retreat within the stream estuary; one coming alert on its bank hisses at the walker, an original low open-jawed breath heard within thirty metres; "
                  << "a basking crocodile gapes, holding its jaws open for fourteen seconds a few seconds into each lie-up on the bank; "
                  << "a crocodile under way at the surface draws a faint wake of foam on the estuary behind its snout; "
                  << "a crocodile blinks, its eyes shut for a moment every eight seconds or so; "
                  << "a crocodile under way in the water folds its legs back along its flanks and swims on its tail\n"
                  << "island-simulator: fish " << fishSchools_.Fish().size()
                  << " agents in " << FishSchools::SchoolCount
                  << " distance-throttled shallow-water schools; a fish rolling through a turn flashes silver along its flanks, brightest parting from the shark; "
                  << "a school holding in the shade fans its pectoral fins to hover, folding them back as it moves on\n"
                  << "island-simulator: stingrays " << stingrayGroup_.Stingrays().size()
                  << " agents glide, forage, and settle over shallow sandy seabed; the shark passing within nine metres turns them away and bursts a settled one from the sand; "
                  << "a settled ray two seconds in flicks sand over its back and lies half-buried in the sand's tones until it moves; "
                  << "a settled ray breathes with a slow wave running back along each wing's edge\n"
                  << "island-simulator: octopuses " << octopusGroup_.Octopuses().size()
                  << " agents den, crawl, forage, and swim in reef and rocky shallows; one lying still takes the sand's tones over a few seconds and flashes back to its own colours when it moves or is startled; "
                  << "setting out to swim, or startled for its den, an octopus jets, its mantle drawn in hard and its arms straightened for the first moment; "
                  << "a foraging octopus settled on the bottom reaches one arm out ahead into the crevices every few seconds and draws it back, the arms taking turns\n"
                  << "island-simulator: sea turtles " << seaTurtleGroup_.Turtles().size()
                  << " agents in bounded habitat routes, three based in the sheltered western bay; the shark within eighteen metres sends a surfacing turtle under and the rest away from it; "
                  << "a turtle foraging low over the bottom dips its head to graze, a puff of sand rising at each bite; "
                  << "a turtle squeezes its eyes shut as its head breaks the surface for a breath, and blinks slowly every dozen seconds under; "
                  << "a turtle moving slow draws a front flipper up to rub its eye every twenty seconds or so, the flippers taking turns\n"
                  << "island-simulator: a fish breaking the surface within sixty metres draws the present shark that way for a few seconds before it takes up its patrol again\n"
                  << "island-simulator: one occasional non-hostile offshore shark patrol\n"
                  << "island-simulator: dolphins " << dolphinPod_.Dolphins().size()
                  << " agents in a peaceful offshore cruise, surface, and dive pod; a dolphin within twenty-eight metres of the shark leaves its route to keep its berth; "
                  << "a leap comes down in a splash, the sea's foam ring and the positional splash where the dolphin re-enters; "
                  << "every fourth surfacing after the leap's is a spy-hop, the dolphin slowing to rise head-up out of the water and look around for a few seconds; "
                  << "every other surfacing carries a tail slap, the flukes raised clear and brought down on the water with a splash where they strike; "
                  << "a dolphin's blowhole opens as its back breaks the surface for the breath and closes as it goes under\n"
                  << "island-simulator: sea from within: dolphins leap clear of the water on every fourth surfacing, "
                     "the shark shows its dorsal fin for nine seconds of each patrol, and fish schools part around the swimmer within three metres\n"
                  << "island-simulator: dolphin near/far geometry "
                  << DolphinRenderer::MaximumNearVertexCount << '/'
                  << DolphinRenderer::MaximumFarVertexCount
                  << " vertices, closed fins and CPU faceted daylight\n"
                  << "island-simulator: aquatic wildlife uses depth pitch, banked turns, "
                     "forked tails, and near head detail\n"
                  << "island-simulator: aquatic wildlife uses depth tint and restrained moving "
                     "sunlight bands in CPU vertex colours\n"
                  << "island-simulator: fish near/far geometry "
                  << FishRenderer::NearVertexCount << '/' << FishRenderer::FarVertexCount
                  << " vertices, closed fins and CPU faceted daylight\n"
                  << "island-simulator: near fish use capped smooth-lit bodies, caudal peduncles "
                     "and progressive rear-body bends\n"
                  << "island-simulator: the shark on patrol drops its jaw in a gape for a moment every several seconds, oftener while it investigates a breach\n"
                  << "island-simulator: shark near/far geometry "
                  << SharkRenderer::NearVertexCount << '/' << SharkRenderer::FarVertexCount
                  << " vertices, closed fins and CPU faceted daylight\n"
                  << "island-simulator: near shark uses a smooth-lit blunt head and body, "
                     "caudal peduncle and heterocercal tail\n"
                  << "island-simulator: monkeys " << monkeyTroop_.Monkeys().size()
                  << " agents with portable GPU skeletal animation, "
                  << MonkeyRenderer::SilhouetteCount << " silhouettes, 4 palettes\n"
                  << "island-simulator: monkeys use a horizontal lofted trunk and distance-driven quadrupedal gait\n"
                  << "island-simulator: a monkey idle on the ground scratches now and then, a forelimb up at its flank for a second and a half every fourteen seconds or so\n"
                  << "island-simulator: wetland mist: " << wetlandMist_.Puffs().size() << " low puffs over the seasonal wetland wrap back in at its windward edge as the wind carries them out\n"
                  << "island-simulator: a boar trotting off, a deer bounding or a goat sidestepping on dry sand or bare soil kicks up puffs of earth behind its hooves, gone within the second\n"
                  << "island-simulator: prints on the wet sand at the water's edge are taken back by the swash within a minute, those higher up the beach over four\n"
                  << "island-simulator: a butterfly at rest in the sun basks with its wings held open, pumping slowly, and closes them when a cloud's shadow passes over\n"
                  << "island-simulator: at the campfire the resident crouches to tend the fire, hands to the flames, and stands again to walk on\n"
                  << "island-simulator: at the jetty foot the resident casts a line, the rod held out in the right hand and the float riding the shallows past the deck's end, and takes it in to walk on\n"
                  << "island-simulator: the float rides the swell on its line, and every dozen seconds the resident lifts the rod to check it\n"
                  << "island-simulator: a boar trotting off or a deer bounding away drums its hoofbeats, an original run of dull thuds in fours heard within twenty metres, the deer's lighter and quicker\n"
                  << "island-simulator: monkey tree routes reject isolated canopy and mangroves\n"
                  << "island-simulator: monkeys walk to fruit lying under the crowns and eat it, idling over the spot\n"
                  << "island-simulator: boars forage for the fallen fruit and coconuts before the trunk bases and eat them where they lie\n"
                  << "island-simulator: near monkeys use bone-following finger and toe silhouettes\n"
                  << "island-simulator: near monkeys use an arched shoulder-rump trunk, broad blunt face, robust limbs, and compact tail\n"
                  << "island-simulator: near monkeys use a lower tapered trunk, separated crown, and restrained furry tail root\n"
                  << "island-simulator: near monkeys use a shallow waisted torso and shortened facial projection\n"
                  << "island-simulator: monkey shoulder and hip mantles blend the trunk into all four animated limbs\n"
                  << "island-simulator: near monkey tails keep a compact muscular hook and thirty-six raised fur clumps\n"
                  << "island-simulator: near monkey faces use paired fur cheek ruffs around the skin mask\n"
                  << "island-simulator: near monkey faces use integrated sculpted supraorbital relief and warmer modelled skin\n"
                  << "island-simulator: near monkey faces use a fitted dark nose pad and raised readable corneas\n"
                  << "island-simulator: near monkey eyes use compact recessed corneas, fitted short lids, and bilateral inner catchlights\n"
                  << "island-simulator: monkey face skin uses muted brown palettes and a recessed perimeter shade\n"
                  << "island-simulator: near monkeys use a compact forward-set cranium, reduced cheek ruffs, and fitted ears\n"
                  << "island-simulator: calm monkeys use bounded skeletal head attention\n"
                  << "island-simulator: terrain-conforming contact shadows in one portable "
                     "near-field batch\n"
                  << "island-simulator: near-surface swimming view uses generated depth haze, "
                     "surface scatter, peripheral veil, and sparse suspended motes in one draw\n"
                  << "island-simulator: movement ripples and shoreline-transition bursts "
                     "integrated into the portable ocean shader\n"
                  << "island-simulator: outer-ocean current uses progressive inward return "
                     "with mild coherent circulation\n"
                  << "island-simulator: keyboard run/walk toggle uses a transient mode "
                     "confirmation\n";
        if (debugOverlayVisible_) {
            std::cout << "island-simulator: debug overlay active\n";
        }
        if (worldDebugVisible_) {
            std::cout << "island-simulator: world debug visualization active with 9 ground-contact "
                         "samples, resolved movement trace, wildlife navigation guides, 3 bird "
                         "route regions, 24 bird spawns, 6 audio-zone contours, and positional-"
                         "emitter markers\n";
        }
        if (!benchmarkPath_.empty()) {
            std::cout << "island-simulator: benchmark " << ViewpointName(viewpoint_)
                      << " warming " << BenchmarkWarmupIntervals << " intervals, sampling "
                      << BenchmarkSampleIntervals << " intervals with VSync disabled\n";
            if (viewpoint_ == FixedViewpoint::BenchmarkTraversal) {
                std::cout << "island-simulator: benchmark traversal rail "
                          << benchmarkTraversalRoute_.ApproximateLength() << " m across "
                          << benchmarkTraversalRoute_.Waypoints().size()
                          << " authored waypoints\n";
            }
        }
        std::cout.flush();
        MirrorRuntimeDiagnosticToPlatformLog(rendererDiagnostic);
        std::ostringstream butterflyPopulationDiagnostic;
        butterflyPopulationDiagnostic << "island-simulator: clearing butterflies "
                                      << butterflyMeadow_.Butterflies().size()
                                      << " agents in deterministic low-altitude habitats";
        MirrorRuntimeDiagnosticToPlatformLog(butterflyPopulationDiagnostic.str());
        std::ostringstream geckoPopulationDiagnostic;
        geckoPopulationDiagnostic << "island-simulator: geckos "
                                  << geckoPopulation_.Geckos().size()
                                  << " agents in deterministic inland rest, flee, and hide habitats";
        MirrorRuntimeDiagnosticToPlatformLog(geckoPopulationDiagnostic.str());
        std::ostringstream skinkPopulationDiagnostic;
        skinkPopulationDiagnostic << "island-simulator: skinks "
                                  << skinkPopulation_.Skinks().size()
                                  << " agents in deterministic open-ground bask, forage, and flee habitats";
        MirrorRuntimeDiagnosticToPlatformLog(skinkPopulationDiagnostic.str());
        std::ostringstream dragonflyPopulationDiagnostic;
        dragonflyPopulationDiagnostic<<"island-simulator: dragonflies "
            <<dragonflySwarm_.Dragonflies().size()<<" agents in three stream and wetland habitats";
        MirrorRuntimeDiagnosticToPlatformLog(dragonflyPopulationDiagnostic.str());
        std::ostringstream frogPopulationDiagnostic;
        frogPopulationDiagnostic << "island-simulator: frogs "
            << frogPopulation_.Frogs().size()
            << " agents rest, hop, and retreat beside the stream and wetland";
        MirrorRuntimeDiagnosticToPlatformLog(frogPopulationDiagnostic.str());
        std::ostringstream rodentPopulationDiagnostic;
        rodentPopulationDiagnostic << "island-simulator: field mice "
            << RodentPopulation::MouseCount << " and rats " << RodentPopulation::RatCount
            << " forage, scurry, and hide in bounded inland edge habitats";
        MirrorRuntimeDiagnosticToPlatformLog(rodentPopulationDiagnostic.str());
        std::ostringstream flyPopulationDiagnostic;
        flyPopulationDiagnostic << "island-simulator: flies " << FlySwarms::Count
            << " agents in four close-range inland organic-ground swarms";
        MirrorRuntimeDiagnosticToPlatformLog(flyPopulationDiagnostic.str());
        std::ostringstream antPopulationDiagnostic;
        antPopulationDiagnostic << "island-simulator: ants " << AntTrails::Count
            << " agents in three soil trails and one real broadleaf-trunk trail";
        MirrorRuntimeDiagnosticToPlatformLog(antPopulationDiagnostic.str());
        std::ostringstream cicadaPopulationDiagnostic;
        cicadaPopulationDiagnostic << "island-simulator: cicadas " << CicadaColony::Count
            << " agents resting, calling, and making short flights from real broadleaf trunks";
        MirrorRuntimeDiagnosticToPlatformLog(cicadaPopulationDiagnostic.str());
        std::ostringstream beetlePopulationDiagnostic;
        beetlePopulationDiagnostic << "island-simulator: beetles " << BeetlePopulation::Count
            << " agents resting, foraging, and hiding in three inland leaf-litter colonies";
        MirrorRuntimeDiagnosticToPlatformLog(beetlePopulationDiagnostic.str());
        std::ostringstream crabPopulationDiagnostic;
        crabPopulationDiagnostic << "island-simulator: beach crabs "
                                 << crabColony_.Crabs().size()
                                 << " agents in deterministic sand-and-debris-constrained colonies";
        MirrorRuntimeDiagnosticToPlatformLog(crabPopulationDiagnostic.str());
        std::ostringstream hermitCrabPopulationDiagnostic;
        hermitCrabPopulationDiagnostic << "island-simulator: hermit crabs "
            << hermitCrabGroup_.Crabs().size()
            << " agents in deterministic sheltered-beach habitats";
        MirrorRuntimeDiagnosticToPlatformLog(hermitCrabPopulationDiagnostic.str());
        std::ostringstream coconutCrabPopulationDiagnostic;
        coconutCrabPopulationDiagnostic << "island-simulator: coconut crabs "
            << coconutCrabGroup_.Crabs().size()
            << " agents in deterministic coastal palm-zone habitats";
        MirrorRuntimeDiagnosticToPlatformLog(coconutCrabPopulationDiagnostic.str());
        std::ostringstream seaTurtlePopulationDiagnostic;
        seaTurtlePopulationDiagnostic << "island-simulator: sea turtles "
                                      << seaTurtleGroup_.Turtles().size()
                                      << " agents in bounded near-shore cruise, forage, and surface routes";
        MirrorRuntimeDiagnosticToPlatformLog(seaTurtlePopulationDiagnostic.str());
        std::ostringstream stingrayPopulationDiagnostic;
        stingrayPopulationDiagnostic << "island-simulator: stingrays "
                                     << stingrayGroup_.Stingrays().size()
                                     << " agents glide, forage, and settle over shallow sandy seabed";
        MirrorRuntimeDiagnosticToPlatformLog(stingrayPopulationDiagnostic.str());
    }

    void IslandGame::TryStartSoundscape() noexcept
    {
        if (!soundscape_ || soundscape_->WasStartAttempted() || smokeTest_) {
            return;
        }
        const PlayerPosition& position = playerMotion_.Position();
        const float shoreDistance = terrain_.SignedShoreDistance(position.x, position.z);
        const float windExposure=CoastalWindResponse::Exposure(
            position.x,position.z,shoreDistance);
        const bool swimming = playerMotion_.Mode() == LocomotionMode::Swimming;
        const float waterDepth = WalkerWaterDepth();
        const float horizontalSpeed = std::hypot(playerMotion_.VelocityX(),
                                                 playerMotion_.VelocityZ());
        (void)soundscape_->Start(
            shoreDistance, swimming, waterDepth, horizontalSpeed,
            playerMotion_.OceanCurrentStrength(terrain_), windExposure,
            position.x, position.z,position.y);
        soundscapeNeedsGesture_ = false;
    }

    void IslandGame::ReportControlTransition(const ControlTransitionEvent event) noexcept
    {
        try {
            const auto message=ControlTransitionMessage(event);
            std::cout << message << '\n' << std::flush;
            MirrorRuntimeDiagnosticToPlatformLog(message);
            const auto p=flightMotion_.Position();
            const ControlTransitionSnapshot snapshot{drawnFrames_,
                {cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z}, {p.x,p.y,p.z},
                yaw_,pitch_,flightMotion_.IsActive(),graphics_.getIsFullScreenProperty()};
            MirrorRuntimeDiagnosticToPlatformLog(FormatControlTransition(event,snapshot));
        } catch (...) {
            // Logging must not turn an otherwise successful input transition into a failure.
        }
    }

    void IslandGame::Update(GameTime& gameTime)
    {
        const auto updateStarted = std::chrono::steady_clock::now();
        updateProfile_.Begin("input, weather and walker");
        const auto measureWildlife = [this](std::size_t species, auto&& update) {
            const auto started = std::chrono::steady_clock::now();
            update();
            runtimeWildlifeMilliseconds_[species] += std::chrono::duration<double, std::milli>(
                std::chrono::steady_clock::now() - started).count();
        };
        const bool snakeCaptureClock=UsesSnakeCaptureClock();
        const bool campfireCaptureClock=UsesCampfireCaptureClock();
        if(campfireCaptureClock)++campfireCaptureUpdates_;
        const bool residentCaptureClock=UsesResidentCaptureClock();
        if(residentCaptureClock)++residentCaptureUpdates_;
        const bool bearCaptureClock=UsesBearCaptureClock();
        if(bearCaptureClock)++bearCaptureUpdates_;
        const bool monkeyCaptureClock=UsesMonkeyCaptureClock();
        if(monkeyCaptureClock)++monkeyCaptureUpdates_;
        const bool goatCaptureClock=UsesGoatCaptureClock();
        const bool boarCaptureClock=UsesBoarCaptureClock();
        if(boarCaptureClock)++boarCaptureUpdates_;
        const bool spiderCaptureClock=UsesSpiderCaptureClock();
        if(spiderCaptureClock)++spiderCaptureUpdates_;
        if(goatCaptureClock)++goatCaptureUpdates_;
        if (!snakeFirstDrawReturnReported_ && drawnFrames_ > 0U
            && (viewpoint_ == FixedViewpoint::SnakeDetail || viewpoint_ == FixedViewpoint::SnakeHeadDetail
                || viewpoint_==FixedViewpoint::TreeSnakeDetail)) {
            snakeFirstDrawReturnReported_ = true;
            MirrorRuntimeDiagnosticToPlatformLog("island-simulator: snake QA update resumed after first draw");
        }
        if(snakeCaptureClock) ++snakeCaptureUpdates_;
        const bool quadDriveClock = QuadDriveView() && !capturePath_.empty();
        const float deltaSeconds = canopyLodInspection_ ? 0.0f : (snakeCaptureClock||goatCaptureClock||spiderCaptureClock||boarCaptureClock||campfireCaptureClock||residentCaptureClock||bearCaptureClock||monkeyCaptureClock||quadDriveClock) ? 0.1f : std::min(
            0.1f, static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty())
                / 1000.0f);
        worldSeedCopyFeedbackSeconds_ = std::max(
            0.0f, worldSeedCopyFeedbackSeconds_ - deltaSeconds);
        locomotionModeNoticeSeconds_ = std::max(
            0.0f, locomotionModeNoticeSeconds_ - deltaSeconds);
        // Fixed QA cameras deliberately reject player movement, but their diagnostic overlay
        // remains interactive so automated and manual reviewers can inspect an unobstructed view.
        const KeyboardState frameKeyboard = Keyboard::GetState();
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
        // Web fullscreen is requested synchronously by browser_fullscreen.js in the
        // trusted key event. Android keeps its existing immersive presentation.
        const bool fullscreenDown = frameKeyboard.IsKeyDown(Keys::F11);
        if (fullscreenDown && !fullscreenToggleWasDown_ && getIsActiveProperty()) {
            graphics_.ToggleFullScreen();
            ReportControlTransition(graphics_.getIsFullScreenProperty()
                ? ControlTransitionEvent::FullscreenOn : ControlTransitionEvent::FullscreenOff);
        }
        fullscreenToggleWasDown_ = fullscreenDown;
#endif
        {
            // T moves the clock an hour on (for seeing the night without waiting).
            const bool timeKeyDown = frameKeyboard.IsKeyDown(Keys::T);
            if (timeKeyDown && !timeKeyWasDown_) {
                dayNight_.SetHour(dayNight_.Hour() + 1.0f);
                std::cout << "island-simulator: the clock moves on to hour " << static_cast<int>(dayNight_.Hour()) << '\n';
            }
            timeKeyWasDown_ = timeKeyDown;
            // Y holds the next weather (after a thunderstorm it lets the spells run again).
            const bool weatherKeyDown = frameKeyboard.IsKeyDown(Keys::Y);
            if (weatherKeyDown && !weatherKeyWasDown_) {
                if (weather_.Forced() && weather_.State().kind == WeatherKind::Thunderstorm) {
                    weather_.Release();
                    std::cout << "island-simulator: the weather runs on its own again\n";
                } else {
                    const auto next = static_cast<WeatherKind>((static_cast<int>(weather_.State().kind) + 1) % 5);
                    weather_.Force(next);
                    std::cout << "island-simulator: the weather is now " << WeatherName(next) << '\n';
                }
            }
            weatherKeyWasDown_ = weatherKeyDown;
            const float hoursBefore = dayNight_.Hour();
            dayNight_.Advance(deltaSeconds);
            const float gameHours = dayNight_.Frozen() ? 0.0f
                : std::fmod(dayNight_.Hour() - hoursBefore + 24.0f, 24.0f);
            weather_.Advance(dayNight_.Frozen() && !weather_.Forced() ? 0.0f : deltaSeconds, gameHours);
            weather_now::Set(weather_.State());
            // the gust fronts follow the weather's wind (faster and closer in a storm)
            WindGust::Advance(deltaSeconds, weather_.State().wind);
            auto light = dayNight_.Lighting();
            WeatherSystem::ApplyTo(light, weather_.State());
            weather_now::AddMoonShadows(light);
            sun_light::Set(light);
            if (soundscape_) {
                soundscape_->SetWeather(weather_.State().rain, weather_.State().strikes, weather_.State().lastStrikeDistance);
                {
                    // under the crowns the rain drums on the leaves; in the cave it is far off
                    const PlayerPosition& listener = playerMotion_.Position();
                    // (the crowns' light averaged over some four metres round the listener, so the
                    // rain does not swell and fade with each fleck of the dapple underfoot)
                    float open = 0.0f;
                    for (const auto& offset : {std::array<float, 2>{0.0f, 0.0f}, {4.0f, 0.0f}, {-4.0f, 0.0f}, {0.0f, 4.0f}, {0.0f, -4.0f}})
                        open += 0.2f * std::clamp(canopyLightField_.Sample(listener.x + offset[0], listener.z + offset[1]), 0.0f, 1.0f);
                    const bool inCave = terrainRenderer_ && terrainRenderer_->CaveGround().Contains(listener.x, listener.z)
                        && cameraPosition_.Y < terrain_.Height(cameraPosition_.X, cameraPosition_.Z) - 0.5f;
                    soundscape_->SetRainShelter(1.0f - open, inCave ? 1.0f : 0.0f);
                }
                soundscape_->SetWind(weather_.State().wind);
                soundscape_->SetWetness(weather_.State().wetness);
                soundscape_->SetNightness(1.0f - std::clamp((light.sunElevationDegrees + 2.0f) / 10.0f, 0.0f, 1.0f));
                {
                    // the dawn chorus: from first light to an hour and a half after sunrise
                    const float hour = dayNight_.Hour();
                    const auto smooth = [](float a, float b, float v) { const float t = std::clamp((v - a) / (b - a), 0.0f, 1.0f); return t * t * (3.0f - 2.0f * t); };
                    soundscape_->SetDawn(smooth(5.3f, 5.9f, hour) * (1.0f - smooth(7.0f, 7.8f, hour)) * (1.0f - weather_.State().rain));
                }
            }
            // the birds go to roost at dusk and sit out a downpour, the monkeys too; the fruit
            // bats come out, but not into a storm's rain
            const float dark = 1.0f - std::clamp((light.sunElevationDegrees + 2.0f) / 10.0f, 0.0f, 1.0f);
            const float downpour = std::clamp((weather_.State().rain - 0.5f) / 0.4f, 0.0f, 1.0f);
            birdFlock_.SetNightness(std::max(dark, downpour));
            monkeyTroop_.SetNightness(std::max(dark, downpour));
            if (deer_) deer_->SetNightness(dark);
            if (goats_) goats_->SetNightness(dark);
            boarHerd_.SetNightness(dark);
            if (bear_) bear_->SetNightness(dark);
            coconutCrabGroup_.SetNightness(dark);
            reefHeronGroup_.SetNightness(dark);
            crabColony_.SetNightness(dark);
            hermitCrabGroup_.SetNightness(dark);
            fruitBatColony_.SetNightness(dark * (1.0f - downpour));
            const auto shading = sun_light::Current().shading;
            wildlife_render::FixedSunDirection = Vector3(shading.x, shading.y, shading.z);
        }
        const bool debugOverlayToggleDown = frameKeyboard.IsKeyDown(Keys::F1);
        if (debugOverlayToggleDown && !debugOverlayToggleWasDown_) {
            debugOverlayVisible_ = !debugOverlayVisible_;
            std::cout << "island-simulator: debug overlay "
                      << (debugOverlayVisible_ ? "enabled" : "disabled") << '\n';
        }
        debugOverlayToggleWasDown_ = debugOverlayToggleDown;
        const bool muteToggleDown = frameKeyboard.IsKeyDown(Keys::M);
        if (muteToggleDown && !muteToggleWasDown_ && soundscape_) {
            soundscape_->ToggleMuted();
            userSettings_.audioMuted = soundscape_->IsMuted();
            PersistUserSettings();
        }
        muteToggleWasDown_ = muteToggleDown;
        if (!benchmarkPath_.empty()
            && viewpoint_ == FixedViewpoint::BenchmarkTraversal) {
            UpdateBenchmarkTraversal();
        }
        if (!smokeTest_ && viewpoint_ == FixedViewpoint::None) {
            updateProfile_.Mark("UpdateCamera");
            UpdateCamera(deltaSeconds);
        }
        updateProfile_.Mark("UpdateQuad");
        UpdateQuad(deltaSeconds);
        updateProfile_.Mark("whales");
        whales_.Update(deltaSeconds, wind::DirectionX, wind::DirectionZ);
        updateProfile_.Mark("sailboat");
        sailboat_.Update(deltaSeconds);
        {
            // a swimmer stirs the plankton into light on a dark night (as the surf does)
            const auto& light = sun_light::Current().lighting;
            const float dark = 1.0f - std::clamp((light.sunElevationDegrees + 6.0f) / 8.0f, 0.0f, 1.0f);
            swimGlowStrength_ = dark * (1.0f - 0.55f * light.moonLight);
            const PlayerPosition& swimmer = playerMotion_.Position();
            const float vx = playerMotion_.VelocityX(), vz = playerMotion_.VelocityZ();
            updateProfile_.Mark("swimGlow");
            swimGlow_.Update(deltaSeconds, swimmer.x, 0.0f, swimmer.z, std::hypot(vx, vz), vx, vz,
                             playerMotion_.Mode() == LocomotionMode::Swimming, swimGlowStrength_);
        }
        {
            // squalls breed waterspouts: heavy rain in a strong wind
            const auto& w = weather_now::Current();
            updateProfile_.Mark("waterspout");
            waterspout_.Update(deltaSeconds, std::clamp((w.rain - 0.45f) / 0.30f, 0.0f, 1.0f) * std::clamp((w.wind - 1.2f) / 0.4f, 0.0f, 1.0f),
                               wind::DirectionX, wind::DirectionZ);
        }
        updateProfile_.Mark("summitCloud");
        summitCloud_.Update(deltaSeconds, SummitCloudField::Cover(weather_now::Current().cloudCover,
                                                                  weather_now::Current().rain, dayNight_.Hour()),
                            wind::DirectionX, wind::DirectionZ, weather_now::Current().wind);
        updateProfile_.Mark("morningMist");
        if (morningMist_)
            morningMist_->Update(deltaSeconds, MorningMistField::Thickness(dayNight_.Hour(), weather_now::Current().wind,
                                                                           weather_now::Current().rain),
                                 wind::DirectionX, wind::DirectionZ);
        if (fireflies_) {
            const auto& light = sun_light::Current().lighting;
            updateProfile_.Mark("fireflies");
            fireflies_->Update(deltaSeconds, 1.0f - std::clamp((light.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f),
                               weather_now::Current().rain);
        }
        if (QuadDriveView()) UpdateQuadChaseCamera(deltaSeconds);
        const bool waterViewAllowed = viewpoint_ == FixedViewpoint::None
            || viewpoint_ == FixedViewpoint::WaterlineDetail
            || viewpoint_ == FixedViewpoint::CurrentBoundaryDetail
            || viewpoint_ == FixedViewpoint::ReefHabitatDetail || viewpoint_ == FixedViewpoint::AnemoneDetail
            || viewpoint_ == FixedViewpoint::BenchmarkTraversal;
        updateProfile_.Mark("waterViewModel");
        (void)waterViewModel_.Update(
            waterViewAllowed && !flightMotion_.IsActive() && playerMotion_.Mode() == LocomotionMode::Swimming,
            playerMotion_.GroundHeight(), playerMotion_.OceanCurrentStrength(terrain_),
            deltaSeconds);
        {
            // A descent breathes out a burst of bubbles that thins over the next two seconds.
            const float submerged = flightMotion_.IsActive() ? 0.0f : playerMotion_.SubmergedDepth();
            const float descent = std::max(0.0f, submerged - lastSubmergedDepth_);
            lastSubmergedDepth_ = submerged;
            bubbleBurst_ = std::clamp(std::max(bubbleBurst_ - deltaSeconds * 0.5f, descent * 4.0f), 0.0f, 1.0f);
        }
        const float rippleSpeed = viewpoint_ == FixedViewpoint::WaterlineDetail
            ? 1.35f
            : (viewpoint_ == FixedViewpoint::BenchmarkTraversal
                   ? benchmarkTraversalPresentationSpeed_
                   : std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ()));
        updateProfile_.Mark("waterRippleModel");
        (void)waterRippleModel_.Update(
            !flightMotion_.IsActive() && playerMotion_.Mode() == LocomotionMode::Swimming,
            playerMotion_.GroundHeight(), rippleSpeed, deltaSeconds);
        const float grassInteractionSpeed = viewpoint_ == FixedViewpoint::GrassDetail
            ? 3.2f
            : (viewpoint_ == FixedViewpoint::BenchmarkTraversal
                   ? benchmarkTraversalPresentationSpeed_
                   : std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ()));
        updateProfile_.Mark("grassInteractionModel");
        (void)grassInteractionModel_.Update(grassInteractionSpeed, deltaSeconds);
        {
            // The walker's prints: laid every stride while the feet are on sand.
            const PlayerPosition& walker=playerMotion_.Position();
            const bool onSand=!flightMotion_.IsActive()&&!IsDebugFreeCameraActive()
                &&playerMotion_.Mode()!=LocomotionMode::Swimming&&FootstepGroundUnderPlayer()==FootstepSurface::Sand;
            updateProfile_.Mark("footprints");
            footprints_.Update(deltaSeconds,walker.x,walker.z,playerMotion_.VelocityX(),playerMotion_.VelocityZ(),onSand,
                onSand&&terrain_.Sample(walker.x,walker.z).surface==TerrainSurface::WetSand);
            // The shadow figure's gait: a turn of the phase per stride walked, the limbs swinging
            // wider with the pace and settling when the walker stops, the body turning to the way
            // it walks (or, standing, to the view) over some few tenths of a second.
            const float speed=std::hypot(playerMotion_.VelocityX(),playerMotion_.VelocityZ());
            walkerGaitPhase_=std::fmod(walkerGaitPhase_+speed*deltaSeconds/walker_shadow::StrideMetres
                *2.0f*std::numbers::pi_v<float>,2.0f*std::numbers::pi_v<float>);
            const float swingTarget=std::clamp(speed/1.6f,0.0f,1.0f);
            walkerSwing_+=(swingTarget-walkerSwing_)*std::min(1.0f,deltaSeconds*6.0f);
            if(speed<0.2f&&walkerSwing_<0.05f) walkerGaitPhase_=0.0f;
            const float facing=speed>0.3f?std::atan2(playerMotion_.VelocityX(),playerMotion_.VelocityZ()):yaw_;
            const float turn=std::remainder(facing-walkerFacingYaw_,2.0f*std::numbers::pi_v<float>);
            walkerFacingYaw_+=turn*std::min(1.0f,deltaSeconds*8.0f);
        }
        const auto previousResident=resident_->State();
        const auto& observer=playerMotion_.Position();
        if(viewpoint_==FixedViewpoint::ResidentTendDetail) resident_->PreviewTending();
        if(viewpoint_==FixedViewpoint::ResidentFishDetail) resident_->PreviewFishing();
        updateProfile_.Mark("resident");
        resident_->Update(viewpoint_==FixedViewpoint::ResidentTendDetail||viewpoint_==FixedViewpoint::ResidentFishDetail?0.0f:deltaSeconds,
            {observer.x,observer.y,observer.z});
        {
            // The resident's pause at the pit tends the fire: it builds up while they stay
            // and dies back once they walk on.
            const auto& tender=resident_->State();
            const bool tending=CampfireSite::Tending(tender.x,tender.z,tender.mode==ResidentMode::Pausing);
            campfireTend_+=std::clamp((tending?1.0f:0.0f)-campfireTend_,-0.3f*deltaSeconds,0.6f*deltaSeconds);
            if(campfireRenderer_) campfireRenderer_->SetTending(campfireTend_);
            // At the fire the resident crouches to tend it, hands to the flames; at night they
            // stay there, keeping it up until dawn, instead of walking their round in the dark.
            resident_->SetCrouch(tending?1.0f:0.0f);
            // ...and in a downpour they wait it out wherever they have stopped (under the trees,
            // at the fire, on the jetty foot) before walking on.
            // (The resident's own QA view stages their walk and keeps them walking.)
            resident_->SetHoldPause((tending&&sun_light::Current().lighting.sunElevationDegrees<-2.0f)
                ||(weather_now::Current().rain>0.6f&&viewpoint_!=FixedViewpoint::ResidentDetail));
        }
        {
            // The resident's footfalls: one every 0.68 m of their walk, on the ground under them.
            const auto& walker=resident_->State();
            const bool walking=walker.mode==ResidentMode::Walking&&walker.speed>0.1f;
            const FootstepSurface underfoot=FootstepGroundAt(walker.x,walker.z);
            if(residentSteps_.Step(walker.x,walker.z,walking)&&soundscape_)
                soundscape_->NoteResidentStep(walker.x,walker.y,walker.z,underfoot);
            // and the bare feet print the sand (on the walk to the jetty), the wet sand's sooner gone
            residentPrints_.SetStride(0.68f);
            residentPrints_.Update(deltaSeconds,walker.x,walker.z,
                walking?std::sin(walker.heading)*walker.speed:0.0f,walking?std::cos(walker.heading)*walker.speed:0.0f,
                underfoot==FootstepSurface::Sand||underfoot==FootstepSurface::Shallows,
                underfoot==FootstepSurface::Shallows||terrain_.Sample(walker.x,walker.z).surface==TerrainSurface::WetSand);
            // On the walk the resident whistles a phrase every half minute or so.
            residentWhistleClock_=walking?residentWhistleClock_+deltaSeconds:std::min(residentWhistleClock_,6.0f);
            if(walking&&residentWhistleClock_>=28.0f&&soundscape_) {
                residentWhistleClock_=0.0f;
                soundscape_->NoteResidentWhistle(walker.x,walker.y+1.55f,walker.z);
            }
        }
        if(viewpoint_==FixedViewpoint::ResidentDetail) {
            const auto& current=resident_->State();
            cameraPosition_+=Vector3(current.x-previousResident.x,current.y-previousResident.y,
                current.z-previousResident.z);
            const Vector3 target(current.x,current.y+1.02f,current.z);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        const auto previousBear=bear_->State();
        bear_->SetLyingFruit(fallingFruit_.Fruit());
        if(viewpoint_==FixedViewpoint::BearLyingDetail) bear_->PreviewLie();
        updateProfile_.Mark("bear");
        bear_->Update(viewpoint_==FixedViewpoint::BearLyingDetail?0.0f:deltaSeconds,bearCaptureClock?BearPoint{NAN,NAN,NAN}
            :BearPoint{observer.x,observer.y,observer.z});
        for (const std::size_t eaten : bear_->EatenFruit()) fallingFruit_.Consume(eaten);
        if(viewpoint_==FixedViewpoint::BearDetail) {
            const auto& current=bear_->State();
            cameraPosition_+=Vector3(current.x-previousBear.x,current.y-previousBear.y,current.z-previousBear.z);
            const Vector3 target(current.x,current.y+1.03f,current.z+.22f);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        updateProfile_.Mark("fallingFruit");
        fallingFruit_.Update(deltaSeconds, monkeyTroop_.Monkeys());
        updateProfile_.Mark("streamDrift");
        streamDrift_.Update(deltaSeconds);
        canopyLeafFall_.SetWind(weather_.State().wind);
        updateProfile_.Mark("canopyLeafFall");
        canopyLeafFall_.Update(seaForageSeconds_,deltaSeconds,cameraPosition_.X,cameraPosition_.Z);
        // A leaf or feather that lands on the stream is taken by the current.
        if(canopyLeafFall_.LastLanding().count!=notedLeafLandings_) {
            notedLeafLandings_=canopyLeafFall_.LastLanding().count;
            const auto& landing=canopyLeafFall_.LastLanding();
            float courseDistance=0,lateral=0;
            if(streamDrift_.Course().Locate(landing.x,landing.z,courseDistance,lateral)
               &&streamDrift_.Adopt(courseDistance,lateral,landing.variant)) {
                canopyLeafFall_.Remove(landing.slot);
                streamRings_.Note(landing.x,landing.z,0.3f);
            }
        }
        // the clutches break out after dark (the turtle-nest QA view keeps its daytime clutch)
        if (viewpoint_ != FixedViewpoint::TurtleNestDetail)
            hatchlings_.SetNightness(1.0f - std::clamp((sun_light::Current().lighting.sunElevationDegrees + 2.0f) / 10.0f, 0.0f, 1.0f));
        updateProfile_.Mark("hatchlings");
        hatchlings_.Update(deltaSeconds);
        updateProfile_.Mark("cascadeMist");
        cascadeMist_.Update(deltaSeconds,seaForageSeconds_);
        updateProfile_.Mark("wetlandMist");
        wetlandMist_.Update(deltaSeconds,seaForageSeconds_);
        updateProfile_.Mark("surfSpray");
        surfSpray_.Update(deltaSeconds,seaForageSeconds_,cameraPosition_.X,cameraPosition_.Z);
        updateProfile_.Mark("dolphinBlows");
        dolphinBlows_.Update(deltaSeconds,seaForageSeconds_,dolphinPod_.Dolphins());
        streamFish_.SetLeaves(streamDrift_.Leaves());
        updateProfile_.Mark("streamFish");
        streamFish_.Update(deltaSeconds,playerMotion_.Position().x,playerMotion_.Position().z);
        fallingFruit_.SetWind(weather_.State().wind);
        updateProfile_.Mark("fallingFruit.UpdateCoconuts");
        fallingFruit_.UpdateCoconuts(seaForageSeconds_, cameraPosition_.X, cameraPosition_.Z);
        if (soundscape_ && fallingFruit_.LastCoconutLanding().count != notedThudCount_) {
            const auto& landing = fallingFruit_.LastCoconutLanding();
            notedThudCount_ = landing.count;
            soundscape_->NoteCoconutThud(landing.x, landing.y, landing.z);
            // The thud carries: deer and goats within thirty metres stop and face the fall.
            if (deer_) deer_->NoteDisturbance(landing.x, landing.z);
            if (goats_) goats_->NoteDisturbance(landing.x, landing.z);
            birdFlock_.NoteDisturbance(landing.x, landing.z);
        }
        if (soundscape_ && crocodileGroup_.LastEntry().count != notedEntryCount_) {
            notedEntryCount_ = crocodileGroup_.LastEntry().count;
            soundscape_->NoteBirdSplash(crocodileGroup_.LastEntry().x, 0.0f, crocodileGroup_.LastEntry().z);
        }
        if (soundscape_ && fishSchools_.LastBreach().count != notedBreachCount_) {
            notedBreachCount_ = fishSchools_.LastBreach().count;
            soundscape_->NoteFishBreach(fishSchools_.LastBreach().x, 0.0f, fishSchools_.LastBreach().z);
        }
        if (soundscape_ && reefHeronGroup_.LastFlush().count != notedHeronFlushes_) {
            notedHeronFlushes_ = reefHeronGroup_.LastFlush().count;
            const auto& takeoff = reefHeronGroup_.LastFlush();
            soundscape_->NoteHeronCroak(takeoff.x, takeoff.y, takeoff.z);
        }
        // a heron's bill going into the water makes the same small plip
        if (soundscape_ && reefHeronGroup_.LastStrike().count != notedHeronStrikes_) {
            notedHeronStrikes_ = reefHeronGroup_.LastStrike().count;
            soundscape_->NoteFishBreach(reefHeronGroup_.LastStrike().x, 0.0f, reefHeronGroup_.LastStrike().z);
        }
        {
            // The seabirds' school of the moment: the schools take turns every ninety seconds
            // so the low circling moves around the coast instead of fixing on one bay.
            const auto schools = fishSchools_.Schools();
            seaForageSeconds_ += deltaSeconds;
            if (!schools.empty()) {
                const auto turn = static_cast<std::size_t>(seaForageSeconds_ / 90.0f) % schools.size();
                const FishSchoolState& school = schools[turn];
                birdFlock_.SetSeaForage(school.centreX, school.centreZ, true);
            }
        }
        birdFlock_.SetLyingFruit(fallingFruit_.Fruit());
        const std::array<GrassWalker, GrassWalkers::Slots> birdWalkers = GatherWalkers(cameraPosition_.X, cameraPosition_.Z);
        birdFlock_.SetWalkers(birdWalkers);
        birdFlock_.SetSnakes(snakes_->Snakes());
        // The preen view holds the wreck's perched bird mid-preen every frame (the perch
        // sites are seeded after the camera is placed).
        if(viewpoint_ == FixedViewpoint::BirdPreenDetail) birdFlock_.PreviewPreen(0U);
        updateProfile_.Mark("birdFlock");
        birdFlock_.Update((viewpoint_ == FixedViewpoint::BirdDetail
                              || viewpoint_ == FixedViewpoint::BirdPreenDetail
                              || viewpoint_ == FixedViewpoint::GroundDoveDetail)
                                 ? 0.0f : deltaSeconds,
                          // The preen view's camera sits inside the flush distance, so the
                          // flock sees the walker far off and the bird keeps its perch.
                          viewpoint_ == FixedViewpoint::BirdPreenDetail ? 1.0e4f : cameraPosition_.X,
                          cameraPosition_.Y,
                          viewpoint_ == FixedViewpoint::BirdPreenDetail ? 1.0e4f : cameraPosition_.Z);
        // A bird settling on a branch, and a monkey moving through a crown every few
        // seconds, shake a leaf loose.
        // A bird mobbing a snake chatters the alarm over it every couple of seconds.
        if (soundscape_ && birdFlock_.LastMob().count != notedBirdMobs_) {
            notedBirdMobs_ = birdFlock_.LastMob().count;
            const BirdFlock::BirdMob& mob = birdFlock_.LastMob();
            if (mob.valid) soundscape_->NoteBirdChatter(mob.x, mob.y + 1.5f, mob.z);
        }
        // A bird flushing, from a perch or the ground, claps its wings where it was.
        if (soundscape_ && birdFlock_.AnyFlushCount() != notedFlushes_) {
            notedFlushes_ = birdFlock_.AnyFlushCount();
            const BirdFlock::BirdFlush& flush = birdFlock_.LastFlush();
            if (flush.valid) {
                soundscape_->NoteWingClap(flush.x, flush.y, flush.z);
                canopyLeafFall_.Feather(flush.x, flush.y, flush.z, notedFlushes_ * 7919U);
            }
        }
        if (birdFlock_.LandingCount() != notedLandingCount_) {
            notedLandingCount_ = birdFlock_.LandingCount();
            const BirdPerchLanding& landing = birdFlock_.LastLanding();
            if (landing.valid) canopyLeafFall_.Shake(landing.x, landing.y, landing.z, notedLandingCount_ * 7919U);
        }
        for (std::size_t i = 0; i < monkeyTroop_.Monkeys().size() && i < monkeyShakeClock_.size(); ++i) {
            const MonkeyState& monkey = monkeyTroop_.Monkeys()[i];
            const bool inCrown = monkey.behavior == MonkeyBehavior::InspectTree
                && monkey.simulationTier != WildlifeSimulationTier::Dormant
                && monkey.y > terrain_.Height(monkey.x, monkey.z) + 2.0f;
            if (!inCrown) { monkeyShakeClock_[i] = 0.0f; continue; }
            monkeyShakeClock_[i] += deltaSeconds;
            if (monkeyShakeClock_[i] >= 5.0f) {
                monkeyShakeClock_[i] = 0.0f;
                canopyLeafFall_.Shake(monkey.x, monkey.y, monkey.z, static_cast<std::uint32_t>(i) * 7919U + (++monkeyShakes_) * 104729U);
            }
        }
        updateProfile_.Mark("fruitBatColony");
        fruitBatColony_.Update((viewpoint_ == FixedViewpoint::FruitBatDetail
                                   || viewpoint_ == FixedViewpoint::FruitBatRoostDetail)
                                  ? 0.0f : deltaSeconds,
                               cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
        // A roosting bat stirs in its crown every so often: a fruit lets go from the crown
        // and a leaf comes loose beside the bat.
        for (std::size_t i = 0; i < fruitBatColony_.Bats().size() && i < batStirClock_.size(); ++i) {
            const FruitBatState& bat = fruitBatColony_.Bats()[i];
            if (bat.behavior != FruitBatBehavior::Roost || bat.simulationTier == WildlifeSimulationTier::Dormant) {
                batStirClock_[i] = 0.0f;
                continue;
            }
            batStirClock_[i] += deltaSeconds;
            const float stirSeconds = 24.0f + static_cast<float>(i) * 3.5f;
            if (batStirClock_[i] >= stirSeconds) {
                batStirClock_[i] = 0.0f;
                ++batStirs_;
                fallingFruit_.DropFrom(fruitBatColony_.Roosts()[i].tree);
                canopyLeafFall_.Shake(bat.x, bat.y + 0.5f, bat.z, static_cast<std::uint32_t>(i) * 7919U + batStirs_ * 15485863U);
            }
        }
        const ButterflyState previousButterflySubject=butterflyMeadow_.Butterflies()[2U];
        Vector3 butterflyObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::ButterflyDetail) {
            const ButterflyState& subject=butterflyMeadow_.Butterflies()[2U];
            butterflyObserver=Vector3(subject.x,subject.y+18.0f,subject.z);
        }
        butterflyMeadow_.SetLyingFruit(fallingFruit_.Fruit());
        const std::array<GrassWalker, GrassWalkers::Slots> butterflyWalkers = GatherWalkers(butterflyObserver.X, butterflyObserver.Z);
        butterflyMeadow_.SetWalkers(butterflyWalkers);
        butterflyMeadow_.SetCloudClock(seaForageSeconds_);
        butterflyMeadow_.SetRain(weather_.State().rain);
        updateProfile_.Mark("butterflyMeadow");
        butterflyMeadow_.Update(deltaSeconds,butterflyObserver.X,butterflyObserver.Y,
            butterflyObserver.Z);
        if(viewpoint_==FixedViewpoint::ButterflyDetail) {
            const ButterflyState& subject=butterflyMeadow_.Butterflies()[2U];
            cameraPosition_+=Vector3(subject.x-previousButterflySubject.x,
                                     subject.y-previousButterflySubject.y,
                                     subject.z-previousButterflySubject.z);
            const Vector3 target(subject.x,subject.y+.015f,subject.z);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        const GeckoState previousGeckoSubject=geckoPopulation_.Geckos()[geckoQaSubjectIndex_];
        Vector3 geckoObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::GeckoDetail) {
            const GeckoState& gecko=geckoPopulation_.Geckos()[geckoQaSubjectIndex_];
            geckoObserver=Vector3(gecko.x,gecko.y+18.0f,gecko.z);
        }
        const std::array<GrassWalker, GrassWalkers::Slots> smallFaunaWalkers = GatherWalkers(cameraPosition_.X, cameraPosition_.Z);
        geckoPopulation_.SetWalkers(smallFaunaWalkers);
        geckoPopulation_.SetFlies(flySwarms_.Flies());
        updateProfile_.Mark("geckoPopulation");
        geckoPopulation_.Update(viewpoint_==FixedViewpoint::GeckoDetail?0.0f:deltaSeconds,
                                geckoObserver.X,geckoObserver.Y,
                                geckoObserver.Z);
        // A gecko's snap scatters the flies it snapped at.
        if(geckoPopulation_.LungeCount()!=notedGeckoLunges_) {
            notedGeckoLunges_=geckoPopulation_.LungeCount();
            const auto& lunge=geckoPopulation_.LastLunge();
            if(lunge.valid) flySwarms_.NoteLunge(lunge.x,lunge.z);
        }
        if(viewpoint_==FixedViewpoint::GeckoDetail) {
            const GeckoState& gecko=geckoPopulation_.Geckos()[geckoQaSubjectIndex_];
            cameraPosition_+=Vector3(gecko.x-previousGeckoSubject.x,
                                     gecko.y-previousGeckoSubject.y,
                                     gecko.z-previousGeckoSubject.z);
            const Vector3 target(gecko.x,gecko.y+.080f,gecko.z);
            const Vector3 direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        Vector3 skinkObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::SkinkDetail) {
            const SkinkState& skink = skinkPopulation_.Skinks()[1U];
            skinkObserver = Vector3(skink.x, skink.y + 18.0f, skink.z);
        }
        skinkPopulation_.SetWalkers(smallFaunaWalkers);
        skinkPopulation_.SetCloudClock(seaForageSeconds_);
        updateProfile_.Mark("skinkPopulation");
        skinkPopulation_.Update(viewpoint_ == FixedViewpoint::SkinkDetail ? 0.0f : deltaSeconds,
                                skinkObserver.X, skinkObserver.Y, skinkObserver.Z);
        Vector3 dragonflyObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::DragonflyDetail){const auto& fly=dragonflySwarm_.Dragonflies()[1U];dragonflyObserver=Vector3(fly.x,fly.y+20,fly.z);}
        updateProfile_.Mark("dragonflySwarm");
        dragonflySwarm_.SetRain(weather_.State().rain);
        dragonflySwarm_.Update(viewpoint_==FixedViewpoint::DragonflyDetail?0.0f:deltaSeconds,
            dragonflyObserver.X,dragonflyObserver.Y,dragonflyObserver.Z);
        updateProfile_.Mark("dragonflyDips");
        dragonflyDips_.Update(deltaSeconds,dragonflySwarm_.LastDip());
        Vector3 frogObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::FrogDetail) {
            const auto& frog = frogPopulation_.Frogs()[1U];
            frogObserver = Vector3(frog.x, frog.y + 18.0f, frog.z);
        }
        // A frog call given: that frog's throat swells and pulses with it.
        if (soundscape_ && soundscape_->PlayedFrogCallCount() != notedFrogCalls_) {
            notedFrogCalls_ = soundscape_->PlayedFrogCallCount();
            frogPopulation_.NoteCall(soundscape_->ActiveFrogCall().animalIndex);
        }
        updateProfile_.Mark("frogPopulation");
        frogPopulation_.Update(viewpoint_ == FixedViewpoint::FrogDetail ? 0.0f : deltaSeconds,
                               frogObserver.X, frogObserver.Y, frogObserver.Z);
        // A frog's dive into the plunge pool scatters the fish holding there.
        if (frogPopulation_.DiveCount() != notedFrogDives_) {
            notedFrogDives_ = frogPopulation_.DiveCount();
            const auto& dive = frogPopulation_.LastDive();
            if (dive.valid) {
                streamFish_.NoteSplash(dive.x, dive.z);
                streamRings_.Note(dive.x, dive.z, 1.0f);
            }
        }
        Vector3 rodentObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::MouseDetail
            || viewpoint_ == FixedViewpoint::RatDetail) {
            const std::size_t subject = viewpoint_ == FixedViewpoint::MouseDetail
                ? 0U : RodentPopulation::MouseCount + 6U;
            const RodentState& rodent = rodentPopulation_.Rodents()[subject];
            rodentObserver = Vector3(rodent.x, rodent.y + 18.0f, rodent.z);
        }
        rodentPopulation_.SetWalkers(smallFaunaWalkers);
        rodentPopulation_.SetLyingFruit(fallingFruit_.Fruit());
        rodentPopulation_.SetSnakes(snakes_->Snakes());
        updateProfile_.Mark("rodentPopulation");
        rodentPopulation_.Update(
            viewpoint_ == FixedViewpoint::MouseDetail
                    || viewpoint_ == FixedViewpoint::RatDetail
                ? 0.0f : deltaSeconds,
            rodentObserver.X, rodentObserver.Y, rodentObserver.Z);
        Vector3 flyObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::FlyDetail) {
            const FlyState& fly = flySwarms_.Flies()[FlySwarms::PerSwarm];
            flyObserver = Vector3(fly.x, fly.y + 12.0f, fly.z);
        }
        flySwarms_.SetLyingFruit(fallingFruit_.Fruit());
        updateProfile_.Mark("flySwarms");
        flySwarms_.Update(viewpoint_ == FixedViewpoint::FlyDetail ? 0.0f : deltaSeconds,
                          flyObserver.X, flyObserver.Y, flyObserver.Z);
        Vector3 antObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::AntDetail) {
            const auto& ant=antTrails_.Ants()[5U];
            antObserver=Vector3(ant.x,ant.y+10.0f,ant.z);
        }
        antTrails_.SetLyingFruit(fallingFruit_.Fruit());
        updateProfile_.Mark("antTrails");
        antTrails_.Update(viewpoint_==FixedViewpoint::AntDetail?0.0f:deltaSeconds,
                          antObserver.X,antObserver.Y,antObserver.Z);
        Vector3 cicadaObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::CicadaDetail) {
            const auto& cicada = cicadaColony_.Cicadas()[1U];
            cicadaObserver = Vector3(cicada.x, cicada.y + 20.0f, cicada.z);
        }
        cicadaColony_.SetWalkers(smallFaunaWalkers);
        updateProfile_.Mark("cicadaColony");
        cicadaColony_.Update(viewpoint_ == FixedViewpoint::CicadaDetail ? 0.0f : deltaSeconds,
                             cicadaObserver.X, cicadaObserver.Y, cicadaObserver.Z);
        Vector3 beetleObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::BeetleDetail) {
            const auto& beetle = beetlePopulation_.Beetles()[2U];
            beetleObserver = Vector3(beetle.x, beetle.y + 14.0f, beetle.z);
        }
        updateProfile_.Mark("beetlePopulation");
        beetlePopulation_.Update(viewpoint_ == FixedViewpoint::BeetleDetail ? 0.0f : deltaSeconds,
                                 beetleObserver.X, beetleObserver.Y, beetleObserver.Z);
        const CrabState previousCrabSubject = crabColony_.Crabs()[crabViewSubject_];
        Vector3 crabObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::CrabDetail) {
            const float radius=std::hypot(previousCrabSubject.x,previousCrabSubject.z);
            const Vector3 outward(previousCrabSubject.x/radius,0,previousCrabSubject.z/radius);
            crabObserver=Vector3(previousCrabSubject.x,previousCrabSubject.y,previousCrabSubject.z)
                +outward*18.0f+Vector3::Up*1.5f;
        }
        crabColony_.SetWalker(resident_->State().x, resident_->State().z, true);
        updateProfile_.Mark("crabColony");
        crabColony_.Update(deltaSeconds, crabObserver.X, crabObserver.Y, crabObserver.Z);
        updateProfile_.Mark("crabBubbles");
        crabBubbles_.Update(deltaSeconds, crabColony_.Crabs());
        Vector3 hermitCrabObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::HermitCrabDetail) {
            const auto& crab = hermitCrabGroup_.Crabs()[3U];
            hermitCrabObserver = Vector3(crab.x, crab.y + 20.0f, crab.z);
        }
        updateProfile_.Mark("hermitCrabGroup");
        hermitCrabGroup_.Update(
            viewpoint_ == FixedViewpoint::HermitCrabDetail ? 0.0f : deltaSeconds,
            hermitCrabObserver.X, hermitCrabObserver.Y, hermitCrabObserver.Z);
        updateProfile_.Mark("shellDrags");
        shellDrags_.Update(deltaSeconds, hermitCrabGroup_.Crabs());
        Vector3 coconutCrabObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::CoconutCrabDetail) {
            const auto& crab = coconutCrabGroup_.Crabs()[2U];
            coconutCrabObserver = Vector3(crab.x, crab.y + 20.0f, crab.z);
        }
        coconutCrabGroup_.SetLyingCoconuts(fallingFruit_.Fruit());
        updateProfile_.Mark("coconutCrabGroup");
        coconutCrabGroup_.Update(
            viewpoint_ == FixedViewpoint::CoconutCrabDetail ? 0.0f : deltaSeconds,
            coconutCrabObserver.X, coconutCrabObserver.Y, coconutCrabObserver.Z);
        for (const std::size_t eaten : coconutCrabGroup_.EatenCoconuts()) fallingFruit_.Consume(eaten);
        // A crab working a coconut: its claws knock on the shell.
        if (soundscape_ && coconutCrabGroup_.KnockCount() != notedClawKnocks_) {
            notedClawKnocks_ = coconutCrabGroup_.KnockCount();
            const auto& knock = coconutCrabGroup_.LastKnock();
            soundscape_->NoteClawKnock(knock.x, knock.y, knock.z);
        }
        if (viewpoint_ == FixedViewpoint::CrabDetail) {
            const CrabState& subject = crabColony_.Crabs()[crabViewSubject_];
            cameraPosition_ += Vector3(subject.x - previousCrabSubject.x,
                                       subject.y - previousCrabSubject.y,
                                       subject.z - previousCrabSubject.z);
        }
        const CrocodileState previousCrocodile=crocodileGroup_.Crocodiles()[2U];
        Vector3 crocodileObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::CrocodileDetail)
            crocodileObserver=Vector3(previousCrocodile.x,previousCrocodile.y+20.0f,
                                      previousCrocodile.z);
        updateProfile_.Mark("crocodileGroup");
        crocodileGroup_.Update(deltaSeconds,crocodileObserver.X,crocodileObserver.Y,
                               crocodileObserver.Z);
        updateProfile_.Mark("crocodileWake");
        crocodileWake_.Update(deltaSeconds,crocodileGroup_.Crocodiles());
        {
            // hauled out on the estuary's sand or mud, the crocodiles drag their bellies and tails
            std::array<bool,CrocodileGroup::CrocodileCount> dry{};
            for(std::size_t i=0;i<crocodileGroup_.Crocodiles().size()&&i<dry.size();++i) {
                const auto& crocodile=crocodileGroup_.Crocodiles()[i];
                const FootstepSurface ground=FootstepGroundAt(crocodile.x,crocodile.z);
                dry[i]=ground==FootstepSurface::Sand||ground==FootstepSurface::Soil;
            }
            crocodileDrags_.Update(deltaSeconds,crocodileGroup_.Crocodiles(),dry);
        }
        // A crocodile coming alert on its bank hisses at the walker.
        if(soundscape_&&crocodileGroup_.LastHiss().count!=notedCrocHisses_) {
            notedCrocHisses_=crocodileGroup_.LastHiss().count;
            const auto& hiss=crocodileGroup_.LastHiss();
            if(hiss.valid) soundscape_->NoteCrocodileHiss(hiss.x,hiss.y,hiss.z);
        }
        if(viewpoint_==FixedViewpoint::CrocodileDetail) {
            const CrocodileState& subject=crocodileGroup_.Crocodiles()[2U];
            cameraPosition_+=Vector3(subject.x-previousCrocodile.x,
                                     subject.y-previousCrocodile.y,
                                     subject.z-previousCrocodile.z);
        }
        boarHerd_.SetLyingFruit(fallingFruit_.Fruit());
        boarHerd_.SetBear(bear_->State().x, bear_->State().z, true);
        if (viewpoint_ == FixedViewpoint::BoarLyingDetail) boarHerd_.PreviewLie(1U);
        updateProfile_.Mark("boarHerd");
        if (UsesBoarCaptureClock())
            boarHerd_.Update(deltaSeconds, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        else
            boarHerd_.Update(viewpoint_ == FixedViewpoint::BoarLyingDetail ? 0.0f : deltaSeconds,
                             cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
        for (const std::size_t eaten : boarHerd_.EatenFruit()) fallingFruit_.Consume(eaten);
        // A boar trotting off drums its hoofbeats.
        if (soundscape_ && boarHerd_.RetreatCount() != notedBoarRetreats_) {
            notedBoarRetreats_ = boarHerd_.RetreatCount();
            const auto& runner = boarHerd_.Boars()[boarHerd_.LastRetreatIndex()];
            soundscape_->NoteHoofbeats(runner.x, runner.y + 0.4f, runner.z, 0U);
        }
        // The boars' hooves print the stream's drainage mud on the wallow trip and any wet
        // sand they cross.
        for(std::size_t i=0;i<boarHerd_.Boars().size()&&i<boarPrints_.size();++i) {
            const auto& b=boarHerd_.Boars()[i];
            const FootstepSurface ground=FootstepGroundAt(b.x,b.z);
            boarPrints_[i].Update(deltaSeconds,b.x,b.z,std::cos(b.headingRadians)*b.speed,std::sin(b.headingRadians)*b.speed,
                b.inChannel||ground==FootstepSurface::Sand||ground==FootstepSurface::Shallows,
                ground==FootstepSurface::Shallows||terrain_.Sample(b.x,b.z).surface==TerrainSurface::WetSand);
        }
        // A boar the bear sends off gives the alarm grunt.
        if (soundscape_ && boarHerd_.BearRetreatCount() != notedBearRetreats_) {
            notedBearRetreats_ = boarHerd_.BearRetreatCount();
            soundscape_->NoteBoarAlarm(boarHerd_.LastBearRetreatIndex());
        }
        updateProfile_.Mark("rootingDust");
        rootingDust_.Update(deltaSeconds, seaForageSeconds_, boarHerd_.Boars());
        updateProfile_.Mark("mudFlecks");
        mudFlecks_.Update(deltaSeconds, boarHerd_.LastShake());
        // The dust the running animals kick up: a boar trotting off, a deer bounding, a goat
        // sidestepping, on dry sand or bare soil, puffs of earth behind the hooves.
        hoofDustRunners_.clear();
        const auto dryUnder=[&](const float x,const float z) {
            const FootstepSurface ground=FootstepGroundAt(x,z);
            return ground==FootstepSurface::Sand||ground==FootstepSurface::Soil;
        };
        for(const auto& b:boarHerd_.Boars())
            hoofDustRunners_.push_back({b.x,b.y,b.z,b.headingRadians,b.speed,b.scale,dryUnder(b.x,b.z)});
        for(const auto& d:deer_->Deer())
            hoofDustRunners_.push_back({d.x,d.y,d.z,d.headingRadians,d.speed,d.scale,dryUnder(d.x,d.z)});
        for(const auto& g:goats_->Goats())
            hoofDustRunners_.push_back({g.x,g.y,g.z,g.heading,g.speed,g.scale,dryUnder(g.x,g.z)});
        updateProfile_.Mark("hoofDust");
        hoofDust_.Update(deltaSeconds, seaForageSeconds_, hoofDustRunners_);
        // The stream rings where a wading walker or animal cuts its water (the quad's wheels
        // ring it from the quad's own update).
        updateProfile_.Mark("streamRings");
        {
            const auto wade = [this](const float x, const float z, const float speed, const float strength) {
                float bed = 0.0f, depth = 0.0f;
                if (speed > 0.2f && StreamWaterAt(x, z, bed, depth) && depth > 0.04f) streamRings_.Note(x, z, strength);
            };
            if (!flightMotion_.IsActive() && !riding_ && playerMotion_.Mode() == LocomotionMode::Walking)
                wade(playerMotion_.Position().x, playerMotion_.Position().z,
                     std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ()), 0.6f);
            for (const auto& b : boarHerd_.Boars()) wade(b.x, b.z, b.speed, 0.5f);
            for (const auto& d : deer_->Deer()) wade(d.x, d.z, d.speed, 0.5f);
            for (const auto& goat : goats_->Goats()) wade(goat.x, goat.z, goat.speed, 0.45f);
            streamRings_.Update(deltaSeconds);
        }
        updateProfile_.Mark("blownSand");
        blownSand_.Update(deltaSeconds, seaForageSeconds_, cameraPosition_.X, cameraPosition_.Z);
        if(viewpoint_==FixedViewpoint::BoarDetail) {
            const auto& current=boarHerd_.Boars().front();
            // Follow both translation and heading. The old camera translated with
            // the subject but retained its original world-space offset, so a normal
            // habitat turn converted the intended front-quarter audit into a rear
            // view and hid the face that this scope is meant to qualify.
            const float c=std::cos(current.headingRadians);
            const float s=std::sin(current.headingRadians);
            const float x=current.x+c*2.8f+s*6.6f;
            const float z=current.z+s*2.8f-c*6.6f;
            cameraPosition_=Vector3(x,
                std::max(terrain_.Height(x,z)+.4f,current.y+2.4f),z);
            const Vector3 target(current.x,current.y+.5f,current.z);
            const auto direction=target-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        const auto previousGoat=goats_->Goats().front();
        const auto previousDeer=deer_->Deer().front();
        SpiderPoint spiderObserver{cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z};
        if(UsesSpiderViewpoint()) {
            // The inspection camera follows the animal's front quarter. Keep the
            // simulation observer overhead so that this capture-only orbit does
            // not make the spider turn away forever instead of demonstrating gait.
            const auto& subject=spiders_->Spiders()[spiderViewSubject_];
            spiderObserver={subject.x,subject.y+3.0f,subject.z};
        }
        updateProfile_.Mark("spiders");
        measureWildlife(0, [&] {
            updateProfile_.Mark("spiders");
            spiders_->Update(deltaSeconds,spiderObserver);
        });
        if(UsesSpiderViewpoint()) {
            const auto& current=spiders_->Spiders()[spiderViewSubject_];
            const Vector3 forward(std::cos(current.heading),0,std::sin(current.heading));
            const Vector3 right(forward.Z,0,-forward.X);
            cameraPosition_=Vector3(current.x,current.y+.24f,current.z)+forward*.38f+right*.30f;
            const auto direction=Vector3(current.x,current.y+.035f,current.z)-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        // The goat and boar eight-stage capture clocks exist to evidence gait and foraging
        // from a close QA camera.  Since animals now stand and face a nearby watcher, those
        // sequences treat the QA camera as absent; ordinary play keeps the camera as observer
        // and the facing/oblique-retreat behaviour is gated by IslandCloseObserverTurningTests.
        const GoatPoint goatObserver=UsesGoatCaptureClock()
            ?GoatPoint{std::numeric_limits<float>::quiet_NaN(),0,0}
            :GoatPoint{cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z};
        if(viewpoint_==FixedViewpoint::GoatLyingDetail) goats_->PreviewLie(1U);
        updateProfile_.Mark("goats");
        measureWildlife(1, [&] { goats_->Update(viewpoint_==FixedViewpoint::GoatLyingDetail?0.0f:deltaSeconds,goatObserver); });
        // The sentinel coming alert at the walker gives the alarm bleat.
        if(soundscape_&&goats_->SentinelAlert()&&!goatSentinelWasAlert_) soundscape_->NoteGoatAlarm(0U);
        goatSentinelWasAlert_=goats_->SentinelAlert();
        // The goats' cloven hooves print the wet soil round their basin as they come to drink
        // (the rest of their dry slope takes none).
        if(goats_->WaterholeSite().valid) {
            const auto& basin=goats_->WaterholeSite();
            for(std::size_t i=0;i<goats_->Goats().size()&&i<goatPrints_.size();++i) {
                // (under the forefeet, which come to the water's edge as the goat drinks)
                const auto& goat=goats_->Goats()[i];
                const float footX=goat.x+std::cos(goat.heading)*0.4f*goat.scale,footZ=goat.z+std::sin(goat.heading)*0.4f*goat.scale;
                const float fromBasin=std::hypot(footX-basin.x,footZ-basin.z);
                const bool wetSoil=fromBasin>GoatHerd::WaterholeRadius*0.85f&&fromBasin<GoatHerd::WaterholeRadius*1.5f;
                goatPrints_[i].Update(deltaSeconds,footX,footZ,std::cos(goat.heading)*goat.speed,
                    std::sin(goat.heading)*goat.speed,wetSoil,false);
            }
        }
        if(viewpoint_==FixedViewpoint::GoatDetail) {
            const auto& current=goats_->Goats().front();
            cameraPosition_+=Vector3(current.x-previousGoat.x,current.y-previousGoat.y,current.z-previousGoat.z);
        }
        updateProfile_.Mark("deer");
        deer_->Update(viewpoint_==FixedViewpoint::DeerDetail||viewpoint_==FixedViewpoint::DeerBeddedDetail?0.0f:deltaSeconds,
            cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z);
        // The deer sentinel coming alert at the walker snorts the alarm.
        if(soundscape_&&deer_->SentinelAlert()&&!deerSentinelWasAlert_&&!deer_->Deer().empty()) {
            const auto& sentinel=deer_->Deer().front();
            soundscape_->NoteDeerSnort(sentinel.x,sentinel.y+1.1f*sentinel.scale,sentinel.z);
        }
        deerSentinelWasAlert_=deer_->SentinelAlert();
        // A deer bounding off drums its hoofbeats, lighter and quicker than a boar's.
        if (soundscape_ && deer_->BoltCount() != notedDeerBolts_) {
            notedDeerBolts_ = deer_->BoltCount();
            const auto& bolter = deer_->Deer()[deer_->LastBoltIndex()];
            soundscape_->NoteHoofbeats(bolter.x, bolter.y + 0.6f, bolter.z, 1U);
        }
        // The deer's hooves press cloven prints into wet sand and the shallows' bed as they
        // walk (the stream bank they drink at), a stride apart.
        for(std::size_t i=0;i<deer_->Deer().size()&&i<deerPrints_.size();++i) {
            const auto& d=deer_->Deer()[i];
            const FootstepSurface ground=FootstepGroundAt(d.x,d.z);
            float bankX=0.0f,bankZ=0.0f;
            const bool atBank=deer_->DrinkPoint(i,bankX,bankZ)&&std::hypot(d.x-bankX,d.z-bankZ)<3.0f;
            deerPrints_[i].Update(deltaSeconds,d.x,d.z,std::cos(d.headingRadians)*d.speed,std::sin(d.headingRadians)*d.speed,
                atBank||ground==FootstepSurface::Sand||ground==FootstepSurface::Shallows,
                ground==FootstepSurface::Shallows||terrain_.Sample(d.x,d.z).surface==TerrainSurface::WetSand);
        }
        if(viewpoint_==FixedViewpoint::DeerDetail||viewpoint_==FixedViewpoint::DeerBeddedDetail) {
            const auto& current=deer_->Deer().front();
            cameraPosition_+=Vector3(current.x-previousDeer.x,current.y-previousDeer.y,
                                     current.z-previousDeer.z);
        }
        const auto snakeCentre = [this]() {
            Vector3 centre = Vector3::Zero;
            const auto population = viewpoint_==FixedViewpoint::TreeSnakeDetail
                ? snakes_->TreeSnakes() : snakes_->Snakes();
            if(viewpoint_==FixedViewpoint::SnakeHeadDetail) {
                const auto& head=population.front().body.front();
                return Vector3(head.x,head.y,head.z);
            }
            for (const auto& p : population.front().body) centre += Vector3(p.x,p.y,p.z);
            return centre/static_cast<float>(SnakeState::StationCount);
        };
        const auto previousSnakeCentre = snakeCentre();
        Vector3 snakeObserver=cameraPosition_;
        if(viewpoint_==FixedViewpoint::SnakeHeadDetail) {
            // The anatomy camera sits inside the normal 8 m retreat radius. Drive this
            // isolated QA view with a nearby but non-threatening virtual observer so a
            // constrained retreat cannot freeze the very motion sequence being reviewed.
            const auto& subject=snakes_->Snakes().front();
            const Vector3 head(subject.body[0].x,subject.body[0].y,subject.body[0].z);
            const Vector3 neck(subject.body[1].x,subject.body[1].y,subject.body[1].z);
            snakeObserver=head+Vector3::Normalize(head-neck)*12.0f+Vector3::Up*1.7f;
        }
        updateProfile_.Mark("snakes");
        measureWildlife(2, [&] {
            updateProfile_.Mark("snakes");
            snakes_->Update(deltaSeconds,snakeObserver.X,snakeObserver.Y,snakeObserver.Z);
        });
        // The head view holds the subject's tongue out so the fork can be reviewed.
        if(viewpoint_==FixedViewpoint::SnakeHeadDetail) snakes_->PreviewTongue(0U);
        if (viewpoint_ == FixedViewpoint::SnakeDetail
                || viewpoint_==FixedViewpoint::TreeSnakeDetail) {
            // Follow translation only: preserve the viewing direction and live retreat behavior.
            // Software frames can span many simulation updates; otherwise the close subject exits.
            cameraPosition_ += snakeCentre()-previousSnakeCentre;
        } else if(viewpoint_==FixedViewpoint::SnakeHeadDetail) {
            // Follow both translation and heading so the close audit continues to see the
            // moving face rather than drifting into foliage as the live snake turns.
            const auto& subject=snakes_->Snakes().front();
            const Vector3 head(subject.body[0].x,subject.body[0].y,subject.body[0].z);
            const Vector3 neck(subject.body[1].x,subject.body[1].y,subject.body[1].z);
            const Vector3 forward=Vector3::Normalize(head-neck);
            const Vector3 right=Vector3::Normalize(Vector3::Cross(Vector3::Up,forward));
            cameraPosition_=head+forward*.38f+right*.25f+Vector3::Up*.10f;
            const Vector3 direction=head+Vector3::Up*.012f-cameraPosition_;
            yaw_=std::atan2(direction.X,direction.Z);
            pitch_=std::atan2(direction.Y,std::hypot(direction.X,direction.Z));
        }
        const FishState previousFishSubject = fishSchools_.Fish().front();
        {
            const SharkState& shark = sharkPatrol_.State();
            fishSchools_.SetThreat(shark.x, shark.y, shark.z, shark.present && shark.visible);
        }
        updateProfile_.Mark("fishSchools");
        fishSchools_.Update(deltaSeconds, cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
        if (!reefFish_) reefFish_ = std::make_unique<ReefFishField>(naturalPropLayout_.ReefHabitat());
        reefFish_->Update(deltaSeconds, cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
        // A fish breaking the surface draws the shark that way.
        if (fishSchools_.LastBreach().count != notedSharkBreach_) {
            notedSharkBreach_ = fishSchools_.LastBreach().count;
            sharkPatrol_.NoteBreach(fishSchools_.LastBreach().x, fishSchools_.LastBreach().z);
        }
        if (viewpoint_ == FixedViewpoint::FishDetail) {
            // Follow translation only so a close CPU-rendered audit retains the subject while
            // still exposing its live heading, banking, tail bend and school movement.
            const FishState& subject = fishSchools_.Fish().front();
            cameraPosition_ += Vector3(subject.x - previousFishSubject.x,
                                       subject.y - previousFishSubject.y,
                                       subject.z - previousFishSubject.z);
        }
        const StingrayState previousStingraySubject=stingrayGroup_.Stingrays().front();
        // Keep the focused anatomy frame symmetric; the deterministic CPU test separately
        // exercises the full undulating glide over 120 seconds.
        {
            const SharkState& shark=sharkPatrol_.State();
            stingrayGroup_.SetThreat(shark.x,shark.z,shark.present&&shark.visible);
        }
        if(viewpoint_==FixedViewpoint::StingrayBuriedDetail) stingrayGroup_.PreviewBury(2U);
        updateProfile_.Mark("stingrayGroup");
        stingrayGroup_.Update(viewpoint_==FixedViewpoint::StingrayDetail||viewpoint_==FixedViewpoint::StingrayBuriedDetail?0.0f:deltaSeconds,
                              cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z);
        if (viewpoint_ == FixedViewpoint::ReefHeronDetail) reefHeronGroup_.PreviewFreeze(0U);
        {
            // What else a wading heron will not let near: the resident on the shore (closer
            // when they walk), the quad while its engine runs, the bear, the boars, and a
            // crocodile on the move.
            std::vector<ReefHeronGroup::Disturber> disturbers;
            const auto& resident = resident_->State();
            disturbers.push_back({resident.x, resident.z, resident.speed > 0.2f ? 11.0f : 6.0f});
            if (quadEngineOn_) {
                const auto quad = quad_.Position();
                disturbers.push_back({quad.x, quad.z, std::abs(quad_.Speed()) > 1.0f ? 30.0f : 16.0f});
            }
            disturbers.push_back({bear_->State().x, bear_->State().z, 14.0f});
            for (const auto& boar : boarHerd_.Boars()) disturbers.push_back({boar.x, boar.z, 8.0f});
            for (const auto& crocodile : crocodileGroup_.Crocodiles())
                if (crocodile.speed > 0.05f) disturbers.push_back({crocodile.x, crocodile.z, 7.0f});
            reefHeronGroup_.SetDisturbers(disturbers);
        }
        // the QA camera stands close: it must not flush the bird it frames
        updateProfile_.Mark("reefHeronGroup");
        reefHeronGroup_.Update(deltaSeconds, cameraPosition_.X,
                               viewpoint_ == FixedViewpoint::ReefHeronDetail ? cameraPosition_.Y + 100.0f : cameraPosition_.Y,
                               cameraPosition_.Z);
        updateProfile_.Mark("stingraySand");
        stingraySand_.Update(deltaSeconds,stingrayGroup_.LastBurst());
        // The sand a settling ray flicks over its back rises in the same puffs.
        updateProfile_.Mark("stingrayFlickSand");
        stingrayFlickSand_.Update(deltaSeconds,stingrayGroup_.LastFlick());
        if(viewpoint_==FixedViewpoint::StingrayDetail) {
            const StingrayState& subject=stingrayGroup_.Stingrays().front();
            cameraPosition_+=Vector3(subject.x-previousStingraySubject.x,
                                     subject.y-previousStingraySubject.y,
                                     subject.z-previousStingraySubject.z);
        }
        const auto previousOctopus=octopusGroup_.Octopuses().front();
        updateProfile_.Mark("octopusGroup");
        octopusGroup_.Update(viewpoint_==FixedViewpoint::OctopusDetail?0.0f:deltaSeconds,cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z);
        updateProfile_.Mark("octopusInk");
        octopusInk_.Update(deltaSeconds,octopusGroup_.LastInk());
        if(viewpoint_==FixedViewpoint::OctopusDetail){const auto& o=octopusGroup_.Octopuses().front();cameraPosition_+=Vector3(o.x-previousOctopus.x,o.y-previousOctopus.y,o.z-previousOctopus.z);}
        const SeaTurtleState previousTurtleSubject = seaTurtleGroup_.Turtles()[2U];
        {
            const SharkState& shark = sharkPatrol_.State();
            seaTurtleGroup_.SetThreat(shark.x, shark.z, shark.present && shark.visible);
        }
        updateProfile_.Mark("seaTurtleGroup");
        seaTurtleGroup_.Update(
            deltaSeconds, cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
        updateProfile_.Mark("turtleBreaths");
        turtleBreaths_.Update(deltaSeconds,seaForageSeconds_,seaTurtleGroup_.LastBreath());
        updateProfile_.Mark("turtleGraze");
        turtleGraze_.Update(deltaSeconds,seaTurtleGroup_.LastBite());
        if (viewpoint_ == FixedViewpoint::SeaTurtleDetail) {
            // Follow translation only. A CPU-rendered 60-frame audit spans enough simulation
            // time for the close subject to leave an otherwise fixed camera.
            const SeaTurtleState& subject = seaTurtleGroup_.Turtles()[2U];
            cameraPosition_ += Vector3(subject.x - previousTurtleSubject.x,
                                       subject.y - previousTurtleSubject.y,
                                       subject.z - previousTurtleSubject.z);
        }
        const SharkState previousSharkSubject = sharkPatrol_.State();
        updateProfile_.Mark("sharkPatrol");
        sharkPatrol_.Update(deltaSeconds, cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
        updateProfile_.Mark("finWake");
        finWake_.Update(deltaSeconds, sharkPatrol_.State());
        if (viewpoint_ == FixedViewpoint::SharkDetail) {
            // Keep the occasional close subject framed without freezing its live patrol pose.
            const SharkState& subject = sharkPatrol_.State();
            cameraPosition_ += Vector3(subject.x - previousSharkSubject.x,
                                       subject.y - previousSharkSubject.y,
                                       subject.z - previousSharkSubject.z);
        }
        {
            const SharkState& shark=sharkPatrol_.State();
            dolphinPod_.SetThreat(shark.x,shark.z,shark.present&&shark.visible);
        }
        updateProfile_.Mark("dolphinPod");
        dolphinPod_.Update(deltaSeconds,cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z);
        // The pod's leap tells the seabirds where the fish are.
        if(dolphinPod_.LeapCount()!=notedDolphinLeaps_) {
            notedDolphinLeaps_=dolphinPod_.LeapCount();
            const auto& leap=dolphinPod_.LastLeap();
            if(leap.valid) birdFlock_.NoteDolphinLeap(leap.x,leap.z);
        }
        if (viewpoint_ == FixedViewpoint::DolphinDetail) {
            // Preserve a broadside audit while retaining live pitch, bank, fluke beat and
            // state motion; translation-only following eventually turned the flank away.
            const DolphinState& subject = dolphinPod_.Dolphins().front();
            const Vector3 forward(std::cos(subject.headingRadians), 0.0f,
                                  std::sin(subject.headingRadians));
            const Vector3 side(-forward.Z, 0.0f, forward.X);
            cameraPosition_ = Vector3(subject.x, subject.y, subject.z)
                + side * 3.7f - forward * .45f + Vector3::Up * .35f;
            const Vector3 toDolphin = Vector3(subject.x, subject.y, subject.z)
                + forward * .12f - cameraPosition_;
            yaw_ = std::atan2(toDolphin.X, toDolphin.Z);
            pitch_ = std::atan2(toDolphin.Y, std::hypot(toDolphin.X, toDolphin.Z));
        }
        Vector3 monkeyObserver = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::MonkeyDetail) {
            const MonkeyState& subject = monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
            Vector3 direction(cameraPosition_.X - subject.x, 0.0f,
                              cameraPosition_.Z - subject.z);
            if (direction.LengthSquared() > 0.0001f) {
                direction.Normalize();
                monkeyObserver = Vector3(subject.x, cameraPosition_.Y, subject.z)
                    + direction * 12.0f;
            }
        }
        const MonkeyState previousMonkeySubject = monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
        if (viewpoint_ != FixedViewpoint::MonkeyTreeDetail && viewpoint_ != FixedViewpoint::MonkeyClimbDetail) {
            measureWildlife(3, [&] {
                monkeyTroop_.SetLyingFruit(fallingFruit_.Fruit());
                monkeyTroop_.SetSnakes(snakes_->Snakes());
                updateProfile_.Mark("monkeyTroop");
                monkeyTroop_.Update(deltaSeconds, monkeyObserver.X, monkeyObserver.Y, monkeyObserver.Z,
                    snakeCaptureClock ? static_cast<float>(snakeCaptureUpdates_)*0.1f
                        : monkeyCaptureClock ? static_cast<float>(monkeyCaptureUpdates_)*0.1f
                        : static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()));
                if (viewpoint_ == FixedViewpoint::MonkeyScratchDetail) monkeyTroop_.PreviewScratch(monkeyQaSubjectIndex_);
                // A monkey breaking into flight from the walker gives the alarm.
                if (soundscape_ && monkeyTroop_.FleeCount() != notedMonkeyFlees_) {
                    notedMonkeyFlees_ = monkeyTroop_.FleeCount();
                    const MonkeyTroop::MonkeyFlee& flee = monkeyTroop_.LastFlee();
                    if (flee.valid) soundscape_->NoteMonkeyAlarm(flee.x, flee.y, flee.z);
                }
                for (const std::size_t eaten : monkeyTroop_.EatenFruit()) fallingFruit_.Consume(eaten);
            });
        }
        if (viewpoint_ == FixedViewpoint::MonkeyDetail) {
            // Keep the animated subject in the close QA view even when software rendering
            // requires many simulation updates between captured frames. Preserve the original
            // viewing direction rather than orbiting whenever the animal changes heading.
            const MonkeyState& subject = monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
            cameraPosition_ += Vector3(subject.x - previousMonkeySubject.x,
                                       subject.y - previousMonkeySubject.y,
                                       subject.z - previousMonkeySubject.z);
        }
        updateProfile_.Mark("UpdateSoundscape");
        UpdateSoundscape(deltaSeconds);
        if (soundscape_ && worldDebugVisible_) {
            soundscape_->RefreshDebugSources(
                cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z,
                birdFlock_.Birds(), monkeyTroop_.Monkeys());
        }
        if (inputDiagnostics_) {
            inputDiagnosticsTimer_ += deltaSeconds;
            if (inputDiagnosticsTimer_ >= 0.25f) {
                inputDiagnosticsTimer_ = 0.0f;
                const PlayerPosition& position = playerMotion_.Position();
                const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
                std::cout << "island-simulator: input sample captured "
                          << (mouseCaptured_ ? 1 : 0) << " relative "
                          << (Mouse::getIsRelativeMouseModeEXTProperty() ? 1 : 0)
                          << " player " << position.x << ' ' << position.z
                          << " camera " << cameraPosition_.X << ' '
                          << cameraPosition_.Y << ' ' << cameraPosition_.Z
                          << " free " << (IsDebugFreeCameraActive() ? 1 : 0)
                          << " yaw " << yaw_ << " pitch " << pitch_
                          << " viewport " << viewport.getWidthProperty() << ' '
                          << viewport.getHeightProperty()
                          << " run " << (playerMotion_.IsRunning() ? 1 : 0)
                          << " velocity " << playerMotion_.VelocityX() << ' '
                          << playerMotion_.VelocityZ()
                          << " grass " << (grassRenderer_ ? grassRenderer_->VisibleNearPatchCount() : 0)
                          << ' ' << (grassRenderer_ ? grassRenderer_->VisibleMiddlePatchCount() : 0)
                          << ' ' << (grassRenderer_ ? grassRenderer_->VisibleFarPatchCount() : 0)
                          << " cells " << (grassRenderer_ ? grassRenderer_->VisibleCellCount() : 0)
                          << " draws " << (grassRenderer_ ? grassRenderer_->DrawCallCount() : 0);
                if (jungleRenderer_) jungleRenderer_->WriteVegetationLodDiagnostics(std::cout);
                else std::cout << " canopy 0 0 trees none shrubs 0 0 bushes none";
                std::cout << '\n';
                if (soundscape_) {
                    const auto audio = soundscape_->DebugState();
                    std::cout << "island-simulator: audio sample available " << soundscape_->IsAvailable()
                        << " muted " << soundscape_->IsMuted()
                        << " boars " << soundscape_->PlayedBoarCallCount()
                        << " sources " << audio.playingSourceCount
                        << " wildlife " << static_cast<int>(audio.activeWildlifeCall.type)
                        << " snake-hisses " << soundscape_->PlayedSnakeHissCount()
                        << " goats " << soundscape_->PlayedGoatCallCount()
                        << " frogs " << soundscape_->PlayedFrogCallCount()
                        << " cicadas " << soundscape_->PlayedCicadaCallCount()
                        << " underwater " << audio.underwaterAudio.immersion
                        << " air-gain " << audio.underwaterAudio.airGain
                        << " water-gain " << audio.underwaterAudio.waterGain
                        << " water-pitch-offset " << audio.underwaterAudio.waterPitchOffset << '\n';
                    const auto fire=campfireLayout_.Centre();
                    const float distance=std::hypot(cameraPosition_.X-fire.x,cameraPosition_.Y-fire.y,cameraPosition_.Z-fire.z);
                    const float clearance=std::hypot(position.x-fire.x,position.z-fire.z)
                        -CampfireSite::PitRadius-PlayerMotion::CapsuleRadius;
                    std::cout<<"island-simulator: fire audio available "<<soundscape_->IsAvailable()
                        <<" muted "<<soundscape_->IsMuted()<<" playing "<<audio.campfirePlaying
                        <<" gain "<<audio.campfireVolume<<" distance "<<distance<<" clearance "<<clearance
                        <<" sources "<<audio.playingSourceCount<<'\n';
                }
                if(terrainRenderer_&&terrainRenderer_->StreamBed().Contains(position.x,position.z)) {
                    const auto stream=StreamSoundField::Sample(position.x,position.z);
                    float support=-std::numeric_limits<float>::infinity();
                    for(const auto offset:PlayerMotion::GroundContactOffsets)
                        support=std::max(support,terrainRenderer_->StreamBed().SampleHeight(
                            position.x+offset.x*PlayerMotion::CapsuleRadius,
                            position.z+offset.z*PlayerMotion::CapsuleRadius));
                    std::cout<<"island-simulator: stream crossing position "
                        <<position.x<<' '<<position.y<<' '<<position.z
                        <<" signed-across "<<position.z-stream.z
                        <<" route-distance "<<stream.distance
                        <<" support-error "<<position.y-PlayerMotion::EyeHeight-support
                        <<" mode "<<static_cast<int>(playerMotion_.Mode())<<'\n';
                }
                if (flightMotion_.IsActive())
                    std::cout << "island-simulator: flight position " << cameraPosition_.X << ' '
                              << cameraPosition_.Y << ' ' << cameraPosition_.Z
                              << " above-ground " << flightMotion_.HeightAboveGround() << '\n';
                if(terrainRenderer_&&CaveSite::Reserves(cameraPosition_.X,cameraPosition_.Z,15)) {
                    const auto& cave=terrainRenderer_->Cave();
                    const CavePoint feet{cameraPosition_.X,cameraPosition_.Y-PlayerMotion::EyeHeight,cameraPosition_.Z};
                    const auto local=cave.Local(feet);
                    std::cout<<"island-simulator: cave sample local "<<local.x<<' '<<local.y<<' '<<local.z
                        <<" flight "<<flightMotion_.IsActive()<<" solid "
                        <<cave.IntersectsCapsule(feet,PlayerMotion::CapsuleRadius,PlayerMotion::CapsuleHeight)<<'\n';
                }
                if(playerMotion_.Mode()==LocomotionMode::Swimming && !flightMotion_.IsActive()) {
                    const auto p=playerMotion_.Position();
                    std::cout << "island-simulator: swim position " << p.x << ' ' << p.y << ' ' << p.z
                        << " depth " << playerMotion_.SubmergedDepth() << '\n';
                }
                std::cout << std::flush;
            }
        }
        updateProfile_.End();
        ++runtimeTimingUpdates_;
        runtimeUpdateMilliseconds_ += std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - updateStarted).count();
    }

    void IslandGame::RecordRuntimeFrameTiming(const std::chrono::steady_clock::time_point drawStarted)
    {
        if (smokeTest_ || !benchmarkPath_.empty()) return;
        const auto now = std::chrono::steady_clock::now();
        const auto reset = [&] {
            runtimeTimingWindowStarted_ = now;
            runtimeTimingFrames_ = runtimeTimingUpdates_ = 0;
            runtimeUpdateMilliseconds_ = runtimeDrawMilliseconds_ = 0;
            runtimeWildlifeMilliseconds_.fill(0);
            updateProfile_.Reset();
            RenderPassCpuProfile().Reset();
        };
        if (runtimeTimingWindowStarted_ == std::chrono::steady_clock::time_point{}) {
            reset(); // Exclude world loading and the first shader compilation from play timings.
            return;
        }
        ++runtimeTimingFrames_;
        runtimeDrawMilliseconds_ += std::chrono::duration<double, std::milli>(now-drawStarted).count();
        const double seconds = std::chrono::duration<double>(now-runtimeTimingWindowStarted_).count();
        if (seconds < 5.0) return;
        const double frames = static_cast<double>(runtimeTimingFrames_);
        const double updates = static_cast<double>(std::max<std::uint64_t>(1, runtimeTimingUpdates_));
        std::ostringstream report;
        report << std::fixed << std::setprecision(3)
               << "island-simulator: frame-window seconds=" << seconds
               << " frames=" << runtimeTimingFrames_ << " fps=" << frames/seconds
               << " updates-per-frame=" << static_cast<double>(runtimeTimingUpdates_)/frames
               << " update-ms-per-frame=" << runtimeUpdateMilliseconds_/frames
               << " draw-submit-ms-per-frame=" << runtimeDrawMilliseconds_/frames
               << " wildlife-ms-per-update(spider,goat,snake,monkey)="
               << runtimeWildlifeMilliseconds_[0]/updates << ','
               << runtimeWildlifeMilliseconds_[1]/updates << ','
               << runtimeWildlifeMilliseconds_[2]/updates << ','
               << runtimeWildlifeMilliseconds_[3]/updates
               << " flight=" << flightMotion_.IsActive()
               << " camera=" << cameraPosition_.X << ',' << cameraPosition_.Y << ',' << cameraPosition_.Z;
        if (soundscape_) {
            report << " audio-available=" << soundscape_->IsAvailable()
                   << " audio-muted=" << soundscape_->IsMuted()
                   << " audio-playing=" << soundscape_->DebugState().playingSourceCount;
        }
        std::cout << report.str() << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(report.str());
        // Where the time goes: the costliest parts of an Update (ms per Update) and of a frame's
        // render passes (CPU ms per drawn frame; a pass that waits on the GPU shows that too).
        std::ostringstream profile;
        profile << "island-simulator: frame-profile update-ms-per-update "
                << updateProfile_.Top(updates, 14) << " | render-pass-cpu-ms-per-frame "
                << RenderPassCpuProfile().Top(frames, 16);
        std::cout << profile.str() << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(profile.str());
        reset();
    }

    void IslandGame::UpdateQuadSite()
    {
        const BikeVec p = quad_.Position();
        float floor = 0.0f;
        // The wheels touch about 0.6 m below the body's origin; the bunker's floor plan answers
        // only within 1.15 m of that, so the ridge over its rooms stays ordinary ground.
        if (bunkerLayout_.SupportHeight(p.x, p.z, p.y - 0.6f, floor)) {
            quadSite_ = QuadSite::Bunker;
            quadSiteFloorY_ = floor;
        } else if (terrainRenderer_ && terrainRenderer_->CaveGround().Contains(p.x, p.z)) {
            quadSite_ = QuadSite::Cave;
            quadSiteFloorY_ = terrainRenderer_->CaveGround().SampleHeight(p.x, p.z);
        } else {
            quadSite_ = QuadSite::Open;
        }
    }

    bool IslandGame::QuadOnShelterFloor(const float x, const float z) const noexcept
    {
        return quadSite_ != QuadSite::Open
            || (terrainRenderer_ && terrainRenderer_->CaveGround().Contains(x, z))
            || (jettyPlacement_ && jettyPlacement_->OnDeck(x, z));   // the planks, not the sand under them
    }

    bool IslandGame::QuadWallBlocked(const float x, const float z) const noexcept
    {
        // The bunker: the quad's footprint (its corners and the middles of its sides, as it is
        // heading) must lie in the floor plan's walkable rooms once past the portal plane. Its
        // capsule test is the walker's (0.3 m round): a capsule as wide as the quad never fit
        // through the portal, its tail still outside the plan while its middle was in.
        const BunkerPoint centre = bunkerLayout_.Local({x, 0.0f, z});
        const float floorHere = bunkerLayout_.EntranceGroundY() + bunkerLayout_.FloorRelative(centre.z);
        if (centre.z > -2.5f && centre.z < 37.5f && std::abs(centre.x) < 12.0f
            && std::abs(quad_.Position().y - 0.6f - floorHere) < 1.15f) {
            const BikeVec forward = quad_.Forward();
            const float length = std::hypot(forward.x, forward.z);
            const float fx = length > 1e-4f ? forward.x / length : 0.0f, fz = length > 1e-4f ? forward.z / length : 1.0f;
            for (const float along : {-1.0f, 0.0f, 1.0f}) {
                for (const float across : {-0.62f, 0.0f, 0.62f}) {
                    if (along == 0.0f && across == 0.0f) continue;
                    const float px = x + fx * along + fz * across, pz = z + fz * along - fx * across;
                    if (bunkerLayout_.Local({px, 0.0f, pz}).z < -0.35f) continue;   // still outside
                    if (!bunkerLayout_.ContainsWalkable(px, pz, 0.0f)) return true;
                }
            }
        }
        // The helicopter's airframe (its doorways are a walker's, not a quad's), and the rope
        // bridge with its end posts: its plank walk is narrower than the quad's track.
        if (helicopterPlacement_ && helicopterPlacement_->IntersectsCapsule(x, z, 0.95f)) return true;
        if (bridgePlacement_) {
            const auto& bridge = *bridgePlacement_;
            const float dx = x - bridge.ax, dz = z - bridge.az;
            const float along = dx * bridge.dirX + dz * bridge.dirZ;
            const float across = dx * bridge.AcrossX() + dz * bridge.AcrossZ();
            if (along > -1.1f && along < bridge.span + 1.1f
                && std::abs(across) < BridgePlacement::PostAcross + 0.95f) return true;
        }
        // The lookout's ramp where it runs low enough to strike (under its high end and the
        // platform the quad and its rider pass): the footprint's corners and side middles.
        if (lookoutPlacement_) {
            const BikeVec forward = quad_.Forward();
            const float length = std::hypot(forward.x, forward.z);
            const float fx = length > 1e-4f ? forward.x / length : 0.0f, fz = length > 1e-4f ? forward.z / length : 1.0f;
            for (const float along : {-1.0f, 0.0f, 1.0f}) {
                for (const float across : {-0.62f, 0.0f, 0.62f}) {
                    const float px = x + fx * along + fz * across, pz = z + fz * along - fx * across;
                    if (!lookoutPlacement_->OnRamp(px, pz)) continue;
                    const float below = QuadGround(px, pz).height;
                    if (lookoutPlacement_->SupportHeight(px, pz, -1.0e6f) - below < 1.9f) return true;
                }
            }
        }
        // The cave's rock (an arched shell five metres wide): the quad and its rider as one
        // upright capsule, 0.95 m round and 1.9 m tall.
        constexpr float Radius = 0.95f, Height = 1.9f;
        const float feet = QuadGround(x, z).height + 0.03f;
        return terrainRenderer_ && terrainRenderer_->Cave().IntersectsCapsule({x, feet, z}, Radius, Height);
    }

    BikeGround IslandGame::QuadGround(const float x, const float z) const noexcept
    {
        BikeGround ground;
        // In the bunker its ramp and rooms are the ground, from its floor plan, and in the cave
        // its excavated floor (beyond which, while inside, the floor under the quad goes on
        // under the hull's points that reach into the rock).
        // (Behind the portal plane the hillside is still the ground: the plan's gentler apron
        // there lifted the rear wheels as the front went in, and the quad nosed over the ramp.)
        if (quadSite_ == QuadSite::Bunker
            && bunkerLayout_.Local({x, bunkerLayout_.EntranceGroundY(), z}).z >= -0.35f) {
            const auto floorAt = [this](const float px, const float pz) {
                const auto local = bunkerLayout_.Local({px, bunkerLayout_.EntranceGroundY(), pz});
                return bunkerLayout_.EntranceGroundY() + bunkerLayout_.FloorRelative(local.z);
            };
            ground.height = floorAt(x, z);
            const float slopeX = (floorAt(x + 0.25f, z) - floorAt(x - 0.25f, z)) / 0.5f;
            const float slopeZ = (floorAt(x, z + 0.25f) - floorAt(x, z - 0.25f)) / 0.5f;
            const float length = std::sqrt(slopeX * slopeX + 1.0f + slopeZ * slopeZ);
            ground.normal = {-slopeX / length, 1.0f / length, -slopeZ / length};
            return ground;
        }
        if (terrainRenderer_ && (quadSite_ == QuadSite::Cave || terrainRenderer_->CaveGround().Contains(x, z))) {
            const CaveGroundPatch& cave = terrainRenderer_->CaveGround();
            const auto heightAt = [&](const float px, const float pz) {
                if (cave.Contains(px, pz)) return cave.SampleHeight(px, pz);
                if (quadSite_ == QuadSite::Cave) return quadSiteFloorY_;
                return DrawnOrAnalyticHeight(*drawnGround_, terrain_, px, pz);
            };
            ground.height = heightAt(x, z);
            const float slopeX = (heightAt(x + 0.25f, z) - heightAt(x - 0.25f, z)) / 0.5f;
            const float slopeZ = (heightAt(x, z + 0.25f) - heightAt(x, z - 0.25f)) / 0.5f;
            const float length = std::sqrt(slopeX * slopeX + 1.0f + slopeZ * slopeZ);
            ground.normal = {-slopeX / length, 1.0f / length, -slopeZ / length};
            ground.grip = 1.1f;   // packed earth and grit
            return ground;
        }
        const float extent = TerrainMeshLayout::Extent - 1.0f;
        if (quadSurface_ && std::isfinite(x) && std::isfinite(z) && std::abs(x) < extent && std::abs(z) < extent) {
            const auto sample = quadSurface_->Sample(x, z);
            ground.height = sample.height;
            ground.normal = {sample.normal.x, sample.normal.y, sample.normal.z};
        } else {
            ground.height = terrain_.Height(std::clamp(x, -900.0f, 900.0f), std::clamp(z, -900.0f, 900.0f));
        }
        // the stream's water over its bed (which the surface gives in its channel)
        float streamBed = 0.0f, streamDepth = 0.0f;
        (void)StreamWaterAt(x, z, streamBed, streamDepth);
        // The jetty's deck carries the quad as it does the walker, up its ramp from the sand
        // (off its side, the sea).
        if (jettyPlacement_) {
            const float deck = jettyPlacement_->SupportHeight(x, z, ground.height);
            if (deck > ground.height) {
                ground.height = deck;
                ground.normal = {0.0f, 1.0f, 0.0f};
            }
        }
        // Tyre grip by the ground, less in the wet: loose sand, firm soil, grippy rock. The kind
        // is the analytic terrain's at the centre of the quarter-metre cell, remembered: a full
        // terrain sample (five height evaluations and the coast) for each of the fourteen ground
        // queries of each of the eight physics substeps was most of the quad's update.
        const float cellSize = 0.25f;
        const float qx = std::isfinite(x) ? std::clamp(x, -900.0f, 900.0f) : 0.0f;
        const float qz = std::isfinite(z) ? std::clamp(z, -900.0f, 900.0f) : 0.0f;
        const auto cellX = static_cast<std::int32_t>(std::floor(qx / cellSize));
        const auto cellZ = static_cast<std::int32_t>(std::floor(qz / cellSize));
        auto& memo = quadSurfaceMemo_[(static_cast<std::uint32_t>(cellX) * 73856093U
            ^ static_cast<std::uint32_t>(cellZ) * 19349663U) % quadSurfaceMemo_.size()];
        if (memo.cellX != cellX || memo.cellZ != cellZ) {
            memo = {cellX, cellZ, terrain_.Sample((static_cast<float>(cellX) + 0.5f) * cellSize,
                                                  (static_cast<float>(cellZ) + 0.5f) * cellSize).surface};
        }
        const auto surface = memo.surface;
        switch (surface) {
        case TerrainSurface::DrySand: ground.grip = 0.75f; break;
        case TerrainSurface::WetSand: ground.grip = 0.85f; break;
        case TerrainSurface::RockyGround: ground.grip = 1.3f; break;
        case TerrainSurface::SteepGround: ground.grip = 1.05f; break;
        case TerrainSurface::DeepSeabed:
        case TerrainSurface::ShallowSeabed: ground.grip = 0.7f; break;
        default: ground.grip = 1.0f; break;
        }
        ground.grip *= 1.0f - 0.25f * weather_now::Current().wetness;
        ground.waterDepth = std::max(streamDepth, -ground.height);
        return ground;
    }

    bool IslandGame::StreamWaterAt(const float x, const float z, float& bed, float& waterDepth) const noexcept
    {
        // Near the stream: its carved bed, and the stream's own water over it.
        if (!terrainRenderer_ || !std::isfinite(x) || !std::isfinite(z) || std::abs(z - StreamRoute::CentreZ(x)) > 6.0f)
            return false;
        const StreamBedPatch& patch = terrainRenderer_->StreamBed();
        if (!patch.Contains(x, z)) return false;
        bed = patch.SampleHeight(x, z);
        float level = 0.0f;
        waterDepth = streamDrift_.Course().WaterLevel(x, z, level) ? std::max(0.0f, level - bed) : 0.0f;
        return true;
    }

    float IslandGame::WalkerWaterDepth() const noexcept
    {
        // the sea's, or the stream's where the walker wades it
        const PlayerPosition& position = playerMotion_.Position();
        float bed = 0.0f, stream = 0.0f;
        (void)StreamWaterAt(position.x, position.z, bed, stream);
        return std::max(std::max(0.0f, -playerMotion_.GroundHeight()), stream);
    }

    bool IslandGame::QuadObstacle(const float x, const float z, const float radius, float& pushX, float& pushZ) const noexcept
    {
        pushX = pushZ = 0.0f;
        bool hit = false;
        // The cave's rock and the bunker's walls and roof are not circles: the quad is moved to
        // the nearest place where it clears them (searched only while it touches one).
        if (QuadWallBlocked(x, z)) {
            // of the free places at the smallest step, the one most against the way it moves
            const BikeVec velocity = quad_.Velocity();
            const float speed = std::hypot(velocity.x, velocity.z);
            for (const float step : {0.06f, 0.15f, 0.3f, 0.55f}) {
                float best = 2.0f, bestX = 0.0f, bestZ = 0.0f;
                for (int k = 0; k < 12; ++k) {
                    const float angle = static_cast<float>(k) * (2.0f * std::numbers::pi_v<float> / 12.0f);
                    const float dx = std::cos(angle), dz = std::sin(angle);
                    if (QuadWallBlocked(x + dx * step, z + dz * step)) continue;
                    const float along = speed > 0.05f ? (dx * velocity.x + dz * velocity.z) / speed : 0.0f;
                    if (along < best) { best = along; bestX = dx * step; bestZ = dz * step; }
                }
                if (best < 1.5f) {
                    pushX += bestX;
                    pushZ += bestZ;
                    break;
                }
            }
            hit = true;
        }
        for (const auto& obstacle : worldCollisionGrid_.Obstacles()) {
            const float dx = x - obstacle.x, dz = z - obstacle.z;
            const float reach = radius + obstacle.radius;
            if (std::abs(dx) > reach || std::abs(dz) > reach) continue;
            const float distance = std::hypot(dx, dz);
            if (distance >= reach || distance < 1e-4f) continue;
            pushX += dx / distance * (reach - distance);
            pushZ += dz / distance * (reach - distance);
            hit = true;
        }
        // The large animals and the resident are bodies too (circles at their centres): the quad
        // drove through a boar, a goat or the man himself as if they were air.
        const auto body = [&](const float bodyX, const float bodyZ, const float bodyRadius) {
            const float dx = x - bodyX, dz = z - bodyZ;
            const float reach = radius + bodyRadius;
            if (std::abs(dx) > reach || std::abs(dz) > reach) return;
            const float distance = std::hypot(dx, dz);
            if (distance >= reach || distance < 1e-4f) return;
            pushX += dx / distance * (reach - distance);
            pushZ += dz / distance * (reach - distance);
            hit = true;
        };
        if (bear_) body(bear_->State().x, bear_->State().z, 0.9f);
        for (const auto& boar : boarHerd_.Boars()) body(boar.x, boar.z, 0.45f);
        if (goats_) for (const auto& goat : goats_->Goats()) body(goat.x, goat.z, 0.40f);
        if (deer_) for (const auto& deer : deer_->Deer()) body(deer.x, deer.z, 0.45f);
        for (const auto& crocodile : crocodileGroup_.Crocodiles()) body(crocodile.x, crocodile.z, 0.60f);
        if (resident_) body(resident_->State().x, resident_->State().z, 0.30f);
        return hit;
    }

    Vector3 IslandGame::TorchAxis() const noexcept
    {
        // along the view, held a little low
        Vector3 forward(-cameraView_.M13, -cameraView_.M23, -cameraView_.M33);
        if (forward.LengthSquared() < 1e-6f) forward = Vector3::UnitZ;
        forward.Normalize();
        return Vector3::Normalize(forward - Vector3::Up * 0.10f);
    }

    Vector3 IslandGame::TorchLamp() const noexcept
    {
        // in the right hand at the chest, a little ahead
        const Vector3 right(cameraView_.M11, cameraView_.M21, cameraView_.M31);
        return renderCameraPosition_ + TorchAxis() * 0.25f + right * 0.18f - Vector3::Up * 0.35f;
    }

    float IslandGame::QaSightClearance(const Vector3& eye, const Vector3& target) const
    {
        const float ex = eye.X, ez = eye.Z;
        const float dx = target.X - ex, dz = target.Z - ez;
        const float length = std::max(std::hypot(dx, dz), 0.01f);
        const float reach = std::max(0.0f, 1.0f - 0.35f / length);   // spare the subject's own spot
        float clearance = 1.0e6f;
        const auto consider = [&](const float x, const float z, const float radius) {
            const float t = std::clamp(((x - ex) * dx + (z - ez) * dz) / (length * length), 0.0f, reach);
            clearance = std::min(clearance, std::hypot(x - (ex + dx * t), z - (ez + dz * t)) - radius);
        };
        for (const auto& plant : jungleLayout_.GroundPlants()) consider(plant.x, plant.z, 0.45f * plant.scale);
        for (const auto& plant : jungleLayout_.Understory()) consider(plant.x, plant.z, 0.9f * plant.scale);
        for (const auto& bush : jungleLayout_.Bushes()) consider(bush.x, bush.z, 1.0f * bush.scale);
        // canopy trunks flare broad at the foot, with their buttress roots
        for (const auto& tree : jungleLayout_.Canopy()) consider(tree.x, tree.z, 0.7f * tree.scale);
        return clearance;
    }

    bool IslandGame::TouchRideOffered() const noexcept
    {
        // the touch ride button, where F would do something: on the quad, or on foot on land
        return quadRenderer_ != nullptr
            && (riding_ || (!flightMotion_.IsActive() && playerMotion_.Mode() != LocomotionMode::Swimming));
    }

    bool IslandGame::TouchTorchOffered() const noexcept
    {
        // the torch button on foot from dusk on (and to put a lit torch out by day)
        return !riding_ && (torchOn_ || QuadLampNightness() > 0.05f);
    }

    float IslandGame::QuadLampNightness() const noexcept
    {
        // How dark it is for the lamps: the sun's height, and a heavy sky.
        const auto& light = sun_light::Current().lighting;
        const float dusk = 1.0f - std::clamp((light.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f);
        return std::clamp(dusk + 0.3f * std::max(weather_now::Current().cloudCover, 0.0f), 0.0f, 1.0f);
    }

    void IslandGame::PlaceQuadNear(const float x, const float z, const float yawRadians)
    {
        quadSite_ = QuadSite::Open;   // placed anew, it is not where it was
        quad_.Place({x, 0.0f, z}, yawRadians, [this](float px, float pz) { return QuadGround(px, pz); });
        quadPlaced_ = true;
        quadRestSeconds_ = 0.0f;
        quadRestAnchor_ = quad_.Position();
    }

    void IslandGame::ToggleQuad()
    {
        if (!quadRenderer_) return;
        const auto p = quad_.Position();
        if (riding_) {
            // Step off on the left (or the right, or behind), where the ground is clear.
            const BikeVec right = quad_.Right(), forward = quad_.Forward();
            const std::array<std::array<float, 2>, 3> sides{{{-1.7f, 0.0f}, {1.7f, 0.0f}, {0.0f, -2.3f}}};
            for (const auto& side : sides) {
                const float sx = p.x + right.x * side[0] + forward.x * side[1];
                const float sz = p.z + right.z * side[0] + forward.z * side[1];
                if (!worldCollisionGrid_.IntersectsCircle(sx, sz, 0.4f) && terrain_.Height(sx, sz) > -0.3f) {
                    playerMotion_.SetPosition(terrain_, sx, sz);
                    break;
                }
            }
            riding_ = false;
            yaw_ = quad_.YawRadians();
            const auto& position = playerMotion_.Position();
            cameraPosition_ = Vector3(position.x, position.y, position.z);
            std::cout << "island-simulator: stepped off the quad\n";
        } else {
            if (flightMotion_.IsActive() || playerMotion_.Mode() == LocomotionMode::Swimming) return;
            const auto& position = playerMotion_.Position();
            if (std::hypot(position.x - p.x, position.z - p.z) > 9.0f) {
                // Far from it: the quad is brought to the walker, a few metres ahead.
                const float ax = position.x + std::sin(yaw_) * 3.5f, az = position.z + std::cos(yaw_) * 3.5f;
                const bool clear = !worldCollisionGrid_.IntersectsCircle(ax, az, 1.2f) && terrain_.Height(ax, az) > 0.0f;
                PlaceQuadNear(clear ? ax : position.x, clear ? az : position.z, yaw_);
            }
            riding_ = true;
            quadEngineOn_ = true;
            quadIdleSeconds_ = 0.0f;
            quadOrbit_ = 0.0f;
            quadCameraYaw_ = quad_.YawRadians();
            const auto now = quad_.Position();
            quadCamera_ = Vector3(now.x - std::sin(quadCameraYaw_) * 4.8f, now.y + 1.9f, now.z - std::cos(quadCameraYaw_) * 4.8f);
            std::cout << "island-simulator: riding the quad\n";
        }
        cameraMotion_.Reset();
    }

    void IslandGame::UpdateQuad(const float deltaSeconds)
    {
        if (!quadRenderer_ || !quadPlaced_) return;
        const auto ground = [this](float x, float z) { return QuadGround(x, z); };
        const auto obstacles = [this](float x, float z, float r, float& px, float& pz) { return QuadObstacle(x, z, r, px, pz); };
        if (viewpoint_ == FixedViewpoint::QuadJettyDetail && jettyPlacement_) {
            // the jetty view rides six metres out along the deck, then brakes and stands on it
            const auto q = quad_.Position();
            const float along = (q.x - jettyPlacement_->rootX) * jettyPlacement_->dirX
                + (q.z - jettyPlacement_->rootZ) * jettyPlacement_->dirZ;
            if (along > 6.0f) quadControls_ = {0.0f, 0.0f, true};
        }
        // the stream view stands a moment, so that it is in the water as the capture is taken
        const bool standing = viewpoint_ == FixedViewpoint::QuadStreamDetail && quadStreamWaitUpdates_-- > 0;
        // (flooded, the dead engine drives nothing: the rider can only steer it as it drifts)
        const BikeControls controls = riding_ && !standing
            ? (quadFlooded_ ? BikeControls{0.0f, quadControls_.steer, false} : quadControls_)
            : BikeControls{0.0f, 0.0f, true};
        // A parked quad that has settled sleeps: its physics (eight substeps, each sampling the
        // ground and the obstacles under four wheels) cost about 1.5 ms an update for a machine
        // standing still, and on its springs it never quite stopped (a few cm/s and a fifth of a
        // radian a second of tremble), so it is judged by where it is: still within 2 cm of
        // where it stood a second ago. Getting on, or placing it, wakes it.
        if (riding_) {
            quadRestSeconds_ = 0.0f;
            quadRestAnchor_ = quad_.Position();
        } else if (quadRestSeconds_ <= 1.0f) {
            const BikeVec p = quad_.Position();
            const float dx = p.x - quadRestAnchor_.x, dy = p.y - quadRestAnchor_.y, dz = p.z - quadRestAnchor_.z;
            if (dx * dx + dy * dy + dz * dz > 0.0004f) {
                quadRestSeconds_ = 0.0f;
                quadRestAnchor_ = p;
            } else {
                quadRestSeconds_ += deltaSeconds;
            }
        }
        const bool asleep = quadRestSeconds_ > 1.0f;
        updateProfile_.Mark(asleep ? "quad (asleep)" : "quad physics");
        if (!asleep) {
            UpdateQuadSite();
            quad_.Step(deltaSeconds, controls, ground, obstacles);
        }
        updateProfile_.Mark("quad wheels");
        // What the wheels leave and throw: ruts in sand and soil, dust off dry ground, spray in
        // the shallows.
        std::array<WheelOnGround, 4> wheels = quadWheels_;
        const BikeVec up = quad_.Up();
        const float wetness = weather_now::Current().wetness;
        for (int w = 0; w < 4 && !asleep; ++w) {
            const BikeVec hub = quad_.WheelHub(w);
            WheelOnGround& wheel = wheels[static_cast<std::size_t>(w)];
            wheel.x = hub.x - up.x * QuadBike::WheelRadius;
            wheel.z = hub.z - up.z * QuadBike::WheelRadius;
            if (QuadOnShelterFloor(wheel.x, wheel.z)) {
                // on the cave's or the bunker's floor: no ruts, dust or spray (the heightfield's
                // soil is up on the hill above)
                wheel.y = QuadGround(wheel.x, wheel.z).height;
                wheel.width = w < 2 ? 0.20f : 0.245f;
                wheel.load = quad_.Compression(w);
                wheel.down = quad_.WheelContact(w);
                wheel.waterDepth = 0.0f;
                wheel.wet = wheel.soft = wheel.dusty = false;
                continue;
            }
            const auto sample = terrain_.Sample(wheel.x, wheel.z);
            wheel.y = sample.height;
            wheel.width = w < 2 ? 0.20f : 0.245f;
            wheel.load = quad_.Compression(w);
            wheel.down = quad_.WheelContact(w);
            wheel.waterDepth = std::max(0.0f, -sample.height);
            // in the stream's channel: its bed and its water
            float streamBed = 0.0f, streamDepth = 0.0f;
            const bool inChannel = StreamWaterAt(wheel.x, wheel.z, streamBed, streamDepth)
                && streamBed < sample.height - 0.03f;
            if (inChannel) {
                wheel.y = streamBed;
                wheel.waterDepth = std::max(wheel.waterDepth, streamDepth);
                // the wheel rings the stream's water it runs through
                const float speed = std::hypot(quad_.Velocity().x, quad_.Velocity().z);
                if (streamDepth > 0.03f && speed > 0.5f) streamRings_.Note(wheel.x, wheel.z, std::min(1.0f, 0.3f + speed / 8.0f));
            }
            const auto surface = sample.surface;
            // a rain puddle (where the terrain shader paints one): a few centimetres of water
            // that the tyres throw up as spray
            const float puddle = terrain_noise::PuddleAt(wheel.x, wheel.z, sample.height, sample.slopeRadians,
                sample.signedShoreDistance, surface == TerrainSurface::DrySand || surface == TerrainSurface::WetSand, wetness);
            if (puddle > 0.35f && wheel.waterDepth < 0.05f) wheel.waterDepth = 0.035f + 0.03f * puddle;
            wheel.wet = inChannel || surface == TerrainSurface::WetSand || wetness > 0.6f;
            // out of the water the tyre stays wet for some fourteen metres, printing as it dries
            float& treadWater = quadTreadWater_[static_cast<std::size_t>(w)];
            treadWater = wheel.waterDepth > 0.03f ? 1.0f
                : std::max(0.0f, treadWater - quad_.Speed() * deltaSeconds / 14.0f);
            wheel.wetTread = wheel.waterDepth > 0.03f ? 0.0f : treadWater;
            wheel.soft = wheel.waterDepth < 0.05f && (surface == TerrainSurface::DrySand || surface == TerrainSurface::WetSand
                || surface == TerrainSurface::CoastalGround || surface == TerrainSurface::InteriorGround);
            // the forest floor stays damp under the canopy: it dusts only after a dry spell
            wheel.dusty = (wetness < 0.35f && (surface == TerrainSurface::DrySand || surface == TerrainSurface::CoastalGround))
                || (wetness < 0.1f && surface == TerrainSurface::InteriorGround);
        }
        quadWheels_ = wheels;
        // The quad gathers dirt: mud off wet ground and dust off dry, thrown up by the wheels
        // the faster it goes; wading the sea washes it off, and the rain rinses it slowly. The
        // mud stays dark while fresh or rained on and dries pale over a few minutes.
        {
            const BikeVec velocity = quad_.Velocity();
            const float speed = std::min(std::hypot(velocity.x, velocity.z), 12.0f);
            float gain = 0.0f, wash = 0.0f, freshen = 0.0f;
            for (const auto& wheel : wheels) {
                if (wheel.waterDepth > 0.15f) wash += 0.05f;
                if (!wheel.down || wheel.waterDepth >= 0.05f) continue;
                if (wheel.wet) { gain += 0.0008f * speed; freshen += 0.25f; }
                else if (wheel.dusty) gain += 0.0002f * speed;
            }
            const float rain = weather_now::Current().rain;
            quadDirt_ = std::clamp(quadDirt_ + deltaSeconds * (gain - wash - 0.004f * rain), 0.0f, 1.0f);
            if ((freshen > 0.0f && speed > 0.5f) || rain > 0.3f) quadDirtWet_ = std::min(1.0f, quadDirtWet_ + deltaSeconds * 0.5f);
            else quadDirtWet_ = std::max(0.0f, quadDirtWet_ - deltaSeconds / 300.0f);
        }
        updateProfile_.Mark("quad tyre tracks");
        tyreTracks_.Update(deltaSeconds, wheels);
        updateProfile_.Mark("quad dust and spray");
        wheelDust_.Update(deltaSeconds, wheels, quad_.Velocity(), wind::DirectionX, wind::DirectionZ);
        wheelSpray_.Update(deltaSeconds, wheels, quad_.Velocity(), wind::DirectionX, wind::DirectionZ);
        // A hard knock (a trunk, a rock, the chassis on the ground, a heavy landing) or the
        // quad going over.
        const float knock = quad_.LastImpact();
        quadImpactCooldown_ = std::max(0.0f, quadImpactCooldown_ - deltaSeconds);
        const bool rolled = quad_.Rollovers() != quadPreviousRollovers_;
        quadPreviousRollovers_ = quad_.Rollovers();
        if (soundscape_ && quadImpactCooldown_ <= 0.0f && (knock > 2.0f || rolled)) {
            const auto p = quad_.Position();
            soundscape_->NoteQuadImpact(rolled ? 1.0f : std::clamp((knock - 2.0f) / 6.0f, 0.0f, 1.0f), p.x, p.y, p.z, riding_);
            quadImpactCooldown_ = 0.35f;
        }
        // Driven into water over the seat the airbox floods and the engine dies (its lamps out,
        // its exhaust and its sound gone); it catches again two seconds after the airbox is clear.
        {
            const auto q = quad_.Position();
            if (QuadBike::AirboxFlooded(q, QuadGround(q.x, q.z))) {
                if (!quadFlooded_ && quadEngineOn_) std::cout << "island-simulator: the quad's airbox floods and the engine dies\n";
                quadFlooded_ = true;
                quadRestartSeconds_ = 2.0f;
            } else if (quadFlooded_ && (quadRestartSeconds_ -= deltaSeconds) <= 0.0f) {
                quadFlooded_ = false;
                if (riding_) std::cout << "island-simulator: out of the water the quad's engine catches again\n";
            }
        }
        // The engine: started when the rider gets on, idling on after the rider steps off until
        // it is switched off a minute and a half later (or the quad lies on its side).
        if (riding_) {
            quadEngineOn_ = !quadFlooded_;
            quadIdleSeconds_ = 0.0f;
        } else if (quadEngineOn_) {
            quadIdleSeconds_ += deltaSeconds;
            if (quadIdleSeconds_ > QuadIdleOffSeconds || quad_.Overturned() || quadFlooded_) quadEngineOn_ = false;
        }
        // The wildlife hears it: a rider on the roaring quad alarms the deer, goats and boars
        // from up to two and a half times as far as a walker, and the perched birds flush as
        // it races past.
        const float quadSpeed = std::hypot(quad_.Velocity().x, quad_.Velocity().z);
        const float threat = riding_ && quadEngineOn_
            ? 1.0f + 1.5f * std::clamp(0.5f * std::max(quad_.Throttle(), 0.0f) + quadSpeed / 15.0f, 0.0f, 1.0f) : 1.0f;
        if (deer_) deer_->SetObserverThreat(threat);
        if (goats_) goats_->SetObserverThreat(threat);
        boarHerd_.SetObserverThreat(threat);
        quadFlushSeconds_ = std::max(0.0f, quadFlushSeconds_ - deltaSeconds);
        if (riding_ && quadEngineOn_ && quadSpeed > 4.0f && quadFlushSeconds_ <= 0.0f) {
            const auto p = quad_.Position();
            birdFlock_.NoteDisturbance(p.x, p.z);
            quadFlushSeconds_ = 1.5f;
        }
        const BikeVec tip = quad_.ToWorld(QuadBikeRenderer::ExhaustTip);
        const BikeVec back{-quad_.Forward().x, -quad_.Forward().y, -quad_.Forward().z};
        updateProfile_.Mark("quad exhaust and sound");
        exhaust_.Update(deltaSeconds, quadEngineOn_, quad_.Throttle(), tip, back, quad_.Velocity(), wind::DirectionX, wind::DirectionZ);
        if (soundscape_) {
            const auto p = quad_.Position();
            soundscape_->SetEngine(quadEngineOn_, quad_.EngineRpm(), quad_.Throttle(), p.x, p.y, p.z, riding_);
            bool anyDown = false;
            for (int w = 0; w < 4; ++w) anyDown = anyDown || quad_.WheelContact(w);
            // how deep the wheels wade: their water over the hubs' height, the four averaged
            float wading = 0.0f;
            for (const auto& wheel : quadWheels_) wading += std::clamp(wheel.waterDepth / QuadBike::WheelRadius, 0.0f, 1.0f) * 0.25f;
            const auto v = quad_.Velocity();
            soundscape_->SetTyres(anyDown ? std::hypot(v.x, v.z) : 0.0f, anyDown ? quad_.SlideSpeed() : 0.0f, wading,
                                  p.x, p.y, p.z, riding_);
        }
        if (riding_) {
            // The walker goes where the quad goes (the wildlife and the sounds follow).
            const auto p = quad_.Position();
            playerMotion_.SetPosition(terrain_, p.x, p.z);
        }
    }

    void IslandGame::UpdateQuadChaseCamera(const float deltaSeconds)
    {
        // A chase camera behind the quad; the mouse swings it round, and it settles back
        // behind when the mouse rests.
        const float looked = std::remainder(yaw_ - quadPreviousYaw_, 6.2831853f);
        quadOrbit_ = std::remainder(quadOrbit_ + looked, 6.2831853f);
        quadOrbitIdle_ = std::abs(looked) > 1e-4f ? 0.0f : quadOrbitIdle_ + deltaSeconds;
        if (quadOrbitIdle_ > 1.5f) quadOrbit_ *= std::exp(-deltaSeconds * 1.8f);
        const auto p = quad_.Position();
        const float targetYaw = quad_.YawRadians() + quadOrbit_;
        quadCameraYaw_ += std::remainder(targetYaw - quadCameraYaw_, 6.2831853f) * (1.0f - std::exp(-deltaSeconds * 6.0f));
        Vector3 desired(p.x - std::sin(quadCameraYaw_) * 4.8f, p.y + 1.9f, p.z - std::cos(quadCameraYaw_) * 4.8f);
        const auto insideBunker = [this](const Vector3& at) {
            return bunkerLayout_.Local({at.X, at.Y, at.Z}).z >= -0.35f;
        };
        if (quadSite_ == QuadSite::Bunker && insideBunker(desired)) {
            // In the bunker the camera stays in its rooms, under the roof (the ridge's terrain
            // above them had lifted it out onto the grass): drawn in behind the quad until it
            // stands in the plan, between the floor and the ceiling.
            for (float back = 4.8f; back > 0.9f && !bunkerLayout_.ContainsWalkable(desired.X, desired.Z, 0.3f); back -= 0.3f) {
                desired.X = p.x - std::sin(quadCameraYaw_) * back;
                desired.Z = p.z - std::cos(quadCameraYaw_) * back;
            }
            const float depth = bunkerLayout_.Local({desired.X, desired.Y, desired.Z}).z;
            const float floor = bunkerLayout_.EntranceGroundY() + bunkerLayout_.FloorRelative(depth);
            const float ceiling = floor + (depth < 10.0f ? 2.75f : 3.05f);   // the ramp's and the rooms' roofs
            desired.Y = std::clamp(desired.Y, floor + 0.6f, ceiling - 0.35f);
        } else {
            desired.Y = std::max(desired.Y, terrain_.Height(desired.X, desired.Z) + 0.6f);
        }
        quadCamera_ = Vector3::Lerp(quadCamera_, desired, 1.0f - std::exp(-deltaSeconds * 9.0f));
        cameraPosition_ = quadCamera_;
        const Vector3 look(p.x - quadCamera_.X, p.y + 0.7f - quadCamera_.Y, p.z - quadCamera_.Z);
        yaw_ = std::atan2(look.X, look.Z);
        pitch_ = std::atan2(look.Y, std::hypot(look.X, look.Z));
        quadPreviousYaw_ = yaw_;
        cameraMotion_.Reset();
    }

    void IslandGame::UpdateBenchmarkTraversal()
    {
        float progress = 0.0f;
        if (benchmarkWarmupIntervals_ >= BenchmarkWarmupIntervals) {
            progress = static_cast<float>(benchmarkFrameMilliseconds_.size())
                / static_cast<float>(BenchmarkSampleIntervals - 1U);
        }
        const BenchmarkRouteSample sample = benchmarkTraversalRoute_.Sample(progress);
        playerMotion_.SetPosition(terrain_, sample.x, sample.z);
        const PlayerPosition& player = playerMotion_.Position();
        cameraPosition_ = Vector3(player.x, player.y, player.z);
        yaw_ = std::atan2(sample.forwardX, sample.forwardZ);

        constexpr float lookAheadDistance = 5.0f;
        const float targetX = sample.x + sample.forwardX * lookAheadDistance;
        const float targetZ = sample.z + sample.forwardZ * lookAheadDistance;
        const bool targetIsWater = terrain_.Height(targetX, targetZ) < -0.16f;
        const float targetY = targetIsWater
            ? PlayerMotion::SwimmingEyeHeight
            : terrain_.Height(targetX, targetZ) + PlayerMotion::EyeHeight;
        pitch_ = std::clamp(
            std::atan2(targetY - cameraPosition_.Y, lookAheadDistance) - 0.025f,
            -0.18f, 0.14f);
        benchmarkTraversalPresentationSpeed_ = playerMotion_.Mode()
            == LocomotionMode::Swimming ? PlayerMotion::SwimSpeed : PlayerMotion::WalkSpeed;
    }

    void IslandGame::UpdateCamera(const float deltaSeconds)
    {
        const KeyboardState keyboard = Keyboard::GetState();
        const bool flightDown = keyboard.IsKeyDown(Keys::Tab);
        if (flightDown && !flightToggleWasDown_ && getIsActiveProperty() && !riding_) {   // no flight off the quad's seat
            if (flightMotion_.IsActive()) {
                const auto landing=flightMotion_.FindLanding(terrain_,worldCollisionGrid_);
                if (landing) {
                    playerMotion_.SetPosition(terrain_,landing->x,landing->z);
                    playerMotion_.Update(terrain_,0,0,0,worldCollisionGrid_);
                    flightMotion_.Deactivate();
                    cameraPosition_=Vector3(landing->x,landing->y,landing->z);
                    ReportControlTransition(ControlTransitionEvent::FlightOff);
                } else {
                    ReportControlTransition(ControlTransitionEvent::LandingRefused);
                }
            } else {
#ifndef NDEBUG
                debugFreeCamera_.Deactivate();
#endif
                flightMotion_.Activate(terrain_,{cameraPosition_.X,cameraPosition_.Y,cameraPosition_.Z});
                const auto p=playerMotion_.Position();
                playerMotion_.SetPosition(terrain_,p.x,p.z); // Stop stale walking velocity/footsteps.
                ReportControlTransition(ControlTransitionEvent::FlightOn);
            }
            cameraMotion_.Reset();
            locomotionModeNoticeSeconds_=LocomotionModeNoticeDuration;
        }
        flightToggleWasDown_=flightDown;
        const MouseState mouse = Mouse::GetState();
        const auto touches = Touch::TouchPanel::GetState();
        std::vector<TouchContact> touchContacts;
        touchContacts.reserve(static_cast<std::size_t>(touches.getCountProperty()));
        for (const Touch::TouchLocation& touch : touches) {
            TouchPhase phase = TouchPhase::Moved;
            switch (touch.getStateProperty()) {
            case Touch::TouchLocationState::Pressed:
                phase = TouchPhase::Pressed;
                break;
            case Touch::TouchLocationState::Released:
            case Touch::TouchLocationState::Invalid:
                phase = TouchPhase::Released;
                break;
            case Touch::TouchLocationState::Moved:
                phase = TouchPhase::Moved;
                break;
            }
            const Vector2& position = touch.getPositionProperty();
            touchContacts.push_back({touch.getIdProperty(), position.X, position.Y, phase});
        }

        const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
        const float touchWidth = Touch::TouchPanel::getDisplayWidthProperty() > 0
            ? static_cast<float>(Touch::TouchPanel::getDisplayWidthProperty())
            : static_cast<float>(viewport.getWidthProperty());
        const float touchHeight = Touch::TouchPanel::getDisplayHeightProperty() > 0
            ? static_cast<float>(Touch::TouchPanel::getDisplayHeightProperty())
            : static_cast<float>(viewport.getHeightProperty());
        touchDisplayWidth_ = touchWidth;
        touchDisplayHeight_ = touchHeight;
        touchControls_.SetActionsEnabled(TouchRideOffered(), TouchTorchOffered());
        touchControlState_ = touchControls_.Update(touchContacts, touchWidth, touchHeight);
        if (!touchContacts.empty()) {
            touchOverlayVisible_ = true;
        }
        if (touchControlState_.lookSensitivityChanged) {
            userSettings_.touchLookSensitivity = touchControlState_.lookSensitivity;
            PersistUserSettings();
            const std::string_view diagnostic = [this]() -> std::string_view {
                switch (touchControlState_.lookSensitivity) {
                case TouchLookSensitivityLevel::Low:
                    return "island-simulator: touch look sensitivity low (0.72x)";
                case TouchLookSensitivityLevel::Standard:
                    return "island-simulator: touch look sensitivity standard (1.00x)";
                case TouchLookSensitivityLevel::High:
                    return "island-simulator: touch look sensitivity high (1.35x)";
                }
                return "island-simulator: touch look sensitivity standard (1.00x)";
            }();
            std::cout << diagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
        }
        const bool touchInteracting = touchControlState_.movementActive
            || touchControlState_.lookActive
            || touchControlState_.sensitivitySelectorActive
            || touchControlState_.actionButtonActive;
#if defined(__EMSCRIPTEN__)
        // The page acquires pointer lock synchronously in the trusted click handler. A short
        // click can finish between game updates, so polled button state is not its receipt.
        // Observe the browser's granted lock instead of relying on the transient button state.
        EmscriptenPointerlockChangeEvent pointerLock{};
        if (emscripten_get_pointerlock_status(&pointerLock) != EMSCRIPTEN_RESULT_SUCCESS) {
            throw std::runtime_error("Unable to query browser pointer-lock state");
        }
        const bool browserPointerLocked = pointerLock.isActive
            && std::string_view(pointerLock.id) == "canvas";
        if (!browserPointerLocked) {
            browserPointerReleasePending_ = false;
            if (Mouse::getIsRelativeMouseModeEXTProperty()) {
                Mouse::setIsRelativeMouseModeEXTProperty(false);
                setIsMouseVisibleProperty(true);
            }
            mouseCaptured_ = false;
        }
#endif

        const bool runToggleDown = keyboard.IsKeyDown(Keys::LeftShift)
            || keyboard.IsKeyDown(Keys::RightShift);
        if (locomotionPaceToggle_.Update(runToggleDown, !IsDebugFreeCameraActive() && !flightMotion_.IsActive()
            && playerMotion_.Mode()!=LocomotionMode::Swimming)) {
            userSettings_.runningSelected = locomotionPaceToggle_.IsRunningSelected();
            PersistUserSettings();
            locomotionModeNoticeSeconds_ = LocomotionModeNoticeDuration;
            const std::string_view diagnostic = locomotionPaceToggle_.IsRunningSelected()
                ? "island-simulator: run mode enabled (8.0 m/s)"
                : "island-simulator: run mode disabled (4.3 m/s)";
            std::cout << diagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
        }

        const int webMovementKeyMask =
#if defined(__EMSCRIPTEN__)
            WebMovementKeyMaskValue;
#else
            0;
#endif
        const FirstPersonDigitalInput digitalInput{
            keyboard.IsKeyDown(Keys::W) || keyboard.IsKeyDown(Keys::Up)
                || (webMovementKeyMask & 1) != 0,
            keyboard.IsKeyDown(Keys::S) || keyboard.IsKeyDown(Keys::Down)
                || (webMovementKeyMask & 2) != 0,
            keyboard.IsKeyDown(Keys::A) || keyboard.IsKeyDown(Keys::Left)
                || (webMovementKeyMask & 4) != 0,
            keyboard.IsKeyDown(Keys::D) || keyboard.IsKeyDown(Keys::Right)
                || (webMovementKeyMask & 8) != 0,
            locomotionPaceToggle_.IsRunningSelected() || touchControlState_.runRequested,
        };
        const bool movementKeyDown = digitalInput.forward || digitalInput.backward
            || digitalInput.left || digitalInput.right;
        const bool movementKeyPressed = movementKeyDown && !movementKeyWasDown_;
        movementKeyWasDown_ = movementKeyDown;
        if (soundscapeNeedsGesture_
            && (mouse.getLeftButtonProperty() == ButtonState::Pressed
                || !touchContacts.empty() || movementKeyDown)) {
            TryStartSoundscape();
        }

        const bool cameraMotionToggleDown = keyboard.IsKeyDown(Keys::B);
        if (cameraMotionToggleDown && !cameraMotionToggleWasDown_) {
            cameraMotion_.SetEnabled(!cameraMotion_.IsEnabled());
            userSettings_.cameraMotionEnabled = cameraMotion_.IsEnabled();
            PersistUserSettings();
            std::cout << "island-simulator: camera motion "
                      << (cameraMotion_.IsEnabled() ? "enabled" : "disabled") << '\n';
        }
        cameraMotionToggleWasDown_ = cameraMotionToggleDown;

        if (touchInteracting && mouseCaptured_) {
            mouseCaptured_ = false;
            Mouse::setIsRelativeMouseModeEXTProperty(false);
            setIsMouseVisibleProperty(true);
        }

        if (mouseCaptured_ && !Mouse::getIsRelativeMouseModeEXTProperty()) {
            mouseCaptured_ = false;
            setIsMouseVisibleProperty(true);
        }

        const bool worldDebugToggleDown = keyboard.IsKeyDown(Keys::F2);
        if (worldDebugToggleDown && !worldDebugToggleWasDown_) {
            worldDebugVisible_ = !worldDebugVisible_;
            std::cout << "island-simulator: world debug visualization "
                      << (worldDebugVisible_
                              ? "enabled with 9 ground-contact samples, resolved movement trace, "
                                "wildlife navigation guides, 3 bird route regions, 24 bird "
                                "spawns, 6 audio-zone contours, and positional-emitter markers"
                              : "disabled")
                      << '\n';
        }
        worldDebugToggleWasDown_ = worldDebugToggleDown;

        const bool worldSeedCopyDown = keyboard.IsKeyDown(Keys::F4);
        if (debugOverlayVisible_ && worldSeedCopyDown && !worldSeedCopyWasDown_) {
            const std::string seedText = WorldSeedClipboardText(AuthoritativeWorldSeeds());
            CNA::Input::Clipboard::SetTextEXT(seedText);
            worldSeedCopySucceeded_ = CNA::Input::Clipboard::GetTextEXT() == seedText;
            worldSeedCopyFeedbackSeconds_ = 3.0f;
            std::cout << "island-simulator: world seed clipboard copy "
                      << (worldSeedCopySucceeded_ ? "confirmed" : "unavailable")
                      << '\n' << std::flush;
        }
        worldSeedCopyWasDown_ = worldSeedCopyDown;

#ifndef NDEBUG
        const bool debugFreeCameraToggleDown = keyboard.IsKeyDown(Keys::F3);
        if (debugOverlayVisible_ && !flightMotion_.IsActive() && debugFreeCameraToggleDown
            && !debugFreeCameraToggleWasDown_) {
            if (debugFreeCamera_.IsActive()) {
                debugFreeCamera_.Deactivate();
                const PlayerPosition& player = playerMotion_.Position();
                cameraPosition_ = Vector3(player.x, player.y, player.z);
            } else {
                debugFreeCamera_.Activate(
                    cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
                cameraMotion_.Reset();
            }
            std::cout << "island-simulator: debug free camera "
                      << (debugFreeCamera_.IsActive() ? "enabled" : "disabled")
                      << '\n' << std::flush;
        }
        debugFreeCameraToggleWasDown_ = debugFreeCameraToggleDown;
        if (!debugOverlayVisible_ && debugFreeCamera_.IsActive()) {
            debugFreeCamera_.Deactivate();
            const PlayerPosition& player = playerMotion_.Position();
            cameraPosition_ = Vector3(player.x, player.y, player.z);
            std::cout << "island-simulator: debug free camera disabled with overlay\n";
        }
#endif

        constexpr std::array<Keys, 12> teleportKeys = {
            Keys::F5, Keys::F6, Keys::F7, Keys::F8, Keys::F9, Keys::F10, Keys::J,
            Keys::F12, Keys::K, Keys::L, Keys::O, Keys::P};
        constexpr std::array<const char*, 12> teleportNames = {
            "south beach", "jungle trail", "east clearing", "west clearing",
            "fish shallows", "offshore shark", "monkey troop", "offshore current", "boar herd", "cave entrance", "campfire", "stream crossing"};
        for (std::size_t index = 0; index < teleportKeys.size(); ++index) {
            const bool down = keyboard.IsKeyDown(teleportKeys[index]);
            if (debugOverlayVisible_ && down && !debugTeleportWasDown_[index]) {
                flightMotion_.Deactivate();
                float x = 0.0f;
                float z = -225.0f;
                yaw_ = 0.0f;
                pitch_ = -0.03f;
                if(index==11U) {
                    x=-110.0f;z=StreamRoute::CentreZ(x)-4.0f;
                    yaw_=0.0f;pitch_=-.10f;
                } else if (index == 1U) {
                    z = 45.0f;
                    x = VegetationLayout::PrimaryPathCenterX(z);
                    yaw_ = std::numbers::pi_v<float> * 0.5f;
                    pitch_ = -0.065f;
                } else if (index == 2U) {
                    // The campfire pit now occupies the clearing centre; land six metres
                    // west of it so collision does not displace the teleported player.
                    x = VegetationLayout::EastClearing.x - 6.0f;
                    z = VegetationLayout::EastClearing.z;
                    yaw_ = -std::numbers::pi_v<float> * 0.5f;
                } else if (index == 3U) {
                    x = VegetationLayout::WestClearing.x;
                    z = VegetationLayout::WestClearing.z;
                    yaw_ = std::numbers::pi_v<float> * 0.25f;
                } else if (index == 4U && !fishSchools_.Fish().empty()) {
                    const FishState& fish = fishSchools_.Fish().front();
                    const float angle = std::atan2(fish.z, fish.x);
                    const float radius = terrain_.ShoreRadius(angle) - 2.5f;
                    x = std::cos(angle) * radius;
                    z = std::sin(angle) * radius;
                    yaw_ = std::atan2(fish.x - x, fish.z - z);
                    pitch_ = -0.16f;
                } else if (index == 5U) {
                    const SharkState& shark = sharkPatrol_.State();
                    const float angle = std::atan2(shark.z, shark.x);
                    const float radius = terrain_.ShoreRadius(angle) - 2.5f;
                    x = std::cos(angle) * radius;
                    z = std::sin(angle) * radius;
                    yaw_ = std::atan2(shark.x - x, shark.z - z);
                    pitch_ = -0.095f;
                } else if (index == 6U) {
                    const std::span<const MonkeyState> monkeys = monkeyTroop_.Monkeys();
                    const MonkeyState& monkey = monkeys[monkeyTeleportIndex_ % monkeys.size()];
                    monkeyTeleportIndex_ = (monkeyTeleportIndex_ + 1U) % monkeys.size();
                    x = monkey.x + std::cos(monkey.headingRadians) * 5.0f;
                    z = monkey.z + std::sin(monkey.headingRadians) * 5.0f;
                    yaw_ = std::atan2(monkey.x - x, monkey.z - z);
                    pitch_ = -0.08f;
                } else if(index==10U) {
                    x=CampfireSite::X-3.8f;z=CampfireSite::Z;
                    yaw_=std::numbers::pi_v<float>*.5f;pitch_=-.10f;
                } else if (index == 9U) {
                    const auto& cave=terrainRenderer_->Cave();
                    const auto start=cave.World(0,0,-8),target=cave.World(0,0,0);
                    x=start.x;z=start.z;
                    yaw_=std::atan2(target.x-x,target.z-z);pitch_=0;
                } else if (index == 8U) {
                    const auto boars = boarHerd_.Boars();
                    const auto& boar = boars[boarTeleportIndex_++ % boars.size()];
                    x = boar.x + std::cos(boar.headingRadians) * 7.0f;
                    z = boar.z + std::sin(boar.headingRadians) * 7.0f;
                    yaw_ = std::atan2(boar.x - x, boar.z - z);
                    pitch_ = -0.08f;
                } else if (index == 7U) {
                    constexpr float angle = -std::numbers::pi_v<float> * 0.5f;
                    const float radius = terrain_.ShoreRadius(angle) + 62.0f;
                    x = std::cos(angle) * radius;
                    z = std::sin(angle) * radius;
                    yaw_ = 0.0f;
                    pitch_ = -0.015f;
                }
                playerMotion_.SetPosition(terrain_, x, z);
                cameraMotion_.Reset();
                const PlayerPosition& teleported = playerMotion_.Position();
                cameraPosition_ = Vector3(teleported.x, teleported.y, teleported.z);
#ifndef NDEBUG
                if (debugFreeCamera_.IsActive()) {
                    debugFreeCamera_.Activate(
                        cameraPosition_.X, cameraPosition_.Y, cameraPosition_.Z);
                }
#endif
                std::cout << "island-simulator: debug teleport "
                          << teleportNames[index] << " x " << x << " z " << z << '\n';
            }
            debugTeleportWasDown_[index] = down;
        }

        const bool escapeDown = keyboard.IsKeyDown(Keys::Escape);
        const bool escapePressed = escapeDown && !escapeWasDown_;
        escapeWasDown_ = escapeDown;
        if (escapePressed) {
#if defined(__EMSCRIPTEN__)
            // Browser Escape can release pointer lock before the game observes this key edge.
            // Never turn that ordering difference into a request to terminate the Web session.
            std::cout << "island-simulator: browser Escape state before release captured "
                      << mouseCaptured_ << " relative "
                      << Mouse::getIsRelativeMouseModeEXTProperty() << '\n';
            // The HTML page can own a lock before CNA relative mode is enabled. Release that
            // lock explicitly too; SDL's already-disabled relative mode is otherwise a no-op.
            browserPointerReleasePending_ = browserPointerLocked;
            if (browserPointerLocked && emscripten_exit_pointerlock() != EMSCRIPTEN_RESULT_SUCCESS) {
                throw std::runtime_error("Unable to release browser pointer lock");
            }
            mouseCaptured_ = false;
            Mouse::setIsRelativeMouseModeEXTProperty(false);
            setIsMouseVisibleProperty(true);
            std::cout << "island-simulator: Escape released browser pointer lock without exiting\n"
                      << std::flush;
#else
            if (mouseCaptured_) {
                mouseCaptured_ = false;
                Mouse::setIsRelativeMouseModeEXTProperty(false);
                setIsMouseVisibleProperty(true);
            } else {
                Exit();
            }
#endif
            return;
        }

        bool acquiredMouseThisFrame = false;
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
        (void)movementKeyPressed;
        constexpr bool keyboardCanCaptureMouse = false;
#else
        const bool keyboardCanCaptureMouse = movementKeyPressed;
#endif
#if defined(__EMSCRIPTEN__)
        (void)keyboardCanCaptureMouse;
        const bool pointerCanCapture = browserPointerLocked && !browserPointerReleasePending_;
#else
        const bool pointerCanCapture = mouse.getLeftButtonProperty() == ButtonState::Pressed
            || keyboardCanCaptureMouse;
#endif
        if (!touchInteracting && !smokeTest_ && !mouseCaptured_
            && pointerCanCapture) {
            mouseCaptured_ = true;
            Mouse::setIsRelativeMouseModeEXTProperty(true);
            setIsMouseVisibleProperty(false);
            acquiredMouseThisFrame = true;
        }
        if (touchControlState_.lookSensitivityChanged && !mouseCaptured_) {
            constexpr std::string_view diagnostic =
                "island-simulator: touch sensitivity selector retained touch ownership "
                "without mouse capture";
            std::cout << diagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
        }
        if (mouseCaptured_ && !acquiredMouseThisFrame) {
            yaw_ -= static_cast<float>(mouse.getXProperty()) * MouseSensitivity * MouseLookScale;
            pitch_ = std::clamp(pitch_ - static_cast<float>(mouse.getYProperty()) * MouseSensitivity * MouseLookScale,
                                -1.48f, 1.48f);
        }
        if (touchControlState_.lookActive) {
            const float resolutionScale = 720.0f / std::max(1.0f, touchHeight);
            const float sensitivityScale = TouchControls::LookSensitivityScale(
                touchControlState_.lookSensitivity);
            yaw_ -= touchControlState_.lookDeltaX * TouchLookSensitivity
                * sensitivityScale * resolutionScale;
            pitch_ = std::clamp(
                pitch_ - touchControlState_.lookDeltaY * TouchLookSensitivity
                    * sensitivityScale * resolutionScale,
                -1.48f, 1.48f);
        }

        // H: the hand torch on or off (on foot; the quad has its lamps).
        const bool torchKeyDown = keyboard.IsKeyDown(Keys::H);
        if ((torchKeyDown && !torchKeyWasDown_ && getIsActiveProperty()) || touchControlState_.torchToggled) {
            torchOn_ = !torchOn_;
            std::cout << "island-simulator: hand torch " << (torchOn_ ? "on" : "off") << '\n';
        }
        torchKeyWasDown_ = torchKeyDown;
        // F: ride the quad or step off it.
        const bool quadKeyDown = keyboard.IsKeyDown(Keys::F);
        if ((quadKeyDown && !quadKeyWasDown_ && getIsActiveProperty()) || touchControlState_.rideToggled) ToggleQuad();
        touchControlState_.rideToggled = touchControlState_.torchToggled = false;   // each tap acts once
        quadKeyWasDown_ = quadKeyDown;
        if (riding_) {
            const bool rightKeyDown = keyboard.IsKeyDown(Keys::Back);
            if (rightKeyDown && !quadRightKeyWasDown_)
                quad_.Right([this](float x, float z) { return QuadGround(x, z); });
            quadRightKeyWasDown_ = rightKeyDown;
            const float throttle = (digitalInput.forward ? 1.0f : 0.0f) - (digitalInput.backward ? 1.0f : 0.0f)
                + touchControlState_.moveForward;
            const float steer = (digitalInput.right ? 1.0f : 0.0f) - (digitalInput.left ? 1.0f : 0.0f)
                + touchControlState_.moveRight;
            quadControls_ = {std::clamp(throttle, -1.0f, 1.0f), std::clamp(steer, -1.0f, 1.0f), keyboard.IsKeyDown(Keys::Space)};
            UpdateQuadChaseCamera(deltaSeconds);
            return;
        }
        quadPreviousYaw_ = yaw_;
        const Vector3 forward(std::sin(yaw_), 0.0f, std::cos(yaw_));
        const Vector3 right(-forward.Z, 0.0f, forward.X);
        const FirstPersonMovementIntent movementIntent = FirstPersonInput::Resolve(
            digitalInput, touchControlState_.moveForward,
            touchControlState_.moveRight, yaw_);
        const Vector3 wish(movementIntent.worldX, 0.0f, movementIntent.worldZ);
        playerMotion_.SetRunning(movementIntent.run);

        if (flightMotion_.IsActive()) {
            const float vertical=(keyboard.IsKeyDown(Keys::Space) ? 1.0f : 0.0f)
                -(runToggleDown ? 1.0f : 0.0f);
            flightMotion_.Update(wish.X,vertical,wish.Z,false,deltaSeconds);
            const auto p=flightMotion_.Position();
            cameraPosition_=Vector3(p.x,p.y,p.z);
            cameraMotion_.Reset();
        } else {
#ifndef NDEBUG
        if (debugFreeCamera_.IsActive()) {
            const float vertical = (keyboard.IsKeyDown(Keys::E) ? 1.0f : 0.0f)
                - (keyboard.IsKeyDown(Keys::Q) ? 1.0f : 0.0f);
            const bool fast = runToggleDown;
            debugFreeCamera_.Update(
                yaw_, DebugFreeCameraInput{Vector3::Dot(wish, forward),
                                            Vector3::Dot(wish, right),
                                            vertical, fast},
                deltaSeconds);
            const DebugFreeCameraPosition& freePosition = debugFreeCamera_.Position();
            cameraPosition_ = Vector3(freePosition.x, freePosition.y, freePosition.z);
            cameraMotion_.Reset();
        } else {
#endif
            const PlayerPosition positionBeforeMovement = playerMotion_.Position();
            const LocomotionMode modeBeforeMovement = playerMotion_.Mode();
            const bool ascend=keyboard.IsKeyDown(Keys::Space);
            if(modeBeforeMovement==LocomotionMode::Swimming && (ascend||runToggleDown)
                && !swimTerrainSurface_)
                swimTerrainSurface_=std::make_unique<RenderedTerrainSurface>(terrain_);
            playerMotion_.SetSwimInput(swimTerrainSurface_.get(),ascend,runToggleDown);
            playerMotion_.Update(terrain_, wish.X, wish.Z, deltaSeconds, worldCollisionGrid_);
            const PlayerPosition& position = playerMotion_.Position();
            cameraPosition_ = Vector3(position.x, position.y, position.z);
            if (!swimmingEntryDiagnosticReported_
                && modeBeforeMovement == LocomotionMode::Walking
                && playerMotion_.Mode() == LocomotionMode::Swimming) {
                constexpr std::string_view diagnostic =
                    "island-simulator: first traversal entered swimming state";
                swimmingEntryDiagnosticReported_ = true;
                std::cout << diagnostic << '\n' << std::flush;
                MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
            }
            const float travelledDistance = std::hypot(position.x - positionBeforeMovement.x,
                                                       position.z - positionBeforeMovement.z);
            const float physicalSpeed = deltaSeconds > 0.000001f
                ? travelledDistance / deltaSeconds
                : 0.0f;
            (void)cameraMotion_.Update(physicalSpeed,
                                       playerMotion_.Mode(), deltaSeconds);
#ifndef NDEBUG
        }
#endif
        }
    }

    void IslandGame::UpdateSoundscape(const float deltaSeconds) noexcept
    {
        if (!soundscape_) return;
        const PlayerPosition& position = playerMotion_.Position();
        const float sampleX=flightMotion_.IsActive()?cameraPosition_.X:position.x;
        const float sampleZ=flightMotion_.IsActive()?cameraPosition_.Z:position.z;
        const float shoreDistance=terrain_.SignedShoreDistance(sampleX,sampleZ);
        const float windExposure=CoastalWindResponse::Exposure(
            sampleX,sampleZ,shoreDistance);
        const Vector3 forward(std::sin(yaw_), 0.0f, std::cos(yaw_));
        const SoundscapeListenerState listener{
            cameraPosition_.X,
            cameraPosition_.Y,
            cameraPosition_.Z,
            forward.X * std::cos(pitch_),
            std::sin(pitch_),
            forward.Z * std::cos(pitch_),
            IsDebugFreeCameraActive() || flightMotion_.IsActive()
                ? 0.0f : playerMotion_.VelocityX(),
            IsDebugFreeCameraActive() || flightMotion_.IsActive()
                ? 0.0f : playerMotion_.VelocityZ(),
        };
        soundscape_->SetFootstepGround(FootstepGroundUnderPlayer());
        soundscape_->SetWindClock(seaForageSeconds_);
        soundscape_->Update(
            shoreDistance,
            !flightMotion_.IsActive() && playerMotion_.Mode() == LocomotionMode::Swimming,
            flightMotion_.IsActive() ? 0.0f : WalkerWaterDepth(),
            flightMotion_.IsActive() ? 0.0f
                : std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ()),
            flightMotion_.IsActive() ? 0.0f : playerMotion_.OceanCurrentStrength(terrain_),
            deltaSeconds,windExposure,listener,
            birdFlock_.Birds(), monkeyTroop_.Monkeys(), boarHerd_.Boars(),
            goats_->Goats(),snakes_->Snakes(),&bear_->State(),
            flightMotion_.IsActive()?0.0f:playerMotion_.SubmergedDepth(),
            frogPopulation_.Frogs(), cicadaColony_.Cicadas(), flySwarms_.Flies());
    }

    std::array<GrassWalker, GrassWalkers::Slots> IslandGame::GatherWalkers(const float x, const float z) const
    {
        std::vector<GrassWalker> candidates;
        for (const auto& deer : deer_->Deer()) candidates.push_back({deer.x, deer.z, 0.55f, GrassWalkers::Strength(deer.speed)});
        for (const auto& goat : goats_->Goats()) candidates.push_back({goat.x, goat.z, 0.45f, GrassWalkers::Strength(goat.speed)});
        for (const auto& boar : boarHerd_.Boars()) candidates.push_back({boar.x, boar.z, 0.62f, GrassWalkers::Strength(boar.speed)});
        candidates.push_back({bear_->State().x, bear_->State().z, 0.95f, GrassWalkers::Strength(bear_->State().speed)});
        candidates.push_back({resident_->State().x, resident_->State().z, 0.38f, GrassWalkers::Strength(resident_->State().speed)});
        std::array<GrassWalker, GrassWalkers::Slots> walkers{};
        (void)GrassWalkers::Select(candidates, x, z, walkers);
        return walkers;
    }

    std::span<const SpiderState> IslandGame::AwakeSpiders()
    {
        // The ground spiders hunt by night: by day three in four keep to their retreats under
        // the litter, and at dusk all come out (five large spiders crossing an open patch of
        // floor at noon read as an infestation). The close-ups keep the whole colony. Only those
        // within forty metres of the camera are drawn or cast shadows, so only they are kept.
        const auto all = spiders_->Spiders();
        if (UsesSpiderViewpoint()) return all;
        const float day = std::clamp((sun_light::Current().lighting.sunElevationDegrees + 2.0f) / 10.0f, 0.0f, 1.0f);
        awakeSpiders_.clear();
        for (std::size_t i = 0; i < all.size(); ++i) {
            const auto& spider = all[i];
            const float dx = spider.x - renderCameraPosition_.X, dz = spider.z - renderCameraPosition_.Z;
            if (dx * dx + dz * dz > 40.0f * 40.0f) continue;
            if (static_cast<float>((i * 61U) % 100U) / 100.0f >= 1.0f - 0.75f * day) continue;
            awakeSpiders_.push_back(spider);
        }
        return std::span<const SpiderState>(awakeSpiders_.data(), awakeSpiders_.size());
    }

    FootstepSurface IslandGame::FootstepGroundUnderPlayer() const noexcept
    {
        if (flightMotion_.IsActive() || IsDebugFreeCameraActive()) return FootstepSurface::None;
        const PlayerPosition& position = playerMotion_.Position();
        return FootstepGroundAt(position.x, position.z);
    }

    FootstepSurface IslandGame::FootstepGroundAt(const float x, const float z) const noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z)) return FootstepSurface::None;
        // Plank decks ring hollow underfoot wherever they carry the walker.
        if ((jettyPlacement_ && jettyPlacement_->OnDeck(x, z))
            || (bridgePlacement_ && bridgePlacement_->OnDeck(x, z))
            || (lookoutPlacement_ && lookoutPlacement_->OnDeck(x, z)))
            return FootstepSurface::Wood;
        const TerrainSample sample = terrain_.Sample(x, z);
        // Ankle-deep water before the swim threshold wades; the scheduler stays silent once
        // the player is actually swimming. The stream's water is waded too.
        if (sample.height < -0.06f) return FootstepSurface::Shallows;
        if (float bed = 0.0f, stream = 0.0f; StreamWaterAt(x, z, bed, stream) && stream > 0.04f) return FootstepSurface::Shallows;
        switch (sample.surface) {
        case TerrainSurface::DeepSeabed:
        case TerrainSurface::ShallowSeabed:
        case TerrainSurface::WetSand:
        case TerrainSurface::DrySand:
            return FootstepSurface::Sand;
        case TerrainSurface::SteepGround:
        case TerrainSurface::RockyGround:
            return FootstepSurface::Rock;
        case TerrainSurface::CoastalGround:
        case TerrainSurface::InteriorGround:
        default:
            break;
        }
        if (sample.coastalRockiness >= 0.60f) return FootstepSurface::Rock;
        // The same living-turf density that places grass clusters decides whether the step
        // swishes through blades or lands on bare humus.
        const auto environment = IslandEnvironmentField::SampleInland(
            x, z, sample.signedShoreDistance);
        const float turf = sample.signedShoreDistance >= 20.0f && sample.slopeRadians <= 0.4886922f
            ? GroundCover::Density(std::clamp(canopyLightField_.Sample(x, z), 0.0f, 1.0f),
                                   std::clamp(groundCoverField_.SamplePatch(x, z), 0.0f, 1.0f),
                                   sample.signedShoreDistance, sample.slopeRadians,
                                   sample.coastalRockiness, sample.beachSuitability,
                                   environment.moisture, environment.drainage)
            : 0.0f;
        if (turf >= 0.30f) return FootstepSurface::Grass;
        // Soaked bare soil squelches and splashes underfoot like the shallows' edge.
        if (weather_now::Current().wetness > 0.65f && sample.slopeRadians < 0.25f) return FootstepSurface::Shallows;
        return FootstepSurface::Soil;
    }

    bool IslandGame::IsDebugFreeCameraActive() const noexcept
    {
#ifndef NDEBUG
        return debugFreeCamera_.IsActive();
#else
        return false;
#endif
    }

    void IslandGame::UpdateCameraMatrices()
    {
        const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
        const float aspect = static_cast<float>(viewport.getWidthProperty())
            / static_cast<float>(std::max(1, viewport.getHeightProperty()));
        const float cosPitch = std::cos(pitch_);
        const Vector3 forward(std::sin(yaw_) * cosPitch, std::sin(pitch_),
                              std::cos(yaw_) * cosPitch);
        renderCameraPosition_ = cameraPosition_;
        if (viewpoint_ == FixedViewpoint::None && !smokeTest_
            && !IsDebugFreeCameraActive() && !flightMotion_.IsActive()
            && playerMotion_.SubmergedDepth()==0.0f) {
            const CameraMotionOffset& motion = cameraMotion_.Offset();
            const Vector3 right(std::cos(yaw_), 0.0f, -std::sin(yaw_));
            renderCameraPosition_ = renderCameraPosition_
                + right * motion.lateralMeters + Vector3::Up * motion.verticalMeters;
        }
        cameraView_ = Matrix::CreateLookAt(renderCameraPosition_,
                                           renderCameraPosition_ + forward,
                                           Vector3::Up);
        cameraProjection_ = Matrix::CreatePerspectiveFieldOfView(
            (viewpoint_==FixedViewpoint::BoarDetail||viewpoint_==FixedViewpoint::BoarLyingDetail||viewpoint_==FixedViewpoint::GoatDetail||viewpoint_==FixedViewpoint::GoatLyingDetail
                ||viewpoint_==FixedViewpoint::DeerDetail||viewpoint_==FixedViewpoint::DeerBeddedDetail
                ||viewpoint_==FixedViewpoint::ResidentDetail||viewpoint_==FixedViewpoint::ResidentTendDetail||viewpoint_==FixedViewpoint::ResidentFishDetail||viewpoint_==FixedViewpoint::BearDetail||viewpoint_==FixedViewpoint::BearLyingDetail
                ||viewpoint_==FixedViewpoint::MouseDetail||viewpoint_==FixedViewpoint::RatDetail
                ||viewpoint_==FixedViewpoint::FlyDetail||viewpoint_==FixedViewpoint::AntDetail
                ||viewpoint_==FixedViewpoint::CicadaDetail
                ||viewpoint_==FixedViewpoint::BeetleDetail
                ||UsesSpiderViewpoint())
                ?0.50f:std::numbers::pi_v<float> / 3.0f,
            aspect, 0.08f, 1200.0f);
    }

    void IslandGame::CaptureFrame(const std::string& path)
    {
        auto& device = getGraphicsDeviceProperty();
        const auto& viewport = device.getViewportProperty();
        const int width = viewport.getWidthProperty();
        const int height = viewport.getHeightProperty();
        std::vector<Color> pixels(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
        device.GetBackBufferData(pixels.data(), static_cast<int>(pixels.size()));

        std::ofstream output(path, std::ios::binary);
        if (!output) {
            throw std::runtime_error("Cannot create frame capture at '" + path + "'");
        }
        output << "P6\n" << width << ' ' << height << "\n255\n";
        for (const Color& pixel : pixels) {
            const std::array<char, 3> rgb = {
                static_cast<char>(pixel.getRProperty()),
                static_cast<char>(pixel.getGProperty()),
                static_cast<char>(pixel.getBProperty())};
            output.write(rgb.data(), static_cast<std::streamsize>(rgb.size()));
        }
        if (!output) {
            throw std::runtime_error("Failed while writing frame capture at '" + path + "'");
        }
        std::cout << "island-simulator: captured frame to " << path << '\n';
    }

    int IslandGame::TrackedSceneDrawCount() const noexcept
    {
        return 2 + terrainRenderer_->DrawCallCount()+streamWaterRenderer_->DrawCallCount()+caveRenderer_->DrawCallCount()
            +bunkerRenderer_->DrawCallCount()
            +campfireRenderer_->DrawCallCount()
            + palmRenderer_->DrawCallCount() + jungleRenderer_->DrawCallCount()
            + naturalPropRenderer_->DrawCallCount() + grassRenderer_->DrawCallCount()
            + shipwreckRenderer_->DrawCallCount()
            + (hutRenderer_?hutRenderer_->DrawCallCount():0)
            + helicopterRenderer_->DrawCallCount()
            + distantSceneryRenderer_->DrawCallCount()
            + contactShadowRenderer_->DrawCallCount()
            + birdRenderer_->DrawCallCount() + fruitBatRenderer_->DrawCallCount()
            + butterflyRenderer_->DrawCallCount()
            + crabRenderer_->DrawCallCount()
            + crocodileRenderer_->DrawCallCount()
            + boarRenderer_->DrawCallCount()
            + goatRenderer_->DrawCallCount()
            + deerRenderer_->DrawCallCount()
            + spiderRenderer_->DrawCallCount()
            + spiderWebRenderer_->DrawCallCount()
            + residentRenderer_->DrawCallCount()
            + bearRenderer_->DrawCallCount()
            + snakeRenderer_->DrawCallCount()
            + fishRenderer_->DrawCallCount()
            + stingrayRenderer_->DrawCallCount() + reefHeronRenderer_->DrawCallCount()
            + (anemoneRenderer_ ? anemoneRenderer_->DrawCallCount() : 0)
            + octopusRenderer_->DrawCallCount()
            + seaTurtleRenderer_->DrawCallCount()
            + sharkRenderer_->DrawCallCount()+dolphinRenderer_->DrawCallCount()
            + monkeyRenderer_->DrawCallCount()
            + shorelineRenderer_->DrawCallCount() + waterViewOverlay_->DrawCallCount();
    }

    void IslandGame::RecordBenchmarkSceneSample()
    {
        const int trackedDraws = TrackedSceneDrawCount();
        const int terrainCells = terrainRenderer_->VisibleCellCount();
        const int terrainDraws = terrainRenderer_->DrawCallCount();
        const int contactShadows = contactShadowRenderer_->VisibleShadowCount();
        const int palms = palmRenderer_->VisiblePalmCount();
        const int canopy = jungleRenderer_->VisibleCanopyCount();
        const int grass = grassRenderer_->VisiblePatchCount();
        const int crabs = crabRenderer_->VisibleCrabCount();
        const int boars = boarRenderer_->VisibleCount();
        const int butterflies = butterflyRenderer_->VisibleButterflyCount();
        const int seaTurtles = seaTurtleRenderer_->VisibleTurtleCount();
        const int dolphins = dolphinRenderer_->VisibleCount();
        const float shoreDistance = terrain_.SignedShoreDistance(
            cameraPosition_.X, cameraPosition_.Z);
        BenchmarkSceneRanges& ranges = benchmarkSceneRanges_;
        if (ranges.sampleCount == 0U) {
            ranges.minimumTrackedDraws = ranges.maximumTrackedDraws = trackedDraws;
            ranges.minimumTerrainCells = ranges.maximumTerrainCells = terrainCells;
            ranges.minimumTerrainDraws = ranges.maximumTerrainDraws = terrainDraws;
            ranges.minimumContactShadows = ranges.maximumContactShadows = contactShadows;
            ranges.minimumPalms = ranges.maximumPalms = palms;
            ranges.minimumCanopy = ranges.maximumCanopy = canopy;
            ranges.minimumGrass = ranges.maximumGrass = grass;
            ranges.minimumCrabs = ranges.maximumCrabs = crabs;
            ranges.minimumBoars = ranges.maximumBoars = boars;
            ranges.minimumButterflies = ranges.maximumButterflies = butterflies;
            ranges.minimumSeaTurtles = ranges.maximumSeaTurtles = seaTurtles;
            ranges.minimumDolphins = ranges.maximumDolphins = dolphins;
            ranges.minimumShoreDistance = ranges.maximumShoreDistance = shoreDistance;
            ranges.firstCameraPosition = cameraPosition_;
        } else {
            ranges.minimumTrackedDraws = std::min(ranges.minimumTrackedDraws, trackedDraws);
            ranges.maximumTrackedDraws = std::max(ranges.maximumTrackedDraws, trackedDraws);
            ranges.minimumTerrainCells = std::min(ranges.minimumTerrainCells, terrainCells);
            ranges.maximumTerrainCells = std::max(ranges.maximumTerrainCells, terrainCells);
            ranges.minimumTerrainDraws = std::min(ranges.minimumTerrainDraws, terrainDraws);
            ranges.maximumTerrainDraws = std::max(ranges.maximumTerrainDraws, terrainDraws);
            ranges.minimumContactShadows = std::min(
                ranges.minimumContactShadows, contactShadows);
            ranges.maximumContactShadows = std::max(
                ranges.maximumContactShadows, contactShadows);
            ranges.minimumPalms = std::min(ranges.minimumPalms, palms);
            ranges.maximumPalms = std::max(ranges.maximumPalms, palms);
            ranges.minimumCanopy = std::min(ranges.minimumCanopy, canopy);
            ranges.maximumCanopy = std::max(ranges.maximumCanopy, canopy);
            ranges.minimumGrass = std::min(ranges.minimumGrass, grass);
            ranges.maximumGrass = std::max(ranges.maximumGrass, grass);
            ranges.minimumCrabs = std::min(ranges.minimumCrabs, crabs);
            ranges.maximumCrabs = std::max(ranges.maximumCrabs, crabs);
            ranges.minimumBoars = std::min(ranges.minimumBoars, boars);
            ranges.maximumBoars = std::max(ranges.maximumBoars, boars);
            ranges.minimumButterflies = std::min(
                ranges.minimumButterflies, butterflies);
            ranges.maximumButterflies = std::max(
                ranges.maximumButterflies, butterflies);
            ranges.minimumSeaTurtles = std::min(
                ranges.minimumSeaTurtles, seaTurtles);
            ranges.maximumSeaTurtles = std::max(
                ranges.maximumSeaTurtles, seaTurtles);
            ranges.minimumDolphins = std::min(ranges.minimumDolphins, dolphins);
            ranges.maximumDolphins = std::max(ranges.maximumDolphins, dolphins);
            ranges.minimumShoreDistance = std::min(
                ranges.minimumShoreDistance, shoreDistance);
            ranges.maximumShoreDistance = std::max(
                ranges.maximumShoreDistance, shoreDistance);
        }
        ranges.totalTrackedDraws += static_cast<std::uint64_t>(trackedDraws);
        ranges.totalTerrainCells += static_cast<std::uint64_t>(terrainCells);
        ranges.totalTerrainDraws += static_cast<std::uint64_t>(terrainDraws);
        ranges.totalContactShadows += static_cast<std::uint64_t>(contactShadows);
        ranges.totalPalms += static_cast<std::uint64_t>(palms);
        ranges.totalCanopy += static_cast<std::uint64_t>(canopy);
        ranges.totalGrass += static_cast<std::uint64_t>(grass);
        ranges.totalCrabs += static_cast<std::uint64_t>(crabs);
        ranges.totalBoars += static_cast<std::uint64_t>(boars);
        ranges.totalButterflies += static_cast<std::uint64_t>(butterflies);
        ranges.totalSeaTurtles += static_cast<std::uint64_t>(seaTurtles);
        ranges.totalDolphins += static_cast<std::uint64_t>(dolphins);
        ranges.lastCameraPosition = cameraPosition_;
        ++ranges.sampleCount;
    }

    void IslandGame::RecordBenchmarkFrame()
    {
        const auto now = std::chrono::steady_clock::now();
        if (!benchmarkHasPreviousFrame_) {
            benchmarkPreviousFrameTime_ = now;
            benchmarkHasPreviousFrame_ = true;
            return;
        }

        const double frameMilliseconds =
            std::chrono::duration<double, std::milli>(now - benchmarkPreviousFrameTime_).count();
        benchmarkPreviousFrameTime_ = now;
        if (benchmarkWarmupIntervals_ < BenchmarkWarmupIntervals) {
            ++benchmarkWarmupIntervals_;
            return;
        }
        benchmarkFrameMilliseconds_.push_back(frameMilliseconds);
        benchmarkSceneSamplePending_ = true;
        if (benchmarkFrameMilliseconds_.size() >= BenchmarkSampleIntervals) {
            benchmarkReportPending_ = true;
        }
    }

    void IslandGame::WriteBenchmarkReport()
    {
        const FrameTimeSummary summary =
            CalculateFrameTimeSummary(benchmarkFrameMilliseconds_);
        const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
        const int trackedSceneDraws = TrackedSceneDrawCount();
        const BenchmarkSceneRanges& ranges = benchmarkSceneRanges_;
        const auto meanCount = [&ranges](const std::uint64_t total) {
            return ranges.sampleCount == 0U ? 0.0
                : static_cast<double>(total) / static_cast<double>(ranges.sampleCount);
        };

        std::ofstream output(benchmarkPath_);
        if (!output) {
            throw std::runtime_error(
                "Cannot create benchmark report at '" + benchmarkPath_ + "'");
        }
        output.imbue(std::locale::classic());
        output << std::fixed << std::setprecision(4)
               << "{\n"
               << "  \"schema_version\": 3,\n"
               << "  \"renderer\": \""
               << getGraphicsDeviceProperty().GetGraphicsRendererName() << "\",\n"
               << "  \"viewpoint\": \"" << ViewpointName(viewpoint_) << "\",\n"
               << "  \"resolution\": {\"width\": " << viewport.getWidthProperty()
               << ", \"height\": " << viewport.getHeightProperty() << "},\n"
               << "  \"settings\": {\"vsync\": false, \"fixed_timestep\": false, "
               << "\"warmup_intervals\": " << BenchmarkWarmupIntervals
               << ", \"sample_intervals\": " << BenchmarkSampleIntervals << "},\n"
               << "  \"timing\": {\n"
               << "    \"sample_count\": " << summary.sampleCount << ",\n"
               << "    \"mean_ms\": " << summary.meanMilliseconds << ",\n"
               << "    \"median_ms\": " << summary.medianMilliseconds << ",\n"
               << "    \"p95_ms\": " << summary.percentile95Milliseconds << ",\n"
               << "    \"p99_ms\": " << summary.percentile99Milliseconds << ",\n"
               << "    \"min_ms\": " << summary.minimumMilliseconds << ",\n"
               << "    \"max_ms\": " << summary.maximumMilliseconds << ",\n"
               << "    \"mean_fps\": " << summary.meanFramesPerSecond << ",\n"
               << "    \"budget_misses\": {\"frames_over_8_333_ms\": "
               << summary.framesOver120HzBudget
               << ", \"frames_over_16_667_ms\": " << summary.framesOver60HzBudget
               << ", \"frames_over_33_333_ms\": " << summary.framesOver30HzBudget
               << "}\n"
               << "  },\n"
               << "  \"route\": ";
        if (viewpoint_ == FixedViewpoint::BenchmarkTraversal) {
            output << "{\n"
                   << "    \"name\": \"island_traversal\",\n"
                   << "    \"waypoint_count\": "
                   << benchmarkTraversalRoute_.Waypoints().size() << ",\n"
                   << "    \"approximate_horizontal_distance_m\": "
                   << benchmarkTraversalRoute_.ApproximateLength() << ",\n"
                   << "    \"sampled_shore_distance_m\": {\"min\": "
                   << ranges.minimumShoreDistance << ", \"max\": "
                   << ranges.maximumShoreDistance << "},\n"
                   << "    \"first_camera_m\": {\"x\": "
                   << ranges.firstCameraPosition.X << ", \"y\": "
                   << ranges.firstCameraPosition.Y << ", \"z\": "
                   << ranges.firstCameraPosition.Z << "},\n"
                   << "    \"last_camera_m\": {\"x\": "
                   << ranges.lastCameraPosition.X << ", \"y\": "
                   << ranges.lastCameraPosition.Y << ", \"z\": "
                   << ranges.lastCameraPosition.Z << "}\n"
                   << "  },\n";
        } else {
            output << "null,\n";
        }
        output << "  \"scene_range\": {\n"
               << "    \"sample_count\": " << ranges.sampleCount << ",\n"
               << "    \"tracked_draw_submissions\": {\"min\": "
               << ranges.minimumTrackedDraws << ", \"max\": "
               << ranges.maximumTrackedDraws << ", \"mean\": "
               << meanCount(ranges.totalTrackedDraws) << "},\n"
               << "    \"terrain_cells\": {\"min\": "
               << ranges.minimumTerrainCells << ", \"max\": "
               << ranges.maximumTerrainCells << ", \"mean\": "
               << meanCount(ranges.totalTerrainCells) << "},\n"
               << "    \"terrain_draw_submissions\": {\"min\": "
               << ranges.minimumTerrainDraws << ", \"max\": "
               << ranges.maximumTerrainDraws << ", \"mean\": "
               << meanCount(ranges.totalTerrainDraws) << "},\n"
               << "    \"contact_shadows\": {\"min\": "
               << ranges.minimumContactShadows << ", \"max\": "
               << ranges.maximumContactShadows << ", \"mean\": "
               << meanCount(ranges.totalContactShadows) << "},\n"
               << "    \"palms\": {\"min\": " << ranges.minimumPalms
               << ", \"max\": " << ranges.maximumPalms << ", \"mean\": "
               << meanCount(ranges.totalPalms) << "},\n"
               << "    \"canopy\": {\"min\": " << ranges.minimumCanopy
               << ", \"max\": " << ranges.maximumCanopy << ", \"mean\": "
               << meanCount(ranges.totalCanopy) << "},\n"
               << "    \"grass_clusters\": {\"min\": "
               << ranges.minimumGrass << ", \"max\": "
               << ranges.maximumGrass << ", \"mean\": "
               << meanCount(ranges.totalGrass) << "},\n"
               << "    \"crabs\": {\"min\": " << ranges.minimumCrabs
               << ", \"max\": " << ranges.maximumCrabs << ", \"mean\": "
               << meanCount(ranges.totalCrabs) << "},\n"
               << "    \"boars\": {\"min\": " << ranges.minimumBoars
               << ", \"max\": " << ranges.maximumBoars << ", \"mean\": "
               << meanCount(ranges.totalBoars) << "},\n"
               << "    \"butterflies\": {\"min\": "
               << ranges.minimumButterflies << ", \"max\": "
               << ranges.maximumButterflies << ", \"mean\": "
               << meanCount(ranges.totalButterflies) << "},\n"
               << "    \"sea_turtles\": {\"min\": "
               << ranges.minimumSeaTurtles << ", \"max\": "
               << ranges.maximumSeaTurtles << ", \"mean\": "
               << meanCount(ranges.totalSeaTurtles) << "},\n"
               << "    \"dolphins\": {\"min\": "
               << ranges.minimumDolphins << ", \"max\": "
               << ranges.maximumDolphins << ", \"mean\": "
               << meanCount(ranges.totalDolphins) << "}\n"
               << "  },\n"
               << "  \"scene\": {\n"
               << "    \"tracked_draw_submissions\": " << trackedSceneDraws << ",\n"
               << "    \"terrain_cells\": " << terrainRenderer_->VisibleCellCount() << ",\n"
               << "    \"terrain_draw_submissions\": "
               << terrainRenderer_->DrawCallCount() << ",\n"
               << "    \"distant_islets\": "
               << distantSceneryRenderer_->IsletCount() << ",\n"
               << "    \"contact_shadows\": "
               << contactShadowRenderer_->VisibleShadowCount() << ",\n"
               << "    \"contact_shadow_triangles\": "
               << contactShadowRenderer_->PrimitiveCount() << ",\n"
               << "    \"coastal_contact_shadows\": "
               << contactShadowRenderer_->VisibleCoastalShadowCount() << ",\n"
               << "    \"palms\": " << palmRenderer_->VisiblePalmCount() << ",\n"
               << "    \"palms_near\": " << palmRenderer_->VisibleNearPalmCount() << ",\n"
               << "    \"palms_far\": " << palmRenderer_->VisibleFarPalmCount() << ",\n"
               << "    \"canopy\": " << jungleRenderer_->VisibleCanopyCount() << ",\n"
               << "    \"canopy_near\": "
               << jungleRenderer_->VisibleCanopyNearCount() << ",\n"
               << "    \"canopy_far\": "
               << jungleRenderer_->VisibleCanopyFarCount() << ",\n"
               << "    \"canopy_frustum_rejected\": "
               << jungleRenderer_->CanopyFrustumRejectedCount() << ",\n"
               << "    \"understory\": " << jungleRenderer_->VisibleUnderstoryCount() << ",\n"
               << "    \"bushes\": " << jungleRenderer_->VisibleBushCount() << ",\n"
               << "    \"ground_plants\": " << jungleRenderer_->VisibleGroundPlantCount()
               << ",\n"
               << "    \"grass_clusters\": " << grassRenderer_->VisiblePatchCount() << ",\n"
               << "    \"grass_interaction\": "
               << grassInteractionModel_.Strength() << ",\n"
               << "    \"rocks\": " << naturalPropRenderer_->VisibleRockCount() << ",\n"
               << "    \"reef_outcrops\": "
               << naturalPropRenderer_->VisibleReefRockCount() << ",\n"
               << "    \"reef_habitat\": "
               << naturalPropRenderer_->VisibleReefHabitatCount() << ",\n"
               << "    \"fallen_logs\": " << naturalPropRenderer_->VisibleLogCount() << ",\n"
               << "    \"driftwood\": " << naturalPropRenderer_->VisibleDriftwoodCount()
               << ",\n"
               << "    \"fallen_fronds\": "
               << naturalPropRenderer_->VisibleFallenFrondCount() << ",\n"
               << "    \"beach_details\": "
               << naturalPropRenderer_->VisibleBeachDetailCount() << ",\n"
               << "    \"birds\": " << birdRenderer_->VisibleBirdCount() << ",\n"
               << "    \"butterflies\": "
               << butterflyRenderer_->VisibleButterflyCount() << ",\n"
               << "    \"crabs\": " << crabRenderer_->VisibleCrabCount() << ",\n"
               << "    \"boars\": " << boarRenderer_->VisibleCount() << ",\n"
               << "    \"goats\": " << goatRenderer_->VisibleCount() << ",\n"
               << "    \"spiders\": " << spiderRenderer_->VisibleCount() << ",\n"
               << "    \"canopy_webs\": " << spiderWebRenderer_->VisibleCount() << ",\n"
               << "    \"web_strands\": " << spiderWebRenderer_->StrandCount() << ",\n"
               << "    \"fish\": " << fishRenderer_->VisibleFishCount() << ",\n"
               << "    \"sea_turtles\": "
               << seaTurtleRenderer_->VisibleTurtleCount() << ",\n"
               << "    \"shark\": " << (sharkRenderer_->IsVisible() ? 1 : 0) << ",\n"
               << "    \"dolphins\": " << dolphinRenderer_->VisibleCount() << ",\n"
               << "    \"player_water_ripple\": "
               << waterRippleModel_.Intensity() << ",\n"
               << "    \"player_water_ripple_phase\": "
               << waterRippleModel_.Phase() << ",\n"
               << "    \"monkeys\": " << monkeyRenderer_->VisibleMonkeyCount() << "\n"
               << "  }\n"
               << "}\n";
        if (!output) {
            throw std::runtime_error(
                "Failed while writing benchmark report at '" + benchmarkPath_ + "'");
        }
        std::cout << "island-simulator: benchmark completed " << summary.sampleCount
                  << " samples, mean " << summary.meanMilliseconds << " ms, p95 "
                  << summary.percentile95Milliseconds << " ms, report " << benchmarkPath_
                  << '\n';
    }

    void IslandGame::RecordStartupTiming(
        const std::chrono::steady_clock::time_point drawStarted)
    {
        if (startupTimingRecorded_) {
            return;
        }
        const auto drawFinished = std::chrono::steady_clock::now();
        postContentMilliseconds_ = std::chrono::duration<double, std::milli>(
            drawStarted - contentFinished_).count();
        firstDrawMilliseconds_ = std::chrono::duration<double, std::milli>(
            drawFinished - drawStarted).count();
        totalStartupMilliseconds_ = std::chrono::duration<double, std::milli>(
            drawFinished - constructionStarted_).count();
        startupTimingRecorded_ = true;

        std::ostringstream status;
        status.imbue(std::locale::classic());
        status << std::fixed << std::setprecision(3)
               << "island-simulator: startup world " << worldGenerationMilliseconds_
               << " ms, framework " << frameworkInitializationMilliseconds_
               << " ms, content " << contentCreationMilliseconds_
               << " ms, pre-draw " << postContentMilliseconds_
               << " ms, first draw " << firstDrawMilliseconds_
               << " ms, total " << totalStartupMilliseconds_ << " ms\n";
        const std::string startupDiagnostic = status.str();
        std::cout << startupDiagnostic;
        MirrorRuntimeDiagnosticToPlatformLog(startupDiagnostic);
        if (!startupReportPath_.empty()) {
            WriteStartupReport();
            std::cout << "island-simulator: startup report " << startupReportPath_
                      << " first draw recorded\n";
        }
    }

    void IslandGame::WriteStartupReport()
    {
        std::ofstream output(startupReportPath_, std::ios::binary | std::ios::trunc);
        if (!output) {
            throw std::runtime_error(
                "Cannot create startup report at '" + startupReportPath_ + "'");
        }
        const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
        output.imbue(std::locale::classic());
        output << std::fixed << std::setprecision(4)
               << "{\n"
               << "  \"schema_version\": 1,\n"
               << "  \"renderer\": \""
               << getGraphicsDeviceProperty().GetGraphicsRendererName() << "\",\n"
               << "  \"viewpoint\": \"" << ViewpointName(viewpoint_) << "\",\n"
               << "  \"resolution\": {\"width\": " << viewport.getWidthProperty()
               << ", \"height\": " << viewport.getHeightProperty() << "},\n"
               << "  \"audio\": {\"attempted\": "
               << (soundscape_ && soundscape_->WasStartAttempted() ? "true" : "false")
               << ", \"available\": "
               << (soundscape_ && soundscape_->IsAvailable() ? "true" : "false")
               << ", \"deferred_for_gesture\": "
               << (soundscapeNeedsGesture_ ? "true" : "false")
               << ", \"skipped_for_smoke_test\": "
               << (smokeTest_ ? "true" : "false") << "},\n"
               << "  \"timing_ms\": {\n"
               << "    \"world_generation\": " << worldGenerationMilliseconds_ << ",\n"
               << "    \"framework_initialization\": "
               << frameworkInitializationMilliseconds_ << ",\n"
               << "    \"gpu_content_creation\": " << contentCreationMilliseconds_ << ",\n"
               << "    \"post_content_to_first_draw\": " << postContentMilliseconds_ << ",\n"
               << "    \"first_draw\": " << firstDrawMilliseconds_ << ",\n"
               << "    \"total\": " << totalStartupMilliseconds_ << "\n"
               << "  }\n"
               << "}\n";
        if (!output) {
            throw std::runtime_error(
                "Failed while writing startup report at '" + startupReportPath_ + "'");
        }
    }

    void IslandGame::Draw(const GameTime& gameTime)
    {
        const std::size_t treeQaStage = drawnFrames_/4U;
        const std::span<const MonkeyState> renderedMonkeys = treeQaStage < monkeyTreeQaPoses_.size()
            ? std::span<const MonkeyState>(monkeyTreeQaPoses_[treeQaStage]) : monkeyTroop_.Monkeys();
        const auto drawStarted = std::chrono::steady_clock::now();
        const float timeSeconds = canopyLodInspection_ ? 1.0f : UsesStreamCaptureClock()
            ?static_cast<float>(drawnFrames_+1U)*.1f:UsesCampfireCaptureClock()
            ?static_cast<float>(campfireCaptureUpdates_)*.1f:UsesSnakeCaptureClock()
            ? static_cast<float>(snakeCaptureUpdates_)*0.1f : UsesGoatCaptureClock()
            ? static_cast<float>(goatCaptureUpdates_)*0.1f : UsesSpiderCaptureClock()
            ? static_cast<float>(spiderCaptureUpdates_)*0.1f : UsesBoarCaptureClock()
            ? static_cast<float>(boarCaptureUpdates_)*0.1f : UsesBearCaptureClock()
            ? static_cast<float>(bearCaptureUpdates_)*0.1f : UsesMonkeyCaptureClock()
            ? static_cast<float>(monkeyCaptureUpdates_)*0.1f : static_cast<float>(gameTime.getTotalGameTimeProperty()
                                                         .getTotalSecondsProperty());
        auto& device = getGraphicsDeviceProperty();
        BeginGpuDiagnosticFrame(drawnFrames_, drawnFrames_ < 2U
            && (viewpoint_ == FixedViewpoint::SnakeDetail || viewpoint_ == FixedViewpoint::SnakeHeadDetail
                || viewpoint_==FixedViewpoint::TreeSnakeDetail));
        if (drawnFrames_ == 1U) RecordRuntimeThreadInventory();
        if (!benchmarkPath_.empty() && !benchmarkReportPending_) {
            RecordBenchmarkFrame();
        }
        debugOverlay_->UpdateFrameTiming();
        {
            GpuDiagnosticScope gpuPass("clear");
            device.Clear(Color(136, 202, 224, 255), 1.0f);
        }
        UpdateCameraMatrices();
        if (sunShadowMap_) {
            if (!sunShadowReceiversBound_) {
                terrainRenderer_->SetSunShadowMap(sunShadowMap_.get());
                grassRenderer_->SetSunShadowMap(sunShadowMap_.get());
                jungleRenderer_->SetSunShadowMap(sunShadowMap_.get());
                palmRenderer_->SetSunShadowMap(sunShadowMap_.get());
                if (hutRenderer_) hutRenderer_->SetSunShadowMap(sunShadowMap_.get());
                naturalPropRenderer_->SetSunShadowMap(sunShadowMap_.get());
                if (jettyRenderer_) jettyRenderer_->SetSunShadowMap(sunShadowMap_.get());
                if (bridgeRenderer_) bridgeRenderer_->SetSunShadowMap(sunShadowMap_.get());
                if (lookoutRenderer_) lookoutRenderer_->SetSunShadowMap(sunShadowMap_.get());
                helicopterRenderer_->SetSunShadowMap(sunShadowMap_.get());
                if (quadRenderer_) quadRenderer_->SetSunShadowMap(sunShadowMap_.get());
                shipwreckRenderer_->SetSunShadowMap(sunShadowMap_.get());
                walkerShadowRenderer_->SetSunShadowMap(sunShadowMap_.get());
                sunShadowReceiversBound_ = true;
            }
            SunShadowMap::SetCloudClock(timeSeconds);
            if (sunShadowMap_->Prepare(renderCameraPosition_)) {
                GpuDiagnosticScope gpuPass("sun-shadow");
                sunShadowMap_->Begin();
                device.setRasterizerStateProperty(RasterizerState::CullNone);
                device.setBlendStateProperty(BlendState::Opaque);
                device.setDepthStencilStateProperty(DepthStencilState::Default);
                jungleRenderer_->DrawSunShadow(*sunShadowMap_);
                palmRenderer_->DrawSunShadow(*sunShadowMap_);
                if (hutRenderer_) hutRenderer_->DrawSunShadow(*sunShadowMap_);
                if (jettyRenderer_) jettyRenderer_->DrawSunShadow(*sunShadowMap_);
                if (bridgeRenderer_) bridgeRenderer_->DrawSunShadow(*sunShadowMap_);
                if (lookoutRenderer_) lookoutRenderer_->DrawSunShadow(*sunShadowMap_);
                shipwreckRenderer_->DrawSunShadow(*sunShadowMap_);
                helicopterRenderer_->DrawSunShadow(*sunShadowMap_);
                naturalPropRenderer_->DrawSunShadow(*sunShadowMap_);
                sunShadowMap_->End();
            }
        }
        {
            // Under the sea surface the water limits the swimmer's sight (UnderwaterFog).
            const Vector3& eye = renderCameraPosition_;
            const bool overSea = std::isfinite(eye.X) && std::isfinite(eye.Z)
                && terrain_.Height(eye.X, eye.Z) < 0.0f && terrain_.SignedShoreDistance(eye.X, eye.Z) < 0.0f;
            underwater_fog::Update(overSea && eye.Y < 0.0f ? -eye.Y : 0.0f);
        }
        {
            GpuDiagnosticScope gpuPass("sky");
            skyRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,timeSeconds);
        }
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        {
            GpuDiagnosticScope gpuPass("terrain");
            terrainRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,timeSeconds);
            caveRenderer_->Draw(cameraView_,cameraProjection_);
            bunkerRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
            campfireRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,timeSeconds,false);
        }
        {
            GpuDiagnosticScope gpuPass("distant-scenery");
            distantSceneryRenderer_->Draw(cameraView_, cameraProjection_);
        }
        {
            GpuDiagnosticScope gpuPass("contact-shadows");
            ContactShadowRenderer::PlayerShadow playerShadow;
            if (!flightMotion_.IsActive() && !IsDebugFreeCameraActive()
                && playerMotion_.Mode() == LocomotionMode::Walking
                && playerMotion_.GroundHeight() > -0.06f) {
                const PlayerPosition& walker = playerMotion_.Position();
                playerShadow = {walker.x, walker.z, true};
            }
            contactShadowRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                         renderedMonkeys, boarHerd_.Boars(),*snakes_,goats_->Goats(),AwakeSpiders(),
                                         playerShadow);
            residentRenderer_->DrawShadow(cameraView_,cameraProjection_,renderCameraPosition_,resident_->State());
            walkerShadowRenderer_->BeginFrame();
            const auto& light = sun_light::Current();
            const auto toSun = light.shading;
            // the ground the figures fall on: the drawn terrain, or the jetty's, the bridge's or the
            // lookout's planks where they cross it
            const auto groundAt = [this](const float x, const float z) {
                float height = DrawnOrAnalyticHeight(*drawnGround_, terrain_, x, z);
                if (jettyPlacement_) height = jettyPlacement_->SupportHeight(x, z, height);
                if (bridgePlacement_) height = bridgePlacement_->SupportHeight(x, z, height);
                if (lookoutPlacement_) height = lookoutPlacement_->SupportHeight(x, z, height);
                return height;
            };
            {
                // The resident's figure too, laid from their posed skeleton, within 80 m and where
                // they stand on the drawn ground (not on the jetty's deck or the lookout).
                const ResidentState& resident = resident_->State();
                const float ground = groundAt(resident.x, resident.z);
                if (Vector3::DistanceSquared(Vector3(resident.x, resident.y, resident.z), renderCameraPosition_) < 80.0f * 80.0f
                    && std::abs(resident.y - ground) < 0.25f) {
                    const walker_shadow::Shape figure = walker_shadow::Lay(walker_shadow::ResidentLimbs(resident),
                        resident.x, ground, resident.z, toSun.x, toSun.y, toSun.z, resident.pose.pelvisY + 0.83f - ground);
                    walkerShadowRenderer_->Draw(cameraView_, cameraProjection_, figure, Vector3(resident.x, ground, resident.z),
                        Vector3(toSun.x, toSun.y, toSun.z), light.lighting.shadowStrength, groundAt);
                }
            }
            for (const DeerState& deer : deer_->Deer()) {
                // and the deer's, which had no shadow at all: within 60 m, on the drawn ground
                const float ground = groundAt(deer.x, deer.z);
                if (Vector3::DistanceSquared(Vector3(deer.x, deer.y, deer.z), renderCameraPosition_) > 60.0f * 60.0f
                    || std::abs(deer.y - ground) > 0.25f) continue;
                const float stature = (deer.antlered ? 2.6f : 2.1f) * deer.scale * (1.0f - 0.35f * std::clamp(deer.bedAmount, 0.0f, 1.0f));
                walkerShadowRenderer_->Draw(cameraView_, cameraProjection_,
                    walker_shadow::Lay(walker_shadow::DeerLimbs(deer), deer.x, ground, deer.z, toSun.x, toSun.y, toSun.z, stature),
                    Vector3(deer.x, ground, deer.z), Vector3(toSun.x, toSun.y, toSun.z), light.lighting.shadowStrength, groundAt);
            }
            {
                // and the bear's
                const BearState& bear = bear_->State();
                const float ground = groundAt(bear.x, bear.z);
                if (Vector3::DistanceSquared(Vector3(bear.x, bear.y, bear.z), renderCameraPosition_) < 60.0f * 60.0f
                    && std::abs(bear.y - ground) < 0.25f)
                    walkerShadowRenderer_->Draw(cameraView_, cameraProjection_,
                        walker_shadow::Lay(walker_shadow::BearLimbs(bear), bear.x, ground, bear.z, toSun.x, toSun.y, toSun.z, 1.8f),
                        Vector3(bear.x, ground, bear.z), Vector3(toSun.x, toSun.y, toSun.z), light.lighting.shadowStrength, groundAt);
            }
            for (const GoatState& goat : goats_->Goats()) {
                // the goats' and the boars' too, from the poses their renderers draw
                const float ground = groundAt(goat.x, goat.z);
                if (Vector3::DistanceSquared(Vector3(goat.x, goat.y, goat.z), renderCameraPosition_) > 60.0f * 60.0f
                    || std::abs(goat.y - ground) > 0.25f) continue;
                walkerShadowRenderer_->Draw(cameraView_, cameraProjection_,
                    walker_shadow::Lay(walker_shadow::GoatLimbs(goat), goat.x, ground, goat.z, toSun.x, toSun.y, toSun.z,
                                       1.5f * goat.scale),
                    Vector3(goat.x, ground, goat.z), Vector3(toSun.x, toSun.y, toSun.z), light.lighting.shadowStrength, groundAt);
            }
            for (const BoarState& boar : boarHerd_.Boars()) {
                const float ground = groundAt(boar.x, boar.z);
                if (Vector3::DistanceSquared(Vector3(boar.x, boar.y, boar.z), renderCameraPosition_) > 60.0f * 60.0f
                    || std::abs(boar.y - ground) > 0.25f) continue;
                std::optional<BoarGroundPose> fitted;
                try { fitted = FitBoarGroundPose(terrain_, *drawnGround_, boar); } catch (const std::exception&) { continue; }
                walkerShadowRenderer_->Draw(cameraView_, cameraProjection_,
                    walker_shadow::Lay(walker_shadow::BoarLimbs(boar, *fitted), boar.x, ground, boar.z, toSun.x, toSun.y, toSun.z,
                                       1.1f * boar.scale),
                    Vector3(boar.x, ground, boar.z), Vector3(toSun.x, toSun.y, toSun.z), light.lighting.shadowStrength, groundAt);
            }
            if (playerShadow.visible) {
                // ...and the figure the sun lays from the walker's feet, where the feet stand on the
                // drawn ground or a deck (not on a rock top above them) and out of the cave.
                const PlayerPosition& walker = playerMotion_.Position();
                const float ground = groundAt(walker.x, walker.z);
                const bool inCave = terrainRenderer_ && terrainRenderer_->CaveGround().Contains(walker.x, walker.z)
                    && cameraPosition_.Y < terrain_.Height(cameraPosition_.X, cameraPosition_.Z) - 0.5f;
                // Only the walker's own eye sees the walker's body cast: a QA close-up camera set low
                // by a crab or a gecko is no one standing there (the contact and walker-shadow views
                // stand the walker in their frame on purpose).
                const bool ownEye = std::abs(renderCameraPosition_.Y - (playerMotion_.GroundHeight() + PlayerMotion::EyeHeight)) < 0.35f
                    && std::hypot(renderCameraPosition_.X - walker.x, renderCameraPosition_.Z - walker.z) < 0.5f;
                const bool framed = viewpoint_ == FixedViewpoint::GroundContactDetail || viewpoint_ == FixedViewpoint::WalkerShadowDetail;
                if (std::abs(playerMotion_.GroundHeight() - ground) < 0.25f && !inCave && (ownEye || framed)) {
                    const walker_shadow::Shape figure = walker_shadow::Build(
                        {std::sin(walkerFacingYaw_), std::cos(walkerFacingYaw_), walkerGaitPhase_, walkerSwing_},
                        toSun.x, toSun.y, toSun.z);
                    walkerShadowRenderer_->Draw(cameraView_, cameraProjection_, figure, Vector3(walker.x, ground, walker.z),
                        Vector3(toSun.x, toSun.y, toSun.z), light.lighting.shadowStrength, groundAt);
                }
            }
            if (quadRenderer_ && quadPlaced_) {
                const auto q = quad_.Position();
                const BikeGround ground = QuadGround(q.x, q.z);
                quadRenderer_->DrawShadow(cameraView_, cameraProjection_, renderCameraPosition_, quad_,
                    riding_ && !quad_.Overturned(), BikeVec{q.x, ground.height, q.z}, ground.normal,
                    sun_light::Current().lighting.shadowStrength,
                    [this](const float x, const float z) { return QuadGround(x, z).height; });
            }
            bearRenderer_->DrawShadow(cameraView_,cameraProjection_,renderCameraPosition_,bear_->State());
            birdShadowRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,birdFlock_.Birds());
            seabedShadowRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,
                fishSchools_.Fish(),stingrayGroup_.Stingrays(),seaTurtleGroup_.Turtles(),&sharkPatrol_.State());
            if (waterholeRenderer_) waterholeRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
            footprintRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,footprints_.Prints());
            if(tyreTrackRenderer_) tyreTrackRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,tyreTracks_.Tracks());
            if(turtleTrackRenderer_) turtleTrackRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
            for(const auto& trail:deerPrints_) if(trail.ActiveCount()>0U) footprintRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,trail.Prints());
            for(const auto& trail:boarPrints_) if(trail.ActiveCount()>0U) footprintRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,trail.Prints());
            if(residentPrints_.ActiveCount()>0U) footprintRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,residentPrints_.Prints());
            for(const auto& trail:goatPrints_) if(trail.ActiveCount()>0U) footprintRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,trail.Prints());
            // (the crocodiles' grooves first: the hermit crabs' draw keeps the renderer's counts)
            shellDragRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,crocodileDrags_.Segments());
            shellDragRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,shellDrags_.Segments());
            wallowScarRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
        }

        {
            GpuDiagnosticScope gpuPass("natural-props");
            naturalPropRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_);
        }
        {
            GpuDiagnosticScope gpuPass("shipwreck");
            shipwreckRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_);
        }
        if(jettyRenderer_) {
            GpuDiagnosticScope gpuPass("jetty");
            jettyRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
        }
        if(bridgeRenderer_) {
            GpuDiagnosticScope gpuPass("bridge");
            bridgeRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
        }
        if(lookoutRenderer_) {
            GpuDiagnosticScope gpuPass("lookout");
            lookoutRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
        }
        if(fallingFruitRenderer_) {
            GpuDiagnosticScope gpuPass("falling-fruit");
            fallingFruitRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,fallingFruit_.Fruit());
            hatchlingRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,hatchlings_.Hatchlings());
            forestTwigRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
        }
        if(hutRenderer_) {
            GpuDiagnosticScope gpuPass("hut");
            hutRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,timeSeconds);
        }
        {
            GpuDiagnosticScope gpuPass("helicopter");
            helicopterRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_);
        }
        if (quadRenderer_ && quadPlaced_) {
            GpuDiagnosticScope gpuPass("quad-bike");
            const bool seated = riding_ && !quad_.Overturned();
            const bool avatar = seated && quadRider_ && quadRider_->Available();
            quadRenderer_->SetDirt(quadDirt_, quadDirtWet_);
            quadRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, quad_, quadEngineOn_, seated && !avatar);
            if (avatar) quadRider_->Draw(cameraView_, cameraProjection_, quad_);
        }

        // The bodies in the grass and the low growth besides the walker; the jungle and the
        // grass share them.
        const std::array<GrassWalker, GrassWalkers::Slots> grassWalkers = GatherWalkers(renderCameraPosition_.X, renderCameraPosition_.Z);
        {
            GpuDiagnosticScope gpuPass("jungle");
            jungleRenderer_->SetDew(weather_now::Dew(dayNight_.Hour(), weather_now::Current().cloudCover));
            jungleRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                  timeSeconds,renderedMonkeys,grassWalkers);
        }

        const PlayerPosition& grassPlayer = playerMotion_.Position();
        Vector3 grassInteractionPosition(grassPlayer.x,grassPlayer.y,grassPlayer.z);
        float grassInteractionStrength=grassInteractionModel_.Strength();
        if(viewpoint_==FixedViewpoint::SnakeHeadDetail) {
            const auto head=snakes_->Snakes().front().body.front();
            grassInteractionPosition=Vector3(head.x,head.y,head.z);
            grassInteractionStrength=1.0f;
        }
        {
            GpuDiagnosticScope gpuPass("grass");
            // The blades are wet after rain, and with the dew of clear nights until mid-morning.
            grassRenderer_->SetWetness(std::max(weather_now::Current().wetness,
                                                weather_now::Dew(dayNight_.Hour(), weather_now::Current().cloudCover)));
            grassRenderer_->Draw(
                cameraView_, cameraProjection_, renderCameraPosition_,
                grassInteractionPosition,grassInteractionStrength,timeSeconds,grassWalkers);
        }

        {
            GpuDiagnosticScope gpuPass("palms");
            palmRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                timeSeconds);
        }

        {
            GpuDiagnosticScope gpuPass("butterflies");
            // The day fliers (butterflies, dragonflies, flies) go to roost in the leaves at dusk,
            // one by one as the light goes, and come out again in the morning; they take
            // shelter from the rain the same way, most of them already in a shower's.
            const float dusk = std::max(std::clamp((-sun_light::Current().lighting.sunElevationDegrees + 1.0f) / 6.0f, 0.0f, 1.0f),
                                        std::clamp((weather_now::Current().rain - 0.15f) / 0.45f, 0.0f, 1.0f));
            const auto awake = [dusk](const auto& all, auto& kept) {
                kept.clear();
                for (std::size_t i = 0; i < all.size(); ++i)
                    if (dusk <= 0.0f || static_cast<float>((i * 37U) % 100U) / 100.0f >= dusk) kept.push_back(all[i]);
                return std::span(kept.data(), kept.size());
            };
            const auto butterflies = awake(butterflyMeadow_.Butterflies(), awakeButterflies_);
            const auto dragonflies = awake(dragonflySwarm_.Dragonflies(), awakeDragonflies_);
            const auto flies = awake(flySwarms_.Flies(), awakeFlies_);
            // The day geckos and the skinks are basking lizards: they go to their crevices at dusk
            // as the fliers go to the leaves (the dark only, not the rain).
            const float sundown = std::clamp((-sun_light::Current().lighting.sunElevationDegrees + 1.0f) / 6.0f, 0.0f, 1.0f);
            allGeckos_.clear();
            for (std::size_t i = 0; i < geckoPopulation_.Geckos().size(); ++i)
                if (sundown <= 0.0f || static_cast<float>((i * 37U) % 100U) / 100.0f >= sundown) allGeckos_.push_back(geckoPopulation_.Geckos()[i]);
            awakeSkinks_.clear();
            for (std::size_t i = 0; i < skinkPopulation_.Skinks().size(); ++i)
                if (sundown <= 0.0f || static_cast<float>((i * 37U) % 100U) / 100.0f >= sundown) awakeSkinks_.push_back(skinkPopulation_.Skinks()[i]);
            const float night = 1.0f - std::clamp((sun_light::Current().lighting.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f);
            // The frogs keep the other hours: by day most hide in the wet bank and a few bask
            // by the water; at night, or in the rain, they are all out (the close-up's stays).
            const float frogsOut = std::max(night, 0.8f * weather_now::Current().wetness);
            awakeFrogs_.clear();
            for (std::size_t i = 0; i < frogPopulation_.Frogs().size(); ++i)
                if (i == 1U || static_cast<float>((i * 53U + 17U) % 100U) / 100.0f < 0.35f + 0.65f * frogsOut)
                    awakeFrogs_.push_back(frogPopulation_.Frogs()[i]);
            // The mice and rats are night foragers too: by day a few are about, at night all
            // (the two close-ups' stay).
            awakeRodents_.clear();
            for (std::size_t i = 0; i < rodentPopulation_.Rodents().size(); ++i)
                if (i == 0U || i == RodentPopulation::MouseCount + 6U
                    || static_cast<float>((i * 61U + 29U) % 100U) / 100.0f < 0.30f + 0.70f * night)
                    awakeRodents_.push_back(rodentPopulation_.Rodents()[i]);
            // At night two house geckos come out on the hut's wall by the lantern.
            if (hutPlacement_) {
                if (night > 0.45f)
                    for (const auto& gecko : hut_geckos::At(*hutPlacement_, timeSeconds, night)) allGeckos_.push_back(gecko);
            }
            butterflyRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                     butterflies,
                                     butterflyMeadow_.FlowerPatches(),
                                     allGeckos_, awakeSkinks_,
                                     dragonflies, awakeFrogs_,
                                     awakeRodents_, flies,
                                     antTrails_.Ants(), cicadaColony_.Cicadas(),
                                     beetlePopulation_.Beetles());
        }

        {
            GpuDiagnosticScope gpuPass("crabs");
            crabRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                crabColony_.Crabs(), crabColony_.Burrows(),
                                hermitCrabGroup_.Crabs(), coconutCrabGroup_.Crabs());
        }

        {
            GpuDiagnosticScope gpuPass("boars");
            boarRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, boarHerd_.Boars());
        }
        {
            GpuDiagnosticScope gpuPass("snakes");
            snakeRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,
                snakes_->Snakes(),snakes_->TreeSnakes());
        }
        {
            GpuDiagnosticScope gpuPass("goats");
            goatRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,goats_->Goats());
        }
        {
            GpuDiagnosticScope gpuPass("deer");
            deerRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,deer_->Deer());
        }
        {
            GpuDiagnosticScope gpuPass("spiders");
            spiderRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,AwakeSpiders());
        }
        {
            GpuDiagnosticScope gpuPass("resident");
            residentRenderer_->SetWetness(std::min(1.0f, 1.2f * weather_now::Current().wetness));
            residentRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,resident_->State());
        }
        {
            GpuDiagnosticScope gpuPass("bear");
            bearRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,bear_->State());
        }

        {
            GpuDiagnosticScope gpuPass("birds");
            birdRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                birdFlock_.Birds());
        }

        {
            GpuDiagnosticScope gpuPass("fruit-bats");
            fruitBatRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                    fruitBatColony_.Bats());
        }

        {
            GpuDiagnosticScope gpuPass("fish");
            // the anemones on the reef, then its small fish (the schools' draw last keeps the
            // fish renderer's counts)
            if (reefFish_ && anemoneRenderer_)
                anemoneRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, reefFish_->Anemones(), timeSeconds);
            if (reefFish_ && !reefFish_->Fish().empty())
                fishRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                    reefFish_->Fish(), timeSeconds);
            fishRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                fishSchools_.Fish(), timeSeconds);
        }

        {
            GpuDiagnosticScope gpuPass("stingrays");
            stingrayRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,
                                    stingrayGroup_.Stingrays());
        }
        {
            GpuDiagnosticScope gpuPass("reef-herons");
            reefHeronRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, reefHeronGroup_.Herons());
        }
        {GpuDiagnosticScope gpuPass("octopuses");octopusRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,octopusGroup_.Octopuses());}

        {
            GpuDiagnosticScope gpuPass("sea-turtles");
            seaTurtleRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                     seaTurtleGroup_.Turtles(), timeSeconds);
        }

        {
            GpuDiagnosticScope gpuPass("crocodiles");
            crocodileRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,
                                     crocodileGroup_.Crocodiles());
        }

        {
            GpuDiagnosticScope gpuPass("shark");
            sharkRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                 sharkPatrol_.State(), timeSeconds);
        }

        {
            GpuDiagnosticScope gpuPass("dolphins");
            if (whaleRenderer_) whaleRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, whales_.Surfacing());
            if (seagrassRenderer_) seagrassRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, timeSeconds,
                                                           weather_now::SeaState());
            if (sailboatRenderer_) {
                const auto& boat = sailboat_.State();
                const float swell = boat.active ? SampleOceanWaves(boat.x, boat.z, timeSeconds).height * weather_now::SeaState() : 0.0f;
                sailboatRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, boat, swell,
                                        weather_now::Current().wind, timeSeconds);
            }
            dolphinRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,
                dolphinPod_.Dolphins(),timeSeconds);
        }

        {
            GpuDiagnosticScope gpuPass("monkeys");
            monkeyRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                  renderedMonkeys,timeSeconds,monkeyQaSubjectIndex_);
        }
        {
            GpuDiagnosticScope gpuPass("spider-webs");
            // the webs bead with the morning dew and after rain
            spiderWebRenderer_->SetDew(std::max(2.0f * weather_now::Dew(dayNight_.Hour(), weather_now::Current().cloudCover),
                                                0.8f * weather_now::Current().wetness));
            spiderWebRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,timeSeconds);
        }

        const PlayerPosition& oceanPlayer = playerMotion_.Position();
        {
            GpuDiagnosticScope gpuPass("ocean");
            const BirdDiveSplash& diveSplash = birdFlock_.DiveSplash();
            // A crocodile sliding into the water takes the seabird's splash ring when its
            // entry is the more recent of the two.
            const CrocodileGroup::Entry& crocEntry = crocodileGroup_.LastEntry();
            // The sea's one foam ring goes to the most recent of the seabird's strike, the
            // crocodile's entry and a dolphin's re-entry from its leap.
            float ringX = diveSplash.x, ringZ = diveSplash.z;
            float ringFraction = diveSplash.valid ? diveSplash.ageSeconds / BirdFlock::SplashSeconds : -1.0f;
            if (crocEntry.valid && (ringFraction < 0.0f || crocEntry.ageSeconds / CrocodileGroup::EntrySeconds < ringFraction)) {
                ringX = crocEntry.x; ringZ = crocEntry.z;
                ringFraction = crocEntry.ageSeconds / CrocodileGroup::EntrySeconds;
            }
            const DolphinPod::Splash& dolphinSplash = dolphinPod_.LastSplash();
            if (dolphinSplash.valid && (ringFraction < 0.0f || dolphinSplash.ageSeconds / DolphinPod::SplashSeconds < ringFraction)) {
                ringX = dolphinSplash.x; ringZ = dolphinSplash.z;
                ringFraction = dolphinSplash.ageSeconds / DolphinPod::SplashSeconds;
            }
            if (soundscape_ && birdFlock_.DiveCount() != notedDiveCount_) {
                notedDiveCount_ = birdFlock_.DiveCount();
                soundscape_->NoteBirdSplash(diveSplash.x, 0.0f, diveSplash.z);
            }
            if (soundscape_ && dolphinPod_.SplashCount() != notedDolphinSplashes_) {
                notedDolphinSplashes_ = dolphinPod_.SplashCount();
                if (dolphinSplash.valid) soundscape_->NoteBirdSplash(dolphinSplash.x, 0.0f, dolphinSplash.z);
            }
            const FishSchools::FishBreach& fishBreach = fishSchools_.LastBreach();
            // The small splash on the surface: a fish breaking it, or a reef heron's bill going
            // in, whichever is the more recent (they share the fleck and the small ring).
            struct { float x, z, phase; } smallSplash{fishBreach.x, fishBreach.z,
                fishBreach.valid ? fishBreach.ageSeconds / FishSchools::BreachSeconds : -1.0f};
            {
                const ReefHeronGroup::Strike& strike = reefHeronGroup_.LastStrike();
                const float strikePhase = strike.ageSeconds / ReefHeronGroup::StrikeSeconds;
                if (strike.valid && (smallSplash.phase < 0.0f || strikePhase < smallSplash.phase))
                    smallSplash = {strike.x, strike.z, strikePhase};
            }
            oceanRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                 timeSeconds, oceanPlayer.x, oceanPlayer.z,
                                 waterRippleModel_.Intensity(), waterRippleModel_.Phase(),
                                 waterRippleModel_.SplashIntensity(),
                                 waterRippleModel_.SplashPhase(),
                                 ringX, ringZ, ringFraction,
                                 jettyPlacement_ ? jettyPlacement_->rootX : 0.0f,
                                 jettyPlacement_ ? jettyPlacement_->rootZ : 0.0f,
                                 jettyPlacement_ ? jettyPlacement_->dirX : 0.0f,
                                 jettyPlacement_ ? jettyPlacement_->dirZ : 0.0f,
                                 smallSplash.x, smallSplash.z, smallSplash.phase);
        }
        {
            GpuDiagnosticScope gpuPass("shoreline");
            shorelineRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                     timeSeconds);
        }
        {
            GpuDiagnosticScope gpuPass("stream-water");
            streamFishRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,streamFish_.Fish());
            streamWaterRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,timeSeconds,streamRings_.Rings());
            // The crowns' shed leaves share the stream's leaf renderer; the stream's own
            // leaves draw last so the renderer's counts stay the stream's.
            streamDriftRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,canopyLeafFall_.Leaves());
            streamDriftRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,streamDrift_.Leaves());
            cascadeMistRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,cascadeMist_.Puffs());
            wetlandMistRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,wetlandMist_.Puffs());
            rootingDustRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,rootingDust_.Puffs());
            rootingDustRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,mudFlecks_.Puffs());
            rootingDustRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,hoofDust_.Puffs());
            stingraySandRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,stingraySand_.Puffs());
            stingraySandRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,stingrayFlickSand_.Puffs());
            stingraySandRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,turtleGraze_.Puffs());
            blownSandRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,blownSand_.Puffs());
            finWakeRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,finWake_.Puffs());
            finWakeRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,crocodileWake_.Puffs());
            octopusInkRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,octopusInk_.Puffs());
            surfSprayRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,surfSpray_.Puffs());
            dolphinBlowRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,dolphinBlows_.Puffs());
            if (whaleBlowRenderer_) whaleBlowRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,whales_.Blow());
            if (sunbeamMotes_) {
                const auto& sun = sun_light::Current().lighting;
                sunbeamMotes_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,timeSeconds,
                                    sun.shadowStrength * sun.sunVisibility,Vector3(sun.toSun.x,sun.toSun.y,sun.toSun.z));
            }
            dolphinBlowRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,dragonflyDips_.Puffs());
            dolphinBlowRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,crabBubbles_.Puffs());
            turtleBreathRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,turtleBreaths_.Puffs());
            if (exhaustRenderer_) exhaustRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,exhaust_.Puffs());
            if (wheelDustRenderer_) wheelDustRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,wheelDust_.Puffs());
            if (wheelSprayRenderer_) wheelSprayRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,wheelSpray_.Puffs());
            // the morning mist last of the translucent things, over everything solid it lies among
            if (mistBankRenderer_ && morningMist_) mistBankRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,morningMist_->Puffs());
            if (waterspoutRenderer_) waterspoutRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,waterspout_.State(),timeSeconds);
            if (summitCloudRenderer_) summitCloudRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,summitCloud_.Puffs());
        }
        {
            // The hour: the scene graded (golden hours, a dark blue night), then the stars and
            // the moon; the campfire's flames after it keep their own light.
            GpuDiagnosticScope gpuPass("time-of-day");
            const auto& light = sun_light::Current().lighting;
            // The rain, before the grade so the night darkens it; not inside the hill (the cave,
            // the bunker) or under the sea.
            const auto& weather = weather_now::Current();
            const bool underground = renderCameraPosition_.Y < terrain_.Height(renderCameraPosition_.X, renderCameraPosition_.Z) - 0.5f;
            if (!underground && renderCameraPosition_.Y > 0.0f)
            {
                timeOfDay_->DrawRainCurtains(cameraView_, cameraProjection_, renderCameraPosition_, light,
                                             std::max(weather.rain, weather.cloudCover > 0.6f ? 0.35f * weather.cloudCover : 0.0f),
                                             timeSeconds);
                rain_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, weather.rain, weather.wind, timeSeconds);
                rainRings_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, weather.rain, timeSeconds);
                rainSplashes_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, weather.rain, timeSeconds);
                // the crowns drip as long as the leaves are wet, through the rain and after it
                canopyDrips_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                   std::max(weather.rain, 0.0f) * 0.5f + weather.wetness * 0.8f, timeSeconds);
                if (eaveDrips_) eaveDrips_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, weather.rain, timeSeconds);
            }
            // The local lights (the campfire, the quad's headlights) light the ground through the
            // grade: each writes how strongly each pixel is lit into the frame's alpha, and the
            // grade lifts it there.
            bool localLit = false;
            if (localLights_ && TimeOfDayRenderer::GradeActive(light) && !underground) {
                localLights_->ResetCounts();
                const auto ground = [this](float x, float z) {
                    BikeGround g;
                    const float extent = TerrainMeshLayout::Extent - 1.0f;
                    if (quadSurface_ && std::abs(x) < extent && std::abs(z) < extent) {
                        const auto sample = quadSurface_->Sample(x, z);
                        g.height = sample.height;
                        g.normal = {sample.normal.x, sample.normal.y, sample.normal.z};
                    } else {
                        g.height = terrain_.Height(x, z);
                    }
                    return g;
                };
                const Vector3 fire(CampfireSite::X, CampfireLayout::GroundY() + 0.45f, CampfireSite::Z);
                const float night = 1.0f - std::clamp((light.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f);
                const bool fireLit = night > 0.02f && (fire - renderCameraPosition_).LengthSquared() < 120.0f * 120.0f;
                const bool lanternLit = hutRenderer_ && night > 0.02f
                    && (hutRenderer_->LanternPosition() - renderCameraPosition_).LengthSquared() < 120.0f * 120.0f;
                const bool lampsLit = quadRenderer_ && quadEngineOn_ && !quad_.Overturned();
                const bool torchLit = torchOn_ && !riding_ && night > 0.02f && !underground;
                if (fireLit || lampsLit || lanternLit || torchLit) {
                    timeOfDay_->ResetAlpha();
                    localLit = true;
                }
                if (fireLit) {
                    // the same flicker as the firelight the ground's shaders add
                    const float pulse = 0.83f + 0.10f * std::sin(timeSeconds * 8.7f) + 0.07f * std::sin(timeSeconds * 17.1f + 1.3f);
                    localLights_->DrawPoint(cameraView_, cameraProjection_, renderCameraPosition_, fire, 8.0f, ground,
                                            CampfireLighting::Strength() * night * pulse, Vector3(1.0f, 0.62f, 0.34f));
                    // and the resident by it, warm on the side turned to the flames
                    if (residentRenderer_) {
                        const auto& sitter = resident_->State();
                        if (Vector3::DistanceSquared(Vector3(sitter.x, sitter.y, sitter.z), fire) < 8.0f * 8.0f)
                            localLights_->DrawMesh(cameraView_, cameraProjection_, renderCameraPosition_, fire, 8.0f,
                                                   residentRenderer_->LastMesh(), CampfireLighting::Strength() * night * pulse,
                                                   Vector3(1.0f, 0.62f, 0.34f));
                    }
                }
                if (lanternLit) {
                    const float flicker = 0.90f + 0.06f * std::sin(timeSeconds * 5.3f) + 0.04f * std::sin(timeSeconds * 11.7f + 0.8f);
                    localLights_->DrawPoint(cameraView_, cameraProjection_, renderCameraPosition_, hutRenderer_->LanternPosition(),
                                            6.0f, ground, 1.0f * night * flicker, Vector3(1.0f, 0.74f, 0.44f));
                    // and the boards beside it: the front wall, its gable and the veranda floor
                    for (const auto& plane : hutRenderer_->LanternPlanes())
                        localLights_->DrawPlane(cameraView_, cameraProjection_, renderCameraPosition_, hutRenderer_->LanternPosition(),
                                                4.0f, plane.triangles, plane.normal, 1.0f * night * flicker, Vector3(1.0f, 0.74f, 0.44f));
                }
                if (torchLit) {
                    // the walker's hand torch: a cool white beam from the hand along the view
                    localLights_->DrawSpot(cameraView_, cameraProjection_, renderCameraPosition_, TorchLamp(), TorchAxis(),
                                           ground, 0.85f * night, Vector3(0.96f, 0.98f, 1.0f));
                }
                if (lampsLit) {
                    const BikeVec l = quad_.ToWorld(QuadBikeRenderer::HeadlightLeft), r = quad_.ToWorld(QuadBikeRenderer::HeadlightRight);
                    const BikeVec f = quad_.Forward(), u = quad_.Up();
                    // the lamps aim a little down the bike's nose
                    localLights_->DrawSpot(cameraView_, cameraProjection_, renderCameraPosition_,
                        Vector3((l.x + r.x) * 0.5f, (l.y + r.y) * 0.5f, (l.z + r.z) * 0.5f),
                        Vector3(f.x - u.x * 0.10f, f.y - u.y * 0.10f, f.z - u.z * 0.10f), ground, 0.15f + 0.85f * QuadLampNightness(),
                        Vector3(1.0f, 0.97f, 0.90f));
                }
            }
            timeOfDay_->DrawGrade(light, localLit);
            timeOfDay_->DrawNightSky(cameraView_, cameraProjection_, renderCameraPosition_, light, timeSeconds);
            if (!underground)
                timeOfDay_->DrawRainbow(cameraView_, cameraProjection_, renderCameraPosition_, light,
                                        TimeOfDayRenderer::RainbowStrength(light, weather.rain));
            // Rain falling through the night's lamps glitters in their light: the quad's beams,
            // the hut's lantern, the fire.
            if (!underground && renderCameraPosition_.Y > 0.0f && weather.rain > 0.05f) {
                const float night = 1.0f - std::clamp((light.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f);
                std::array<RainLight, 4> lamps{};
                std::size_t count = 0;
                if (night > 0.05f) {
                    if (quadRenderer_ && quadEngineOn_ && !quad_.Overturned()) {
                        const BikeVec l = quad_.ToWorld(QuadBikeRenderer::HeadlightLeft), r = quad_.ToWorld(QuadBikeRenderer::HeadlightRight);
                        const BikeVec f = quad_.Forward(), u = quad_.Up();
                        RainLight& beam = lamps[count++];
                        beam.position = Vector3((l.x + r.x) * 0.5f, (l.y + r.y) * 0.5f, (l.z + r.z) * 0.5f);
                        beam.axis = Vector3::Normalize(Vector3(f.x - u.x * 0.10f, f.y - u.y * 0.10f, f.z - u.z * 0.10f));
                        beam.cosCone = 0.93f;
                        beam.reach = 26.0f;
                        beam.strength = 0.9f * QuadLampNightness();
                        beam.tint = Vector3(1.0f, 0.97f, 0.90f);
                    }
                    if (torchOn_ && !riding_) {
                        RainLight& torch = lamps[count++];
                        torch.position = TorchLamp();
                        torch.axis = TorchAxis();
                        torch.cosCone = 0.93f;
                        torch.reach = 18.0f;
                        torch.strength = 0.8f * night;
                        torch.tint = Vector3(0.96f, 0.98f, 1.0f);
                    }
                    if (hutRenderer_) {
                        RainLight& lantern = lamps[count++];
                        lantern.position = hutRenderer_->LanternPosition();
                        lantern.reach = 5.0f;
                        lantern.strength = 0.8f * night;
                        lantern.tint = Vector3(1.0f, 0.74f, 0.44f);
                    }
                    RainLight& fire = lamps[count++];
                    fire.position = Vector3(CampfireSite::X, CampfireLayout::GroundY() + 0.6f, CampfireSite::Z);
                    fire.reach = 7.0f;
                    fire.strength = 0.9f * night * CampfireLighting::Strength();
                    fire.tint = Vector3(1.0f, 0.62f, 0.34f);
                }
                rain_->DrawLit(cameraView_, cameraProjection_, renderCameraPosition_, weather.rain, weather.wind, timeSeconds,
                               std::span<const RainLight>(lamps.data(), count));
            }
            timeOfDay_->DrawFlash(weather.flash);
            if (!underground)
                timeOfDay_->DrawBolt(cameraView_, cameraProjection_, renderCameraPosition_, weather.strikes,
                                     weather.lastStrikeDistance, weather.flash);
            // the surf glitters blue on dark nights, less under a bright moon
            if (!underground) {
                const float dark = 1.0f - std::clamp((light.sunElevationDegrees + 6.0f) / 8.0f, 0.0f, 1.0f);
                shorelineRenderer_->DrawGlow(cameraView_, cameraProjection_, renderCameraPosition_, timeSeconds,
                                             dark * (1.0f - 0.55f * light.moonLight));
                if (swimGlowRenderer_)
                    swimGlowRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, swimGlow_.Specks(), swimGlowStrength_);
            }
            if (fireflyRenderer_ && fireflies_ && fireflies_->Presence() > 0.0f && !underground)
                fireflyRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, fireflies_->Flies());
            if (hutRenderer_)
                hutRenderer_->DrawNightLights(cameraView_, cameraProjection_, renderCameraPosition_,
                    1.0f - std::clamp((light.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f), timeSeconds);
            if (quadRenderer_ && quadEngineOn_) {
                // braking: pulling back while it still rolls forward, or the handbrake on the move
                const float rolling = quad_.Speed();
                quadRenderer_->SetBraking(riding_ && ((quadControls_.throttle < -0.05f && rolling > 0.3f)
                                                      || (quadControls_.handbrake && std::abs(rolling) > 0.3f)));
                quadRenderer_->DrawLights(cameraView_, cameraProjection_, renderCameraPosition_, quad_, QuadLampNightness());
            }
            // the animals' eyes shine back in the quad's beam
            const bool torchShines = torchOn_ && !riding_ && !underground;
            if (eyeshineRenderer_ && (torchShines || (quadRenderer_ && quadEngineOn_ && !quad_.Overturned()))) {
                allEyes_.clear();
                if (deerRenderer_) allEyes_.insert(allEyes_.end(), deerRenderer_->Eyes().begin(), deerRenderer_->Eyes().end());
                if (goatRenderer_) allEyes_.insert(allEyes_.end(), goatRenderer_->Eyes().begin(), goatRenderer_->Eyes().end());
                if (boarRenderer_) allEyes_.insert(allEyes_.end(), boarRenderer_->Eyes().begin(), boarRenderer_->Eyes().end());
                if (crocodileRenderer_) allEyes_.insert(allEyes_.end(), crocodileRenderer_->Eyes().begin(), crocodileRenderer_->Eyes().end());
                if (torchShines) {
                    // the walker's torch, right by the eye: the strongest eyeshine of all
                    eyeshineRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, allEyes_,
                        TorchLamp(), TorchAxis(), 0.90f, 35.0f, QuadLampNightness());
                } else {
                    const BikeVec l = quad_.ToWorld(QuadBikeRenderer::HeadlightLeft), r = quad_.ToWorld(QuadBikeRenderer::HeadlightRight);
                    const BikeVec f = quad_.Forward(), u = quad_.Up();
                    eyeshineRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_, allEyes_,
                        Vector3((l.x + r.x) * 0.5f, (l.y + r.y) * 0.5f, (l.z + r.z) * 0.5f),
                        Vector3(f.x - u.x * 0.10f, f.y - u.y * 0.10f, f.z - u.z * 0.10f), 0.86f, 40.0f, QuadLampNightness());
                }
            }
            if (sailboatRenderer_)
                sailboatRenderer_->DrawLights(cameraView_, cameraProjection_, renderCameraPosition_,
                    1.0f - std::clamp((light.sunElevationDegrees + 4.0f) / 12.0f, 0.0f, 1.0f));
        }
        {
            GpuDiagnosticScope gpuPass("campfire-effects");
            campfireRenderer_->Draw(cameraView_,cameraProjection_,renderCameraPosition_,timeSeconds,true);
        }

        const auto& viewport = device.getViewportProperty();
        {
            GpuDiagnosticScope gpuPass("water-view-overlay");
            waterViewOverlay_->Draw(waterViewModel_.Intensity(),
                                    playerMotion_.OceanCurrentStrength(terrain_),
                                    viewport.getWidthProperty(), viewport.getHeightProperty());
        }
        {
            // Whatever is below the sea surface sees the shafts; only a swimmer breathes bubbles.
            GpuDiagnosticScope gpuPass("underwater-effect");
            const Vector3& eye = renderCameraPosition_;
            const bool overSea = std::isfinite(eye.X) && std::isfinite(eye.Z)
                && terrain_.Height(eye.X, eye.Z) < 0.0f && terrain_.SignedShoreDistance(eye.X, eye.Z) < 0.0f;
            const float cameraDepth = overSea && eye.Y < 0.0f ? -eye.Y : 0.0f;
            underwaterEffect_->Draw(cameraDepth, waterViewModel_.Intensity(), bubbleBurst_, timeSeconds,
                                    cameraView_, viewport.getWidthProperty(), viewport.getHeightProperty());
            // The screen-space canopy sun shafts (2026-10-20) are no longer drawn: locked to the
            // screen rather than the world, and switching on and off with the light at the
            // camera, they swept the whole view into flickering stripes in flight (operator
            // report). A world-anchored replacement would need volumes under real crown gaps.
        }

        if (locomotionModeNoticeSeconds_ > 0.0f) {
            const float opacity = std::clamp(
                locomotionModeNoticeSeconds_ / 0.32f, 0.0f, 1.0f);
            GpuDiagnosticScope gpuPass("locomotion-overlay");
            debugOverlay_->DrawLocomotionModeNotice(
                locomotionPaceToggle_.IsRunningSelected(), opacity,
                viewport.getWidthProperty(), viewport.getHeightProperty(),flightMotion_.IsActive());
        }

        if (debugOverlayVisible_ && worldDebugVisible_) {
            const SoundscapeDebugState soundscapeDebug = soundscape_
                ? soundscape_->DebugState() : SoundscapeDebugState{};
            const OceanCurrentDrift oceanCurrent =
                playerMotion_.OceanCurrentVelocity(terrain_);
            GpuDiagnosticScope gpuPass("world-debug");
            worldDebugRenderer_->Draw(cameraView_, cameraProjection_, renderCameraPosition_,
                                      playerMotion_.Position(), playerMotion_.GroundHeight(),
                                      playerMotion_.Mode() == LocomotionMode::Swimming,
                                      oceanCurrent,
                                      playerMotion_.MovementTrace(),
                                      worldCollisionGrid_.Obstacles(), birdFlock_.Birds(),
                                      birdFlock_.Routes(), birdFlock_.RouteRegions(),
                                      fishSchools_.Fish(), fishSchools_.Schools(),
                                      sharkPatrol_.State(), monkeyTroop_.Monkeys(),
                                      soundscapeDebug);
        }

        if (touchOverlayVisible_) {
            const auto& viewport = device.getViewportProperty();
            GpuDiagnosticScope gpuPass("touch-overlay");
            touchOverlay_->Draw(touchControlState_, touchDisplayWidth_, touchDisplayHeight_,
                                viewport.getWidthProperty(), viewport.getHeightProperty(), true,
                                {TouchRideOffered(), riding_, TouchTorchOffered(), torchOn_});
        }

        if (debugOverlayVisible_) {
            const PlayerPosition& player = playerMotion_.Position();
            DebugOverlayState overlayState;
            overlayState.playerPosition = Vector3(player.x, player.y, player.z);
            overlayState.cameraPosition = renderCameraPosition_;
            overlayState.freeCameraActive = IsDebugFreeCameraActive();
            overlayState.flightActive = flightMotion_.IsActive();
            overlayState.renderer = device.GetGraphicsRendererName();
            overlayState.viewportWidth = viewport.getWidthProperty();
            overlayState.viewportHeight = viewport.getHeightProperty();
            overlayState.worldSeeds = AuthoritativeWorldSeeds();
            overlayState.worldSeedCopyFeedbackActive =
                worldSeedCopyFeedbackSeconds_ > 0.0f;
            overlayState.worldSeedCopySucceeded = worldSeedCopySucceeded_;
            overlayState.swimming = playerMotion_.Mode() == LocomotionMode::Swimming;
            overlayState.running = playerMotion_.IsRunning();
            overlayState.groundHeight = playerMotion_.GroundHeight();
            overlayState.groundSlopeDegrees = playerMotion_.GroundSlopeRadians()
                * 180.0f / std::numbers::pi_v<float>;
            overlayState.offshoreDistance = playerMotion_.OffshoreDistance(terrain_);
            overlayState.oceanCurrentStrength = playerMotion_.OceanCurrentStrength(terrain_);
            const OceanCurrentDrift oceanCurrent =
                playerMotion_.OceanCurrentVelocity(terrain_);
            overlayState.oceanCurrentX = oceanCurrent.x;
            overlayState.oceanCurrentZ = oceanCurrent.z;
            overlayState.waterRippleIntensity = waterRippleModel_.Intensity();
            overlayState.waterRipplePhase = waterRippleModel_.Phase();
            overlayState.waterSplashIntensity = waterRippleModel_.SplashIntensity();
            overlayState.waterSplashPhase = waterRippleModel_.SplashPhase();
            overlayState.waterViewIntensity = waterViewModel_.Intensity();
            overlayState.waterViewDraws = waterViewOverlay_->DrawCallCount();
            overlayState.visibleTerrainCells = terrainRenderer_->VisibleCellCount();
            overlayState.totalTerrainCells = terrainRenderer_->TotalCellCount();
            overlayState.terrainDraws = terrainRenderer_->DrawCallCount();
            overlayState.distantIslets = distantSceneryRenderer_->IsletCount();
            overlayState.distantSceneryDraws = distantSceneryRenderer_->DrawCallCount();
            overlayState.visibleContactShadows =
                contactShadowRenderer_->VisibleShadowCount();
            overlayState.visiblePalmShadows =
                contactShadowRenderer_->VisiblePalmShadowCount();
            overlayState.visibleCanopyShadows =
                contactShadowRenderer_->VisibleCanopyShadowCount();
            overlayState.visibleRockShadows =
                contactShadowRenderer_->VisibleRockShadowCount();
            overlayState.visibleWoodShadows =
                contactShadowRenderer_->VisibleWoodShadowCount();
            overlayState.visibleMonkeyShadows =
                contactShadowRenderer_->VisibleMonkeyShadowCount();
            overlayState.contactShadowDraws = contactShadowRenderer_->DrawCallCount();
            overlayState.visiblePalms = palmRenderer_->VisiblePalmCount();
            overlayState.totalPalms = palmRenderer_->TotalPalmCount();
            overlayState.nearPalms = palmRenderer_->VisibleNearPalmCount();
            overlayState.farPalms = palmRenderer_->VisibleFarPalmCount();
            overlayState.palmCells = palmRenderer_->VisibleCellCount();
            overlayState.palmDraws = palmRenderer_->DrawCallCount();
            overlayState.visibleCanopy = jungleRenderer_->VisibleCanopyCount();
            overlayState.totalCanopy = jungleRenderer_->TotalCanopyCount();
            overlayState.nearCanopy = jungleRenderer_->VisibleCanopyNearCount();
            overlayState.farCanopy = jungleRenderer_->VisibleCanopyFarCount();
            overlayState.visibleUnderstory = jungleRenderer_->VisibleUnderstoryCount();
            overlayState.totalUnderstory = jungleRenderer_->TotalUnderstoryCount();
            overlayState.visibleBushes = jungleRenderer_->VisibleBushCount();
            overlayState.totalBushes = jungleRenderer_->TotalBushCount();
            overlayState.visibleGroundPlants = jungleRenderer_->VisibleGroundPlantCount();
            overlayState.totalGroundPlants = jungleRenderer_->TotalGroundPlantCount();
            overlayState.jungleCells = jungleRenderer_->VisibleCellCount();
            overlayState.jungleDraws = jungleRenderer_->DrawCallCount();
            overlayState.visibleRocks = naturalPropRenderer_->VisibleRockCount();
            overlayState.totalRocks = naturalPropRenderer_->TotalRockCount();
            overlayState.visibleReefRocks = naturalPropRenderer_->VisibleReefRockCount();
            overlayState.totalReefRocks = naturalPropRenderer_->TotalReefRockCount();
            overlayState.visibleLogs = naturalPropRenderer_->VisibleLogCount();
            overlayState.totalLogs = naturalPropRenderer_->TotalLogCount();
            overlayState.visibleDriftwood = naturalPropRenderer_->VisibleDriftwoodCount();
            overlayState.totalDriftwood = naturalPropRenderer_->TotalDriftwoodCount();
            overlayState.visibleBeachDetails =
                naturalPropRenderer_->VisibleBeachDetailCount();
            overlayState.totalBeachDetails = naturalPropRenderer_->TotalBeachDetailCount();
            overlayState.visibleFallenFronds =
                naturalPropRenderer_->VisibleFallenFrondCount();
            overlayState.totalFallenFronds =
                naturalPropRenderer_->TotalFallenFrondCount();
            overlayState.propCells = naturalPropRenderer_->VisibleCellCount();
            overlayState.propDraws = naturalPropRenderer_->DrawCallCount();
            overlayState.visibleGrassPatches = grassRenderer_->VisiblePatchCount();
            overlayState.totalGrassPatches = grassRenderer_->TotalPatchCount();
            overlayState.nearGrassPatches = grassRenderer_->VisibleNearPatchCount();
            overlayState.middleGrassPatches = grassRenderer_->VisibleMiddlePatchCount();
            overlayState.farGrassPatches = grassRenderer_->VisibleFarPatchCount();
            overlayState.mixedGrassLodCells = grassRenderer_->MixedLodCellCount();
            overlayState.grassCells = grassRenderer_->VisibleCellCount();
            overlayState.grassDraws = grassRenderer_->DrawCallCount();
            overlayState.grassInteractionStrength = grassInteractionModel_.Strength();
            overlayState.visibleBirds = birdRenderer_->VisibleBirdCount();
            overlayState.totalBirds = static_cast<int>(birdFlock_.Birds().size());
            overlayState.nearBirds = birdRenderer_->VisibleNearBirdCount();
            overlayState.farBirds = birdRenderer_->VisibleFarBirdCount();
            overlayState.fullSimulationBirds = birdFlock_.FullSimulationCount();
            overlayState.reducedSimulationBirds = birdFlock_.ReducedSimulationCount();
            overlayState.dormantSimulationBirds = birdFlock_.DormantSimulationCount();
            overlayState.approachingBirds = birdFlock_.ApproachingCount();
            overlayState.perchedBirds = birdFlock_.PerchedCount();
            overlayState.departingBirds = birdFlock_.DepartingCount();
            overlayState.birdDraws = birdRenderer_->DrawCallCount();
            overlayState.visibleButterflies =
                butterflyRenderer_->VisibleButterflyCount();
            overlayState.totalButterflies = static_cast<int>(
                butterflyMeadow_.Butterflies().size());
            overlayState.nearButterflies =
                butterflyRenderer_->VisibleNearButterflyCount();
            overlayState.farButterflies =
                butterflyRenderer_->VisibleFarButterflyCount();
            overlayState.fullSimulationButterflies =
                butterflyMeadow_.FullSimulationCount();
            overlayState.reducedSimulationButterflies =
                butterflyMeadow_.ReducedSimulationCount();
            overlayState.dormantSimulationButterflies =
                butterflyMeadow_.DormantSimulationCount();
            for (const ButterflyState& butterfly : butterflyMeadow_.Butterflies()) {
                switch (butterfly.behavior) {
                case ButterflyBehavior::Rest: ++overlayState.restingButterflies; break;
                case ButterflyBehavior::NectarHover: ++overlayState.hoveringButterflies; break;
                case ButterflyBehavior::Flutter: ++overlayState.flutteringButterflies; break;
                }
            }
            overlayState.butterflyDraws = butterflyRenderer_->DrawCallCount();
            overlayState.visibleCrabs = crabRenderer_->VisibleCrabCount();
            overlayState.visibleBoars = boarRenderer_->VisibleCount();
            overlayState.visibleGoats = goatRenderer_->VisibleCount();
            overlayState.totalGoats = static_cast<int>(goats_->Goats().size());
            overlayState.visibleSpiders = spiderRenderer_->VisibleCount();
            overlayState.totalSpiders = static_cast<int>(spiders_->Spiders().size());
            overlayState.spiderDraws = spiderRenderer_->DrawCallCount();
            overlayState.spiderShadows = contactShadowRenderer_->VisibleSpiderShadowCount();
            overlayState.goatDraws = goatRenderer_->DrawCallCount();
            overlayState.goatShadows = contactShadowRenderer_->VisibleGoatShadowCount();
            overlayState.totalBoars = static_cast<int>(boarHerd_.Boars().size());
            overlayState.boarDraws = boarRenderer_->DrawCallCount();
            overlayState.boarShadows = contactShadowRenderer_->VisibleBoarShadowCount();
            overlayState.visibleSnakes = snakeRenderer_->VisibleCount();
            overlayState.totalSnakes = static_cast<int>(snakes_->Snakes().size()+snakes_->TreeSnakes().size());
            overlayState.snakeShadows = contactShadowRenderer_->VisibleSnakeShadowCount();
            overlayState.snakeDraws = snakeRenderer_->DrawCallCount();
            overlayState.visibleFruitBats = fruitBatRenderer_->VisibleCount();
            overlayState.totalFruitBats = static_cast<int>(fruitBatColony_.Bats().size());
            overlayState.visibleGeckos = butterflyRenderer_->VisibleGeckoCount();
            overlayState.totalGeckos = static_cast<int>(geckoPopulation_.Geckos().size());
            overlayState.visibleSkinks = butterflyRenderer_->VisibleSkinkCount();
            overlayState.totalSkinks = static_cast<int>(skinkPopulation_.Skinks().size());
            overlayState.visibleDragonflies = butterflyRenderer_->VisibleDragonflyCount();
            overlayState.totalDragonflies = static_cast<int>(dragonflySwarm_.Dragonflies().size());
            overlayState.visibleFrogs = butterflyRenderer_->VisibleFrogCount();
            overlayState.totalFrogs = static_cast<int>(frogPopulation_.Frogs().size());
            overlayState.visibleMice = butterflyRenderer_->VisibleMouseCount();
            overlayState.totalMice = static_cast<int>(RodentPopulation::MouseCount);
            overlayState.visibleRats = butterflyRenderer_->VisibleRatCount();
            overlayState.totalRats = static_cast<int>(RodentPopulation::RatCount);
            overlayState.visibleFlies = butterflyRenderer_->VisibleFlyCount();
            overlayState.totalFlies = static_cast<int>(FlySwarms::Count);
            overlayState.visibleAnts = butterflyRenderer_->VisibleAntCount();
            overlayState.totalAnts = static_cast<int>(AntTrails::Count);
            overlayState.visibleCicadas = butterflyRenderer_->VisibleCicadaCount();
            overlayState.totalCicadas = static_cast<int>(CicadaColony::Count);
            overlayState.visibleBeetles = butterflyRenderer_->VisibleBeetleCount();
            overlayState.totalBeetles = static_cast<int>(BeetlePopulation::Count);
            overlayState.visibleHermitCrabs = crabRenderer_->VisibleHermitCrabCount();
            overlayState.totalHermitCrabs = static_cast<int>(hermitCrabGroup_.Crabs().size());
            overlayState.visibleCoconutCrabs = crabRenderer_->VisibleCoconutCrabCount();
            overlayState.totalCoconutCrabs = static_cast<int>(coconutCrabGroup_.Crabs().size());
            overlayState.visibleDeer = deerRenderer_->VisibleCount();
            overlayState.totalDeer = static_cast<int>(deer_->Deer().size());
            overlayState.visibleResident = residentRenderer_->VisibleCount();
            overlayState.totalResident = 1;
            overlayState.visibleBear = bearRenderer_->VisibleCount();
            overlayState.totalBear = 1;
            overlayState.visibleCrocodiles = crocodileRenderer_->VisibleCount();
            overlayState.totalCrocodiles = static_cast<int>(crocodileGroup_.Crocodiles().size());
            overlayState.visibleStingrays = stingrayRenderer_->VisibleCount();
            overlayState.totalStingrays = static_cast<int>(stingrayGroup_.Stingrays().size());
            overlayState.visibleOctopuses = octopusRenderer_->VisibleCount();
            overlayState.totalOctopuses = static_cast<int>(octopusGroup_.Octopuses().size());
            overlayState.totalCrabs = static_cast<int>(crabColony_.Crabs().size());
            overlayState.nearCrabs = crabRenderer_->VisibleNearCrabCount();
            overlayState.farCrabs = crabRenderer_->VisibleFarCrabCount();
            overlayState.fullSimulationCrabs = crabColony_.FullSimulationCount();
            overlayState.reducedSimulationCrabs = crabColony_.ReducedSimulationCount();
            overlayState.dormantSimulationCrabs = crabColony_.DormantSimulationCount();
            overlayState.crabDraws = crabRenderer_->DrawCallCount();
            overlayState.visibleFish = fishRenderer_->VisibleFishCount();
            overlayState.totalFish = static_cast<int>(fishSchools_.Fish().size());
            overlayState.nearFish = fishRenderer_->VisibleNearFishCount();
            overlayState.farFish = fishRenderer_->VisibleFarFishCount();
            overlayState.fullSimulationFish = fishSchools_.FullSimulationCount();
            overlayState.reducedSimulationFish = fishSchools_.ReducedSimulationCount();
            overlayState.dormantSimulationFish = fishSchools_.DormantSimulationCount();
            overlayState.fishDraws = fishRenderer_->DrawCallCount();
            overlayState.visibleSeaTurtles = seaTurtleRenderer_->VisibleTurtleCount();
            overlayState.totalSeaTurtles = static_cast<int>(seaTurtleGroup_.Turtles().size());
            overlayState.nearSeaTurtles = seaTurtleRenderer_->VisibleNearTurtleCount();
            overlayState.farSeaTurtles = seaTurtleRenderer_->VisibleFarTurtleCount();
            overlayState.fullSimulationSeaTurtles = seaTurtleGroup_.FullSimulationCount();
            overlayState.reducedSimulationSeaTurtles = seaTurtleGroup_.ReducedSimulationCount();
            overlayState.dormantSimulationSeaTurtles = seaTurtleGroup_.DormantSimulationCount();
            for (const SeaTurtleState& turtle : seaTurtleGroup_.Turtles()) {
                switch (turtle.behavior) {
                case SeaTurtleBehavior::Cruise: ++overlayState.cruisingSeaTurtles; break;
                case SeaTurtleBehavior::Forage: ++overlayState.foragingSeaTurtles; break;
                case SeaTurtleBehavior::Surface: ++overlayState.surfacingSeaTurtles; break;
                }
            }
            overlayState.seaTurtleDraws = seaTurtleRenderer_->DrawCallCount();
            overlayState.sharkVisible = sharkRenderer_->IsVisible();
            overlayState.sharkPresent = sharkPatrol_.State().present;
            overlayState.sharkNear = sharkRenderer_->UsesNearLod();
            overlayState.sharkActivity = sharkPatrol_.State().activity;
            overlayState.sharkSimulationTier = static_cast<int>(
                sharkPatrol_.State().simulationTier);
            overlayState.sharkDraws = sharkRenderer_->DrawCallCount();
            overlayState.sharkPresenceCycles = static_cast<int>(
                sharkPatrol_.PresenceCycleCount());
            overlayState.visibleDolphins = dolphinRenderer_->VisibleCount();
            overlayState.totalDolphins = static_cast<int>(dolphinPod_.Dolphins().size());
            overlayState.nearDolphins = dolphinRenderer_->VisibleNearCount();
            overlayState.farDolphins = dolphinRenderer_->VisibleFarCount();
            overlayState.fullSimulationDolphins = dolphinPod_.FullSimulationCount();
            overlayState.reducedSimulationDolphins = dolphinPod_.ReducedSimulationCount();
            overlayState.dormantSimulationDolphins = dolphinPod_.DormantSimulationCount();
            for(const DolphinState& dolphin:dolphinPod_.Dolphins()) {
                switch(dolphin.behavior) {
                case DolphinBehavior::Cruise:++overlayState.cruisingDolphins;break;
                case DolphinBehavior::Surface:++overlayState.surfacingDolphins;break;
                case DolphinBehavior::Dive:++overlayState.divingDolphins;break;
                case DolphinBehavior::Leap:++overlayState.surfacingDolphins;break;
                case DolphinBehavior::SpyHop:++overlayState.surfacingDolphins;break;
                }
            }
            overlayState.dolphinDraws = dolphinRenderer_->DrawCallCount();
            overlayState.visibleMonkeys = monkeyRenderer_->VisibleMonkeyCount();
            overlayState.totalMonkeys = static_cast<int>(monkeyTroop_.Monkeys().size());
            overlayState.nearMonkeys = monkeyRenderer_->VisibleNearMonkeyCount();
            overlayState.farMonkeys = monkeyRenderer_->VisibleFarMonkeyCount();
            overlayState.fullSimulationMonkeys = monkeyTroop_.FullSimulationCount();
            overlayState.reducedSimulationMonkeys = monkeyTroop_.ReducedSimulationCount();
            overlayState.dormantSimulationMonkeys = monkeyTroop_.DormantSimulationCount();
            for (const MonkeyState& monkey : monkeyTroop_.Monkeys()) {
                switch (monkey.behavior) {
                case MonkeyBehavior::Idle:
                    ++overlayState.idleMonkeys;
                    break;
                case MonkeyBehavior::Observe:
                case MonkeyBehavior::InspectTree:
                    ++overlayState.observeMonkeys;
                    break;
                case MonkeyBehavior::Wander:
                case MonkeyBehavior::Walk:
                case MonkeyBehavior::ApproachTree:
                    ++overlayState.walkingMonkeys;
                    break;
                case MonkeyBehavior::RunAway:
                    ++overlayState.runningMonkeys;
                    break;
                }
            }
            overlayState.monkeyDraws = monkeyRenderer_->DrawCallCount();
            overlayState.monkeyObstacleDeflections = monkeyTroop_.ObstacleDeflectionCount();
            overlayState.activeMonkeyCrossfades = monkeyRenderer_->ActiveCrossfadeCount();
            overlayState.completedMonkeyCrossfades = monkeyRenderer_->CompletedCrossfadeCount();
            overlayState.audioAvailable = soundscape_ && soundscape_->IsAvailable();
            overlayState.audioMuted = soundscape_ && soundscape_->IsMuted();
            if (soundscape_) {
                overlayState.surfVolume = soundscape_->Mix().surfVolume;
                overlayState.breezeVolume = soundscape_->Mix().breezeVolume;
                overlayState.jungleVolume = soundscape_->Mix().jungleVolume;
                overlayState.waterMovementVolume = soundscape_->WaterMix().volume;
                overlayState.oceanCurrentAudioCue = soundscape_->WaterMix().currentCue;
                overlayState.playedWaveAccents = soundscape_->PlayedWaveAccentCount();
                overlayState.playedShipwreckCreaks =
                    soundscape_->PlayedShipwreckCreakCount();
                overlayState.playedWaterTransitions =
                    soundscape_->PlayedWaterTransitionCount();
                overlayState.playedFootsteps = soundscape_->PlayedFootstepCount();
                overlayState.playedBirdCalls = soundscape_->PlayedBirdCallCount();
                overlayState.playedMonkeyCalls = soundscape_->PlayedMonkeyCallCount();
                overlayState.playedBoarCalls = soundscape_->PlayedBoarCallCount();
                overlayState.playedGoatCalls = soundscape_->PlayedGoatCallCount();
                overlayState.playedSnakeHisses = soundscape_->PlayedSnakeHissCount();
                overlayState.playedBearCalls = soundscape_->PlayedBearCallCount();
                overlayState.playedFrogCalls = soundscape_->PlayedFrogCallCount();
                overlayState.playedCicadaCalls = soundscape_->PlayedCicadaCallCount();
                overlayState.playedFlyBuzzes = soundscape_->PlayedFlyBuzzCount();
                overlayState.wildlifeCallActive = soundscape_->IsWildlifeCallActive();
                const SoundscapeDebugState soundscapeDebug = soundscape_->DebugState();
                overlayState.playingAudioSources = soundscapeDebug.playingSourceCount;
                overlayState.audioSourceBudget = soundscapeDebug.sourceBudget;
            }
            overlayState.worldDebugVisible = worldDebugVisible_;
            overlayState.collisionCandidates = static_cast<int>(
                worldCollisionGrid_.CandidateCount(player.x, player.z,
                                                   PlayerMotion::CapsuleRadius));
            overlayState.collisionObstacles = static_cast<int>(
                worldCollisionGrid_.Obstacles().size());
            if (worldDebugVisible_) {
                const PlayerMovementTrace& movementTrace = playerMotion_.MovementTrace();
                overlayState.worldDebugObstacles = worldDebugRenderer_->VisibleObstacleCount();
                overlayState.worldDebugCells = worldDebugRenderer_->VisibleCellCount();
                overlayState.worldDebugWildlife = worldDebugRenderer_->WildlifeMarkerCount();
                overlayState.worldDebugWildlifeTargets =
                    worldDebugRenderer_->WildlifeTargetCount();
                overlayState.worldDebugMonkeyHomes =
                    worldDebugRenderer_->MonkeyHomeRangeCount();
                overlayState.worldDebugBirdRouteRegions =
                    worldDebugRenderer_->BirdRouteRegionCount();
                overlayState.worldDebugBirdSpawns =
                    worldDebugRenderer_->BirdSpawnMarkerCount();
                overlayState.worldDebugBirdRouteTargets =
                    worldDebugRenderer_->BirdRouteTargetCount();
                overlayState.worldDebugAudioZones = worldDebugRenderer_->AudioZoneCount();
                overlayState.worldDebugAudioCandidates =
                    worldDebugRenderer_->AudioCandidateCount();
                overlayState.worldDebugActiveAudioEmitters =
                    worldDebugRenderer_->ActiveAudioEmitterCount();
                overlayState.worldDebugGroundSamples =
                    worldDebugRenderer_->GroundContactSampleCount();
                overlayState.worldDebugGroundSupports =
                    worldDebugRenderer_->GroundSupportSampleCount();
                overlayState.worldDebugSteepSamples =
                    worldDebugRenderer_->SteepGroundSampleCount();
                overlayState.movementObstacleRejections = movementTrace.obstacleRejections;
                overlayState.movementTerrainRejections = movementTrace.terrainRejections;
                overlayState.movementSlideSubsteps = movementTrace.slideSubsteps;
                overlayState.movementObstacleDepenetrations =
                    movementTrace.obstacleDepenetrations;
                overlayState.movementSafetyClamped = movementTrace.safetyClamped;
                overlayState.worldDebugLines = worldDebugRenderer_->LineCount();
                overlayState.worldDebugDraws = worldDebugRenderer_->DrawCallCount();
            }
            GpuDiagnosticScope gpuPass("debug-overlay");
            debugOverlay_->Draw(overlayState);
        }

        if (drawnFrames_ == 0U) {
            RecordStartupTiming(drawStarted);
            const PlayerPosition& player = playerMotion_.Position();
            const float currentStrength = playerMotion_.OceanCurrentStrength(terrain_);
            const float undertowTarget = WaterMovementModel::TargetMix(
                std::max(0.0f, -playerMotion_.GroundHeight()),
                playerMotion_.Mode() == LocomotionMode::Swimming,
                std::hypot(playerMotion_.VelocityX(), playerMotion_.VelocityZ()),
                currentStrength).currentCue;
            std::ostringstream viewportStatus;
            viewportStatus << "island-simulator: viewport "
                           << viewport.getWidthProperty() << 'x'
                           << viewport.getHeightProperty() << " logical backbuffer";
            const std::string viewportDiagnostic = viewportStatus.str();
            std::cout << viewportDiagnostic << std::endl;
            MirrorRuntimeDiagnosticToPlatformLog(viewportDiagnostic);
            std::cout << "island-simulator: visible terrain cells "
                      << terrainRenderer_->VisibleCellCount() << '/'
                      << terrainRenderer_->TotalCellCount() << " in "
                      << terrainRenderer_->DrawCallCount()
                      << " shared-buffer draws, collision candidates "
                      << worldCollisionGrid_.CandidateCount(
                             player.x, player.z, PlayerMotion::CapsuleRadius)
                      << '/' << worldCollisionGrid_.Obstacles().size() << ", palms "
                      << palmRenderer_->VisiblePalmCount() << '/'
                      << palmRenderer_->TotalPalmCount() << " (near "
                      << palmRenderer_->VisibleNearPalmCount() << ", far "
                      << palmRenderer_->VisibleFarPalmCount() << ") in "
                      << palmRenderer_->VisibleCellCount() << " cells, "
                      << palmRenderer_->DrawCallCount() << " draws; jungle "
                      << jungleRenderer_->VisibleCanopyCount() << '/'
                      << jungleRenderer_->TotalCanopyCount() << " canopy (near "
                      << jungleRenderer_->VisibleCanopyNearCount() << ", far "
                      << jungleRenderer_->VisibleCanopyFarCount() << "), "
                      << jungleRenderer_->VisibleStandingSnagCount() << '/'
                      << jungleRenderer_->TotalStandingSnagCount() << " standing snags, "
                      << jungleRenderer_->VisibleUnderstoryCount() << '/'
                      << jungleRenderer_->TotalUnderstoryCount() << " understory (near "
                      << jungleRenderer_->VisibleUnderstoryNearCount() << ", far "
                      << jungleRenderer_->VisibleUnderstoryFarCount() << "), "
                      << jungleRenderer_->VisibleBushCount() << '/'
                      << jungleRenderer_->TotalBushCount() << " bushes (near "
                      << jungleRenderer_->VisibleBushNearCount() << ", far "
                      << jungleRenderer_->VisibleBushFarCount() << "), "
                      << jungleRenderer_->VisibleGroundPlantCount() << '/'
                      << jungleRenderer_->TotalGroundPlantCount() << " ground plants (near "
                      << jungleRenderer_->VisibleGroundPlantNearCount() << ", far "
                      << jungleRenderer_->VisibleGroundPlantFarCount() << "), "
                      << jungleRenderer_->DrawCallCount() << " draws; canopy frustum rejects "
                      << jungleRenderer_->CanopyFrustumRejectedCount() << "; props "
                      << naturalPropRenderer_->VisibleRockCount() << '/'
                      << naturalPropRenderer_->TotalRockCount() << " rocks, "
                      << naturalPropRenderer_->VisibleReefRockCount() << '/'
                      << naturalPropRenderer_->TotalReefRockCount() << " reef outcrops, "
                      << naturalPropRenderer_->VisibleReefHabitatCount() << '/'
                      << naturalPropRenderer_->TotalReefHabitatCount() << " reef habitat, "
                      << naturalPropRenderer_->VisibleLogCount() << '/'
                      << naturalPropRenderer_->TotalLogCount() << " logs, "
                      << naturalPropRenderer_->VisibleDriftwoodCount() << '/'
                      << naturalPropRenderer_->TotalDriftwoodCount() << " driftwood (near "
                      << naturalPropRenderer_->VisibleDriftwoodNearCount() << ", far "
                      << naturalPropRenderer_->VisibleDriftwoodFarCount() << "), "
                      << naturalPropRenderer_->VisibleBeachDetailCount() << '/'
                      << naturalPropRenderer_->TotalBeachDetailCount()
                      << " shell/pebble/wrack clusters, "
                      << naturalPropRenderer_->VisibleFallenFrondCount() << '/'
                      << naturalPropRenderer_->TotalFallenFrondCount()
                      << " fallen fronds, "
                      << naturalPropRenderer_->DrawCallCount() << " draws; shipwreck "
                      << (shipwreckRenderer_->IsVisible() ? "visible" : "culled") << ", "
                      << shipwreckRenderer_->DebrisCount() << " debris, "
                      << shipwreckRenderer_->DrawCallCount() << " draw; hut "
                      << (hutRenderer_?hutRenderer_->DrawCallCount():0) << " draw, "
                      << contactShadowRenderer_->VisibleHutShadowCount() << " contact patches; helicopter "
                      << helicopterRenderer_->DrawCallCount() << " draw; grass "
                      << grassRenderer_->VisiblePatchCount() << '/'
                      << grassRenderer_->TotalPatchCount() << " patches in "
                      << grassRenderer_->VisibleCellCount() << " cells, "
                      << grassRenderer_->DrawCallCount() << " draws, interaction "
                      << grassInteractionModel_.Strength() << ", custom shader "
                      << (grassRenderer_->UsesCustomShader() ? 1 : 0) << ", biome tint "
                      << (grassRenderer_->UsesCustomShader() ? 1 : 0) << ", LOD near "
                      << grassRenderer_->VisibleNearPatchCount() << ", middle "
                      << grassRenderer_->VisibleMiddlePatchCount() << ", far "
                      << grassRenderer_->VisibleFarPatchCount() << ", mixed cells "
                      << grassRenderer_->MixedLodCellCount() << "; birds "
                      << birdRenderer_->VisibleBirdCount() << '/'
                      << birdFlock_.Birds().size() << " visible (near "
                      << birdRenderer_->VisibleNearBirdCount() << ", far "
                      << birdRenderer_->VisibleFarBirdCount() << "), "
                      << birdRenderer_->DrawCallCount() << " draw; fruit bats "
                      << fruitBatRenderer_->VisibleCount() << '/'
                      << fruitBatColony_.Bats().size() << " visible (near "
                      << fruitBatRenderer_->NearCount() << ", far "
                      << fruitBatRenderer_->FarCount() << "), "
                      << fruitBatRenderer_->DrawCallCount() << " draw; butterflies "
                      << butterflyRenderer_->VisibleButterflyCount() << '/'
                      << butterflyMeadow_.Butterflies().size() << " visible (near "
                      << butterflyRenderer_->VisibleNearButterflyCount() << ", far "
                      << butterflyRenderer_->VisibleFarButterflyCount() << "), "
                      << butterflyRenderer_->DrawCallCount() << " draw; geckos "
                      << butterflyRenderer_->VisibleGeckoCount() << '/'
                      << geckoPopulation_.Geckos().size()
                      << " visible in shared draw; skinks "
                      << butterflyRenderer_->VisibleSkinkCount() << '/'
                      << skinkPopulation_.Skinks().size()
                      << " visible in shared draw; dragonflies "
                      << butterflyRenderer_->VisibleDragonflyCount() << '/'
                      << dragonflySwarm_.Dragonflies().size()
                      << " visible in shared draw; frogs "
                      << butterflyRenderer_->VisibleFrogCount() << '/'
                      << frogPopulation_.Frogs().size()
                      << " visible in shared draw; mice "
                      << butterflyRenderer_->VisibleMouseCount() << '/'
                      << RodentPopulation::MouseCount
                      << " visible in shared draw; rats "
                      << butterflyRenderer_->VisibleRatCount() << '/'
                      << RodentPopulation::RatCount
                      << " visible in shared draw; flies "
                      << butterflyRenderer_->VisibleFlyCount() << '/'
                      << FlySwarms::Count
                      << " visible in shared draw; ants "
                      << butterflyRenderer_->VisibleAntCount() << '/'
                      << AntTrails::Count
                      << " visible in shared draw; cicadas "
                      << butterflyRenderer_->VisibleCicadaCount() << '/'
                      << CicadaColony::Count
                      << " visible in shared draw; beetles "
                      << butterflyRenderer_->VisibleBeetleCount() << '/'
                      << BeetlePopulation::Count
                      << " visible in shared draw; crabs "
                      << crabRenderer_->VisibleCrabCount() << '/'
                      << crabColony_.Crabs().size() << " visible (near "
                      << crabRenderer_->VisibleNearCrabCount() << ", far "
                      << crabRenderer_->VisibleFarCrabCount() << "), "
                      << crabRenderer_->DrawCallCount() << " draw; hermit crabs "
                      << crabRenderer_->VisibleHermitCrabCount() << '/'
                      << hermitCrabGroup_.Crabs().size()
                      << " visible in shared draw; coconut crabs "
                      << crabRenderer_->VisibleCoconutCrabCount() << '/'
                      << coconutCrabGroup_.Crabs().size()
                      << " visible in shared draw; boars "
                      << boarRenderer_->VisibleCount() << '/' << boarHerd_.Boars().size()
                      << " visible, " << boarRenderer_->DrawCallCount() << " draw; goats "
                      << goatRenderer_->VisibleCount() << '/' << goats_->Goats().size()
                      << " visible, " << goatRenderer_->DrawCallCount() << " draw; deer "
                      << deerRenderer_->VisibleCount() << '/' << deer_->Deer().size()
                      << " visible, " << deerRenderer_->DrawCallCount() << " draw; resident "
                      << residentRenderer_->VisibleCount() << "/1 visible, "
                      << residentRenderer_->DrawCallCount() << " draws, "
                      << residentRenderer_->TriangleCount() << " body triangles; bear "
                      << bearRenderer_->VisibleCount() << "/1 visible, "
                      << bearRenderer_->DrawCallCount() << " draws, "
                      << bearRenderer_->TriangleCount() << " body triangles; snakes "
                      << snakeRenderer_->VisibleCount() << '/'
                      << snakes_->Snakes().size()+snakes_->TreeSnakes().size()
                      << " visible, " << snakeRenderer_->DrawCallCount() << " draw, "
                      << contactShadowRenderer_->VisibleSnakeShadowCount() << " shadows; spiders "
                      << spiderRenderer_->VisibleCount() << '/' << spiders_->Spiders().size()
                      << " visible, " << spiderRenderer_->DrawCallCount() << " draws; webs "
                      << spiderWebRenderer_->VisibleCount() << '/' << spiderWebRenderer_->SiteCount()
                      << " visible, " << spiderWebRenderer_->StrandCount() << " strands, "
                      << spiderWebRenderer_->DrawCallCount() << " draw; fish "
                      << fishRenderer_->VisibleFishCount() << '/'
                      << fishSchools_.Fish().size() << " visible (near "
                      << fishRenderer_->VisibleNearFishCount() << ", far "
                      << fishRenderer_->VisibleFarFishCount() << "), "
                      << fishRenderer_->DrawCallCount() << " draw; stingrays "
                      << stingrayRenderer_->VisibleCount() << '/'
                      << stingrayGroup_.Stingrays().size() << " visible (near "
                      << stingrayRenderer_->NearCount() << ", far "
                      << stingrayRenderer_->FarCount() << "), "
                      << stingrayRenderer_->DrawCallCount() << " draw; octopuses "
                      << octopusRenderer_->VisibleCount() << '/'
                      << octopusGroup_.Octopuses().size() << " visible (near "
                      << octopusRenderer_->NearCount() << ", far " << octopusRenderer_->FarCount()
                      << "), " << octopusRenderer_->DrawCallCount() << " draw; turtles "
                      << seaTurtleRenderer_->VisibleTurtleCount() << '/'
                      << seaTurtleGroup_.Turtles().size() << " visible (near "
                      << seaTurtleRenderer_->VisibleNearTurtleCount() << ", far "
                      << seaTurtleRenderer_->VisibleFarTurtleCount() << "), "
                      << seaTurtleRenderer_->DrawCallCount() << " draw; crocodiles "
                      << crocodileRenderer_->VisibleCount() << '/'
                      << crocodileGroup_.Crocodiles().size() << " visible (near "
                      << crocodileRenderer_->VisibleNearCount() << ", far "
                      << crocodileRenderer_->VisibleFarCount() << "), "
                      << crocodileRenderer_->DrawCallCount() << " draw; shark "
                      << (sharkRenderer_->IsVisible() ? "visible" : "culled")
                      << " activity " << sharkPatrol_.State().activity << ", "
                      << sharkRenderer_->DrawCallCount() << " draw; dolphins "
                      << dolphinRenderer_->VisibleCount() << '/' << dolphinPod_.Dolphins().size()
                      << " visible (near " << dolphinRenderer_->VisibleNearCount() << ", far "
                      << dolphinRenderer_->VisibleFarCount() << "), "
                      << dolphinRenderer_->DrawCallCount() << " draw; monkeys "
                      << monkeyRenderer_->VisibleMonkeyCount() << '/'
                      << monkeyTroop_.Monkeys().size() << " visible, "
                      << monkeyRenderer_->DrawCallCount() << " draws, blends "
                      << monkeyRenderer_->ActiveCrossfadeCount() << '/'
                      << monkeyRenderer_->CompletedCrossfadeCount()
                      << " active/completed, canopy transmission "
                      << monkeyRenderer_->MinimumCanopyTransmission() << ".."
                      << monkeyRenderer_->MaximumCanopyTransmission() << "; distant "
                      << distantSceneryRenderer_->IsletCount() << " islets, "
                      << distantSceneryRenderer_->DrawCallCount()
                      << " draw; contact shadows "
                      << contactShadowRenderer_->VisibleShadowCount() << " (walker's own shadow "
                      << contactShadowRenderer_->VisiblePlayerShadowCount() << ", sun figure "
                      << walkerShadowRenderer_->TriangleCount() << " triangles), "
                      << contactShadowRenderer_->PrimitiveCount() << " triangles, "
                      << contactShadowRenderer_->DrawCallCount()
                      << " draw, contact-batch mass shadows "
                      << contactShadowRenderer_->VisibleStructureShadowCount()
                      << (sunShadowMap_ ? " (only where the sun shadow map's shadows fade out)" : "")
                      << "; sun shadow map " << (sunShadowMap_ ? sunShadowMap_->RenderCount() : 0)
                      << " renders, " << (sunShadowMap_ ? sunShadowMap_->LastCasterCount() : 0) << " casters"
                      << "; coastal contact shadows " << contactShadowRenderer_->VisibleCoastalShadowCount()
                      << "; water view " << waterViewModel_.Intensity()
                      << ", " << waterViewOverlay_->DrawCallCount()
                      << " draw, player ripple " << waterRippleModel_.Intensity()
                      << ", phase " << waterRippleModel_.Phase()
                      << ", splash " << waterRippleModel_.SplashIntensity()
                      << ", phase " << waterRippleModel_.SplashPhase() << ", current "
                      << currentStrength << ", undertow target " << undertowTarget << '\n';
            if (viewpoint_ == FixedViewpoint::GroundPlantDetail) {
                std::cout << "island-simulator: ground QA per-instance near LOD "
                          << jungleRenderer_->VisibleGroundPlantNearCount() << " plants\n";
            }
            if (viewpoint_ == FixedViewpoint::DriftwoodDetail) {
                std::cout << "island-simulator: driftwood QA per-instance near LOD "
                          << naturalPropRenderer_->VisibleDriftwoodNearCount() << " pieces\n";
            }
            if (worldDebugVisible_) {
                const PlayerMovementTrace& movementTrace = playerMotion_.MovementTrace();
                std::cout << "island-simulator: world debug frame ground samples "
                          << worldDebugRenderer_->GroundContactSampleCount() << ", supports "
                          << worldDebugRenderer_->GroundSupportSampleCount()
                          << ", steep " << worldDebugRenderer_->SteepGroundSampleCount()
                          << "; wildlife markers "
                          << worldDebugRenderer_->WildlifeMarkerCount()
                          << ", targets " << worldDebugRenderer_->WildlifeTargetCount()
                          << ", homes " << worldDebugRenderer_->MonkeyHomeRangeCount()
                          << "; bird routes " << worldDebugRenderer_->BirdRouteRegionCount()
                          << ", spawns " << worldDebugRenderer_->BirdSpawnMarkerCount()
                          << ", radial targets "
                          << worldDebugRenderer_->BirdRouteTargetCount()
                          << "; movement depenetrate/obstacle/terrain/slide/clamp "
                          << movementTrace.obstacleDepenetrations << '/'
                          << movementTrace.obstacleRejections << '/'
                          << movementTrace.terrainRejections << '/'
                          << movementTrace.slideSubsteps << '/'
                          << (movementTrace.safetyClamped ? 1 : 0)
                          << "; audio zones " << worldDebugRenderer_->AudioZoneCount()
                          << ", candidates "
                          << worldDebugRenderer_->AudioCandidateCount()
                          << ", active emitters "
                          << worldDebugRenderer_->ActiveAudioEmitterCount() << '\n';
            }
        }
        ++drawnFrames_;
        if(UsesSpiderViewpoint()&&drawnFrames_==1U) {
            std::ostringstream report;
            report << "island-simulator: spider render proof: spiders " << spiderRenderer_->VisibleCount()
                << '/' << spiders_->Spiders().size() << " visible, " << spiderRenderer_->DrawCallCount() << " draw;"
                << " contact-shadows=" << contactShadowRenderer_->VisibleSpiderShadowCount();
            std::cout << report.str() << std::endl;MirrorRuntimeDiagnosticToPlatformLog(report.str());
            const auto& subject=spiders_->Spiders()[spiderViewSubject_];
            const float groundTransmission=canopyLightField_.Sample(subject.x,subject.z);
            const float bodyTransmission=canopyLightField_.SampleReceiver(
                subject.x,subject.y+.04f,subject.z,subject.y);
            const auto tint=CanopyTransmissionTint(bodyTransmission);
            std::ostringstream lighting;lighting.precision(9);
            lighting << "island-simulator: spider lighting ground-transmission=" << groundTransmission
                << " body-transmission=" << bodyTransmission
                << " body-tint=" << tint.red << ',' << tint.green << ',' << tint.blue;
            std::cout << lighting.str() << std::endl;MirrorRuntimeDiagnosticToPlatformLog(lighting.str());
            const auto spot=terrain_.Sample(subject.x,subject.z);
            std::ostringstream where;where.precision(6);
            where << "island-simulator: spider QA spot position=" << subject.x << ',' << subject.y << ',' << subject.z
                << " shore-distance=" << spot.signedShoreDistance << " slope=" << spot.slopeRadians
                << " cloud-cover=" << CloudShadow::Cover(subject.x,subject.z,timeSeconds)
                << " sun-shadow-casters=" << (sunShadowMap_?sunShadowMap_->LastCasterCount():0);
            std::cout << where.str() << std::endl;MirrorRuntimeDiagnosticToPlatformLog(where.str());
        }
        if(viewpoint_==FixedViewpoint::SpiderWebDetail&&drawnFrames_==1U) {
            std::ostringstream report;
            report<<"island-simulator: spider-web render proof: webs "
                <<spiderWebRenderer_->VisibleCount()<<'/'<<spiderWebRenderer_->SiteCount()
                <<" visible, strands "<<spiderWebRenderer_->StrandCount()<<", "
                <<spiderWebRenderer_->DrawCallCount()<<" draw";
            std::cout<<report.str()<<std::endl;MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if(viewpoint_==FixedViewpoint::GoatDetail&&drawnFrames_==1U) {
            std::ostringstream report;
            report << "island-simulator: goat render proof: goats " << goatRenderer_->VisibleCount()
                << '/' << goats_->Goats().size() << " visible, " << goatRenderer_->DrawCallCount() << " draw;";
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if(viewpoint_==FixedViewpoint::SeasonalWetlandDetail&&drawnFrames_==1U&&wetlandMistRenderer_) {
            const auto& first=wetlandMist_.Puffs().front();
            std::ostringstream report;
            report << "island-simulator: wetland mist proof: visible=" << wetlandMistRenderer_->VisibleCount()
                << " first=" << first.x << ',' << first.y << ',' << first.z << " density=" << first.density << " size=" << first.size
                << " camera=" << renderCameraPosition_.X << ',' << renderCameraPosition_.Y << ',' << renderCameraPosition_.Z;
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if(viewpoint_==FixedViewpoint::BirdPreenDetail&&drawnFrames_==1U&&!birdFlock_.Birds().empty()) {
            const BirdState& bird=birdFlock_.Birds().front();
            std::ostringstream report;
            report << "island-simulator: bird preen proof: bird 0 behavior=" << static_cast<int>(bird.behavior)
                << " preen=" << bird.preenAmount << " preening=" << birdFlock_.PreeningCount()
                << " near=" << birdRenderer_->VisibleNearBirdCount();
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if((viewpoint_==FixedViewpoint::DeerDetail||viewpoint_==FixedViewpoint::DeerBeddedDetail)&&drawnFrames_==1U) {
            std::ostringstream report;
            report << "island-simulator: deer render proof: deer "
                << deerRenderer_->VisibleCount() << '/' << deer_->Deer().size()
                << " visible, " << deerRenderer_->DrawCallCount() << " draw";
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if ((viewpoint_ == FixedViewpoint::MouseDetail
             || viewpoint_ == FixedViewpoint::RatDetail) && drawnFrames_ == 1U) {
            const bool mouse = viewpoint_ == FixedViewpoint::MouseDetail;
            std::ostringstream report;
            report << "island-simulator: " << (mouse ? "mouse" : "rat")
                << " render proof: " << (mouse ? "mice " : "rats ")
                << (mouse ? butterflyRenderer_->VisibleMouseCount()
                          : butterflyRenderer_->VisibleRatCount())
                << '/' << (mouse ? RodentPopulation::MouseCount : RodentPopulation::RatCount)
                << " visible in shared draw";
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if (viewpoint_ == FixedViewpoint::FlyDetail && drawnFrames_ == 1U) {
            std::ostringstream report;
            report << "island-simulator: fly render proof: flies "
                << butterflyRenderer_->VisibleFlyCount() << '/' << FlySwarms::Count
                << " visible in shared draw";
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if(viewpoint_==FixedViewpoint::AntDetail&&drawnFrames_==1U) {
            std::ostringstream report;
            report<<"island-simulator: ant render proof: ants "
                <<butterflyRenderer_->VisibleAntCount()<<'/'<<AntTrails::Count
                <<" visible in shared draw";
            std::cout<<report.str()<<'\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if (viewpoint_ == FixedViewpoint::CicadaDetail && drawnFrames_ == 1U) {
            std::ostringstream report;
            report << "island-simulator: cicada render proof: cicadas "
                << butterflyRenderer_->VisibleCicadaCount() << '/' << CicadaColony::Count
                << " visible in shared draw";
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if (viewpoint_ == FixedViewpoint::BeetleDetail && drawnFrames_ == 1U) {
            std::ostringstream report;
            report << "island-simulator: beetle render proof: beetles "
                << butterflyRenderer_->VisibleBeetleCount() << '/' << BeetlePopulation::Count
                << " visible in shared draw";
            std::cout << report.str() << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        const bool snakeCapture = UsesSnakeCaptureClock();
        const bool goatCapture = UsesGoatCaptureClock();
        const bool boarCapture = UsesBoarCaptureClock();
        const bool residentCapture=UsesResidentCaptureClock();
        const bool bearCapture=UsesBearCaptureClock();
        if(boarCapture&&drawnFrames_%8U==0&&drawnFrames_<=64U) {
            GpuDiagnosticScope gpuPass("capture-readback");
            CaptureFrame(capturePath_+".boar-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            const auto& subject=boarHerd_.Boars().front();
            std::ostringstream report;report.precision(9);
            report << "island-simulator: boar motion stage=" << drawnFrames_/8U-1U
                << " updates=" << boarCaptureUpdates_ << " time=" << timeSeconds
                << " visible=" << boarRenderer_->VisibleCount() << " position="
                << subject.x << ',' << subject.y << ',' << subject.z
                << " speed=" << subject.speed << " phase=" << subject.gaitPhase
                << " rooting=" << subject.rootingPose;
            std::cout << report.str() << std::endl;
        }
        const bool spiderCapture = UsesSpiderCaptureClock();
        if(spiderCapture&&drawnFrames_%8U==0&&drawnFrames_<=64U) {
            GpuDiagnosticScope gpuPass("capture-readback");
            CaptureFrame(capturePath_+".spider-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            const auto& subject=spiders_->Spiders()[spiderViewSubject_];
            std::ostringstream report;report.precision(9);
            report << "island-simulator: spider motion stage=" << drawnFrames_/8U-1U
                << " updates=" << spiderCaptureUpdates_ << " time=" << timeSeconds
                << " visible=" << spiderRenderer_->VisibleCount() << " position="
                << subject.x << ',' << subject.y << ',' << subject.z
                << " heading=" << subject.heading << " speed=" << subject.speed << " feet=";
            for(std::size_t leg=0;leg<8;++leg) {
                const auto p=subject.pose.feet[leg];
                report << p.x << ',' << p.y << ',' << p.z << ',' << (subject.pose.stance[leg]?1:0) << ';';
            }
            std::cout << report.str() << std::endl;
        }
        if(goatCapture&&drawnFrames_%8U==0&&drawnFrames_<=64U) {
            GpuDiagnosticScope gpuPass("capture-readback");
            CaptureFrame(capturePath_+".goat-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            const auto& subject=goats_->Goats().front();
            std::cout << "island-simulator: goat motion stage=" << drawnFrames_/8U-1U
                << " updates=" << goatCaptureUpdates_ << " time=" << timeSeconds
                << " visible=" << goatRenderer_->VisibleCount() << " position="
                << subject.x << ',' << subject.y << ',' << subject.z
                << " speed=" << subject.speed << " phase=" << subject.phase
                << " grazing=" << subject.grazingPose << '\n';
        }
        if(residentCapture&&drawnFrames_%8U==0&&drawnFrames_<=64U) {
            GpuDiagnosticScope gpuPass("capture-readback");
            CaptureFrame(capturePath_+".resident-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            const auto& subject=resident_->State();
            const float forwardX=std::sin(subject.heading),forwardZ=std::cos(subject.heading);
            const auto armSwing=[&](const std::size_t side) {
                return (subject.pose.wrists[side].x-subject.pose.shoulders[side].x)*forwardX
                    +(subject.pose.wrists[side].z-subject.pose.shoulders[side].z)*forwardZ;
            };
            std::cout<<"island-simulator: resident motion stage="<<drawnFrames_/8U-1U
                <<" updates="<<residentCaptureUpdates_<<" visible="<<residentRenderer_->VisibleCount()
                <<" position="<<subject.x<<','<<subject.y<<','<<subject.z
                <<" speed="<<subject.speed<<" mode="<<static_cast<int>(subject.mode)
                <<" feet="<<subject.pose.legs[0].foot.centre.x<<','<<subject.pose.legs[0].foot.centre.y
                <<','<<subject.pose.legs[0].foot.centre.z<<','<<(subject.pose.legs[0].stance?1:0)<<';'
                <<subject.pose.legs[1].foot.centre.x<<','<<subject.pose.legs[1].foot.centre.y
                <<','<<subject.pose.legs[1].foot.centre.z<<','<<(subject.pose.legs[1].stance?1:0)
                <<" wrists="<<subject.pose.wrists[0].x<<','<<subject.pose.wrists[0].y<<','
                <<subject.pose.wrists[0].z<<';'<<subject.pose.wrists[1].x<<','
                <<subject.pose.wrists[1].y<<','<<subject.pose.wrists[1].z
                <<" arm-swing="<<armSwing(0)<<','<<armSwing(1)
                <<" triangles="<<residentRenderer_->TriangleCount()<<'/'
                <<residentRenderer_->ShadowTriangleCount()<<'\n';
        }
        if(bearCapture&&drawnFrames_%8U==0&&drawnFrames_<=64U) {
            GpuDiagnosticScope gpuPass("capture-readback");
            CaptureFrame(capturePath_+".bear-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            const auto& subject=bear_->State();
            std::cout<<"island-simulator: bear motion stage="<<drawnFrames_/8U-1U
                <<" updates="<<bearCaptureUpdates_<<" visible="<<bearRenderer_->VisibleCount()
                <<" position="<<subject.x<<','<<subject.y<<','<<subject.z
                <<" speed="<<subject.speed<<" behavior="<<static_cast<int>(subject.behavior)
                <<" forage="<<subject.foragePose<<" feet=";
            for(const auto& leg:subject.pose.legs)
                std::cout<<leg.paw.x<<','<<leg.paw.y<<','<<leg.paw.z<<','<<(leg.stance?1:0)<<';';
            std::cout<<" triangles="<<bearRenderer_->TriangleCount()<<'/'
                <<bearRenderer_->ShadowTriangleCount()<<'\n';
        }
        if(!snakeCapture && (viewpoint_==FixedViewpoint::SnakeDetail || viewpoint_==FixedViewpoint::SnakeHeadDetail
                || viewpoint_==FixedViewpoint::TreeSnakeDetail)
            && drawnFrames_%8U==0 && drawnFrames_<=32U) {
            const auto& subject=viewpoint_==FixedViewpoint::TreeSnakeDetail
                ? snakes_->TreeSnakes().front() : snakes_->Snakes().front();
            const auto head=subject.body.front();
            std::ostringstream report;
            report << "island-simulator: snake runtime sample frame=" << drawnFrames_
                << " head=" << head.x << ',' << head.y << ',' << head.z
                << " speed=" << subject.speed << " visible=" << snakeRenderer_->VisibleCount();
            std::cout << report.str() << '\n' << std::flush;
            MirrorRuntimeDiagnosticToPlatformLog(report.str());
        }
        if (snakeCapture && drawnFrames_%8U == 0 && drawnFrames_ <= 32U) {
            GpuDiagnosticScope gpuPass("capture-readback");
            CaptureFrame(capturePath_+".snake-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            const auto& snake = viewpoint_==FixedViewpoint::TreeSnakeDetail
                ? snakes_->TreeSnakes().front() : snakes_->Snakes().front();
            std::cout << "island-simulator: snake motion stage=" << drawnFrames_/8U-1U
                << " updates=" << snakeCaptureUpdates_ << " time=" << timeSeconds
                << " visible=" << snakeRenderer_->VisibleCount() << " speed=" << snake.speed
                << " phase=" << snake.phase << " body=";
            for (const auto& p : snake.body) std::cout << p.x << ',' << p.y << ',' << p.z << ';';
            std::cout << '\n';
        }
        if (treeQaStage < monkeyTreeQaPoses_.size() && drawnFrames_%4U == 0) {
            GpuDiagnosticScope gpuPass("capture-readback");
            if (!capturePath_.empty()) CaptureFrame(capturePath_+".tree-stage-"+std::to_string(treeQaStage)+".ppm");
            const auto& subject = renderedMonkeys[monkeyQaSubjectIndex_];
            const auto weights = monkeyRenderer_->SubjectTreeReachWeights();
            std::cout << "island-simulator: monkey tree stage=" << treeQaStage
                << " subject=" << monkeyQaSubjectIndex_ << " tree=" << subject.treeIndex
                << " elapsed=" << subject.treeInspectionSeconds << " clock=" << subject.animationSeconds
                << " weights=" << weights[0] << ',' << weights[1] << '\n';
            if (viewpoint_==FixedViewpoint::MonkeyClimbDetail) {
                const auto centre=MonkeyVisualCentre(subject);
                std::ostringstream report;
                report << "island-simulator: monkey climb stage=" << treeQaStage << " subject=" << monkeyQaSubjectIndex_
                    << " tree=" << subject.treeIndex << " time=" << subject.treeClock.seconds
                    << " active=" << static_cast<bool>(subject.treeExcursion) << " centre-y=" << centre.y;
                std::cout << report.str() << '\n';
                if (capturePath_.empty()) MirrorRuntimeDiagnosticToPlatformLog(report.str());
            }
        }
        if (viewpoint_==FixedViewpoint::MonkeyClimbDetail && drawnFrames_==36U) {
            const char* report="island-simulator: monkey climb held summit presented after eight cycle stages";
            std::cout << report << '\n';
            if (capturePath_.empty()) MirrorRuntimeDiagnosticToPlatformLog(report);
        }
        if(UsesCampfireCaptureClock()&&drawnFrames_%8U==0&&drawnFrames_<=32U) {
            CaptureFrame(capturePath_+".fire-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            std::cout<<"island-simulator: campfire motion stage="<<drawnFrames_/8U-1U
                <<" updates="<<campfireCaptureUpdates_<<" time="<<timeSeconds
                <<" draws="<<campfireRenderer_->DrawCallCount()<<" effect-triangles="
                <<campfireRenderer_->EffectTriangleCount()<<'\n';
        }
        if(UsesStreamCaptureClock()&&drawnFrames_%8U==0&&drawnFrames_<=32U) {
            CaptureFrame(capturePath_+".stream-stage-"+std::to_string(drawnFrames_/8U-1U)+".ppm");
            std::cout<<"island-simulator: stream motion stage="<<drawnFrames_/8U-1U
                <<" frames="<<drawnFrames_<<" time="<<timeSeconds
                <<" draws="<<streamWaterRenderer_->DrawCallCount()<<" triangles=6080\n";
        }
        const std::uint32_t targetFrames = goatCapture||spiderCapture||boarCapture||residentCapture||bearCapture ? 64U : snakeCapture||UsesCampfireCaptureClock()||UsesStreamCaptureClock()||QuadDriveView() ? 32U : !monkeyTreeQaPoses_.empty() ? 36U : !capturePath_.empty()
                && (viewpoint_ == FixedViewpoint::MonkeyDetail || viewpoint_ == FixedViewpoint::MonkeyTreeDetail || viewpoint_ == FixedViewpoint::MonkeyClimbDetail)
            ? MonkeyCaptureFrames : SmokeTestFrames;
        if (!capturePath_.empty() && drawnFrames_ == targetFrames) {
            {
                // the frames so far (in the software renderer: CPU work and its vertex shading)
                std::ostringstream profile;
                profile << "island-simulator: capture-profile frames=" << drawnFrames_
                        << " updates=" << runtimeTimingUpdates_ << " update-ms-per-update "
                        << updateProfile_.Top(static_cast<double>(std::max<std::uint64_t>(1, runtimeTimingUpdates_)), 20)
                        << " | render-pass-cpu-ms-per-frame "
                        << RenderPassCpuProfile().Top(static_cast<double>(std::max<std::uint32_t>(1, drawnFrames_)), 24);
                std::cout << profile.str() << '\n';
                MirrorRuntimeDiagnosticToPlatformLog(profile.str());
            }
            {
                GpuDiagnosticScope gpuPass("capture-readback");
                CaptureFrame(capturePath_);
            }
            if (QuadDriveView()) {
                const auto q = quad_.Position();
                if (viewpoint_ == FixedViewpoint::QuadSeaDetail) {
                    const BikeGround under = QuadGround(q.x, q.z);
                    std::cout << "island-simulator: quad QA sea: water depth " << under.waterDepth << ", body " << q.y
                              << ", airbox " << (QuadBike::AirboxFlooded(q, under) ? "flooded" : "clear")
                              << ", engine " << (quadEngineOn_ ? "running" : "dead") << '\n';
                } else if (viewpoint_ == FixedViewpoint::QuadStreamDetail) {
                    const StreamCourse& course = streamDrift_.Course();
                    const auto& centre = course.Station(45);
                    float bed = 0.0f, depth = 0.0f;
                    const bool channel = StreamWaterAt(q.x, q.z, bed, depth);
                    std::cout << "island-simulator: quad QA stream: from the centreline "
                              << std::hypot(q.x - centre.x, q.z - centre.z) << " m, ground "
                              << QuadGround(q.x, q.z).height << ", water depth " << (channel ? depth : 0.0f)
                              << ", body " << q.y << ", " << streamRings_.Count() << " rings on the stream\n";
                } else if (viewpoint_ == FixedViewpoint::QuadBearDetail && bear_) {
                    std::cout << "island-simulator: quad QA body: bear, distance "
                              << std::hypot(q.x - bear_->State().x, q.z - bear_->State().z) << " m\n";
                } else if (viewpoint_ == FixedViewpoint::QuadJettyDetail || viewpoint_ == FixedViewpoint::QuadHelicopterDetail
                    || viewpoint_ == FixedViewpoint::QuadLookoutDetail) {
                    std::cout << "island-simulator: quad QA structure: "
                              << (viewpoint_ == FixedViewpoint::QuadJettyDetail ? "jetty"
                                  : viewpoint_ == FixedViewpoint::QuadHelicopterDetail ? "helicopter" : "lookout")
                              << ", ground " << QuadGround(q.x, q.z).height << ", body " << q.y
                              << ", walls " << (QuadWallBlocked(q.x, q.z) ? "touching" : "clear");
                    if (viewpoint_ == FixedViewpoint::QuadHelicopterDetail && helicopterPlacement_) std::cout << ", helicopter distance "
                        << std::hypot(q.x - helicopterPlacement_->x, q.z - helicopterPlacement_->z);
                    if (viewpoint_ == FixedViewpoint::QuadLookoutDetail && lookoutPlacement_) {
                        const auto& lookout = *lookoutPlacement_;
                        const float across = (q.x - lookout.x) * lookout.AcrossX() + (q.z - lookout.z) * lookout.AcrossZ();
                        std::cout << ", across the ramp " << across << " m";
                    }
                    if (viewpoint_ == FixedViewpoint::QuadJettyDetail && jettyPlacement_) {
                        const auto& jetty = *jettyPlacement_;
                        const float along = (q.x - jetty.rootX) * jetty.dirX + (q.z - jetty.rootZ) * jetty.dirZ;
                        const float across = (q.x - jetty.rootX) * jetty.dirZ - (q.z - jetty.rootZ) * jetty.dirX;
                        std::cout << ", along the jetty " << along << " m, across " << across << " m, deck "
                                  << jetty.SupportHeight(q.x, q.z, -100.0f);
                    }
                    std::cout << '\n';
                } else if (viewpoint_ != FixedViewpoint::QuadDriveDetail) {
                    const bool bunker = viewpoint_ == FixedViewpoint::QuadBunkerDetail;
                    const float depth = bunker ? bunkerLayout_.Local({q.x, q.y, q.z}).z
                                               : terrainRenderer_->Cave().Local({q.x, q.y, q.z}).z;
                    std::cout << "island-simulator: quad QA shelter: " << (bunker ? "bunker" : "cave")
                              << ", site " << (quadSite_ == QuadSite::Bunker ? "bunker" : quadSite_ == QuadSite::Cave ? "cave" : "open")
                              << ", depth " << depth << " m, floor " << QuadGround(q.x, q.z).height
                              << ", body " << q.y << ", walls " << (QuadWallBlocked(q.x, q.z) ? "touching" : "clear") << '\n';
                }
                std::cout << "island-simulator: quad QA drive: " << static_cast<int>(std::lround(quad_.Speed() * 3.6f))
                          << " km/h at (" << static_cast<int>(q.x) << ", " << static_cast<int>(q.z) << "), "
                          << tyreTracks_.LaidCount() << " rut pieces laid, " << wheelDust_.Emitted() << " dust puffs, "
                          << wheelSpray_.Emitted() << " spray puffs, " << (soundscape_ ? soundscape_->PlayedQuadImpactCount() : 0)
                          << " knocks, " << (soundscape_ ? soundscape_->PlayedQuadSplashCount() : 0) << " splashes\n";
            }
            if (viewpoint_ == FixedViewpoint::MonkeyDetail || viewpoint_ == FixedViewpoint::MonkeyTreeDetail || viewpoint_ == FixedViewpoint::MonkeyClimbDetail) {
                const auto& subject = monkeyTroop_.Monkeys()[monkeyQaSubjectIndex_];
                std::cout << "island-simulator: monkey capture visible "
                          << monkeyRenderer_->VisibleMonkeyCount() << "; near "
                          << monkeyRenderer_->VisibleNearMonkeyCount() << "; draws "
                          << monkeyRenderer_->DrawCallCount() << "; subject "
                          << subject.x << ',' << subject.y << ',' << subject.z << '\n';
                std::cout << "island-simulator: monkey subject paired tree grips "
                          << monkeyRenderer_->SubjectHasTreeGrips() << '\n';
                std::cout << "island-simulator: monkey crossfades "
                          << monkeyRenderer_->ActiveCrossfadeCount() << '/'
                          << monkeyRenderer_->CompletedCrossfadeCount()
                          << " active/completed\n";
            }
            if(viewpoint_==FixedViewpoint::ResidentDetail) {
                std::cout<<"island-simulator: resident capture visible "<<residentRenderer_->VisibleCount()
                    <<"; draws "<<residentRenderer_->DrawCallCount()<<"; body/shadow triangles "
                    <<residentRenderer_->TriangleCount()<<'/'<<residentRenderer_->ShadowTriangleCount()<<'\n';
            }
            if(viewpoint_==FixedViewpoint::BearDetail) {
                std::cout<<"island-simulator: bear capture visible "<<bearRenderer_->VisibleCount()
                    <<"; draws "<<bearRenderer_->DrawCallCount()<<"; body/shadow triangles "
                    <<bearRenderer_->TriangleCount()<<'/'<<bearRenderer_->ShadowTriangleCount()
                    <<"; near-detail "<<bearRenderer_->UsesNearDetail()<<'\n';
            }
        }
        if (smokeTest_ && drawnFrames_ >= targetFrames) {
            std::cout << "island-simulator: smoke test rendered " << drawnFrames_ << " frames\n";
            Exit();
        }
        if (benchmarkSceneSamplePending_) {
            RecordBenchmarkSceneSample();
            benchmarkSceneSamplePending_ = false;
        }
        if (benchmarkReportPending_) {
            WriteBenchmarkReport();
            benchmarkReportPending_ = false;
            Exit();
        }
        EndGpuDiagnosticFrame();
        RecordRuntimeFrameTiming(drawStarted);
    }

    GetTypeNameCPP(IslandGame, "IslandSimulator.IslandGame")
}
