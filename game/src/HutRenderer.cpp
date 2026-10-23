#include "IslandSimulator/HutRenderer.hpp"
#include "IslandSimulator/LandmarkShapes.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "IslandSimulator/HutThatch.hpp"
#include "IslandSimulator/HutMaterial.hpp"
#include "IslandSimulator/WindGust.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/OpaqueImageLoader.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/ContainmentType.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColorTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <limits>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
namespace island {
HutRenderer::HutRenderer(GraphicsDevice& device,const HutPlacement& hut)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),centre_(hut.x,hut.floorY,hut.z)
{
    std::vector<VertexPositionColorTexture> mesh;
    Vector3 grainOrigin,grainAcross,grainAlong;
    int materialTag=64;
    int hangTag=0;   // 40 for the hung fish, 48 for the net: the material shader swings these
    const float c=std::cos(hut.yawRadians),s=std::sin(hut.yawRadians);
    const auto world=[&](Vector3 p) { return Vector3(hut.x+c*p.X+s*p.Z,hut.floorY+p.Y,hut.z-s*p.X+c*p.Z); };
    const auto quad=[&](Vector3 a,Vector3 b,Vector3 d,Vector3 e,Vector3 pigment) {
        const std::array<Vector3,6> local{a,b,d,a,d,e};
        a=world(a); b=world(b); d=world(d); e=world(e);
        Vector3 normal=Vector3::Cross(b-a,d-a); normal.Normalize();
        const float light=0.48f+0.52f*std::max(0.0f,Vector3::Dot(normal,
            Vector3(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ)));
        const Color color(static_cast<int>(pigment.X*light),static_cast<int>(pigment.Y*light),static_cast<int>(pigment.Z*light),hangTag?hangTag:materialTag);
        for(const auto p:local) {
            const float phase=materialTag==128?0:Vector3::Dot(grainOrigin,Vector3(0.37f,0.19f,0.53f));
            Vector2 uv(Vector3::Dot(p-grainOrigin,grainAcross)+phase,
                Vector3::Dot(p-grainOrigin,grainAlong));
            if(materialTag==224) {
                // Interior atlas windows: no generated seamless-edge assumption.
                const float offset=phase-std::floor(phase);
                uv=Vector2(0.15f+0.7f*offset+Vector3::Dot(p-grainOrigin,grainAcross)*0.8f,
                    0.12f+Vector3::Dot(p-grainOrigin,grainAlong)*0.9f);
            }
            mesh.emplace_back(world(p),color,uv);
        }
    };
    const auto beam=[&](Vector3 a,Vector3 b,float width,float depth,Vector3 pigment) {
        Vector3 along=b-a; along.Normalize();
        Vector3 across=Vector3::Cross(along,std::abs(along.Y)>0.9f?Vector3(0,0,1):Vector3(0,1,0)); across.Normalize();
        Vector3 up=Vector3::Cross(across,along); up.Normalize();
        grainOrigin=a; grainAcross=across; grainAlong=up; materialTag=128;
        const auto r=across*(width/2),t=up*(depth/2);
        const std::array<Vector3,8> p{a-r-t,a+r-t,a+r+t,a-r+t,b-r-t,b+r-t,b+r+t,b-r+t};
        quad(p[0],p[3],p[2],p[1],pigment); quad(p[4],p[5],p[6],p[7],pigment);
        grainAcross=across+up; grainAlong=along; materialTag=64;
        quad(p[0],p[1],p[5],p[4],pigment); quad(p[1],p[2],p[6],p[5],pigment);
        quad(p[2],p[3],p[7],p[6],pigment); quad(p[3],p[0],p[4],p[7],pigment);
    };
    const Vector3 timber(131,100,65),dark(89,67,45);
    // Level deck with individually separated weathered boards.
    for(int i=0;i<24;++i) {
        const float x=-2.1f+(static_cast<float>(i)+0.5f)*4.2f/24;
        beam({x,-0.07f,-1.7f},{x,-0.07f,2.3f},0.166f,0.14f,
            timber+Vector3(static_cast<float>(i%5)*3));
    }
    std::size_t foot=0;
    for(float x:{-2.1f,2.1f}) for(float z:{-1.7f,0.0f,1.7f}) {
        const float bottom=hut.footings[foot++].groundY-hut.floorY-0.08f;
        beam({x,bottom,z},{x,2.25f,z},0.16f,0.16f,dark);
        if(z!=0) beam({x,1.45f,z},{x-std::copysign(0.6f,x),2.25f,z},0.095f,0.095f,timber);
    }
    for(float x:{-2.1f,2.1f}) beam({x,2.2f,-1.8f},{x,2.2f,1.8f},0.16f,0.16f,dark);
    for(float z:{-1.7f,1.7f}) beam({-2.18f,2.2f,z},{2.18f,2.2f,z},0.16f,0.16f,dark);
    // Edge-joined rear wall and front facade. Unequal normalized widths preserve
    // complete coverage while avoiding the corrugated fence rhythm of identical
    // boards; tiny depth offsets catch light without reopening see-through stripes.
    constexpr std::array<float,20> EndBoardWeight{
        .92f,1.08f,.97f,1.13f,.89f,1.04f,.95f,1.10f,.91f,1.01f,
        1.07f,.90f,1.12f,.96f,1.03f,.88f,1.09f,.94f,1.05f,.93f};
    float endWeightTotal=0.0f;
    for(const float weight:EndBoardWeight) endWeightTotal+=weight;
    float endBoardEdge=-2.0f;
    for(int i=0;i<20;++i) {
        const float boardWidth=4.0f*EndBoardWeight[static_cast<std::size_t>(i)]/endWeightTotal;
        const float x=endBoardEdge+boardWidth*.5f;
        endBoardEdge+=boardWidth;
        const float lean=(static_cast<float>((i*7)%5)-2.0f)*.004f;
        const float rearDepth=-1.7f+(static_cast<float>((i*3)%4)-1.5f)*.006f;
        beam({x,0.08f,rearDepth},{x+lean,1.8f,rearDepth},boardWidth*1.01f,0.055f,
             timber+Vector3(static_cast<float>(i%4)*5));
        const float ax=std::abs(x);
        if(ax<0.53f) continue;
        const float frontDepth=1.7f+(static_cast<float>((i*5)%4)-1.5f)*.006f;
        if(ax>0.82f&&ax<1.72f) {
            beam({x,0.08f,frontDepth},{x+lean*.32f,0.55f,frontDepth},boardWidth*1.01f,0.055f,timber);
            beam({x+lean*.77f,1.35f,frontDepth},{x+lean,1.8f,frontDepth},boardWidth*1.01f,0.055f,timber);
        } else beam({x,0.08f,frontDepth},{x+lean,1.8f,frontDepth},boardWidth*1.01f,0.055f,timber);
    }
    constexpr std::array<float,16> SideBoardWeight{
        .94f,1.08f,.91f,1.04f,.97f,1.11f,.89f,1.02f,
        1.07f,.93f,1.05f,.90f,1.09f,.96f,1.03f,.92f};
    float sideWeightTotal=0.0f;
    for(const float weight:SideBoardWeight) sideWeightTotal+=weight;
    for(float x:{-2.1f,2.1f}) {
      float sideBoardEdge=-1.6f;
      for(int i=0;i<16;++i) {
        const float boardWidth=3.2f*SideBoardWeight[static_cast<std::size_t>(i)]/sideWeightTotal;
        const float z=sideBoardEdge+boardWidth*.5f;
        sideBoardEdge+=boardWidth;
        const float outward=std::copysign((static_cast<float>((i*5)%4)-1.5f)*.005f,x);
        const float lean=(static_cast<float>((i*3)%5)-2.0f)*.004f;
        // Vertical beam cross-sections use local X for width and local Z for depth.
        // On an X-constant side wall those roles must be swapped: X is wall
        // thickness and Z is the board coverage direction.
        beam({x+outward,0.08f,z},{x+outward,1.8f,z+lean},0.055f,boardWidth*1.01f,
             timber+Vector3(static_cast<float>(i%3)*5));
      }
    }
    // Muted opaque glazing gives the facade readable windows without a sorted
    // transparency pass. Timber mullions sit slightly forward of the glass.
    grainAcross={1,0,0};grainAlong={0,1,0};materialTag=176;
    for(float x:{-1.26f,1.26f}) {
        grainOrigin={x-.43f,.55f,1.704f};
        quad({x-.43f,.55f,1.704f},{x+.43f,.55f,1.704f},
             {x+.43f,1.35f,1.704f},{x-.43f,1.35f,1.704f},{52,73,73});
        beam({x-.47f,.50f,1.73f},{x-.47f,1.40f,1.73f},.09f,.09f,dark);
        beam({x+.47f,.50f,1.73f},{x+.47f,1.40f,1.73f},.09f,.09f,dark);
        beam({x-.51f,.50f,1.73f},{x+.51f,.50f,1.73f},.09f,.09f,dark);
        beam({x-.51f,1.40f,1.73f},{x+.51f,1.40f,1.73f},.09f,.09f,dark);
        beam({x,.53f,1.79f},{x,1.37f,1.79f},.055f,.055f,dark);
        beam({x-.44f,.95f,1.79f},{x+.44f,.95f,1.79f},.055f,.055f,dark);
    }
    // A pair of outward-open shutters adds real depth and a practical tropical
    // ventilation detail. Each remains visibly attached at the outer window jamb
    // and uses three unequal vertical boards plus two simple back braces.
    for(const float side:{-1.0f,1.0f}) {
        const float hingeX=side*1.73f;
        for(int board=0;board<3;++board) {
            const float z=1.80f+(static_cast<float>(board)+.5f)*.39f/3.0f;
            const float bottom=.54f+.008f*static_cast<float>((board+1)%3);
            const float top=1.36f-.006f*static_cast<float>((board*2)%3);
            beam({hingeX,bottom,z},{hingeX+side*.006f*static_cast<float>(board-1),top,z},
                 .055f,.138f,timber+Vector3(static_cast<float>(board)*5.0f));
        }
        for(const float y:{.69f,1.22f})
            beam({hingeX,y,1.82f},{hingeX,y,2.16f},.052f,.052f,dark);
    }
    // A closed plank door suits the decorative, uninhabited building contract.
    for(int i=0;i<5;++i) {
        const float x=-.40f+static_cast<float>(i)*.20f;
        beam({x,.08f,1.72f},{x,1.68f,1.72f},.17f,.065f,dark+Vector3(15+3*i,11+2*i,6+i));
    }
    beam({-.54f,.04f,1.76f},{-.54f,1.78f,1.76f},.11f,.11f,dark);
    beam({ .54f,.04f,1.76f},{ .54f,1.78f,1.76f},.11f,.11f,dark);
    beam({-.59f,1.78f,1.76f},{.59f,1.78f,1.76f},.11f,.11f,dark);
    beam({-.39f,.30f,1.80f},{.39f,1.48f,1.80f},.065f,.055f,timber);
    beam({.31f,.82f,1.81f},{.31f,.82f,1.94f},.045f,.045f,Vector3(65,52,38));
    // Covered front veranda.  Tall outer posts and a shadowed fascia make the
    // platform read as part of the raised structure rather than three isolated
    // steps stuck onto the facade.
    beam({-2.10f,-.14f,2.25f},{2.10f,-.14f,2.25f},.12f,.16f,dark);
    for(float x:{-2.04f,2.04f})
        beam({x,-.10f,2.24f},{x,2.16f,2.24f},.14f,.14f,dark);
    for(const auto& span:{std::pair{-2.04f,-.72f},std::pair{.72f,2.04f}})
        beam({span.first,.86f,2.24f},{span.second,.86f,2.24f},.09f,.09f,dark);
    for(const auto& span:{std::pair{-2.00f,-.76f},std::pair{.76f,2.00f}})
        beam({span.first,.31f,2.24f},{span.second,.31f,2.24f},.065f,.065f,timber);
    for(float x:{-1.86f,-1.46f,-1.06f,1.06f,1.46f,1.86f})
        beam({x,.02f,2.24f},{x,.90f,2.24f},.07f,.07f,timber);
    for(float x:{-2.04f,2.04f}) {
        beam({x,.86f,1.76f},{x,.86f,2.24f},.09f,.09f,dark);
        beam({x,.02f,2.00f},{x,.90f,2.00f},.07f,.07f,timber);
    }
    beam({-.76f,-.16f,2.28f},{.76f,-.16f,2.28f},.34f,.14f,timber);
    beam({-.76f,-.31f,2.56f},{.76f,-.31f,2.56f},.34f,.14f,timber);
    beam({-.76f,-.46f,2.84f},{.76f,-.46f,2.84f},.34f,.14f,timber);
    // Close both gables with individually varied edge-joined boards. The former
    // front-only loop left the entire rear triangle visibly open from uphill.
    for(float faceZ:{-1.70f,1.70f}) for(int i=0;i<16;++i) {
        const float x=-1.76f+static_cast<float>(i)*3.52f/15;
        const float top=3.45f-.60f*std::abs(x);
        beam({x,2.23f,faceZ},{x,top,faceZ},3.52f/15.0f,.055f,
             timber+Vector3(static_cast<float>(i%4)*4));
    }
    // The front gable exposes the load-bearing frame seen in the reference house.
    beam({-2.08f,2.20f,1.74f},{0,3.52f,1.74f},.10f,.10f,timber);
    beam({ 2.08f,2.20f,1.74f},{0,3.52f,1.74f},.10f,.10f,timber);
    beam({-2.06f,2.20f,1.74f},{2.06f,2.20f,1.74f},.10f,.10f,dark);
    beam({0,2.20f,1.74f},{0,3.48f,1.74f},.09f,.09f,timber);
    // A small recessed attic light is present in the supplied reference. Muted glass
    // and a projecting timber frame break the broad gable without implying an interior.
    grainOrigin={-.28f,2.68f,1.79f}; grainAcross={1,0,0}; grainAlong={0,1,0}; materialTag=176;
    quad({-.28f,2.68f,1.79f},{.28f,2.68f,1.79f},{.28f,3.08f,1.79f},{-.28f,3.08f,1.79f},
         {48,67,67});
    const Vector3 paleTimber(176,139,91);
    beam({-.33f,2.63f,1.82f},{-.33f,3.13f,1.82f},.075f,.075f,paleTimber);
    beam({ .33f,2.63f,1.82f},{ .33f,3.13f,1.82f},.075f,.075f,paleTimber);
    beam({-.36f,2.63f,1.82f},{ .36f,2.63f,1.82f},.075f,.075f,paleTimber);
    beam({-.36f,3.13f,1.82f},{ .36f,3.13f,1.82f},.075f,.075f,paleTimber);
    beam({0,2.68f,1.825f},{0,3.08f,1.825f},.045f,.045f,paleTimber);
    beam({-.28f,2.88f,1.825f},{.28f,2.88f,1.825f},.045f,.045f,paleTimber);
    // A hurricane lantern hung from the front header beside the door on a short iron bracket:
    // a cap, a pale glass globe in a wire guard, a fuel base. Lit at dusk (DrawNightLights).
    {
        const Vector3 iron(46,42,38),glass(198,176,120);
        beam({.80f,2.15f,1.76f},{.80f,2.15f,1.98f},.03f,.03f,iron);
        beam({.80f,2.15f,1.98f},{.80f,2.01f,1.98f},.012f,.012f,iron);
        beam({.80f,1.99f,1.98f},{.80f,1.95f,1.98f},.13f,.13f,iron);
        beam({.80f,1.94f,1.98f},{.80f,1.76f,1.98f},.10f,.10f,glass);
        for(const float dx:{-.055f,.055f}) for(const float dz:{-.055f,.055f})
            beam({.80f+dx,1.95f,1.98f+dz},{.80f+dx,1.75f,1.98f+dz},.012f,.012f,iron);
        beam({.80f,1.76f,1.98f},{.80f,1.70f,1.98f},.14f,.14f,iron);
        lantern_=world({.80f,1.85f,1.98f});
        {
            // The lantern's light on the front wall and gable (just in front of the door's
            // face, behind the posts) and on the veranda floor.
            const Vector3 outward=Vector3::Normalize(world({0,0,1})-world({0,0,0}));
            auto& wall=lanternPlanes_[0];
            wall.normal=outward;
            constexpr float face=1.77f;
            const std::array<Vector3,5> outline{{{-2.1f,.10f,face},{2.1f,.10f,face},{2.1f,2.25f,face},{0.0f,3.35f,face},{-2.1f,2.25f,face}}};
            const Vector3 middle=world({0.0f,1.4f,face});
            for(std::size_t i=0;i<outline.size();++i) {
                wall.triangles.push_back(middle);
                wall.triangles.push_back(world(outline[i]));
                wall.triangles.push_back(world(outline[(i+1)%outline.size()]));
            }
            auto& floor=lanternPlanes_[1];
            floor.normal=Vector3::Up;
            const Vector3 a=world({-2.1f,.012f,1.78f}),b=world({2.1f,.012f,1.78f}),c=world({2.1f,.012f,2.30f}),d=world({-2.1f,.012f,2.30f});
            floor.triangles={a,b,c,a,c,d};
        }
        for(const float x:{-1.26f,1.26f})
            for(const auto& corner:{Vector3(x-.43f,.55f,1.709f),Vector3(x+.43f,.55f,1.709f),
                                    Vector3(x+.43f,1.35f,1.709f),Vector3(x-.43f,1.35f,1.709f)})
                windowCorners_.push_back(world(corner));
    }
    // Four short knee braces tie the veranda posts into the front header instead of
    // leaving two visually unsupported vertical poles below the roof.
    beam({-2.04f,1.72f,2.24f},{-1.48f,2.18f,1.78f},.075f,.075f,timber);
    beam({ 2.04f,1.72f,2.24f},{ 1.48f,2.18f,1.78f},.075f,.075f,timber);
    beam({-2.04f,1.72f,2.24f},{-2.04f,2.18f,1.78f},.075f,.075f,dark);
    beam({ 2.04f,1.72f,2.24f},{ 2.04f,2.18f,1.78f},.075f,.075f,dark);
    // Four sparse fibre ties make the most exposed veranda joints read as
    // assembled connections rather than beams terminating inside one another.
    const Vector3 lashing(117,91,57);
    beam({-2.12f,1.77f,2.22f},{-1.94f,1.77f,2.22f},.045f,.045f,lashing);
    beam({ 1.94f,1.77f,2.22f},{ 2.12f,1.77f,2.22f},.045f,.045f,lashing);
    beam({-2.12f,2.12f,1.80f},{-1.94f,2.12f,1.80f},.045f,.045f,lashing);
    beam({ 1.94f,2.12f,1.80f},{ 2.12f,2.12f,1.80f},.045f,.045f,lashing);
    // Exposed rafters and overlapping narrow thatch bundles, not a single roof pyramid.
    beam({0,3.55f,-2.3f},{0,3.55f,2.3f},0.16f,0.16f,dark);
    for(float side:{-1.0f,1.0f}) {
        for(int i=0;i<7;++i) {
            const float z=-2.1f+static_cast<float>(i)*0.7f;
            beam({0,3.5f,z},{side*2.65f,2.12f,z},0.10f,0.10f,timber);
        }
        for(int row=0;row<7;++row) for(int strip=0;strip<48;++strip) {
            const auto blade=BuildHutThatchBlade(static_cast<int>(side),row,strip);
            const auto origin=blade.sections[0][1];
            grainOrigin=Vector3(origin.x,origin.y,origin.z);
            grainAcross=Vector3(0,0,1); grainAlong=Vector3(side*2.65f,-1.4f,0);
            grainAlong.Normalize(); materialTag=224;
            const Vector3 pigment=Vector3(177,163,131)+Vector3(blade.pigment*16,blade.pigment*14,blade.pigment*9);
            const auto point=[&](int station,int edge) {
                const auto p=blade.sections[static_cast<std::size_t>(station)][static_cast<std::size_t>(edge)];
                return Vector3(p.x,p.y,p.z);
            };
            for(int station=0;station<3;++station) for(int edge=0;edge<2;++edge) {
                const auto a=point(station,edge),b=point(station,edge+1),
                    d=point(station+1,edge+1),e=point(station+1,edge);
                if(side>0) quad(a,b,d,e,pigment);
                else quad(b,a,e,d,pigment);
            }
        }
    }
    // Overlapping folded cap bundles cover the artificial roof seam and explain
    // how the two thatch slopes are secured over the ridge pole. They remain one
    // surface layer in the existing batch, with restrained deterministic variation.
    for(int bundle=0;bundle<24;++bundle) {
        const float z=-2.22f+(static_cast<float>(bundle)+.5f)*4.44f/24.0f;
        const float halfSpan=.108f+.010f*static_cast<float>((bundle*5)%4);
        const float crown=3.635f+.008f*static_cast<float>((bundle*7)%3);
        const float shoulder=3.385f-.008f*static_cast<float>((bundle*3)%4);
        grainOrigin={0.0f,crown,z-halfSpan};
        grainAcross={0,0,1}; grainAlong={1,0,0}; materialTag=224;
        const Vector3 pigment=Vector3(172,157,124)
            +Vector3(static_cast<float>(bundle%5)*2.2f);
        quad({-.48f,shoulder,z-halfSpan},{0,crown,z-halfSpan},
             {0,crown,z+halfSpan},{-.48f,shoulder,z+halfSpan},pigment);
        quad({0,crown,z-halfSpan},{.48f,shoulder,z-halfSpan},
             {.48f,shoulder,z+halfSpan},{0,crown,z+halfSpan},pigment);
    }
    // The resident's yard: signs of occupation on the real ground around the hut, all in the
    // same batch. Material tag 64 keeps the plain board treatment for non-timber pigments.
    const auto tri=[&](Vector3 a,Vector3 b,Vector3 d,Vector3 pigment) {
        const std::array<Vector3,3> local{a,b,d};
        a=world(a); b=world(b); d=world(d);
        Vector3 normal=Vector3::Cross(b-a,d-a); normal.Normalize();
        const float light=0.48f+0.52f*std::max(0.0f,Vector3::Dot(normal,
            Vector3(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ)));
        const Color color(static_cast<int>(pigment.X*light),static_cast<int>(pigment.Y*light),static_cast<int>(pigment.Z*light),materialTag);
        for(const auto p:local)
            mesh.emplace_back(world(p),color,Vector2(Vector3::Dot(p-grainOrigin,grainAcross),Vector3::Dot(p-grainOrigin,grainAlong)));
    };
    const auto yardLocal=[&](std::size_t site) {
        return Vector3(HutLayout::YardSites[site][0],hut.yard[site].groundY-hut.floorY,HutLayout::YardSites[site][1]);
    };
    const Vector3 pole(112,86,56),fishSkin(184,188,180),fishDark(112,118,116),coconutBrown(158,122,72),
        coconutGreen(138,150,74),basketTan(206,172,112),cord(196,180,140);
    {
        // Fish-drying rack: two posts, a bar and four split fish hung to cure.
        const Vector3 g=yardLocal(0);
        beam(g+Vector3(0,-0.05f,-0.7f),g+Vector3(0,1.55f,-0.7f),0.07f,0.07f,pole);
        beam(g+Vector3(0,-0.05f,0.7f),g+Vector3(0,1.55f,0.7f),0.07f,0.07f,pole);
        beam(g+Vector3(0,1.5f,-0.78f),g+Vector3(0,1.5f,0.78f),0.05f,0.05f,pole);
        materialTag=64; grainAcross={1,0,0}; grainAlong={0,1,0};
        hangTag=40; fishBarY_=hut.floorY+g.Y+1.47f;
        for(int fish=0;fish<4;++fish) {
            const float z=-0.5f+static_cast<float>(fish)*0.33f;
            const float lean=(fish%2==0?0.02f:-0.02f);
            grainOrigin=g+Vector3(0,1.1f,z);
            const Vector3 a=g+Vector3(-0.045f,1.47f,z),b=g+Vector3(0.045f,1.47f,z);
            const Vector3 d=g+Vector3(0.04f+lean,1.14f,z+0.012f),e=g+Vector3(-0.04f+lean,1.14f,z+0.012f);
            quad(a,b,d,e,fish%2==0?fishSkin:fishDark); quad(a,e,d,b,fish%2==0?fishDark:fishSkin);
            tri(e,d,g+Vector3(lean*1.5f,1.02f,z+0.018f),fishDark); tri(d,e,g+Vector3(lean*1.5f,1.02f,z+0.018f),fishDark);
        }
    }
    {
        // Coconut pile: a small heap of husked nuts beside the steps.
        hangTag=0;
        const Vector3 g=yardLocal(1);
        materialTag=64; grainAcross={1,0,0}; grainAlong={0,0,1};
        constexpr std::array<std::array<float,3>,7> nuts{{{0,0.09f,0},{0.2f,0.09f,0.05f},{-0.19f,0.09f,0.08f},
            {0.03f,0.09f,-0.2f},{0.08f,0.27f,-0.06f},{-0.11f,0.27f,0.03f},{-0.02f,0.44f,-0.04f}}};
        for(std::size_t nut=0;nut<nuts.size();++nut) {
            const Vector3 centre=g+Vector3(nuts[nut][0],nuts[nut][1],nuts[nut][2]);
            grainOrigin=centre;
            const float r=0.12f+0.014f*static_cast<float>(nut%3),h=0.13f;
            const Vector3 pigment=nut%3==1?coconutGreen:coconutBrown;
            const Vector3 top=centre+Vector3(0,h,0),bottom=centre-Vector3(0,h,0);
            const std::array<Vector3,4> ring{centre+Vector3(r,0,0),centre+Vector3(0,0,r),centre-Vector3(r,0,0),centre-Vector3(0,0,r)};
            for(int side=0;side<4;++side) {
                tri(top,ring[static_cast<std::size_t>(side)],ring[static_cast<std::size_t>((side+1)%4)],pigment);
                tri(bottom,ring[static_cast<std::size_t>((side+1)%4)],ring[static_cast<std::size_t>(side)],pigment);
            }
        }
    }
    {
        // Net frame: two posts, two rails and a hung fishing net of cords.
        const Vector3 g=yardLocal(2);
        beam(g+Vector3(0,-0.05f,-0.95f),g+Vector3(0,1.35f,-0.95f),0.07f,0.07f,pole);
        beam(g+Vector3(0,-0.05f,0.95f),g+Vector3(0,1.35f,0.95f),0.07f,0.07f,pole);
        beam(g+Vector3(0,1.28f,-1.0f),g+Vector3(0,1.28f,1.0f),0.05f,0.05f,pole);
        beam(g+Vector3(0,0.36f,-1.0f),g+Vector3(0,0.36f,1.0f),0.04f,0.04f,pole);
        hangTag=48; netRailY_=hut.floorY+g.Y+1.27f;
        for(int strand=0;strand<8;++strand) {
            const float z=-0.84f+static_cast<float>(strand)*0.24f;
            beam(g+Vector3(0.02f,1.27f,z),g+Vector3(0.02f+0.03f*std::sin(static_cast<float>(strand)*1.7f),0.38f,z+0.03f),0.014f,0.014f,cord);
        }
        for(int row=0;row<4;++row) {
            const float y=0.52f+static_cast<float>(row)*0.22f;
            beam(g+Vector3(0.025f,y,-0.9f),g+Vector3(0.025f,y-0.03f,0.9f),0.012f,0.012f,cord);
        }
    }
    {
        // Woven basket: an open eight-sided tub with a darker rim band.
        hangTag=0;
        const Vector3 g=yardLocal(3);
        materialTag=64; grainAcross={1,0,0}; grainAlong={0,1,0}; grainOrigin=g;
        constexpr int sides=8;
        for(int side=0;side<sides;++side) {
            const float a0=2.0f*std::numbers::pi_v<float>*static_cast<float>(side)/sides;
            const float a1=2.0f*std::numbers::pi_v<float>*static_cast<float>(side+1)/sides;
            const Vector3 b0=g+Vector3(std::cos(a0)*0.26f,0.0f,std::sin(a0)*0.26f);
            const Vector3 b1=g+Vector3(std::cos(a1)*0.26f,0.0f,std::sin(a1)*0.26f);
            const Vector3 t0=g+Vector3(std::cos(a0)*0.32f,0.55f,std::sin(a0)*0.32f);
            const Vector3 t1=g+Vector3(std::cos(a1)*0.32f,0.55f,std::sin(a1)*0.32f);
            quad(b0,b1,t1,t0,basketTan); quad(b1,b0,t0,t1,basketTan*0.86f);
            quad(t0,t1,t1+Vector3(0,0.04f,0),t0+Vector3(0,0.04f,0),pole);
        }
        tri(g+Vector3(0.2f,0.005f,0),g+Vector3(-0.1f,0.005f,0.17f),g+Vector3(-0.1f,0.005f,-0.17f),basketTan*0.8f);
    }
    // The yard props add roughly a thousand vertices to the former 33,000 ceiling.
    if(mesh.empty()||mesh.size()>36000) throw std::runtime_error("Hut mesh exceeds budget");
    Vector3 low(std::numeric_limits<float>::max()),high(std::numeric_limits<float>::lowest());
    for(const auto& vertex:mesh) {
        if(!std::isfinite(vertex.TextureCoordinate.X)||!std::isfinite(vertex.TextureCoordinate.Y))
            throw std::runtime_error("Nonfinite hut grain coordinate");
        if(!std::isfinite(vertex.Position.X)||!std::isfinite(vertex.Position.Y)||!std::isfinite(vertex.Position.Z))
            throw std::runtime_error("Nonfinite hut vertex");
        low=Vector3::Min(low,vertex.Position); high=Vector3::Max(high,vertex.Position);
    }
    for(std::size_t i=0;i<mesh.size();i+=3)
        if(Vector3::Cross(mesh[i+1].Position-mesh[i].Position,mesh[i+2].Position-mesh[i].Position).LengthSquared()<1e-12f)
            throw std::runtime_error("Degenerate hut triangle");
    bounds_=BoundingBox(low,high); triangles_=static_cast<int>(mesh.size()/3);
    landmark_shapes::Note("hut",mesh);
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColorTexture::getVertexDeclarationStatic(),static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    effect_->setVertexColorEnabledProperty(true); effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true); effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(180); effect_->setFogEndProperty(300);
    const auto levels=LoadOpaqueImageMips(device_,"Content/structures/hut-palm-fibre.png",256);
    const auto& mean=levels.back().pixels[0];
    thatchMean_=Vector3(mean[0]/255.0f,mean[1]/255.0f,mean[2]/255.0f);
    if(std::min({thatchMean_.X,thatchMean_.Y,thatchMean_.Z})<0.01f)
        throw std::runtime_error("Hut thatch texture mean too dark");
    thatch_=std::make_unique<Texture2D>(device_,256,256,true,SurfaceFormat::Color);
    for(std::size_t level=0;level<levels.size();++level) {
        const auto& pixels=levels[level].pixels;
        thatch_->SetData(static_cast<int>(level),nullptr,
            reinterpret_cast<const std::uint8_t*>(pixels.data()),0,static_cast<int>(pixels.size()*4));
    }
    const char* textureMessage="island-simulator: hut palm fibre loaded Content/structures/hut-palm-fibre.png; 256x256, 9 mips, 349524 RGBA8 bytes";
    std::cout<<textureMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(textureMessage);
    try {
        if(!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects))
            throw std::runtime_error("custom effects unavailable");
        using CNA::Graphics::ShaderCodeEXT;
        const auto es=[](const char* body){return std::string("#version 300 es\nprecision highp float;\n")
            +environment::FogShaderDefinition+body;};
        const auto desktop=[](const char* body){return std::string("#version 330 core\n")
            +environment::FogShaderDefinition+body;};
        // The fragment stage also receives the sun shadow map and the cumulus shade.
        const auto esFragment=[](const char* body){return std::string("#version 300 es\nprecision highp float;\n")
            +environment::FogShaderDefinition+SunShadowMap::ReceiverShader+body;};
        const auto desktopFragment=[](const char* body){return std::string("#version 330 core\n")
            +environment::FogShaderDefinition+SunShadowMap::ReceiverShader+body;};
        const CNA::Graphics::ShaderPackageEXT package({
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Vertex,"main","hut/es.vert",es(HutMaterialVertex)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Fragment,"main","hut/es.frag",esFragment(HutMaterialFragment)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Vertex,"main","hut/gl.vert",desktop(HutMaterialVertex)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Fragment,"main","hut/gl.frag",desktopFragment(HutMaterialFragment))},
            {CNA::ShaderStageEXT::Vertex,CNA::ShaderStageEXT::Fragment});
        material_=std::make_unique<ShaderEffect>(device,package);
        if(!material_->IsEffectValid()) throw std::runtime_error(material_->GetCompileErrorEXT());
        const char* message="island-simulator: hut beam-aligned weathered grain and filtered thatch fibres";
        std::cout<<message<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(message);
        const char* timberMessage="island-simulator: hut timber material uses coarse warped grain, recessed dark fibres, knot bands, and sun-bleached board variation";
        std::cout<<timberMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(timberMessage);
        const char* facadeMessage="island-simulator: hut four-pane muted glazing and braced plank door";
        std::cout<<facadeMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(facadeMessage);
        const char* yardMessage="island-simulator: hut yard shows the resident's fish-drying rack, coconut pile, net frame and woven basket; the hung fish and the net swing in the breeze and lay over in a gust front";
        std::cout<<yardMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(yardMessage);
        std::cout<<"island-simulator: hut yard ground offsets";
        for(const auto& site:hut.yard) std::cout<<' '<<(site.groundY-hut.floorY);
        std::cout<<'\n';
        const char* verandaMessage="island-simulator: hut raised veranda uses full-height outer posts, split rails, fascia, and three steps";
        std::cout<<verandaMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(verandaMessage);
        const char* structureMessage="island-simulator: hut enclosed slatted gable, framed attic light, and four knee-braced veranda supports";
        std::cout<<structureMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(structureMessage);
        const char* wallMessage="island-simulator: hut wall boards meet edge-to-edge on all four sides and both gables without see-through gaps";
        std::cout<<wallMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(wallMessage);
        const char* craftMessage="island-simulator: hut uses unequal fitted wall boards, overlapping ridge-cap bundles, joint lashings, and attached open shutters";
        std::cout<<craftMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(craftMessage);
        const char* shadowMessage="island-simulator: the hut receives the sun shadows: its eaves shade its walls and the trees' shadows and the cumulus shade fall across it";
        std::cout<<shadowMessage<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(shadowMessage);
    } catch(const std::exception& error) {
        material_.reset();
        const std::string message=std::string("island-simulator: hut material fallback: ")+error.what();
        std::cerr<<message<<'\n'; MirrorRuntimeDiagnosticToPlatformLog(message);
    }
    MirrorRuntimeDiagnosticToPlatformLog("island-simulator: hut timber frame and layered thatch, triangles="+std::to_string(triangles_));
}
HutRenderer::~HutRenderer()=default;
void HutRenderer::DrawNightLights(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                  const float nightness,const float seconds) {
    if(!(nightness>0.02f)||(lantern_-camera).LengthSquared()>260.0f*260.0f) return;
    if(!glowEffect_) {
        glowEffect_=std::make_unique<BasicEffect>(device_);
        glowEffect_->setVertexColorEnabledProperty(true);glowEffect_->setLightingEnabledProperty(false);
        glowEffect_->setFogEnabledProperty(false);
        glow_=std::make_unique<DynamicVertexBuffer>(device_,VertexPositionColor::getVertexDeclarationStatic(),256,BufferUsage::WriteOnly);
    }
    glowStaging_.clear();
    // the flame's slow breathing
    const float flicker=.90f+.06f*std::sin(seconds*5.3f)+.04f*std::sin(seconds*11.7f+.8f);
    const float k=std::clamp(nightness,0.0f,1.0f)*flicker;
    const auto c=[k](float v){return static_cast<int>(std::lround(std::clamp(v*k,0.0f,1.0f)*255.0f));};
    // the windows: warm light from within, a little brighter low (the lamp sits on a table)
    const Color low(c(.62f),c(.36f),c(.14f),255),high(c(.46f),c(.25f),c(.09f),255);
    for(std::size_t w=0;w+3<windowCorners_.size();w+=4) {
        const auto& p=windowCorners_;
        glowStaging_.emplace_back(p[w],low);glowStaging_.emplace_back(p[w+1],low);glowStaging_.emplace_back(p[w+2],high);
        glowStaging_.emplace_back(p[w],low);glowStaging_.emplace_back(p[w+2],high);glowStaging_.emplace_back(p[w+3],high);
    }
    // the lantern's glow: a soft disc facing the eye
    Vector3 toCamera=camera-lantern_;
    const float distance=toCamera.Length();
    if(distance>0.3f) {
        toCamera/=distance;
        Vector3 right=Vector3::Cross(Vector3::Up,toCamera);
        if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
        right.Normalize();
        const Vector3 up=Vector3::Cross(toCamera,right);
        const Color centre(c(1.0f),c(.72f),c(.38f),255),edge(0,0,0,0);
        const float size=.22f+.14f*k;
        constexpr int segments=16;
        for(int i=0;i<segments;++i) {
            const float a0=6.2831853f*static_cast<float>(i)/segments,a1=6.2831853f*static_cast<float>(i+1)/segments;
            const Vector3 o=lantern_+toCamera*.08f;
            glowStaging_.emplace_back(o,centre);
            glowStaging_.emplace_back(o+(right*std::cos(a0)+up*std::sin(a0))*size,edge);
            glowStaging_.emplace_back(o+(right*std::cos(a1)+up*std::sin(a1))*size,edge);
        }
    }
    // Moths drawn to the lantern: pale specks lit by its flame, each looping round it on its
    // own erratic path within a forearm's reach, now and then bumping in close.
    if(distance>0.3f&&distance<22.0f) {
        Vector3 toCamera=Vector3::Normalize(camera-lantern_);
        Vector3 right=Vector3::Cross(Vector3::Up,toCamera);
        if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
        right.Normalize();
        const Vector3 up=Vector3::Cross(toCamera,right);
        const float near=1.0f-std::clamp((distance-14.0f)/8.0f,0.0f,1.0f);
        for(int moth=0;moth<7;++moth) {
            const float m=static_cast<float>(moth);
            const float a=seconds*(2.1f+0.37f*m)+m*1.9f;
            const float bump=0.5f+0.5f*std::sin(seconds*(0.9f+0.21f*m)+m);
            const float r=0.16f+0.42f*bump;
            const Vector3 at=lantern_+Vector3(std::cos(a)*r,0.18f*std::sin(seconds*(3.1f+0.5f*m)+m*2.3f)+0.05f,std::sin(a*1.13f)*r);
            // brighter the nearer the flame, flickering as the wings beat
            const float lit=k*near*(0.35f+0.65f*(1.0f-std::clamp((r-0.16f)/0.42f,0.0f,1.0f)))*(0.7f+0.3f*std::sin(seconds*31.0f+m*5.0f));
            const Color moth0(c(0.95f*lit),c(0.85f*lit),c(0.62f*lit),255),edge(0,0,0,0);
            const float size=0.018f+0.004f*std::fmod(m*1.7f,1.0f);
            constexpr int mothSegments=5;
            for(int i=0;i<mothSegments;++i) {
                const float a0=6.2831853f*static_cast<float>(i)/mothSegments,a1=6.2831853f*static_cast<float>(i+1)/mothSegments;
                glowStaging_.emplace_back(at,moth0);
                glowStaging_.emplace_back(at+(right*std::cos(a0)+up*std::sin(a0))*size*1.8f,edge);
                glowStaging_.emplace_back(at+(right*std::cos(a1)+up*std::sin(a1))*size*1.8f,edge);
            }
        }
    }
    if(glowStaging_.empty()) return;
    glow_->SetData(glowStaging_.data(),0,static_cast<int>(glowStaging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(glow_.get());device_.setIndicesProperty(nullptr);
    glowEffect_->setWorldProperty(Matrix::getIdentityProperty());
    glowEffect_->setViewProperty(view);glowEffect_->setProjectionProperty(projection);glowEffect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(glowStaging_.size()/3));
    ++drawCalls_;
}

void HutRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z))return;
    if(!sunShadowEffect_) {
        try { sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"hut"); }
        catch(const std::exception& error) {
            std::cout<<"island-simulator: hut sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    map.CountCasters(1);
}
void HutRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const float seconds) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>300*300 || BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque); device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get()); device_.setIndicesProperty(nullptr);
    if(material_) {
        float matrix[16]; (view*projection).ToColumnMajor(matrix);
        material_->Apply(); material_->SetUniformMat4("uViewProjection",matrix);
        device_.getSamplerStatesProperty()[0]=SamplerState::LinearClamp;
        material_->SetTexture(0,*thatch_); material_->SetUniformInt("uThatchTexture",0);
        material_->SetUniformVec3("uThatchMean",thatchMean_.X,thatchMean_.Y,thatchMean_.Z);
        material_->SetUniformVec3("uCameraPosition",camera.X,camera.Y,camera.Z);
        material_->SetUniformVec3("uHutCentre",centre_.X,centre_.Y,centre_.Z);
        material_->SetUniformFloat("uGust",std::isfinite(seconds)?WindGust::Strength(centre_.X,centre_.Z,seconds):0.0f);
        material_->SetUniformFloat("uSeconds",std::isfinite(seconds)?seconds:0.0f);
        material_->SetUniformVec2("uHangTop",fishBarY_,netRailY_);
        material_->SetUniformFloat("uWetness",weather_now::Current().wetness);
        material_->SetUniformVec3("uToSun",sun_light::UniformDirection().x,sun_light::UniformDirection().y,
            sun_light::UniformDirection().z);
        if(sunShadow_) sunShadow_->Bind(*material_);
        else SunShadowMap::BindDisabled(*material_);
    } else {
        effect_->setWorldProperty(Matrix::getIdentityProperty()); effect_->setViewProperty(view); effect_->setProjectionProperty(projection); effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_); drawCalls_=1;
}
}
