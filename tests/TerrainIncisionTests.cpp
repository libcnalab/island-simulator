#include "IslandSimulator/TerrainIncision.hpp"
#include "IslandSimulator/StreamBedPatch.hpp"
#include <iostream>
#include <limits>
#include <memory>

int main() try
{
    namespace cut=island::terrain_incision;
    const auto require=[](bool condition,const char* message) {
        if(!condition) throw std::runtime_error(message);
    };
    require(cut::Pack(7,7)==255 && cut::Pack(7,8)==255,"Uncut or raised ground must keep opaque/default metadata");
    for(int i=0;i<=1000;++i) {
        const float depth=static_cast<float>(i)*.001f;
        require(std::abs(cut::Depth(cut::Pack(7,7-depth))-depth)<=.002f,
                "One-byte incision must preserve depth within two millimetres");
    }
    require(cut::Pack(7,5)==0 && cut::Depth(0)==1,"Deep cuts must saturate safely");
    require(cut::BankCoverage(0,1)==0 && cut::BankCoverage(.5f,0)==0,
            "Slope alone and a flat river bed must not become an exposed bank");
    island::IslandTerrain terrain;
    const auto original=std::make_unique<island::RenderedTerrainSurface>(terrain);
    const island::StreamCourse course(terrain);
    const island::StreamBedPatch bed(course,*original);
    int exposed=0,left=0,right=0,uncut=0;
    const auto height=[&](float x,float z){return bed.Contains(x,z)?bed.SampleHeight(x,z):original->Height(x,z);};
    for(std::size_t i=0;i<bed.Vertices().size();i+=7) {
        const auto p=bed.Vertices()[i];
        const float incision=original->Height(p.x,p.z)-p.y;
        const auto alpha=cut::Pack(original->Height(p.x,p.z),p.y);
        if(incision<=0) {require(alpha==255,"Outside the actual incision the material must remain untouched");++uncut;}
        const float dx=(height(p.x+.1f,p.z)-height(p.x-.1f,p.z))/.2f;
        const float dz=(height(p.x,p.z+.1f)-height(p.x,p.z-.1f))/.2f;
        const float slope=std::atan(std::hypot(dx,dz));
        const float coverage=cut::BankCoverage(cut::Depth(alpha),slope);
        require(std::isfinite(coverage)&&coverage>=0&&coverage<=1,"Bank coverage escaped its bounded field");
        if(coverage>.5f) {
            require(incision>.07f && slope>.20f,"Bank pigment escaped real incised slope geometry");
            ++exposed;
            (p.z<island::StreamRoute::CentreZ(p.x)?left:right)++;
        }
    }
    require(exposed>100&&left>30&&right>30&&uncut>100,"World test must cover both actual banks and unchanged shoulders");
    bool rejected=false;
    try{(void)cut::Pack(std::numeric_limits<float>::quiet_NaN(),0);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"Invalid height data must not enter packed material metadata");
    std::cout<<"Actual stream incision grounds both bank materials: "<<exposed<<" exposed, "<<uncut<<" unchanged samples\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
