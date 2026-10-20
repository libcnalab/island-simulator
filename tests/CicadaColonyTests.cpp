#include "IslandSimulator/CicadaColony.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/JungleLayout.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
    void Check(const bool condition, const char* message)
    {
        if (!condition) throw std::runtime_error(message);
    }
}

int main()
try {
    const island::IslandTerrain terrain;
    const island::JungleLayout jungle(terrain);
    island::CicadaColony colony(jungle), repeated(jungle);
    Check(colony.Cicadas().size() == island::CicadaColony::Count,
          "wrong cicada count");
    bool rested = false, called = false, flew = false;
    float maximumExcursion = 0.0f;
    for (int frame = 0; frame < 60 * 180; ++frame) {
        colony.Update(1.0f / 60.0f, 0.0f, 8.0f, -70.0f);
        repeated.Update(1.0f / 60.0f, 0.0f, 8.0f, -70.0f);
        for (std::size_t index = 0; index < colony.Cicadas().size(); ++index) {
            const auto& cicada = colony.Cicadas()[index];
            const auto& copy = repeated.Cicadas()[index];
            const auto& home = colony.Homes()[index];
            Check(cicada.x == copy.x && cicada.y == copy.y && cicada.z == copy.z
                && cicada.behavior == copy.behavior,
                "cicada simulation is not deterministic");
            Check(std::isfinite(cicada.x) && std::isfinite(cicada.y)
                && std::isfinite(cicada.z) && std::isfinite(cicada.wingPhase),
                "cicada pose is not finite");
            const float forwardLength = std::hypot(std::hypot(cicada.forwardX,
                cicada.forwardZ), cicada.forwardY);
            const float normalLength = std::hypot(std::hypot(cicada.normalX,
                cicada.normalZ), cicada.normalY);
            Check(std::abs(forwardLength - 1.0f) < 0.001f
                && std::abs(normalLength - 1.0f) < 0.001f,
                "cicada frame is not normalized");
            rested |= cicada.behavior == island::CicadaBehavior::Rest;
            called |= cicada.behavior == island::CicadaBehavior::Call;
            flew |= cicada.behavior == island::CicadaBehavior::Flight;
            maximumExcursion = std::max(maximumExcursion,
                std::hypot(cicada.x - home.x, cicada.z - home.z) - home.radius);
            if (cicada.behavior != island::CicadaBehavior::Flight) {
                Check(std::abs(std::hypot(cicada.x - home.x, cicada.z - home.z)
                    - home.radius) < 0.001f && std::abs(cicada.y - home.y) < 0.001f,
                    "perched cicada left its host trunk");
            }
        }
    }
    Check(rested && called && flew && maximumExcursion > 0.8f,
          "cicada behavior cycle was not covered");
    const auto subject = colony.Cicadas()[1];
    colony.Update(0.0f, subject.x, subject.y, subject.z);
    Check(colony.Cicadas()[1].simulationTier == island::WildlifeSimulationTier::Full,
          "near cicada did not enter full simulation");
    colony.Update(0.0f, subject.x + 44.0f, subject.y, subject.z);
    Check(colony.Cicadas()[1].simulationTier == island::WildlifeSimulationTier::Reduced,
          "mid-distance cicada did not enter reduced simulation");
    colony.Update(std::numeric_limits<float>::quiet_NaN(),
                  std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
    for (const auto& cicada : colony.Cicadas()) {
        Check(std::isfinite(cicada.x) && std::isfinite(cicada.wingPhase),
              "invalid cicada input poisoned state");
    }
    {
        // The hush: a calling cicada falls silent when the walker comes within seven metres,
        // stays silent while the walker lingers, and starts up again once the walker has
        // gone; a passing animal hushes it as the walker does.
        island::CicadaColony hushed(jungle);
        std::size_t caller = island::CicadaColony::Count;
        for (int step = 0; step < 600 && caller == island::CicadaColony::Count; ++step) {
            hushed.Update(0.05f, 1000.0f, 0.0f, 1000.0f);
            for (std::size_t i = 0; i < island::CicadaColony::Count; ++i)
                if (hushed.Cicadas()[i].behavior == island::CicadaBehavior::Call) caller = i;
        }
        Check(caller < island::CicadaColony::Count, "no cicada called with the walker far off");
        const auto& voice = hushed.Cicadas()[caller];
        hushed.Update(0.05f, voice.x + 3.0f, voice.y, voice.z);
        Check(voice.behavior == island::CicadaBehavior::Rest && hushed.HushCount() >= 1U,
              "a walker three metres off did not hush the calling cicada");
        bool calledUnderWalker = false;
        for (int step = 0; step < 240; ++step) {
            hushed.Update(0.05f, voice.x + 3.0f, voice.y, voice.z);
            if (voice.behavior == island::CicadaBehavior::Call) calledUnderWalker = true;
        }
        Check(!calledUnderWalker, "the cicada started calling again with the walker still close");
        bool resumed = false;
        for (int step = 0; step < 600 && !resumed; ++step) {
            hushed.Update(0.05f, voice.x + 40.0f, voice.y, voice.z);
            resumed = voice.behavior == island::CicadaBehavior::Call;
        }
        Check(resumed, "the cicada did not start up again within thirty seconds of the walker leaving");
        const std::array<island::GrassWalker, 1> body{{{voice.x + 2.0f, voice.z, 0.6f, 1.0f}}};
        const std::uint32_t before = hushed.HushCount();
        hushed.SetWalkers(body);
        // Forty metres off the colony runs at the reduced rate: give it a few frames.
        for (int step = 0; step < 8; ++step) hushed.Update(0.05f, voice.x + 40.0f, voice.y, voice.z);
        Check(hushed.HushCount() == before + 1U && voice.behavior == island::CicadaBehavior::Rest,
              "a passing animal did not hush the calling cicada");
    }
    std::cout << "CicadaColonyTests: 12 deterministic trunk cicadas passed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << "CicadaColonyTests: " << error.what() << '\n';
    return 1;
}
