#include "IslandSimulator/HumanSkeleton.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>
using Microsoft::Xna::Framework::Vector3;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
const Vector3 Up(0,1,0);
Vector3 Unit(Vector3 v) {v.Normalize();return v;}
Vector3 Floored(Vector3 v,const float floorY,const float lift) {v.Y=std::max(v.Y,floorY+lift);return v;}
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

SkeletonPose LyingSkeleton(const Vector3& o,const Vector3& heading,const bool prone,const float headTurn) {
    SkeletonPose p;
    const Vector3 h=Unit(Vector3(heading.X,0.0f,heading.Z));
    p.front=prone?-Up:Up;
    const Vector3 right=Unit(Vector3::Cross(p.front,h));
    const float floor=o.Y;
    p.pelvis=o+Up*0.10f;p.chest=o+h*0.47f+Up*0.12f;p.neck=o+h*0.58f+Up*0.10f;
    p.head=o+h*0.70f+Up*0.085f+right*0.02f;
    p.face=Unit(p.front*std::cos(headTurn)+right*std::sin(headTurn));p.headUp=h;
    for(int i=0;i<2;++i) {
        const float s=i==0?-1.0f:1.0f;
        const Vector3 shoulder=p.chest-h*0.03f+right*(s*0.19f);
        p.elbow[i]=Floored(shoulder-h*0.29f+right*(s*0.07f),floor,0.035f);p.elbow[i].Y=floor+0.035f;
        p.wrist[i]=p.elbow[i]-h*0.25f+right*(s*0.05f);p.wrist[i].Y=floor+0.03f;
        p.fingers[i]=Unit(-h+right*(s*0.25f));
        const Vector3 hip=p.pelvis-h*0.02f+right*(s*0.09f);
        p.knee[i]=hip-h*0.44f+right*(s*0.04f);p.knee[i].Y=floor+0.045f;
        p.ankle[i]=p.knee[i]-h*0.41f+right*(s*0.07f);p.ankle[i].Y=floor+0.045f;
        p.toes[i]=prone?Unit(-h+right*(s*0.2f)):Unit(Up*0.55f-h*0.3f+right*(s*0.6f));
    }
    p.jaw=prone?0.1f:0.5f;
    return p;
}

SkeletonPose SittingSkeleton(const Vector3& o,const Vector3& intoRoom) {
    SkeletonPose p;
    const Vector3 n=Unit(Vector3(intoRoom.X,0.0f,intoRoom.Z));
    const Vector3 right=Unit(Vector3::Cross(n,Up));
    const float floor=o.Y;
    p.front=n;
    p.pelvis=o+n*0.13f+Up*0.11f;
    const Vector3 back=Unit(Up*0.9f-n*0.38f);
    p.chest=p.pelvis+back*0.46f;p.neck=p.chest+back*0.09f;
    // the head fallen forward onto the chest and to one side
    p.head=p.neck+Up*0.04f+n*0.09f+right*0.06f;
    p.face=Unit(n*0.75f-Up*0.6f+right*0.25f);p.headUp=Unit(Up*0.45f+n*0.6f+right*0.35f);
    for(int i=0;i<2;++i) {
        const float s=i==0?-1.0f:1.0f;
        const Vector3 shoulder=p.chest-back*0.03f+right*(s*0.19f);
        p.elbow[i]=shoulder-Up*0.27f+n*0.05f+right*(s*0.04f);
        p.wrist[i]=Floored(p.elbow[i]-Up*0.13f+n*0.17f+right*(s*0.02f),floor,0.03f);
        p.fingers[i]=Unit(n*0.6f-Up*0.4f+right*(s*0.2f));
        const Vector3 hip=p.pelvis+right*(s*0.09f)+n*0.02f;
        p.knee[i]=hip+n*0.40f+Up*(i==0?0.16f:0.07f)+right*(s*0.06f);
        p.ankle[i]=Floored(p.knee[i]+n*0.38f-Up*0.25f+right*(s*0.08f),floor,0.05f);
        p.toes[i]=Unit(Up*0.65f+n*0.3f+right*(s*0.45f));
    }
    p.jaw=0.65f;
    return p;
}

