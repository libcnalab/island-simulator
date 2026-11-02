#include "IslandSimulator/BunkerQuadSupport.hpp"
#include "IslandSimulator/BunkerTurnInspection.hpp"
#include "IslandSimulator/BunkerChaseCamera.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <iostream>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;const RenderedTerrainSurface surface(terrain);const BunkerLayout bunker(terrain);
    for(float dt:{.05f,.1f}) {
        QuadBike quad;BunkerTurnInspection drive;bool inside=false;int collisions=0,reverseFrames=0;
        const auto ground=[&](float x,float z) {
            if(auto g=SampleBunkerQuadGround(bunker,x,z,quad.Position().y-.6f,inside))return *g;
            const auto s=surface.Sample(x,z);BikeGround g;
            g.height=s.height;g.normal={s.normal.x,s.normal.y,s.normal.z};return g;
        };
        const auto start=bunker.World(-.5f,0,-6),toward=bunker.World(-.5f,0,-5);
        quad.Place({start.x,0,start.z},std::atan2(toward.x-start.x,toward.z-start.z),ground);
        int frame=0;float deepest=-6;BunkerPoint camera{start.x,start.y+1.9f,start.z};
        for(;frame<static_cast<int>(80/dt)&&!drive.Completed();++frame) {
            auto p=quad.Position();float floor;inside=bunker.SupportHeight(p.x,p.z,p.y-.6f,floor);
            const auto controls=drive.Controls(bunker,quad);
            quad.Step(dt,controls,ground,[&](float x,float z,float,float&,float&) {
                // The accepted route must be collision-free at EVERY physics substep.
                // Returning true with no push cannot masquerade as a resolved collision.
                const bool hit=BunkerQuadFootprintBlocked(bunker,quad,x,z);collisions+=hit;return hit;
            });
            p=quad.Position();const auto local=bunker.Local({p.x,p.y,p.z});deepest=std::max(deepest,local.z);
            drive.Observe(quad,ground(p.x,p.z).height,BunkerQuadFootprintBlocked(bunker,quad,p.x,p.z));
            if(inside) {
                const auto f=quad.Forward();
                const BunkerPoint desired{p.x-f.x*4.8f,p.y+1.9f,p.z-f.z*4.8f};
                const float blend=1-std::exp(-9*dt);
                camera={std::lerp(camera.x,desired.x,blend),std::lerp(camera.y,desired.y,blend),
                        std::lerp(camera.z,desired.z,blend)};
                camera=FitBunkerChaseCamera(bunker,{p.x,p.y+.7f,p.z},camera);
                const BunkerPoint from{p.x,p.y+.7f-.18f,p.z},to{camera.x,camera.y-.18f,camera.z};
                require(!bunker.IntersectsCapsule(to,.18f,.36f)
                    &&bunker.SafeTravelFraction(from,to,.18f,.36f)>.999f,
                    "Chase camera clips or sees through bunker wall during continuous turn");
            }
            require(std::isfinite(p.y)&&!quad.Overturned(),"Bunker round trip overturns or corrupts the quad");
            require(collisions==0,"Bunker turn cuts a rendered wall or door jamb");
            reverseFrames+=quad.Speed()<-.2f;
            const auto head=quad.ToWorld({0,1.10f,-.15f});const auto h=bunker.Local({head.x,head.y,head.z});
            if(h.z>=0&&h.z<36)require(h.y<bunker.FloorRelative(h.z)+(h.z<10?2.75f:3.05f)+.03f,
                                     "Continuous drive pushes rider through bunker ceiling");
            // The workshop's rendered cabinets and bench are not generic room floor.
            // A conservative full-body AABB proves this route never exploits their
            // missing individual collision proxies. No extra obstacle is invented in play.
            float left=1e30f,right=-1e30f,front=1e30f,back=-1e30f;
            for(float along:{-1.0f,1.0f})for(float across:{-.62f,.62f}) {
                const auto c=quad.ToWorld({across,0,along});const auto q=bunker.Local({c.x,c.y,c.z});
                left=std::min(left,q.x);right=std::max(right,q.x);front=std::min(front,q.z);back=std::max(back,q.z);
            }
            const auto overlaps=[&](float a,float b,float c,float d){return right>a&&left<b&&back>c&&front<d;};
            if(local.x>1.5f&&local.z>25.8f) {
                for(float depth:{27.0f,29.1f,33.8f})require(!overlaps(6.42f,6.94f,depth-.72f,depth+.72f),
                    "Turn drives through a rendered workshop cabinet");
                require(!overlaps(3.15f,5.95f,34.25f,35.15f),"Turn drives through the rendered workbench");
                require(!overlaps(6.70f,7.04f,29.84f,33.0f),"Turn drives through the distribution board");
            }
        }
        require(drive.Completed()&&drive.TurnSegment()==6&&deepest>31&&reverseFrames>20,
                "Quad did not enter, perform a driven multipoint turn and escape without re-placement");
        require(drive.BlockedSamples()==0&&drive.OverturnedSamples()==0&&drive.MaximumFloorGap()<.3f,
                "Bunker round trip loses floor support or crosses collision");
        const auto p=quad.Position();const auto local=bunker.Local({p.x,p.y,p.z});
        require(local.z<-4&&quad.Speed()>.2f,"Round trip did not leave the portal in forward gear");
        std::cout<<"dt="<<dt<<" frames="<<frame<<" depth="<<local.z<<" reverse-frames="<<reverseFrames
                 <<" max-floor-gap="<<drive.MaximumFloorGap()<<" collision-substeps="<<collisions<<'\n';
    }
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
