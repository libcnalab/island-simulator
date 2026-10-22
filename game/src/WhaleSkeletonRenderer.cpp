#include "IslandSimulator/WhaleSkeletonRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/StructureMeshBuilder.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// Sun-bleached bone: pigment past full so the lit faces whiten and the sides stay cream (the
// build lights each face 0.46 + 0.54 n.s); the sand's stain and the nostrils' shadow.
const Vector3 Bone(300,290,262),SandStain(232,210,166),Nostril(112,98,80);

float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}

// The bones in the skeleton's own frame: `along` metres from the skull's back toward the tail,
// `across` toward the sea, `lift` above the sand there.
struct Bones {
    StructureMeshBuilder builder;
    const WhaleSkeletonPlacement& placement;
    const IslandTerrain& terrain;
    const RenderedTerrainSurface& ground;
    // a point inside the piece being built: each face is wound to face away from it, as the
    // build's light (and the hour's relight) takes its normal from the winding
    Vector3 inside{};
    [[nodiscard]] float Ground(const float x,const float z) const {return DrawnOrAnalyticHeight(ground,terrain,x,z);}
    [[nodiscard]] Vector3 At(const float along,const float across,const float lift) const {
        const auto q=placement.At(along,across);
        return Vector3(q[0],Ground(q[0],q[1])+lift,q[1]);
    }
    // the bone's own colour, stained toward the sand's where it lies in it
    void Face(const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& bone) {
        const Vector3 m=(a+b+c)/3.0f;
        const float above=m.Y-Ground(m.X,m.Z);
        const float stain=1.0f-std::clamp((above+0.02f)/0.14f,0.0f,1.0f);
        // weathered bone: a broad mottle and finer pitting, a little greyer in patches
        const float broad=std::sin(m.X*1.9f+m.Z*1.3f)*std::sin(m.X*0.8f-m.Z*2.1f+m.Y*3.0f);
        const float fine=std::sin(m.X*13.0f+m.Y*17.0f)*std::sin(m.Z*11.0f-m.Y*7.0f);
        Vector3 weathered=bone*(0.97f+0.03f*broad+0.012f*fine);
        weathered=Vector3::Lerp(weathered,Vector3(weathered.Y,weathered.Y,weathered.Y)*0.97f,std::clamp(broad-0.35f,0.0f,1.0f)*0.6f);
        const Vector3 pigment=Vector3::Lerp(weathered,SandStain,stain*0.75f);
        if(Vector3::Dot(Vector3::Cross(b-a,c-a),m-inside)>=0.0f) builder.Tri(a,b,c,pigment);
        else builder.Tri(a,c,b,pigment);
    }
    void Quad(const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& d,const Vector3& bone) {
        Face(a,b,c,bone);Face(a,c,d,bone);
    }
    // A tube through the points with their radii, flattened by `squash` toward the vertical,
    // closed at both ends.
    void Tube(const std::vector<Vector3>& points,const std::vector<float>& radii,const int sides,
              const Vector3& bone,const float squash=1.0f) {
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
                Quad(rings[i][k0],rings[i+1][k0],rings[i+1][k1],rings[i][k1],bone);
            }
        }
        for(int k=0;k<sides;++k) {
            const auto k0=static_cast<std::size_t>(k),k1=static_cast<std::size_t>((k+1)%sides);
            inside=points[1];
            Face(points.front(),rings.front()[k1],rings.front()[k0],bone);
            inside=points[n-2];
            Face(points.back(),rings.back()[k0],rings.back()[k1],bone);
        }
    }
    // A thin plate: centre, its three axes, half extents.
    void Plate(const Vector3& centre,const Vector3& right,const Vector3& up,const Vector3& forward,
               const float halfRight,const float halfUp,const float halfForward,const Vector3& bone) {
        inside=centre;
        const Vector3 r=right*halfRight,u=up*halfUp,f=forward*halfForward;
        const Vector3 p[8]={centre-r-u-f,centre+r-u-f,centre+r+u-f,centre-r+u-f,
                            centre-r-u+f,centre+r-u+f,centre+r+u+f,centre-r+u+f};
        Quad(p[3],p[2],p[6],p[7],bone);Quad(p[0],p[4],p[5],p[1],bone);
        Quad(p[0],p[1],p[2],p[3],bone);Quad(p[5],p[4],p[7],p[6],bone);
        Quad(p[4],p[0],p[3],p[7],bone);Quad(p[1],p[5],p[6],p[2],bone);
    }
};