SkeletonPose SlumpedSkeleton(const Vector3& seat,const Vector3& towardTable,const float floorY,const float tableY) {
    SkeletonPose p;
    const Vector3 t=Unit(Vector3(towardTable.X,0.0f,towardTable.Z));
    const Vector3 right=Unit(Vector3::Cross(t,Up));
    p.pelvis=seat+Up*0.10f;
    const Vector3 lean=Unit(Up*0.75f+t*0.65f);
    p.front=Unit(t*0.75f-Up*0.65f);
    p.chest=p.pelvis+lean*0.46f;p.neck=p.chest+lean*0.08f;
    // the head fallen forward onto the table, cheek to the boards
    p.head=p.neck+t*0.24f;p.head.Y=tableY+0.08f;
    p.face=Unit(right*0.85f-Up*0.3f+t*0.25f);p.headUp=t;
    for(int i=0;i<2;++i) {
        const float s=i==0?-1.0f:1.0f;
        const Vector3 shoulder=p.chest+right*(s*0.19f);
        p.elbow[i]=shoulder+t*0.22f+right*(s*0.08f);p.elbow[i].Y=tableY+0.03f;
        p.wrist[i]=p.elbow[i]+t*0.20f-right*(s*0.10f);p.wrist[i].Y=tableY+0.025f;
        p.fingers[i]=Unit(t*0.3f-right*(s*0.7f));
        const Vector3 hip=p.pelvis+right*(s*0.09f);
        p.knee[i]=hip+t*0.42f+right*(s*0.05f);p.knee[i].Y=seat.Y+0.09f;
        p.ankle[i]=p.knee[i]+t*0.06f;p.ankle[i].Y=floorY+0.07f;
        p.toes[i]=Unit(t+right*(s*0.25f));
    }
    p.jaw=0.15f;
    return p;
}

SkeletonPose CurledSkeleton(const Vector3& o,const Vector3& heading) {
    SkeletonPose p;
    const Vector3 h=Unit(Vector3(heading.X,0.0f,heading.Z));
    p.front=Unit(Vector3::Cross(Up,h));
    const float floor=o.Y;
    p.pelvis=o+Up*0.12f;
    p.chest=o+h*0.42f+p.front*0.10f+Up*0.13f;
    p.neck=p.chest+h*0.10f+p.front*0.05f;
    p.head=p.neck+h*0.11f+p.front*0.07f;p.head.Y=floor+0.08f;
    p.face=Unit(p.front*0.8f-h*0.15f-Up*0.35f);p.headUp=h;
    for(int i=0;i<2;++i) {
        const float lift=i==0?0.17f:0.03f;   // the left side uppermost
        const Vector3 shoulder=p.chest-h*0.03f;
        p.elbow[i]=Floored(shoulder+p.front*0.25f-h*0.08f+Up*(lift-0.13f),floor,0.035f);
        p.wrist[i]=Floored(p.elbow[i]+p.front*0.05f+h*0.19f,floor,0.03f);
        p.fingers[i]=Unit(h*0.6f+p.front*0.4f);
        const Vector3 hip=p.pelvis+Up*(lift-0.10f);
        p.knee[i]=Floored(hip+p.front*0.38f+h*0.12f,floor,0.045f);
        p.ankle[i]=Floored(p.knee[i]-h*0.38f+p.front*0.05f,floor,0.045f);
        p.toes[i]=Unit(-h*0.7f+p.front*0.3f);
    }
    p.jaw=0.25f;
    return p;
}

