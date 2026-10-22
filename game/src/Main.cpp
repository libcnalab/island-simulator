#include "CNA/Platform/Entrypoint.hpp"

#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "IslandSimulator/IslandGame.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#ifdef __ANDROID__
#include "IslandSimulator/DiagnosticStreamMirror.hpp"
#endif

#include <exception>
#include <charconv>
#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>
#include <utility>

namespace
{
    [[nodiscard]] const char* ViewpointName(const island::FixedViewpoint viewpoint) noexcept
    {
        using island::FixedViewpoint;
        switch (viewpoint) {
        case FixedViewpoint::None: return "interactive";
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
        case FixedViewpoint::FrigatebirdDetail: return "frigatebird";
        case FixedViewpoint::WhaleSkeletonDetail: return "whale-skeleton";
        case FixedViewpoint::IdolDetail: return "idol";
        case FixedViewpoint::StoneSignDetail: return "help-sign";
        case FixedViewpoint::LighthouseDetail: return "lighthouse";
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
        case FixedViewpoint::QuadBearDetail: return "quad-bear";
        case FixedViewpoint::QuadStreamDetail: return "quad-stream";
        case FixedViewpoint::QuadSeaDetail: return "quad-sea";
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
        }
        return "unknown";
    }
}

int main(const int argc, char* argv[])
{
    island::InitializeRuntimeDiagnostics(argc, argv);
#ifdef __ANDROID__
    island::DiagnosticStreamMirror stderrMirror(
        std::cerr, island::MirrorRuntimeDiagnosticToPlatformLog);
#endif
    bool smokeTest = false;
    bool showDebugOverlay = false;
    bool showWorldDebug = false;
    bool inputDiagnostics = false;
    bool noUserSettings = false;
    island::FixedViewpoint viewpoint = island::FixedViewpoint::None;
    std::string capturePath;
    std::string benchmarkPath;
    std::string startupReportPath;
    std::optional<island::CanopyLodInspection> canopyLodInspection;
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument(argv[index]);
        if (argument == "--canopy-lod-inspection") {
            island::CanopyLodInspection inspection{};
            const auto coordinate = [](const std::string_view text, int& value) {
                const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
                return result.ec == std::errc{} && result.ptr == text.data() + text.size()
                    && value >= -500000 && value <= 500000;
            };
            if (canopyLodInspection || index + 3 >= argc
                || !coordinate(argv[index + 1], inspection.xMillimetres)
                || !coordinate(argv[index + 2], inspection.zMillimetres)
                || (std::string_view(argv[index + 3]) != "near"
                    && std::string_view(argv[index + 3]) != "far")) {
                std::cerr << "island-simulator: --canopy-lod-inspection requires unique "
                             "X_MM Z_MM near|far (integer coordinates within +/-500000)\n";
                return 2;
            }
            inspection.near = std::string_view(argv[index + 3]) == "near";
            canopyLodInspection = inspection;
            index += 3;
        } else if (argument == "--smoke-test") {
            smokeTest = true;
        } else if (argument == "--debug-overlay") {
            showDebugOverlay = true;
        } else if (argument == "--world-debug") {
            showDebugOverlay = true;
            showWorldDebug = true;
        } else if (argument == "--input-diagnostics") {
            inputDiagnostics = true;
        } else if (argument == "--no-user-settings") {
            noUserSettings = true;
        } else if (argument == "--weather" && index + 1 < argc) {
            const auto kind = island::WeatherFromName(argv[++index]);
            if (!kind) {
                std::cerr << "island-simulator: unknown --weather (clear, partly-cloudy, overcast, showers, thunderstorm)\n";
                return 2;
            }
            island::weather_now::SetRequested(kind);
        } else if (argument == "--wet-ground") {
            island::weather_now::SetRequestedWetGround(true);
        } else if (argument == "--game-day" && index + 1 < argc) {
            island::day_night_options::SetRequestedDay(std::strtof(argv[++index], nullptr));
        } else if (argument == "--time-of-day" && index + 1 < argc) {
            island::day_night_options::SetRequestedHour(std::strtof(argv[++index], nullptr));
        } else if (argument == "--no-sun-shadows") {
            island::SunShadowMap::SetQaDisabled(true);
        } else if (argument == "--overview") {
            viewpoint = island::FixedViewpoint::Overview;
        } else if (argument == "--viewpoint") {
            if (++index >= argc) {
                std::cerr << "island-simulator: --viewpoint requires overview, bay, bay-ground, beach, jungle, snags, sapling, "
                             "east, headland, tide-pools, turtle-nest, cave, cave-top, cave-inside, cave-back, bunker, bunker-corridor, bunker-computers, bunker-reactor, bunker-service, bunker-end, campfire, campfire-wide, resident, bear, clearing-edge, canopy (inspection only), west, stream, wetland, mangroves, grass, ground, ground-broadleaf, bush, litter, contact, movement, palms, fallen-palm, broken-palm, water, "
                             "wet-sand, current, distant, props, driftwood, reef, reef-habitat, fronds, birds, herons, bats, bat-roost, fish, "
                             "shark, dolphins, monkeys, monkey-tree, monkey-climb, shipwreck, shipwreck-wide, tracks, fruit, cascade, jetty, bridge, lookout, log, hut, hut-material, helicopter, helicopter-inside, helicopter-approach, crabs, hermit-crabs, coconut-crabs, crocodiles, boars, goats, spiders, spiders-bright, spider-web, snakes, snake-head, tree-snakes, butterflies, geckos, skinks, dragonflies, frogs, mice, rats, flies, ants, cicadas, turtles, or traversal\n";
                return 2;
            }
            const std::string_view name(argv[index]);
            if (name == "overview") {
                viewpoint = island::FixedViewpoint::Overview;
            } else if (name == "bay") {
                viewpoint = island::FixedViewpoint::BayOverview;
            } else if (name == "headland") {
                viewpoint = island::FixedViewpoint::HeadlandDetail;
            } else if (name == "tide-pools") {
                viewpoint = island::FixedViewpoint::TidePoolDetail;
            } else if (name == "turtle-nest") {
                viewpoint = island::FixedViewpoint::TurtleNestDetail;
            } else if (name == "cave") {
                viewpoint = island::FixedViewpoint::CaveExterior;
            } else if (name == "cave-top") {
                viewpoint = island::FixedViewpoint::CaveOverhead;
            } else if (name == "cave-inside") {
                viewpoint = island::FixedViewpoint::CaveInterior;
            } else if (name == "cave-back") {
                viewpoint = island::FixedViewpoint::CaveBack;
            } else if(name=="bunker") {
                viewpoint=island::FixedViewpoint::BunkerExterior;
            } else if(name=="bunker-corridor") {
                viewpoint=island::FixedViewpoint::BunkerCorridor;
            } else if(name=="bunker-computers") {
                viewpoint=island::FixedViewpoint::BunkerComputerRoom;
            } else if(name=="bunker-reactor") {
                viewpoint=island::FixedViewpoint::BunkerReactorRoom;
            } else if(name=="bunker-service") {
                viewpoint=island::FixedViewpoint::BunkerServiceRoom;
            } else if(name=="bunker-end") {
                viewpoint=island::FixedViewpoint::BunkerEndBulkhead;
            } else if(name=="campfire") {
                viewpoint=island::FixedViewpoint::CampfireDetail;
            } else if(name=="campfire-wide") {
                viewpoint=island::FixedViewpoint::CampfireWide;
            } else if(name=="resident") {
                viewpoint=island::FixedViewpoint::ResidentDetail;
            } else if(name=="resident-tending") {
                viewpoint=island::FixedViewpoint::ResidentTendDetail;
            } else if(name=="resident-fishing") {
                viewpoint=island::FixedViewpoint::ResidentFishDetail;
            } else if(name=="bear") {
                viewpoint=island::FixedViewpoint::BearDetail;
            } else if(name=="bear-lying") {
                viewpoint=island::FixedViewpoint::BearLyingDetail;
            } else if (name == "bay-ground") {
                viewpoint = island::FixedViewpoint::BayBeachDetail;
            } else if (name == "beach") {
                viewpoint = island::FixedViewpoint::BeachPath;
            } else if (name == "jungle") {
                viewpoint = island::FixedViewpoint::JungleTrail;
            } else if (name == "snags") {
                viewpoint = island::FixedViewpoint::StandingSnagDetail;
            } else if (name == "sapling") {
                viewpoint = island::FixedViewpoint::SaplingDetail;
            } else if (name == "east") {
                viewpoint = island::FixedViewpoint::EastClearingDetail;
            } else if (name == "clearing-edge") {
                viewpoint = island::FixedViewpoint::ClearingEdgeDetail;
            } else if (name == "canopy") {
                viewpoint = island::FixedViewpoint::CanopyInspection;
            } else if (name == "west") {
                viewpoint = island::FixedViewpoint::WestClearingDetail;
            } else if (name == "stream") {
                viewpoint = island::FixedViewpoint::StreamDetail;
            } else if (name == "wetland") {
                viewpoint = island::FixedViewpoint::SeasonalWetlandDetail;
            } else if (name == "mangroves") {
                viewpoint = island::FixedViewpoint::MangroveDetail;
            } else if (name == "grass") {
                viewpoint = island::FixedViewpoint::GrassDetail;
            } else if (name == "ground") {
                viewpoint = island::FixedViewpoint::GroundPlantDetail;
            } else if (name == "ground-broadleaf") {
                viewpoint = island::FixedViewpoint::GroundBroadleafDetail;
            } else if (name == "bush") {
                viewpoint = island::FixedViewpoint::BushDetail;
            } else if (name == "litter") {
                viewpoint = island::FixedViewpoint::FallenLeafDetail;
            } else if (name == "contact") {
                viewpoint = island::FixedViewpoint::GroundContactDetail;
            } else if (name == "movement") {
                viewpoint = island::FixedViewpoint::MovementTraceDetail;
            } else if (name == "palms") {
                viewpoint = island::FixedViewpoint::PalmDetail;
            } else if (name == "fallen-palm") {
                viewpoint = island::FixedViewpoint::FallenPalmDetail;
            } else if (name == "broken-palm") {
                viewpoint = island::FixedViewpoint::BrokenPalmDetail;
            } else if (name == "water") {
                viewpoint = island::FixedViewpoint::WaterlineDetail;
            } else if (name == "wet-sand") {
                viewpoint = island::FixedViewpoint::WetSandDetail;
            } else if (name == "current") {
                viewpoint = island::FixedViewpoint::CurrentBoundaryDetail;
            } else if (name == "distant") {
                viewpoint = island::FixedViewpoint::DistantSceneryDetail;
            } else if (name == "props") {
                viewpoint = island::FixedViewpoint::PropDetail;
            } else if (name == "driftwood") {
                viewpoint = island::FixedViewpoint::DriftwoodDetail;
            } else if (name == "reef") {
                viewpoint = island::FixedViewpoint::ReefDetail;
            } else if (name == "reef-habitat") {
                viewpoint = island::FixedViewpoint::ReefHabitatDetail;
            } else if (name == "anemone") {
                viewpoint = island::FixedViewpoint::AnemoneDetail;
            } else if (name == "walker-shadow") {
                viewpoint = island::FixedViewpoint::WalkerShadowDetail;
            } else if (name == "fronds") {
                viewpoint = island::FixedViewpoint::FallenFrondDetail;
            } else if (name == "birds") {
                viewpoint = island::FixedViewpoint::BirdDetail;
            } else if (name == "bird-preen") {
                viewpoint = island::FixedViewpoint::BirdPreenDetail;
            } else if (name == "frigatebird") {
                viewpoint = island::FixedViewpoint::FrigatebirdDetail;
            } else if (name == "whale-skeleton") {
                viewpoint = island::FixedViewpoint::WhaleSkeletonDetail;
            } else if (name == "idol") {
                viewpoint = island::FixedViewpoint::IdolDetail;
            } else if (name == "help-sign") {
                viewpoint = island::FixedViewpoint::StoneSignDetail;
            } else if (name == "lighthouse") {
                viewpoint = island::FixedViewpoint::LighthouseDetail;
            } else if (name == "ground-doves") {
                viewpoint = island::FixedViewpoint::GroundDoveDetail;
            } else if (name == "bats") {
                viewpoint = island::FixedViewpoint::FruitBatDetail;
            } else if (name == "bat-roost") {
                viewpoint = island::FixedViewpoint::FruitBatRoostDetail;
            } else if (name == "fish") {
                viewpoint = island::FixedViewpoint::FishDetail;
            } else if (name == "stingrays") {
                viewpoint = island::FixedViewpoint::StingrayDetail;
            } else if (name == "herons") {
                viewpoint = island::FixedViewpoint::ReefHeronDetail;
            } else if (name == "stingray-buried") {
                viewpoint = island::FixedViewpoint::StingrayBuriedDetail;
            } else if(name=="octopuses") {
                viewpoint=island::FixedViewpoint::OctopusDetail;
            } else if (name == "shark") {
                viewpoint = island::FixedViewpoint::SharkDetail;
            } else if (name == "dolphins") {
                viewpoint = island::FixedViewpoint::DolphinDetail;
            } else if (name == "monkeys") {
                viewpoint = island::FixedViewpoint::MonkeyDetail;
            } else if (name == "monkey-scratch") {
                viewpoint = island::FixedViewpoint::MonkeyScratchDetail;
            } else if (name == "monkey-tree") {
                viewpoint = island::FixedViewpoint::MonkeyTreeDetail;
            } else if (name == "monkey-climb") {
                viewpoint = island::FixedViewpoint::MonkeyClimbDetail;
            } else if (name == "shipwreck") {
                viewpoint = island::FixedViewpoint::ShipwreckDetail;
            } else if (name == "shipwreck-wide") {
                viewpoint = island::FixedViewpoint::ShipwreckWide;
            } else if (name == "tracks") {
                viewpoint = island::FixedViewpoint::AnimalTrackDetail;
            } else if (name == "fruit") {
                viewpoint = island::FixedViewpoint::FruitTreeDetail;
            } else if (name == "cascade") {
                viewpoint = island::FixedViewpoint::StreamCascadeDetail;
            } else if (name == "jetty") {
                viewpoint = island::FixedViewpoint::JettyDetail;
            } else if (name == "jetty-end") {
                viewpoint = island::FixedViewpoint::JettyEndDetail;
            } else if (name == "flight") {
                viewpoint = island::FixedViewpoint::FlightOverview;
            } else if (name == "quad") {
                viewpoint = island::FixedViewpoint::QuadDetail;
            } else if (name == "quad-ride") {
                viewpoint = island::FixedViewpoint::QuadRideDetail;
            } else if (name == "quad-chase") {
                viewpoint = island::FixedViewpoint::QuadChaseDetail;
            } else if (name == "quad-drive") {
                viewpoint = island::FixedViewpoint::QuadDriveDetail;
            } else if (name == "quad-bunker") {
                viewpoint = island::FixedViewpoint::QuadBunkerDetail;
            } else if (name == "quad-cave") {
                viewpoint = island::FixedViewpoint::QuadCaveDetail;
            } else if (name == "quad-jetty") {
                viewpoint = island::FixedViewpoint::QuadJettyDetail;
            } else if (name == "quad-helicopter") {
                viewpoint = island::FixedViewpoint::QuadHelicopterDetail;
            } else if (name == "quad-lookout") {
                viewpoint = island::FixedViewpoint::QuadLookoutDetail;
            } else if (name == "quad-bear") {
                viewpoint = island::FixedViewpoint::QuadBearDetail;
            } else if (name == "quad-stream") {
                viewpoint = island::FixedViewpoint::QuadStreamDetail;
            } else if (name == "quad-sea") {
                viewpoint = island::FixedViewpoint::QuadSeaDetail;
            } else if (name == "bridge") {
                viewpoint = island::FixedViewpoint::BridgeDetail;
            } else if (name == "lookout") {
                viewpoint = island::FixedViewpoint::LookoutDetail;
            } else if (name == "log") {
                viewpoint = island::FixedViewpoint::FallenLogDetail;
            } else if (name == "beach-vine") {
                viewpoint = island::FixedViewpoint::BeachVineDetail;
            } else if (name == "hut") {
                viewpoint = island::FixedViewpoint::HutDetail;
            } else if (name == "hut-material") {
                viewpoint = island::FixedViewpoint::HutMaterialDetail;
            } else if (name == "helicopter") {
                viewpoint = island::FixedViewpoint::HelicopterDetail;
            } else if (name == "helicopter-inside") {
                viewpoint = island::FixedViewpoint::HelicopterInteriorDetail;
            } else if (name == "helicopter-approach") {
                viewpoint = island::FixedViewpoint::HelicopterApproach;
            } else if (name == "crabs") {
                viewpoint = island::FixedViewpoint::CrabDetail;
            } else if (name == "hermit-crabs") {
                viewpoint = island::FixedViewpoint::HermitCrabDetail;
            } else if (name == "coconut-crabs") {
                viewpoint = island::FixedViewpoint::CoconutCrabDetail;
            } else if (name == "crocodiles") {
                viewpoint = island::FixedViewpoint::CrocodileDetail;
            } else if (name == "snakes") {
                viewpoint = island::FixedViewpoint::SnakeDetail;
            } else if (name == "snake-head") {
                viewpoint = island::FixedViewpoint::SnakeHeadDetail;
            } else if (name == "tree-snakes") {
                viewpoint = island::FixedViewpoint::TreeSnakeDetail;
            } else if (name == "goats") {
                viewpoint = island::FixedViewpoint::GoatDetail;
            } else if (name == "goat-lying") {
                viewpoint = island::FixedViewpoint::GoatLyingDetail;
            } else if (name == "deer") {
                viewpoint = island::FixedViewpoint::DeerDetail;
            } else if (name == "deer-bedded") {
                viewpoint = island::FixedViewpoint::DeerBeddedDetail;
            } else if (name == "spiders") {
                viewpoint = island::FixedViewpoint::SpiderDetail;
            } else if (name == "spiders-bright") {
                viewpoint = island::FixedViewpoint::SpiderBrightDetail;
            } else if(name=="spider-web") {
                viewpoint=island::FixedViewpoint::SpiderWebDetail;
            } else if (name == "boars") {
                viewpoint = island::FixedViewpoint::BoarDetail;
            } else if (name == "boar-lying") {
                viewpoint = island::FixedViewpoint::BoarLyingDetail;
            } else if (name == "butterflies") {
                viewpoint = island::FixedViewpoint::ButterflyDetail;
            } else if (name == "geckos") {
                viewpoint = island::FixedViewpoint::GeckoDetail;
            } else if (name == "skinks") {
                viewpoint = island::FixedViewpoint::SkinkDetail;
            } else if (name == "dragonflies") {
                viewpoint = island::FixedViewpoint::DragonflyDetail;
            } else if (name == "frogs") {
                viewpoint = island::FixedViewpoint::FrogDetail;
            } else if (name == "mice") {
                viewpoint = island::FixedViewpoint::MouseDetail;
            } else if (name == "rats") {
                viewpoint = island::FixedViewpoint::RatDetail;
            } else if (name == "flies") {
                viewpoint = island::FixedViewpoint::FlyDetail;
            } else if(name=="ants") {
                viewpoint=island::FixedViewpoint::AntDetail;
            } else if (name == "cicadas") {
                viewpoint = island::FixedViewpoint::CicadaDetail;
            } else if (name == "beetles") {
                viewpoint = island::FixedViewpoint::BeetleDetail;
            } else if (name == "turtles") {
                viewpoint = island::FixedViewpoint::SeaTurtleDetail;
            } else if (name == "traversal") {
                viewpoint = island::FixedViewpoint::BenchmarkTraversal;
            } else {
                std::cerr << "island-simulator: unknown viewpoint '" << name << "'\n";
                return 2;
            }
        } else if (argument == "--capture") {
            if (++index >= argc) {
                std::cerr << "island-simulator: --capture requires a path\n";
                return 2;
            }
            capturePath = argv[index];
            smokeTest = true;
        } else if (argument == "--benchmark") {
            if (++index >= argc) {
                std::cerr << "island-simulator: --benchmark requires a JSON output path\n";
                return 2;
            }
            benchmarkPath = argv[index];
        } else if (argument == "--startup-report") {
            if (++index >= argc) {
                std::cerr << "island-simulator: --startup-report requires a JSON output path\n";
                return 2;
            }
            startupReportPath = argv[index];
        } else {
            std::cerr << "island-simulator: unknown argument '" << argument << "'\n";
            return 2;
        }
    }
    if (viewpoint == island::FixedViewpoint::CanopyInspection && !canopyLodInspection) {
        std::cerr << "island-simulator: canopy viewpoint requires --canopy-lod-inspection\n";
        return 2;
    }
    if (canopyLodInspection && (capturePath.empty() || viewpoint == island::FixedViewpoint::None
        || viewpoint == island::FixedViewpoint::BenchmarkTraversal || !benchmarkPath.empty()
        || !startupReportPath.empty() || showDebugOverlay || showWorldDebug || inputDiagnostics)) {
        std::cerr << "island-simulator: canopy LOD inspection requires --capture and a fixed "
                     "viewpoint, without benchmark, startup report or debug modes\n";
        return 2;
    }
    const std::string_view mode = canopyLodInspection ? "canopy-lod-inspection"
        : !benchmarkPath.empty() ? "benchmark"
        : !capturePath.empty() ? "capture"
        : !startupReportPath.empty() ? "startup-report"
        : inputDiagnostics ? "input-diagnostics"
        : smokeTest ? "smoke-test"
        : "interactive";
    if (!benchmarkPath.empty()
        && (smokeTest || showDebugOverlay || showWorldDebug || inputDiagnostics)) {
        std::cerr << "island-simulator: --benchmark cannot be combined with smoke, capture, "
                     "or debug modes\n";
        return 2;
    }
    if (viewpoint == island::FixedViewpoint::BenchmarkTraversal && benchmarkPath.empty()) {
        std::cerr << "island-simulator: the traversal viewpoint requires --benchmark\n";
        return 2;
    }
    island::RecordRuntimeWorkload(mode, ViewpointName(viewpoint), capturePath,
                                  benchmarkPath, startupReportPath);
    try {
        island::IslandGame game(smokeTest, std::move(capturePath), viewpoint, showDebugOverlay,
                                inputDiagnostics, showWorldDebug, std::move(benchmarkPath),
                                std::move(startupReportPath),
                                !noUserSettings && !smokeTest && !inputDiagnostics
                                    && viewpoint == island::FixedViewpoint::None,
                                canopyLodInspection);
        game.Run();
        return 0;
    } catch (const std::exception& error) {
        const std::string diagnostic =
            std::string("island-simulator: fatal error: ") + error.what();
        std::cerr << diagnostic << '\n';
        island::MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
        return 1;
    } catch (...) {
        constexpr std::string_view diagnostic =
            "island-simulator: fatal error: unknown non-standard exception";
        std::cerr << diagnostic << '\n';
        island::MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
        return 1;
    }
}
