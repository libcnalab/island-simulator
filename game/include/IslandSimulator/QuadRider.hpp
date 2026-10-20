#pragma once

#include "IslandSimulator/QuadBike.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include <memory>
#include <string>

namespace Microsoft::Xna::Framework { class Game; }

namespace island
{
    /**
     * The quad bike's rider: a CNA avatar (GamerServices AvatarRenderer, a fixed description so
     * every run shows the same rider) posed astride the seat, the hands on the grips and the feet
     * in the footwells, lit by the hour's sun. Where the CNA avatar is not built (Web, Android) or
     * still loading, Available() is false and the quad draws its simple stand-in rider.
     */
    class QuadRider final
    {
    public:
        explicit QuadRider(Microsoft::Xna::Framework::Game& game);
        ~QuadRider();
        [[nodiscard]] bool Available() const noexcept;
        [[nodiscard]] const std::string& Status() const noexcept { return status_; }
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const QuadBike& bike);

    private:
        struct Avatar;
        std::unique_ptr<Avatar> avatar_;
        std::string status_;
    };
}