int BuildSkeleton(SculptMeshBuilder& b,const SkeletonPose& pose,const float shade,const std::uint32_t seed) {
    const std::size_t before=b.builder.mesh.size();
    const Vector3 bone=Vector3(240,226,194)*shade,knuckle=Vector3(206,188,150)*shade,socket=Vector3(38,30,24)*shade,
        teeth=Vector3(248,240,214)*shade,rag=Vector3(60,68,90)*shade,leather=Vector3(70,50,36)*shade,wool=Vector3(46,50,66)*shade;
    const float tone=0.92f+0.08f*Hash(seed);
    const auto longBone=[&](const Vector3& a,const Vector3& c,const float end,const float shaft,const Vector3& pigment) {
        const Vector3 d=c-a;
        b.Tube({a,a+d*0.12f,c-d*0.12f,c},{end,shaft,shaft,end*0.9f},6,pigment*tone);
    };
    // the torso's frame
    const Vector3 rise=Unit(pose.chest-pose.pelvis);
    const Vector3 right=Unit(Vector3::Cross(pose.front,rise));
    const Vector3 front=Unit(Vector3::Cross(rise,right));
    // the spine: lumbar, thoracic and cervical vertebrae, each a body and its spinous process
    {
        const Vector3 skullBase=pose.head-Unit(pose.headUp)*0.07f-Unit(pose.face)*0.01f;
        const auto along=[&](const float t) {   // 0 sacrum .. 1 the skull's base
            if(t<0.75f) {
                const float u=t/0.75f;
                return Vector3::Lerp(pose.pelvis+rise*0.04f,pose.chest,u)-front*(0.035f+0.015f*std::sin(u*Pi));
            }
            const float u=(t-0.75f)/0.25f;
            return Vector3::Lerp(Vector3::Lerp(pose.chest,pose.neck,u),Vector3::Lerp(pose.neck,skullBase,u),u)-front*0.02f;
        };
        constexpr int Count=22;
        for(int k=0;k<Count;++k) {
            const float t=(static_cast<float>(k)+0.5f)/static_cast<float>(Count);
            const Vector3 c=along(t);
            const Vector3 axis=Unit(along(std::min(t+0.02f,1.0f))-along(std::max(t-0.02f,0.0f)));
            const Vector3 side=Unit(Vector3::Cross(front,axis)),ahead=Unit(Vector3::Cross(axis,side));
            const float size=t<0.25f?1.25f:(t<0.75f?1.0f:0.75f);
            b.Box(c,side,axis,ahead,0.024f*size,0.011f,0.019f*size,bone*tone,bone*tone);
            b.Blade(c-ahead*0.015f,axis,-ahead,side,0.005f,0.035f*size,0.009f,0.004f,knuckle*tone);
        }
        // the sacrum
        b.Box(pose.pelvis-front*0.04f,right,rise,front,0.045f,0.055f,0.018f,knuckle*tone,bone*tone);
    }
    // the ribs, the sternum, the collarbones and the shoulder blades
    {
        const float chestDepth=0.17f;
        for(int i=0;i<10;++i) {
            const float f=static_cast<float>(i)/9.0f;
            const Vector3 origin=Vector3::Lerp(pose.chest-rise*0.03f,pose.chest-rise*0.30f,f)-front*0.035f;
            const float width=0.10f+0.05f*std::sin(Pi*(static_cast<float>(i)+1.5f)/11.0f);
            const float reach=i>=8?1.7f:2.5f;
            for(const float s:{-1.0f,1.0f}) {
                if(Hash(seed^static_cast<std::uint32_t>(i*7+(s>0?3:5)))<0.08f) continue;   // one gone here and there
                std::vector<Vector3> points;std::vector<float> radii;
                for(int k=0;k<=5;++k) {
                    const float q=reach*static_cast<float>(k)/5.0f;
                    points.push_back(origin+right*(s*(width*std::sin(std::min(q,Pi*0.5f+0.4f))+0.015f))
                        +front*(chestDepth*0.5f*(1.0f-std::cos(q)))-rise*(0.07f*q/2.5f));
                    radii.push_back(0.007f);
                }
                b.Tube(points,radii,4,bone*tone);
            }
        }
        b.Box(pose.chest-rise*0.13f+front*(chestDepth*0.86f),right,rise,front,0.018f,0.085f,0.007f,bone*tone,bone*tone);
        const Vector3 notch=pose.chest-rise*0.02f+front*(chestDepth*0.8f);
        for(const float s:{-1.0f,1.0f}) {
            const Vector3 shoulder=pose.chest+right*(s*0.19f)-rise*0.03f;
            b.Tube({notch+right*(s*0.02f),Vector3::Lerp(notch,shoulder,0.5f)+front*0.02f+rise*0.01f,shoulder+front*0.02f},{0.009f,0.008f,0.01f},5,bone*tone);
            const Vector3 a=shoulder-front*0.05f,c=shoulder-rise*0.16f-right*(s*0.03f)-front*0.07f,d=shoulder-rise*0.05f-right*(s*0.11f)-front*0.08f;
            const Vector3 centre=(a+c+d)/3.0f;
            b.inside=centre+front*0.05f;b.Face(a,c,d,bone*tone*0.95f);
            b.inside=centre-front*0.05f;b.Face(a+front*0.006f,c+front*0.006f,d+front*0.006f,bone*tone*0.95f);
        }
    }
    // the pelvis: the iliac wings, the hip sockets, the pubic arch
    {
        for(const float s:{-1.0f,1.0f}) {
            const Vector3 joint=pose.pelvis+right*(s*0.05f)-front*0.035f;
            const Vector3 crest=pose.pelvis+right*(s*0.15f)+rise*0.10f+front*0.01f;
            const Vector3 back=pose.pelvis+right*(s*0.09f)+rise*0.12f-front*0.045f;
            const Vector3 socket=pose.pelvis+right*(s*0.09f)-rise*0.03f+front*0.02f;
            const Vector3 centre=(joint+crest+back+socket)/4.0f;
            b.inside=centre+right*(s*0.05f);b.Quad(joint,back,crest,socket,bone*tone);
            b.inside=centre-right*(s*0.05f);
            b.Quad(joint-right*(s*0.008f),socket-right*(s*0.008f),crest-right*(s*0.008f),back-right*(s*0.008f),knuckle*tone);
        }
        b.Tube({pose.pelvis+right*0.085f+front*0.05f-rise*0.04f,pose.pelvis+front*0.075f-rise*0.07f,pose.pelvis-right*0.085f+front*0.05f-rise*0.04f},
               {0.012f,0.014f,0.012f},5,bone*tone);
    }
    // the arms and the hands
    for(int i=0;i<2;++i) {
        const float s=i==0?-1.0f:1.0f;
        const Vector3 shoulder=pose.chest+right*(s*0.19f)-rise*0.03f;
        longBone(shoulder,pose.elbow[i],0.026f,0.014f,bone);
        Vector3 forearm=pose.wrist[i]-pose.elbow[i];forearm.Normalize();
        Vector3 offset=Vector3::Cross(forearm,Up);
        if(offset.LengthSquared()<1e-4f) offset=right;
        offset.Normalize();
        longBone(pose.elbow[i]+offset*0.012f,pose.wrist[i]+offset*0.014f,0.013f,0.008f,bone);
        longBone(pose.elbow[i]-offset*0.01f,pose.wrist[i]-offset*0.012f,0.012f,0.007f,bone);
        const Vector3 dir=Unit(pose.fingers[i]);
        Vector3 spread=Vector3::Cross(dir,Up);
        if(spread.LengthSquared()<1e-4f) spread=right;
        spread.Normalize();
        const Vector3 palm=pose.wrist[i]+dir*0.03f;
        b.Box(palm,spread,Up,dir,0.024f,0.009f,0.018f,knuckle*tone,knuckle*tone);
        for(int k=0;k<5;++k) {
            const float lateral=(static_cast<float>(k)-2.0f)*0.011f;
            const Vector3 base=palm+dir*0.015f+spread*lateral;
            const float length=k==0?0.07f:(k==2?0.13f:0.11f);
            const Vector3 bend=dir+(k==0?spread*(-0.8f*s):Vector3(0,0,0));
            b.Tube({base,base+Unit(bend)*(length*0.55f),base+Unit(bend)*length-Up*0.006f},{0.0045f,0.004f,0.0035f},4,bone*tone);
        }
    }
    // the legs, the trousers' rags and the feet: bare bones or still in their boots
    for(int i=0;i<2;++i) {
        const float s=i==0?-1.0f:1.0f;
        const Vector3 hip=pose.pelvis+right*(s*0.09f)-rise*0.02f;
        longBone(hip,pose.knee[i],0.034f,0.018f,bone);
        Vector3 shin=pose.ankle[i]-pose.knee[i];shin.Normalize();
        Vector3 outward=Vector3::Cross(shin,Up);
        if(outward.LengthSquared()<1e-4f) outward=right;
        outward.Normalize();
        if(Vector3::Dot(outward,right*s)<0.0f) outward=-outward;
        longBone(pose.knee[i],pose.ankle[i],0.03f,0.016f,bone);
        longBone(pose.knee[i]+outward*0.028f-shin*0.03f,pose.ankle[i]+outward*0.026f,0.009f,0.006f,bone);
        b.Box(pose.knee[i]+Unit(Vector3::Cross(outward,shin))*(-0.028f),outward,shin,Unit(Vector3::Cross(outward,shin)),0.02f,0.022f,0.008f,knuckle*tone,knuckle*tone);
        if(pose.trousers) {
            // what the sun and the rats left of the trousers: the cloth fallen in round the thigh
            // bone, torn short above the knee
            const Vector3 d=pose.knee[i]-hip;
            b.Tube({hip+d*0.05f,hip+d*0.5f,hip+d*(0.72f+0.15f*Hash(seed^static_cast<std::uint32_t>(i+11)))},{0.055f,0.048f,0.044f},7,rag*tone,0.6f);
        }
        const Vector3 toe=Unit(pose.toes[i]);
        Vector3 width=Vector3::Cross(toe,Up);
        if(width.LengthSquared()<1e-4f) width=right;
        width.Normalize();
        const Vector3 sole=Unit(Vector3::Cross(toe,width));
        if(pose.boots) {
            const Vector3 heel=pose.ankle[i]-toe*0.05f;
            b.Tube({pose.ankle[i]+(-shin)*0.17f,pose.ankle[i]-toe*0.01f},{0.055f,0.06f},8,leather*tone);
            b.Box(heel+toe*0.12f-sole*0.0f,width,sole,toe,0.048f,0.045f,0.15f,leather*tone,leather*tone*1.1f);
        } else {
            b.Box(pose.ankle[i]+toe*0.02f,width,sole,toe,0.026f,0.022f,0.05f,knuckle*tone,bone*tone);
            for(int k=0;k<5;++k) {
                const float lateral=(static_cast<float>(k)-2.0f)*0.012f;
                const Vector3 base=pose.ankle[i]+toe*0.06f+width*lateral;
                b.Tube({base,base+toe*(0.13f-0.01f*std::abs(static_cast<float>(k)-1.0f))},{0.006f,0.004f},4,bone*tone);
            }
        }
    }
    // the skull: the cranium, the cheekbones and the upper jaw with its teeth, the sockets and
    // the nose's opening, the lower jaw dropped open
    {
        const Vector3 fz=Unit(pose.face);
        Vector3 hu=pose.headUp-fz*Vector3::Dot(pose.headUp,fz);hu.Normalize();
        const Vector3 hr=Unit(Vector3::Cross(fz,hu));
        const Vector3 c=pose.head+hu*0.02f-fz*0.02f;
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(int j=1;j<=7;++j) {
            const float lat=0.64f*Pi*static_cast<float>(j)/7.0f;
            std::vector<Vector3> ring;
            for(int k=0;k<12;++k) {
                const float q=2.0f*Pi*static_cast<float>(k)/12.0f;
                ring.push_back(c+hu*(0.085f*std::cos(lat))+(fz*(0.096f*std::cos(q))+hr*(0.074f*std::sin(q)))*std::sin(lat));
            }
            rings.push_back(ring);centres.push_back(c+hu*(0.085f*std::cos(lat)*0.8f));
        }
        {
            // the crown closed by a fan from the top
            const Vector3 top=c+hu*0.085f;
            b.inside=c;
            for(int k=0;k<12;++k) b.Face(top,rings.front()[static_cast<std::size_t>(k)],rings.front()[static_cast<std::size_t>((k+1)%12)],bone*tone);
        }
        b.Loft(rings,centres,[&](std::size_t,std::size_t) {return bone*tone;},false,true);
        b.Box(c+fz*0.068f-hu*0.062f,hr,hu,fz,0.044f,0.03f,0.026f,bone*tone,bone*tone);
        for(const float s:{-1.0f,1.0f})
            b.Box(c+fz*0.058f+hr*(s*0.058f)-hu*0.025f,hr,hu,fz,0.014f,0.012f,0.026f,bone*tone,bone*tone);
        const auto socketDisc=[&](const Vector3& centre,const float rx,const float ry,const int sides) {
            b.inside=centre-fz*0.02f;
            for(int k=0;k<sides;++k) {
                const float q0=2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides),q1=2.0f*Pi*static_cast<float>(k+1)/static_cast<float>(sides);
                b.Face(centre,centre+hr*(rx*std::cos(q0))+hu*(ry*std::sin(q0)),centre+hr*(rx*std::cos(q1))+hu*(ry*std::sin(q1)),socket);
            }
        };
        for(const float s:{-1.0f,1.0f}) socketDisc(c+fz*0.09f+hr*(s*0.032f)-hu*0.008f,0.02f,0.018f,8);
        socketDisc(c+fz*0.096f-hu*0.04f,0.01f,0.016f,5);
        for(int k=0;k<8;++k) {
            const float lateral=(static_cast<float>(k)-3.5f)*0.0085f;
            b.Box(c+fz*(0.093f-std::abs(lateral)*0.5f)+hr*lateral-hu*0.094f,hr,hu,fz,0.0036f,0.0065f,0.004f,teeth,teeth);
        }
        // the lower jaw, hinged at the condyles in front of the ears
        const auto drop=[&](const Vector3& v) {return v*std::cos(pose.jaw)-(hu*Vector3::Dot(v,fz)+fz*Vector3::Dot(v,-hu))*std::sin(pose.jaw)
                                                  +hr*Vector3::Dot(v,hr)*(1.0f-std::cos(pose.jaw));};
        const Vector3 hinge=c-hu*0.045f+fz*0.005f;
        const auto jawPoint=[&](const float lateral,const float forward,const float down) {
            return hinge+drop(hr*lateral+fz*forward-hu*down);
        };
        b.Tube({jawPoint(-0.058f,0.0f,0.0f),jawPoint(-0.055f,0.012f,0.055f),jawPoint(-0.035f,0.07f,0.07f),jawPoint(0.0f,0.09f,0.075f),
                jawPoint(0.035f,0.07f,0.07f),jawPoint(0.055f,0.012f,0.055f),jawPoint(0.058f,0.0f,0.0f)},
               {0.009f,0.012f,0.012f,0.014f,0.012f,0.012f,0.009f},5,bone*tone);
        for(int k=0;k<6;++k) {
            const float lateral=(static_cast<float>(k)-2.5f)*0.009f;
            const Vector3 t=jawPoint(lateral,0.084f-std::abs(lateral)*0.6f,0.06f);
            b.Box(t,hr,hu,fz,0.0035f,0.006f,0.004f,teeth,teeth);
        }
        if(pose.cap) {
            // a knitted watch cap, still on the skull
            std::vector<std::vector<Vector3>> capRings;std::vector<Vector3> capCentres;
            for(int j=0;j<4;++j) {
                const float lat=0.22f*Pi+0.28f*Pi*static_cast<float>(j)/3.0f;
                std::vector<Vector3> ring;
                for(int k=0;k<12;++k) {
                    const float q=2.0f*Pi*static_cast<float>(k)/12.0f;
                    ring.push_back(c+hu*(0.092f*std::cos(lat))+(fz*(0.103f*std::cos(q))+hr*(0.081f*std::sin(q)))*std::sin(lat));
                }
                capRings.push_back(ring);capCentres.push_back(c+hu*(0.092f*std::cos(lat)));
            }
            std::reverse(capRings.begin(),capRings.end());std::reverse(capCentres.begin(),capCentres.end());
            capCentres.back()=c+hu*0.097f;
            b.Loft(capRings,capCentres,[&](std::size_t,std::size_t) {return wool*tone;},false,true);
        }
    }
    return static_cast<int>((b.builder.mesh.size()-before)/3U);
}
}
