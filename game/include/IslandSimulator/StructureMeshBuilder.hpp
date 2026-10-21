#pragma once
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>

namespace island {
// Flat-lit vertex-coloured primitives for the small built structures (jetty, rope bridge):
// every face takes the fixed sun once at build time, so the renderer is one opaque draw (the
// structure shadow receiver relights each face by the hour's sun with the same 0.46 + 0.54 rule).
struct StructureMeshBuilder {
    using Vector3=Microsoft::Xna::Framework::Vector3;
    using Color=Microsoft::Xna::Framework::Color;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> mesh;
    Vector3 sun{environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ};
    StructureMeshBuilder() { sun.Normalize(); }
    void Tri(const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& pigment) {
        Vector3 normal=Vector3::Cross(b-a,c-a);
        if(normal.LengthSquared()<=1e-10f) return;
        normal.Normalize();
        const float light=0.46f+0.54f*std::max(0.0f,Vector3::Dot(normal,sun));
        const Color color(static_cast<int>(std::min(255.0f,pigment.X*light)),
            static_cast<int>(std::min(255.0f,pigment.Y*light)),
            static_cast<int>(std::min(255.0f,pigment.Z*light)),255);
        mesh.emplace_back(a,color);mesh.emplace_back(b,color);mesh.emplace_back(c,color);
    }
    void Quad(const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& d,const Vector3& pigment) {
        Tri(a,b,c,pigment);Tri(a,c,d,pigment);
    }
    // Box in an orthonormal frame: centre, right/up/forward axes, half extents.
    void Box(const Vector3& centre,const Vector3& right,const Vector3& up,const Vector3& forward,
             float halfRight,float halfUp,float halfForward,const Vector3& pigment,
             const Vector3& sidePigment) {
        const Vector3 r=right*halfRight,u=up*halfUp,f=forward*halfForward;
        const Vector3 p[8]={centre-r-u-f,centre+r-u-f,centre+r+u-f,centre-r+u-f,
            centre-r-u+f,centre+r-u+f,centre+r+u+f,centre-r+u+f};
        Quad(p[3],p[2],p[6],p[7],pigment);        // top
        Quad(p[0],p[4],p[5],p[1],sidePigment);    // bottom
        Quad(p[0],p[1],p[2],p[3],sidePigment);    // back
        Quad(p[5],p[4],p[7],p[6],sidePigment);    // front
        Quad(p[4],p[0],p[3],p[7],sidePigment);    // left
        Quad(p[1],p[5],p[6],p[2],sidePigment);    // right
    }
    void Cylinder(const Vector3& bottom,const Vector3& top,float radius,int sides,const Vector3& pigment,
                  bool capTop=true) {
        constexpr float Pi=std::numbers::pi_v<float>;
        Vector3 axis=top-bottom;
        if(axis.LengthSquared()<=1e-8f) return;
        axis.Normalize();
        const Vector3 reference=std::abs(axis.Y)<0.9f?Vector3::Up:Vector3::Right;
        Vector3 u=Vector3::Cross(axis,reference);u.Normalize();
        const Vector3 v=Vector3::Cross(axis,u);
        for(int side=0;side<sides;++side) {
            const float a0=2*Pi*static_cast<float>(side)/static_cast<float>(sides);
            const float a1=2*Pi*static_cast<float>(side+1)/static_cast<float>(sides);
            const Vector3 r0=(u*std::cos(a0)+v*std::sin(a0))*radius;
            const Vector3 r1=(u*std::cos(a1)+v*std::sin(a1))*radius;
            Quad(bottom+r0,bottom+r1,top+r1,top+r0,pigment);
            if(capTop) Tri(top,top+r0,top+r1,pigment);
        }
    }
};
}
