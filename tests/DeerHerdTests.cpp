#include "IslandSimulator/DeerHerd.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace
{
    void Check(const bool condition, const char* message)
    {
        if (!condition) throw std::runtime_error(message);
    }
}

int main()
{
    try {
        island::IslandTerrain terrain;
        const std::vector<island::CircularObstacle> obstacles;
        island::DeerHerd herd(terrain, obstacles), repeated(terrain, obstacles);
        Check(herd.Deer().size() == island::DeerHerd::Count, "wrong deer count");
        Check(herd.Deer().front().antlered, "lead deer should supply antlered silhouette");
        for (std::size_t index = 0; index < herd.Deer().size(); ++index) {
            const auto& deer = herd.Deer()[index];
            Check(herd.IsValidPosition(index, deer.x, deer.z), "initial deer habitat invalid");
            Check(deer.x == repeated.Deer()[index].x
                      && deer.z == repeated.Deer()[index].z
                      && deer.headingRadians == repeated.Deer()[index].headingRadians,
                  "deer construction is not deterministic");
        }
        const auto initial = herd.Deer().front();
        bool walked = false, grazed = false;
        for (int step = 0; step < 7200; ++step) {
            herd.Update(1.0f / 60.0f, 1000.0f, 1000.0f, 1000.0f);
            repeated.Update(1.0f / 60.0f, 1000.0f, 1000.0f, 1000.0f);
            for (std::size_t index = 0; index < herd.Deer().size(); ++index) {
                const auto& deer = herd.Deer()[index];
                const auto& copy = repeated.Deer()[index];
                Check(herd.IsValidPosition(index, deer.x, deer.z), "deer left safe home");
                Check(deer.x == copy.x && deer.y == copy.y && deer.z == copy.z
                          && deer.behavior == copy.behavior,
                      "deer simulation is not deterministic");
                walked |= deer.behavior == island::DeerBehavior::Walk;
                grazed |= deer.behavior == island::DeerBehavior::Graze;
            }
        }
        Check(walked && grazed, "deer did not exercise walking and grazing");
        float previousHeading = herd.Deer().front().headingRadians;
        float worstCloseTurn = 0.0f;
        for (int step = 0; step < 180; ++step) {
            const auto& deer = herd.Deer().front();
            herd.Update(1.0f / 60.0f, deer.x + .3f, deer.y + 1.0f, deer.z);
            const float heading = herd.Deer().front().headingRadians;
            worstCloseTurn = std::max(worstCloseTurn,
                std::abs(std::remainder(heading - previousHeading,
                    2.0f * std::numbers::pi_v<float>)));
            previousHeading = heading;
        }
        Check(worstCloseTurn <= 1.45f / 60.0f + .00001f,
              "nearby player caused an abrupt deer heading snap");
        Check(herd.Deer().front().behavior == island::DeerBehavior::Retreat
                  && std::hypot(herd.Deer().front().x - initial.x,
                                herd.Deer().front().z - initial.z) > .15f,
              "nearby player did not cause bounded deer retreat");
        // A walker 16 m off leaves a deer standing; the same distance on a roaring quad
        // (threat 2.5) sends it off.
        island::DeerHerd calm(terrain, obstacles), alarmed(terrain, obstacles);
        alarmed.SetObserverThreat(2.5f);
        for (int step = 0; step < 30; ++step) {
            const auto& a = calm.Deer()[2], b = alarmed.Deer()[2];
            calm.Update(1.0f / 60.0f, a.x + 16.0f, a.y + 1.0f, a.z);
            alarmed.Update(1.0f / 60.0f, b.x + 16.0f, b.y + 1.0f, b.z);
        }
        Check(calm.Deer()[2].behavior != island::DeerBehavior::Retreat
                  && alarmed.Deer()[2].behavior == island::DeerBehavior::Retreat,
              "a roaring quad 16 m off should send a deer off where a walker does not");
        // After dark every deer beds down within two minutes (no observer about).
        island::DeerHerd sleepy(terrain, obstacles);
        sleepy.SetNightness(1.0f);
        for (int step = 0; step < 60 * 120; ++step)
            sleepy.Update(1.0f / 60.0f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        bool allBedded = true;
        for (const auto& deer : sleepy.Deer()) allBedded = allBedded && deer.behavior == island::DeerBehavior::Rest && deer.bedAmount > 0.9f;
        Check(allBedded, "after dark every deer should be bedded down");
        std::cout << "Four deterministic inland deer graze, walk and retreat safely\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return 1;
    }
}