float SpineAcross(const float along) {return WhaleSkeletonPlacement::SpineAcross(along);}
}

WhaleSkeletonRenderer::WhaleSkeletonRenderer(GraphicsDevice& device,const WhaleSkeletonPlacement& placement,
                                             const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    Bones bones{StructureMeshBuilder{},placement,terrain,ground};
    // The skull of a rorqual: the long, flat, slightly arched rostrum (a lens in section with a
    // low ridge down its middle), widening behind into the thin supraorbital wings over the eyes,
    // the skull's widest point, then the occipital shield rising to its crest and narrowing down
    // to the condyles the spine meets; the dark nasal openings where the rostrum meets the wings.
    {
        constexpr int Stations=34,Arc=18;
        // control points along the skull: station, width, height, fullness (exponent of the arch)
        constexpr std::array<std::array<float,4>,9> Profile{{
            {-3.90f,0.22f,0.10f,1.40f},{-2.50f,0.75f,0.18f,1.40f},{-1.40f,1.15f,0.25f,1.40f},
            {-0.95f,1.35f,0.30f,1.35f},{-0.60f,2.25f,0.42f,2.20f},{-0.30f,2.35f,0.62f,1.80f},
            {0.00f,1.95f,0.76f,1.00f},{0.25f,1.45f,0.62f,0.80f},{0.45f,1.15f,0.45f,0.80f}}};
        const auto profile=[&](const float a,const int field) {
            if(a<=Profile.front()[0]) return Profile.front()[static_cast<std::size_t>(field)];
            for(std::size_t i=0;i+1<Profile.size();++i) {
                if(a<=Profile[i+1][0]) {
                    float t=(a-Profile[i][0])/(Profile[i+1][0]-Profile[i][0]);
                    t=t*t*(3.0f-2.0f*t);
                    return Profile[i][static_cast<std::size_t>(field)]
                        +(Profile[i+1][static_cast<std::size_t>(field)]-Profile[i][static_cast<std::size_t>(field)])*t;
                }
            }
            return Profile.back()[static_cast<std::size_t>(field)];
        };
        const auto width=[&](const float a) {return profile(a,1);};
        const auto height=[&](const float a) {return profile(a,2);};
        const auto fullness=[&](const float a) {return profile(a,3);};
        constexpr float Base=-0.08f;
        std::vector<std::vector<Vector3>> rings(Stations,std::vector<Vector3>(Arc+1));
        std::vector<float> stations(Stations);
        for(int j=0;j<Stations;++j) {
            const float a=-WhaleSkeletonPlacement::SkullLength+(WhaleSkeletonPlacement::SkullLength+0.45f)*static_cast<float>(j)/(Stations-1);
            stations[static_cast<std::size_t>(j)]=a;
            const float w=width(a),h=height(a);
            for(int k=0;k<=Arc;++k) {
                const float theta=Pi*static_cast<float>(k)/Arc;
                const float ridge=a<-0.6f?0.035f*std::pow(1.0f-std::abs(std::cos(theta)),6.0f):0.0f;
                rings[static_cast<std::size_t>(j)][static_cast<std::size_t>(k)]=bones.At(a,0.5f*w*std::cos(theta),
                    Base+h*std::pow(std::sin(theta),fullness(a))+ridge);
            }
        }
        for(int j=0;j+1<Stations;++j) for(int k=0;k<Arc;++k) {
            const float a=0.5f*(stations[static_cast<std::size_t>(j)]+stations[static_cast<std::size_t>(j+1)]);
            const bool nostril=a>-1.15f&&a<-0.80f&&(k==7||k==10);
            const auto& r0=rings[static_cast<std::size_t>(j)];const auto& r1=rings[static_cast<std::size_t>(j+1)];
            bones.inside=bones.At(a,0.0f,Base);
            bones.Quad(r0[static_cast<std::size_t>(k)],r1[static_cast<std::size_t>(k)],r1[static_cast<std::size_t>(k+1)],
                       r0[static_cast<std::size_t>(k+1)],nostril?Nostril:Bone*(0.97f+0.03f*std::sin(a*3.1f+k)));
        }
        // the occiput's back closed over the sand, and the rostrum's tip
        const Vector3 back=bones.At(0.45f,0.0f,Base+0.30f),tip=bones.At(-WhaleSkeletonPlacement::SkullLength-0.06f,0.0f,Base+0.05f);
        for(int k=0;k<Arc;++k) {
            bones.inside=bones.At(0.2f,0.0f,Base+0.2f);
            bones.Face(back,rings.back()[static_cast<std::size_t>(k+1)],rings.back()[static_cast<std::size_t>(k)],Bone*0.94f);
            bones.inside=bones.At(-WhaleSkeletonPlacement::SkullLength+0.4f,0.0f,Base);
            bones.Face(tip,rings.front()[static_cast<std::size_t>(k)],rings.front()[static_cast<std::size_t>(k+1)],Bone);
        }
    }
    // The jaws: two long bowed bones lying either side of the rostrum, splayed a little.
    for(const float side:{-1.0f,1.0f}) {
        std::vector<Vector3> points;std::vector<float> radii;
        for(int i=0;i<=15;++i) {
            const float t=static_cast<float>(i)/15.0f;
            const float a=0.35f+(-WhaleSkeletonPlacement::SkullLength-0.05f-0.35f)*t;
            const float r=0.13f*(1.0f-0.45f*t);
            points.push_back(bones.At(a,side*(1.0f*(1.0f-t)+0.32f*t+0.42f*std::sin(Pi*t)),r*0.45f));
            radii.push_back(r);
        }
        bones.Tube(points,radii,8,Bone*(side<0.0f?0.95f:0.99f),0.8f);
    }
    // The spine: cervical discs, the thoracic vertebrae the ribs hang from, the lumbar ones
    // with tall spines and broad transverse processes, the tail's shrinking to nothing; each a
    // centrum sunk to some sixty per cent of its depth, the last three washed out of line.
    float along=0.40f;
    for(int k=0;k<VertebraCount;++k) {
        float spacing,radius,spine,process;
        if(k<7) {spacing=0.13f;radius=0.19f;spine=0.10f;process=0.0f;}
        else if(k<21) {const float t=static_cast<float>(k-7)/13.0f;spacing=0.30f;radius=0.20f+0.02f*t;spine=0.30f;process=0.0f;}
        else if(k<33) {const float t=static_cast<float>(k-21)/11.0f;spacing=0.32f;radius=0.22f-0.04f*t;spine=0.36f-0.08f*t;process=0.42f-0.10f*t;}
        else {const float t=static_cast<float>(k-33)/10.0f;spacing=0.28f-0.14f*t;radius=0.17f-0.12f*t;spine=0.18f*(1.0f-t);process=0.0f;}
        float a=along,c=SpineAcross(along),turn=0.0f;
        if(k>=VertebraCount-3) {
            const float loose=static_cast<float>(k-(VertebraCount-4));
            a+=0.35f*loose;c+=0.55f+0.45f*loose;turn=0.9f*loose;
        }
        const Vector3 centre=bones.At(a,c,radius*0.40f);
        const Vector3 ahead=bones.At(a+0.1f,SpineAcross(a+0.1f)+(c-SpineAcross(a)),radius*0.40f);
        Vector3 axis=ahead-centre;axis.Y=0.0f;
        if(axis.LengthSquared()<1e-8f) axis=Vector3(placement.dirX,0,placement.dirZ);
        axis.Normalize();
        if(turn!=0.0f) axis=Vector3(axis.X*std::cos(turn)-axis.Z*std::sin(turn),0.0f,axis.X*std::sin(turn)+axis.Z*std::cos(turn));
        const Vector3 side(axis.Z,0.0f,-axis.X);
        const float length=k<7?0.09f:0.78f*spacing;
        const Vector3 shade=Bone*(0.90f+0.10f*Hash(static_cast<std::uint32_t>(k)*7919U));
        // the centrum, waisted a little between its rims, its end plates darker
        bones.Tube({centre-axis*(length*0.5f),centre-axis*(length*0.2f),centre+axis*(length*0.2f),centre+axis*(length*0.5f)},
                   {radius,radius*0.92f,radius*0.92f,radius},10,shade);
        for(const float end:{-1.0f,1.0f}) {
            const Vector3 plate=centre+axis*(end*length*0.505f);
            bones.inside=centre;
            for(int k=0;k<10;++k) {
                const float a0=2.0f*Pi*static_cast<float>(k)/10.0f,a1=2.0f*Pi*static_cast<float>(k+1)/10.0f;
                bones.Face(plate,plate+side*(std::cos(a0)*radius*0.8f)+Vector3::Up*(std::sin(a0)*radius*0.8f),
                           plate+side*(std::cos(a1)*radius*0.8f)+Vector3::Up*(std::sin(a1)*radius*0.8f),shade*0.86f);
            }
        }
        if(spine>0.02f) {
            // the neural spine, leaning back toward the tail
            Vector3 rise=Vector3::Up-axis*0.45f;rise.Normalize();
            bones.Plate(centre+Vector3::Up*(radius*0.85f)+rise*(spine*0.5f),side,rise,axis,0.022f,spine*0.5f,0.22f*spacing,shade);
        }
        if(process>0.02f) {
            for(const float s:{-1.0f,1.0f}) {
                Vector3 out=side*s-Vector3::Up*0.18f;out.Normalize();
                bones.Plate(centre+out*(radius+process*0.5f),out,Vector3::Up,axis,process*0.5f,0.02f,0.24f*spacing,shade);
            }
        }
        // The ribs from the thoracic vertebrae: splayed out over the sand either side, the
        // longest mid-chest, swept back a little; some fallen flat and lying askew, two gone.
        if(k>=7&&k<7+RibPairs) {
            const int i=k-7;
            for(const float s:{-1.0f,1.0f}) {
                if((i==3&&s<0.0f)||(i==10&&s>0.0f)) continue;
                const float h=Hash(static_cast<std::uint32_t>(i*2+(s>0.0f?1:0))*104729U+17U);
                const bool fallen=h>0.72f;
                // a few snapped part way down, the outer piece lying in the sand below the break
                const bool broken=!fallen&&h>0.58f;
                const float ribLength=1.35f+0.95f*std::sin(Pi*(static_cast<float>(i)+0.5f)/RibPairs);
                const auto ribPoint=[&](const float t,const bool lying) {
                    float lateral,lift,sweep;
                    if(fallen) {
                        lateral=0.30f+ribLength*0.85f*t;lift=0.035f;sweep=(h-0.86f)*3.0f*t;
                    } else if(lying) {
                        lateral=0.22f+ribLength*0.82f*std::sin(t*Pi*0.5f)+0.18f;lift=0.03f;sweep=0.30f*t+0.12f;
                    } else {
                        lateral=0.22f+ribLength*0.82f*std::sin(t*Pi*0.5f);
                        lift=0.32f+0.50f*std::sin(t*Pi)*(1.0f-0.55f*t)-0.36f*t;
                        sweep=0.30f*t;
                    }
                    return bones.At(along+sweep,SpineAcross(along)+s*lateral,lift);
                };
                const Vector3 shade=Bone*(0.92f+0.08f*h);
                std::vector<Vector3> points;std::vector<float> radii;
                const float end=broken?0.52f:1.0f;
                for(int n=0;n<=9;++n) {
                    const float t=end*static_cast<float>(n)/9.0f;
                    points.push_back(ribPoint(t,false));
                    radii.push_back(0.055f*(1.0f-0.45f*t));
                }
                bones.Tube(points,radii,6,shade,0.65f);
                if(broken) {
                    points.clear();radii.clear();
                    for(int n=0;n<=5;++n) {
                        const float t=0.58f+0.42f*static_cast<float>(n)/5.0f;
                        points.push_back(ribPoint(t,true));
                        radii.push_back(0.055f*(1.0f-0.45f*t));
                    }
                    bones.Tube(points,radii,6,shade*0.97f,0.65f);
                }
                ++ribs_;
            }
        }
        along+=spacing;
    }
    // The shoulder blades: broad fans lying on the sand either side of the chest.
    for(const float s:{-1.0f,1.0f}) {
        const float neckA=1.35f,neckC=SpineAcross(1.35f)+s*0.95f;
        const Vector3 neck=bones.At(neckA,neckC,0.07f);
        constexpr int Rim=7;
        std::vector<Vector3> rim,low;
        for(int n=0;n<=Rim;++n) {
            const float a=(-55.0f+110.0f*static_cast<float>(n)/Rim)*Pi/180.0f;
            const float ra=std::sin(a)*0.85f,rc=s*std::cos(a)*0.85f;
            rim.push_back(bones.At(neckA+ra,neckC+rc,0.035f));
            low.push_back(bones.At(neckA+ra,neckC+rc,-0.02f));
        }
        for(int n=0;n<Rim;++n) {
            const auto a0=static_cast<std::size_t>(n),a1=a0+1U;
            bones.inside=neck-Vector3::Up*0.5f;
            bones.Face(neck,rim[a0],rim[a1],Bone*0.97f);
            bones.inside=neck-Vector3::Up*0.05f;
            bones.Quad(rim[a0],low[a0],low[a1],rim[a1],Bone*0.93f);
        }
    }
    // A flipper's bones by the seaward blade: the humerus, the radius and ulna, four fingers.
    {
        const auto bone=[&](const float a0,const float c0,const float a1,const float c1,const float r) {
            bones.Tube({bones.At(a0,c0,r*0.6f),bones.At(a1,c1,r*0.6f)},{r,r*0.85f},7,Bone*0.95f);
        };
        bone(1.9f,2.30f,2.20f,2.65f,0.085f);
        bone(2.25f,2.72f,2.52f,3.12f,0.045f);
        bone(2.12f,2.78f,2.36f,3.18f,0.045f);
        for(int finger=0;finger<4;++finger) {
            float a=2.44f+0.06f*static_cast<float>(finger),c=3.18f+0.03f*static_cast<float>(finger);
            const float heading=0.15f+0.32f*static_cast<float>(finger);
            for(int joint=0;joint<3;++joint) {
                const float length=0.13f-0.02f*static_cast<float>(joint);
                const float na=a+std::sin(heading)*length,nc=c+std::cos(heading)*length;
                bone(a,c,na,nc,0.026f-0.004f*static_cast<float>(joint));
                a=na+std::sin(heading)*0.03f;c=nc+std::cos(heading)*0.03f;   // a little gap at each joint
            }
        }
    }

    auto& mesh=bones.builder.mesh;
    triangles_=static_cast<int>(mesh.size()/3);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(0.2f,0.2f,0.2f),high+Vector3(0.2f,0.2f,0.2f));
    centre_=(low+high)*0.5f;
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"whale-skeleton",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: whale skeleton sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a stranded whale's bleached skeleton lies half sunk in the beach sand at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<"): skull and jaws, "<<VertebraCount<<" vertebrae, "<<ribs_<<" ribs, shoulder blades and a flipper; "
        <<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void WhaleSkeletonRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>300*300
        ||BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
    else {
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);drawCalls_=1;
}

void WhaleSkeletonRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"whale-skeleton");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: whale skeleton sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    map.CountCasters(1);
}
}
