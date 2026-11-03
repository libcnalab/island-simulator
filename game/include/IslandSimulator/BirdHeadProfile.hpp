#pragma once

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>

namespace island::bird_head
{
    struct Section { float z, halfWidth, halfHeight, lift, toHead; };
    inline constexpr std::array<Section,7> Sections{{
        {.300f,.090f,.088f,.012f,0.0f},{.340f,.112f,.112f,.030f,.55f},
        {.385f,.124f,.130f,.046f,.95f},{.430f,.122f,.128f,.054f,1.0f},
        {.475f,.106f,.112f,.054f,1.0f},{.512f,.078f,.084f,.050f,1.0f},
        {.538f,.040f,.046f,.046f,1.0f}}};
    inline constexpr int Sides=14;
    inline constexpr int EyeSides=10;
    inline constexpr float EyeY=.080f,EyeZ=.455f;

    // The same local vertices serve the visible loft and its lateral attachments. Sampling
    // the actual triangles avoids estimating an ellipse which does not match the faceted skin.
    class Profile
    {
        using Vector3=Microsoft::Xna::Framework::Vector3;
        std::array<Vector3,Sections.size()*(Sides+1)> points_{};
        float heightScale_;
    public:
        explicit Profile(float heightScale):heightScale_(heightScale)
        {
            if(!std::isfinite(heightScale)||heightScale<=0)
                throw std::invalid_argument("Bird head scale must be finite and positive");
            for(std::size_t ring=0;ring<Sections.size();++ring)
                for(int side=0;side<=Sides;++side) {
                    const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                    const auto& s=Sections[ring];
                    points_[ring*(Sides+1)+side]={std::cos(angle)*s.halfWidth*heightScale,
                        s.lift+std::sin(angle)*s.halfHeight*heightScale,s.z};
                }
        }
        [[nodiscard]] const Vector3& Point(std::size_t ring,int side) const
        {
            return points_.at(ring*(Sides+1)+side);
        }
        [[nodiscard]] std::optional<float> LateralX(float y,float z) const noexcept
        {
            if(!std::isfinite(y)||!std::isfinite(z))return std::nullopt;
            std::optional<float> result;
            const auto triangle=[&](const Vector3& a,const Vector3& b,const Vector3& c) {
                const float by=b.Y-a.Y,bz=b.Z-a.Z,cy=c.Y-a.Y,cz=c.Z-a.Z;
                const float determinant=by*cz-bz*cy;
                if(std::abs(determinant)<1e-10f)return;
                const float u=((y-a.Y)*cz-(z-a.Z)*cy)/determinant;
                const float v=(by*(z-a.Z)-bz*(y-a.Y))/determinant;
                if(u<-.00001f||v<-.00001f||u+v>1.00001f)return;
                const float x=a.X+(b.X-a.X)*u+(c.X-a.X)*v;
                if(x>=0&&(!result||x>*result))result=x;
            };
            for(std::size_t ring=0;ring+1<Sections.size();++ring) {
                if(z<Sections[ring].z||z>Sections[ring+1].z)continue;
                for(int side=0;side<Sides;++side) {
                    const auto& a=Point(ring,side);const auto& b=Point(ring,side+1);
                    const auto& c=Point(ring+1,side+1);const auto& d=Point(ring+1,side);
                    triangle(a,b,c);triangle(a,c,d);
                }
            }
            return result;
        }
        [[nodiscard]] std::array<Vector3,EyeSides+1> EyeRing(float radius,float outset) const
        {
            std::array<Vector3,EyeSides+1> ring{};
            for(int i=0;i<=EyeSides;++i) {
                const float angle=6.28318530718f*static_cast<float>(i-1)/EyeSides;
                const float r=i==0?0:radius;
                const float y=(EyeY+std::sin(angle)*r)*heightScale_;
                const float z=EyeZ+std::cos(angle)*r*heightScale_*1.15f;
                const auto x=LateralX(y,z);
                if(!x)throw std::runtime_error("Bird eye lies outside its head profile");
                ring[static_cast<std::size_t>(i)]={*x+outset,y,z};
            }
            return ring;
        }
    };
}
