#pragma once
#include "IslandSimulator/StructureMeshBuilder.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <numbers>
#include <vector>

namespace island {
// Shaped pieces for the larger sculpted landmarks (a statue, a wreck, a lighthouse) on top of the
// structures' flat-lit builder: each face is wound to face away from `inside` (the builder's light
// and the hour's relight take the normal from the winding), and, given the ground, faces are
// stained toward `stainColour` where they meet it.
struct SculptMeshBuilder {
    using Vector3=Microsoft::Xna::Framework::Vector3;
    StructureMeshBuilder builder;
    Vector3 inside{};                                // a point inside the piece being built
    std::function<float(float,float)> ground;        // optional: the ground's height under (x, z)
    Vector3 stainColour{196,178,140};
    // optional: the face's colour from its pigment, its outward normal and its centre (weathering)
    std::function<Vector3(const Vector3&,const Vector3&,const Vector3&)> tint;
    float stainHeight=0.14f,stainAmount=0.75f;
    // when set, a quad's two triangles take their tint and stain from the quad's centre, so its
    // diagonal never shows as a step in the weathering
    bool tintPerQuad=false;
    // when set, the tint and the stain are taken at each corner instead, so the weathering runs
    // smoothly across the faces (the light stays each face's own)
    bool tintPerVertex=false;

