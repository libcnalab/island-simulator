#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/BirdRenderer.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"

#include "IslandSimulator/RuntimeDiagnostics.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string_view>

#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr float NearLodDistance = 255.0f;
        // Past this a bird is a few pixels long: six sides round the body and head, no eyes
        // and no feet (sixteen sides and every detail out to 255 m was the birds' pass cost).
        constexpr float DetailLodDistance = 35.0f;
        constexpr int MaximumVerticesPerBird = 4200;
        constexpr int MaximumVertexCount = static_cast<int>(BirdFlock::BirdCount)
            * MaximumVerticesPerBird;
        constexpr std::string_view NearBirdFeatherDiagnostic =
            "island-simulator: near birds use layered trailing flight-feather silhouettes";
        constexpr std::string_view BirdPerchSilhouetteDiagnostic =
            "island-simulator: perched birds fold wings and add near feet silhouettes";
        constexpr std::string_view NearBirdHeadDiagnostic =
            "island-simulator: near birds use closed lofted bodies, multi-ring heads, fitted beaks, and lateral eyes";
        constexpr std::string_view TropicalBirdDiagnostic =
            "island-simulator: canopy flock includes hooked-beak long-tailed parrots and compact tropical fruit doves";
        constexpr std::string_view NearBirdProfileDiagnostic =
            "island-simulator: near birds use compact rounded heads, camera-facing six-facet eyes, fitted hooked beaks, solid swept wings, and split tail feathers";
        constexpr std::string_view GroundDoveAnatomyDiagnostic =
            "island-simulator: ground fruit doves use plump bodies, rounded folded wings, fitted eye rings, and solid planted toes";

        struct BirdPalette
        {
            Color body;
            Color upperWing;
            Color lowerWing;
            Color tail;
            Color head;
            Color beak;
            Color eye;
            Color flight;   // the flight feathers' outer vanes and tips
        };

        const std::array<BirdPalette, 3> Palettes = {{
            {Color(61, 70, 70, 255), Color(104, 113, 108, 255),
             Color(214, 218, 203, 255), Color(48, 57, 58, 255),
             Color(161, 168, 164, 255), Color(194, 145, 57, 255), Color(15, 14, 12, 255), Color(34, 36, 38, 255)},
            {Color(90, 72, 54, 255), Color(139, 117, 82, 255),
             Color(222, 204, 161, 255), Color(67, 56, 46, 255),
             Color(130, 105, 73, 255), Color(184, 127, 47, 255), Color(14, 12, 9, 255), Color(58, 46, 36, 255)},
            {Color(50, 62, 72, 255), Color(78, 99, 114, 255),
             Color(195, 211, 215, 255), Color(39, 50, 59, 255),
             Color(104, 124, 133, 255), Color(177, 122, 45, 255), Color(12, 13, 14, 255), Color(30, 36, 44, 255)}
        }};

        const std::array<BirdPalette, 2> ParrotPalettes = {{
            {Color(48, 130, 58, 255), Color(28, 96, 48, 255),
             Color(210, 176, 40, 255), Color(26, 82, 47, 255),
             Color(196, 54, 43, 255), Color(203, 187, 145, 255), Color(13, 12, 9, 255), Color(34, 72, 150, 255)},
            {Color(57, 140, 65, 255), Color(35, 92, 118, 255),
             Color(218, 182, 46, 255), Color(27, 84, 82, 255),
             Color(224, 174, 43, 255), Color(59, 53, 47, 255), Color(12, 11, 8, 255), Color(30, 70, 140, 255)}
        }};

        // Fruit-dove greens, and a pigeon grey with a rose breast and iridescent neck.
        const std::array<BirdPalette, 3> DovePalettes = {{
            {Color(91, 137, 84, 255), Color(48, 102, 62, 255),
             Color(181, 191, 150, 255), Color(45, 88, 59, 255),
             Color(158, 86, 128, 255), Color(78, 70, 64, 255), Color(43, 20, 18, 255), Color(40, 70, 58, 255)},
            {Color(99, 143, 94, 255), Color(54, 108, 69, 255),
             Color(201, 196, 163, 255), Color(49, 92, 65, 255),
             Color(111, 126, 144, 255), Color(83, 72, 62, 255), Color(47, 21, 17, 255), Color(44, 74, 62, 255)},
            {Color(176, 132, 142, 255), Color(118, 122, 132, 255),
             Color(206, 206, 202, 255), Color(72, 74, 84, 255),
             Color(96, 148, 128, 255), Color(86, 76, 70, 255), Color(40, 22, 20, 255), Color(52, 54, 64, 255)}
        }};

        // Glossy black, the wings' undersides a shade browner, the bill a horn grey.
        const BirdPalette FrigatePalette{Color(24, 23, 27, 255), Color(30, 28, 33, 255),
            Color(46, 41, 40, 255), Color(22, 21, 25, 255), Color(26, 25, 30, 255),
            Color(146, 146, 140, 255), Color(10, 10, 10, 255), Color(18, 17, 20, 255)};

        [[nodiscard]] const BirdPalette& PaletteForBird(const BirdState& bird)
        {
            if (bird.species == BirdSpecies::Frigatebird) return FrigatePalette;
            if (bird.species == BirdSpecies::Parrot) {
                return ParrotPalettes[bird.colorVariant % ParrotPalettes.size()];
            }
            if (bird.species == BirdSpecies::FruitDove) {
                return DovePalettes[bird.colorVariant % DovePalettes.size()];
            }
            return Palettes[bird.colorVariant % Palettes.size()];
        }

        void AddTriangle(std::vector<VertexPositionColor>& vertices,
                         const Vector3& a, const Vector3& b, const Vector3& c,
                         const Color& color)
        {
            vertices.emplace_back(a, color);
            vertices.emplace_back(b, color);
            vertices.emplace_back(c, color);
        }

        void AddDoubleSidedTriangle(std::vector<VertexPositionColor>& vertices,
                                    const Vector3& a, const Vector3& b, const Vector3& c,
                                    const Color& color)
        {
            AddTriangle(vertices, a, b, c, color);
            AddTriangle(vertices, c, b, a, color);
        }

        [[nodiscard]] Color Lit(const Color& base,const Vector3& normal)
        {
            const float light=.62f+.30f*std::max(0.0f,
                Vector3::Dot(normal,wildlife_render::FixedSunDirection))
                +.08f*std::max(0.0f,normal.Y);
            const auto channel=[light](int value) {
                return static_cast<int>(std::clamp(std::lround(value*light),0L,255L));
            };
            return Color(channel(base.getRProperty()),channel(base.getGProperty()),
                channel(base.getBProperty()),static_cast<int>(base.getAProperty()));
        }

        void AppendEllipsoid(std::vector<VertexPositionColor>& vertices,
                             const Vector3& centre,const Vector3& right,
                             const Vector3& up,const Vector3& forward,
                             const float radiusX,const float radiusY,const float radiusZ,
                             const Color& upper,const Color& lower,
                             const int rings=4,const int segments=10)
        {
            constexpr float Pi=3.14159265359f;
            for(int ring=0;ring<rings;++ring) {
                const float v0=-.5f*Pi+Pi*static_cast<float>(ring)/rings;
                const float v1=-.5f*Pi+Pi*static_cast<float>(ring+1)/rings;
                for(int segment=0;segment<segments;++segment) {
                    const float u0=2.0f*Pi*static_cast<float>(segment)/segments;
                    const float u1=2.0f*Pi*static_cast<float>(segment+1)/segments;
                    const auto point=[&](const float u,const float v) {
                        return centre+right*(std::cos(v)*std::cos(u)*radiusX)
                            +up*(std::sin(v)*radiusY)
                            +forward*(std::cos(v)*std::sin(u)*radiusZ);
                    };
                    // shaded at each corner from the surface's own normal, the upper colour
                    // blending into the lower round the middle: no facets
                    const auto shade=[&](const float u,const float v) {
                        Vector3 normal=right*(std::cos(v)*std::cos(u)/radiusX)+up*(std::sin(v)/radiusY)
                            +forward*(std::cos(v)*std::sin(u)/radiusZ);
                        if(normal.LengthSquared()>1e-10f) normal.Normalize(); else normal=up;
                        return Lit(Color::Lerp(lower,upper,std::clamp(.5f+std::sin(v)*1.6f,0.0f,1.0f)),normal);
                    };
                    vertices.emplace_back(point(u0,v0),shade(u0,v0));
                    vertices.emplace_back(point(u0,v1),shade(u0,v1));
                    vertices.emplace_back(point(u1,v1),shade(u1,v1));
                    vertices.emplace_back(point(u0,v0),shade(u0,v0));
                    vertices.emplace_back(point(u1,v1),shade(u1,v1));
                    vertices.emplace_back(point(u1,v0),shade(u1,v0));
                }
            }
        }

        void AppendTaperedSegment(std::vector<VertexPositionColor>& vertices,
                                  const Vector3& start,const Vector3& end,
                                  const float startRadius,const float endRadius,
                                  const Color& color,const int sides=5)
        {
            Vector3 axis=end-start;
            if(axis.LengthSquared()<1.0e-8f)return;
            axis.Normalize();
            Vector3 side=Vector3::Cross(axis,Vector3::Up);
            if(side.LengthSquared()<1.0e-6f)side=Vector3::Right;
            else side.Normalize();
            const Vector3 normal=Vector3::Normalize(Vector3::Cross(side,axis));
            constexpr float Pi=3.14159265359f;
            for(int index=0;index<sides;++index) {
                const float a0=2.0f*Pi*static_cast<float>(index)/sides;
                const float a1=2.0f*Pi*static_cast<float>(index+1)/sides;
                const Vector3 r0=side*std::cos(a0)+normal*std::sin(a0);
                const Vector3 r1=side*std::cos(a1)+normal*std::sin(a1);
                const Vector3 a=start+r0*startRadius,b=start+r1*startRadius;
                const Vector3 c=end+r1*endRadius,d=end+r0*endRadius;
                AddTriangle(vertices,a,b,c,
                    wildlife_render::FacetColor(a,b,c,color,true));
                AddTriangle(vertices,a,c,d,
                    wildlife_render::FacetColor(a,c,d,color,true));
            }
        }

        template<std::size_t Count>
        void AppendClosedSurface(std::vector<VertexPositionColor>& vertices,
                                 const std::array<Vector3, Count>& outline,
                                 const Vector3& halfThickness,
                                 const Color& upper, const Color& lower)
        {
            static_assert(Count>=3U);
            std::array<Vector3, Count> top{}, bottom{};
            for(std::size_t point=0;point<outline.size();++point) {
                top[point]=outline[point]+halfThickness;
                bottom[point]=outline[point]-halfThickness;
            }
            for(std::size_t point=1;point+1<outline.size();++point) {
                AddTriangle(vertices,top[0],top[point],top[point+1],
                    wildlife_render::FacetColor(top[0],top[point],top[point+1],upper,true));
                AddTriangle(vertices,bottom[0],bottom[point+1],bottom[point],
                    wildlife_render::FacetColor(bottom[0],bottom[point+1],bottom[point],lower,true));
            }
            const Color edge=wildlife_render::ScaleColor(upper,.78f);
            for(std::size_t point=0;point<outline.size();++point) {
                const std::size_t next=(point+1U)%outline.size();
                AddTriangle(vertices,top[point],bottom[point],bottom[next],
                    wildlife_render::FacetColor(top[point],bottom[point],bottom[next],edge,true));
                AddTriangle(vertices,top[point],bottom[next],top[next],
                    wildlife_render::FacetColor(top[point],bottom[next],top[next],edge,true));
            }
        }

        // As AppendClosedSurface, each outline point with its own half-thickness and lift along
        // `up`: a wing is a thick rounded bar of bone and coverts at its leading edge, thinning
        // to the feather vanes behind and held at a slight angle to the air, so seen edge-on it
        // shows a tapering blade instead of a flat stick.
        template <std::size_t Count>
        void AppendProfiledSurface(std::vector<VertexPositionColor>& vertices,
                                   const std::array<Vector3, Count>& outline,
                                   const Vector3& up,
                                   const std::array<float, Count>& halfThickness,
                                   const std::array<float, Count>& lift,
                                   const Color& upper, const Color& lower)
        {
            static_assert(Count>=3U);
            std::array<Vector3, Count> top{}, bottom{};
            for(std::size_t point=0;point<outline.size();++point) {
                const Vector3 centreLine=outline[point]+up*lift[point];
                top[point]=centreLine+up*halfThickness[point];
                bottom[point]=centreLine-up*halfThickness[point];
            }
            for(std::size_t point=1;point+1<outline.size();++point) {
                AddTriangle(vertices,top[0],top[point],top[point+1],
                    wildlife_render::FacetColor(top[0],top[point],top[point+1],upper,true));
                AddTriangle(vertices,bottom[0],bottom[point+1],bottom[point],
                    wildlife_render::FacetColor(bottom[0],bottom[point+1],bottom[point],lower,true));
            }
            const Color edge=wildlife_render::ScaleColor(upper,.78f);
            for(std::size_t point=0;point<outline.size();++point) {
                const std::size_t next=(point+1U)%outline.size();
                AddTriangle(vertices,top[point],bottom[point],bottom[next],
                    wildlife_render::FacetColor(top[point],bottom[point],bottom[next],edge,true));
                AddTriangle(vertices,top[point],bottom[next],top[next],
                    wildlife_render::FacetColor(top[point],bottom[next],top[next],edge,true));
            }
        }

        [[nodiscard]] Vector3 LocalPoint(const Vector3& centre, const Vector3& right,
                                         const Vector3& up, const Vector3& forward,
                                         const float localX,
                                         const float localY, const float localZ,
                                         const float scale)
        {
            return centre + right * (localX * scale) + up * (localY * scale)
                + forward * (localZ * scale);
        }

        // One feather: a thin vane `length` along `along` from its root, `width` across at its
        // widest in the plane of `along` and `across`, tapering in at the root and rounding to
        // its tip; the upper face blends from `root` to `tip`, the under face is `under`, both
        // shaded smoothly from `up` (48 vertices). The two faces stand `thickness` apart.
        void AppendFeather(std::vector<VertexPositionColor>& vertices,const Vector3& start,
                           const Vector3& along,const Vector3& across,const Vector3& up,
                           const float length,const float width,const float thickness,
                           const Color& root,const Color& tip,const Color& under)
        {
            constexpr std::array<float,5> At{{0.0f,.16f,.55f,.84f,1.0f}};
            constexpr std::array<float,5> Breadth{{.34f,.86f,1.0f,.78f,.06f}};
            std::array<Vector3,5> left{},right{};
            for(std::size_t i=0;i<At.size();++i) {
                // the outer vane narrower than the inner, as a flight feather's is
                const Vector3 centre=start+along*(length*At[i]);
                left[i]=centre-across*(width*.36f*Breadth[i]);
                right[i]=centre+across*(width*.64f*Breadth[i]);
            }
            for(const float face:{1.0f,-1.0f}) {
                const Vector3 lift=up*(thickness*.5f*face);
                const Vector3 normal=up*face;
                for(std::size_t i=0;i+1<At.size();++i) {
                    const Color c0=face>0.0f?Lit(Color::Lerp(root,tip,At[i]),normal)
                                            :Lit(wildlife_render::ScaleColor(under,1.0f-.18f*At[i]),normal);
                    const Color c1=face>0.0f?Lit(Color::Lerp(root,tip,At[i+1]),normal)
                                            :Lit(wildlife_render::ScaleColor(under,1.0f-.18f*At[i+1]),normal);
                    vertices.emplace_back(left[i]+lift,c0);
                    vertices.emplace_back(right[i]+lift,c0);
                    vertices.emplace_back(right[i+1]+lift,c1);
                    vertices.emplace_back(left[i]+lift,c0);
                    vertices.emplace_back(right[i+1]+lift,c1);
                    vertices.emplace_back(left[i+1]+lift,c1);
                }
            }
        }

        [[nodiscard]] Vector3 Unit(const Vector3& v,const Vector3& fallback)
        {
            return v.LengthSquared()>1e-10f?Vector3::Normalize(v):fallback;
        }

        constexpr int PrimaryCount=7,SecondaryCount=6,CovertCount=6,TailFeatherCount=6;

        // A near wing of feathers (the detail LOD): the arm's coverts as one profiled shield
        // from the shoulder to the wrist, the hand's coverts over the primaries' roots, six
        // secondaries hanging back from the arm and seven primaries fanned from the hand, their
        // tips apart in flight; folded, all of them lie back along the body.
        void AppendFeatheredWing(std::vector<VertexPositionColor>& vertices,const Vector3& centre,
                                 const Vector3& right,const Vector3& up,const Vector3& forward,
                                 const float scale,const float side,const float spread,const float lift,
                                 const BirdPalette& palette,const BirdSpecies species)
        {
            const bool frigate=species==BirdSpecies::Frigatebird;
            const bool parrot=species==BirdSpecies::Parrot;
            // the frigatebird's long narrow hand, the parrot's and the dove's broad rounded one
            const float reach=frigate?1.30f:(parrot?.94f:1.0f);
            const float chord=frigate?.70f:1.0f;
            const auto at=[&](const float x,const float y,const float z) {
                return LocalPoint(centre,right,up,forward,side*x,y,z,scale);
            };
            const float s=spread;
            const Vector3 shoulder=at(.07f,.035f,.17f);
            const Vector3 armRoot=at(.09f,.020f,-.08f);
            // spread, the wings held in a shallow V (a level eye sees their upper sides, not
            // only their edges)
            const Vector3 wrist=at(std::lerp(.17f,.47f,s)*reach,.045f+lift*.45f+(frigate?.10f*s:.06f*s),
                                   std::lerp(-.12f,frigate?.15f:.10f,s));
            const Vector3 tip=at(std::lerp(.20f,.97f,s)*reach,lift+(frigate?-.06f*s:.09f*s),
                                 std::lerp(-.34f,frigate?-.36f:-.10f,s));
            const Vector3 wristBack=wrist-forward*(.16f*chord*scale)-up*(.010f*scale);
            const Vector3 armBack=armRoot-forward*(.05f*scale);
            // the arm's coverts: thick at the leading edge, thinning back
            const std::array<Vector3,4> arm{shoulder,wrist,wristBack,armBack};
            const std::array<float,4> armThickness{.030f*scale,.022f*scale,.008f*scale,.012f*scale};
            const std::array<float,4> armLift{.012f*scale,.016f*scale*s,.004f*scale,.004f*scale};
            AppendProfiledSurface(vertices,arm,up,armThickness,armLift,
                                  palette.upperWing,palette.lowerWing);
            const Vector3 handEnd=Vector3::Lerp(wrist,tip,.42f);
            const Vector3 handBack=handEnd-forward*(.10f*chord*scale);
            const std::array<Vector3,4> hand{wrist,handEnd,handBack,wristBack};
            const std::array<float,4> handThickness{.016f*scale,.010f*scale,.005f*scale,.007f*scale};
            const std::array<float,4> handLift{.010f*scale,.006f*scale,.003f*scale,.004f*scale};
            AppendProfiledSurface(vertices,hand,up,handThickness,handLift,
                                  Color::Lerp(palette.upperWing,palette.flight,.35f),palette.lowerWing);
            const Vector3 outward=Unit(tip-wrist,right*side);
            const Vector3 back=Unit(-forward*(1.0f-.25f*s)+right*(side*.12f*s)-up*(.05f*s),-forward);
            // (the flight feathers' undersides a shade darker than the coverts', one continuous
            // underside: lighter, each feather read as a separate scale from below)
            const Color under=wildlife_render::ScaleColor(palette.lowerWing,.86f);
            const float thickness=.006f*scale;
            // the secondaries, from the arm's trailing edge, the innermost shortest
            for(int k=0;k<SecondaryCount;++k) {
                const float t=(static_cast<float>(k)+.5f)/SecondaryCount;
                const Vector3 root=Vector3::Lerp(armBack,wristBack,t)+forward*(.05f*scale);
                const Vector3 along=Unit(Vector3::Lerp(back,-forward,.3f*(1.0f-t))-up*(.04f*s),-forward);
                const Vector3 across=Unit(Vector3::Cross(up,along)*side,right);
                const float length=(.25f+.05f*t)*chord*scale*(frigate?.8f:1.0f);
                // broad enough to overlap their neighbours (apart they read as loose dashes)
                const float width=std::max(.085f*chord*scale,2.3f*Vector3::Distance(armBack,wristBack)/SecondaryCount);
                AppendFeather(vertices,root,along,across,up,length,width,thickness,
                              Color::Lerp(palette.upperWing,palette.flight,.55f),palette.flight,under);
            }
            // the greater coverts, a row of short feathers over the secondaries' roots, a shade
            // lighter than the arm (the arm read as one flat board without them)
            for(int k=0;k<CovertCount;++k) {
                const float t=(static_cast<float>(k)+.5f)/CovertCount;
                const Vector3 root=Vector3::Lerp(Vector3::Lerp(shoulder,armRoot,.55f),Vector3::Lerp(wrist,wristBack,.45f),t)
                    +up*(.024f*scale);
                const Vector3 along=Unit(back-up*(.10f*s),-forward);
                const Vector3 across=Unit(Vector3::Cross(up,along)*side,right);
                const float width=1.8f*Vector3::Distance(armRoot,wristBack)/CovertCount;
                AppendFeather(vertices,root,along,across,up,.11f*chord*scale,width,thickness,
                              wildlife_render::ScaleColor(palette.upperWing,1.12f),palette.upperWing,palette.lowerWing);
            }
            // the primaries fanned from the hand: the innermost back, the outer ones out to the
            // tip, the longest near the outside; spread, their tips stand apart
            for(int k=0;k<PrimaryCount;++k) {
                const float t=static_cast<float>(k)/(PrimaryCount-1);
                const Vector3 root=Vector3::Lerp(wristBack,handEnd,t*.9f);
                // (the outer tips bent up by the air's lift)
                const Vector3 along=Unit(Vector3::Lerp(back,outward,std::lerp(.10f,.92f,t)*s+.02f*t)
                                         +up*(.12f*s*t),-forward);
                const Vector3 across=Unit(Vector3::Cross(up,along)*side,right);
                const float longest=frigate?.62f:(parrot?.46f:.50f);
                const float length=longest*scale*std::lerp(.62f,1.0f,std::min(1.0f,t*1.25f))*(k==PrimaryCount-1?.92f:1.0f);
                AppendFeather(vertices,root,along,across,up,length,(frigate?.070f:.090f)*scale,thickness,
                              Color::Lerp(palette.upperWing,palette.flight,.75f),
                              wildlife_render::ScaleColor(palette.flight,.70f),under);
            }
        }

        // A near tail of six feathers fanned from the rump: the parrot's long and graduated, the
        // frigatebird's deeply forked, the coastal birds' square, the doves' rounded.
        void AppendFeatheredTail(std::vector<VertexPositionColor>& vertices,const Vector3& centre,
                                 const Vector3& right,const Vector3& up,const Vector3& forward,
                                 const float scale,const BirdPalette& palette,const BirdSpecies species)
        {
            const bool parrot=species==BirdSpecies::Parrot,frigate=species==BirdSpecies::Frigatebird;
            const bool dove=species==BirdSpecies::FruitDove;
            // centre pair to outer pair
            const std::array<float,3> lengths=parrot?std::array<float,3>{.74f,.60f,.46f}
                :frigate?std::array<float,3>{.52f,.74f,1.00f}
                :dove?std::array<float,3>{.34f,.32f,.28f}:std::array<float,3>{.42f,.44f,.47f};
            const float fan=parrot?.10f:frigate?.22f:dove?.30f:.34f;
            const float width=(parrot?.075f:frigate?.070f:.085f)*scale;
            const Vector3 root=LocalPoint(centre,right,up,forward,0.0f,.004f,-.40f,scale);
            const Color under=Color::Lerp(palette.lowerWing,palette.tail,.4f);
            for(int k=0;k<TailFeatherCount;++k) {
                const int pair=k<3?2-k:k-3;     // 2,1,0 on the left, 0,1,2 on the right
                const float side=k<3?-1.0f:1.0f;
                const float angle=side*fan*(static_cast<float>(pair)+.5f)/3.0f;
                const Vector3 along=Unit(-forward*std::cos(angle)+right*std::sin(angle)-up*.06f,-forward);
                const Vector3 across=Unit(Vector3::Cross(up,along),right);
                AppendFeather(vertices,root+right*(side*.012f*scale)+up*(.002f*scale*static_cast<float>(pair)),
                              along,across,up,lengths[static_cast<std::size_t>(pair)]*scale,width,.006f*scale,
                              palette.tail,parrot?palette.flight:wildlife_render::ScaleColor(palette.tail,.72f),under);
            }
        }

        void AppendNearBody(std::vector<VertexPositionColor>& vertices,
                            const Vector3& centre,const Vector3& right,
                            const Vector3& up,const Vector3& forward,const float scale,
                            const BirdPalette& palette,const BirdSpecies species,
                            const float ruffle=0.0f,const int sides=16)
        {
            const int Sides=sides;
            const bool dove=species==BirdSpecies::FruitDove;
            // Ruffled, the body's rings puff out by up to a fifth; the same vertices.
            const float puff=1.0f+.20f*std::clamp(ruffle,0.0f,1.0f);
            const Vector3 tail=LocalPoint(centre,right,up,forward,0,dove?.008f:0.0f,
                dove?-.45f:-.49f,scale);
            const Vector3 neck=LocalPoint(centre,right,up,forward,0,dove?.030f:.015f,
                dove?.365f:.34f,scale);
            // A smooth spindle from the tail root to the neck, shaded at its corners: the old
            // three rings read as a cut diamond, each facet one flat colour.
            struct Section { float z, halfWidth, halfHeight; };
            constexpr std::array<Section,7> Sections{{
                {-.42f,.045f,.040f},{-.33f,.100f,.088f},{-.20f,.155f,.132f},{-.04f,.190f,.160f},
                {.12f,.178f,.150f},{.24f,.138f,.122f},{.31f,.088f,.082f}}};
            const float wide=dove?1.17f:1.0f,tall=dove?1.12f:1.0f;
            const auto corner=[&](const std::size_t section,const int side) {
                const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                const auto& at=Sections[section];
                return LocalPoint(centre,right,up,forward,std::cos(angle)*at.halfWidth*wide*puff,
                    std::sin(angle)*at.halfHeight*tall*puff,at.z,scale);
            };
            const auto shade=[&](const int side) {
                const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                const Vector3 normal=Vector3::Normalize(right*std::cos(angle)+up*std::sin(angle));
                return Lit(Color::Lerp(palette.lowerWing,palette.body,
                    std::clamp((std::sin(angle)+.35f)/.45f,0.0f,1.0f)),normal);
            };
            const Color tailColor=Lit(palette.body,-forward),neckColor=Lit(palette.body,forward);
            for(int side=0;side<Sides;++side) {
                const int next=side+1;
                vertices.emplace_back(tail,tailColor);
                vertices.emplace_back(corner(0,next),shade(next));
                vertices.emplace_back(corner(0,side),shade(side));
                for(std::size_t section=0;section+1<Sections.size();++section) {
                    vertices.emplace_back(corner(section,side),shade(side));
                    vertices.emplace_back(corner(section,next),shade(next));
                    vertices.emplace_back(corner(section+1,next),shade(next));
                    vertices.emplace_back(corner(section,side),shade(side));
                    vertices.emplace_back(corner(section+1,next),shade(next));
                    vertices.emplace_back(corner(section+1,side),shade(side));
                }
                vertices.emplace_back(neck,neckColor);
                vertices.emplace_back(corner(Sections.size()-1,side),shade(side));
                vertices.emplace_back(corner(Sections.size()-1,next),shade(next));
            }
        }

        // A near head lofted along the bill's axis (the detail LOD, all but the doves): from the
        // neck, in the body's colour, swelling over the crown and narrowing to the face, shaded
        // at every corner; the eye a pale ring round a coloured iris and a black pupil on the
        // side the camera sees, set into the surface; the parrot's upper mandible a deep hook
        // curling down over the small lower one, the others' a straight tapering bill.
        void AppendLoftedHead(std::vector<VertexPositionColor>& vertices,const Vector3& centre,
                              const Vector3& right,const Vector3& up,const Vector3& forward,
                              const float scale,const BirdPalette& palette,const BirdSpecies species,
                              const Vector3& cameraPosition)
        {
            const bool parrot=species==BirdSpecies::Parrot;
            const float h=parrot?.95f:(species==BirdSpecies::Frigatebird?.80f:.86f);
            struct Section { float z, halfWidth, halfHeight, lift, toHead; };
            constexpr std::array<Section,7> Sections{{
                {.300f,.090f,.088f,.012f,0.0f},{.340f,.112f,.112f,.030f,.55f},{.385f,.124f,.130f,.046f,.95f},
                {.430f,.122f,.128f,.054f,1.0f},{.475f,.106f,.112f,.054f,1.0f},{.512f,.078f,.084f,.050f,1.0f},
                {.538f,.040f,.046f,.046f,1.0f}}};
            constexpr int Sides=14;
            const auto point=[&](const std::size_t i,const int side) {
                const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                const auto& at=Sections[i];
                return LocalPoint(centre,right,up,forward,std::cos(angle)*at.halfWidth*h,
                                  at.lift+std::sin(angle)*at.halfHeight*h,at.z,scale);
            };
            const auto colour=[&](const std::size_t i,const int side) {
                const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                const auto& at=Sections[i];
                // round its middle the profile's slope tips the normal back toward the neck
                const float slope=i==0||i+1==Sections.size()?0.0f:
                    (Sections[i+1].halfWidth-Sections[i-1].halfWidth)/(Sections[i+1].z-Sections[i-1].z);
                const Vector3 normal=Unit(right*std::cos(angle)+up*std::sin(angle)-forward*(slope*.8f),up);
                // the throat a shade paler than the crown
                const Color base=Color::Lerp(palette.body,palette.head,at.toHead);
                return Lit(Color::Lerp(wildlife_render::ScaleColor(base,.86f),base,.5f+.5f*std::sin(angle)),normal);
            };
            for(int side=0;side<Sides;++side) {
                const int next=side+1;
                for(std::size_t i=0;i+1<Sections.size();++i) {
                    vertices.emplace_back(point(i,side),colour(i,side));
                    vertices.emplace_back(point(i,next),colour(i,next));
                    vertices.emplace_back(point(i+1,next),colour(i+1,next));
                    vertices.emplace_back(point(i,side),colour(i,side));
                    vertices.emplace_back(point(i+1,next),colour(i+1,next));
                    vertices.emplace_back(point(i+1,side),colour(i+1,side));
                }
                const Vector3 face=LocalPoint(centre,right,up,forward,0.0f,Sections.back().lift,
                                              Sections.back().z+.006f,scale);
                vertices.emplace_back(point(Sections.size()-1,side),colour(Sections.size()-1,side));
                vertices.emplace_back(point(Sections.size()-1,next),colour(Sections.size()-1,next));
                vertices.emplace_back(face,Lit(palette.head,forward));
            }
            // the bill: rings along a curve, a hook for the parrot
            const std::array<std::array<float,4>,6> bill=parrot
                ? std::array<std::array<float,4>,6>{{{.505f,.060f,.040f,.050f},{.540f,.072f,.036f,.044f},
                    {.572f,.062f,.028f,.036f},{.592f,.030f,.020f,.028f},{.596f,-.006f,.012f,.018f},{.584f,-.034f,.004f,.006f}}}
                : std::array<std::array<float,4>,6>{{{.525f,.046f,.026f,.020f},{.560f,.045f,.020f,.016f},
                    {.595f,.044f,.015f,.012f},{.625f,.042f,.010f,.009f},{.650f,.040f,.006f,.005f},{.668f,.038f,.002f,.002f}}};
            constexpr int BillSides=8;
            const auto billPoint=[&](const std::size_t i,const int side) {
                const float angle=6.28318530718f*static_cast<float>(side)/BillSides;
                return LocalPoint(centre,right,up,forward,std::cos(angle)*bill[i][2]*h,
                                  bill[i][1]*h+std::sin(angle)*bill[i][3]*h,bill[i][0],scale);
            };
            for(int side=0;side<BillSides;++side) {
                const float angle=6.28318530718f*static_cast<float>(side)/BillSides;
                const Color c=Lit(palette.beak,Unit(right*std::cos(angle)+up*std::sin(angle)+forward*.3f,forward));
                const Color c1=Lit(palette.beak,Unit(right*std::cos(angle+.785f)+up*std::sin(angle+.785f)+forward*.3f,forward));
                for(std::size_t i=0;i+1<bill.size();++i) {
                    vertices.emplace_back(billPoint(i,side),c);
                    vertices.emplace_back(billPoint(i,side+1),c1);
                    vertices.emplace_back(billPoint(i+1,side+1),c1);
                    vertices.emplace_back(billPoint(i,side),c);
                    vertices.emplace_back(billPoint(i+1,side+1),c1);
                    vertices.emplace_back(billPoint(i+1,side),c);
                }
            }
            if(parrot) {
                // the lower mandible, a short blunt scoop under the hook
                const Color lower=wildlife_render::ScaleColor(palette.beak,.72f);
                const Vector3 a=LocalPoint(centre,right,up,forward,-.032f*h,.010f*h,.508f,scale);
                const Vector3 b=LocalPoint(centre,right,up,forward,.032f*h,.010f*h,.508f,scale);
                const Vector3 c=LocalPoint(centre,right,up,forward,0.0f,-.012f*h,.560f,scale);
                const Vector3 d=LocalPoint(centre,right,up,forward,0.0f,-.030f*h,.512f,scale);
                AddTriangle(vertices,a,b,c,wildlife_render::FacetColor(a,b,c,lower,true));
                AddTriangle(vertices,a,d,b,wildlife_render::FacetColor(a,d,b,lower,true));
                AddTriangle(vertices,a,c,d,wildlife_render::FacetColor(a,c,d,lower,true));
                AddTriangle(vertices,b,d,c,wildlife_render::FacetColor(b,d,c,lower,true));
            }
            // the eye on the camera's side: a pale ring, the iris, the pupil, a speck of light
            const float eyeSide=Vector3::Dot(cameraPosition-centre,right)>=0.0f?1.0f:-1.0f;
            constexpr float EyeZ=.455f,EyeY=.080f;
            const float surface=(Sections[3].halfWidth+(Sections[4].halfWidth-Sections[3].halfWidth)*((EyeZ-.430f)/.045f))*h;
            const float ringRadius=parrot?.030f:.020f,irisRadius=parrot?.016f:.014f,pupilRadius=parrot?.008f:.009f;
            const Color ring=parrot?Color(226,222,210,255):wildlife_render::ScaleColor(palette.head,.70f);
            const Color iris=parrot?Color(214,150,40,255):Color(96,58,30,255);
            constexpr int EyeSides=10;
            const auto disc=[&](const float radius,const float out,const Color& c) {
                const Vector3 middle=LocalPoint(centre,right,up,forward,eyeSide*(surface*.88f+out),EyeY*h,EyeZ,scale);
                for(int k=0;k<EyeSides;++k) {
                    const float a0=6.28318530718f*static_cast<float>(k)/EyeSides,a1=6.28318530718f*static_cast<float>(k+1)/EyeSides;
                    const auto edge=[&](const float a) {
                        return middle+up*(std::sin(a)*radius*h*scale)+forward*(std::cos(a)*radius*h*1.15f*scale)
                            -right*(eyeSide*radius*radius*6.0f*scale);
                    };
                    AddTriangle(vertices,middle,edge(a0),edge(a1),c);
                }
            };
            disc(ringRadius,.004f,ring);
            disc(irisRadius,.006f,iris);
            disc(pupilRadius,.008f,palette.eye);
            disc(.0035f,.0095f,Color(236,236,230,255));
        }

        void AppendNearHead(std::vector<VertexPositionColor>& vertices,
                            const Vector3& centre,const Vector3& right,
                            const Vector3& up,const Vector3& forward,const float scale,
                            const BirdPalette& palette, const BirdSpecies species,
                            const Vector3& cameraPosition, const bool coarse=false)
        {
            const int Sides=coarse?6:16;
            const float headScale = species == BirdSpecies::Parrot ? .88f
                : (species == BirdSpecies::FruitDove ? .82f : .90f);
            const bool parrot = species == BirdSpecies::Parrot;
            const bool dove = species == BirdSpecies::FruitDove;
            const Vector3 top=LocalPoint(centre,right,up,forward,0,.180f*headScale,.405f,scale);
            const Vector3 bottom=LocalPoint(centre,right,up,forward,0,-.072f*headScale,.395f,scale);
            const auto ring=[&](float angle,float xRadius,float zRadius,float y) {
                return LocalPoint(centre,right,up,forward,std::cos(angle)*xRadius*headScale,
                    y*headScale,.405f+std::sin(angle)*zRadius*headScale,scale);
            };
            if(dove) {
                const Vector3 doveHead=LocalPoint(centre,right,up,forward,0,.042f,.415f,scale);
                AppendEllipsoid(vertices,doveHead,right,up,forward,
                    .108f*scale,.126f*scale,.140f*scale,palette.head,
                    wildlife_render::ScaleColor(palette.head,.90f),coarse?4:6,coarse?6:12);
            } else {
                // four rings, rounded crown to throat, shaded at the corners
                struct HeadRing { float x, z, y, tilt; };
                constexpr std::array<HeadRing,4> HeadRings{{
                    {.072f,.085f,.142f,.62f},{.114f,.134f,.088f,.25f},{.124f,.144f,.022f,-.10f},
                    {.088f,.104f,-.038f,-.55f}}};
                const auto at=[&](const std::size_t r,const int side) {
                    const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                    return ring(angle,HeadRings[r].x,HeadRings[r].z,HeadRings[r].y);
                };
                const auto shade=[&](const std::size_t r,const int side) {
                    const float angle=6.28318530718f*static_cast<float>(side)/Sides;
                    return Lit(palette.head,Vector3::Normalize(right*std::cos(angle)+forward*std::sin(angle)
                        +up*HeadRings[r].tilt));
                };
                const Color topColor=Lit(palette.head,up),bottomColor=Lit(palette.head,-up);
                for(int side=0;side<Sides;++side) {
                    const int next=side+1;
                    vertices.emplace_back(top,topColor);
                    vertices.emplace_back(at(0,side),shade(0,side));
                    vertices.emplace_back(at(0,next),shade(0,next));
                    for(std::size_t r=0;r+1<HeadRings.size();++r) {
                        vertices.emplace_back(at(r,side),shade(r,side));
                        vertices.emplace_back(at(r+1,side),shade(r+1,side));
                        vertices.emplace_back(at(r+1,next),shade(r+1,next));
                        vertices.emplace_back(at(r,side),shade(r,side));
                        vertices.emplace_back(at(r+1,next),shade(r+1,next));
                        vertices.emplace_back(at(r,next),shade(r,next));
                    }
                    vertices.emplace_back(bottom,bottomColor);
                    vertices.emplace_back(at(HeadRings.size()-1,next),shade(HeadRings.size()-1,next));
                    vertices.emplace_back(at(HeadRings.size()-1,side),shade(HeadRings.size()-1,side));
                }
            }
            const float beakWidth = parrot ? .046f : (dove ? .026f : .038f);
            // (a dove's slender bill stands clear of its round head)
            const float beakBase = parrot ? .526f : (dove ? .520f : .520f);
            const float beakReach = parrot ? .616f : (dove ? .640f : .625f);
            const float baseTop=parrot?.098f:.078f;
            const float baseBottom=parrot?.025f:.026f;
            const float midZ=beakBase+(beakReach-beakBase)*.58f;
            const float midWidth=beakWidth*(parrot?.72f:.62f);
            const std::array<Vector3,4> beakRoot{
                LocalPoint(centre,right,up,forward,-beakWidth,.050f,beakBase,scale),
                LocalPoint(centre,right,up,forward,0,baseTop,beakBase,scale),
                LocalPoint(centre,right,up,forward,beakWidth,.050f,beakBase,scale),
                LocalPoint(centre,right,up,forward,0,baseBottom,beakBase,scale)};
            const std::array<Vector3,4> beakMiddle{
                LocalPoint(centre,right,up,forward,-midWidth,parrot?.036f:.047f,midZ,scale),
                LocalPoint(centre,right,up,forward,0,parrot?.078f:.064f,midZ,scale),
                LocalPoint(centre,right,up,forward,midWidth,parrot?.036f:.047f,midZ,scale),
                LocalPoint(centre,right,up,forward,0,parrot?.010f:.033f,midZ,scale)};
            const Vector3 beakTip=LocalPoint(centre,right,up,forward,0,
                parrot?-.012f:.043f,beakReach,scale);
            for(std::size_t face=0;face<beakRoot.size();++face) {
                const std::size_t next=(face+1U)%beakRoot.size();
                const auto addBeakTriangle=[&](const Vector3& a,const Vector3& b,
                                               const Vector3& c) {
                    AddTriangle(vertices,a,b,c,
                        wildlife_render::FacetColor(a,b,c,palette.beak,true));
                };
                addBeakTriangle(beakRoot[face],beakMiddle[face],beakMiddle[next]);
                addBeakTriangle(beakRoot[face],beakMiddle[next],beakRoot[next]);
                addBeakTriangle(beakMiddle[face],beakTip,beakMiddle[next]);
            }
            if(coarse) return;
            constexpr int EyeSides=6;
            const float eyeSide=Vector3::Dot(cameraPosition-centre,right)>=0.0f?1.0f:-1.0f;
            const float eyeX=eyeSide*(dove?.109f:.106f*headScale);
            const Vector3 eyeCentre=LocalPoint(
                centre,right,up,forward,eyeX,.078f*headScale,.469f,scale);
            for(int segment=0;segment<EyeSides;++segment) {
                const float a0=6.28318530718f*static_cast<float>(segment)/EyeSides;
                const float a1=6.28318530718f*static_cast<float>(segment+1)/EyeSides;
                const auto eyeEdge=[&](const float angle) {
                    return LocalPoint(centre,right,up,forward,eyeX,
                        .078f*headScale+std::sin(angle)*.013f,
                        .469f+std::cos(angle)*.015f,scale);
                };
                if(dove) {
                    const float outerRadiusY=.017f,outerRadiusZ=.019f;
                    const float innerRadiusY=.008f,innerRadiusZ=.009f;
                    const auto ringPoint=[&](const float angle,const float ry,const float rz,
                                             const float outward) {
                        return LocalPoint(centre,right,up,forward,
                            eyeX+eyeSide*outward,
                            .078f*headScale+std::sin(angle)*ry,
                            .469f+std::cos(angle)*rz,scale);
                    };
                    const Vector3 outer0=ringPoint(a0,outerRadiusY,outerRadiusZ,.003f);
                    const Vector3 outer1=ringPoint(a1,outerRadiusY,outerRadiusZ,.003f);
                    const Vector3 inner0=ringPoint(a0,innerRadiusY,innerRadiusZ,.005f);
                    const Vector3 inner1=ringPoint(a1,innerRadiusY,innerRadiusZ,.005f);
                    const Color eyeRing(188,145,111,255);
                    if(eyeSide>0.0f) {
                        AddTriangle(vertices,outer0,outer1,inner1,eyeRing);
                        AddTriangle(vertices,outer0,inner1,inner0,eyeRing);
                        AddTriangle(vertices,eyeCentre,inner1,inner0,palette.eye);
                    } else {
                        AddTriangle(vertices,outer0,inner1,outer1,eyeRing);
                        AddTriangle(vertices,outer0,inner0,inner1,eyeRing);
                        AddTriangle(vertices,eyeCentre,inner0,inner1,palette.eye);
                    }
                } else if(eyeSide>0.0f)
                    AddTriangle(vertices,eyeCentre,eyeEdge(a1),eyeEdge(a0),palette.eye);
                else AddTriangle(vertices,eyeCentre,eyeEdge(a0),eyeEdge(a1),palette.eye);
                }
        }

        void AppendNearBird(std::vector<VertexPositionColor>& vertices,
                            const BirdState& bird,const Vector3& cameraPosition,
                            const bool coarse=false)
        {
            const std::size_t startingVertexCount=vertices.size();
            const BirdPalette& palette = PaletteForBird(bird);
            const Vector3 centre(bird.x, bird.y, bird.z);
            const Vector3 forward(std::cos(bird.headingRadians), 0.0f,
                                  std::sin(bird.headingRadians));
            const Vector3 levelRight(-forward.Z, 0.0f, forward.X);
            const float bankCosine = std::cos(bird.bankRadians);
            const float bankSine = std::sin(bird.bankRadians);
            const Vector3 right = levelRight * bankCosine + Vector3::Up * bankSine;
            const Vector3 up = Vector3::Up * bankCosine - levelRight * bankSine;
            const float scale = bird.scale;
            // A dove on the ground stands on its legs with its breast raised: sat down on its
            // folded toes, level, it read as a bird lying flat. The body, head, wings and tail
            // stand on the legs (the feet stay where the flock puts them).
            const bool groundStance = (bird.behavior == BirdBehavior::GroundApproach
                || bird.behavior == BirdBehavior::GroundForage
                || bird.behavior == BirdBehavior::GroundDepart) && bird.perchAmount > 0.35f;
            const float stanceLift = groundStance ? 0.075f : 0.0f, stancePitch = groundStance ? 0.20f : 0.0f;
            const Vector3 bodyCentre = centre + Vector3::Up * (stanceLift * scale);
            const Vector3 bodyForward = forward * std::cos(stancePitch) + up * std::sin(stancePitch);
            const Vector3 bodyUp = up * std::cos(stancePitch) - forward * std::sin(stancePitch);
            AppendNearBody(vertices,bodyCentre,right,bodyUp,bodyForward,scale,palette,bird.species,bird.ruffleAmount,
                           coarse?6:16);
            // The walking dove's head thrusts forward and dips slightly with each step.
            // Preening, the head turns back over one shoulder (a hundred and twenty degrees)
            // and down into the wing, pivoting about the neck root.
            const float preen = std::clamp(bird.preenAmount, 0.0f, 1.0f);
            const float preenSide = bird.preenSide < 0.0f ? -1.0f : 1.0f;
            const float turn = preen * 2.1f * preenSide, dip = preen * 0.55f;
            const Vector3 turnedForward = bodyForward * std::cos(turn) + right * std::sin(turn);
            const Vector3 turnedRight = right * std::cos(turn) - bodyForward * std::sin(turn);
            const Vector3 headForward = turnedForward * std::cos(dip) - bodyUp * std::sin(dip);
            const Vector3 headUp = bodyUp * std::cos(dip) + turnedForward * std::sin(dip);
            const float pivotZ = (bird.species == BirdSpecies::FruitDove ? .365f : .34f) * scale;
            const Vector3 headCentre = bodyCentre + bodyForward * (bird.headBob * 0.055f * scale + pivotZ)
                - headForward * pivotZ - bodyUp * (bird.headBob * 0.018f * scale);
            if(!coarse&&bird.species!=BirdSpecies::FruitDove)
                AppendLoftedHead(vertices,headCentre,turnedRight,headUp,headForward,scale,palette,bird.species,cameraPosition);
            else AppendNearHead(vertices,headCentre,turnedRight,headUp,headForward,scale,palette,bird.species,
                                cameraPosition,coarse);

            const float perch = std::clamp(bird.perchAmount, 0.0f, 1.0f);
            const bool groundDove = bird.behavior == BirdBehavior::GroundApproach
                || bird.behavior == BirdBehavior::GroundForage
                || bird.behavior == BirdBehavior::GroundDepart;
            const float baseWingLift = std::lerp(bird.wingPose * 0.38f, -0.025f, perch);
            const float stretchSide = bird.stretchSide < 0.0f ? -1.0f : 1.0f;
            const Vector3 leftFront = LocalPoint(
                bodyCentre, right, bodyUp, bodyForward, -0.06f, 0.035f, 0.20f, scale);
            const Vector3 leftBack = LocalPoint(
                bodyCentre, right, bodyUp, bodyForward, -0.08f, 0.015f, -0.24f, scale);
            const Vector3 rightFront = LocalPoint(
                bodyCentre, right, bodyUp, bodyForward, 0.06f, 0.035f, 0.20f, scale);
            const Vector3 rightBack = LocalPoint(
                bodyCentre, right, bodyUp, bodyForward, 0.08f, 0.015f, -0.24f, scale);
            for(const float side:{-1.0f,1.0f}) {
                // Stretching, the one wing opens out and down from its perch fold by the
                // stretch amount; the other keeps its fold.
                const float stretch=side==stretchSide?std::clamp(bird.stretchAmount,0.0f,1.0f):0.0f;
                const float sidePerch=perch*(1.0f-0.85f*stretch);
                const float wingLift=std::lerp(baseWingLift,-0.16f,stretch);
                const float wingSpan=std::lerp(.96f,0.21f,sidePerch);
                const float wingRear=std::lerp(-0.08f,-0.29f,sidePerch);
                if(groundDove) {
                    const Vector3 foldedWing=LocalPoint(bodyCentre,right,bodyUp,bodyForward,
                        side*.145f,.045f,-.055f,scale);
                    AppendEllipsoid(vertices,foldedWing,right,bodyUp,bodyForward,
                        .105f*scale,.100f*scale,.345f*scale,
                        palette.upperWing,wildlife_render::ScaleColor(palette.upperWing,.82f));
                    continue;
                }
                if(!coarse) {
                    AppendFeatheredWing(vertices,bodyCentre,right,bodyUp,bodyForward,scale,side,
                                        1.0f-sidePerch,wingLift,palette,bird.species);
                    continue;
                }
                const Vector3 front=side<0.0f?leftFront:rightFront;
                const Vector3 back=side<0.0f?leftBack:rightBack;
                // A frigatebird's wing is long, narrow and crooked: the arm raised forward to a
                // sharp wrist, the hand swept far back and drooped to a pointed tip.
                const bool frigate=bird.species==BirdSpecies::Frigatebird;
                const float reach=frigate?std::lerp(1.32f,1.0f,sidePerch):1.0f;
                const Vector3 elbow=LocalPoint(bodyCentre,right,bodyUp,bodyForward,side*.40f*(frigate?1.25f:1.0f),
                    wingLift*.30f+.035f+(frigate?.10f*(1.0f-sidePerch):0.0f),
                    std::lerp(frigate?.16f:.055f,-.20f,sidePerch),scale);
                const Vector3 tip=LocalPoint(
                    bodyCentre,right,bodyUp,bodyForward,side*wingSpan*reach,wingLift-(frigate?.05f*(1.0f-sidePerch):0.0f),
                    wingRear-(frigate?.42f*(1.0f-sidePerch):0.0f),scale);
                const Vector3 outerTrailing=LocalPoint(bodyCentre,right,bodyUp,bodyForward,
                    side*std::lerp(.62f,.17f,sidePerch)*(frigate?1.15f:1.0f),wingLift*.62f-.018f,
                    std::lerp(frigate?-.30f:-.39f,-.48f,sidePerch),scale);
                const Vector3 innerTrailing=LocalPoint(bodyCentre,right,bodyUp,bodyForward,
                    side*std::lerp(.25f,.11f,sidePerch),wingLift*.20f-.010f,
                    std::lerp(-.34f,-.43f,sidePerch),scale);
                // Spread, the outer trailing edge splits into three primaries, the feather tips
                // standing out behind the notches between them; folded they close up.
                const float spread=1.0f-sidePerch;
                const auto edge=[&](const float t,const float out) {
                    return Vector3::Lerp(tip,outerTrailing,t)-bodyForward*(out*spread*scale);
                };
                const std::array<Vector3,12> wingOutline{
                    front,elbow,tip,edge(.14f,-.018f),edge(.26f,.060f),edge(.40f,-.012f),
                    edge(.52f,.050f),edge(.66f,-.008f),edge(.78f,.036f),outerTrailing,innerTrailing,back};
                // thick at the leading edge (root and wrist), thin through the feathers; the
                // leading edge carried a little above the trailing one; and, spread, the arm
                // rises to the wrist and the hand angles down to the tip, so the wing is never
                // one flat plane a level eye sees only edge-on
                const float s=scale;
                const float bend=spread*s;
                const std::array<float,12> wingThickness{
                    .034f*s,.026f*s,.008f*s,.005f*s,.004f*s,.004f*s,
                    .004f*s,.004f*s,.004f*s,.006f*s,.010f*s,.016f*s};
                const auto handDroop=[bend](const float t) {return bend*(-.045f+.075f*t);};
                const std::array<float,12> wingCamber{
                    .022f*s,.020f*s+.065f*bend,.006f*s-.045f*bend,
                    handDroop(.14f),handDroop(.26f),handDroop(.40f),handDroop(.52f),
                    handDroop(.66f),handDroop(.78f),.030f*bend,.034f*bend,.004f*s};
                AppendProfiledSurface(vertices,wingOutline,bodyUp,wingThickness,wingCamber,
                                      palette.upperWing,palette.lowerWing);
            }

            // (a frigatebird's tail long and deeply forked, its two streamers apart)
            const bool forked = bird.species == BirdSpecies::Frigatebird;
            const float tailHalfWidth = bird.species == BirdSpecies::Parrot ? .16f
                : (bird.species == BirdSpecies::FruitDove ? .21f : forked ? .25f : .34f);
            const float tailLength = bird.species == BirdSpecies::Parrot ? -1.08f
                : (bird.species == BirdSpecies::FruitDove ? -.64f : forked ? -1.30f : -.83f);
            const float featherRoot=bird.species==BirdSpecies::Parrot?.075f:forked?.085f:tailHalfWidth*.42f;
            if(!coarse) AppendFeatheredTail(vertices,bodyCentre,right,bodyUp,bodyForward,scale,palette,bird.species);
            else for(const float side:{-1.0f,1.0f}) {
                const float tipZ=tailLength-(side>0.0f&&bird.species==BirdSpecies::Parrot?.055f:0.0f);
                const std::array<Vector3,4> feather{
                    LocalPoint(bodyCentre,right,bodyUp,bodyForward,side*.025f,0.0f,-.35f,scale),
                    LocalPoint(bodyCentre,right,bodyUp,bodyForward,side*featherRoot,-.006f,-.47f,scale),
                    LocalPoint(bodyCentre,right,bodyUp,bodyForward,side*tailHalfWidth,side*.012f,tipZ,scale),
                    LocalPoint(bodyCentre,right,bodyUp,bodyForward,side*(forked?.07f:.018f),side*.018f,
                               tipZ+(forked?.42f:.10f),scale)};
                AppendClosedSurface(vertices,feather,bodyUp*(.010f*scale),palette.tail,
                    wildlife_render::ScaleColor(palette.tail,.84f));
            }

            if (perch > 0.35f && !coarse) {
                for (const float side : {-1.0f, 1.0f}) {
                    // Scratching, the one foot comes up past the wing to the head by the
                    // scratch amount; the same points either way.
                    const float scratch = (side == (bird.scratchSide < 0.0f ? -1.0f : 1.0f) && !groundDove)
                        ? std::clamp(bird.scratchAmount, 0.0f, 1.0f) : 0.0f;
                    const float lift = scratch * 0.24f, ahead = scratch * 0.19f, in = scratch * 0.02f;
                    const Vector3 leg = LocalPoint(
                        centre, right, up, forward, side * 0.055f, -0.075f + stanceLift + lift * 0.4f, 0.01f + ahead * 0.4f, scale);
                    const Vector3 foot = LocalPoint(
                        centre, right, up, forward, side * (0.055f - in),
                        (groundDove ? -0.20f : -0.12f) + lift, 0.01f + ahead, scale);
                    const Vector3 toe = LocalPoint(
                        centre, right, up, forward, side * (0.055f - in),
                        (groundDove ? -0.197f : -0.117f) + lift * 1.15f, 0.105f + ahead, scale);
                    const Vector3 rearToe = LocalPoint(
                        centre, right, up, forward, side * (0.055f - in),
                        (groundDove ? -0.197f : -0.117f) + lift * 0.9f, -0.06f + ahead, scale);
                    if(groundDove) {
                        const Color footColor(132,82,67,255);
                        const Vector3 outerToe=LocalPoint(centre,right,up,forward,
                            side*.080f,-.197f,.090f,scale);
                        AppendTaperedSegment(vertices,leg,foot,.013f*scale,.011f*scale,
                                             footColor);
                        AppendTaperedSegment(vertices,foot,toe,.010f*scale,.004f*scale,
                                             footColor);
                        AppendTaperedSegment(vertices,foot,outerToe,.009f*scale,.004f*scale,
                                             footColor);
                        AppendTaperedSegment(vertices,foot,rearToe,.009f*scale,.004f*scale,
                                             footColor);
                    } else {
                        AddDoubleSidedTriangle(vertices, leg, foot, toe, palette.tail);
                        AddDoubleSidedTriangle(vertices, leg, rearToe, foot, palette.tail);
                    }
                }
            }
            // the near mesh stays inside its fixed share of the buffer (feathered wings and tail,
            // lofted head, the eye and the bill)
            if(vertices.size()-startingVertexCount>static_cast<std::size_t>(MaximumVerticesPerBird)) {
                throw std::runtime_error("Bird near mesh does not match its fixed vertex budget");
            }
        }

        void AppendFarBird(std::vector<VertexPositionColor>& vertices,
                           const BirdState& bird)
        {
            const BirdPalette& palette = PaletteForBird(bird);
            const Vector3 centre(bird.x, bird.y, bird.z);
            const Vector3 forward(std::cos(bird.headingRadians), 0.0f,
                                  std::sin(bird.headingRadians));
            const Vector3 levelRight(-forward.Z, 0.0f, forward.X);
            const float bankCosine = std::cos(bird.bankRadians);
            const float bankSine = std::sin(bird.bankRadians);
            const Vector3 right = levelRight * bankCosine + Vector3::Up * bankSine;
            const Vector3 up = Vector3::Up * bankCosine - levelRight * bankSine;
            const float scale = bird.scale;
            const float perch = std::clamp(bird.perchAmount, 0.0f, 1.0f);
            const float wingLift = std::lerp(bird.wingPose * 0.30f, -0.02f, perch);
            const float wingSpan = std::lerp(0.98f, 0.20f, perch);
            const float wingRear = std::lerp(-0.04f, -0.28f, perch);
            const Vector3 nose = LocalPoint(
                centre, right, up, forward, 0.0f, 0.0f, 0.60f, scale);
            const Vector3 tail = LocalPoint(
                centre, right, up, forward, 0.0f, 0.0f, -0.55f, scale);
            const Vector3 leftTip = LocalPoint(
                centre, right, up, forward, -wingSpan, wingLift, wingRear, scale);
            const Vector3 rightTip = LocalPoint(
                centre, right, up, forward, wingSpan, wingLift, wingRear, scale);
            if (bird.species == BirdSpecies::Frigatebird) {
                // the crooked wings (an M from below) and the forked tail's two streamers
                for (const float side : {-1.0f, 1.0f}) {
                    const Vector3 wrist = LocalPoint(centre, right, up, forward, side * 0.62f, wingLift + 0.08f, 0.12f, scale);
                    const Vector3 tip = LocalPoint(centre, right, up, forward, side * 1.28f, wingLift - 0.05f, -0.48f, scale);
                    const Vector3 hand = LocalPoint(centre, right, up, forward, side * 0.55f, wingLift * 0.5f, -0.18f, scale);
                    AddDoubleSidedTriangle(vertices, nose, wrist, tail, palette.upperWing);
                    AddDoubleSidedTriangle(vertices, wrist, tip, hand, palette.upperWing);
                    AddDoubleSidedTriangle(vertices, tail, LocalPoint(centre, right, up, forward, side * 0.05f, 0.0f, -0.62f, scale),
                                           LocalPoint(centre, right, up, forward, side * 0.20f, 0.0f, -1.30f, scale), palette.tail);
                }
                return;
            }
            AddDoubleSidedTriangle(vertices, nose, leftTip, tail, palette.upperWing);
            AddDoubleSidedTriangle(vertices, nose, tail, rightTip, palette.upperWing);
        }
    }

    BirdRenderer::BirdRenderer(GraphicsDevice& device, const float maximumDistance)
        : device_(device),
          effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(
              device, VertexPositionColor::getVertexDeclarationStatic(), MaximumVertexCount,
              BufferUsage::WriteOnly)),
          maximumDistance_(maximumDistance)
    {
        if (!std::isfinite(maximumDistance_) || maximumDistance_ <= 0.0f) {
            throw std::invalid_argument("Bird maximum draw distance must be finite and positive");
        }
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(true);
        effect_->setFogColorProperty(Vector3(environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue));
        effect_->setFogStartProperty(310.0f);
        effect_->setFogEndProperty(700.0f);
        stagingVertices_.reserve(static_cast<std::size_t>(MaximumVertexCount));
        std::cout << NearBirdFeatherDiagnostic << '\n';
        std::cout << BirdPerchSilhouetteDiagnostic << '\n';
        std::cout << NearBirdHeadDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(NearBirdFeatherDiagnostic);
        MirrorRuntimeDiagnosticToPlatformLog(BirdPerchSilhouetteDiagnostic);
        MirrorRuntimeDiagnosticToPlatformLog(NearBirdHeadDiagnostic);
        std::cout << TropicalBirdDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(TropicalBirdDiagnostic);
        std::cout << NearBirdProfileDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(NearBirdProfileDiagnostic);
        std::cout << GroundDoveAnatomyDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(GroundDoveAnatomyDiagnostic);
    }

    BirdRenderer::~BirdRenderer() = default;

    void BirdRenderer::Draw(const Matrix& view, const Matrix& projection,
                            const Vector3& cameraPosition,
                            const std::span<const BirdState> birds)
    {
        stagingVertices_.clear();
        visibleBirdCount_ = 0;
        visibleNearBirdCount_ = 0;
        visibleFarBirdCount_ = 0;
        drawCallCount_ = 0;
        const BoundingFrustum frustum(view * projection);

        for (const BirdState& bird : birds) {
            const Vector3 centre(bird.x, bird.y, bird.z);
            const float distanceSquared = Vector3::DistanceSquared(centre, cameraPosition);
            if (distanceSquared > maximumDistance_ * maximumDistance_
                || !frustum.Intersects(BoundingSphere(centre, bird.scale * 1.25f))) {
                continue;
            }
            if (distanceSquared <= NearLodDistance * NearLodDistance) {
                AppendNearBird(stagingVertices_, bird, cameraPosition,
                               distanceSquared > DetailLodDistance * DetailLodDistance);
                ++visibleNearBirdCount_;
            } else {
                AppendFarBird(stagingVertices_, bird);
                ++visibleFarBirdCount_;
            }
            ++visibleBirdCount_;
        }

        if (stagingVertices_.empty()) {
            return;
        }

        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        vertices_->SetData(stagingVertices_.data(), 0,
                           static_cast<int>(stagingVertices_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0,
                               static_cast<int>(stagingVertices_.size() / 3U));
        ++drawCallCount_;
    }
}
