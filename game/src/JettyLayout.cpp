#include "IslandSimulator/JettyLayout.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>

namespace island {
float JettyPlacement::DeckHeight(const float along) const noexcept {
    if(!std::isfinite(along)) return deckY;
    const float t=std::clamp(along/RampLength,0.0f,1.0f);
    return rootDeckY+(deckY-rootDeckY)*t*t*(3-2*t);
}

bool JettyPlacement::OnDeck(const float x,const float z) const noexcept {
    if(!std::isfinite(x)||!std::isfinite(z)) return false;
    const float dx=x-rootX,dz=z-rootZ;
    const float along=dx*dirX+dz*dirZ,across=dx*AcrossX()+dz*AcrossZ();
    return along>=-0.25f&&along<=Length&&std::abs(across)<=DeckHalfWidth;
}

float JettyPlacement::SupportHeight(const float x,const float z,const float terrainHeight) const noexcept {
    if(!std::isfinite(terrainHeight)||!OnDeck(x,z)) return terrainHeight;
    const float along=(x-rootX)*dirX+(z-rootZ)*dirZ;
    return std::max(terrainHeight,DeckHeight(std::clamp(along,0.0f,Length)));
}

std::array<CircularObstacle,2> JettyPlacement::CanoeObstacles() const noexcept {
    const float ax=std::sin(canoeYaw),az=std::cos(canoeYaw);
    const float offset=CanoeLength*0.24f,radius=CanoeBeam*0.5f+0.42f;
    return {{{canoeX+ax*offset,canoeZ+az*offset,radius},{canoeX-ax*offset,canoeZ-az*offset,radius}}};
}

std::array<AnimalTrackPlacement,JettyPlacement::FootprintCount> JettyPlacement::FootprintLines(
        const IslandTerrain& terrain,const RenderedTerrainSurface& ground) const noexcept {
    std::array<AnimalTrackPlacement,FootprintCount> prints{};
    // The ground behind this root rises onto rock within a few metres and the shoreline
    // curves, so the walk home follows the actual beach: a stride every 0.62 m along the
    // shore arc on the canoe's side, 1.4 m inside the shoreline, feet alternating either
    // side of the line and slightly toed out.
    const float rootAngle=std::atan2(rootZ,rootX);
    const float canoeAngle=std::atan2(canoeZ,canoeX);
    const float turn=std::remainder(canoeAngle-rootAngle,6.2831853f)<0.0f?-1.0f:1.0f;
    const float radius=std::max(std::hypot(rootX,rootZ),1.0f);
    for(std::size_t i=0;i<FootprintCount;++i) {
        const float arc=1.6f+0.62f*static_cast<float>(i);
        const float angle=rootAngle+turn*arc/radius;
        const float side=(i%2U==0U)?-1.0f:1.0f;
        const float shore=terrain.ShoreRadius(angle)-1.4f+side*0.11f;
        AnimalTrackPlacement& print=prints[i];
        print.x=std::cos(angle)*shore;
        print.z=std::sin(angle)*shore;
        print.y=ground.Sample(print.x,print.z).height;
        print.yawRadians=std::atan2(-std::sin(angle)*turn,std::cos(angle)*turn)+side*0.12f;
        print.scale=1.0f;
        print.variant=2U;
        print.streamBank=false;
    }
    return prints;
}

BirdPerchSite JettyPlacement::MooringPerch() const noexcept {
    const auto& post=posts[PostCount-1];
    return {post.x,post.topY,post.z,std::atan2(dirZ,dirX),BirdPerchKind::Wreck};
}

std::optional<JettyPlacement> JettyLayout::Try(const IslandTerrain& terrain,const float angle) {
    JettyPlacement jetty;
    jetty.dirX=std::cos(angle);jetty.dirZ=std::sin(angle);
    const float shore=terrain.ShoreRadius(angle);
    jetty.rootX=jetty.dirX*(shore-RootInland);jetty.rootZ=jetty.dirZ*(shore-RootInland);
    const float rootHeight=terrain.Height(jetty.rootX,jetty.rootZ);
    if(rootHeight<MinimumRootHeight||rootHeight>MaximumRootHeight) return std::nullopt;
    const float endX=jetty.rootX+jetty.dirX*JettyPlacement::Length;
    const float endZ=jetty.rootZ+jetty.dirZ*JettyPlacement::Length;
    const float endHeight=terrain.Height(endX,endZ);
    if(endHeight<MinimumEndHeight||endHeight>MaximumEndHeight) return std::nullopt;
    // The deck must clear the ground everywhere and the far half must stand in water.
    for(float along=1.0f;along<JettyPlacement::Length;along+=1.0f) {
        const float height=terrain.Height(jetty.rootX+jetty.dirX*along,jetty.rootZ+jetty.dirZ*along);
        if(height>rootHeight+0.35f) return std::nullopt;
        if(along>=JettyPlacement::Length*0.5f&&height>0.0f) return std::nullopt;
    }
    jetty.rootDeckY=rootHeight+RootStep;
    jetty.deckY=std::max(DeckClearance,jetty.rootDeckY);
    // The canoe lies on the sand beside the root, angled a little toward the water.
    jetty.canoeX=jetty.rootX-jetty.dirX*1.0f+jetty.AcrossX()*3.4f;
    jetty.canoeZ=jetty.rootZ-jetty.dirZ*1.0f+jetty.AcrossZ()*3.4f;
    jetty.canoeY=terrain.Height(jetty.canoeX,jetty.canoeZ);
    if(jetty.canoeY<MinimumCanoeHeight||jetty.canoeY>MaximumCanoeHeight) return std::nullopt;
    jetty.canoeYaw=std::atan2(jetty.dirX,jetty.dirZ)+1.15f;
    for(std::size_t pair=0;pair<JettyPlacement::PairCount;++pair) {
        const float along=1.6f+JettyPlacement::PostPitch*static_cast<float>(pair);
        for(std::size_t side=0;side<2;++side) {
            const float across=side==0?-JettyPlacement::PostAcross:JettyPlacement::PostAcross;
            JettyPost& post=jetty.posts[pair*2+side];
            post.x=jetty.rootX+jetty.dirX*along+jetty.AcrossX()*across;
            post.z=jetty.rootZ+jetty.dirZ*along+jetty.AcrossZ()*across;
            post.bottomY=terrain.Height(post.x,post.z)-0.35f;
            post.topY=jetty.DeckHeight(along)-0.02f;
        }
    }
    JettyPost& mooring=jetty.posts[JettyPlacement::PostCount-1];
    mooring.x=jetty.rootX+jetty.dirX*JettyPlacement::MooringAlong
        +jetty.AcrossX()*JettyPlacement::PostAcross;
    mooring.z=jetty.rootZ+jetty.dirZ*JettyPlacement::MooringAlong
        +jetty.AcrossZ()*JettyPlacement::PostAcross;
    mooring.bottomY=terrain.Height(mooring.x,mooring.z)-0.35f;
    mooring.topY=jetty.deckY+JettyPlacement::MooringHeight;
    return jetty;
}

std::optional<JettyPlacement> JettyLayout::FindInWorld(const IslandTerrain& terrain,
                                                       const HutPlacement& hut) {
    if(!std::isfinite(hut.x)||!std::isfinite(hut.z)) return std::nullopt;
    const float base=std::atan2(hut.z,hut.x);
    for(int attempt=0;attempt<25;++attempt) {
        const float fan=static_cast<float>((attempt+1)/2)*0.03f*(attempt%2==0?1.0f:-1.0f);
        if(auto jetty=Try(terrain,base+fan)) return jetty;
    }
    return std::nullopt;
}
}