    void Face(const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& pigment) {
        Vector3 colour=pigment;
        const Vector3 m=quadCentre_?*quadCentre_:(a+b+c)/3.0f;
        Vector3 normal=Vector3::Cross(b-a,c-a);
        const bool outward=Vector3::Dot(normal,m-inside)>=0.0f;
        if(!outward) normal=-normal;
        if(tintPerVertex) {
            if(normal.LengthSquared()>1e-12f) normal.Normalize();
            const auto at=[&](const Vector3& p) {
                Vector3 c=tint&&normal.LengthSquared()>0.5f?tint(pigment,normal,p):pigment;
                if(ground) {
                    const float stain=1.0f-std::clamp((p.Y-ground(p.X,p.Z)+0.02f)/stainHeight,0.0f,1.0f);
                    c=Vector3::Lerp(c,stainColour,stain*stainAmount);
                }
                return c;
            };
            if(outward) builder.Tri(a,b,c,at(a),at(b),at(c));
            else builder.Tri(a,c,b,at(a),at(c),at(b));
            return;
        }
        // the tint sees the pigment itself (a marker pigment stays recognisable), the stain after it
        if(tint&&normal.LengthSquared()>1e-12f) {normal.Normalize();colour=tint(colour,normal,m);}
        if(ground) {
            const float stain=1.0f-std::clamp((m.Y-ground(m.X,m.Z)+0.02f)/stainHeight,0.0f,1.0f);
            colour=Vector3::Lerp(colour,stainColour,stain*stainAmount);
        }
        if(outward) builder.Tri(a,b,c,colour);
        else builder.Tri(a,c,b,colour);
    }
    void Quad(const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& d,const Vector3& pigment) {
        if(!tintPerQuad) {Face(a,b,c,pigment);Face(a,c,d,pigment);return;}
        const Vector3 centre=(a+b+c+d)/4.0f;
        quadCentre_=&centre;
        Face(a,b,c,pigment);Face(a,c,d,pigment);
        quadCentre_=nullptr;
    }
    // A box: centre, its three axes, half extents; `top` for the face along +up.
    void Box(const Vector3& centre,const Vector3& right,const Vector3& up,const Vector3& forward,
             const float halfRight,const float halfUp,const float halfForward,const Vector3& pigment,
             const Vector3& top) {
        inside=centre;
        const Vector3 r=right*halfRight,u=up*halfUp,f=forward*halfForward;
        const Vector3 p[8]={centre-r-u-f,centre+r-u-f,centre+r+u-f,centre-r+u-f,
                            centre-r-u+f,centre+r-u+f,centre+r+u+f,centre-r+u+f};
        Quad(p[3],p[2],p[6],p[7],top);Quad(p[0],p[4],p[5],p[1],pigment);
        Quad(p[0],p[1],p[2],p[3],pigment);Quad(p[5],p[4],p[7],p[6],pigment);
        Quad(p[4],p[0],p[3],p[7],pigment);Quad(p[1],p[5],p[6],p[2],pigment);
    }
    // A tapering blade from its root at `base` out along `up`.
    void Blade(const Vector3& base,const Vector3& right,const Vector3& up,const Vector3& forward,
               const float halfThick,const float length,const float halfBase,const float halfTop,const Vector3& pigment) {
        inside=base+up*(length*0.5f);
        const Vector3 tip=base+up*length;
        const Vector3 r0=right*halfThick,r1=right*(halfThick*0.6f),f0=forward*halfBase,f1=forward*halfTop;
        const Vector3 p[8]={base-r0-f0,base+r0-f0,base+r0+f0,base-r0+f0,tip-r1-f1,tip+r1-f1,tip+r1+f1,tip-r1+f1};
        Quad(p[0],p[1],p[5],p[4],pigment);Quad(p[1],p[2],p[6],p[5],pigment);
        Quad(p[2],p[3],p[7],p[6],pigment);Quad(p[3],p[0],p[4],p[7],pigment);
        Quad(p[4],p[5],p[6],p[7],pigment*1.02f);Quad(p[0],p[3],p[2],p[1],pigment);
    }
    // A tube through the points with their radii, flattened by `squash` toward the vertical,
    // closed at both ends.
    void Tube(const std::vector<Vector3>& points,const std::vector<float>& radii,const int sides,
              const Vector3& pigment,const float squash=1.0f) {
        constexpr float Pi=std::numbers::pi_v<float>;
        const std::size_t n=points.size();
        if(n<2U) return;
        std::vector<std::vector<Vector3>> rings(n,std::vector<Vector3>(static_cast<std::size_t>(sides)));
        for(std::size_t i=0;i<n;++i) {
            Vector3 tangent=points[std::min(i+1U,n-1U)]-points[i==0U?0U:i-1U];
            if(tangent.LengthSquared()<1e-10f) tangent=Vector3::UnitX;
            tangent.Normalize();
            Vector3 side=Vector3::Cross(tangent,Vector3::Up);
            if(side.LengthSquared()<1e-6f) side=Vector3::UnitZ;
            side.Normalize();
            const Vector3 up=Vector3::Cross(side,tangent);
            for(int k=0;k<sides;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides);
                rings[i][static_cast<std::size_t>(k)]=points[i]+side*(std::cos(a)*radii[i])+up*(std::sin(a)*radii[i]*squash);
            }
        }
        for(std::size_t i=0;i+1<n;++i) {
            inside=(points[i]+points[i+1])*0.5f;
            for(int k=0;k<sides;++k) {
                const auto k0=static_cast<std::size_t>(k),k1=static_cast<std::size_t>((k+1)%sides);
                Quad(rings[i][k0],rings[i+1][k0],rings[i+1][k1],rings[i][k1],pigment);
            }
        }
        for(int k=0;k<sides;++k) {
            const auto k0=static_cast<std::size_t>(k),k1=static_cast<std::size_t>((k+1)%sides);
            inside=points[1];Face(points.front(),rings.front()[k1],rings.front()[k0],pigment);
            inside=points[n-2];Face(points.back(),rings.back()[k0],rings.back()[k1],pigment);
        }
    }
    // A loft through rings of points (each ring closed, all the same count) round an axis
    // through `centres`, each quad coloured by `pigment(ring, segment)`; the ends capped.
    template<class Pigment>
    void Loft(const std::vector<std::vector<Vector3>>& rings,const std::vector<Vector3>& centres,
              const Pigment& pigment,const bool capStart=true,const bool capEnd=true) {
        const std::size_t n=rings.size();
        if(n<2U) return;
        const std::size_t count=rings.front().size();
        for(std::size_t i=0;i+1<n;++i) {
            inside=(centres[i]+centres[i+1])*0.5f;
            for(std::size_t k=0;k<count;++k) {
                const std::size_t k1=(k+1)%count;
                Quad(rings[i][k],rings[i+1][k],rings[i+1][k1],rings[i][k1],pigment(i,k));
            }
        }
        for(std::size_t k=0;k<count;++k) {
            const std::size_t k1=(k+1)%count;
            if(capStart) {inside=centres[1];Face(centres.front(),rings.front()[k1],rings.front()[k],pigment(0U,k));}
            if(capEnd) {inside=centres[n-2];Face(centres.back(),rings.back()[k],rings.back()[k1],pigment(n-2U,k));}
        }
    }
private:
    const Vector3* quadCentre_=nullptr;
};
}
