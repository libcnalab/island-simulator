#include "IslandSimulator/AudioAnalysis.hpp"
#include "IslandSimulator/AquaticLightingModel.hpp"
#include "IslandSimulator/BenchmarkTraversalRoute.hpp"
#include "IslandSimulator/BirdFlock.hpp"
#include "IslandSimulator/WindGust.hpp"
#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/BirdPerchLayout.hpp"
#include "IslandSimulator/ButterflyMeadow.hpp"
#include "IslandSimulator/GeckoPopulation.hpp"
#include "IslandSimulator/SkinkPopulation.hpp"
#include "IslandSimulator/DragonflySwarm.hpp"
#include "IslandSimulator/FrogPopulation.hpp"
#include "IslandSimulator/HermitCrabGroup.hpp"
#include "IslandSimulator/CoconutCrabGroup.hpp"
#include "IslandSimulator/AntTrails.hpp"
#include "IslandSimulator/StingraySandField.hpp"
#include "IslandSimulator/GrassWalkers.hpp"
#include "IslandSimulator/RodentPopulation.hpp"
#include "IslandSimulator/ForestTwigLayout.hpp"
#include "IslandSimulator/OctopusInkField.hpp"
#include "IslandSimulator/CrocodileGroup.hpp"
#include "IslandSimulator/CampfireSite.hpp"
#include "IslandSimulator/ResidentMotion.hpp"
#include "IslandSimulator/SnakePopulation.hpp"
#include "IslandSimulator/BearMotion.hpp"
#include "IslandSimulator/TurtleBreathField.hpp"
#include "IslandSimulator/CrabColony.hpp"
#include "IslandSimulator/CoastalMaterial.hpp"
#include "IslandSimulator/CoastalWindResponse.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/ContactShadowPatch.hpp"
#include "IslandSimulator/DebugFreeCamera.hpp"
#include "IslandSimulator/DistantSceneryLayout.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/FishSchools.hpp"
#include "IslandSimulator/FirstPersonCameraMotion.hpp"
#include "IslandSimulator/FirstPersonInput.hpp"
#include "IslandSimulator/GeneratedTextureBudget.hpp"
#include "IslandSimulator/GrassInteractionModel.hpp"
#include "IslandSimulator/CanopyLightField.hpp"
#include "IslandSimulator/GrassLayout.hpp"
#include "IslandSimulator/GroundCoverField.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/SkyCloudDrift.hpp"
#include "IslandSimulator/TrunkSurfaceMaterial.hpp"
#include "IslandSimulator/GrassLod.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WreckScourPatch.hpp"
#include "IslandSimulator/StreamCourse.hpp"
#include "IslandSimulator/StreamBedPatch.hpp"
#include "IslandSimulator/StreamRingField.hpp"
#include "IslandSimulator/StreamDriftField.hpp"
#include "IslandSimulator/SunShadeField.hpp"
#include "IslandSimulator/BlownSandField.hpp"
#include "IslandSimulator/WetlandMistField.hpp"
#include "IslandSimulator/HoofDustField.hpp"
#include "IslandSimulator/FinWakeField.hpp"
#include "IslandSimulator/TurtleGrazeField.hpp"
#include "IslandSimulator/DragonflyDipField.hpp"
#include "IslandSimulator/CrocodileWakeField.hpp"
#include "IslandSimulator/MudFleckField.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/SunShadowFade.hpp"
#include "IslandSimulator/VegetationFade.hpp"
#include "IslandSimulator/SlopeScrubLayout.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/QuadBike.hpp"
#include "IslandSimulator/ExhaustSmokeField.hpp"
#include "IslandSimulator/QuadWheelTraces.hpp"
#include "IslandSimulator/RainRings.hpp"
#include "IslandSimulator/FireflySwarm.hpp"
#include "IslandSimulator/MorningMistField.hpp"
#include "IslandSimulator/WhalePod.hpp"
#include "IslandSimulator/SailboatPassage.hpp"
#include "IslandSimulator/Satellites.hpp"
#include "IslandSimulator/Waterspout.hpp"
#include "IslandSimulator/SwimGlowField.hpp"
#include "IslandSimulator/ShootingStars.hpp"
#include "IslandSimulator/CrabBubbleField.hpp"
#include "IslandSimulator/BeetlePopulation.hpp"
#include "IslandSimulator/CicadaColony.hpp"
#include "IslandSimulator/StreamFishField.hpp"
#include "IslandSimulator/HatchlingField.hpp"
#include "IslandSimulator/BirdShadowProjection.hpp"
#include "IslandSimulator/SeabedShadowProjection.hpp"
#include "IslandSimulator/CloudShadow.hpp"
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/CanopyShafts.hpp"
#include "IslandSimulator/CanopyLeafFallField.hpp"
#include "IslandSimulator/FootprintTrail.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include "IslandSimulator/ShellDragTrail.hpp"
#include "IslandSimulator/ReefFishField.hpp"
#include "IslandSimulator/WalkerShadow.hpp"
#include "IslandSimulator/ValleyMistField.hpp"
#include "IslandSimulator/FootprintGround.hpp"
#include "IslandSimulator/SurfSprayField.hpp"
#include "IslandSimulator/DolphinBlowField.hpp"
#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include "IslandSimulator/HutGeckos.hpp"
#include "IslandSimulator/SunSteamField.hpp"
#include "IslandSimulator/JettyLayout.hpp"
#include "IslandSimulator/LookoutLayout.hpp"
#include "IslandSimulator/FallingFruitField.hpp"
#include "IslandSimulator/CrabColony.hpp"
#include "IslandSimulator/WrackLine.hpp"
#include "IslandSimulator/BridgeLayout.hpp"
#include "IslandSimulator/ButterflyMeadow.hpp"
#include "IslandSimulator/DolphinPod.hpp"
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/LocomotionPaceToggle.hpp"
#include "IslandSimulator/MonkeyTroop.hpp"
#include "IslandSimulator/MonkeyTreeReach.hpp"
#include "IslandSimulator/MonkeyAttachmentSite.hpp"
#include "IslandSimulator/MonkeyTreeExcursion.hpp"
#include "IslandSimulator/MonkeyExcursionClearance.hpp"
#include <chrono>
#include "IslandSimulator/NaturalPropLayout.hpp"
#include "IslandSimulator/OceanMeshLayout.hpp"
#include "IslandSimulator/PerformanceStatistics.hpp"
#include "IslandSimulator/PlayerMotion.hpp"
#include "IslandSimulator/SwimDepth.hpp"
#include "IslandSimulator/ProceduralMeshValidation.hpp"
#include "IslandSimulator/RockSurface.hpp"
#include "IslandSimulator/SurfaceNormals.hpp"
#include "IslandSimulator/RuntimeQualityProfile.hpp"
#include "IslandSimulator/SeaTurtleGroup.hpp"
#include "IslandSimulator/SeaTurtleProfile.hpp"
#include "IslandSimulator/SharkPatrol.hpp"
#include "IslandSimulator/ShipwreckLayout.hpp"
#include "IslandSimulator/ShorelineMeshLayout.hpp"
#include "IslandSimulator/SoundscapeModel.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include "IslandSimulator/StreamSoundField.hpp"
#include "IslandSimulator/TerrainMeshLayout.hpp"
#include "IslandSimulator/TidePoolLayout.hpp"
#include "IslandSimulator/TouchControls.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"
#include "IslandSimulator/ReefHeronGroup.hpp"
#include "IslandSimulator/UserSettings.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include "IslandSimulator/WaterRippleModel.hpp"
#include "IslandSimulator/WaterViewModel.hpp"
#include "IslandSimulator/WorldCollisionGrid.hpp"
#include "IslandSimulator/WorldIdentity.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <numbers>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace
{
    int failures = 0;

    void Check(const bool condition, const std::string_view description)
    {
        if (!condition) {
            std::cerr << "FAIL: " << description << '\n';
            ++failures;
        }
    }

    void CheckNear(const float actual, const float expected, const float tolerance,
                   const std::string_view description)
    {
        Check(std::abs(actual - expected) <= tolerance, description);
    }
}

int main()
{
    {
        const std::string_view fogShader = island::environment::FogShaderDefinition;
        Check(island::environment::FogColorRed == 0.53f
            && island::environment::FogColorGreen == 0.78f
            && island::environment::FogColorBlue == 0.88f
            && fogShader == "const vec3 islandAtmosphereFog=vec3(0.53,0.78,0.88);\n",
            "CPU and portable shader atmosphere fog share one authored colour");
    }
    {
        island::SwimDepth dive;
        Check(dive.EyeHeight()==.58f,"diving starts at the swimming surface");
        for(int step=0;step<200;++step) dive.Update(-12.0f,false,true,.1f);
        Check(std::abs(dive.EyeHeight()+11.55f)<1e-5f,"descent stops above the seabed");
        Check(dive.SubmergedDepth()>11.5f,"diving records actual submerged depth");
        const float held=dive.EyeHeight();
        dive.Update(-12,true,true,.1f);
        dive.Update(-12,false,false,.1f);
        Check(dive.EyeHeight()==held,"opposed or released keys hold depth");
        Check(!dive.Update(std::numeric_limits<float>::quiet_NaN(),true,false,.1f)
            &&dive.EyeHeight()==held,"invalid seabed rejects motion");
        for(float dt:{-1.0f,std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::quiet_NaN()}) {
            dive.Update(-12,true,false,dt);
            Check(dive.EyeHeight()==held,"invalid dive time is neutral");
        }
        dive.Update(-12,true,false,1000);
        Check(std::abs(dive.EyeHeight()-held-.16f)<1e-5f,"dive stalls clamp to one bounded step");
        dive.Update(-2,false,false,0);
        Check(std::abs(dive.EyeHeight()+1.55f)<1e-5f,"rising support cannot bury the swimmer");
        for(int step=0;step<200;++step) dive.Update(-12,true,false,.1f);
        Check(dive.EyeHeight()==.58f&&dive.SubmergedDepth()==0,"ascent cannot leave the surface");
        dive.Update(1,false,true,.1f);
        Check(dive.EyeHeight()==.58f,"dry support does not allow diving");
        dive.Update(-12,false,true,.1f); dive.Reset();
        Check(dive.EyeHeight()==.58f,"teleport/reset clears diving depth");
    }
    {
        // Bent, tapered V-fold strip; three shared vertices per level, including a centre ridge.
        const std::vector<std::array<float,3>> strip{
            {-0.03f,0,0}, {0,0,0.014f}, {0.03f,0,0},
            {-0.025f,0.3f,0.04f}, {0,0.3f,0.052f}, {0.025f,0.3f,0.04f},
            {-0.002f,0.55f,0.15f}, {0,0.55f,0.151f}, {0.002f,0.55f,0.15f}};
        const std::vector<std::uint16_t> indices{0,3,1,1,3,4,1,4,2,2,4,5,
                                               3,6,4,4,6,7,4,7,5,5,7,8};
        const auto rest = island::BuildSurfaceNormals(strip,indices);
        Check(std::abs(rest[1][1]-rest[7][1]) > 0.1f,
              "grass normal changes along the bent centreline");
        Check(rest[0][0]*rest[2][0] < 0,
              "grass fold sides have opposing transverse normal components");
        for (float spread : {0.4f,1.0f,1.6f}) {
            for (float stature : {0.3f,0.84f,1.16f,1.5f}) {
                for (float yaw : {-2.4f,0.0f,1.2f}) {
                    const float c=std::cos(yaw), s=std::sin(yaw);
                    const auto rotate = [c,s](std::array<float,3> v) {
                        return std::array<float,3>{c*v[0]-s*v[2],v[1],s*v[0]+c*v[2]};
                    };
                    std::vector<std::array<float,3>> transformed;
                    for (auto p : strip) transformed.push_back(rotate({p[0]*spread,p[1]*stature,p[2]*spread}));
                    const auto reference = island::BuildSurfaceNormals(transformed,indices);
                    for (std::size_t i=0; i<rest.size(); ++i) {
                        auto n=rotate({rest[i][0]/spread,rest[i][1]/stature,rest[i][2]/spread});
                        const float length=std::hypot(n[0],n[1],n[2]);
                        for (int axis=0; axis<3; ++axis) CheckNear(n[axis]/length,reference[i][axis],
                            0.000003f,"scaled grass normals match independently transformed triangles");
                    }
                }
            }
        }
    }
    {
        const std::vector<std::array<float,3>> positions{{0,0,0},{2,0,0},{0,1,0},{0,0,0.5f}};
        const std::vector<std::uint16_t> indices{0,1,2,0,3,1};
        const auto normals = island::BuildSurfaceNormals(positions,indices);
        CheckNear(normals[0][1], 1/std::sqrt(5.0f), 0.000001f,
                  "shared normal weights the smaller triangle by its area");
        CheckNear(normals[0][2], 2/std::sqrt(5.0f), 0.000001f,
                  "shared normal weights the larger triangle by its area");
        const std::vector<std::array<float,3>> split{positions[0],positions[1],positions[2],
                                                   positions[0],positions[3],positions[1]};
        const auto seam = island::BuildSurfaceNormals(split,std::array<std::uint16_t,6>{0,1,2,3,4,5});
        Check(seam[0] == std::array<float,3>{0,0,1} && seam[3] == std::array<float,3>{0,1,0},
              "duplicate cap/material vertices preserve a hard normal seam");
        auto reversed = indices;
        for (std::size_t i=0; i<reversed.size(); i+=3) std::swap(reversed[i+1],reversed[i+2]);
        const auto reversedNormals = island::BuildSurfaceNormals(positions,reversed);
        Check(island::BuildOutwardSurfaceNormals(positions, indices, normals) == normals,
              "outward validation preserves the computed unit normals");
        const std::array<std::array<float, 3>, 6> wedge{{
            {.25f,.025f,-.13f}, {.25f,.025f,.13f}, {.265f,.64f,0},
            {.60f,.018f,-.037f}, {.60f,.018f,.037f}, {.575f,.06f,0}}};
        const std::array<std::array<float, 3>, 6> wedgeReferences{{
            {0,0,-1}, {0,0,1}, {0,1,0}, {0,0,-1}, {0,0,1}, {0,1,0}}};
        std::array<std::uint16_t, 18> wedgeIndices{0,3,2,2,3,5,2,5,1,1,5,4,0,2,1,3,4,5};
        bool inwardWedgeRejected = false;
        try { (void)island::BuildOutwardSurfaceNormals(wedge, wedgeIndices, wedgeReferences); }
        catch (const std::invalid_argument&) { inwardWedgeRejected = true; }
        Check(inwardWedgeRejected, "historical inward root wedge is rejected");
        for (std::size_t i = 0; i < wedgeIndices.size(); i += 3)
            std::swap(wedgeIndices[i+1], wedgeIndices[i+2]);
        Check(island::BuildOutwardSurfaceNormals(wedge, wedgeIndices, wedgeReferences).size() == 6,
              "open tapered root wedge accepts semantic side and crest references");
        for (int invalid = 0; invalid < 5; ++invalid) {
            auto references = normals;
            if (invalid == 0) references.pop_back();
            if (invalid == 1) references[0] = {0, 0, 0};
            if (invalid == 2) references[0][0] = std::numeric_limits<float>::quiet_NaN();
            if (invalid == 3) references[0] = {1, 0, 0}; // Tangent is not outward.
            bool rejected = false;
            try { (void)island::BuildOutwardSurfaceNormals(positions, invalid == 4 ? reversed : indices, references); }
            catch (const std::invalid_argument&) { rejected = true; }
            Check(rejected, "outward normals reject missing, invalid, tangent and inward references");
        }
        for (std::size_t i=0; i<normals.size(); ++i) {
            CheckNear(std::hypot(normals[i][0],normals[i][1],normals[i][2]),1,0.000001f,
                      "surface normals are finite and unit length");
            for (int axis=0; axis<3; ++axis) CheckNear(reversedNormals[i][axis],-normals[i][axis],
                0.000001f,"normal direction follows triangle winding");
        }
        for (int invalid=0; invalid<7; ++invalid) {
            auto p=positions;
            auto i=indices;
            if (invalid==0) p.clear();
            if (invalid==1) i.pop_back();
            if (invalid==2) i[0]=99;
            if (invalid==3) p[0][0]=std::numeric_limits<float>::infinity();
            if (invalid==4) i[1]=i[0];
            if (invalid==5) p.push_back({3,3,3});
            if (invalid==6) i.insert(i.end(),reversed.begin(),reversed.end());
            bool rejected=false;
            try { (void)island::BuildSurfaceNormals(p,i); }
            catch (const std::invalid_argument&) { rejected=true; }
            Check(rejected,"invalid or undefined normal input is rejected before upload");
        }
    }
    for (int variant = 0; variant < 4; ++variant) {
        for (bool farMesh : {false, true}) {
            const auto rock = island::BuildRockSurface(variant, farMesh);
            std::vector<std::array<float,3>> normalPositions;
            for (const auto& vertex : rock.vertices) normalPositions.push_back(vertex.position);
            const auto sharedNormals = island::BuildSurfaceNormals(normalPositions,rock.indices);
            Check(rock.vertices.size() == (farMesh ? 36U : 265U), "rock LOD has a fixed bounded vertex count");
            Check(rock.indices.size() == (farMesh ? 189U : 1512U), "rock LOD has a fixed bounded index count");
            island::ValidateProceduralTriangleMeshOrThrow(rock.vertices.size(), rock.indices, "rock CPU test");
            std::vector<std::array<double,3>> expected(rock.vertices.size());
            for (std::size_t i = 0; i < rock.indices.size(); i += 3) {
                const auto& a = rock.vertices[rock.indices[i]].position;
                const auto& b = rock.vertices[rock.indices[i+1]].position;
                const auto& c = rock.vertices[rock.indices[i+2]].position;
                const double ux=b[0]-a[0], uy=b[1]-a[1], uz=b[2]-a[2];
                const double vx=c[0]-a[0], vy=c[1]-a[1], vz=c[2]-a[2];
                const std::array n{uy*vz-uz*vy,uz*vx-ux*vz,ux*vy-uy*vx};
                Check(std::hypot(n[0],n[1],n[2]) > 0.00001, "rock triangle is not degenerate");
                for (int corner = 0; corner < 3; ++corner) {
                    for (int axis = 0; axis < 3; ++axis) expected[rock.indices[i+corner]][axis] += n[axis];
                }
            }
            for (std::size_t i = 0; i < rock.vertices.size(); ++i) {
                const auto& p = rock.vertices[i].position;
                const auto& n = rock.vertices[i].normal;
                for (int axis=0; axis<3; ++axis) CheckNear(sharedNormals[i][axis],n[axis],0.000001f,
                    "wood normal builder matches independently generated rock normals");
                Check(std::hypot(p[0],p[2]) <= 1 && p[1] >= 0 && p[1] <= island::RockSurfaceMaximumHeight,
                      "refined rock fits existing collision/culling extents");
                CheckNear(std::hypot(n[0],n[1],n[2]), 1, 0.000001f, "rock normals are finite and unit length");
                const auto& e = expected[i];
                for (int axis=0; axis<3; ++axis) CheckNear(n[axis], e[axis]/std::hypot(e[0],e[1],e[2]),
                    0.000001f, "shared normals match independent area-weighted face accumulation");
                Check(n[0]*p[0] + n[2]*p[2] + n[1]*std::max(0.0f,p[1]-0.6f) > 0,
                      "rock normals face away from the interior");
                for (float yaw : {-2.3f, 0.0f, 1.7f}) {
                    const std::array<double,3> scale{0.4, 3.2, 1.7};
                    const std::array<double,3> tangent{n[1], -n[0], 0};
                    const auto rotate = [yaw](const std::array<double,3>& v) {
                        return std::array{std::cos(yaw)*v[0]+std::sin(yaw)*v[2], v[1],
                                          -std::sin(yaw)*v[0]+std::cos(yaw)*v[2]};
                    };
                    const auto transformedNormal = rotate({n[0]/scale[0],n[1]/scale[1],n[2]/scale[2]});
                    const auto transformedTangent = rotate({tangent[0]*scale[0],tangent[1]*scale[1],tangent[2]*scale[2]});
                    double dot=0;
                    for (int axis=0; axis<3; ++axis) dot += transformedNormal[axis]*transformedTangent[axis];
                    Check(std::abs(dot) < 0.000001, "inverse-scale yaw normal remains perpendicular to transformed surface");
                }
            }
        }
    }
    for (int invalid : {-1,4}) {
        bool rejected=false;
        try { (void)island::BuildRockSurface(invalid,false); }
        catch (const std::invalid_argument&) { rejected=true; }
        Check(rejected,"invalid rock variant is rejected before upload");
    }
    for (float knot : {-22.0f, -3.0f, 0.0f, 0.6f, 1.4f}) {
        const auto lower = island::CoastalSandColor(knot - 0.00001f);
        const auto upper = island::CoastalSandColor(knot + 0.00001f);
        for (std::size_t channel = 0; channel < lower.size(); ++channel) {
            CheckNear(lower[channel], upper[channel], 0.001f,
                      "coastal sand palette has no discontinuity at its depth knots");
        }
    }
    float previousOpacity = 0, previousLagoon = 1, previousCoverage = 0;
    float previousReflection = -1;
    for (int step=0; step<=10000; ++step) {
        const float slope = (static_cast<float>(step)/10000-0.5f)*0.42f;
        const float response = island::CoastalSurfaceAppearance::ReflectionModulation(slope);
        Check(response >= previousReflection && std::abs(response) < 0.54f,
              "supplementary grazing response is monotonic and restrained over the wave slope bound");
        CheckNear(response,-island::CoastalSurfaceAppearance::ReflectionModulation(-slope),0.000001f,
                  "grazing response has no brightness bias");
        previousReflection=response;
    }
    for (float shore : {-20.0f,-4.0f,0.0f,4.0f,20.0f}) {
        float previousFoam=1;
        for (int step=1; step<=3000; ++step) {
            const float depth=step*0.001f;
            const float coverage=island::CoastalSurfaceAppearance::BreakingCoverage(depth,shore);
            Check(coverage >= 0 && coverage <= previousFoam,
                  "calm breaker coverage decreases with depth");
            if (depth >= 1.25f) Check(coverage == 0,"deep water does not generate broad breaker foam");
            CheckNear(coverage,island::CoastalSurfaceAppearance::BreakingCoverage(depth,-shore),0.000001f,
                      "breaker envelope is symmetric about the shoreline");
            previousFoam=coverage;
        }
    }
    Check(island::CoastalSurfaceAppearance::BreakingCoverage(0,0)==0
              && island::CoastalSurfaceAppearance::BreakingCoverage(-1,0)==0
              && island::CoastalSurfaceAppearance::BreakingCoverage(0.1f,0)==1
              && island::CoastalSurfaceAppearance::BreakingCoverage(0.1f,20)<0.007f,
          "breaker foam remains in a bounded wet near-shore region");
    for (int step = 0; step <= 10000; ++step) {
        const float depth = static_cast<float>(step) * 0.003f;
        const float opacity = island::CoastalWaterOptics::BodyOpacity(depth);
        const float lagoon = island::CoastalWaterOptics::LagoonWeight(depth);
        const float coverage = island::CoastalWaterOptics::ContactCoverage(depth);
        Check(opacity >= previousOpacity && opacity <= 0.92f
                  && lagoon <= previousLagoon && lagoon >= 0
                  && coverage >= previousCoverage && coverage <= 1,
              "coastal attenuation, colour and contact coverage are bounded and monotonic");
        previousOpacity = opacity;
        previousLagoon = lagoon;
        previousCoverage = coverage;
        for (float channel : island::CoastalSandColor(-depth)) {
            Check(std::isfinite(channel) && channel >= 0 && channel <= 255,
                  "coastal sand colour stays in byte range");
        }
    }
    Check(island::CoastalWaterOptics::BodyOpacity(0) == 0
              && island::CoastalWaterOptics::ContactCoverage(-1) == 0
              && island::CoastalWaterOptics::ContactCoverage(0.12f) == 1
              && island::CoastalWaterOptics::BodyOpacity(0.1f) < 0.04f
              && island::CoastalWaterOptics::BodyOpacity(10) > 0.9f,
          "thin water reveals sand while deep water attenuates the bottom");
    Check(island::ReefMacroAppearance::LagoonWeight(20, 1.5f) > .98f
              && island::ReefMacroAppearance::LagoonWeight(55, 1.5f) == 0
              && island::ReefMacroAppearance::CrestWeight(74, 3) > .98f
              && island::ReefMacroAppearance::CrestWeight(40, 3) == 0
              && island::ReefMacroAppearance::DropoffWeight(110, 9) == 1
              && island::ReefMacroAppearance::DropoffWeight(60, 9) == 0,
          "reef macro zones separate lagoon, crest, and deep shelf drop-off");
    float maximumReefBreaker = 0;
    float minimumReefBreaker = 1;
    for (int sample = 0; sample < 360; ++sample) {
        const float angle = static_cast<float>(sample)
            * std::numbers::pi_v<float> / 180.0f;
        const float breaker = island::ReefMacroAppearance::BreakerWeight(
            std::cos(angle) * 320.0f, std::sin(angle) * 320.0f, 74.0f, 3.0f);
        maximumReefBreaker = std::max(maximumReefBreaker, breaker);
        minimumReefBreaker = std::min(minimumReefBreaker, breaker);
    }
    Check(maximumReefBreaker > .98f && minimumReefBreaker == 0,
          "reef breaker envelope forms separated angular patches instead of a white ring");
    Check(island::ReefMacroAppearance::BreakerWeight(320, 0, 40, 3) == 0
              && island::ReefMacroAppearance::BreakerWeight(320, 0, 95, 3) == 0,
          "reef breakers remain confined to the shared 62-86 metre crest");
    for (float invalid : {std::numeric_limits<float>::quiet_NaN(),
                           std::numeric_limits<float>::infinity()}) {
        Check(island::CoastalSurfaceAppearance::ReflectionModulation(invalid)==0
                  && island::CoastalSurfaceAppearance::BreakingCoverage(invalid,0)==0
                  && island::CoastalSurfaceAppearance::BreakingCoverage(1,invalid)==0,
              "invalid surface appearance input contributes no artificial contrast");
        Check(std::isfinite(island::CoastalWaterOptics::BodyOpacity(invalid))
                  && island::CoastalWaterOptics::ContactCoverage(invalid) == 0
                  && island::CoastalWaterOptics::LagoonWeight(invalid) == 0,
              "invalid optical inputs produce bounded conservative values");
        Check(island::CoastalSandColor(invalid) == island::CoastalSandColor(-22),
              "invalid coastal height uses the finite deep palette");
        Check(island::ReefMacroAppearance::LagoonWeight(invalid, 1) == 0
                  && island::ReefMacroAppearance::CrestWeight(72, invalid) == 0
                  && island::ReefMacroAppearance::DropoffWeight(invalid, invalid) == 0
                  && island::ReefMacroAppearance::BreakerWeight(invalid, 0, 74, 3) == 0,
              "invalid reef macro inputs contribute no seabed contrast");
    }
    const island::UserSettings defaultSettings;
    Check(defaultSettings.IsValid(), "default user settings are valid");
    Check(defaultSettings.ToText()
              == "schema=1\nrun_mode=walk\ncamera_motion=enabled\n"
                 "audio=enabled\ntouch_look=standard\n",
          "default user settings use a stable human-readable representation");
    const island::UserSettingsParseResult parsedSettings = island::UserSettings::Parse(
        "# portable preferences\n"
        "schema = 1\nunknown_future_key = retained_elsewhere\n"
        "touch_look=high\naudio=muted\ncamera_motion=disabled\nrun_mode=run\n");
    Check(parsedSettings.IsValid()
              && parsedSettings.settings.runningSelected
              && !parsedSettings.settings.cameraMotionEnabled
              && parsedSettings.settings.audioMuted
              && parsedSettings.settings.touchLookSensitivity
                  == island::TouchLookSensitivityLevel::High,
          "settings parser accepts reordered values and ignores forward-compatible keys");
    Check(island::UserSettings::Parse(parsedSettings.settings.ToText()).settings
              == parsedSettings.settings,
          "settings serialization round-trips every current preference");
    Check(!island::UserSettings::Parse("run_mode=run\n").IsValid(),
          "settings parser requires an explicit schema");
    Check(!island::UserSettings::Parse("schema=2\n").IsValid(),
          "settings parser rejects unsupported schemas");
    Check(!island::UserSettings::Parse(
              "schema=1\nrun_mode=walk\ncamera_motion=enabled\naudio=enabled\n").IsValid(),
          "settings parser rejects truncated records with missing required keys");
    Check(!island::UserSettings::Parse(
              "schema=1\nrun_mode=walk\nrun_mode=run\n").IsValid(),
          "settings parser rejects duplicate known keys");
    Check(!island::UserSettings::Parse("schema=1\naudio=loud\n").IsValid(),
          "settings parser rejects invalid preference values");
    Check(!island::UserSettings::Parse(std::string(
              island::UserSettings::MaximumSerializedBytes + 1U, 'x')).IsValid(),
          "settings parser rejects oversized input before scanning it");
    std::string settingsWithNul = "schema=1\n";
    settingsWithNul.push_back('\0');
    Check(!island::UserSettings::Parse(settingsWithNul).IsValid(),
          "settings parser rejects embedded NUL bytes");

    Check(island::GeneratedTextureBudget::TotalPixelCount == 1195811U,
          "generated texture inventory retains its exact base-level pixel count");
    Check(island::GeneratedTextureBudget::TotalRgba8Bytes == 4783244U,
          "generated texture inventory stays below its audited RGBA8 payload");
    constexpr island::RuntimeQualityProfile fullQuality =
        island::RuntimeQualityProfile::Full();
    constexpr island::RuntimeQualityProfile mobileQuality =
        island::RuntimeQualityProfile::Mobile();
    constexpr island::RuntimeQualityProfile nativeQuality =
        island::RuntimeQualityProfile::NativePerformance();
    Check(fullQuality.IsValid() && mobileQuality.IsValid() && nativeQuality.IsValid(),
          "runtime quality profiles expose valid renderer settings");
    Check(fullQuality.name == "full" && mobileQuality.name == "mobile"
              && nativeQuality.name == "native-performance",
          "runtime quality profiles retain stable diagnostic names");
    CheckNear(fullQuality.grassMaximumDistance, 135.0f, 0.0f,
              "full quality retains the authored grass distance");
    CheckNear(mobileQuality.grassMaximumDistance, 112.0f, 0.0f,
              "mobile quality reduces only distant grass work");
    CheckNear(nativeQuality.grassMaximumDistance, 72.0f, 0.0f,
              "native performance quality strongly bounds distant grass work");
    Check(nativeQuality.vegetationMaximumDistanceScale
              < mobileQuality.vegetationMaximumDistanceScale,
          "native performance quality bounds elevated-view vegetation");
    Check(mobileQuality.contactShadowDistanceScale < fullQuality.contactShadowDistanceScale
              && mobileQuality.wildlifeMaximumDistanceScale
                  < fullQuality.wildlifeMaximumDistanceScale,
          "mobile quality bounds distant shadows and wildlife");
    Check(island::GeneratedTextureBudget::TotalRuntimeRgba8Bytes == 10091644U,
          "runtime texture budget must include floor, leaf, wildlife, hut, cave, resident, helicopter and bear mip chains");
    Check(mobileQuality.textureByteBudget
              >= island::GeneratedTextureBudget::TotalRuntimeRgba8Bytes,
          "mobile quality admits the compact generated texture inventory");
    Check(mobileQuality.maximumPlayingAudioSources == 6,
          "mobile quality preserves the bounded full soundscape");
    Check(fullQuality.preferredBackBufferSamples == 4 && mobileQuality.preferredBackBufferSamples == 2,
          "portable antialiasing requests bound full and mobile sample counts");
    Check(nativeQuality.preferredBackBufferSamples == 0
              && nativeQuality.oceanQuadsPerAxis == 96,
          "native performance quality removes multisampling and reduces the ocean grid");
    for (const int samples : {-1, 1, 3, 8, 1024}) {
        auto invalid = fullQuality;
        invalid.preferredBackBufferSamples = samples;
        Check(!invalid.IsValid(), "quality policy rejects unsupported or unbounded sample requests");
    }
    auto noAntialiasing = fullQuality;
    noAntialiasing.preferredBackBufferSamples = 0;
    Check(noAntialiasing.IsValid(), "quality policy admits explicit single-sample fallback");

    const island::AquaticLightProfile shallowLight = island::AquaticLightAt(
        7.0f, -0.35f, -4.0f, 2.0f);
    const island::AquaticLightProfile middleLight = island::AquaticLightAt(
        7.0f, -4.0f, -4.0f, 2.0f);
    const island::AquaticLightProfile deepLight = island::AquaticLightAt(
        7.0f, -8.0f, -4.0f, 2.0f);
    CheckNear(shallowLight.waterTint, 0.04f, 0.0001f,
              "shallow aquatic lighting retains only the minimum water tint");
    Check(middleLight.waterTint > shallowLight.waterTint
              && deepLight.waterTint > middleLight.waterTint,
          "aquatic water tint increases smoothly with depth");
    CheckNear(deepLight.waterTint, 0.26f, 0.0001f,
              "deep aquatic lighting reaches its bounded water tint");
    CheckNear(deepLight.brightness, 0.88f, 0.0001f,
              "deep aquatic lighting suppresses shallow sunlight bands");
    const island::AquaticLightProfile movingShallowLight = island::AquaticLightAt(
        7.0f, -0.35f, -4.0f, 8.0f);
    Check(std::abs(movingShallowLight.brightness - shallowLight.brightness) > 0.001f,
          "shallow aquatic sunlight changes over time");
    Check(shallowLight.brightness >= 0.88f && shallowLight.brightness <= 1.12f
              && middleLight.brightness >= 0.88f && middleLight.brightness <= 1.12f,
          "aquatic sunlight brightness remains inside its renderer colour budget");
    const island::AquaticLightProfile invalidLight = island::AquaticLightAt(
        std::numeric_limits<float>::quiet_NaN(), -1.0f, 0.0f, 1.0f);
    CheckNear(invalidLight.waterTint, 0.0f, 0.0f,
              "non-finite aquatic position fails closed to neutral tint");
    CheckNear(invalidLight.brightness, 1.0f, 0.0f,
              "non-finite aquatic position fails closed to neutral brightness");
    const island::AquaticLightProfile invalidTimeLight = island::AquaticLightAt(
        0.0f, -1.0f, 0.0f, std::numeric_limits<float>::infinity());
    CheckNear(invalidTimeLight.waterTint, 0.0f, 0.0f,
              "non-finite aquatic clock fails closed to neutral tint");
    CheckNear(invalidTimeLight.brightness, 1.0f, 0.0f,
              "non-finite aquatic clock fails closed to neutral brightness");

    island::AmbientLoop knownSignal;
    knownSignal.sampleRate = 4;
    knownSignal.samples = {0, 16384, -16384, 0};
    const island::AudioSignalStatistics knownStatistics =
        island::AnalyzeAudioSignal(knownSignal);
    Check(knownStatistics.finite && knownStatistics.sampleCount == 4U
              && knownStatistics.sampleRate == 4,
          "audio analysis accepts a finite PCM16 signal");
    CheckNear(static_cast<float>(knownStatistics.durationSeconds), 1.0f, 0.0001f,
              "audio analysis reports duration from sample count and rate");
    CheckNear(static_cast<float>(knownStatistics.mean), 0.0f, 0.0001f,
              "audio analysis reports normalized DC offset");
    CheckNear(static_cast<float>(knownStatistics.rootMeanSquare), 0.353553f, 0.0001f,
              "audio analysis reports normalized RMS energy");
    CheckNear(static_cast<float>(knownStatistics.peak), 0.5f, 0.0001f,
              "audio analysis reports normalized peak magnitude");
    CheckNear(static_cast<float>(knownStatistics.seamStep), 0.0f, 0.0001f,
              "audio analysis reports the loop seam step");
    CheckNear(static_cast<float>(knownStatistics.maximumInternalStep), 1.0f, 0.0001f,
              "audio analysis reports the largest adjacent PCM step");
    Check(!island::AnalyzeAudioSignal({}).finite,
          "audio analysis rejects an empty signal without a sample rate");

    const island::WorldSeedSet worldSeeds = island::AuthoritativeWorldSeeds();
    Check(worldSeeds == island::WorldSeedSet{0x7a11d5e3U, 0x6b4d2f91U,
                                             0x43b0a7d1U, 0xa7816c39U},
          "world identity exposes the four authoritative layout seeds");
    Check(island::WorldSeedOverlayHex(0x00001a2bU) == "00001A2B",
          "world seed overlay formatting is fixed-width uppercase hexadecimal");
    Check(island::WorldSeedClipboardText(worldSeeds)
              == "island-simulator world seeds\n"
                 "palms=0x7a11d5e3\n"
                 "grass=0x6b4d2f91\n"
                 "jungle=0x43b0a7d1\n"
                 "natural-props=0xa7816c39\n",
          "world seed clipboard text is stable and report-friendly");

    constexpr std::array<std::uint16_t, 3> validTriangle = {0U, 1U, 2U};
    bool validProceduralMeshAccepted = true;
    try {
        island::ValidateProceduralTriangleMeshOrThrow(
            3U, validTriangle, "test-valid");
        island::ValidateProceduralTriangleMeshOrThrow(
            65'536U, std::array<std::uint16_t, 3>{65'535U, 1U, 0U},
            "test-maximum");
    } catch (const std::runtime_error&) {
        validProceduralMeshAccepted = false;
    }
    Check(validProceduralMeshAccepted,
          "procedural mesh validation accepts complete in-range 16-bit triangle lists");

    const auto rejectsProceduralMesh = [](const std::size_t vertexCount,
                                           const std::span<const std::uint16_t> indices,
                                           const std::string_view expectedDiagnostic) {
        try {
            island::ValidateProceduralTriangleMeshOrThrow(
                vertexCount, indices, "test-invalid");
        } catch (const std::runtime_error& exception) {
            return std::string_view(exception.what()).find(expectedDiagnostic)
                != std::string_view::npos;
        }
        return false;
    };
    Check(rejectsProceduralMesh(0U, validTriangle, "no vertices")
              && rejectsProceduralMesh(3U, {}, "no triangle indices")
              && rejectsProceduralMesh(
                  3U, std::array<std::uint16_t, 2>{0U, 1U}, "complete triangle list")
              && rejectsProceduralMesh(
                  3U, std::array<std::uint16_t, 3>{0U, 1U, 3U}, "out-of-range")
              && rejectsProceduralMesh(65'537U, validTriangle, "16-bit vertex limit"),
          "procedural mesh validation rejects every malformed portable index boundary");

    bool validInstanceRangesAccepted = true;
    try {
        island::ValidateInstancedDrawRangeOrThrow(8, 0, 8, "test-full-instance");
        island::ValidateInstancedDrawRangeOrThrow(8, 3, 5, "test-sliced-instance");
    } catch (const std::runtime_error&) {
        validInstanceRangesAccepted = false;
    }
    Check(validInstanceRangesAccepted,
          "instanced draw validation accepts complete and sliced in-range buffers");

    const auto rejectsInstanceRange = [](const int capacity, const int offset,
                                         const int count,
                                         const std::string_view expectedDiagnostic) {
        try {
            island::ValidateInstancedDrawRangeOrThrow(
                capacity, offset, count, "test-invalid-instance");
        } catch (const std::runtime_error& exception) {
            return std::string_view(exception.what()).find(expectedDiagnostic)
                != std::string_view::npos;
        }
        return false;
    };
    Check(rejectsInstanceRange(-1, 0, 1, "negative instance-buffer capacity")
              && rejectsInstanceRange(8, -1, 1, "negative instance-buffer offset")
              && rejectsInstanceRange(8, 0, 0, "non-positive instance count")
              && rejectsInstanceRange(8, 7, 2, "beyond the instance buffer")
              && rejectsInstanceRange(8, 9, 1, "beyond the instance buffer"),
          "instanced draw validation rejects every out-of-buffer range shape");

    const island::BenchmarkTraversalRoute benchmarkRoute;
    const island::BenchmarkRouteSample routeStart = benchmarkRoute.Sample(0.0f);
    const island::BenchmarkRouteSample routeEnd = benchmarkRoute.Sample(1.0f);
    CheckNear(routeStart.x, benchmarkRoute.Waypoints().front().x, 0.0001f,
              "benchmark route starts at its first authored waypoint");
    CheckNear(routeStart.z, benchmarkRoute.Waypoints().front().z, 0.0001f,
              "benchmark route start retains its authored depth");
    CheckNear(routeEnd.x, benchmarkRoute.Waypoints().back().x, 0.0001f,
              "benchmark route ends at its last authored waypoint");
    CheckNear(routeEnd.z, benchmarkRoute.Waypoints().back().z, 0.0001f,
              "benchmark route end retains its authored depth");
    CheckNear(benchmarkRoute.Sample(-1.0f).z, routeStart.z, 0.0001f,
              "benchmark route clamps negative progress");
    CheckNear(benchmarkRoute.Sample(2.0f).z, routeEnd.z, 0.0001f,
              "benchmark route clamps progress beyond its end");
    CheckNear(benchmarkRoute.Sample(std::numeric_limits<float>::quiet_NaN()).z,
              routeStart.z, 0.0001f,
              "benchmark route fails closed on non-finite progress");
    const float benchmarkRouteLength = benchmarkRoute.ApproximateLength();
    Check(benchmarkRouteLength > 600.0f && benchmarkRouteLength < 850.0f,
          "benchmark route has a sustained but island-scale horizontal length");
    float minimumRouteShoreDistance = std::numeric_limits<float>::max();
    float maximumRouteShoreDistance = std::numeric_limits<float>::lowest();
    const island::IslandTerrain routeTerrain;
    for (int index = 0; index <= 600; ++index) {
        const island::BenchmarkRouteSample sample = benchmarkRoute.Sample(
            static_cast<float>(index) / 600.0f);
        Check(std::isfinite(sample.x) && std::isfinite(sample.z)
                  && std::isfinite(sample.forwardX) && std::isfinite(sample.forwardZ),
              "benchmark route samples remain finite");
        CheckNear(std::hypot(sample.forwardX, sample.forwardZ), 1.0f, 0.0001f,
                  "benchmark route look direction remains normalized");
        Check(std::hypot(sample.x, sample.z) < 270.0f,
              "benchmark route stays within the island and near-shore profiling envelope");
        const float shoreDistance = routeTerrain.SignedShoreDistance(sample.x, sample.z);
        minimumRouteShoreDistance = std::min(minimumRouteShoreDistance, shoreDistance);
        maximumRouteShoreDistance = std::max(maximumRouteShoreDistance, shoreDistance);
    }
    Check(minimumRouteShoreDistance < -3.0f,
          "benchmark route includes shallow-water renderer coverage");
    Check(maximumRouteShoreDistance > 120.0f,
          "benchmark route includes deep-interior renderer coverage");

    const std::array<double, 5> frameTimes = {50.0, 10.0, 30.0, 20.0, 40.0};
    const island::FrameTimeSummary frameSummary =
        island::CalculateFrameTimeSummary(frameTimes);
    Check(frameSummary.sampleCount == frameTimes.size(),
          "frame-time statistics retain their sample count");
    CheckNear(static_cast<float>(frameSummary.minimumMilliseconds), 10.0f, 0.0001f,
              "frame-time statistics report the minimum");
    CheckNear(static_cast<float>(frameSummary.maximumMilliseconds), 50.0f, 0.0001f,
              "frame-time statistics report the maximum");
    CheckNear(static_cast<float>(frameSummary.meanMilliseconds), 30.0f, 0.0001f,
              "frame-time statistics report the arithmetic mean");
    CheckNear(static_cast<float>(frameSummary.medianMilliseconds), 30.0f, 0.0001f,
              "frame-time statistics report the interpolated median");
    CheckNear(static_cast<float>(frameSummary.percentile95Milliseconds), 48.0f, 0.0001f,
              "frame-time statistics report the interpolated 95th percentile");
    CheckNear(static_cast<float>(frameSummary.percentile99Milliseconds), 49.6f, 0.0001f,
              "frame-time statistics report the interpolated 99th percentile");
    CheckNear(static_cast<float>(frameSummary.meanFramesPerSecond), 33.3333f, 0.001f,
              "frame-time statistics derive mean frames per second");
    Check(frameSummary.framesOver120HzBudget == 5U,
          "frame-time statistics count samples outside a 120 Hz budget");
    Check(frameSummary.framesOver60HzBudget == 4U,
          "frame-time statistics count samples outside a 60 Hz budget");
    Check(frameSummary.framesOver30HzBudget == 2U,
          "frame-time statistics count samples outside a 30 Hz budget");
    bool rejectedEmptyFrameTimes = false;
    try {
        (void)island::CalculateFrameTimeSummary(std::span<const double>());
    } catch (const std::invalid_argument&) {
        rejectedEmptyFrameTimes = true;
    }
    Check(rejectedEmptyFrameTimes, "frame-time statistics reject an empty sample set");
    bool rejectedInvalidFrameTime = false;
    const std::array<double, 1> invalidFrameTime = {
        std::numeric_limits<double>::quiet_NaN()};
    try {
        (void)island::CalculateFrameTimeSummary(invalidFrameTime);
    } catch (const std::invalid_argument&) {
        rejectedInvalidFrameTime = true;
    }
    Check(rejectedInvalidFrameTime,
          "frame-time statistics reject non-finite sample durations");

    const island::FirstPersonMovementIntent forwardIntent =
        island::FirstPersonInput::Resolve(
            island::FirstPersonDigitalInput{true, false, false, false, false},
            0.0f, 0.0f, 0.0f);
    CheckNear(forwardIntent.worldX, 0.0f, 0.0001f,
              "first-person forward input has no lateral drift at zero yaw");
    CheckNear(forwardIntent.worldZ, 1.0f, 0.0001f,
              "first-person forward input follows zero yaw");
    const island::FirstPersonMovementIntent rightIntent =
        island::FirstPersonInput::Resolve(
            island::FirstPersonDigitalInput{false, false, false, true, false},
            0.0f, 0.0f, 0.0f);
    CheckNear(rightIntent.worldX, -1.0f, 0.0001f,
              "first-person right input follows screen-right in the view convention");
    CheckNear(rightIntent.worldZ, 0.0f, 0.0001f,
              "first-person right input has no forward drift at zero yaw");
    const island::FirstPersonMovementIntent leftIntent =
        island::FirstPersonInput::Resolve(
            island::FirstPersonDigitalInput{false, false, true, false, false},
            0.0f, 0.0f, 0.0f);
    CheckNear(leftIntent.worldX, 1.0f, 0.0001f,
              "first-person left input opposes screen-right in the view convention");
    const island::FirstPersonMovementIntent rotatedRunIntent =
        island::FirstPersonInput::Resolve(
            island::FirstPersonDigitalInput{true, false, false, false, true},
            0.0f, 0.0f, -std::numbers::pi_v<float> * 0.5f);
    CheckNear(rotatedRunIntent.worldX, -1.0f, 0.0001f,
              "first-person forward intent immediately follows changed mouse yaw");
    CheckNear(rotatedRunIntent.worldZ, 0.0f, 0.0001f,
              "first-person changed-yaw intent remains normalized");
    Check(rotatedRunIntent.run, "first-person intent preserves the selected run mode");
    const island::FirstPersonMovementIntent invalidAnalogIntent =
        island::FirstPersonInput::Resolve(
            island::FirstPersonDigitalInput{},
            std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::quiet_NaN());
    CheckNear(invalidAnalogIntent.worldX, 0.0f, 0.0001f,
              "first-person input rejects non-finite lateral values");
    CheckNear(invalidAnalogIntent.worldZ, 0.0f, 0.0001f,
              "first-person input rejects non-finite forward values");

    island::LocomotionPaceToggle paceToggle;
    Check(!paceToggle.IsRunningSelected(),
          "locomotion pace begins in walking mode");
    Check(paceToggle.Update(true) && paceToggle.IsRunningSelected(),
          "first Shift edge selects persistent running mode");
    Check(!paceToggle.Update(true) && paceToggle.IsRunningSelected(),
          "holding Shift does not repeatedly toggle the selected pace");
    Check(!paceToggle.Update(false) && paceToggle.IsRunningSelected(),
          "releasing Shift retains the selected running mode");
    Check(paceToggle.Update(true) && !paceToggle.IsRunningSelected(),
          "second Shift edge restores persistent walking mode");
    Check(!paceToggle.Update(false) && !paceToggle.IsRunningSelected(),
          "walking mode remains selected after the second release");
    Check(!paceToggle.Update(true, false) && !paceToggle.IsRunningSelected(),
          "debug free-camera Shift does not change player pace");
    Check(!paceToggle.Update(true, true) && !paceToggle.IsRunningSelected(),
          "a held debug-camera Shift cannot leak into player pace selection");
    Check(!paceToggle.Update(false, true) && !paceToggle.IsRunningSelected(),
          "releasing the debug-camera Shift rearms the player toggle");
    Check(paceToggle.Update(true, true) && paceToggle.IsRunningSelected(),
          "a fresh Shift edge works after leaving the debug camera");
    island::LocomotionPaceToggle restoredPaceToggle;
    restoredPaceToggle.SetRunningSelected(true);
    Check(restoredPaceToggle.IsRunningSelected()
              && !restoredPaceToggle.Update(false),
          "persisted run selection restores without synthesizing a Shift edge");

    island::DebugFreeCamera freeCamera;
    freeCamera.Update(0.0f, island::DebugFreeCameraInput{1.0f, 0.0f, 0.0f, false}, 0.1f);
    Check(!freeCamera.IsActive()
              && freeCamera.Position().x == 0.0f
              && freeCamera.Position().y == 0.0f
              && freeCamera.Position().z == 0.0f,
          "inactive debug free camera ignores movement input");
    freeCamera.Activate(10.0f, 20.0f, 30.0f);
    freeCamera.Update(0.0f, island::DebugFreeCameraInput{1.0f, 0.0f, 0.0f, false}, 1.0f);
    CheckNear(freeCamera.Position().x, 10.0f, 0.0001f,
              "debug free camera forward input follows yaw without lateral drift");
    CheckNear(freeCamera.Position().z, 31.2f, 0.0001f,
              "debug free camera clamps long frames before movement");
    const island::DebugFreeCameraPosition beforeDiagonal = freeCamera.Position();
    freeCamera.Update(
        0.0f, island::DebugFreeCameraInput{1.0f, 1.0f, 1.0f, false}, 0.1f);
    CheckNear(std::hypot(
                  freeCamera.Position().y - beforeDiagonal.y,
                  std::hypot(freeCamera.Position().x - beforeDiagonal.x,
                             freeCamera.Position().z - beforeDiagonal.z)),
              island::DebugFreeCamera::BaseSpeed * 0.1f, 0.0001f,
              "debug free camera normalizes three-axis input");
    freeCamera.Activate(10.0f, 20.0f, 30.0f);
    freeCamera.Update(0.0f, island::DebugFreeCameraInput{0.0f, 1.0f, 0.0f, false}, 0.1f);
    CheckNear(freeCamera.Position().x, 8.8f, 0.0001f,
              "debug free camera right input follows the visible view convention");
    CheckNear(freeCamera.Position().z, 30.0f, 0.0001f,
              "debug free camera right input has no forward drift");
    const float beforeFastX = freeCamera.Position().x;
    freeCamera.Update(
        std::numbers::pi_v<float> * 0.5f,
        island::DebugFreeCameraInput{1.0f, 0.0f, 0.0f, true}, 0.1f);
    CheckNear(freeCamera.Position().x - beforeFastX,
              island::DebugFreeCamera::BaseSpeed
                  * island::DebugFreeCamera::FastMultiplier * 0.1f,
              0.0001f, "debug free camera fast mode preserves yaw-relative motion");
    freeCamera.Activate(900.0f, -90.0f, -900.0f);
    CheckNear(freeCamera.Position().x, island::DebugFreeCamera::HorizontalExtent, 0.0001f,
              "debug free camera activation clamps positive world extent");
    CheckNear(freeCamera.Position().y, island::DebugFreeCamera::MinimumHeight, 0.0001f,
              "debug free camera activation clamps minimum inspection height");
    CheckNear(freeCamera.Position().z, -island::DebugFreeCamera::HorizontalExtent, 0.0001f,
              "debug free camera activation clamps negative world extent");
    freeCamera.Deactivate();
    const island::DebugFreeCameraPosition retainedFreePosition = freeCamera.Position();
    freeCamera.Update(0.0f, island::DebugFreeCameraInput{1.0f, 1.0f, 1.0f, true}, 0.1f);
    Check(!freeCamera.IsActive()
              && freeCamera.Position().x == retainedFreePosition.x
              && freeCamera.Position().y == retainedFreePosition.y
              && freeCamera.Position().z == retainedFreePosition.z,
          "deactivated debug free camera retains its last bounded position");

    island::FirstPersonCameraMotion cameraMotion;
    CheckNear(cameraMotion.Offset().lateralMeters, 0.0f, 0.000001f,
              "camera motion starts centred laterally");
    CheckNear(cameraMotion.Offset().verticalMeters, 0.0f, 0.000001f,
              "camera motion starts centred vertically");
    float largestWalkingLateral = 0.0f;
    float largestWalkingVertical = 0.0f;
    for (int frame = 0; frame < 240; ++frame) {
        const island::CameraMotionOffset offset = cameraMotion.Update(
            island::PlayerMotion::WalkSpeed, island::LocomotionMode::Walking, 1.0f / 60.0f);
        largestWalkingLateral = std::max(largestWalkingLateral,
                                         std::abs(offset.lateralMeters));
        largestWalkingVertical = std::max(largestWalkingVertical,
                                          std::abs(offset.verticalMeters));
        Check(std::abs(offset.lateralMeters)
                  <= island::FirstPersonCameraMotion::MaximumLateralOffset + 0.000001f,
              "walking camera motion stays inside its lateral comfort bound");
        Check(std::abs(offset.verticalMeters)
                  <= island::FirstPersonCameraMotion::MaximumVerticalOffset + 0.000001f,
              "walking camera motion stays inside its vertical comfort bound");
    }
    Check(largestWalkingLateral > 0.005f && largestWalkingVertical > 0.009f,
          "full-speed walking produces restrained but nonzero camera motion");
    island::FirstPersonCameraMotion runningCameraMotion;
    float largestRunningLateral = 0.0f;
    float largestRunningVertical = 0.0f;
    for (int frame = 0; frame < 240; ++frame) {
        const island::CameraMotionOffset offset = runningCameraMotion.Update(
            island::PlayerMotion::RunSpeed, island::LocomotionMode::Walking, 1.0f / 60.0f);
        largestRunningLateral = std::max(largestRunningLateral,
                                        std::abs(offset.lateralMeters));
        largestRunningVertical = std::max(largestRunningVertical,
                                         std::abs(offset.verticalMeters));
        Check(std::abs(offset.lateralMeters)
                  <= island::FirstPersonCameraMotion::MaximumLateralOffset + 0.000001f
                  && std::abs(offset.verticalMeters)
                  <= island::FirstPersonCameraMotion::MaximumVerticalOffset + 0.000001f,
              "running camera motion stays inside both comfort bounds");
    }
    Check(largestRunningLateral > largestWalkingLateral
              && largestRunningVertical > largestWalkingVertical,
          "running has a stronger camera-motion signature than walking");
    for (int frame = 0; frame < 180; ++frame) {
        (void)cameraMotion.Update(island::PlayerMotion::SwimSpeed,
                                  island::LocomotionMode::Swimming, 1.0f / 60.0f);
    }
    Check(cameraMotion.SwimBlend() > 0.99f,
          "camera motion smoothly reaches its swimming style");
    Check(std::abs(cameraMotion.Offset().lateralMeters)
              <= island::FirstPersonCameraMotion::MaximumLateralOffset + 0.000001f
              && std::abs(cameraMotion.Offset().verticalMeters)
              <= island::FirstPersonCameraMotion::MaximumVerticalOffset + 0.000001f,
          "swimming camera motion stays inside both comfort bounds");
    cameraMotion.SetEnabled(false);
    for (int frame = 0; frame < 180; ++frame) {
        (void)cameraMotion.Update(island::PlayerMotion::WalkSpeed,
                                  island::LocomotionMode::Walking, 1.0f / 60.0f);
    }
    Check(cameraMotion.MotionStrength() < 0.00001f
              && std::abs(cameraMotion.Offset().lateralMeters) < 0.000001f
              && std::abs(cameraMotion.Offset().verticalMeters) < 0.000001f,
          "disabling camera motion smoothly returns to the physical eye point");
    island::FirstPersonCameraMotion clampedCameraMotion;
    island::FirstPersonCameraMotion referenceCameraMotion;
    const island::CameraMotionOffset clampedOffset = clampedCameraMotion.Update(
        island::PlayerMotion::WalkSpeed, island::LocomotionMode::Walking, 5.0f);
    const island::CameraMotionOffset referenceOffset = referenceCameraMotion.Update(
        island::PlayerMotion::WalkSpeed, island::LocomotionMode::Walking, 0.1f);
    CheckNear(clampedOffset.lateralMeters, referenceOffset.lateralMeters, 0.000001f,
              "camera motion clamps a long frame before phase integration");
    CheckNear(clampedOffset.verticalMeters, referenceOffset.verticalMeters, 0.000001f,
              "camera motion long-frame clamp is deterministic");
    cameraMotion.Reset();
    CheckNear(cameraMotion.MotionStrength(), 0.0f, 0.000001f,
              "camera motion reset clears residual movement strength");

    CheckNear(island::GrassInteractionModel::TargetStrength(0.0f), 0.0f, 0.000001f,
              "stationary grass interaction retains only the shader's body-radius bend");
    CheckNear(island::GrassInteractionModel::TargetStrength(-4.0f), 0.0f, 0.000001f,
              "negative grass interaction speed fails closed");
    CheckNear(island::GrassInteractionModel::TargetStrength(
                  std::numeric_limits<float>::quiet_NaN()),
              0.0f, 0.000001f,
              "non-finite grass interaction speed fails closed");
    const float walkingGrassInteraction =
        island::GrassInteractionModel::TargetStrength(island::PlayerMotion::WalkSpeed);
    CheckNear(walkingGrassInteraction, 1.0f, 0.000001f,
              "full walking speed reaches the bounded grass interaction envelope");
    island::GrassInteractionModel grassInteraction;
    const float firstGrassInteractionStep = grassInteraction.Update(
        island::PlayerMotion::WalkSpeed, 1.0f / 60.0f);
    Check(firstGrassInteractionStep > 0.0f && firstGrassInteractionStep < 1.0f,
          "grass interaction eases in instead of appearing in one frame");
    const float beforeLongGrassFrame = grassInteraction.Strength();
    (void)grassInteraction.Update(island::PlayerMotion::WalkSpeed, 5.0f);
    Check(grassInteraction.Strength() - beforeLongGrassFrame
              <= island::GrassInteractionModel::EntryRatePerSecond
                    * island::GrassInteractionModel::MaximumStepSeconds + 0.000001f,
          "grass interaction clamps a long frame");
    grassInteraction.Reset(island::PlayerMotion::WalkSpeed);
    CheckNear(grassInteraction.Strength(), 1.0f, 0.000001f,
              "grass interaction reset supports deterministic QA placement");
    const float beforeInvalidGrassDelta = grassInteraction.Strength();
    (void)grassInteraction.Update(0.0f, std::numeric_limits<float>::quiet_NaN());
    CheckNear(grassInteraction.Strength(), beforeInvalidGrassDelta, 0.000001f,
              "grass interaction rejects a non-finite frame duration safely");
    for (int frame = 0; frame < 180; ++frame) {
        (void)grassInteraction.Update(0.0f, 1.0f / 60.0f);
    }
    CheckNear(grassInteraction.Strength(), 0.0f, 0.000001f,
              "grass interaction releases fully after movement stops");

    CheckNear(island::WaterViewModel::TargetIntensity(false, -2.0f, 1.0f), 0.0f, 0.000001f,
              "water view remains absent outside swimming mode");
    const float shallowWaterView = island::WaterViewModel::TargetIntensity(true, -0.48f, 0.0f);
    const float deepWaterView = island::WaterViewModel::TargetIntensity(true, -2.5f, 0.0f);
    const float currentWaterView = island::WaterViewModel::TargetIntensity(true, -2.5f, 1.0f);
    Check(shallowWaterView >= 0.60f && shallowWaterView < 0.70f,
          "near-shore swimming receives a restrained minimum water-view cue");
    Check(deepWaterView > shallowWaterView && deepWaterView < 0.80f,
          "deeper swimming strengthens the restrained near-shore water-view cue");
    CheckNear(currentWaterView, 1.0f, 0.000001f,
              "full offshore current makes the peripheral warning unambiguous but bounded");
    CheckNear(island::WaterViewModel::TargetIntensity(
                  true, -2.5f, std::numeric_limits<float>::quiet_NaN()),
              deepWaterView, 0.000001f,
              "non-finite current input falls back to the calm-water presentation");
    island::WaterViewModel waterView;
    const float firstWaterViewStep = waterView.Update(true, -0.48f, 0.0f, 1.0f / 60.0f);
    Check(firstWaterViewStep > 0.0f && firstWaterViewStep < shallowWaterView,
          "water-view entry fades instead of appearing in one frame");
    const float beforeLongWaterFrame = waterView.Intensity();
    (void)waterView.Update(true, -2.5f, 1.0f, 5.0f);
    Check(waterView.Intensity() - beforeLongWaterFrame
              <= island::WaterViewModel::EntryRatePerSecond
                    * island::WaterViewModel::MaximumStepSeconds + 0.000001f,
          "water-view entry clamps long frames");
    waterView.Reset(true, -2.5f, 1.0f);
    CheckNear(waterView.Intensity(), currentWaterView, 0.000001f,
              "water-view reset snaps deterministic QA placement to its target");
    for (int frame = 0; frame < 180; ++frame) {
        (void)waterView.Update(false, -2.5f, 1.0f, 1.0f / 60.0f);
    }
    CheckNear(waterView.Intensity(), 0.0f, 0.000001f,
              "water-view exit fades fully back to a clear dry view");
    const float beforeInvalidWaterDelta = waterView.Intensity();
    (void)waterView.Update(true, -1.0f, 0.0f,
                           std::numeric_limits<float>::quiet_NaN());
    CheckNear(waterView.Intensity(), beforeInvalidWaterDelta, 0.000001f,
              "water-view transition rejects a non-finite frame duration safely");

    CheckNear(island::WaterRippleModel::TargetIntensity(false, 1.0f, 3.0f),
              0.0f, 0.000001f,
              "dry movement does not create an ocean-surface ripple");
    CheckNear(island::WaterRippleModel::TargetIntensity(true, -1.0f, 0.0f),
              0.0f, 0.000001f,
              "stationary swimming does not create a perpetual ripple");
    const float wadingRipple = island::WaterRippleModel::TargetIntensity(
        false, -0.20f, 1.0f);
    const float swimmingRipple = island::WaterRippleModel::TargetIntensity(
        true, -1.0f, 2.0f);
    Check(wadingRipple > 0.15f && wadingRipple < 0.35f,
          "moving through shallow water creates a restrained wading ripple");
    CheckNear(swimmingRipple, 1.0f, 0.000001f,
              "active swimming can reach the bounded full ripple intensity");
    CheckNear(island::WaterRippleModel::TargetIntensity(
                  true, -1.0f, std::numeric_limits<float>::quiet_NaN()),
              0.0f, 0.000001f,
              "non-finite ripple input fails closed");
    island::WaterRippleModel waterRipple;
    const float firstRippleStep = waterRipple.Update(true, -1.0f, 2.0f, 1.0f / 60.0f);
    Check(firstRippleStep > 0.0f && firstRippleStep < swimmingRipple,
          "player ripple intensity eases in instead of appearing in one frame");
    const float firstRipplePhase = waterRipple.Phase();
    Check(firstRipplePhase > 0.0f && firstRipplePhase < 1.0f,
          "active player ripples advance a wrapped radial phase");
    const float beforeLongRippleFrame = waterRipple.Intensity();
    (void)waterRipple.Update(true, -1.0f, 2.0f, 5.0f);
    Check(waterRipple.Intensity() - beforeLongRippleFrame
              <= island::WaterRippleModel::EntryRatePerSecond
                    * island::WaterRippleModel::MaximumStepSeconds + 0.000001f,
          "player ripple intensity clamps a long frame");
    waterRipple.Reset(true, -1.0f, 2.0f);
    CheckNear(waterRipple.Intensity(), 1.0f, 0.000001f,
              "player ripple reset snaps deterministic QA placement to its target");
    CheckNear(waterRipple.Phase(), 0.0f, 0.000001f,
              "player ripple reset returns to a deterministic phase");
    for (int frame = 0; frame < 600; ++frame) {
        (void)waterRipple.Update(true, -1.0f, 2.0f, 1.0f / 60.0f);
        Check(waterRipple.Phase() >= 0.0f && waterRipple.Phase() < 1.0f,
              "player ripple phase remains wrapped during sustained movement");
    }
    for (int frame = 0; frame < 180; ++frame) {
        (void)waterRipple.Update(true, -1.0f, 0.0f, 1.0f / 60.0f);
    }
    CheckNear(waterRipple.Intensity(), 0.0f, 0.000001f,
              "stationary water movement releases the ripple fully");

    island::WaterRippleModel waterSplash;
    waterSplash.Reset(false, 0.0f, 0.0f);
    (void)waterSplash.Update(false, -0.10f, 3.0f, 1.0f / 60.0f);
    const float entrySplashIntensity = waterSplash.SplashIntensity();
    Check(entrySplashIntensity > 0.70f && entrySplashIntensity <= 1.0f,
          "moving across the wet threshold starts a bounded entry burst");
    Check(waterSplash.SplashPhase() > 0.0f && waterSplash.SplashPhase() < 0.05f,
          "entry burst begins with a smoothly advancing normalized phase");
    const float hysteresisPhase = waterSplash.SplashPhase();
    (void)waterSplash.Update(false, -0.04f, 3.0f, 1.0f / 60.0f);
    Check(waterSplash.SplashPhase() > hysteresisPhase,
          "water-depth hysteresis does not retrigger a burst in the transition band");
    const float cooldownPhase = waterSplash.SplashPhase();
    (void)waterSplash.Update(false, 0.0f, 3.0f, 1.0f / 60.0f);
    Check(waterSplash.SplashPhase() > cooldownPhase,
          "an opposite crossing during cooldown cannot restart the active burst");
    for (int frame = 0; frame < 120; ++frame) {
        (void)waterSplash.Update(false, -0.10f, 0.0f, 1.0f / 60.0f);
    }
    CheckNear(waterSplash.SplashIntensity(), 0.0f, 0.000001f,
              "shoreline burst retires completely after its bounded lifetime");
    (void)waterSplash.Update(false, 0.0f, 2.0f, 1.0f / 60.0f);
    Check(waterSplash.SplashIntensity() > 0.0f
              && waterSplash.SplashIntensity() < entrySplashIntensity,
          "leaving the water starts a gentler exit burst after cooldown");
    waterSplash.Reset(false, 0.0f, 0.0f);
    (void)waterSplash.Update(false, -0.10f,
                             std::numeric_limits<float>::quiet_NaN(), 1.0f / 60.0f);
    CheckNear(waterSplash.SplashIntensity(), 0.0f, 0.000001f,
              "non-finite crossing input cannot start a surface burst");

    const island::AmbientLoop surfLoop = island::GenerateSurfLoop();
    const island::AmbientLoop repeatedSurfLoop = island::GenerateSurfLoop();
    const island::AmbientLoop breezeLoop = island::GenerateBreezeLoop();
    const island::AmbientLoop jungleLoop = island::GenerateJungleLoop();
    const island::AmbientLoop repeatedJungleLoop = island::GenerateJungleLoop();
    const island::AmbientLoop streamLoop = island::GenerateStreamLoop();
    const island::AmbientLoop repeatedStreamLoop = island::GenerateStreamLoop();
    const island::AmbientLoop bunkerHumLoop = island::GenerateBunkerHumLoop();
    const island::AmbientLoop repeatedBunkerHumLoop = island::GenerateBunkerHumLoop();
    const island::AmbientLoop waveAccent = island::GenerateWaveAccent();
    const island::AmbientLoop waveAccentAlternate = island::GenerateWaveAccentAlternate();
    const island::AmbientLoop shipwreckCreak = island::GenerateShipwreckCreak();
    const island::AmbientLoop shipwreckCreakAlternate =
        island::GenerateShipwreckCreakAlternate();
    const island::AmbientLoop repeatedShipwreckCreak = island::GenerateShipwreckCreak();
    const island::AmbientLoop waterMovementLoop = island::GenerateWaterMovementLoop();
    const island::AmbientLoop repeatedWaterMovementLoop = island::GenerateWaterMovementLoop();
    const island::AmbientLoop waterTransitionAccent = island::GenerateWaterTransitionAccent();
    const island::AmbientLoop waterTransitionAccentAlternate =
        island::GenerateWaterTransitionAccentAlternate();
    const island::AmbientLoop sandFootstep = island::GenerateSandFootstep();
    const island::AmbientLoop sandFootstepAlternate =
        island::GenerateSandFootstepAlternate();
    const island::AmbientLoop soilFootstep = island::GenerateSoilFootstep();
    const island::AmbientLoop soilFootstepAlternate =
        island::GenerateSoilFootstepAlternate();
    const island::AmbientLoop mudFootstep = island::GenerateMudFootstep();
    const island::AmbientLoop mudFootstepAlternate = island::GenerateMudFootstepAlternate();
    {
        // The mud step: deterministic, the two variants distinct, silent at both ends, and its
        // energy in two parts, the landing's smack and, after a lull, the suck of the sole.
        const auto energy = [&](const island::AmbientLoop& clip, const float from, const float to) {
            const std::size_t a = static_cast<std::size_t>(from * static_cast<float>(clip.samples.size()));
            const std::size_t b = static_cast<std::size_t>(to * static_cast<float>(clip.samples.size()));
            double sum = 0.0;
            for (std::size_t i = a; i < b; ++i) sum += std::abs(static_cast<double>(clip.samples[i]));
            return sum / static_cast<double>(std::max<std::size_t>(1U, b - a));
        };
        Check(mudFootstep.sampleRate == 22050 && mudFootstep.samples.size() == 8820U
                  && mudFootstep.samples == island::GenerateMudFootstep().samples
                  && mudFootstep.samples != mudFootstepAlternate.samples
                  && mudFootstep.samples.front() == 0 && mudFootstep.samples.back() == 0
                  && energy(mudFootstep, 0.0f, 0.12f) > 2.0 * energy(mudFootstep, 0.25f, 0.38f)
                  && energy(mudFootstep, 0.45f, 0.60f) > 2.0 * energy(mudFootstep, 0.25f, 0.38f),
              "the mud step is a smack, a lull and the suck of the sole pulling free");
    }
    const island::AmbientLoop birdCall = island::GenerateBirdCall();
    const island::AmbientLoop birdCallAlternate = island::GenerateBirdCallAlternate();
    const island::AmbientLoop parrotCall = island::GenerateParrotCall();
    const island::AmbientLoop parrotCallAlternate = island::GenerateParrotCallAlternate();
    const island::AmbientLoop monkeyCall = island::GenerateMonkeyCall();
    const island::AmbientLoop monkeyCallAlternate = island::GenerateMonkeyCallAlternate();
    const island::AmbientLoop frogCall = island::GenerateFrogCall(0U);
    const island::AmbientLoop frogCallAlternate = island::GenerateFrogCall(1U);
    const island::AmbientLoop repeatedBirdCall = island::GenerateBirdCall();
    const island::AmbientLoop repeatedBirdCallAlternate = island::GenerateBirdCallAlternate();
    const island::AmbientLoop repeatedParrotCall = island::GenerateParrotCall();
    const island::AmbientLoop repeatedParrotCallAlternate = island::GenerateParrotCallAlternate();
    const island::AmbientLoop repeatedMonkeyCall = island::GenerateMonkeyCall();
    const island::AmbientLoop repeatedMonkeyCallAlternate = island::GenerateMonkeyCallAlternate();
    const island::AmbientLoop repeatedFrogCall = island::GenerateFrogCall(0U);
    const island::AmbientLoop cicadaCall = island::GenerateCicadaCall(0U);
    const island::AmbientLoop cicadaCallAlternate = island::GenerateCicadaCall(1U);
    const island::AmbientLoop repeatedCicadaCall = island::GenerateCicadaCall(0U);
    const island::AmbientLoop flyBuzz = island::GenerateFlyBuzz(0U);
    const island::AmbientLoop flyBuzzAlternate = island::GenerateFlyBuzz(1U);
    Check(surfLoop.sampleRate == 22050 && breezeLoop.sampleRate == 22050,
          "procedural ambience uses the intended portable sample rate");
    Check(frogCall.sampleRate==22050&&frogCall.samples.size()==24255U
              &&frogCallAlternate.sampleRate==22050&&frogCallAlternate.samples.size()==24255U
              &&frogCall.samples!=frogCallAlternate.samples
              &&frogCall.samples==repeatedFrogCall.samples
              &&frogCall.samples.front()==0&&frogCall.samples.back()==0
              &&frogCallAlternate.samples.front()==0&&frogCallAlternate.samples.back()==0,
          "frog croaks are deterministic, distinct and have bounded silent endpoints");
    Check(cicadaCall.sampleRate==22050&&cicadaCall.samples.size()==30870U
              &&cicadaCallAlternate.sampleRate==22050
              &&cicadaCallAlternate.samples.size()==30870U
              &&cicadaCall.samples!=cicadaCallAlternate.samples
              &&cicadaCall.samples==repeatedCicadaCall.samples
              &&cicadaCall.samples.front()==0&&cicadaCall.samples.back()==0
              &&cicadaCallAlternate.samples.front()==0&&cicadaCallAlternate.samples.back()==0,
          "cicada buzzes are deterministic, distinct and have bounded silent endpoints");
    Check(flyBuzz.sampleRate==22050&&flyBuzz.samples.size()==19845U
              &&flyBuzzAlternate.sampleRate==22050&&flyBuzzAlternate.samples.size()==19845U
              &&flyBuzz.samples!=flyBuzzAlternate.samples
              &&flyBuzz.samples==island::GenerateFlyBuzz(0U).samples
              &&flyBuzz.samples.front()==0&&flyBuzz.samples.back()==0
              &&flyBuzzAlternate.samples.front()==0&&flyBuzzAlternate.samples.back()==0,
          "fly buzzes are deterministic, distinct and have bounded silent endpoints");
    std::array<island::FlyState,2> audioFlies{};
    audioFlies[0].x=1.0f;audioFlies[0].y=.7f;
    audioFlies[1].x=2.0f;audioFlies[1].y=.9f;
    for(auto& fly:audioFlies)fly.simulationTier=island::WildlifeSimulationTier::Full;
    island::FlyBuzzScheduler flyScheduler,repeatedFlyScheduler;
    flyScheduler.Reset();repeatedFlyScheduler.Reset();
    int flyCalls=0;bool flyScheduleDeterministic=true,flyScheduleBounded=true;
    for(int step=0;step<240;++step) {
        const auto call=flyScheduler.Update(.25f,0,1.72f,0,audioFlies,true);
        const auto repeated=repeatedFlyScheduler.Update(.25f,0,1.72f,0,audioFlies,true);
        flyScheduleDeterministic=flyScheduleDeterministic&&call.trigger==repeated.trigger
            &&call.animalIndex==repeated.animalIndex&&call.variant==repeated.variant
            &&call.volume==repeated.volume&&call.pitch==repeated.pitch;
        if(!call.trigger)continue;
        ++flyCalls;
        flyScheduleBounded=flyScheduleBounded&&call.animalIndex<audioFlies.size()
            &&call.variant<2U&&call.volume>=.018f&&call.volume<=.13f
            &&call.pitch>=-.04f&&call.pitch<=.05f&&call.source.available;
    }
    Check(flyCalls>8&&flyScheduleDeterministic&&flyScheduleBounded,
        "fly buzz scheduling is deterministic, local, varied and bounded");
    island::FlyBuzzScheduler distantFlyScheduler;distantFlyScheduler.Reset();
    bool distantFlySilent=true;
    for(int step=0;step<120;++step)
        distantFlySilent=distantFlySilent&&!distantFlyScheduler.Update(
            .25f,40,1.72f,0,audioFlies,true).trigger;
    Check(distantFlySilent,"fly buzz leaked beyond its close-range habitat");
    Check(surfLoop.samples.size() == 220500U && breezeLoop.samples.size() == 242550U,
          "procedural ambient loops have their intended staggered portable lengths");
    Check(jungleLoop.sampleRate == 22050 && jungleLoop.samples.size() == 639450U,
          "daytime-jungle ambience has its intended twenty-nine-second portable format");
    Check(streamLoop.sampleRate == 22050 && streamLoop.samples.size() == 286650U,
          "stream ambience has its intended thirteen-second portable format");
    Check(bunkerHumLoop.sampleRate==22050&&bunkerHumLoop.samples.size()==374850U,
          "bunker electrical hum has its intended seventeen-second portable format");
    Check(waveAccent.sampleRate == 22050 && waveAccent.samples.size() == 88200U
              && waveAccentAlternate.sampleRate == 22050
              && waveAccentAlternate.samples.size() == 88200U,
          "both stronger-wave accents have the intended four-second portable format");
    Check(shipwreckCreak.sampleRate == 22050
              && shipwreckCreak.samples.size() == 39690U
              && shipwreckCreakAlternate.sampleRate == 22050
              && shipwreckCreakAlternate.samples.size() == 39690U,
          "both shipwreck creaks have the intended 1.8-second portable format");
    Check(waterMovementLoop.sampleRate == 22050
              && waterMovementLoop.samples.size() == 154350U,
          "player water movement has its intended seven-second portable format");
    const std::array<std::size_t, 5> persistentLoopSeconds = {
        surfLoop.samples.size() / 22050U,
        breezeLoop.samples.size() / 22050U,
        jungleLoop.samples.size() / 22050U,
        waterMovementLoop.samples.size() / 22050U,
        streamLoop.samples.size() / 22050U,
    };
    const std::size_t combinedLoopPeriod = std::accumulate(
        persistentLoopSeconds.begin(), persistentLoopSeconds.end(), std::size_t{1U},
        [](const std::size_t left, const std::size_t right) {
            return std::lcm(left, right);
        });
    Check(combinedLoopPeriod == 290290U,
          "persistent ambience avoids complete phase realignment for over eighty hours");
    Check(waterTransitionAccent.sampleRate == 22050
              && waterTransitionAccent.samples.size() == 33075U
              && waterTransitionAccentAlternate.sampleRate == 22050
              && waterTransitionAccentAlternate.samples.size() == 33075U,
          "both water transitions have the intended one-and-a-half-second portable format");
    Check(sandFootstep.sampleRate == 22050 && sandFootstep.samples.size() == 6615U
              && sandFootstepAlternate.sampleRate == 22050
              && sandFootstepAlternate.samples.size() == 6615U
              && soilFootstep.sampleRate == 22050 && soilFootstep.samples.size() == 6615U
              && soilFootstepAlternate.sampleRate == 22050
              && soilFootstepAlternate.samples.size() == 6615U,
          "all land footsteps have the intended 300-millisecond portable format");
    Check(birdCall.sampleRate == 22050 && birdCall.samples.size() == 26460U
              && birdCallAlternate.sampleRate == 22050
              && birdCallAlternate.samples.size() == 29767U,
          "both bird phrases have their intended short portable formats");
    Check(parrotCall.sampleRate == 22050 && parrotCall.samples.size() == 31972U
              && parrotCallAlternate.sampleRate == 22050
              && parrotCallAlternate.samples.size() == 35280U,
          "both parrot squawks have their intended short portable formats");
    Check(monkeyCall.sampleRate == 22050 && monkeyCall.samples.size() == 39690U
              && monkeyCallAlternate.sampleRate == 22050
              && monkeyCallAlternate.samples.size() == 46305U,
          "both monkey phrases have their intended short portable formats");
    Check(surfLoop.samples == repeatedSurfLoop.samples,
          "procedural surf generation is deterministic");
    Check(waterMovementLoop.samples == repeatedWaterMovementLoop.samples,
          "procedural player water movement generation is deterministic");
    Check(jungleLoop.samples == repeatedJungleLoop.samples,
          "procedural daytime-jungle generation is deterministic");
    Check(streamLoop.samples == repeatedStreamLoop.samples,
          "procedural stream generation is deterministic");
    Check(bunkerHumLoop.samples==repeatedBunkerHumLoop.samples,
          "procedural bunker electrical hum generation is deterministic");
    Check(waveAccent.samples != waveAccentAlternate.samples,
          "stronger-wave variants contain genuinely different source PCM");
    Check(shipwreckCreak.samples != shipwreckCreakAlternate.samples
              && shipwreckCreak.samples == repeatedShipwreckCreak.samples,
          "shipwreck creaks are distinct variants and regenerate deterministically");
    Check(waterTransitionAccent.samples != waterTransitionAccentAlternate.samples,
          "water-transition variants contain genuinely different source PCM");
    Check(sandFootstep.samples != sandFootstepAlternate.samples
              && soilFootstep.samples != soilFootstepAlternate.samples
              && sandFootstep.samples != soilFootstep.samples,
          "footstep variants and terrain surfaces contain distinct source PCM");
    Check(birdCall.samples != birdCallAlternate.samples
              && parrotCall.samples != parrotCallAlternate.samples
              && parrotCall.samples != birdCall.samples
              && monkeyCall.samples != monkeyCallAlternate.samples,
          "wildlife variants contain genuinely different source PCM");
    Check(birdCall.samples == repeatedBirdCall.samples
              && birdCallAlternate.samples == repeatedBirdCallAlternate.samples
              && parrotCall.samples == repeatedParrotCall.samples
              && parrotCallAlternate.samples == repeatedParrotCallAlternate.samples
              && monkeyCall.samples == repeatedMonkeyCall.samples
              && monkeyCallAlternate.samples == repeatedMonkeyCallAlternate.samples,
          "all procedural wildlife phrases regenerate deterministically");
    const auto peakMagnitude = [](const island::AmbientLoop& loop) {
        int peak = 0;
        for (const std::int16_t sample : loop.samples) {
            peak = std::max(peak, std::abs(static_cast<int>(sample)));
        }
        return peak;
    };
    const auto rootMeanSquare = [](const island::AmbientLoop& loop) {
        double squaredSum = 0.0;
        for (const std::int16_t sample : loop.samples) {
            const double value = static_cast<double>(sample);
            squaredSum += value * value;
        }
        return std::sqrt(squaredSum / static_cast<double>(loop.samples.size()));
    };
    Check(peakMagnitude(surfLoop) < 32767 && peakMagnitude(breezeLoop) < 32767,
          "procedural ambience retains headroom before runtime mixing");
    Check(rootMeanSquare(surfLoop) > 1200.0 && rootMeanSquare(breezeLoop) > 900.0,
          "procedural ambience contains meaningful audible energy");
    Check(peakMagnitude(jungleLoop) < 32767 && rootMeanSquare(jungleLoop) > 700.0,
          "daytime-jungle ambience is audible while retaining mixing headroom");
    Check(peakMagnitude(streamLoop) < 32767 && rootMeanSquare(streamLoop) > 700.0,
          "stream ambience is audible while retaining mixing headroom");
    Check(peakMagnitude(bunkerHumLoop)<32767&&rootMeanSquare(bunkerHumLoop)>700.0,
          "bunker electrical hum is audible while retaining mixing headroom");
    Check(peakMagnitude(waveAccent) < 32767 && rootMeanSquare(waveAccent) > 900.0,
          "stronger-wave accent is audible while retaining mixing headroom");
    Check(peakMagnitude(waveAccentAlternate) < 32767
              && rootMeanSquare(waveAccentAlternate) > 900.0,
          "alternate stronger wave is audible while retaining mixing headroom");
    Check(waveAccent.samples.front() == 0 && waveAccent.samples.back() == 0
              && waveAccentAlternate.samples.front() == 0
              && waveAccentAlternate.samples.back() == 0,
          "both stronger waves start and end at silence without a boundary click");
    Check(peakMagnitude(waterMovementLoop) < 32767
              && rootMeanSquare(waterMovementLoop) > 900.0,
          "player water movement is audible while retaining mixing headroom");
    Check(peakMagnitude(waterTransitionAccent) < 32767
              && rootMeanSquare(waterTransitionAccent) > 700.0,
          "water transition accent is audible while retaining mixing headroom");
    Check(peakMagnitude(waterTransitionAccentAlternate) < 32767
              && rootMeanSquare(waterTransitionAccentAlternate) > 700.0,
          "alternate water transition is audible while retaining mixing headroom");
    Check(waterTransitionAccent.samples.front() == 0
              && waterTransitionAccent.samples.back() == 0
              && waterTransitionAccentAlternate.samples.front() == 0
              && waterTransitionAccentAlternate.samples.back() == 0,
          "both water transition accents start and end at silence");
    Check(peakMagnitude(sandFootstep) < 32767 && rootMeanSquare(sandFootstep) > 700.0
              && peakMagnitude(sandFootstepAlternate) < 32767
              && rootMeanSquare(sandFootstepAlternate) > 700.0
              && peakMagnitude(soilFootstep) < 32767
              && rootMeanSquare(soilFootstep) > 700.0
              && peakMagnitude(soilFootstepAlternate) < 32767
              && rootMeanSquare(soilFootstepAlternate) > 700.0,
          "land footsteps are audible while retaining mixing headroom");
    Check(sandFootstep.samples.front() == 0 && sandFootstep.samples.back() == 0
              && sandFootstepAlternate.samples.front() == 0
              && sandFootstepAlternate.samples.back() == 0
              && soilFootstep.samples.front() == 0 && soilFootstep.samples.back() == 0
              && soilFootstepAlternate.samples.front() == 0
              && soilFootstepAlternate.samples.back() == 0,
          "all land footsteps begin and end at exact silence");
    Check(peakMagnitude(birdCall) < 32767 && rootMeanSquare(birdCall) > 1500.0
              && peakMagnitude(birdCallAlternate) < 32767
              && rootMeanSquare(birdCallAlternate) > 1500.0,
          "bird phrases carry audible energy while retaining headroom");
    Check(peakMagnitude(monkeyCall) < 32767 && rootMeanSquare(monkeyCall) > 1500.0
              && peakMagnitude(monkeyCallAlternate) < 32767
              && rootMeanSquare(monkeyCallAlternate) > 1500.0,
          "monkey phrases carry audible energy while retaining headroom");
    Check(birdCall.samples.front() == 0 && birdCall.samples.back() == 0
              && birdCallAlternate.samples.front() == 0
              && birdCallAlternate.samples.back() == 0
              && monkeyCall.samples.front() == 0 && monkeyCall.samples.back() == 0
              && monkeyCallAlternate.samples.front() == 0
              && monkeyCallAlternate.samples.back() == 0,
          "all wildlife phrases begin and end at silence");
    Check(std::abs(static_cast<int>(surfLoop.samples.front())
                   - static_cast<int>(surfLoop.samples.back())) < 8000,
          "surf loop boundary does not contain a large discontinuity");
    Check(std::abs(static_cast<int>(breezeLoop.samples.front())
                   - static_cast<int>(breezeLoop.samples.back())) < 8000,
          "breeze loop boundary does not contain a large discontinuity");
    Check(std::abs(static_cast<int>(jungleLoop.samples.front())
                   - static_cast<int>(jungleLoop.samples.back())) < 8000,
          "daytime-jungle loop boundary does not contain a large discontinuity");
    Check(std::abs(static_cast<int>(waterMovementLoop.samples.front())
                   - static_cast<int>(waterMovementLoop.samples.back())) < 8000,
          "player water movement loop has no large boundary discontinuity");
    Check(std::abs(static_cast<int>(streamLoop.samples.front())
                   - static_cast<int>(streamLoop.samples.back())) < 8000,
          "stream loop has no large boundary discontinuity");
    Check(std::abs(static_cast<int>(bunkerHumLoop.samples.front())
                   -static_cast<int>(bunkerHumLoop.samples.back()))<8000,
          "bunker electrical hum loop has no large boundary discontinuity");
    for (const island::AmbientLoop* loop : {
             &surfLoop, &breezeLoop, &jungleLoop, &waterMovementLoop, &streamLoop,
             &bunkerHumLoop}) {
        Check(std::abs(island::AnalyzeAudioSignal(*loop).mean) <= 0.0001,
              "persistent ambience removes material DC before PCM quantization");
    }

    const island::SoundscapeMix beachMix = island::SoundscapeModel::TargetMix(2.0f, false);
    const island::SoundscapeMix jungleMix = island::SoundscapeModel::TargetMix(140.0f, false);
    const island::SoundscapeMix swimmingMix = island::SoundscapeModel::TargetMix(-8.0f, true);
    const island::SoundscapeMix forcedInlandSwimmingMix =
        island::SoundscapeModel::TargetMix(140.0f, true);
    const island::SoundscapeMix wetInteriorMix =
        island::SoundscapeModel::TargetMix(80.0f, false, 0.5f, 1.0f);
    const island::SoundscapeMix dryInteriorMix =
        island::SoundscapeModel::TargetMix(80.0f, false, 0.5f, 0.0f);
    const island::SoundscapeMix seasonalWetlandMix =
        island::SoundscapeModel::TargetMix(80.0f, false, 0.5f, 0.0f, 1.0f);
    Check(beachMix.surfVolume > jungleMix.surfVolume,
          "surf is louder at the beach than deep inland");
    Check(jungleMix.breezeVolume > beachMix.breezeVolume,
          "canopy breeze grows toward the island interior");
    CheckNear(beachMix.jungleVolume, 0.0f, 0.0001f,
              "daytime-jungle texture stays out of the beach mix");
    Check(jungleMix.jungleVolume >= 0.075f
              && forcedInlandSwimmingMix.jungleVolume < jungleMix.jungleVolume * 0.1f,
          "jungle texture is audible inland and explicitly attenuated while swimming");
    Check(swimmingMix.surfVolume >= 0.35f && swimmingMix.breezeVolume < beachMix.breezeVolume,
          "swimming emphasizes nearby water while softening canopy breeze");
    Check(seasonalWetlandMix.streamVolume==dryInteriorMix.streamVolume
            &&seasonalWetlandMix.jungleVolume>dryInteriorMix.jungleVolume
            &&seasonalWetlandMix.breezeVolume<dryInteriorMix.breezeVolume,
        "seasonal wetland emphasizes insect-rich jungle ambience without false flowing water");
    const auto surfaceAudio=island::UnderwaterAudioResponse(0.0f);
    const auto shallowAudio=island::UnderwaterAudioResponse(.55f);
    const auto deepAudio=island::UnderwaterAudioResponse(2.0f);
    const auto invalidDepthAudio=island::UnderwaterAudioResponse(
        std::numeric_limits<float>::quiet_NaN());
    Check(surfaceAudio.immersion==0&&surfaceAudio.airGain==1&&surfaceAudio.surfGain==1
          &&surfaceAudio.waterGain==1&&surfaceAudio.waterPitchOffset==0,
          "surface swimming leaves the existing sound mix unchanged");
    Check(shallowAudio.immersion>0&&shallowAudio.immersion<1
          &&shallowAudio.airGain<1&&shallowAudio.surfGain<1
          &&shallowAudio.waterGain>1&&shallowAudio.waterPitchOffset<0,
          "submersion progressively muffles air and deepens the water layer");
    Check(deepAudio.immersion==1&&deepAudio.airGain==.18f&&deepAudio.surfGain==.72f
          &&deepAudio.waterGain==1.22f&&deepAudio.waterPitchOffset==-.12f,
          "deep-water audio response stays inside its authored bounds");
    Check(invalidDepthAudio.immersion==0&&invalidDepthAudio.airGain==1,
          "invalid submersion depth fails safely to the surface mix");
    Check(wetInteriorMix.streamVolume >= 0.22f
              && dryInteriorMix.streamVolume == 0.0f
              && wetInteriorMix.jungleVolume > dryInteriorMix.jungleVolume
              && wetInteriorMix.breezeVolume < dryInteriorMix.breezeVolume,
          "the local stream biome adds flowing water and denser damp-jungle ambience");
    const auto nearStream = island::StreamSoundField::Sample(
        island::StreamRoute::SourceX - 48.0f,
        island::StreamRoute::CentreZ(island::StreamRoute::SourceX - 48.0f));
    const auto farFromStream = island::StreamSoundField::Sample(120.0f, 120.0f);
    const auto invalidStream = island::StreamSoundField::Sample(
        std::numeric_limits<float>::quiet_NaN(), 0.0f);
    Check(nearStream.valid && nearStream.distance < 0.01f && nearStream.presence > 0.999f
              && farFromStream.valid && farFromStream.presence == 0.0f
              && !invalidStream.valid && invalidStream.presence == 0.0f,
          "stream sound field follows the shared route and fails safely for invalid listeners");
    Check(island::StreamSoundField::ShouldUseStreamPlayback(false, 0.02f, false)
              && island::StreamSoundField::ShouldUseStreamPlayback(false, 0.01f, true)
              && !island::StreamSoundField::ShouldUseStreamPlayback(false, 0.01f, false)
              && !island::StreamSoundField::ShouldUseStreamPlayback(true, 1.0f, true)
              && !island::StreamSoundField::ShouldUseStreamPlayback(false,
                  std::numeric_limits<float>::quiet_NaN(), true),
          "stream and player-water share one voice with finite hysteretic arbitration");
    const float eastExposure=island::CoastalWindResponse::Exposure(230.0f,0.0f,2.0f);
    const float westExposure=island::CoastalWindResponse::Exposure(-180.0f,0.0f,2.0f);
    const auto eastCoastMix=island::SoundscapeModel::TargetMix(2.0f,false,eastExposure);
    const auto westBayMix=island::SoundscapeModel::TargetMix(2.0f,false,westExposure);
    Check(eastExposure>.98f&&westExposure<.27f
        &&island::CoastalWindResponse::WaveScale(eastExposure)>1.20f
        &&island::CoastalWindResponse::WaveScale(westExposure)<.90f
        &&island::CoastalWindResponse::FoamScale(eastExposure)>1.24f
        &&eastCoastMix.surfVolume>westBayMix.surfVolume*1.25f
        &&eastCoastMix.breezeVolume>westBayMix.breezeVolume*1.25f,
        "windward east coast has stronger waves, foam, surf and breeze than the west bay");
    CheckNear(island::CoastalWindResponse::Exposure(0.0f,0.0f,500.0f),.5f,.0001f,
        "coastal wind modulation returns to neutral away from the shore");
    CheckNear(island::CoastalWindResponse::AmbientScale(
        std::numeric_limits<float>::quiet_NaN()),1.0f,.0001f,
        "invalid coastal wind exposure fails safely to the neutral mix");
    const island::SoundscapeMix invalidPositionMix = island::SoundscapeModel::TargetMix(
        std::numeric_limits<float>::quiet_NaN(), false);
    Check(std::isfinite(invalidPositionMix.surfVolume)
              && std::isfinite(invalidPositionMix.breezeVolume)
              && std::isfinite(invalidPositionMix.jungleVolume)
              && invalidPositionMix.surfVolume == 0.0f
              && invalidPositionMix.breezeVolume == 0.0f
              && invalidPositionMix.jungleVolume == 0.0f
              && invalidPositionMix.streamVolume == 0.0f,
          "non-finite soundscape positions fail closed to a finite silent target");
    island::SoundscapeModel soundscapeModel;
    soundscapeModel.Reset(2.0f, false);
    const island::SoundscapeMix beforeInvalidDelta = soundscapeModel.Mix();
    const island::SoundscapeMix afterInvalidDelta = soundscapeModel.Update(
        140.0f, false, std::numeric_limits<float>::infinity());
    Check(afterInvalidDelta.surfVolume == beforeInvalidDelta.surfVolume
              && afterInvalidDelta.breezeVolume == beforeInvalidDelta.breezeVolume
              && afterInvalidDelta.jungleVolume == beforeInvalidDelta.jungleVolume
              && afterInvalidDelta.streamVolume == beforeInvalidDelta.streamVolume,
          "non-finite soundscape time cannot poison or advance the current mix");
    const float initialSurfVolume = soundscapeModel.Mix().surfVolume;
    for (int step = 0; step < 300; ++step) {
        (void)soundscapeModel.Update(140.0f, false, 1.0f / 60.0f);
    }
    Check(soundscapeModel.Mix().surfVolume < initialSurfVolume
              && soundscapeModel.Mix().surfVolume > jungleMix.surfVolume,
          "soundscape transitions smoothly toward the new zone instead of snapping");
    CheckNear(soundscapeModel.Mix().surfVolume, jungleMix.surfVolume, 0.025f,
              "soundscape approaches the inland surf target over several seconds");
    Check(soundscapeModel.Mix().jungleVolume > 0.045f
              && soundscapeModel.Mix().jungleVolume < jungleMix.jungleVolume,
          "the slower jungle layer fades in without a hard biome boundary");

    constexpr float ambientVariationDistance = 70.0f;
    const island::SoundscapeMix ambientVariationBase =
        island::SoundscapeModel::TargetMix(ambientVariationDistance, false);
    island::SoundscapeModel ambientVariationModel;
    ambientVariationModel.Reset(ambientVariationDistance, false);
    for (int step = 0; step < 240; ++step) {
        (void)ambientVariationModel.Update(
            ambientVariationDistance, false, 0.25f);
    }
    const island::SoundscapeMix variedAmbient = ambientVariationModel.Mix();
    Check(variedAmbient.surfVolume < ambientVariationBase.surfVolume
              && variedAmbient.surfVolume >= ambientVariationBase.surfVolume * 0.965f,
          "slow surf-bed variation remains a restrained downward gain drift");
    Check(variedAmbient.breezeVolume < ambientVariationBase.breezeVolume
              && variedAmbient.breezeVolume >= ambientVariationBase.breezeVolume * 0.925f,
          "slow canopy-bed variation remains inside its authored gain depth");
    Check(variedAmbient.jungleVolume < ambientVariationBase.jungleVolume
              && variedAmbient.jungleVolume >= ambientVariationBase.jungleVolume * 0.945f,
          "slow jungle-bed variation remains inside its authored gain depth");
    island::SoundscapeModel repeatedAmbientVariationModel;
    repeatedAmbientVariationModel.Reset(ambientVariationDistance, false);
    for (int step = 0; step < 240; ++step) {
        (void)repeatedAmbientVariationModel.Update(
            ambientVariationDistance, false, 0.25f);
    }
    Check(variedAmbient.surfVolume == repeatedAmbientVariationModel.Mix().surfVolume
              && variedAmbient.breezeVolume
                  == repeatedAmbientVariationModel.Mix().breezeVolume
              && variedAmbient.jungleVolume
                  == repeatedAmbientVariationModel.Mix().jungleVolume,
          "slow ambient-bed variation is deterministic after reset");
    // A gust front at the listener swells the breeze and canopy beds above the zone mix,
    // within their ceilings; with the front gone the beds settle back to the calm mix.
    island::SoundscapeModel gustModel, calmModel;
    gustModel.Reset(ambientVariationDistance, false);
    calmModel.Reset(ambientVariationDistance, false);
    for (int step = 0; step < 240; ++step) {
        (void)gustModel.Update(ambientVariationDistance, false, 0.25f, 0.5f, 0.0f, 0.0f, 1.0f);
        (void)calmModel.Update(ambientVariationDistance, false, 0.25f);
    }
    Check(gustModel.Mix().breezeVolume > calmModel.Mix().breezeVolume * 1.5f
              && gustModel.Mix().breezeVolume <= island::SoundscapeModel::GustBreezeCeiling
              && gustModel.Mix().jungleVolume > calmModel.Mix().jungleVolume * 1.2f
              && gustModel.Mix().jungleVolume <= island::SoundscapeModel::GustJungleCeiling,
          "a gust front at the listener swells the breeze and canopy beds within their ceilings");
    for (int step = 0; step < 240; ++step) {
        (void)gustModel.Update(ambientVariationDistance, false, 0.25f, 0.5f, 0.0f, 0.0f, 0.0f);
        (void)calmModel.Update(ambientVariationDistance, false, 0.25f);
    }
    Check(std::abs(gustModel.Mix().breezeVolume - calmModel.Mix().breezeVolume) < 0.002f,
          "the beds settle back to the calm mix once the front has passed");

    const island::WaterMovementMix dryMovement =
        island::WaterMovementModel::TargetMix(0.0f, false, 4.3f);
    const island::WaterMovementMix idleWater =
        island::WaterMovementModel::TargetMix(0.5f, true, 0.0f);
    const island::WaterMovementMix wadingMovement =
        island::WaterMovementModel::TargetMix(0.24f, false, 3.0f);
    const island::WaterMovementMix swimmingMovement =
        island::WaterMovementModel::TargetMix(0.7f, true, 2.0f);
    const island::WaterMovementMix idleCurrent =
        island::WaterMovementModel::TargetMix(2.5f, true, 0.0f, 1.0f);
    const island::WaterMovementMix dryCurrent =
        island::WaterMovementModel::TargetMix(0.0f, false, 4.3f, 1.0f);
    const island::WaterMovementMix invalidCurrent =
        island::WaterMovementModel::TargetMix(
            2.5f, true, 0.0f, std::numeric_limits<float>::quiet_NaN());
    const island::WaterMovementMix invalidWaterInputs =
        island::WaterMovementModel::TargetMix(
            std::numeric_limits<float>::quiet_NaN(), false,
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::quiet_NaN());
    CheckNear(dryMovement.volume, 0.0f, 0.0001f,
              "dry walking never activates the player water layer");
    CheckNear(idleWater.volume, 0.0f, 0.0001f,
              "standing still in water does not emit continuous movement noise");
    Check(wadingMovement.volume > 0.0f && swimmingMovement.volume > wadingMovement.volume,
          "wading is audible and active swimming carries the stronger water mix");
    Check(swimmingMovement.pitch < wadingMovement.pitch,
          "swimming shifts the shared water texture below the wading pitch");
    Check(idleCurrent.currentCue == 1.0f
              && idleCurrent.volume >= 0.075f && idleCurrent.volume <= 0.081f
              && idleCurrent.pitch < swimmingMovement.pitch,
          "full offshore current reuses the water loop as a bounded deeper undertow cue");
    CheckNear(dryCurrent.currentCue, 0.0f, 0.0001f,
              "ocean-current audio remains disabled outside swimming");
    CheckNear(invalidCurrent.currentCue, 0.0f, 0.0001f,
              "non-finite ocean-current audio input degrades to the calm mix");
    Check(std::isfinite(invalidWaterInputs.volume)
              && std::isfinite(invalidWaterInputs.pitch)
              && std::isfinite(invalidWaterInputs.currentCue)
              && invalidWaterInputs.volume == 0.0f
              && invalidWaterInputs.currentCue == 0.0f,
          "non-finite water depth and speed fail closed to a finite dry mix");
    island::WaterMovementModel waterMovementModel;
    waterMovementModel.Reset(0.0f, false, 0.0f);
    const island::WaterMovementMix beforeInvalidWaterMixDelta = waterMovementModel.Mix();
    const island::WaterMovementMix afterInvalidWaterDelta = waterMovementModel.Update(
        std::numeric_limits<float>::quiet_NaN(), false,
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::quiet_NaN());
    Check(afterInvalidWaterDelta.volume == beforeInvalidWaterMixDelta.volume
              && afterInvalidWaterDelta.pitch == beforeInvalidWaterMixDelta.pitch
              && afterInvalidWaterDelta.currentCue == beforeInvalidWaterMixDelta.currentCue,
          "non-finite water-mix time cannot poison or advance the current mix");
    const island::WaterMovementMix firstWadingFrame =
        waterMovementModel.Update(0.24f, false, 3.0f, 1.0f / 60.0f);
    Check(firstWadingFrame.volume > 0.0f
              && firstWadingFrame.volume < wadingMovement.volume,
          "water movement fades in instead of starting with a hard gain edge");
    for (int step = 0; step < 180; ++step) {
        (void)waterMovementModel.Update(0.24f, false, 3.0f, 1.0f / 60.0f);
    }
    CheckNear(waterMovementModel.Mix().volume, wadingMovement.volume, 0.002f,
              "water movement converges to its speed and depth target");
    const float movingWaterVolume = waterMovementModel.Mix().volume;
    const island::WaterMovementMix firstDryFrame =
        waterMovementModel.Update(0.0f, false, 0.0f, 1.0f / 60.0f);
    Check(firstDryFrame.volume > 0.0f && firstDryFrame.volume < movingWaterVolume,
          "water movement fades out without rapid stop/start chatter");
    waterMovementModel.Reset(2.5f, true, 0.0f, 0.0f);
    const island::WaterMovementMix firstCurrentFrame = waterMovementModel.Update(
        2.5f, true, 0.0f, 1.0f / 60.0f, 1.0f);
    Check(firstCurrentFrame.currentCue > 0.0f && firstCurrentFrame.currentCue < 0.03f
              && firstCurrentFrame.volume > 0.0f && firstCurrentFrame.volume < 0.001f,
          "undertow audio fades in instead of snapping at the current threshold");
    for (int step = 0; step < 240; ++step) {
        (void)waterMovementModel.Update(2.5f, true, 0.0f, 1.0f / 60.0f, 1.0f);
    }
    CheckNear(waterMovementModel.Mix().currentCue, 1.0f, 0.003f,
              "undertow audio converges to the full-current target smoothly");
    const island::WaterMovementMix firstCalmFrame = waterMovementModel.Update(
        2.5f, true, 0.0f, 1.0f / 60.0f, 0.0f);
    Check(firstCalmFrame.currentCue > 0.0f && firstCalmFrame.currentCue < 1.0f,
          "undertow audio releases smoothly after leaving the current field");
    for (int step = 0; step < 360; ++step) {
        (void)waterMovementModel.Update(2.5f, true, 0.0f, 1.0f / 60.0f, 0.0f);
    }
    CheckNear(waterMovementModel.Mix().currentCue, 0.0f, 0.0002f,
              "undertow cue fully returns to the calm stationary-water state");
    CheckNear(waterMovementModel.Mix().volume, 0.0f, 0.0002f,
              "undertow audio leaves no residual gain in calm stationary water");

    island::WaterTransitionScheduler waterTransitionScheduler;
    waterTransitionScheduler.Reset(0.0f, false);
    const island::WaterTransitionRequest waterEntry =
        waterTransitionScheduler.Update(0.12f, false, 3.0f, 1.0f / 60.0f);
    Check(waterEntry.type == island::WaterTransitionType::Enter,
          "crossing the wet threshold emits one water-entry accent");
    Check(waterEntry.variant <= 1U
              && waterEntry.volume >= 0.055f && waterEntry.volume <= 0.14f
              && waterEntry.pitch >= -0.055f && waterEntry.pitch <= -0.005f
              && waterEntry.pan >= -0.18f && waterEntry.pan <= 0.18f,
          "water-entry variation remains inside conservative playback bounds");
    const island::WaterTransitionRequest rapidExit =
        waterTransitionScheduler.Update(0.0f, false, 3.0f, 0.1f);
    Check(rapidExit.type == island::WaterTransitionType::None,
          "water transition cooldown prevents overlapping shoreline chatter");
    for (int step = 0; step < 7; ++step) {
        (void)waterTransitionScheduler.Update(0.0f, false, 0.0f, 0.25f);
    }
    const island::WaterTransitionRequest secondEntry =
        waterTransitionScheduler.Update(0.12f, false, 2.0f, 0.01f);
    Check(secondEntry.type == island::WaterTransitionType::Enter,
          "water entry becomes eligible again after the authored cooldown");
    waterTransitionScheduler.Reset(0.2f, false);
    const island::WaterTransitionRequest waterExit =
        waterTransitionScheduler.Update(0.0f, false, 2.0f, 0.01f);
    Check(waterExit.type == island::WaterTransitionType::Exit
              && waterExit.pitch >= 0.025f && waterExit.pitch <= 0.075f,
          "leaving shallow water emits the distinct higher-pitched exit accent");
    waterTransitionScheduler.Reset(0.0f, false);
    const island::WaterTransitionRequest stationaryEntry =
        waterTransitionScheduler.Update(0.12f, false, 0.0f, 0.01f);
    Check(stationaryEntry.type == island::WaterTransitionType::None,
          "stationary water-state changes do not emit a teleport-like splash");
    island::WaterTransitionScheduler invalidTransitionScheduler;
    invalidTransitionScheduler.Reset(0.0f, false);
    const island::WaterTransitionRequest invalidDepthTransition =
        invalidTransitionScheduler.Update(
            std::numeric_limits<float>::quiet_NaN(), false, 3.0f, 0.01f);
    const island::WaterTransitionRequest invalidSpeedTransition =
        invalidTransitionScheduler.Update(
            0.12f, false, std::numeric_limits<float>::infinity(), 0.01f);
    const island::WaterTransitionRequest recoveredTransition =
        invalidTransitionScheduler.Update(0.12f, false, 3.0f, 0.01f);
    Check(invalidDepthTransition.type == island::WaterTransitionType::None
              && invalidSpeedTransition.type == island::WaterTransitionType::None
              && recoveredTransition.type == island::WaterTransitionType::Enter,
          "invalid shoreline audio inputs do not mutate state or block the next valid crossing");
    island::WaterTransitionScheduler transitionVariantScheduler;
    transitionVariantScheduler.Reset(0.0f, false);
    std::array<bool, 2> observedTransitionVariants = {false, false};
    bool transitionVariantOutOfRange = false;
    for (int crossing = 0; crossing < 10; ++crossing) {
        const bool entering = crossing % 2 == 0;
        const island::WaterTransitionRequest transition = transitionVariantScheduler.Update(
            entering ? 0.12f : 0.0f, false, 2.5f, 0.01f);
        if (transition.type != island::WaterTransitionType::None) {
            if (transition.variant < observedTransitionVariants.size()) {
                observedTransitionVariants[transition.variant] = true;
            } else {
                transitionVariantOutOfRange = true;
            }
        }
        for (int cooldownStep = 0; cooldownStep < 7; ++cooldownStep) {
            (void)transitionVariantScheduler.Update(
                entering ? 0.12f : 0.0f, false, 0.0f, 0.25f);
        }
    }
    Check(!transitionVariantOutOfRange
              && observedTransitionVariants[0] && observedTransitionVariants[1],
          "deterministic shoreline scheduling reaches both source variants");

    island::FootstepScheduler footstepScheduler;
    footstepScheduler.Reset();
    Check(footstepScheduler.Update(1.0f / 60.0f, false, 6.0f,
                                   island::PlayerMotion::WalkSpeed, 0.0f, 0.0f).surface
              == island::FootstepSurface::None,
          "footstep scheduling latches its first resolved player position silently");
    int walkingFootsteps = 0;
    std::array<bool, 2> sandFootstepVariants = {false, false};
    bool footstepPlaybackBoundsValid = true;
    float walkingPosition = 0.0f;
    for (int frame = 0; frame < 20 * 60; ++frame) {
        walkingPosition += island::PlayerMotion::WalkSpeed / 60.0f;
        const island::FootstepRequest step = footstepScheduler.Update(
            1.0f / 60.0f, false, 6.0f, island::PlayerMotion::WalkSpeed,
            walkingPosition, 0.0f);
        if (step.surface == island::FootstepSurface::None) {
            continue;
        }
        ++walkingFootsteps;
        if (step.variant < sandFootstepVariants.size()) {
            sandFootstepVariants[step.variant] = true;
        } else {
            footstepPlaybackBoundsValid = false;
        }
        footstepPlaybackBoundsValid = footstepPlaybackBoundsValid
            && step.surface == island::FootstepSurface::Sand
            && step.volume >= 0.12f && step.volume <= 0.203f
            && step.pitch >= -0.015f && step.pitch <= 0.10f
            && step.pan >= -0.070f && step.pan <= 0.070f;
    }
    Check(walkingFootsteps >= 40 && walkingFootsteps <= 44
              && sandFootstepVariants[0] && sandFootstepVariants[1]
              && footstepPlaybackBoundsValid,
          "walk footsteps follow resolved distance with bounded varied sand playback");

    island::FootstepScheduler runningFootstepScheduler;
    runningFootstepScheduler.Reset();
    (void)runningFootstepScheduler.Update(
        1.0f / 60.0f, false, 80.0f, island::PlayerMotion::RunSpeed, 0.0f, 0.0f);
    int runningFootsteps = 0;
    std::array<bool, 2> soilFootstepVariants = {false, false};
    float runningPosition = 0.0f;
    for (int frame = 0; frame < 20 * 60; ++frame) {
        runningPosition += island::PlayerMotion::RunSpeed / 60.0f;
        const island::FootstepRequest step = runningFootstepScheduler.Update(
            1.0f / 60.0f, false, 80.0f, island::PlayerMotion::RunSpeed,
            runningPosition, 0.0f);
        if (step.surface == island::FootstepSurface::None) {
            continue;
        }
        ++runningFootsteps;
        Check(step.surface == island::FootstepSurface::Soil,
              "interior travel selects the dry-soil footstep family");
        if (step.variant < soilFootstepVariants.size()) {
            soilFootstepVariants[step.variant] = true;
        }
    }
    Check(runningFootsteps >= 57 && runningFootsteps <= 60
              && runningFootsteps > walkingFootsteps
              && soilFootstepVariants[0] && soilFootstepVariants[1],
          "running increases cadence while retaining both interior footstep variants");

    island::FootstepScheduler guardedFootstepScheduler;
    guardedFootstepScheduler.Reset();
    (void)guardedFootstepScheduler.Update(
        1.0f / 60.0f, false, 50.0f, 3.0f, 0.0f, 0.0f);
    const island::FootstepRequest invalidFootstep = guardedFootstepScheduler.Update(
        std::numeric_limits<float>::quiet_NaN(), false, 50.0f, 3.0f, 0.1f, 0.0f);
    const island::FootstepRequest teleportedFootstep = guardedFootstepScheduler.Update(
        1.0f / 60.0f, false, 50.0f, 3.0f, 100.0f, 0.0f);
    const island::FootstepRequest swimmingFootstep = guardedFootstepScheduler.Update(
        1.0f / 60.0f, true, -5.0f, 2.0f, 100.02f, 0.0f);
    Check(invalidFootstep.surface == island::FootstepSurface::None
              && teleportedFootstep.surface == island::FootstepSurface::None
              && swimmingFootstep.surface == island::FootstepSurface::None,
          "invalid time, teleports, and swimming cannot fabricate land footsteps");

    island::WaveAccentScheduler waveScheduler;
    waveScheduler.Reset();
    bool invalidDeltaWave = false;
    for (int step = 0; step < 100; ++step) {
        invalidDeltaWave = invalidDeltaWave || waveScheduler.Update(
            std::numeric_limits<float>::quiet_NaN(), beachMix.surfVolume).trigger;
    }
    Check(!invalidDeltaWave,
          "non-finite time cannot advance the stronger-wave scheduler");
    island::WaveAccentRequest firstWave;
    for (int step = 0; step < 60 && !firstWave.trigger; ++step) {
        firstWave = waveScheduler.Update(0.25f, beachMix.surfVolume);
    }
    Check(firstWave.trigger, "shoreline scheduler emits its first wave within the authored window");
    Check(firstWave.variant <= 1U
              && firstWave.volume >= 0.06f && firstWave.volume <= 0.22f
              && firstWave.pitch >= -0.06f && firstWave.pitch <= 0.06f
              && firstWave.pan >= -0.32f && firstWave.pan <= 0.32f,
          "wave request variation remains inside conservative playback bounds");
    bool overlappedWave = false;
    for (int step = 0; step < 52; ++step) {
        overlappedWave = overlappedWave
            || waveScheduler.Update(0.25f, beachMix.surfVolume).trigger;
    }
    Check(!overlappedWave,
          "wave scheduler cannot overlap a four-second accent or repeat it too quickly");
    waveScheduler.Reset();
    bool inlandWave = false;
    for (int step = 0; step < 80; ++step) {
        inlandWave = inlandWave || waveScheduler.Update(0.25f, jungleMix.surfVolume).trigger;
    }
    Check(!inlandWave, "quiet inland ambience virtualizes stronger-wave accents");
    Check(waveScheduler.Update(0.25f, beachMix.surfVolume).trigger,
          "returning to audible surf permits a waiting wave accent");
    island::WaveAccentScheduler waveVariantScheduler;
    waveVariantScheduler.Reset();
    std::array<bool, 2> observedWaveVariants = {false, false};
    bool waveVariantOutOfRange = false;
    for (int step = 0; step < 1600; ++step) {
        const island::WaveAccentRequest wave =
            waveVariantScheduler.Update(0.25f, beachMix.surfVolume);
        if (wave.trigger) {
            if (wave.variant < observedWaveVariants.size()) {
                observedWaveVariants[wave.variant] = true;
            } else {
                waveVariantOutOfRange = true;
            }
        }
    }
    Check(!waveVariantOutOfRange && observedWaveVariants[0] && observedWaveVariants[1],
          "deterministic stronger-wave scheduling reaches both source variants");

    island::ShipwreckCreakScheduler creakScheduler;
    creakScheduler.Reset();
    bool distantCreak = false;
    for (int step = 0; step < 240; ++step) {
        distantCreak = distantCreak || creakScheduler.Update(
            0.25f, beachMix.surfVolume, 60.0f, true).trigger;
    }
    Check(!distantCreak, "shipwreck creaks remain inaudible away from the wreck");
    bool busyCreak = false;
    for (int step = 0; step < 120; ++step) {
        busyCreak = busyCreak || creakScheduler.Update(
            0.25f, beachMix.surfVolume, 8.0f, false).trigger;
    }
    Check(!busyCreak,
          "shipwreck creaks cannot escape a busy shared shoreline voice");
    const island::ShipwreckCreakRequest releasedCreak = creakScheduler.Update(
        0.01f, beachMix.surfVolume, 8.0f, true);
    Check(releasedCreak.trigger && releasedCreak.variant <= 1U
              && releasedCreak.volume >= 0.035f && releasedCreak.volume <= 0.16f
              && releasedCreak.pitch >= -0.075f && releasedCreak.pitch <= 0.045f,
          "a due nearby wreck creak resumes with bounded playback parameters");
    island::ShipwreckCreakScheduler creakVariantScheduler;
    creakVariantScheduler.Reset();
    std::array<bool, 2> observedCreakVariants = {false, false};
    bool invalidCreak = false;
    for (int step = 0; step < 4000; ++step) {
        const island::ShipwreckCreakRequest creak = creakVariantScheduler.Update(
            0.25f, beachMix.surfVolume, 7.0f, true);
        if (creak.trigger && creak.variant < observedCreakVariants.size()) {
            observedCreakVariants[creak.variant] = true;
        }
        invalidCreak = invalidCreak || creakVariantScheduler.Update(
            std::numeric_limits<float>::quiet_NaN(), beachMix.surfVolume,
            std::numeric_limits<float>::infinity(), true).trigger;
    }
    Check(!invalidCreak && observedCreakVariants[0] && observedCreakVariants[1],
          "wreck scheduling rejects invalid input and reaches both sparse variants");

    const island::WildlifeAudioSource nearbyBird{true, 28.0f, 16.0f, -8.0f};
    const island::WildlifeAudioSource nearbyMonkey{true, -18.0f, 2.0f, 21.0f};
    island::WildlifeCallScheduler heldWildlifeScheduler;
    heldWildlifeScheduler.Reset();
    bool callEscapedBusySlots = false;
    for (int step = 0; step < 80; ++step) {
        callEscapedBusySlots = callEscapedBusySlots
            || heldWildlifeScheduler.Update(0.25f, false, 90.0f, 0.0f, 2.0f, 0.0f,
                                            nearbyBird, nearbyMonkey, false).type
                != island::WildlifeCallType::None;
    }
    Check(!callEscapedBusySlots,
          "wildlife scheduling never exceeds the transient playback-slot budget");
    const island::WildlifeCallRequest releasedCall = heldWildlifeScheduler.Update(
        0.01f, false, 90.0f, 0.0f, 2.0f, 0.0f, nearbyBird, nearbyMonkey, true);
    Check(releasedCall.type != island::WildlifeCallType::None,
          "a due wildlife call resumes when a transient slot becomes available");

    island::WildlifeCallScheduler swimmingMonkeyScheduler;
    swimmingMonkeyScheduler.Reset();
    bool monkeyCalledWhileSwimming = false;
    const island::WildlifeAudioSource noBird;
    for (int step = 0; step < 160; ++step) {
        monkeyCalledWhileSwimming = monkeyCalledWhileSwimming
            || swimmingMonkeyScheduler.Update(
                   0.25f, true, -4.0f, 0.0f, 2.0f, 0.0f, noBird, nearbyMonkey, true).type
                == island::WildlifeCallType::Monkey;
    }
    Check(!monkeyCalledWhileSwimming,
          "interior monkey calls are suppressed while the player is swimming");

    island::WildlifeCallScheduler distantWildlifeScheduler;
    distantWildlifeScheduler.Reset();
    const island::WildlifeAudioSource distantBird{true, 440.0f, 80.0f, -310.0f};
    bool distantCallTriggered = false;
    for (int step = 0; step < 240; ++step) {
        distantCallTriggered = distantCallTriggered
            || distantWildlifeScheduler.Update(
                   0.25f, false, 90.0f, 0.0f, 2.0f, 0.0f,
                   distantBird, noBird, true).type != island::WildlifeCallType::None;
    }
    Check(!distantCallTriggered,
          "distant wildlife is virtualized instead of consuming an inaudible voice");

    island::WildlifeCallScheduler invalidWildlifeScheduler;
    invalidWildlifeScheduler.Reset();
    bool invalidWildlifeCallTriggered = false;
    for (int step = 0; step < 100; ++step) {
        invalidWildlifeCallTriggered = invalidWildlifeCallTriggered
            || invalidWildlifeScheduler.Update(
                   std::numeric_limits<float>::quiet_NaN(), false, 90.0f,
                   0.0f, 2.0f, 0.0f, nearbyBird, nearbyMonkey, true).type
                != island::WildlifeCallType::None;
    }
    Check(!invalidWildlifeCallTriggered,
          "non-finite time cannot advance the positional wildlife-call scheduler");
    bool invalidListenerCallTriggered = false;
    for (int step = 0; step < 80; ++step) {
        invalidListenerCallTriggered = invalidListenerCallTriggered
            || invalidWildlifeScheduler.Update(
                   0.25f, false, 90.0f,
                   std::numeric_limits<float>::quiet_NaN(), 2.0f, 0.0f,
                   nearbyBird, nearbyMonkey, true).type
                != island::WildlifeCallType::None;
    }
    Check(!invalidListenerCallTriggered,
          "non-finite listener positions cannot emit a positional wildlife call");

    island::WildlifeCallScheduler longWildlifeScheduler;
    longWildlifeScheduler.Reset();
    int longBirdCallCount = 0;
    int longMonkeyCallCount = 0;
    std::array<bool, 2> longBirdCallVariants = {false, false};
    std::array<bool, 2> longMonkeyCallVariants = {false, false};
    bool wildlifeRequestBoundsValid = true;
    for (int step = 0; step < 20 * 60 * 4; ++step) {
        const island::WildlifeCallRequest call = longWildlifeScheduler.Update(
            0.25f, false, 90.0f, 0.0f, 2.0f, 0.0f,
            nearbyBird, nearbyMonkey, true);
        if (call.type == island::WildlifeCallType::None) {
            continue;
        }
        wildlifeRequestBoundsValid = wildlifeRequestBoundsValid
            && call.variant <= 1U && call.source.available
            && call.volume >= 0.36f && call.volume <= 0.60f
            && call.pitch >= -0.08f && call.pitch <= 0.09f;
        if (call.type == island::WildlifeCallType::Bird) {
            ++longBirdCallCount;
            if (call.variant < longBirdCallVariants.size()) {
                longBirdCallVariants[call.variant] = true;
            }
        } else if (call.type == island::WildlifeCallType::Monkey) {
            ++longMonkeyCallCount;
            if (call.variant < longMonkeyCallVariants.size()) {
                longMonkeyCallVariants[call.variant] = true;
            }
        }
    }
    Check(wildlifeRequestBoundsValid
              && longBirdCallVariants[0] && longBirdCallVariants[1]
              && longMonkeyCallVariants[0] && longMonkeyCallVariants[1],
          "twenty-minute wildlife scheduling keeps playback values bounded and varies sources");
    Check(longBirdCallCount >= 20 && longBirdCallCount <= 70
              && longMonkeyCallCount >= 5 && longMonkeyCallCount <= 30,
          "wildlife calls remain sparse over a twenty-minute session");

    // Boar audio scheduling is independent of rendering and of the playback device.
    for (const auto kind : {island::BoarCallKind::Breath, island::BoarCallKind::Snuffle,
                           island::BoarCallKind::Grunt}) {
        const auto a = island::GenerateBoarCall(kind, 0);
        const auto b = island::GenerateBoarCall(kind, 1);
        Check(a.sampleRate == 22050 && a.samples.size() == 26460U
            && a.samples.front() == 0 && a.samples.back() == 0
            && b.samples.front() == 0 && b.samples.back() == 0
            && a.samples == island::GenerateBoarCall(kind, 0).samples
            && a.samples != b.samples
            && std::any_of(a.samples.begin(), a.samples.end(), [](auto s) { return s > 1000; })
            && std::all_of(a.samples.begin(), a.samples.end(), [](auto s) { return std::abs(s) < 21000; }),
            "boar PCM clips are deterministic, distinct, bounded and silent at endpoints");
        if (kind == island::BoarCallKind::Breath) {
            int zeroCrossings = 0;
            for (std::size_t sample = 1; sample < a.samples.size(); ++sample)
                zeroCrossings += (a.samples[sample - 1] < 0 && a.samples[sample] >= 0)
                    || (a.samples[sample - 1] > 0 && a.samples[sample] <= 0);
            Check(zeroCrossings < 4000,
                "boar breath synthesis regressed into high-frequency hiss");
        }
        Check(island::BoarCallVolume(kind, 0.0f) > island::BoarCallVolume(kind, 5.0f)
            && island::BoarCallVolume(kind, 50.0f) == 0.0f
            && island::BoarCallVolume(kind, -1.0f) == 0.0f
            && island::BoarCallVolume(kind, std::numeric_limits<float>::quiet_NaN()) == 0.0f,
            "boar attenuation is bounded and rejects invalid distances");
    }
    std::array<island::BoarState, island::BoarHerd::BoarCount> audioBoars{};
    for (std::size_t i = 0; i < audioBoars.size(); ++i) {
        audioBoars[i].x = static_cast<float>(i);
        audioBoars[i].simulationTier = island::WildlifeSimulationTier::Full;
        audioBoars[i].behavior = static_cast<island::BoarBehavior>(i % 4U);
    }
    {
        // The alarm: a loud grunt from the boar the bear sent off at the next free slot,
        // ahead of the herd's cooldowns; held while the slot is busy; dropped once stale.
        island::BoarCallScheduler alarmed; alarmed.Reset();
        (void)alarmed.Update(.25f, 0, 1.0f, 0, audioBoars, true);
        alarmed.NoteAlarm(2);
        const auto held = alarmed.Update(.25f, 0, 1.0f, 0, audioBoars, false);
        Check(held.kind == island::BoarCallKind::None && alarmed.AlarmPending(), "the boar alarm waits for a playback slot");
        const auto alarm = alarmed.Update(.25f, 0, 1.0f, 0, audioBoars, true);
        Check(alarm.kind == island::BoarCallKind::Grunt && alarm.animalIndex == 2U && alarm.variant == 1U
                  && alarm.pitch > .06f && alarm.volume > .3f && !alarmed.AlarmPending(),
              "the boar alarm is a loud pitched-up grunt from the boar the bear sent off, ahead of the herd's turns");
        island::BoarCallScheduler stale; stale.Reset();
        stale.NoteAlarm(2);
        for (int step = 0; step < 20; ++step) (void)stale.Update(.25f, 0, 1.0f, 0, audioBoars, false);
        Check(!stale.AlarmPending(), "a boar alarm with no slot for five seconds is dropped");
    }
    island::BoarCallScheduler boarCalls, repeatedBoarCalls;
    boarCalls.Reset(); repeatedBoarCalls.Reset();
    std::array<int, island::BoarHerd::BoarCount> boarCallCounts{};
    std::array<int, island::BoarHerd::BoarCount> previousBoarCall{};
    previousBoarCall.fill(-1000);
    std::array<bool, 2> boarVariants{};
    int previousHerdCall = -1000;
    bool boarBounds = true, boarDeterminism = true;
    for (int step = 0; step < 2400; ++step) {
        const auto call = boarCalls.Update(0.25f, 0.0f, 1.72f, 0.0f, audioBoars, true);
        const auto repeated = repeatedBoarCalls.Update(0.25f, 0.0f, 1.72f, 0.0f, audioBoars, true);
        boarDeterminism = boarDeterminism && call.kind == repeated.kind
            && call.animalIndex == repeated.animalIndex && call.variant == repeated.variant
            && call.volume == repeated.volume && call.pitch == repeated.pitch;
        if (call.kind == island::BoarCallKind::None) continue;
        const std::size_t i = call.animalIndex;
        const auto behavior = audioBoars[i].behavior;
        const auto expected = behavior == island::BoarBehavior::Rest ? island::BoarCallKind::Breath
            : behavior == island::BoarBehavior::Forage ? island::BoarCallKind::Snuffle
            : island::BoarCallKind::Grunt;
        boarBounds = boarBounds && (call.kind == expected || call.kind == island::BoarCallKind::Grunt)
            && step - previousHerdCall >= 10
            && step - previousBoarCall[i] >= 24 && call.volume > 0.0f && call.volume <= 0.56f
            && call.pitch >= -0.07f && call.pitch <= 0.07f && call.variant < 2U
            && call.source.available && call.source.x == audioBoars[i].x
            && std::abs(call.source.y - 0.55f) < 0.0001f;
        previousHerdCall = step; previousBoarCall[i] = step;
        ++boarCallCounts[i]; boarVariants[call.variant] = true;
    }
    Check(boarBounds && boarDeterminism && boarVariants[0] && boarVariants[1]
        && std::all_of(boarCallCounts.begin(), boarCallCounts.end(), [](int n) { return n > 5; }),
        "boar calls match behavior, vary, remain bounded and fairly service each animal");
    for(const auto behavior:{island::BoarBehavior::Rest,island::BoarBehavior::Forage}) {
        for(float distance:{2.0f,7.0f,15.0f,30.0f}) {
            std::array<island::BoarState,1> animals{};
            animals[0].scale=1;animals[0].behavior=behavior;
            animals[0].simulationTier=island::WildlifeSimulationTier::Full;
            island::BoarCallScheduler scheduler;scheduler.Reset();
            int grunts=0,details=0,firstGrunt=-1;
            for(int step=0;step<480;++step) {
                const auto call=scheduler.Update(.25f,distance,.55f,0,animals,true);
                if(call.kind==island::BoarCallKind::Grunt){++grunts;if(firstGrunt<0)firstGrunt=step;}
                if(call.kind==island::BoarCallKind::Breath||call.kind==island::BoarCallKind::Snuffle)++details;
            }
            Check(grunts>=2&&firstGrunt>=0&&firstGrunt<44,
                "resting/foraging boar must grunt without requiring movement or player approach");
            if(distance==2)Check(details>=2,"near boar lost its breathing/snuffling variation");
        }
    }
    const auto noBoarCall = [](const auto& animals, float x, float dt, bool slot) {
        island::BoarCallScheduler scheduler;
        scheduler.Reset();
        bool silent = true;
        for (int step = 0; step < 200; ++step) {
            const auto call = scheduler.Update(dt, x, 1.72f, 0.0f, animals, slot);
            silent = silent && call.kind == island::BoarCallKind::None;
        }
        return silent;
    };
    Check(noBoarCall(audioBoars, 0.0f, 0.25f, false)
        && noBoarCall(audioBoars, 300.0f, 0.25f, true)
        && noBoarCall(audioBoars, std::numeric_limits<float>::quiet_NaN(), 0.25f, true)
        && noBoarCall(audioBoars, 0.0f, std::numeric_limits<float>::quiet_NaN(), true)
        && noBoarCall(audioBoars, 0.0f, -1.0f, true)
        && noBoarCall(audioBoars, 0.0f, 0.0f, true),
        "boar calls respect unavailable slots, range, invalid listener and paused/invalid time");
    for (auto& boar : audioBoars) boar.simulationTier = island::WildlifeSimulationTier::Dormant;
    Check(noBoarCall(audioBoars, 0.0f, 0.25f, true), "dormant boars remain silent");
    for (auto& boar : audioBoars) {
        boar.simulationTier = island::WildlifeSimulationTier::Full;
        boar.x = std::numeric_limits<float>::infinity();
    }
    Check(noBoarCall(audioBoars, 0.0f, 0.25f, true), "invalid boar positions remain silent");
    Check(noBoarCall(std::span<const island::BoarState>{}, 0.0f, 0.25f, true),
        "empty boar population remains silent");
    std::array<island::BoarState, 1> loneBoar{};
    loneBoar[0].behavior = island::BoarBehavior::Wander;
    loneBoar[0].simulationTier = island::WildlifeSimulationTier::Full;
    island::BoarCallScheduler nearBoar, farBoar, heldBoar;
    nearBoar.Reset(); farBoar.Reset(); heldBoar.Reset();
    bool quieterAtDistance = false;
    for (int step = 0; step < 200; ++step) {
        const auto near = nearBoar.Update(0.25f, 0.0f, 1.72f, 0.0f, loneBoar, true);
        const auto far = farBoar.Update(0.25f, 20.0f, 1.72f, 0.0f, loneBoar, true);
        (void)heldBoar.Update(0.25f, 0.0f, 1.72f, 0.0f, loneBoar, false);
        if (near.kind != island::BoarCallKind::None) {
            quieterAtDistance = far.kind == near.kind && far.volume < near.volume * 0.40f;
        }
    }
    Check(quieterAtDistance, "boar volume attenuates with listener distance");
    Check(heldBoar.Update(0.0f, 0.0f, 1.72f, 0.0f, loneBoar, true).kind
        == island::BoarCallKind::None, "paused overdue boar calls cannot start playback");
    Check(heldBoar.Update(0.25f, 0.0f, 1.72f, 0.0f, loneBoar, true).kind
        == island::BoarCallKind::Grunt, "one due boar call resumes when a slot opens");
    Check(heldBoar.Update(0.25f, 0.0f, 1.72f, 0.0f, loneBoar, true).kind
        == island::BoarCallKind::None, "resuming boar audio cannot replay a backlog burst");

    for (const auto kind : {island::GoatCallKind::Breath,island::GoatCallKind::Bleat,
                           island::GoatCallKind::Hoof}) {
        const auto a=island::GenerateGoatCall(kind,0),b=island::GenerateGoatCall(kind,1);
        Check(a.sampleRate==22050&&a.samples.size()==26460U&&b.samples.size()==26460U
            &&a.samples.front()==0&&a.samples.back()==0&&b.samples.front()==0
            &&b.samples.back()==0&&a.samples==island::GenerateGoatCall(kind,0).samples
            &&a.samples!=b.samples
            &&std::any_of(a.samples.begin(),a.samples.end(),[](auto sample){return sample>1000;})
            &&std::all_of(a.samples.begin(),a.samples.end(),[](auto sample){return std::abs(sample)<21000;}),
            "goat PCM clips are deterministic, distinct, bounded and silent at endpoints");
        if(kind==island::GoatCallKind::Breath) {
            int zeroCrossings=0;
            for(std::size_t sample=1;sample<a.samples.size();++sample)
                zeroCrossings+=(a.samples[sample-1]<0&&a.samples[sample]>=0)
                    ||(a.samples[sample-1]>0&&a.samples[sample]<=0);
            Check(zeroCrossings<4000,
                "goat breath synthesis regressed into high-frequency hiss");
        } else if(kind==island::GoatCallKind::Bleat) {
            int zeroCrossings=0;
            for(std::size_t sample=1;sample<a.samples.size();++sample)
                zeroCrossings+=(a.samples[sample-1]<0&&a.samples[sample]>=0)
                    ||(a.samples[sample-1]>0&&a.samples[sample]<=0);
            const float crossingsPerSecond=static_cast<float>(zeroCrossings)/1.2f;
            Check(crossingsPerSecond>150.0f&&crossingsPerSecond<290.0f,
                "goat bleat regressed toward a high narrow-band insect buzz");
        }
        Check(island::GoatCallVolume(kind,0)>island::GoatCallVolume(kind,5)
            &&island::GoatCallVolume(kind,70)==0&&island::GoatCallVolume(kind,-1)==0
            &&island::GoatCallVolume(kind,std::numeric_limits<float>::quiet_NaN())==0,
            "goat attenuation is bounded and rejects invalid distances");
    }
    std::array<island::GoatState,island::GoatHabitat::Count> audioGoats{};
    for(std::size_t i=0;i<audioGoats.size();++i) {
        audioGoats[i].x=static_cast<float>(i)*1.25f;
        audioGoats[i].scale=.82f+static_cast<float>(i)*.04f;
        audioGoats[i].behavior=static_cast<island::GoatBehavior>(i%4U);
    }
    island::GoatCallScheduler goatCalls,repeatedGoatCalls;
    goatCalls.Reset();repeatedGoatCalls.Reset();
    std::array<int,island::GoatHabitat::Count> goatCounts{};
    std::array<bool,4> goatKinds{};
    std::array<bool,2> goatVariants{};
    int previousGoatCall=-1000;
    bool goatDeterministic=true,goatBounds=true;
    for(int step=0;step<2400;++step) {
        const auto call=goatCalls.Update(.25f,0,1.72f,0,audioGoats,true);
        const auto repeated=repeatedGoatCalls.Update(.25f,0,1.72f,0,audioGoats,true);
        goatDeterministic=goatDeterministic&&call.kind==repeated.kind
            &&call.animalIndex==repeated.animalIndex&&call.variant==repeated.variant
            &&call.volume==repeated.volume&&call.pitch==repeated.pitch;
        if(call.kind==island::GoatCallKind::None)continue;
        goatBounds=goatBounds&&call.animalIndex<audioGoats.size()
            &&step-previousGoatCall>=11&&call.volume>0&&call.volume<=.56f
            &&call.pitch>=-.07f&&call.pitch<=.08f&&call.variant<2U
            &&call.source.available&&call.source.x==audioGoats[call.animalIndex].x;
        previousGoatCall=step;++goatCounts[call.animalIndex];
        goatKinds[static_cast<std::size_t>(call.kind)]=true;goatVariants[call.variant]=true;
    }
    Check(goatDeterministic&&goatBounds&&goatVariants[0]&&goatVariants[1]
        &&goatKinds[static_cast<std::size_t>(island::GoatCallKind::Breath)]
        &&goatKinds[static_cast<std::size_t>(island::GoatCallKind::Bleat)]
        &&goatKinds[static_cast<std::size_t>(island::GoatCallKind::Hoof)]
        &&std::all_of(goatCounts.begin(),goatCounts.end(),[](int count){return count>4;}),
        "goat calls match behavior, vary, remain bounded and fairly service the herd");
    const auto noGoatCall=[](const auto& goats,float listenerX,float dt,bool slot) {
        island::GoatCallScheduler scheduler;scheduler.Reset();
        for(int step=0;step<200;++step)
            if(scheduler.Update(dt,listenerX,1.72f,0,goats,slot).kind
                !=island::GoatCallKind::None)return false;
        return true;
    };
    Check(noGoatCall(audioGoats,0,.25f,false)&&noGoatCall(audioGoats,300,.25f,true)
        &&noGoatCall(audioGoats,std::numeric_limits<float>::quiet_NaN(),.25f,true)
        &&noGoatCall(audioGoats,0,0,true)
        &&noGoatCall(std::span<const island::GoatState>{},0,.25f,true),
        "goat calls respect shared slots, range, valid listeners, paused time and empty herds");
    {
        // The sentinel's alarm: a loud bleat from goat 0 at the next free slot, ahead of the
        // herd's cooldowns; held while the slot is busy; dropped once stale.
        island::GoatCallScheduler alarmed;alarmed.Reset();
        (void)alarmed.Update(.25f,0,1.72f,0,audioGoats,true);   // the herd's first call takes the cooldowns
        alarmed.NoteAlarm(0);
        const auto held=alarmed.Update(.25f,0,1.72f,0,audioGoats,false);
        Check(held.kind==island::GoatCallKind::None&&alarmed.AlarmPending(),"the alarm waits for a playback slot");
        const auto alarm=alarmed.Update(.25f,0,1.72f,0,audioGoats,true);
        Check(alarm.kind==island::GoatCallKind::Bleat&&alarm.animalIndex==0U&&alarm.variant==1U
            &&alarm.pitch>.07f&&alarm.volume>.3f&&!alarmed.AlarmPending(),
            "the sentinel's alarm is a loud pitched-up bleat from goat 0 ahead of the herd's turns");
        island::GoatCallScheduler stale;stale.Reset();
        stale.NoteAlarm(0);
        for(int step=0;step<20;++step) (void)stale.Update(.25f,0,1.72f,0,audioGoats,false);
        Check(!stale.AlarmPending(),"an alarm with no slot for five seconds is dropped");
    }

    const auto hissA=island::GenerateSnakeHiss(0),hissB=island::GenerateSnakeHiss(1);
    Check(hissA.sampleRate==22050 && hissA.samples.size()==22050U
        && hissB.sampleRate==22050 && hissB.samples.size()==22050U
        && hissA.samples!=hissB.samples && hissA.samples.front()==0
        && hissA.samples.back()==0 && hissB.samples.front()==0 && hissB.samples.back()==0
        && hissA.samples==island::GenerateSnakeHiss(0).samples,
        "snake hiss PCM is deterministic, distinct, bounded and silent at endpoints");
    {
        // The deer's alarm snort: deterministic, two distinct variants, a second of PCM,
        // silent at both ends, its blast in the first half second and the tail gone by
        // the end.
        const auto snortA=island::GenerateDeerSnort(0),snortB=island::GenerateDeerSnort(1);
        auto peakIn=[](const island::AmbientLoop& loop,std::size_t from,std::size_t to){int peak=0;for(std::size_t i=from;i<to&&i<loop.samples.size();++i)peak=std::max(peak,std::abs(static_cast<int>(loop.samples[i])));return peak;};
        Check(snortA.sampleRate==22050&&snortA.samples.size()==22050U&&snortB.samples.size()==22050U
            &&snortA.samples!=snortB.samples&&snortA.samples==island::GenerateDeerSnort(0).samples
            &&snortA.samples.front()==0&&snortA.samples.back()==0&&snortB.samples.front()==0&&snortB.samples.back()==0,
            "deer snort PCM is deterministic, distinct, a second long and silent at endpoints");
        Check(peakIn(snortA,0,11025)>8000&&peakIn(snortA,16000,22050)<400&&peakIn(snortB,8800,15500)>6000,
            "the snort's blast lies in the first half second, the doubled variant's second blast after it, and the tail is gone");
    }
    {
        // The house gecko's chirp: deterministic, three distinct variants of 1.1 s, silent at
        // both ends; its "chik"s, counted as bursts, are five, six and seven, and high: some
        // three to six thousand zero crossings a second (halved) in the first chirp.
        bool pcm=true,counts=true,high=true;
        for(std::uint8_t variant=0;variant<3;++variant){
            const auto chirp=island::GenerateGeckoChirp(variant);
            pcm=pcm&&chirp.sampleRate==22050&&chirp.samples.size()==static_cast<std::size_t>(22050*1.1f)
                &&chirp.samples==island::GenerateGeckoChirp(variant).samples
                &&chirp.samples.front()==0&&chirp.samples.back()==0
                &&(variant==0||chirp.samples!=island::GenerateGeckoChirp(0).samples);
            int bursts=0;bool loud=false;
            for(std::size_t from=0;from<chirp.samples.size();from+=110){
                int peak=0;
                for(std::size_t i=from;i<from+110&&i<chirp.samples.size();++i)peak=std::max(peak,std::abs(static_cast<int>(chirp.samples[i])));
                if(peak>6000&&!loud)++bursts;
                loud=peak>6000;
            }
            counts=counts&&bursts==5+variant;
            int crossings=0;
            for(std::size_t i=662;i<992;++i)crossings+=(chirp.samples[i-1]<0)!=(chirp.samples[i]<0);
            const float hertz=static_cast<float>(crossings)*0.5f/(330.0f/22050.0f);
            high=high&&hertz>3000.0f&&hertz<6000.0f;
        }
        Check(pcm,"gecko chirp PCM is deterministic, three distinct variants of 1.1 s, silent at the ends");
        Check(counts,"a gecko chirp variant runs five, six or seven chik bursts");
        Check(high,"a gecko's chik is a high tone");
        // A palm's frond clatter: deterministic, three distinct variants of 1.6 s, silent at
        // both ends, a run of separate hard clacks (bursts), loudest in its middle, and dry
        // (bright: many zero crossings).
        bool clatterPcm=true,clacks=true,dry=true,swells=true;
        for(std::uint8_t variant=0;variant<3;++variant){
            const auto clatter=island::GeneratePalmClatter(variant);
            clatterPcm=clatterPcm&&clatter.sampleRate==22050&&clatter.samples.size()==static_cast<std::size_t>(22050*1.6f)
                &&clatter.samples==island::GeneratePalmClatter(variant).samples
                &&clatter.samples.front()==0&&clatter.samples.back()==0
                &&(variant==0||clatter.samples!=island::GeneratePalmClatter(0).samples);
            int bursts=0;bool loud=false;
            for(std::size_t from=0;from<clatter.samples.size();from+=110){
                int peak=0;
                for(std::size_t i=from;i<from+110&&i<clatter.samples.size();++i)peak=std::max(peak,std::abs(static_cast<int>(clatter.samples[i])));
                if(peak>5000&&!loud)++bursts;
                loud=peak>5000;
            }
            clacks=clacks&&bursts>=8;
            const auto peakIn=[&](std::size_t from,std::size_t to){int peak=0;for(std::size_t i=from;i<to;++i)peak=std::max(peak,std::abs(static_cast<int>(clatter.samples[i])));return peak;};
            swells=swells&&peakIn(11025,24255)>peakIn(0,2200);
            int crossings=0;
            for(std::size_t i=1;i<clatter.samples.size();++i)crossings+=(clatter.samples[i-1]<0)!=(clatter.samples[i]<0);
            dry=dry&&static_cast<float>(crossings)*0.5f/1.6f>1500.0f;
        }
        Check(clatterPcm,"palm clatter PCM is deterministic, three distinct variants of 1.6 s, silent at the ends");
        Check(clacks&&swells,"a palm clatter is a run of separate clacks, louder in the gust's middle than at its start");
        Check(dry,"a palm clatter is dry and bright");
        // Through an hour of night the two house geckos at the hut chirp between sixty and a
        // hundred and sixty times, both of them, each from home at rest; by day not once.
        island::HutPlacement hut{};
        hut.x=10.0f;hut.z=-4.0f;hut.yawRadians=0.6f;hut.floorY=2.0f;
        std::array<int,island::hut_geckos::Count> chirps{};
        int byDay=0;bool atRest=true;
        for(int step=1;step<=3600*30;++step){
            const float from=static_cast<float>(step-1)/30.0f,to=static_cast<float>(step)/30.0f;
            const std::size_t night=island::hut_geckos::ChirpBetween(from,to,1.0f);
            if(night<island::hut_geckos::Count){
                ++chirps[night];
                atRest=atRest&&island::hut_geckos::At(hut,to,1.0f)[night].behavior==island::GeckoBehavior::Rest;
            }
            byDay+=island::hut_geckos::ChirpBetween(from,to,0.0f)<island::hut_geckos::Count;
        }
        std::cout<<"hut gecko chirps in a night hour: "<<chirps[0]<<" + "<<chirps[1]<<'\n';
        Check(chirps[0]+chirps[1]>=60&&chirps[0]+chirps[1]<=160&&chirps[0]>0&&chirps[1]>0&&atRest&&byDay==0,
            "the hut's house geckos chirp every half minute or so through the night, from home, and not by day");
    }
    {
        // The resident's stride: a footfall every 0.68 m of walking, left and right in turn;
        // standing resets the stride; an invalid position gives none.
        island::ResidentStepTracker stride;stride.Reset();
        int steps=0;std::uint8_t last=0;bool alternates=true;
        for(int i=0;i<=34;++i) {
            if(stride.Step(static_cast<float>(i)*0.1f,0.0f,true)) { ++steps; if(steps>1) alternates=alternates&&stride.Variant()!=last; last=stride.Variant(); }
        }
        Check(steps==5&&alternates,"a walk of 3.4 m gives five footfalls, left and right in turn");
        Check(!stride.Step(3.5f,0.0f,false)&&!stride.Step(4.2f,0.0f,true),"standing resets the stride, so the next step is primed afresh");
        Check(!stride.Step(std::numeric_limits<float>::quiet_NaN(),0.0f,true),"an invalid position gives no footfall");
    }
    {
        // The resident's whistle: deterministic, two distinct tunes, 1.4 s of PCM silent at
        // both ends, tonal (a strong, narrow peak) rather than noise.
        const auto tuneA=island::GenerateResidentWhistle(0),tuneB=island::GenerateResidentWhistle(1);
        const std::size_t frames=static_cast<std::size_t>(22050*1.4f);
        int peakA=0;for(const auto s:tuneA.samples) peakA=std::max(peakA,std::abs(static_cast<int>(s)));
        // Zero crossings in the first note (0.1–0.3 s) of tune A at 880 Hz: about 1760 per second.
        int crossings=0;for(std::size_t i=2206;i<6615;++i) if((tuneA.samples[i]>=0)!=(tuneA.samples[i-1]>=0)) ++crossings;
        const float perSecond=static_cast<float>(crossings)/(0.2f);
        Check(tuneA.sampleRate==22050&&tuneA.samples.size()==frames&&tuneB.samples.size()==frames
            &&tuneA.samples!=tuneB.samples&&tuneA.samples==island::GenerateResidentWhistle(0).samples
            &&tuneA.samples.front()==0&&tuneA.samples.back()==0&&tuneB.samples.front()==0&&tuneB.samples.back()==0,
            "resident whistle PCM is deterministic, two tunes, 1.4 s long and silent at endpoints");
        Check(peakA>6000&&perSecond>1500.0f&&perSecond<2100.0f,
            "the whistle's first note is a clear tone near 880 Hz");
    }
    {
        // The wing clap: deterministic, two distinct variants, half a second silent at both
        // ends, beating (several separate bursts) and dying away by the end.
        const auto clapA=island::GenerateWingClap(0),clapB=island::GenerateWingClap(1);
        const std::size_t frames=static_cast<std::size_t>(22050*0.5f);
        int bursts=0;bool loud=false;
        for(std::size_t i=0;i<clapA.samples.size();i+=220) {
            int peak=0;for(std::size_t j=i;j<std::min(i+220,clapA.samples.size());++j) peak=std::max(peak,std::abs(static_cast<int>(clapA.samples[j])));
            const bool now=peak>2500; if(now&&!loud) ++bursts; loud=now;
        }
        int tail=0;for(std::size_t i=frames-900;i<frames;++i) tail=std::max(tail,std::abs(static_cast<int>(clapA.samples[i])));
        Check(clapA.sampleRate==22050&&clapA.samples.size()==frames&&clapB.samples.size()==frames
            &&clapA.samples!=clapB.samples&&clapA.samples==island::GenerateWingClap(0).samples
            &&clapA.samples.front()==0&&clapA.samples.back()==0&&clapB.samples.front()==0&&clapB.samples.back()==0,
            "wing clap PCM is deterministic, two variants, half a second and silent at endpoints");
        Check(bursts>=4&&tail<600,"the wing clap beats several times and dies away by the end");
    }
    Check(island::SnakeHissVolume(0)>.25f
        && island::SnakeHissVolume(8)<island::SnakeHissVolume(2)
        && island::SnakeHissVolume(16)==0 && island::SnakeHissVolume(-1)==0
        && island::SnakeHissVolume(std::numeric_limits<float>::quiet_NaN())==0,
        "snake hiss attenuation is local and rejects invalid distances");
    std::array<island::SnakeState,island::SnakePopulation::Count> audioSnakes{};
    for(std::size_t i=0;i<audioSnakes.size();++i) {
        auto& snake=audioSnakes[i];
        const float angle=2.0f*std::numbers::pi_v<float>*static_cast<float>(i)
            / static_cast<float>(audioSnakes.size());
        snake.body.front()={6.0f*std::cos(angle),.05f,6.0f*std::sin(angle)};
        snake.behavior=island::SnakeBehavior::Retreat;
        snake.tier=island::WildlifeSimulationTier::Full;
    }
    island::SnakeHissScheduler snakeHisses,repeatedSnakeHisses;
    snakeHisses.Reset();repeatedSnakeHisses.Reset();
    std::array<int,island::SnakePopulation::Count> hissCounts{};
    std::array<bool,2> hissVariants{};
    int lastHiss=-1000;
    bool hissDeterministic=true,hissBounded=true;
    for(int step=0;step<1600;++step) {
        const auto hiss=snakeHisses.Update(.25f,0,1.7f,0,audioSnakes,true);
        const auto repeat=repeatedSnakeHisses.Update(.25f,0,1.7f,0,audioSnakes,true);
        hissDeterministic=hissDeterministic&&hiss.trigger==repeat.trigger
            &&hiss.animalIndex==repeat.animalIndex&&hiss.variant==repeat.variant
            &&hiss.volume==repeat.volume&&hiss.pitch==repeat.pitch;
        if(!hiss.trigger)continue;
        hissBounded=hissBounded&&hiss.animalIndex<audioSnakes.size()
            &&step-lastHiss>=12&&hiss.volume>0&&hiss.volume<=.31f
            &&hiss.pitch>=-.08f&&hiss.pitch<=.08f&&hiss.source.available
            &&hiss.source.x==audioSnakes[hiss.animalIndex].body.front().x;
        lastHiss=step;++hissCounts[hiss.animalIndex];hissVariants[hiss.variant]=true;
    }
    Check(hissDeterministic&&hissBounded&&hissVariants[0]&&hissVariants[1]
        &&std::all_of(hissCounts.begin(),hissCounts.end(),[](int count){return count>0;}),
        "snake hisses are deterministic, varied, sparse and fairly service nearby snakes");
    const auto noSnakeHiss=[](auto snakes,float listenerX,float dt,bool slot) {
        island::SnakeHissScheduler scheduler;scheduler.Reset();
        for(int step=0;step<80;++step)
            if(scheduler.Update(dt,listenerX,1.7f,0,snakes,slot).trigger)return false;
        return true;
    };
    Check(noSnakeHiss(audioSnakes,0,.25f,false)
        &&noSnakeHiss(audioSnakes,100,.25f,true)
        &&noSnakeHiss(audioSnakes,std::numeric_limits<float>::quiet_NaN(),.25f,true)
        &&noSnakeHiss(audioSnakes,0,0,true),
        "snake hisses respect shared slots, local range, valid listeners and paused time");
    for(auto& snake:audioSnakes)snake.behavior=island::SnakeBehavior::Explore;
    Check(noSnakeHiss(audioSnakes,0,.25f,true),"non-retreating snakes hissed without close awareness");
    auto oneEligibleSnake=audioSnakes;
    oneEligibleSnake.front().behavior=island::SnakeBehavior::Retreat;
    oneEligibleSnake.front().tier=island::WildlifeSimulationTier::Full;
    for(std::size_t i=1;i<oneEligibleSnake.size();++i)
        oneEligibleSnake[i].tier=island::WildlifeSimulationTier::Dormant;
    island::SnakeHissScheduler oneSnakeScheduler;
    oneSnakeScheduler.Reset();
    bool oneSnakeTriggered=false;
    for(int step=0;step<100&&!oneSnakeTriggered;++step)
        oneSnakeTriggered=oneSnakeScheduler.Update(.1f,0,1.7f,0,oneEligibleSnake,true).trigger;
    Check(oneSnakeTriggered,"one close retreating snake never received a hiss request");
    for(auto& snake:audioSnakes) {
        snake.behavior=island::SnakeBehavior::Retreat;
        snake.tier=island::WildlifeSimulationTier::Dormant;
    }
    Check(noSnakeHiss(audioSnakes,0,.25f,true),"dormant snakes hissed");

    island::SoundscapeModel longSoundscapeModel;
    island::WaterMovementModel longWaterMovementModel;
    island::WaveAccentScheduler longWaveScheduler;
    island::WaterTransitionScheduler longTransitionScheduler;
    longSoundscapeModel.Reset(2.0f, false);
    longWaterMovementModel.Reset(0.0f, false, 0.0f);
    longWaveScheduler.Reset();
    longTransitionScheduler.Reset(0.0f, false);
    constexpr float longStepSeconds = 1.0f / 60.0f;
    constexpr int longSessionSteps = 20 * 60 * 60;
    constexpr int biomeBlockSteps = 20 * 60;
    bool longSessionStable = true;
    int longWaveCount = 0;
    int longTransitionCount = 0;
    std::array<bool, 2> longWaveVariants = {false, false};
    std::array<bool, 2> longTransitionVariants = {false, false};
    for (int step = 0; step < longSessionSteps; ++step) {
        const int block = (step / biomeBlockSteps) % 4;
        const int blockStep = step % biomeBlockSteps;
        const float signedShoreDistance = block == 0 ? 2.0f
            : block == 1 ? 140.0f
            : block == 2 ? -8.0f : 8.0f;
        const bool swimming = block == 2;
        const float waterDepth = block == 2 ? 0.7f
            : block == 3 && blockStep < biomeBlockSteps / 2 ? 0.12f : 0.0f;
        const float speed = block == 1 ? 2.8f : swimming ? 1.8f : 3.2f;
        const float oceanCurrentStrength = swimming
            ? static_cast<float>(blockStep) / static_cast<float>(biomeBlockSteps - 1)
            : 0.0f;
        const island::SoundscapeMix longMix = longSoundscapeModel.Update(
            signedShoreDistance, swimming, longStepSeconds);
        const island::WaterMovementMix longWaterMix = longWaterMovementModel.Update(
            waterDepth, swimming, speed, longStepSeconds, oceanCurrentStrength);
        const island::WaveAccentRequest longWave = longWaveScheduler.Update(
            longStepSeconds, longMix.surfVolume);
        const island::WaterTransitionRequest longTransition = longTransitionScheduler.Update(
            waterDepth, swimming, speed, longStepSeconds);

        longSessionStable = longSessionStable
            && std::isfinite(longMix.surfVolume) && longMix.surfVolume >= 0.0f
            && longMix.surfVolume <= 0.40f
            && std::isfinite(longMix.breezeVolume) && longMix.breezeVolume >= 0.0f
            && longMix.breezeVolume <= 0.20f
            && std::isfinite(longMix.jungleVolume) && longMix.jungleVolume >= 0.0f
            && longMix.jungleVolume <= 0.08f
            && std::isfinite(longWaterMix.volume) && longWaterMix.volume >= 0.0f
            && longWaterMix.volume <= 0.18f
            && std::isfinite(longWaterMix.pitch) && longWaterMix.pitch >= -0.23f
            && longWaterMix.pitch <= 0.15f
            && std::isfinite(longWaterMix.currentCue)
            && longWaterMix.currentCue >= 0.0f && longWaterMix.currentCue <= 1.0f;
        if (longWave.trigger) {
            ++longWaveCount;
            if (longWave.variant < longWaveVariants.size()) {
                longWaveVariants[longWave.variant] = true;
            } else {
                longSessionStable = false;
            }
        }
        if (longTransition.type != island::WaterTransitionType::None) {
            ++longTransitionCount;
            if (longTransition.variant < longTransitionVariants.size()) {
                longTransitionVariants[longTransition.variant] = true;
            } else {
                longSessionStable = false;
            }
        }
    }
    Check(longSessionStable,
          "twenty-minute soundscape simulation remains finite and inside mix bounds");
    Check(longWaveCount >= 20 && longWaveCount <= 80
              && longWaveVariants[0] && longWaveVariants[1],
          "long soundscape simulation keeps wave activity bounded and varied");
    Check(longTransitionCount == 30
              && longTransitionVariants[0] && longTransitionVariants[1],
          "long soundscape simulation emits exactly one varied accent per water crossing");

    island::TouchControls touchControls;
    std::array<island::TouchContact, 2> touchContacts = {{
        {11, 180.0f, 520.0f, island::TouchPhase::Pressed},
        {23, 1000.0f, 360.0f, island::TouchPhase::Pressed},
    }};
    island::TouchControlState touchState = touchControls.Update(touchContacts, 1280.0f, 720.0f);
    Check(touchState.movementActive && touchState.lookActive,
          "touch controls assign simultaneous left movement and right look contacts");
    CheckNear(touchState.moveForward, 0.0f, 0.0001f,
              "movement touch begins at a neutral floating-stick anchor");
    CheckNear(touchState.lookDeltaX, 0.0f, 0.0001f,
              "look touch begins without a camera jump");
    CheckNear(touchState.lookTouchX, 1000.0f, 0.0001f,
              "look presentation tracks its initial contact position");

    touchContacts[0] = {11, 230.0f, 440.0f, island::TouchPhase::Moved};
    touchContacts[1] = {23, 1032.0f, 344.0f, island::TouchPhase::Moved};
    touchState = touchControls.Update(touchContacts, 1280.0f, 720.0f);
    Check(touchState.moveForward > 0.4f && touchState.moveRight > 0.2f,
          "left touch produces proportional forward and strafe movement");
    CheckNear(touchState.lookDeltaX, 32.0f, 0.0001f,
              "right touch produces relative horizontal look delta");
    CheckNear(touchState.lookDeltaY, -16.0f, 0.0001f,
              "right touch produces relative vertical look delta");
    CheckNear(touchState.lookTouchX, 1032.0f, 0.0001f,
              "look presentation follows the owned contact");
    CheckNear(touchState.lookTouchY, 344.0f, 0.0001f,
              "look presentation preserves vertical contact position");
    Check(touchState.runRequested,
          "a near-full movement-stick throw requests the faster land pace");

    touchContacts[0] = {11, 900.0f, -200.0f, island::TouchPhase::Moved};
    touchState = touchControls.Update(touchContacts, 1280.0f, 720.0f);
    Check(std::hypot(touchState.moveRight, touchState.moveForward) <= 1.0001f,
          "movement stick clamps extreme drags to unit magnitude");
    Check(touchState.movementActive && touchState.lookActive,
          "touch roles remain owned after contacts cross the screen centre");

    const std::array<island::TouchContact, 1> releasedMovement = {{
        {11, 900.0f, -200.0f, island::TouchPhase::Released},
    }};
    touchState = touchControls.Update(releasedMovement, 1280.0f, 720.0f);
    Check(!touchState.movementActive && !touchState.lookActive,
          "released and missing contacts cancel both touch roles");
    CheckNear(touchState.moveForward, 0.0f, 0.0001f,
              "cancelled movement returns neutral output");
    Check(!touchState.runRequested,
          "releasing the movement contact cancels its run request");

    const std::array<island::TouchContact, 1> tinyMovement = {{
        {31, 160.0f, 500.0f, island::TouchPhase::Pressed},
    }};
    (void)touchControls.Update(tinyMovement, 1280.0f, 720.0f);
    const std::array<island::TouchContact, 1> tinyMovementMoved = {{
        {31, 164.0f, 496.0f, island::TouchPhase::Moved},
    }};
    touchState = touchControls.Update(tinyMovementMoved, 1280.0f, 720.0f);
    CheckNear(touchState.moveForward, 0.0f, 0.0001f,
              "movement dead zone rejects small finger jitter");
    touchState = touchControls.Update(tinyMovementMoved, 1000.0f, 600.0f);
    Check(touchState.movementActive && touchState.moveForward == 0.0f,
          "display resize safely re-anchors an active movement contact");

    island::TouchControls touchRunControls;
    std::array<island::TouchContact, 1> touchRunContact = {{
        {41, 150.0f, 500.0f, island::TouchPhase::Pressed},
    }};
    (void)touchRunControls.Update(touchRunContact, 1280.0f, 720.0f);
    touchRunContact[0] = {41, 250.0f, 500.0f, island::TouchPhase::Moved};
    island::TouchControlState touchRunState = touchRunControls.Update(
        touchRunContact, 1280.0f, 720.0f);
    Check(touchRunState.runRequested,
          "full touch-stick travel engages touch running");
    touchRunContact[0] = {41, 235.0f, 500.0f, island::TouchPhase::Moved};
    touchRunState = touchRunControls.Update(touchRunContact, 1280.0f, 720.0f);
    Check(touchRunState.runRequested,
          "touch running remains engaged inside its release hysteresis");
    touchRunContact[0] = {41, 220.0f, 500.0f, island::TouchPhase::Moved};
    touchRunState = touchRunControls.Update(touchRunContact, 1280.0f, 720.0f);
    Check(!touchRunState.runRequested,
          "easing the stick inward restores touch walking");

    island::TouchControls touchSensitivityControls;
    const island::TouchSensitivitySelectorLayout sensitivitySelector =
        island::TouchControls::SensitivitySelectorLayout(1280.0f, 720.0f);
    std::array<island::TouchContact, 1> sensitivityContact = {{
        {51, sensitivitySelector.x, sensitivitySelector.y, island::TouchPhase::Pressed},
    }};
    island::TouchControlState sensitivityState = touchSensitivityControls.Update(
        sensitivityContact, 1280.0f, 720.0f);
    Check(sensitivityState.lookSensitivity == island::TouchLookSensitivityLevel::High
              && sensitivityState.lookSensitivityChanged
              && sensitivityState.sensitivitySelectorActive
              && !sensitivityState.lookActive,
          "touch sensitivity selector chooses high without stealing the look role");
    CheckNear(island::TouchControls::LookSensitivityScale(
                  sensitivityState.lookSensitivity), 1.35f, 0.0001f,
              "high touch sensitivity exposes the documented bounded multiplier");
    sensitivityContact[0].phase = island::TouchPhase::Moved;
    sensitivityState = touchSensitivityControls.Update(
        sensitivityContact, 1280.0f, 720.0f);
    Check(sensitivityState.lookSensitivity == island::TouchLookSensitivityLevel::High
              && !sensitivityState.lookSensitivityChanged
              && sensitivityState.sensitivitySelectorActive
              && !sensitivityState.lookActive,
          "holding the sensitivity selector cannot cycle repeatedly or rotate the camera");
    sensitivityContact[0].phase = island::TouchPhase::Released;
    sensitivityState = touchSensitivityControls.Update(
        sensitivityContact, 1280.0f, 720.0f);
    Check(!sensitivityState.sensitivitySelectorActive,
          "releasing the sensitivity selector clears touch-interaction ownership");
    sensitivityContact[0] = {52, sensitivitySelector.x, sensitivitySelector.y,
                             island::TouchPhase::Pressed};
    sensitivityState = touchSensitivityControls.Update(
        sensitivityContact, 1280.0f, 720.0f);
    Check(sensitivityState.lookSensitivity == island::TouchLookSensitivityLevel::Low
              && sensitivityState.lookSensitivityChanged,
          "a second sensitivity tap cycles from high to low");
    (void)touchSensitivityControls.Update({}, 1000.0f, 600.0f);
    Check(touchSensitivityControls.State().lookSensitivity
              == island::TouchLookSensitivityLevel::Low,
          "display resize releases touch ownership without losing sensitivity selection");
    CheckNear(island::TouchControls::LookSensitivityScale(
                  island::TouchLookSensitivityLevel::Low), 0.72f, 0.0001f,
              "low touch sensitivity exposes the documented bounded multiplier");
    // Underwater the water limits sight; above the surface nothing changes, and the change sets in
    // gradually over the first metre so an eye bobbing at the surface does not pop.
    island::underwater_fog::Update(-0.4f);
    Check(island::underwater_fog::Live().amount == 0.0f, "no underwater fog above the sea surface");
    island::underwater_fog::Update(0.3f);
    const float shallowFog = island::underwater_fog::Live().amount;
    island::underwater_fog::Update(3.0f);
    const float deepFog = island::underwater_fog::Live().amount;
    const float deepGreen = island::underwater_fog::Live().green;
    island::underwater_fog::Update(12.0f);
    Check(shallowFog > 0.0f && shallowFog < 0.6f && deepFog == 1.0f
              && island::underwater_fog::Live().green < deepGreen,
          "underwater fog ramps in over the first metre and the water darkens with depth");
    island::underwater_fog::Update(std::numeric_limits<float>::quiet_NaN());
    Check(island::underwater_fog::Live().amount == 0.0f, "a non-finite eye depth leaves the land's fog");

    island::TouchControls touchActionControls;
    const island::TouchSensitivitySelectorLayout rideButton = island::TouchControls::ActionButtonLayout(
        island::TouchAction::Ride, 1280.0f, 720.0f);
    const island::TouchSensitivitySelectorLayout torchButton = island::TouchControls::ActionButtonLayout(
        island::TouchAction::Torch, 1280.0f, 720.0f);
    Check(rideButton.x > 640.0f && rideButton.y > sensitivitySelector.y + sensitivitySelector.radius + rideButton.radius
              && torchButton.y > rideButton.y + rideButton.radius + torchButton.radius
              && torchButton.y + torchButton.radius < 720.0f * 0.5f,
          "the touch action buttons stack under the sensitivity selector in the upper right");
    std::array<island::TouchContact, 1> actionContact = {{
        {61, torchButton.x, torchButton.y, island::TouchPhase::Pressed},
    }};
    island::TouchControlState actionState = touchActionControls.Update(actionContact, 1280.0f, 720.0f);
    Check(!actionState.torchToggled && !actionState.actionButtonActive && actionState.lookActive,
          "a hidden torch button leaves its place to the look role");
    (void)touchActionControls.Update({}, 1280.0f, 720.0f);
    touchActionControls.SetActionsEnabled(true, true);
    actionContact[0] = {62, torchButton.x, torchButton.y, island::TouchPhase::Pressed};
    actionState = touchActionControls.Update(actionContact, 1280.0f, 720.0f);
    Check(actionState.torchToggled && !actionState.rideToggled && actionState.actionButtonActive
              && !actionState.lookActive && !actionState.lookSensitivityChanged,
          "a tap on the torch button toggles the torch without turning the camera");
    actionContact[0].phase = island::TouchPhase::Moved;
    actionContact[0].x += 30.0f;
    actionState = touchActionControls.Update(actionContact, 1280.0f, 720.0f);
    Check(!actionState.torchToggled && actionState.actionButtonActive && !actionState.lookActive,
          "a finger held on an action button toggles once and never turns the camera");
    actionContact[0].phase = island::TouchPhase::Released;
    actionState = touchActionControls.Update(actionContact, 1280.0f, 720.0f);
    Check(!actionState.actionButtonActive, "lifting the finger frees the action button");
    std::array<island::TouchContact, 2> rideAndMove = {{
        {63, 200.0f, 520.0f, island::TouchPhase::Pressed},
        {64, rideButton.x, rideButton.y, island::TouchPhase::Pressed},
    }};
    actionState = touchActionControls.Update(rideAndMove, 1280.0f, 720.0f);
    Check(actionState.rideToggled && !actionState.torchToggled && actionState.movementActive
              && !actionState.lookActive,
          "the ride button answers a tap while the other thumb holds the stick");

    island::TouchControls restoredTouchControls;
    restoredTouchControls.SetLookSensitivity(
        island::TouchLookSensitivityLevel::High);
    Check(restoredTouchControls.State().lookSensitivity
              == island::TouchLookSensitivityLevel::High
              && !restoredTouchControls.State().movementActive
              && !restoredTouchControls.State().lookActive,
          "persisted touch sensitivity restores without retaining pointer ownership");

    const std::array<island::TouchContact, 1> invalidTouch = {{
        {44, std::numeric_limits<float>::quiet_NaN(), 300.0f,
         island::TouchPhase::Pressed},
    }};
    touchState = touchControls.Update(invalidTouch, 1280.0f, 720.0f);
    Check(!touchState.movementActive && !touchState.lookActive
              && std::isfinite(touchState.moveRight)
              && std::isfinite(touchState.moveForward)
              && std::isfinite(touchState.lookDeltaX)
              && std::isfinite(touchState.lookDeltaY),
          "non-finite touch coordinates cannot acquire a role or poison output");

    const std::array<island::TouchContact, 1> validLook = {{
        {45, 1000.0f, 360.0f, island::TouchPhase::Pressed},
    }};
    touchState = touchControls.Update(validLook, 1280.0f, 720.0f);
    Check(touchState.lookActive,
          "a valid look contact acquires its role after an invalid contact");
    const std::array<island::TouchContact, 1> poisonedOwnedLook = {{
        {45, std::numeric_limits<float>::infinity(), 360.0f,
         island::TouchPhase::Moved},
    }};
    touchState = touchControls.Update(poisonedOwnedLook, 1280.0f, 720.0f);
    Check(!touchState.lookActive && touchState.lookDeltaX == 0.0f
              && touchState.lookDeltaY == 0.0f,
          "an invalid owned look contact is cancelled without a camera jump");

    const std::array<island::TouchContact, 1> implausibleTouch = {{
        {46, std::numeric_limits<float>::max(), 300.0f,
         island::TouchPhase::Pressed},
    }};
    touchState = touchControls.Update(implausibleTouch, 1280.0f, 720.0f);
    Check(!touchState.movementActive && !touchState.lookActive,
          "implausibly distant finite touch coordinates are rejected");
    touchState = touchControls.Update(validLook,
                                      std::numeric_limits<float>::quiet_NaN(), 720.0f);
    Check(!touchState.movementActive && !touchState.lookActive
              && touchState.moveRight == 0.0f && touchState.moveForward == 0.0f,
          "non-finite display geometry releases all touch ownership safely");

    const island::IslandTerrain terrain;
    {
        const auto seabed=std::make_unique<island::RenderedTerrainSurface>(terrain);
        island::PlayerMotion swimmer;
        bool found=false;
        for(int angle=0;angle<32&&!found;++angle) {
            const float a=angle*6.2831853f/32;
            const float r=terrain.ShoreRadius(a)+38;
            const float x=std::cos(a)*r,z=std::sin(a)*r;
            if(std::abs(x)>330||std::abs(z)>330)continue;
            if(seabed->HeightRange(x-.34f,z-.34f,x+.34f,z+.34f)[1]<-1.3f) {
                swimmer.SetPosition(terrain,x,z); found=true;
            }
        }
        Check(found,"diving test locates actual water with room below the surface");
        if(found) {
            swimmer.SetSwimInput(seabed.get(),false,true);
            for(int i=0;i<5;++i) swimmer.Update(terrain,0,0,.1f);
            Check(swimmer.Position().y<-.2f,"player movement now reaches below the water surface");
            auto before=swimmer.Position();
            swimmer.SetSwimInput(seabed.get(),true,true);
            swimmer.Update(terrain,0,0,.1f);
            Check(std::abs(swimmer.Position().y-before.y)<1e-5f,"opposed dive input holds live player depth");
            swimmer.SetSwimInput(seabed.get(),false,false);
            int rejected=0;
            for(int i=0;i<300;++i) {
                before=swimmer.Position();
                const float r=std::hypot(before.x,before.z);
                swimmer.Update(terrain,-before.x/r,-before.z/r,.1f);
                const auto p=swimmer.Position();
                const auto support=seabed->HeightRange(std::min(before.x,p.x)-.34f,
                    std::min(before.z,p.z)-.34f,std::max(before.x,p.x)+.34f,
                    std::max(before.z,p.z)+.34f)[1];
                Check(p.y>=support+island::SwimDepth::BottomClearance-.0001f,
                    "submerged movement cannot cross rendered seabed along its swept footprint");
                rejected+=swimmer.MovementTrace().terrainRejections;
            }
            Check(rejected>0,"seabed ascent blocks underwater horizontal motion");
            swimmer.SetSwimInput(seabed.get(),true,false);
            for(int i=0;i<200;++i)swimmer.Update(terrain,0,0,.1f);
            Check(swimmer.Position().y==island::PlayerMotion::SwimmingEyeHeight,"player resurfaces");
            swimmer.SetSwimInput(seabed.get(),false,true);
            swimmer.Update(terrain,0,0,.1f);
            const auto p=swimmer.Position();
            swimmer.SetPosition(terrain,p.x,p.z);
            Check(swimmer.SubmergedDepth()==0&&swimmer.Position().y==.58f,"teleport clears immersion");
        }
    }

    const island::DistantSceneryLayout distantScenery;
    const island::DistantSceneryLayout repeatedDistantScenery;
    Check(distantScenery.Islets().size() == island::DistantSceneryLayout::IsletCount,
          "distant scenery keeps its intentionally small bounded population");
    {
        // Depth on the horizon: at least one islet within 600 m, at least one beyond 950 m, and
        // exactly one treeless sandbar.
        float nearest = 1.0e9f, farthest = 0.0f; int sandbars = 0;
        for (const auto& islet : distantScenery.Islets()) {
            nearest = std::min(nearest, std::hypot(islet.x, islet.z));
            farthest = std::max(farthest, std::hypot(islet.x, islet.z));
            sandbars += islet.variant == island::DistantSceneryLayout::SandbarVariant ? 1 : 0;
        }
        Check(nearest < 600.0f && farthest > 950.0f && sandbars == 1,
              "distant islets step from a nearer wooded islet to far silhouettes with one sandbar");
    }
    for (std::size_t index = 0; index < distantScenery.Islets().size(); ++index) {
        const island::DistantIsletPlacement& islet = distantScenery.Islets()[index];
        const island::DistantIsletPlacement& repeated = repeatedDistantScenery.Islets()[index];
        Check(islet.x == repeated.x && islet.z == repeated.z
                  && islet.radiusX == repeated.radiusX
                  && islet.radiusZ == repeated.radiusZ
                  && islet.height == repeated.height
                  && islet.yawRadians == repeated.yawRadians
                  && islet.variant == repeated.variant,
              "distant scenery placement is exactly deterministic");
        Check(std::hypot(islet.x, islet.z) >= 570.0f,
              "distant islets remain far beyond the playable island");
        Check(terrain.SignedShoreDistance(islet.x, islet.z)
                  < -(island::PlayerMotion::SafetyDistance + 150.0f),
              "distant islets remain well beyond the swimming safety boundary");
        Check(std::abs(islet.x) + islet.radiusX
                      < island::OceanMeshLayout::Extent - 20.0f
                  && std::abs(islet.z) + islet.radiusZ
                      < island::OceanMeshLayout::Extent - 20.0f,
              "distant islets remain inside the finite ocean surface");
        const bool sandbar = islet.variant == island::DistantSceneryLayout::SandbarVariant;
        Check(islet.radiusX >= 24.0f && islet.radiusX <= 42.0f
                  && islet.radiusZ >= 17.0f && islet.radiusZ <= 29.0f
                  && (sandbar ? (islet.height >= 1.5f && islet.height <= 3.0f)
                              : (islet.height >= 5.9f && islet.height <= 10.5f))
                  && islet.variant < island::DistantSceneryLayout::VariantCount,
              "distant islet dimensions and variants stay within the authored low silhouette");
        for (std::size_t other = index + 1U;
             other < distantScenery.Islets().size(); ++other) {
            const island::DistantIsletPlacement& neighbour = distantScenery.Islets()[other];
            Check(std::hypot(islet.x - neighbour.x, islet.z - neighbour.z)
                      > islet.radiusX + neighbour.radiusX + 20.0f,
                  "distant islet silhouettes remain clearly separated on the horizon");
        }
    }

    Check(island::WildlifeActivationPolicy::Classify(25.0f, 100.0f, 250.0f)
              == island::WildlifeSimulationTier::Full,
          "near wildlife receives full-rate simulation");
    Check(island::WildlifeActivationPolicy::Classify(175.0f, 100.0f, 250.0f)
              == island::WildlifeSimulationTier::Reduced,
          "mid-distance wildlife receives reduced-rate simulation");
    Check(island::WildlifeActivationPolicy::Classify(275.0f, 100.0f, 250.0f)
              == island::WildlifeSimulationTier::Dormant,
          "far wildlife receives dormant-rate simulation");
    const float quietNan = std::numeric_limits<float>::quiet_NaN();
    Check(island::WildlifeActivationPolicy::Classify(quietNan, 100.0f, 250.0f)
              == island::WildlifeSimulationTier::Dormant,
          "non-finite observer distance fails closed to dormant wildlife simulation");
    Check(island::WildlifeActivationPolicy::Classify(1.0f, quietNan, quietNan)
              == island::WildlifeSimulationTier::Dormant,
          "non-finite wildlife thresholds fail closed to a zero-distance envelope");
    Check(island::WildlifeActivationPolicy::UpdateDivisor(
              island::WildlifeSimulationTier::Full) == 1U
              && island::WildlifeActivationPolicy::UpdateDivisor(
                  island::WildlifeSimulationTier::Reduced) == 4U
              && island::WildlifeActivationPolicy::UpdateDivisor(
                  island::WildlifeSimulationTier::Dormant) == 30U,
          "wildlife tiers use the documented bounded update divisors");

    island::BirdFlock birds(terrain);
    island::BirdFlock repeatedBirds(terrain);
    island::BirdFlock invalidInputBirds(terrain);
    invalidInputBirds.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputBirds.DormantSimulationCount()
              == static_cast<int>(island::BirdFlock::BirdCount)
              && std::ranges::all_of(invalidInputBirds.Birds(), [](const auto& state) {
                     return std::isfinite(state.x) && std::isfinite(state.y)
                         && std::isfinite(state.z) && std::isfinite(state.headingRadians);
                 }),
          "bird simulation rejects non-finite frame and observer input without state poisoning");
    Check(birds.Birds().size() == island::BirdFlock::BirdCount,
          "bird flock has the intended bounded population");
    Check(birds.Routes().size() == island::BirdFlock::BirdCount,
          "bird flock exposes one authoritative route per agent");
    Check(birds.RouteRegions().size() == 3U,
          "bird flock exposes the three authored route-family regions");
    int shoreBirds = 0;
    int canopyBirds = 0;
    int highBirds = 0;
    for (std::size_t index = 0; index < birds.Birds().size(); ++index) {
        const island::BirdState& bird = birds.Birds()[index];
        const island::BirdState& repeated = repeatedBirds.Birds()[index];
        const island::BirdRouteState& route = birds.Routes()[index];
        const island::BirdRouteState& repeatedRoute = repeatedBirds.Routes()[index];
        Check(bird.x == repeated.x && bird.y == repeated.y && bird.z == repeated.z
                  && bird.headingRadians == repeated.headingRadians
                  && bird.speed == repeated.speed && bird.scale == repeated.scale
                  && bird.bankRadians == repeated.bankRadians
                  && bird.glideAmount == repeated.glideAmount
                  && bird.family == repeated.family
                  && bird.species == repeated.species
                  && bird.colorVariant == repeated.colorVariant,
              "bird initialization is deterministic");
        Check(route.spawnX == repeatedRoute.spawnX
                  && route.spawnY == repeatedRoute.spawnY
                  && route.spawnZ == repeatedRoute.spawnZ
                  && route.spawnHeadingRadians == repeatedRoute.spawnHeadingRadians
                  && route.preferredRadius == repeatedRoute.preferredRadius
                  && route.baseAltitude == repeatedRoute.baseAltitude
                  && route.minimumClearance == repeatedRoute.minimumClearance
                  && route.verticalAmplitude == repeatedRoute.verticalAmplitude
                  && route.direction == repeatedRoute.direction
                  && route.family == repeatedRoute.family,
              "bird route diagnostics initialize deterministically");
        Check(route.spawnX == bird.x && route.spawnY == bird.y && route.spawnZ == bird.z
                  && route.spawnHeadingRadians == bird.headingRadians,
              "bird route diagnostics retain the exact authored spawn pose");
        Check(route.family == bird.family
                  && route.preferredRadius >= 70.0f && route.preferredRadius <= 248.0f
                  && route.baseAltitude >= 28.0f && route.baseAltitude <= 95.0f
                  && route.minimumClearance >= 15.0f
                  && route.verticalAmplitude >= 2.0f
                  && (route.direction == -1 || route.direction == 1),
              "bird route diagnostics retain bounded steering inputs");
        Check(std::isfinite(bird.x) && std::isfinite(bird.y) && std::isfinite(bird.z),
              "bird initial positions are finite");
        Check(bird.y >= terrain.Height(bird.x, bird.z) + 14.9f,
              "bird initial positions clear the terrain and canopy");
        Check(bird.scale >= 0.7f && bird.scale <= 1.2f,
              "bird scale variation stays within the authored range");
        switch (bird.family) {
        case island::BirdPathFamily::Shore:
            ++shoreBirds;
            break;
        case island::BirdPathFamily::Canopy:
            ++canopyBirds;
            break;
        case island::BirdPathFamily::High:
            ++highBirds;
            break;
        }
    }
    Check(shoreBirds == 10 && canopyBirds == 8 && highBirds == 6,
          "bird flock combines shore, canopy, and high route families");
    Check(std::ranges::count_if(birds.Birds(), [](const auto& bird) {
              return bird.species == island::BirdSpecies::Parrot;
          }) == 4
              && std::ranges::count_if(birds.Birds(), [](const auto& bird) {
                     return bird.species == island::BirdSpecies::FruitDove;
                 }) == 4,
          "canopy flock retains four parrots and four tropical fruit doves");
    const std::array<std::size_t, 3> expectedBirdRegionPopulations = {10U, 8U, 6U};
    for (std::size_t index = 0; index < birds.RouteRegions().size(); ++index) {
        const island::BirdRouteRegion& region = birds.RouteRegions()[index];
        const island::BirdRouteRegion& repeated = repeatedBirds.RouteRegions()[index];
        Check(region.minimumRadius == repeated.minimumRadius
                  && region.maximumRadius == repeated.maximumRadius
                  && region.minimumAltitude == repeated.minimumAltitude
                  && region.maximumAltitude == repeated.maximumAltitude
                  && region.population == repeated.population
                  && region.family == repeated.family,
              "bird route-family regions initialize deterministically");
        Check(region.population == expectedBirdRegionPopulations[index]
                  && region.minimumRadius <= region.maximumRadius
                  && region.minimumAltitude < region.maximumAltitude,
              "bird route-family regions account for bounded authored volumes");
    }
    CheckNear(birds.RouteRegions()[0].minimumRadius, 218.0f, 0.0001f,
              "shore-bird route region begins at the authored inner orbit");
    CheckNear(birds.RouteRegions()[0].maximumRadius, 248.0f, 0.0001f,
              "shore-bird route region ends at the authored outer orbit");
    CheckNear(birds.RouteRegions()[1].minimumRadius, 70.0f, 0.0001f,
              "canopy-bird route region begins at the authored inner orbit");
    CheckNear(birds.RouteRegions()[1].maximumRadius, 150.0f, 0.0001f,
              "canopy-bird route region ends at the authored outer orbit");
    CheckNear(birds.RouteRegions()[2].minimumRadius, 125.0f, 0.0001f,
              "high-bird route region begins at the authored inner orbit");
    CheckNear(birds.RouteRegions()[2].maximumRadius, 233.0f, 0.0001f,
              "high-bird route region ends at the authored outer orbit");

    const std::array<island::BirdPerchSite, 8> testPerches = {{
        {80.0f, terrain.Height(80.0f, 0.0f) + 10.0f, 0.0f, 0.2f,
         island::BirdPerchKind::Branch},
        {0.0f, terrain.Height(0.0f, 80.0f) + 10.0f, 80.0f, 1.7f,
         island::BirdPerchKind::Branch},
        {-80.0f, terrain.Height(-80.0f, 0.0f) + 10.0f, 0.0f, -2.8f,
         island::BirdPerchKind::Branch},
        {0.0f, terrain.Height(0.0f, -80.0f) + 10.0f, -80.0f, -1.4f,
         island::BirdPerchKind::Branch},
        {57.0f, terrain.Height(57.0f, 57.0f) + 10.0f, 57.0f, 0.8f,
         island::BirdPerchKind::Branch},
        {205.0f, terrain.Height(205.0f, 0.0f) + 2.0f, 0.0f, 1.1f,
         island::BirdPerchKind::Rock},
        {-205.0f, terrain.Height(-205.0f, 0.0f) + 2.0f, 0.0f, -1.9f,
         island::BirdPerchKind::Rock},
        {0.0f, terrain.Height(0.0f, 205.0f) + 5.0f, 205.0f, -0.7f,
         island::BirdPerchKind::Wreck},
    }};
    birds.SetPerchSites(testPerches);
    repeatedBirds.SetPerchSites(testPerches);

    float maximumObservedBirdRadius = 0.0f;
    float minimumObservedBirdRadius = 1000.0f;
    bool observedWingUp = false;
    bool observedWingDown = false;
    bool observedStrongGlide = false;
    bool observedLeftBank = false;
    bool observedRightBank = false;
    bool observedBirdApproach = false;
    bool observedPerchedBird = false;
    bool observedBirdDeparture = false;
    bool observedFoldedPerchWings = false;
    bool observedGroundDoveApproach = false;
    bool observedGroundDoveForage = false;
    bool observedGroundDoveDeparture = false;
    {
        // At night every bird that perches goes to a perch of its own and stays until dawn.
        island::BirdFlock roosting(terrain);
        roosting.SetPerchSites(testPerches);
        roosting.SetNightness(1.0f);
        for (int frame = 0; frame < 30 * 240; ++frame) roosting.Update(1.0f / 30.0f, 0.0f, 15.0f, 0.0f);
        std::vector<std::size_t> perchedEarly;
        std::vector<int> perSite(testPerches.size(), 0);
        for (std::size_t i = 0; i < roosting.Birds().size(); ++i) {
            const auto& bird = roosting.Birds()[i];
            if (bird.behavior != island::BirdBehavior::Perched) continue;
            perchedEarly.push_back(i);
            if (bird.perchSiteIndex < perSite.size()) ++perSite[bird.perchSiteIndex];
        }
        for (int frame = 0; frame < 30 * 120; ++frame) roosting.Update(1.0f / 30.0f, 0.0f, 15.0f, 0.0f);
        bool stayed = true;
        for (const std::size_t i : perchedEarly) stayed = stayed && roosting.Birds()[i].behavior == island::BirdBehavior::Perched;
        const bool alone = std::all_of(perSite.begin(), perSite.end(), [](int n) { return n <= 1; });
        Check(perchedEarly.size() >= 4U && stayed && alone, "at night the birds roost a perch apiece and stay until dawn");
    }
    constexpr int birdSimulationFrames = 36000;
    for (int frame = 0; frame < birdSimulationFrames; ++frame) {
        birds.Update(1.0f / 30.0f, 0.0f, 15.0f, 0.0f);
        repeatedBirds.Update(1.0f / 30.0f, 0.0f, 15.0f, 0.0f);
        const auto birdStates = birds.Birds();
        const auto repeatedStates = repeatedBirds.Birds();
        for (std::size_t index = 0; index < birdStates.size(); ++index) {
            const island::BirdState& bird = birdStates[index];
            const island::BirdState& repeated = repeatedStates[index];
            Check(bird.x == repeated.x && bird.y == repeated.y && bird.z == repeated.z
                      && bird.headingRadians == repeated.headingRadians
                      && bird.wingPose == repeated.wingPose
                      && bird.bankRadians == repeated.bankRadians
                      && bird.glideAmount == repeated.glideAmount
                      && bird.perchAmount == repeated.perchAmount
                      && bird.behavior == repeated.behavior
                      && bird.perchSiteIndex == repeated.perchSiteIndex
                      && bird.simulationTier == repeated.simulationTier,
                  "bird simulation remains deterministic over a long session");
            const float radius = std::hypot(bird.x, bird.z);
            maximumObservedBirdRadius = std::max(maximumObservedBirdRadius, radius);
            minimumObservedBirdRadius = std::min(minimumObservedBirdRadius, radius);
            Check(std::isfinite(bird.headingRadians) && std::isfinite(bird.wingPose)
                      && std::isfinite(bird.bankRadians) && std::isfinite(bird.glideAmount),
                  "bird flight pose remains finite over a long session");
            Check(std::fabs(bird.bankRadians) <= 0.341f
                      && bird.glideAmount >= 0.0f && bird.glideAmount <= 1.0f,
                  "bird banking and glide blend remain inside portable pose bounds");
            if (bird.behavior == island::BirdBehavior::Fly) {
                Check(bird.y >= terrain.Height(bird.x, bird.z) + 14.8f,
                      "free bird flight maintains its minimum terrain clearance");
            } else if (bird.behavior == island::BirdBehavior::GroundApproach
                       || bird.behavior == island::BirdBehavior::GroundForage
                       || bird.behavior == island::BirdBehavior::GroundDepart) {
                Check(bird.species == island::BirdSpecies::FruitDove,
                      "only fruit doves enter the dedicated ground-foraging states");
                Check(bird.y >= terrain.Height(bird.x, bird.z)
                          + bird.scale * 0.195f,
                      "ground dove keeps its feet on or above authoritative terrain");
            } else {
                Check(bird.y >= terrain.Height(bird.x, bird.z) + 0.34f,
                      "landing bird remains clear of terrain");
            }
            if (bird.behavior == island::BirdBehavior::Approach) {
                observedBirdApproach = true;
            } else if (bird.behavior == island::BirdBehavior::Perched) {
                observedPerchedBird = true;
                const auto& site = testPerches[bird.perchSiteIndex];
                CheckNear(bird.x, site.x, 0.0001f,
                          "perched bird remains on the selected support x coordinate");
                CheckNear(bird.z, site.z, 0.0001f,
                          "perched bird remains on the selected support z coordinate");
                CheckNear(bird.y, site.y + bird.scale * 0.12f, 0.0001f,
                          "perched bird body places its feet on the support surface");
                observedFoldedPerchWings = observedFoldedPerchWings
                    || (bird.perchAmount > 0.999f && std::abs(bird.wingPose) < 0.04f);
            } else if (bird.behavior == island::BirdBehavior::Depart) {
                observedBirdDeparture = true;
            } else if (bird.behavior == island::BirdBehavior::GroundApproach) {
                observedGroundDoveApproach = true;
            } else if (bird.behavior == island::BirdBehavior::GroundForage) {
                observedGroundDoveForage = true;
                Check(bird.perchAmount > 0.999f && std::abs(bird.wingPose) < 0.03f,
                      "foraging ground dove walks with folded wings");
            } else if (bird.behavior == island::BirdBehavior::GroundDepart) {
                observedGroundDoveDeparture = true;
            }
            observedWingUp = observedWingUp || bird.wingPose > 0.55f;
            observedWingDown = observedWingDown || bird.wingPose < -0.55f;
            observedStrongGlide = observedStrongGlide || bird.glideAmount > 0.92f;
            observedLeftBank = observedLeftBank || bird.bankRadians < -0.055f;
            observedRightBank = observedRightBank || bird.bankRadians > 0.055f;
        }
        Check(birds.ApproachingCount() + birds.PerchedCount() + birds.DepartingCount() <= 2,
              "bird landing limits the flock to two simultaneous non-flight agents");
    }
    Check(minimumObservedBirdRadius > 35.0f && maximumObservedBirdRadius < 315.0f,
          "all bird route families remain in the authored island airspace");
    Check(observedWingUp && observedWingDown,
          "bird animation visits both wing stroke extremes");
    Check(observedStrongGlide,
          "bird animation enters sustained intermittent glide periods");
    Check(observedLeftBank && observedRightBank,
          "opposed route directions produce visible banking in both directions");
    Check(observedBirdApproach && observedPerchedBird && observedBirdDeparture
              && observedFoldedPerchWings,
          "bird simulation completes approach, contacted perch and departure states");
    Check(observedGroundDoveApproach && observedGroundDoveForage
              && observedGroundDoveDeparture,
          "fruit doves complete approach, ground forage and departure states");
    Check(birds.FullSimulationCount() + birds.ReducedSimulationCount()
              + birds.DormantSimulationCount() == static_cast<int>(island::BirdFlock::BirdCount),
          "bird simulation tiers account for the complete flock");

    island::ButterflyMeadow butterflies(terrain);
    island::ButterflyMeadow repeatedButterflies(terrain);
    island::ButterflyMeadow invalidInputButterflies(terrain);
    Check(butterflies.Butterflies().size() == island::ButterflyMeadow::ButterflyCount
              && butterflies.Homes().size() == island::ButterflyMeadow::ButterflyCount
              && butterflies.FlowerPatches().size()
                  == island::ButterflyMeadow::HabitatCount,
          "butterfly simulation exposes the intended bounded population and habitats");
    {
        // A crocodile hauled out and crawling drags a broad groove a quarter metre at a time; afloat
        // or on ground that takes no mark it drags none.
        island::CrocodileDragTrail drags;
        std::array<island::CrocodileState, island::CrocodileGroup::CrocodileCount> crocodiles{};
        std::array<bool, island::CrocodileGroup::CrocodileCount> dry{true, false, true};
        for (auto& crocodile : crocodiles) { crocodile.speed = 0.3f; crocodile.scale = 1.2f; }
        crocodiles[2].submerged = 0.8f;
        for (int step = 0; step < 100; ++step) {
            for (auto& crocodile : crocodiles) crocodile.x = 0.03f * static_cast<float>(step);
            drags.Update(0.1f, crocodiles, dry);
        }
        std::size_t wide = 0;
        for (const auto& segment : drags.Segments())
            if (segment.active && std::abs(segment.halfWidth - island::CrocodileDragTrail::HalfWidth * 1.2f) < 1e-5f) ++wide;
        Check(drags.LaidCount() >= 10U && drags.LaidCount() <= 13U && wide == drags.LaidCount(),
              "a crawling crocodile drags a broad groove on sand, none afloat or on unmarked ground");
    }
    {
        // In the rain every dragonfly ends its flight on a perch and holds it; after it they fly.
        island::DragonflySwarm wetDragonflies(terrain);
        for (int step = 0; step < 100; ++step) wetDragonflies.Update(0.05f, 500.0f, 2.0f, 500.0f);
        wetDragonflies.SetRain(0.55f);
        for (int step = 0; step < 80; ++step) wetDragonflies.Update(0.05f, 500.0f, 2.0f, 500.0f);
        const std::size_t perchedInRain = wetDragonflies.PerchedCount();
        for (int step = 0; step < 200; ++step) wetDragonflies.Update(0.05f, 500.0f, 2.0f, 500.0f);
        const std::size_t stillPerched = wetDragonflies.PerchedCount();
        wetDragonflies.SetRain(0.0f);
        for (int step = 0; step < 300; ++step) wetDragonflies.Update(0.05f, 500.0f, 2.0f, 500.0f);
        Check(perchedInRain == island::DragonflySwarm::Count && stillPerched == island::DragonflySwarm::Count
                  && wetDragonflies.PerchedCount() < island::DragonflySwarm::Count,
              "in the rain every dragonfly perches and stays perched; after it they fly again");
    }
    {
        // In the rain the butterflies settle and sit it out with their wings closed; when it
        // stops they fly again.
        island::ButterflyMeadow wet(terrain);
        for (int step = 0; step < 200; ++step) wet.Update(0.05f, 500.0f, 2.0f, 500.0f);
        wet.SetRain(0.55f);
        for (int step = 0; step < 120; ++step) wet.Update(0.05f, 500.0f, 2.0f, 500.0f);
        bool closed = true;
        for (const auto& fly : wet.Butterflies()) closed = closed && fly.wingFold > 0.5f;   // (1: folded shut)
        Check(wet.RestingCount() == wet.Butterflies().size() && closed,
              "in the rain every butterfly settles within six seconds, wings closed");
        wet.SetRain(0.0f);
        for (int step = 0; step < 300; ++step) wet.Update(0.05f, 500.0f, 2.0f, 500.0f);
        Check(wet.RestingCount() < wet.Butterflies().size(), "after the rain the butterflies fly again");
    }
    {
        // Basking: with the cumulus clock set to a sunny moment over the first habitat, a
        // resting butterfly there opens its wings flat within a few seconds; set to a moment
        // under a cloud's shadow, it closes them again.
        {
            // The proboscis: within a minute a butterfly of the first habitat, watched from
            // eight metres, settles into a nectar hover and uncurls its proboscis fully; no
            // butterfly resting or fluttering along has it out.
            island::ButterflyMeadow feeders(terrain);
            const auto& feedPatch = feeders.FlowerPatches()[0];
            bool probed = false, wrong = false;
            for (int step = 0; step < 1200; ++step) {
                feeders.Update(0.05f, feedPatch.x + 8.0f, feedPatch.y + 1.6f, feedPatch.z);
                for (std::size_t i = 0; i < 6U && i < feeders.Butterflies().size(); ++i) {
                    const auto& fly = feeders.Butterflies()[i];
                    if (fly.behavior == island::ButterflyBehavior::NectarHover && fly.proboscis > 0.95f) probed = true;
                    if (fly.behavior != island::ButterflyBehavior::NectarHover && fly.speed > 0.3f && fly.proboscis > 0.5f) wrong = true;
                }
            }
            Check(probed && feeders.ProbeCount() >= 1U && !wrong, "a butterfly settled at its flower uncurls its proboscis to feed and rolls it up to leave");
        }
        island::ButterflyMeadow basking(terrain);
        const auto& patch = basking.FlowerPatches()[0];
        float sunny = -1.0f, shaded = -1.0f;
        for (float t = 0.0f; t < 1200.0f && (sunny < 0.0f || shaded < 0.0f); t += 0.5f) {
            const float cover = island::CloudShadow::Cover(patch.x, patch.z, t);
            if (sunny < 0.0f && cover <= 0.05f) sunny = t;
            if (shaded < 0.0f && cover >= 0.8f) shaded = t;
        }
        Check(sunny >= 0.0f && shaded >= 0.0f, "the cumulus clock has a sunny and a shaded moment over the first butterfly habitat");
        const auto resting = [&](std::size_t habitat) {
            for (std::size_t i = habitat * 6U; i < habitat * 6U + 6U && i < basking.Butterflies().size(); ++i)
                if (basking.Butterflies()[i].behavior == island::ButterflyBehavior::Rest) return static_cast<int>(i);
            return -1;
        };
        basking.SetCloudClock(sunny);
        bool opened = false;
        for (int step = 0; step < 600 && !opened; ++step) {
            basking.Update(0.05f, patch.x + 8.0f, patch.y + 1.6f, patch.z);
            const int index = resting(0);
            if (index >= 0 && basking.Butterflies()[static_cast<std::size_t>(index)].wingFold < 0.3f) opened = true;
        }
        Check(opened && basking.BaskingCount() >= 1U, "a butterfly resting in the sun opens its wings to bask");
        basking.SetCloudClock(shaded);
        bool closed = false;
        for (int step = 0; step < 600 && !closed; ++step) {
            basking.Update(0.05f, patch.x + 8.0f, patch.y + 1.6f, patch.z);
            const int index = resting(0);
            if (index >= 0 && basking.Butterflies()[static_cast<std::size_t>(index)].wingFold > 0.7f) closed = true;
        }
        Check(closed, "under a cloud's shadow a resting butterfly closes its wings");
    }
    for (std::size_t patchIndex = 0;
         patchIndex < butterflies.FlowerPatches().size(); ++patchIndex) {
        const auto& patch = butterflies.FlowerPatches()[patchIndex];
        const auto& repeatedPatch = repeatedButterflies.FlowerPatches()[patchIndex];
        const auto sample = terrain.Sample(patch.x, patch.z);
        Check(patch.x == repeatedPatch.x && patch.y == repeatedPatch.y
                  && patch.z == repeatedPatch.z && patch.radius == repeatedPatch.radius
                  && patch.groundSlopeX == repeatedPatch.groundSlopeX
                  && patch.groundSlopeZ == repeatedPatch.groundSlopeZ,
              "butterfly nectar patches derive deterministically");
        Check(std::isfinite(patch.x) && std::isfinite(patch.y)
                  && std::isfinite(patch.z) && patch.radius >= 1.8f
                  && patch.radius <= 2.25f && std::isfinite(patch.groundSlopeX)
                  && std::isfinite(patch.groundSlopeZ)
                  && std::fabs(patch.y - sample.height) < 0.001f,
              "visible nectar flowers are grounded inside each clearing habitat");
        for (std::size_t member = 0; member < 6U; ++member) {
            const auto& home = butterflies.Homes()[patchIndex * 6U + member];
            Check(home.x == patch.x && home.z == patch.z,
                  "each butterfly home is anchored to its concrete nectar patch");
        }
    }
    for (std::size_t index = 0; index < butterflies.Butterflies().size(); ++index) {
        const island::ButterflyState& butterfly = butterflies.Butterflies()[index];
        const island::ButterflyState& repeated = repeatedButterflies.Butterflies()[index];
        const island::ButterflyHomeRange& home = butterflies.Homes()[index];
        const island::TerrainSample sample = terrain.Sample(butterfly.x, butterfly.z);
        Check(butterfly.x == repeated.x && butterfly.y == repeated.y
                  && butterfly.z == repeated.z
                  && butterfly.headingRadians == repeated.headingRadians
                  && butterfly.behavior == repeated.behavior
                  && butterfly.colorVariant == repeated.colorVariant,
              "butterfly construction is deterministic");
        Check(std::hypot(butterfly.x - home.x, butterfly.z - home.z) <= home.radius
                  && home.radius >= 7.5f && home.radius <= 11.5f,
              "each butterfly begins inside one bounded clearing habitat");
        Check(sample.signedShoreDistance
                      >= island::ButterflyMeadow::MinimumInlandDistance
                  && sample.slopeRadians < 0.48f
                  && (sample.surface == island::TerrainSurface::CoastalGround
                      || sample.surface == island::TerrainSurface::InteriorGround),
              "each butterfly begins above suitable non-beach vegetation ground");
        Check(butterfly.y >= sample.height + 0.14f
                  && butterfly.y <= sample.height + 1.8f,
              "each butterfly begins inside the low-altitude presentation envelope");
    }
    invalidInputButterflies.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputButterflies.DormantSimulationCount()
              == static_cast<int>(island::ButterflyMeadow::ButterflyCount)
              && std::ranges::all_of(
                  invalidInputButterflies.Butterflies(), [](const auto& butterfly) {
                      return std::isfinite(butterfly.x) && std::isfinite(butterfly.y)
                          && std::isfinite(butterfly.z)
                          && std::isfinite(butterfly.headingRadians);
                  }),
          "butterflies reject non-finite frame and observer input without state poisoning");
    bool observedButterflyRest = false;
    bool observedButterflyFlutter = false;
    bool observedButterflyHover = false;
    for (int frame = 0; frame < 12000; ++frame) {
        butterflies.Update(1.0f / 30.0f, 0.0f, 100.0f, 0.0f);
        repeatedButterflies.Update(1.0f / 30.0f, 0.0f, 100.0f, 0.0f);
        for (std::size_t index = 0; index < butterflies.Butterflies().size(); ++index) {
            const island::ButterflyState& butterfly = butterflies.Butterflies()[index];
            const island::ButterflyState& repeated =
                repeatedButterflies.Butterflies()[index];
            const island::ButterflyHomeRange& home = butterflies.Homes()[index];
            const island::TerrainSample sample = terrain.Sample(butterfly.x, butterfly.z);
            Check(butterfly.x == repeated.x && butterfly.y == repeated.y
                      && butterfly.z == repeated.z
                      && butterfly.headingRadians == repeated.headingRadians
                      && butterfly.wingFold == repeated.wingFold
                      && butterfly.behavior == repeated.behavior,
                  "butterfly simulation remains deterministic over a long session");
            Check(std::hypot(butterfly.x - home.x, butterfly.z - home.z)
                          <= home.radius + 0.01f
                      && sample.signedShoreDistance
                          >= island::ButterflyMeadow::MinimumInlandDistance - 0.01f
                      && sample.slopeRadians < 0.481f
                      && butterfly.y >= sample.height + 0.13f
                      && butterfly.y <= sample.height + 1.81f,
                  "butterflies remain in bounded low-altitude clearing habitats");
            Check(std::isfinite(butterfly.speed) && butterfly.speed >= 0.0f
                      && std::isfinite(butterfly.wingFold)
                      && butterfly.wingFold >= 0.0f && butterfly.wingFold <= 1.0f,
                  "butterfly movement and wing pose remain finite and bounded");
            observedButterflyRest = observedButterflyRest
                || butterfly.behavior == island::ButterflyBehavior::Rest;
            observedButterflyFlutter = observedButterflyFlutter
                || butterfly.behavior == island::ButterflyBehavior::Flutter;
            observedButterflyHover = observedButterflyHover
                || butterfly.behavior == island::ButterflyBehavior::NectarHover;
        }
    }
    Check(observedButterflyRest && observedButterflyFlutter && observedButterflyHover,
          "butterfly habitats reach rest, flutter, and nectar-hover activity states");
    Check(butterflies.FullSimulationCount() + butterflies.ReducedSimulationCount()
              + butterflies.DormantSimulationCount()
              == static_cast<int>(island::ButterflyMeadow::ButterflyCount),
          "butterfly simulation tiers account for the complete population");
    island::ButterflyMeadow reactiveButterflies(terrain);
    const island::ButterflyState initialReactiveButterfly =
        reactiveButterflies.Butterflies().front();
    for (int frame = 0; frame < 120; ++frame) {
        reactiveButterflies.Update(
            1.0f / 60.0f, initialReactiveButterfly.x, initialReactiveButterfly.y,
            initialReactiveButterfly.z);
    }
    const island::ButterflyState& escapedButterfly =
        reactiveButterflies.Butterflies().front();
    Check(escapedButterfly.behavior == island::ButterflyBehavior::Flutter
              && std::hypot(escapedButterfly.x - initialReactiveButterfly.x,
                            escapedButterfly.z - initialReactiveButterfly.z) > 0.25f
              && escapedButterfly.y > terrain.Height(
                     escapedButterfly.x, escapedButterfly.z) + 0.35f,
          "a nearby observer prompts a mild airborne butterfly retreat");

    island::GeckoPopulation geckos(terrain);
    island::GeckoPopulation repeatedGeckos(terrain);
    island::GeckoPopulation invalidInputGeckos(terrain);
    Check(geckos.Geckos().size() == island::GeckoPopulation::Count
              && geckos.Homes().size() == island::GeckoPopulation::Count,
          "gecko simulation exposes eight bounded inland animals");
    for (std::size_t index=0;index<geckos.Geckos().size();++index) {
        const auto& gecko=geckos.Geckos()[index];
        const auto& repeated=repeatedGeckos.Geckos()[index];
        const auto& home=geckos.Homes()[index];
        const auto sample=terrain.Sample(gecko.x,gecko.z);
        Check(gecko.x==repeated.x && gecko.y==repeated.y && gecko.z==repeated.z
                  && gecko.headingRadians==repeated.headingRadians
                  && gecko.behavior==repeated.behavior
                  && gecko.colorVariant==repeated.colorVariant,
              "gecko construction is deterministic");
        Check(std::hypot(gecko.x-home.x,gecko.z-home.z)<=home.radius
                  && home.radius>=5.2f && home.radius<=6.2f,
              "each gecko begins inside its compact home range");
        Check(sample.signedShoreDistance>=34.0f && sample.slopeRadians<.52f
                  && (sample.surface==island::TerrainSurface::CoastalGround
                      || sample.surface==island::TerrainSurface::InteriorGround
                      || sample.surface==island::TerrainSurface::RockyGround)
                  && std::fabs(gecko.y-sample.height-.038f)<.001f,
              "each gecko begins grounded in suitable inland habitat");
    }
    invalidInputGeckos.Update(quietNan,quietNan,quietNan,quietNan);
    Check(invalidInputGeckos.DormantSimulationCount()
                  ==static_cast<int>(island::GeckoPopulation::Count)
              && std::ranges::all_of(invalidInputGeckos.Geckos(),[](const auto& gecko) {
                     return std::isfinite(gecko.x)&&std::isfinite(gecko.y)
                         &&std::isfinite(gecko.z)&&std::isfinite(gecko.headingRadians)
                         &&std::isfinite(gecko.visibility);
                 }),
          "geckos reject non-finite frame and observer input without state poisoning");
    bool observedGeckoRest=false;
    bool observedGeckoScurry=false;
    bool observedGeckoHidden=false;
    bool observedInvisibleGecko=false;
    for(int frame=0;frame<4000;++frame) {
        geckos.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        repeatedGeckos.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        for(std::size_t index=0;index<geckos.Geckos().size();++index) {
            const auto& gecko=geckos.Geckos()[index];
            const auto& repeated=repeatedGeckos.Geckos()[index];
            const auto& home=geckos.Homes()[index];
            const auto sample=terrain.Sample(gecko.x,gecko.z);
            Check(gecko.x==repeated.x && gecko.y==repeated.y && gecko.z==repeated.z
                      && gecko.headingRadians==repeated.headingRadians
                      && gecko.visibility==repeated.visibility
                      && gecko.behavior==repeated.behavior,
                  "gecko simulation remains deterministic over a long session");
            Check(std::hypot(gecko.x-home.x,gecko.z-home.z)<=home.radius+.01f
                      && sample.signedShoreDistance>=33.99f
                      && sample.slopeRadians<.521f
                      && std::fabs(gecko.y-sample.height-.038f)<.002f,
                  "geckos remain grounded in bounded inland habitats");
            Check(std::isfinite(gecko.speed)&&gecko.speed>=0.0f
                      && gecko.visibility>=0.0f&&gecko.visibility<=1.0f,
                  "gecko motion and visibility remain finite and bounded");
            observedGeckoRest|=gecko.behavior==island::GeckoBehavior::Rest;
            observedGeckoScurry|=gecko.behavior==island::GeckoBehavior::Scurry;
            observedGeckoHidden|=gecko.behavior==island::GeckoBehavior::Hidden;
            observedInvisibleGecko|=gecko.visibility<.04f;
        }
    }
    Check(observedGeckoRest&&observedGeckoScurry&&observedGeckoHidden
              &&observedInvisibleGecko,
          "geckos complete visible rest, scurry, and hidden activity states");
    Check(geckos.FullSimulationCount()+geckos.ReducedSimulationCount()
                  +geckos.DormantSimulationCount()
              ==static_cast<int>(island::GeckoPopulation::Count),
          "gecko simulation tiers account for the complete population");
    island::GeckoPopulation reactiveGeckos(terrain);
    const auto initialReactiveGecko=reactiveGeckos.Geckos()[1U];
    for(int frame=0;frame<75;++frame) {
        reactiveGeckos.Update(1.0f/60.0f,initialReactiveGecko.x,
                             initialReactiveGecko.y,initialReactiveGecko.z);
    }
    const auto& hiddenGecko=reactiveGeckos.Geckos()[1U];
    Check(hiddenGecko.behavior==island::GeckoBehavior::Hidden
              && hiddenGecko.visibility<.3f
              && std::hypot(hiddenGecko.x-initialReactiveGecko.x,
                            hiddenGecko.z-initialReactiveGecko.z)>.2f,
          "a nearby observer prompts a gecko to scurry away and hide");

    island::SkinkPopulation skinks(terrain);
    island::SkinkPopulation repeatedSkinks(terrain);
    island::SkinkPopulation invalidInputSkinks(terrain);
    Check(skinks.Skinks().size()==island::SkinkPopulation::Count
              &&skinks.Homes().size()==island::SkinkPopulation::Count,
          "skink simulation exposes eight bounded open-ground animals");
    {
        // The push-up display: over a sunny minute a basking skink lifts its front body in
        // push-up bouts, each push-up to the full lift, and no skink up and foraging ever does.
        island::SkinkPopulation displayers(terrain);
        const auto first=displayers.Skinks()[1];
        float sunny=-1.0f;
        for(float t=0.0f;t<1200.0f&&sunny<0.0f;t+=0.5f) if(island::CloudShadow::Cover(first.x,first.z,t)<=0.05f) sunny=t;
        Check(sunny>=0.0f,"the cumulus clock has a sunny moment over a skink's ground");
        displayers.SetCloudClock(sunny);
        float peak=0.0f; bool movingLift=false; int lifts=0; std::array<bool,island::SkinkPopulation::Count> was{};
        for(int step=0;step<1200;++step) {
            displayers.Update(0.05f,first.x+10.0f,first.y+1.6f,first.z);
            for(std::size_t i=0;i<displayers.Skinks().size();++i) {
                const auto& skink=displayers.Skinks()[i];
                peak=std::max(peak,skink.pushUp);
                const bool up=skink.pushUp>0.6f;
                if(up&&!was[i]) ++lifts;
                was[i]=up;
                if(skink.pushUp>0.5f&&skink.behavior!=island::SkinkBehavior::Bask) movingLift=true;
            }
        }
        Check(displayers.DisplayCount()>=1U&&lifts>=3&&peak>0.9f&&!movingLift,"a basking skink in the sun does bouts of push-ups and a foraging skink never does");
        {
            // The tongue: among the fully simulated skinks watched from ten metres, within
            // twenty seconds a foraging skink flicks its tongue out fully at least twice,
            // each flick in again within half a second; a basking skink holds it in.
            island::SkinkPopulation flickers(terrain);
            const auto& flickHome=flickers.Homes()[0];
            float tonguePeak=0.0f; bool baskFlick=false; int longest=0,run=0;
            for(int step=0;step<400;++step) {
                flickers.Update(0.05f,flickHome.x+10.0f,terrain.Height(flickHome.x,flickHome.z)+1.0f,flickHome.z);
                bool anyOut=false;
                for(const auto& skink:flickers.Skinks()) {
                    if(skink.simulationTier!=island::WildlifeSimulationTier::Full) continue;
                    tonguePeak=std::max(tonguePeak,skink.tongue);
                    if(skink.tongue>0.9f) anyOut=true;
                    if(skink.behavior==island::SkinkBehavior::Bask&&skink.tongue>0.5f) baskFlick=true;
                }
                run=anyOut?run+1:0; longest=std::max(longest,run);
            }
            Check(flickers.FlickCount()>=2U&&tonguePeak>0.9f&&longest<=10&&!baskFlick,"a foraging skink flicks its tongue at the ground ahead and a basking one holds it in");
            // The flatten: with the cumulus clock at a sunny moment over the first home, a
            // basking skink flattens to full within half a minute and rounds again as it
            // displays or gets up; a foraging skink stays round.
            {
                float sunny=-1.0f;
                for(float t=0.0f;t<1200.0f&&sunny<0.0f;t+=0.5f) if(island::CloudShadow::Cover(flickHome.x,flickHome.z,t)<=0.05f) sunny=t;
                Check(sunny>=0.0f,"the cumulus clock has a sunny moment over the first skink home");
                flickers.SetCloudClock(sunny);
                float flatPeak=0.0f,foragingFlat=0.0f; bool rounded=false;
                for(int step=0;step<1200;++step) {
                    flickers.Update(0.05f,flickHome.x+10.0f,terrain.Height(flickHome.x,flickHome.z)+1.0f,flickHome.z);
                    for(const auto& skink:flickers.Skinks()) {
                        if(skink.simulationTier!=island::WildlifeSimulationTier::Full) continue;
                        if(skink.behavior==island::SkinkBehavior::Bask) flatPeak=std::max(flatPeak,skink.flatten);
                        if(skink.behavior==island::SkinkBehavior::Forage&&skink.speed>0.3f) foragingFlat=std::max(foragingFlat,skink.flatten);
                        if(flatPeak>0.9f&&skink.flatten<0.1f&&(skink.pushUp>0.5f||skink.behavior!=island::SkinkBehavior::Bask)) rounded=true;
                    }
                }
                // (A skink just up from its bask may still be rounding as it sets off, so a
                // forager's flatten is only held under nine tenths.)
                Check(flatPeak>0.9f&&rounded&&foragingFlat<0.9f,"a skink basking in the sun does not flatten to it and round again, or a foraging one is flat");
            }
        }
    }
    for(std::size_t index=0;index<skinks.Skinks().size();++index) {
        const auto& skink=skinks.Skinks()[index];
        const auto& repeated=repeatedSkinks.Skinks()[index];
        const auto& home=skinks.Homes()[index];
        const auto sample=terrain.Sample(skink.x,skink.z);
        Check(skink.x==repeated.x&&skink.y==repeated.y&&skink.z==repeated.z
                  &&skink.headingRadians==repeated.headingRadians
                  &&skink.behavior==repeated.behavior
                  &&skink.colorVariant==repeated.colorVariant,
              "skink construction is deterministic");
        Check(std::hypot(skink.x-home.x,skink.z-home.z)<=home.radius
                  &&home.radius>=6.4f&&home.radius<=7.51f,
              "each skink begins inside its compact home range");
        Check(sample.height>.15f&&sample.signedShoreDistance>=34.0f
                  &&sample.slopeRadians<.52f
                  &&std::fabs(skink.y-sample.height-.034f)<.001f,
              "each skink begins grounded in open inland habitat");
        Check(std::ranges::all_of(geckos.Homes(),[&](const auto& geckoHome) {
                  return std::hypot(home.x-geckoHome.x,home.z-geckoHome.z)>=4.0f;
              }),
              "skink homes remain visually separated from gecko homes");
    }
    invalidInputSkinks.Update(quietNan,quietNan,quietNan,quietNan);
    Check(invalidInputSkinks.DormantSimulationCount()
                  ==static_cast<int>(island::SkinkPopulation::Count)
              &&std::ranges::all_of(invalidInputSkinks.Skinks(),[](const auto& skink) {
                     return std::isfinite(skink.x)&&std::isfinite(skink.y)
                         &&std::isfinite(skink.z)&&std::isfinite(skink.headingRadians)
                         &&std::isfinite(skink.gaitPhase)&&std::isfinite(skink.breathPhase);
                 }),
          "skinks reject non-finite frame and observer input without state poisoning");
    bool observedSkinkBask=false;
    bool observedSkinkForage=false;
    for(int frame=0;frame<3000;++frame) {
        skinks.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        repeatedSkinks.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        for(std::size_t index=0;index<skinks.Skinks().size();++index) {
            const auto& skink=skinks.Skinks()[index];
            const auto& repeated=repeatedSkinks.Skinks()[index];
            const auto& home=skinks.Homes()[index];
            const auto sample=terrain.Sample(skink.x,skink.z);
            Check(skink.x==repeated.x&&skink.y==repeated.y&&skink.z==repeated.z
                      &&skink.headingRadians==repeated.headingRadians
                      &&skink.gaitPhase==repeated.gaitPhase
                      &&skink.behavior==repeated.behavior,
                  "skink simulation remains deterministic over a long session");
            Check(std::hypot(skink.x-home.x,skink.z-home.z)<=home.radius+.01f
                      &&sample.signedShoreDistance>=33.99f
                      &&sample.slopeRadians<.521f
                      &&std::fabs(skink.y-sample.height-.034f)<.002f,
                  "skinks remain grounded in bounded open habitats");
            Check(std::isfinite(skink.speed)&&skink.speed>=0.0f
                      &&std::isfinite(skink.gaitPhase)&&std::isfinite(skink.breathPhase),
                  "skink movement phases remain finite");
            observedSkinkBask|=skink.behavior==island::SkinkBehavior::Bask;
            observedSkinkForage|=skink.behavior==island::SkinkBehavior::Forage;
        }
    }
    Check(observedSkinkBask&&observedSkinkForage,
          "skinks complete bask and forage activity states");
    Check(skinks.FullSimulationCount()+skinks.ReducedSimulationCount()
                  +skinks.DormantSimulationCount()
              ==static_cast<int>(island::SkinkPopulation::Count),
          "skink simulation tiers account for the complete population");
    island::SkinkPopulation reactiveSkinks(terrain);
    const auto initialReactiveSkink=reactiveSkinks.Skinks()[1U];
    bool observedSkinkFlee=false;
    for(int frame=0;frame<70;++frame) {
        reactiveSkinks.Update(1.0f/60.0f,initialReactiveSkink.x,
                              initialReactiveSkink.y,initialReactiveSkink.z);
        observedSkinkFlee|=reactiveSkinks.Skinks()[1U].behavior
            ==island::SkinkBehavior::Flee;
    }
    const auto& escapedSkink=reactiveSkinks.Skinks()[1U];
    Check(observedSkinkFlee
              &&std::hypot(escapedSkink.x-initialReactiveSkink.x,
                            escapedSkink.z-initialReactiveSkink.z)>.25f,
          "a nearby observer prompts a skink to flee across the ground");

    {
        // The dip: within a minute a patrolling dragonfly drops to a hand's height over
        // the ground and touches, the touch noted there; the dip field throws a fleck of
        // spray at it that is gone within the second.
        island::DragonflySwarm dippers(terrain);
        const auto first = dippers.Dragonflies()[0];
        float lowest = 100.0f; bool noted = false;
        for (int step = 0; step < 1200 && dippers.DipCount() < 2U; ++step) {
            dippers.Update(0.05f, first.x + 10.0f, first.y, first.z);
            for (const auto& fly : dippers.Dragonflies())
                if (fly.dip > 0.9f) lowest = std::min(lowest, fly.y - terrain.Height(fly.x, fly.z));
            if (dippers.DipCount() >= 1U) {
                const auto& dip = dippers.LastDip();
                if (std::abs(dip.y - terrain.Height(dip.x, dip.z) - island::DragonflySwarm::DipHeight) < 0.01f) noted = true;
            }
        }
        Check(dippers.DipCount() >= 2U && lowest < island::DragonflySwarm::DipHeight + 0.25f && noted,
              "a patrolling dragonfly dips to touch the water and the touch is noted there");
        // The obelisk: within two minutes a perched dragonfly has its abdomen raised to full
        // and levels it again on lifting off; a flying dragonfly's abdomen is level.
        {
            float raised = 0.0f; bool levelled = false, flyingRaise = false;
            for (int step = 0; step < 2400 && !(raised > 0.9f && levelled); ++step) {
                dippers.Update(0.05f, first.x + 10.0f, first.y, first.z);
                for (const auto& fly : dippers.Dragonflies()) {
                    if (fly.behavior == island::DragonflyBehavior::Perch) raised = std::max(raised, fly.obelisk);
                    if (fly.behavior == island::DragonflyBehavior::Patrol && fly.speed > 0.8f && fly.obelisk > 0.3f) flyingRaise = true;
                    if (raised > 0.9f && fly.behavior != island::DragonflyBehavior::Perch && fly.obelisk < 0.05f) levelled = true;
                }
            }
            Check(raised > 0.9f && levelled && !flyingRaise && dippers.ObeliskCount() <= dippers.Dragonflies().size(),
                  "a perched dragonfly does not raise its abdomen in the obelisk and level it to lift off");
        }
        island::DragonflyDipField spray;
        spray.Update(0.05f, dippers.LastDip());
        Check(spray.ActiveCount() == 1U && spray.DipCount() == dippers.DipCount(), "a dip throws a fleck of spray");
        for (int step = 0; step < 20; ++step) spray.Update(0.05f, dippers.LastDip());
        Check(spray.ActiveCount() == 0U, "the fleck is gone within the second");
    }
    island::DragonflySwarm dragonflies(terrain);
    island::DragonflySwarm repeatedDragonflies(terrain);
    island::DragonflySwarm invalidInputDragonflies(terrain);
    Check(dragonflies.Dragonflies().size()==island::DragonflySwarm::Count
              &&dragonflies.Homes().size()==island::DragonflySwarm::Count,
          "dragonfly simulation exposes twelve bounded wet-habitat animals");
    for(std::size_t index=0;index<dragonflies.Dragonflies().size();++index) {
        const auto& dragonfly=dragonflies.Dragonflies()[index];
        const auto& repeated=repeatedDragonflies.Dragonflies()[index];
        const auto& home=dragonflies.Homes()[index];
        const auto sample=terrain.Sample(dragonfly.x,dragonfly.z);
        const auto inland=island::IslandEnvironmentField::SampleInland(
            dragonfly.x,dragonfly.z,sample.signedShoreDistance);
        Check(dragonfly.x==repeated.x&&dragonfly.y==repeated.y
                  &&dragonfly.z==repeated.z
                  &&dragonfly.headingRadians==repeated.headingRadians
                  &&dragonfly.behavior==repeated.behavior
                  &&dragonfly.colorVariant==repeated.colorVariant,
              "dragonfly construction is deterministic");
        Check(std::hypot(dragonfly.x-home.x,dragonfly.z-home.z)<=home.radius
                  &&home.radius>=8.5f&&home.radius<=10.5f,
              "each dragonfly begins inside a compact stream or wetland range");
        Check(sample.height>0.0f&&sample.signedShoreDistance>12.0f
                  &&sample.slopeRadians<.55f&&inland.moisture>.34f
                  &&dragonfly.y>=sample.height+.70f
                  &&dragonfly.y<=sample.height+1.22f,
              "each dragonfly begins above suitable moist inland habitat");
        Check(dragonfly.scale>=.169f&&dragonfly.scale<=.191f,
              "dragonflies retain a plausible sub-ten-centimetre body scale");
    }
    invalidInputDragonflies.Update(quietNan,quietNan,quietNan,quietNan);
    Check(invalidInputDragonflies.DormantSimulationCount()
                  ==static_cast<int>(island::DragonflySwarm::Count)
              &&std::ranges::all_of(invalidInputDragonflies.Dragonflies(),
                    [](const auto& dragonfly) {
                        return std::isfinite(dragonfly.x)
                            &&std::isfinite(dragonfly.y)
                            &&std::isfinite(dragonfly.z)
                            &&std::isfinite(dragonfly.headingRadians)
                            &&std::isfinite(dragonfly.wingPhase);
                    }),
          "dragonflies reject non-finite frame and observer input without state poisoning");
    bool observedDragonflyHover=false;
    bool observedDragonflyPatrol=false;
    bool observedDragonflyDart=false;
    for(int frame=0;frame<4000;++frame) {
        dragonflies.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        repeatedDragonflies.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        for(std::size_t index=0;index<dragonflies.Dragonflies().size();++index) {
            const auto& dragonfly=dragonflies.Dragonflies()[index];
            const auto& repeated=repeatedDragonflies.Dragonflies()[index];
            const auto& home=dragonflies.Homes()[index];
            const auto sample=terrain.Sample(dragonfly.x,dragonfly.z);
            const auto inland=island::IslandEnvironmentField::SampleInland(
                dragonfly.x,dragonfly.z,sample.signedShoreDistance);
            Check(dragonfly.x==repeated.x&&dragonfly.y==repeated.y
                      &&dragonfly.z==repeated.z
                      &&dragonfly.headingRadians==repeated.headingRadians
                      &&dragonfly.wingPhase==repeated.wingPhase
                      &&dragonfly.behavior==repeated.behavior,
                  "dragonfly simulation remains deterministic over a long session");
            Check(std::hypot(dragonfly.x-home.x,dragonfly.z-home.z)<=home.radius+.01f
                      &&sample.height>0.0f&&sample.signedShoreDistance>12.0f
                      &&sample.slopeRadians<.55f&&inland.moisture>.34f
                      &&dragonfly.y>=sample.height+.45f
                      &&dragonfly.y<=sample.height+1.42f,
                  "dragonflies remain airborne in bounded moist habitats");
            Check(std::isfinite(dragonfly.speed)&&dragonfly.speed>=0.0f
                      &&std::isfinite(dragonfly.wingPhase)
                      &&std::isfinite(dragonfly.bankRadians),
                  "dragonfly flight state remains finite");
            observedDragonflyHover|=
                dragonfly.behavior==island::DragonflyBehavior::Hover;
            observedDragonflyPatrol|=
                dragonfly.behavior==island::DragonflyBehavior::Patrol;
            observedDragonflyDart|=
                dragonfly.behavior==island::DragonflyBehavior::Dart;
        }
    }
    Check(observedDragonflyHover&&observedDragonflyPatrol&&observedDragonflyDart,
          "dragonflies complete hover, patrol, and dart activity states");
    {
        // A perch: one patrol in three ends with the dragonfly settled on a stem's top at
        // half a metre, still, wings held level, and off again within ten seconds.
        island::DragonflySwarm perchers(terrain);
        const auto first=perchers.Dragonflies()[0];
        bool perched=false;int perchedSteps=0;std::size_t percher=3;
        for(int step=0;step<3600&&!perched;++step) {
            perchers.Update(0.05f,first.x+12.0f,first.y+1.0f,first.z);
            for(std::size_t i=0;i<3;++i) {
                const auto& fly=perchers.Dragonflies()[i];
                if(fly.behavior!=island::DragonflyBehavior::Perch) continue;
                const float lift=fly.y-terrain.Height(fly.x,fly.z);
                if(fly.speed<.05f&&fly.wingPhase==0.0f&&std::abs(lift-island::DragonflySwarm::PerchHeight)<.03f) {++perchedSteps;percher=i;}
                if(perchedSteps>=10) perched=true;
            }
        }
        Check(perched&&perchers.PerchCount()>=1U,"a habitat dragonfly perches on a stem's top, still and wings level, within three minutes");
        bool off=false;
        for(int step=0;step<240&&!off&&percher<3;++step) {
            perchers.Update(0.05f,first.x+12.0f,first.y+1.0f,first.z);
            off=perchers.Dragonflies()[percher].behavior!=island::DragonflyBehavior::Perch;
        }
        Check(off,"the perched dragonfly is off again within twelve seconds");
    }
    Check(dragonflies.FullSimulationCount()+dragonflies.ReducedSimulationCount()
                  +dragonflies.DormantSimulationCount()
              ==static_cast<int>(island::DragonflySwarm::Count),
          "dragonfly simulation tiers account for the complete population");
    island::DragonflySwarm reactiveDragonflies(terrain);
    const auto initialReactiveDragonfly=reactiveDragonflies.Dragonflies()[1U];
    bool reactiveDart=false;
    for(int frame=0;frame<50;++frame) {
        reactiveDragonflies.Update(1.0f/60.0f,initialReactiveDragonfly.x,
                                   initialReactiveDragonfly.y,
                                   initialReactiveDragonfly.z);
        reactiveDart|=reactiveDragonflies.Dragonflies()[1U].behavior
            ==island::DragonflyBehavior::Dart;
    }
    const auto& escapedDragonfly=reactiveDragonflies.Dragonflies()[1U];
    Check(reactiveDart
              &&std::hypot(escapedDragonfly.x-initialReactiveDragonfly.x,
                            escapedDragonfly.z-initialReactiveDragonfly.z)>.25f,
          "a nearby observer prompts a short dragonfly escape dart");

    {
        // The beetles' elytra fan: among the beetles of the first colony watched from six
        // metres, within twenty seconds a resting beetle lifts its wing cases fully and
        // lowers them again as it forages on; one hidden never lifts them.
        island::BeetlePopulation beetles(terrain);
        const auto& colony=beetles.Homes()[0];
        float lifted=0.0f; bool lowered=false,hiddenLift=false;
        for(int step=0;step<400;++step) {
            beetles.Update(0.05f,colony.x+6.0f,terrain.Height(colony.x,colony.z)+1.0f,colony.z);
            for(std::size_t i=0;i<island::BeetlePopulation::PerColony;++i) {
                const auto& beetle=beetles.Beetles()[i];
                lifted=std::max(lifted,beetle.elytraLift);
                if(lifted>0.9f&&beetle.behavior==island::BeetleBehavior::Forage&&beetle.elytraLift<0.05f) lowered=true;
            }
            for(const auto& beetle:beetles.Beetles()) if(beetle.visibility<0.3f&&beetle.elytraLift>0.1f) hiddenLift=true;
        }
        Check(beetles.FanCount()>=1U&&lifted>0.9f&&lowered&&!hiddenLift,"a resting beetle lifts its wing cases and lowers them to forage on");
    }
    {
        // The cicadas' call pulse: watched from twelve metres (fully simulated, beyond the
        // hush range), within half a minute a calling cicada's envelope rises to full with
        // its tymbals drumming on, and falls away again within a second of the call's end.
        const island::JungleLayout cicadaJungle(terrain);
        island::CicadaColony cicadas(cicadaJungle);
        const auto& perch=cicadas.Homes()[0];
        float peak=0.0f,drummed=0.0f; bool fell=false;
        for(int step=0;step<600;++step) {
            cicadas.Update(0.05f,perch.x+12.0f,perch.y,perch.z);
            for(const auto& cicada:cicadas.Cicadas()) {
                if(cicada.simulationTier!=island::WildlifeSimulationTier::Full) continue;
                peak=std::max(peak,cicada.call);
                if(cicada.call>0.9f) drummed=std::max(drummed,cicada.tymbalPhase);
                if(peak>0.9f&&cicada.behavior==island::CicadaBehavior::Rest&&cicada.call<0.05f) fell=true;
            }
        }
        Check(peak>0.9f&&drummed>1.0f&&fell,"a calling cicada's envelope rises with the tymbals drumming and falls away with the call");
    }
    island::FrogPopulation frogs(terrain);
    island::FrogPopulation repeatedFrogs(terrain);
    island::FrogPopulation invalidInputFrogs(terrain);
    Check(frogs.Frogs().size()==island::FrogPopulation::Count
              &&frogs.Homes().size()==island::FrogPopulation::Count,
          "frog population and homes have exact deterministic counts");
    {
        // The call: a frog told it has called swells its throat within a quarter second,
        // pulses it through the call's second and a half, and lets it fall slack within a
        // second after; an index off the end is ignored.
        island::FrogPopulation callers(terrain);
        const auto first=callers.Frogs()[0];
        callers.NoteCall(0);
        callers.NoteCall(99);
        float peak=0.0f,trough=1.0f,quarter=0.0f;
        for(int step=0;step<28;++step) {
            callers.Update(0.05f,first.x+10.0f,first.y+1.6f,first.z);
            const float throat=callers.Frogs()[0].throat;
            if(step==4) quarter=throat;
            if(step>=4) { peak=std::max(peak,throat); trough=std::min(trough,throat); }
        }
        Check(callers.CallCount()==1U&&quarter>0.3f&&peak>0.9f&&trough<0.25f,"a calling frog's throat swells within a quarter second and pulses through the call");
        for(int step=0;step<20;++step) callers.Update(0.05f,first.x+10.0f,first.y+1.6f,first.z);
        Check(callers.Frogs()[0].throat<0.05f,"the throat falls slack within a second of the call's end");
    }
    {
        // The blink: among the frogs watched from ten metres, within half a minute at least
        // two blinks begin, a blink closing fully and the eyes up again within half a second;
        // no hopping frog blinks.
        island::FrogPopulation blinkers(terrain);
        const auto first=blinkers.Frogs()[0];
        float peak=0.0f; bool settled=false,hopBlink=false; int longest=0,run=0;
        for(int step=0;step<600;++step) {
            blinkers.Update(0.05f,first.x+10.0f,first.y+1.6f,first.z);
            bool anyClosed=false;
            for(const auto& frog:blinkers.Frogs()) {
                if(frog.simulationTier!=island::WildlifeSimulationTier::Full) continue;   // the throttled tiers hold a blink over their skipped frames
                peak=std::max(peak,frog.blink);
                if(frog.blink>0.9f) anyClosed=true;
                if(frog.behavior==island::FrogBehavior::Hop&&frog.hopPhase>0.3f&&frog.blink>0.5f) hopBlink=true;
            }
            run=anyClosed?run+1:0; longest=std::max(longest,run);
            if(peak>0.9f&&!anyClosed) settled=true;
        }
        Check(blinkers.BlinkCount()>=2U&&peak>0.9f&&settled&&longest<=10&&!hopBlink,"a resting frog blinks for a moment and the eyes come up again");
        // The gular pump: a resting frog's throat swells past nine tenths and falls under
        // a tenth within the next ten seconds; a hopping frog's lies still.
        {
            float pumpHigh=0.0f,pumpLow=1.0f,hopPump=0.0f;
            for(int step=0;step<200;++step) {
                blinkers.Update(0.05f,first.x+10.0f,first.y+1.6f,first.z);
                for(const auto& frog:blinkers.Frogs()) {
                    if(frog.simulationTier!=island::WildlifeSimulationTier::Full) continue;
                    if(frog.behavior==island::FrogBehavior::Rest) { pumpHigh=std::max(pumpHigh,frog.gular); pumpLow=std::min(pumpLow,frog.gular); }
                    if(frog.behavior==island::FrogBehavior::Hop&&frog.hopPhase>0.5f) hopPump=std::max(hopPump,frog.gular);
                }
            }
            Check(pumpHigh>0.9f&&pumpLow<0.1f&&hopPump<0.5f&&blinkers.PumpingCount()<=blinkers.Frogs().size(),"a resting frog's throat does not pump with its breathing, or a hopping one's does");
        }
    }
    for(std::size_t index=0;index<frogs.Frogs().size();++index) {
        const auto& frog=frogs.Frogs()[index];
        const auto& repeated=repeatedFrogs.Frogs()[index];
        const auto& home=frogs.Homes()[index];
        const auto sample=terrain.Sample(frog.x,frog.z);
        const auto inland=island::IslandEnvironmentField::SampleInland(
            frog.x,frog.z,sample.signedShoreDistance);
        Check(std::isfinite(frog.x)&&std::isfinite(frog.y)&&std::isfinite(frog.z)
                  &&std::hypot(frog.x-home.x,frog.z-home.z)<=home.radius+.001f,
              "frogs start finite inside bounded homes");
        Check(std::abs(frog.y-terrain.Height(frog.x,frog.z)-.045f)<.001f
                  &&inland.moisture>.52f&&sample.slopeRadians<.48f,
              "frogs start grounded in moist stream or wetland habitat");
        Check(frog.x==repeated.x&&frog.y==repeated.y&&frog.z==repeated.z
                  &&frog.headingRadians==repeated.headingRadians
                  &&frog.colorVariant==repeated.colorVariant,
              "frog construction is deterministic");
    }
    invalidInputFrogs.Update(quietNan,quietNan,quietNan,quietNan);
    Check(invalidInputFrogs.DormantSimulationCount()
                  ==static_cast<int>(island::FrogPopulation::Count),
          "frogs reject non-finite frame and observer input without state poisoning");
    bool observedFrogRest=false,observedFrogHop=false;
    for(int frame=0;frame<1800;++frame) {
        frogs.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        repeatedFrogs.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        for(std::size_t index=0;index<frogs.Frogs().size();++index) {
            const auto& frog=frogs.Frogs()[index];
            const auto& repeated=repeatedFrogs.Frogs()[index];
            const auto& home=frogs.Homes()[index];
            observedFrogRest|=frog.behavior==island::FrogBehavior::Rest;
            observedFrogHop|=frog.behavior==island::FrogBehavior::Hop;
            Check(std::hypot(frog.x-home.x,frog.z-home.z)<=home.radius+.02f
                      &&std::isfinite(frog.y)&&frog.y>=terrain.Height(frog.x,frog.z)+.044f,
                  "frogs stay finite, above ground, and inside wet homes");
            Check(frog.x==repeated.x&&frog.y==repeated.y&&frog.z==repeated.z
                      &&frog.behavior==repeated.behavior,
                  "frog rest and hopping remain deterministic");
        }
    }
    Check(observedFrogRest&&observedFrogHop,
          "frogs complete both resting and ordinary hopping states");
    island::FrogPopulation reactiveFrogs(terrain);
    const auto initialReactiveFrog=reactiveFrogs.Frogs()[1U];
    bool observedFrogFlee=false;
    for(int frame=0;frame<120;++frame) {
        reactiveFrogs.Update(1.0f/60.0f,initialReactiveFrog.x,
                            initialReactiveFrog.y,initialReactiveFrog.z);
        observedFrogFlee|=reactiveFrogs.Frogs()[1U].behavior
            ==island::FrogBehavior::Flee;
    }
    Check(observedFrogFlee
              &&std::hypot(reactiveFrogs.Frogs()[1U].x-initialReactiveFrog.x,
                            reactiveFrogs.Frogs()[1U].z-initialReactiveFrog.z)>.20f,
          "nearby observer triggers a visible bounded frog retreat");
    {
        // A pool frog fleeing the walker leaps into the pool and goes under, surfaces after
        // its dive and hops back out onto a bank.
        island::FrogPopulation divers(terrain);
        const std::size_t poolFrog=island::FrogPopulation::Count-1U;
        const auto start=divers.Frogs()[poolFrog];
        bool under=false,up=false;
        for(int frame=0;frame<900&&!up;++frame) {
            divers.Update(1.0f/60.0f,start.x,start.y,start.z);
            const auto& frog=divers.Frogs()[poolFrog];
            if(frog.behavior==island::FrogBehavior::Dive&&frog.submerged>.9f
               &&frog.y<divers.PoolWaterHeight()-.08f) under=true;
            if(under&&frog.behavior==island::FrogBehavior::Dive&&frog.submerged<.1f
               &&std::abs(frog.y-divers.PoolWaterHeight()-.02f)<.01f) up=true;
        }
        Check(under&&divers.DiveCount()>=1U,"a pool frog fleeing the walker leaps into the pool and swims under within fifteen seconds");
        Check(up,"the frog surfaces after its dive and floats");
        bool back=false;
        for(int frame=0;frame<1200&&!back;++frame) {
            divers.Update(1.0f/60.0f,start.x+40.0f,start.y,start.z);
            const auto& frog=divers.Frogs()[poolFrog];
            back=frog.behavior!=island::FrogBehavior::Dive&&frog.submerged==0.0f
                &&std::abs(frog.y-terrain.Height(frog.x,frog.z)-.045f)<.001f
                &&std::hypot(frog.x-divers.Homes()[poolFrog].x,frog.z-divers.Homes()[poolFrog].z)<=divers.Homes()[poolFrog].radius+.02f;
        }
        Check(back,"the frog hops back out of the pool onto its bank within twenty seconds");
        island::FrogPopulation bankFrogs(terrain);
        const auto streamFrog=bankFrogs.Frogs()[1U];
        for(int frame=0;frame<300;++frame) bankFrogs.Update(1.0f/60.0f,streamFrog.x,streamFrog.y,streamFrog.z);
        Check(bankFrogs.DiveCount()==0U,"a stream frog fleeing the walker does not dive");
    }

    island::HermitCrabGroup hermitCrabs(terrain);
    island::HermitCrabGroup repeatedHermitCrabs(terrain);
    island::HermitCrabGroup invalidInputHermitCrabs(terrain);
    Check(hermitCrabs.Crabs().size()==island::HermitCrabGroup::Count
              &&hermitCrabs.Homes().size()==island::HermitCrabGroup::Count,
          "hermit-crab simulation exposes ten bounded shell-bearing animals");
    for(std::size_t index=0;index<hermitCrabs.Crabs().size();++index) {
        const auto& crab=hermitCrabs.Crabs()[index];
        const auto& repeated=repeatedHermitCrabs.Crabs()[index];
        const auto& home=hermitCrabs.Homes()[index];
        const auto sample=terrain.Sample(crab.x,crab.z);
        Check(crab.x==repeated.x&&crab.y==repeated.y&&crab.z==repeated.z
                  &&crab.headingRadians==repeated.headingRadians
                  &&crab.behavior==repeated.behavior
                  &&crab.shellVariant==repeated.shellVariant,
              "hermit-crab construction is deterministic");
        Check(std::hypot(crab.x-home.x,crab.z-home.z)<=home.radius
                  &&home.radius>=3.8f&&home.radius<=4.71f,
              "each hermit crab begins inside a compact beach home range");
        Check(sample.height>=-.01f
                  &&sample.signedShoreDistance>=island::HermitCrabGroup::MinimumShoreDistance
                  &&sample.signedShoreDistance<=island::HermitCrabGroup::MaximumShoreDistance
                  &&sample.slopeRadians<.34f
                  &&std::fabs(crab.y-sample.height-.038f)<.001f,
              "each hermit crab begins grounded on qualified sandy coast");
    }
    invalidInputHermitCrabs.Update(quietNan,quietNan,quietNan,quietNan);
    Check(invalidInputHermitCrabs.DormantSimulationCount()
                  ==static_cast<int>(island::HermitCrabGroup::Count)
              &&std::ranges::all_of(invalidInputHermitCrabs.Crabs(),[](const auto& crab) {
                     return std::isfinite(crab.x)&&std::isfinite(crab.y)
                         &&std::isfinite(crab.z)&&std::isfinite(crab.headingRadians)
                         &&std::isfinite(crab.legPhase)&&std::isfinite(crab.antennaPhase);
                 }),
          "hermit crabs reject non-finite frame and observer input without state poisoning");
    bool observedHermitPause=false;
    bool observedHermitForage=false;
    for(int frame=0;frame<4000;++frame) {
        hermitCrabs.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        repeatedHermitCrabs.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        for(std::size_t index=0;index<hermitCrabs.Crabs().size();++index) {
            const auto& crab=hermitCrabs.Crabs()[index];
            const auto& repeated=repeatedHermitCrabs.Crabs()[index];
            const auto& home=hermitCrabs.Homes()[index];
            const auto sample=terrain.Sample(crab.x,crab.z);
            Check(crab.x==repeated.x&&crab.y==repeated.y&&crab.z==repeated.z
                      &&crab.headingRadians==repeated.headingRadians
                      &&crab.legPhase==repeated.legPhase
                      &&crab.behavior==repeated.behavior,
                  "hermit-crab simulation remains deterministic over a long session");
            Check(std::hypot(crab.x-home.x,crab.z-home.z)<=home.radius+.01f
                      &&sample.height>=-.01f
                      &&sample.signedShoreDistance>=island::HermitCrabGroup::MinimumShoreDistance
                      &&sample.signedShoreDistance<=island::HermitCrabGroup::MaximumShoreDistance
                      &&sample.slopeRadians<.34f
                      &&std::fabs(crab.y-sample.height-.038f)<.002f,
                  "hermit crabs remain grounded inside sandy coastal habitats");
            Check(std::isfinite(crab.speed)&&crab.speed>=0.0f
                      &&std::isfinite(crab.legPhase)&&std::fabs(crab.legPhase)<=1.001f,
                  "hermit-crab movement and gait remain finite and bounded");
            observedHermitPause|=crab.behavior==island::HermitCrabBehavior::Pause;
            observedHermitForage|=crab.behavior==island::HermitCrabBehavior::Forage;
        }
    }
    Check(observedHermitPause&&observedHermitForage,
          "hermit crabs complete pause and forage activity states");
    Check(hermitCrabs.FullSimulationCount()+hermitCrabs.ReducedSimulationCount()
                  +hermitCrabs.DormantSimulationCount()
              ==static_cast<int>(island::HermitCrabGroup::Count),
          "hermit-crab simulation tiers account for the complete population");
    island::HermitCrabGroup reactiveHermitCrabs(terrain);
    const auto initialReactiveHermit=reactiveHermitCrabs.Crabs()[3U];
    bool observedRetreat=false;
    for(int frame=0;frame<50;++frame) {
        reactiveHermitCrabs.Update(1.0f/60.0f,initialReactiveHermit.x,
                                   initialReactiveHermit.y,initialReactiveHermit.z);
        observedRetreat|=reactiveHermitCrabs.Crabs()[3U].behavior
            ==island::HermitCrabBehavior::Retreat;
    }
    const auto& escapedHermit=reactiveHermitCrabs.Crabs()[3U];
    Check(observedRetreat
              &&std::hypot(escapedHermit.x-initialReactiveHermit.x,
                            escapedHermit.z-initialReactiveHermit.z)>.12f,
          "a nearby observer prompts a hermit crab to retreat with its shell");
    {
        // Still pressed when the run ends, the crab stops and pulls into its shell; with the
        // observer gone it comes out again after a few clear seconds and goes on.
        bool withdrew=false;
        for(int frame=0;frame<240&&!withdrew;++frame) {
            const auto& crab=reactiveHermitCrabs.Crabs()[3U];
            reactiveHermitCrabs.Update(1.0f/60.0f,crab.x+.4f,crab.y,crab.z);
            withdrew=crab.behavior==island::HermitCrabBehavior::Withdraw
                &&crab.withdrawAmount>.9f&&crab.speed<.01f;
        }
        Check(withdrew&&reactiveHermitCrabs.WithdrawCount()>=1U&&reactiveHermitCrabs.WithdrawnCount()>=1U,
              "a hermit crab still pressed after its retreat pulls into its shell within four seconds");
        bool emerged=false;
        for(int frame=0;frame<600&&!emerged;++frame) {
            reactiveHermitCrabs.Update(1.0f/60.0f,0.0f,100.0f,0.0f);
            const auto& crab=reactiveHermitCrabs.Crabs()[3U];
            emerged=crab.behavior!=island::HermitCrabBehavior::Withdraw&&crab.withdrawAmount<=0.0f;
        }
        Check(emerged,"with the beach clear the withdrawn hermit crab comes out within ten seconds");
    }
    {
        // Feeding: a crab watched from twenty metres feeds on each pause, the small claw's
        // rhythm advancing, and stops to forage again; a crab in its shell does not feed.
        island::HermitCrabGroup feeders(terrain);
        const auto first=feeders.Crabs()[0];
        float peak=0.0f,phase=0.0f; bool stopped=false,shellFed=false;
        for(int frame=0;frame<900;++frame) {
            feeders.Update(1.0f/60.0f,first.x+20.0f,first.y+1.6f,first.z);
            const auto& crab=feeders.Crabs()[0];
            peak=std::max(peak,crab.feedAmount);
            if(crab.feedAmount>.9f) phase=std::max(phase,crab.feedPhase);
            if(peak>.9f&&crab.feedAmount<.1f&&crab.behavior==island::HermitCrabBehavior::Forage) stopped=true;
            for(const auto& other:feeders.Crabs()) if(other.withdrawAmount>.5f&&other.feedAmount>.1f) shellFed=true;
        }
        Check(feeders.FeedCount()>=1U&&peak>.9f&&phase>1.0f&&stopped&&!shellFed,
              "a paused hermit crab feeds with its small claw and goes on foraging");
        // The shell: carried at full on the walk, set down within a second of a pause, and
        // never lifted by a crab in its shell.
        {
            float carried=0.0f; bool setDown=false,withdrawnCarry=false;
            for(int frame=0;frame<900;++frame) {
                feeders.Update(1.0f/60.0f,first.x+20.0f,first.y+1.6f,first.z);
                for(const auto& crab:feeders.Crabs()) {
                    if(crab.behavior==island::HermitCrabBehavior::Forage&&crab.speed>.15f) carried=std::max(carried,crab.shellCarry);
                    if(crab.behavior==island::HermitCrabBehavior::Pause&&crab.speed<.01f&&crab.shellCarry<.1f) setDown=true;
                    if(crab.withdrawAmount>.5f&&crab.shellCarry>.1f) withdrawnCarry=true;
                }
            }
            Check(carried>.9f&&feeders.CarryingCount()<=feeders.Crabs().size()&&setDown&&!withdrawnCarry,
                  "a walking hermit crab does not lift its shell and set it down at a pause");
        }
    }

    {
        // Nocturnal: over ten minutes with no observer the crabs spend far more of their time
        // resting by day than by night.
        island::CoconutCrabGroup day(terrain), night(terrain);
        day.SetNightness(0.0f);
        night.SetNightness(1.0f);
        int dayRest = 0, nightRest = 0, samples = 0;
        for (int step = 0; step < 30 * 600; ++step) {
            day.Update(1.0f / 30.0f, 900.0f, 0.0f, 900.0f);
            night.Update(1.0f / 30.0f, 900.0f, 0.0f, 900.0f);
            if (step % 30 != 0) continue;
            for (const auto& crab : day.Crabs()) dayRest += crab.behavior == island::CoconutCrabBehavior::Rest ? 1 : 0;
            for (const auto& crab : night.Crabs()) nightRest += crab.behavior == island::CoconutCrabBehavior::Rest ? 1 : 0;
            samples += static_cast<int>(day.Crabs().size());
        }
        Check(dayRest > samples * 6 / 10 && nightRest < dayRest / 2,
              "coconut crabs rest out the day and forage through the night");
    }
    island::CoconutCrabGroup coconutCrabs(terrain);
    island::CoconutCrabGroup repeatedCoconutCrabs(terrain);
    island::CoconutCrabGroup invalidInputCoconutCrabs(terrain);
    Check(coconutCrabs.Crabs().size()==island::CoconutCrabGroup::Count
              &&coconutCrabs.Homes().size()==island::CoconutCrabGroup::Count,
          "coconut-crab simulation exposes six bounded terrestrial animals");
    for(std::size_t index=0;index<coconutCrabs.Crabs().size();++index) {
        const auto& crab=coconutCrabs.Crabs()[index];
        const auto& repeated=repeatedCoconutCrabs.Crabs()[index];
        const auto& home=coconutCrabs.Homes()[index];
        const auto sample=terrain.Sample(crab.x,crab.z);
        Check(crab.x==repeated.x&&crab.y==repeated.y&&crab.z==repeated.z
                  &&crab.headingRadians==repeated.headingRadians
                  &&crab.behavior==repeated.behavior
                  &&crab.colorVariant==repeated.colorVariant,
              "coconut-crab construction is deterministic");
        Check(std::hypot(crab.x-home.x,crab.z-home.z)<=home.radius
                  &&home.radius>=7.5f&&home.radius<=9.11f,
              "each coconut crab begins inside a coastal home range");
        Check(sample.height>.15f
                  &&sample.signedShoreDistance>=island::CoconutCrabGroup::MinimumShoreDistance
                  &&sample.signedShoreDistance<=island::CoconutCrabGroup::MaximumShoreDistance
                  &&sample.slopeRadians<.42f
                  &&std::fabs(crab.y-sample.height-.080f)<.001f,
              "each coconut crab begins grounded in coastal palm-zone terrain");
    }
    invalidInputCoconutCrabs.Update(quietNan,quietNan,quietNan,quietNan);
    Check(invalidInputCoconutCrabs.DormantSimulationCount()
                  ==static_cast<int>(island::CoconutCrabGroup::Count)
              &&std::ranges::all_of(invalidInputCoconutCrabs.Crabs(),[](const auto& crab) {
                     return std::isfinite(crab.x)&&std::isfinite(crab.y)
                         &&std::isfinite(crab.z)&&std::isfinite(crab.headingRadians)
                         &&std::isfinite(crab.legPhase)&&std::isfinite(crab.clawPose);
                 }),
          "coconut crabs reject non-finite frame and observer input without state poisoning");
    bool observedCoconutRest=false;
    bool observedCoconutForage=false;
    for(int frame=0;frame<4000;++frame) {
        coconutCrabs.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        repeatedCoconutCrabs.Update(1.0f/30.0f,0.0f,100.0f,0.0f);
        for(std::size_t index=0;index<coconutCrabs.Crabs().size();++index) {
            const auto& crab=coconutCrabs.Crabs()[index];
            const auto& repeated=repeatedCoconutCrabs.Crabs()[index];
            const auto& home=coconutCrabs.Homes()[index];
            const auto sample=terrain.Sample(crab.x,crab.z);
            Check(crab.x==repeated.x&&crab.y==repeated.y&&crab.z==repeated.z
                      &&crab.headingRadians==repeated.headingRadians
                      &&crab.legPhase==repeated.legPhase
                      &&crab.behavior==repeated.behavior,
                  "coconut-crab simulation remains deterministic over a long session");
            Check(std::hypot(crab.x-home.x,crab.z-home.z)<=home.radius+.01f
                      &&sample.signedShoreDistance>=island::CoconutCrabGroup::MinimumShoreDistance
                      &&sample.signedShoreDistance<=island::CoconutCrabGroup::MaximumShoreDistance
                      &&sample.slopeRadians<.42f
                      &&(crab.behavior==island::CoconutCrabBehavior::Climb
                         ? crab.y>=sample.height+.079f&&crab.y<=sample.height+.081f+island::CoconutCrabGroup::ClimbHeight
                         : std::fabs(crab.y-sample.height-.080f)<.002f),
                  "coconut crabs remain grounded in bounded coastal habitats, or on a trunk while climbing");
            Check(std::isfinite(crab.speed)&&crab.speed>=0.0f
                      &&std::isfinite(crab.legPhase)&&std::fabs(crab.legPhase)<=1.001f
                      &&crab.clawPose>=.13f&&crab.clawPose<=.51f,
                  "coconut-crab movement and claw state remain finite and bounded");
            observedCoconutRest|=crab.behavior==island::CoconutCrabBehavior::Rest;
            observedCoconutForage|=crab.behavior==island::CoconutCrabBehavior::Forage;
        }
    }
    Check(observedCoconutRest&&observedCoconutForage,
          "coconut crabs complete rest and forage activity states");
    {
        // A living palm in a crab's range: every fourth forage without a coconut is a climb
        // up the trunk, a pause near three metres up, and the way down to rest at its foot.
        island::CoconutCrabGroup climbers(terrain);
        const auto& home=climbers.Homes()[0];
        std::vector<island::PalmPlacement> palms(1);
        palms[0].x=home.x+2.0f;palms[0].z=home.z;palms[0].y=terrain.Height(palms[0].x,palms[0].z);
        palms[0].scale=1.0f;palms[0].condition=island::PalmCondition::Living;
        climbers.SetPalms(palms);
        bool up=false,turned=false;float top=0.0f;
        for(int frame=0;frame<9000&&!(up&&turned&&top>=island::CoconutCrabGroup::ClimbHeight-.01f);++frame) {
            climbers.Update(1.0f/30.0f,home.x+12.0f,terrain.Height(home.x,home.z)+1.0f,home.z);
            const auto& crab=climbers.Crabs()[0];
            if(crab.behavior==island::CoconutCrabBehavior::Climb) {
                const float lift=crab.y-terrain.Height(crab.x,crab.z)-.080f;
                top=std::max(top,lift);
                if(lift>1.5f) up=true;
                if(crab.climbAmount>.9f&&std::hypot(crab.x-palms[0].x,crab.z-palms[0].z)<island::CoconutCrabGroup::TrunkStand+.15f) turned=true;
            }
        }
        Check(up&&turned&&climbers.ClimbCount()>=1U,
              "a coconut crab walks to the palm in its range and climbs the trunk, body turned up it, within five minutes");
        Check(top>=island::CoconutCrabGroup::ClimbHeight-.01f,"the climb reaches the climbing height");
        bool down=false;
        for(int frame=0;frame<1200&&!down;++frame) {
            climbers.Update(1.0f/30.0f,home.x+12.0f,terrain.Height(home.x,home.z)+1.0f,home.z);
            const auto& crab=climbers.Crabs()[0];
            down=crab.behavior!=island::CoconutCrabBehavior::Climb&&crab.climbAmount==0.0f
                &&std::fabs(crab.y-terrain.Height(crab.x,crab.z)-.080f)<.002f;
        }
        Check(down,"the crab comes down the trunk and is back on the ground within forty seconds of the top");
    }
    Check(coconutCrabs.FullSimulationCount()+coconutCrabs.ReducedSimulationCount()
                  +coconutCrabs.DormantSimulationCount()
              ==static_cast<int>(island::CoconutCrabGroup::Count),
          "coconut-crab simulation tiers account for the complete population");
    island::CoconutCrabGroup reactiveCoconutCrabs(terrain);
    const auto initialReactiveCoconut=reactiveCoconutCrabs.Crabs()[2U];
    bool observedCoconutRetreat=false;
    for(int frame=0;frame<70;++frame) {
        reactiveCoconutCrabs.Update(1.0f/60.0f,initialReactiveCoconut.x,
                                    initialReactiveCoconut.y,initialReactiveCoconut.z);
        observedCoconutRetreat|=reactiveCoconutCrabs.Crabs()[2U].behavior
            ==island::CoconutCrabBehavior::Retreat;
    }
    const auto& escapedCoconut=reactiveCoconutCrabs.Crabs()[2U];
    Check(observedCoconutRetreat
              &&std::hypot(escapedCoconut.x-initialReactiveCoconut.x,
                            escapedCoconut.z-initialReactiveCoconut.z)>.20f,
          "a nearby observer prompts a coconut crab to retreat");

    island::SeaTurtleGroup seaTurtles(terrain);
    island::SeaTurtleGroup repeatedSeaTurtles(terrain);
    island::SeaTurtleGroup invalidInputSeaTurtles(terrain);
    Check(seaTurtles.Turtles().size() == island::SeaTurtleGroup::TurtleCount
              && seaTurtles.Homes().size() == island::SeaTurtleGroup::TurtleCount,
          "sea-turtle simulation exposes the intended small bounded population");
    {
        // Grazing: within a minute a turtle foraging low over the bottom dips its head in
        // bites, each noted at its beak on the bottom; the sand field raises a puff at a bite
        // that settles within two seconds.
        island::SeaTurtleGroup grazers(terrain);
        const auto first = grazers.Turtles()[0];
        float deepest = 0.0f; bool near = true;
        for (int step = 0; step < 1200 && grazers.BiteCount() < 2U; ++step) {
            grazers.Update(0.05f, first.x + 10.0f, 0.0f, first.z);
            for (const auto& turtle : grazers.Turtles()) deepest = std::max(deepest, turtle.graze);
            if (grazers.BiteCount() >= 1U) {
                const auto& bite = grazers.LastBite();
                bool close = false;
                for (const auto& turtle : grazers.Turtles())
                    if (std::hypot(turtle.x - bite.x, turtle.z - bite.z) < 1.5f * turtle.scale + 0.5f) close = true;
                if (!close || std::abs(bite.y - terrain.Height(bite.x, bite.z)) > 0.01f) near = false;
            }
        }
        Check(grazers.BiteCount() >= 2U && deepest > 0.9f && near, "a turtle foraging low over the bottom dips its head in bites noted at its beak on the bottom");
        // The eye rub: over the next two minutes a slow turtle draws a front flipper up to
        // its eye at least twice, the rub to full and let down, the sides taking turns; a
        // turtle moving fast never rubs.
        {
            float rubPeak = 0.0f; bool rubLeft = false, rubRight = false, letDown = false, fastRub = false;
            for (int step = 0; step < 2400; ++step) {
                grazers.Update(0.05f, first.x + 10.0f, 0.0f, first.z);
                for (const auto& turtle : grazers.Turtles()) {
                    rubPeak = std::max(rubPeak, turtle.scratch);
                    if (turtle.scratch > 0.9f) { if (turtle.scratchSide < 0.0f) rubLeft = true; else rubRight = true; }
                    if (rubPeak > 0.9f && turtle.scratch < 0.05f) letDown = true;
                    if (turtle.speed > 0.6f && turtle.scratch > 0.5f) fastRub = true;
                }
            }
            Check(grazers.ScratchCount() >= 2U && rubPeak > 0.9f && rubLeft && rubRight && letDown && !fastRub,
                  "a slow turtle does not rub its eye with a front flipper in turn, or a fast one does");
        }
        island::TurtleGrazeField sand;
        sand.Update(0.05f, grazers.LastBite());
        Check(sand.ActiveCount() == 1U && sand.BiteCount() == grazers.BiteCount(), "a bite raises a puff of sand");
        for (int step = 0; step < 40; ++step) sand.Update(0.05f, grazers.LastBite());
        Check(sand.ActiveCount() == 0U, "the stirred sand settles within two seconds");
    }
    std::size_t shelteredBayTurtleHomes = 0;
    for (std::size_t index = 0; index < seaTurtles.Turtles().size(); ++index) {
        const island::SeaTurtleState& turtle = seaTurtles.Turtles()[index];
        const island::SeaTurtleState& repeated = repeatedSeaTurtles.Turtles()[index];
        const island::SeaTurtleHomeRange& home = seaTurtles.Homes()[index];
        const island::TerrainSample sample = terrain.Sample(turtle.x, turtle.z);
        Check(turtle.x == repeated.x && turtle.y == repeated.y
                  && turtle.z == repeated.z
                  && turtle.headingRadians == repeated.headingRadians
                  && turtle.behavior == repeated.behavior
                  && turtle.colorVariant == repeated.colorVariant,
              "sea-turtle construction is deterministic");
        Check(std::hypot(turtle.x - home.x, turtle.z - home.z) <= home.radius
                  && home.radius >= 11.5f && home.radius <= 15.1f,
              "each sea turtle begins inside one bounded near-shore range");
        Check(sample.signedShoreDistance
                      >= island::SeaTurtleGroup::MinimumShoreDistance
                  && sample.signedShoreDistance
                      <= island::SeaTurtleGroup::MaximumShoreDistance
                  && sample.height <= -0.72f,
              "each sea turtle begins in submerged water inside the current limit");
        Check(island::IslandEnvironmentField::AquaticHabitatSuitability(
                  turtle.x, turtle.z, sample.signedShoreDistance, sample.height) >= .025f,
              "each sea turtle begins in shared reef or seagrass habitat");
        Check(turtle.y >= sample.height + 0.39f && turtle.y <= -0.09f,
              "each sea turtle begins between seabed clearance and the water surface");
        if (home.x < 0.0f && std::abs(home.z) < -home.x * .72f) {
            ++shelteredBayTurtleHomes;
        }
    }
    Check(shelteredBayTurtleHomes >= 3,
          "most sea-turtle homes must occupy the calmer sheltered western bay");
    invalidInputSeaTurtles.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputSeaTurtles.DormantSimulationCount()
              == static_cast<int>(island::SeaTurtleGroup::TurtleCount)
              && std::ranges::all_of(
                  invalidInputSeaTurtles.Turtles(), [](const auto& turtle) {
                      return std::isfinite(turtle.x) && std::isfinite(turtle.y)
                          && std::isfinite(turtle.z)
                          && std::isfinite(turtle.headingRadians);
                  }),
          "sea turtles reject non-finite frame and observer input without state poisoning");
    bool observedTurtleCruise = false;
    bool observedTurtleForage = false;
    bool observedTurtleSurface = false;
    bool observedNearSurfaceTurtle = false;
    for (int frame = 0; frame < 12000; ++frame) {
        seaTurtles.Update(1.0f / 30.0f, 0.0f, 20.0f, 0.0f);
        repeatedSeaTurtles.Update(1.0f / 30.0f, 0.0f, 20.0f, 0.0f);
        for (std::size_t index = 0; index < seaTurtles.Turtles().size(); ++index) {
            const island::SeaTurtleState& turtle = seaTurtles.Turtles()[index];
            const island::SeaTurtleState& repeated = repeatedSeaTurtles.Turtles()[index];
            const island::SeaTurtleHomeRange& home = seaTurtles.Homes()[index];
            const island::TerrainSample sample = terrain.Sample(turtle.x, turtle.z);
            Check(turtle.x == repeated.x && turtle.y == repeated.y
                      && turtle.z == repeated.z
                      && turtle.headingRadians == repeated.headingRadians
                      && turtle.pitchRadians == repeated.pitchRadians
                      && turtle.bankRadians == repeated.bankRadians
                      && turtle.flipperStroke == repeated.flipperStroke
                      && turtle.behavior == repeated.behavior,
                  "sea-turtle simulation remains deterministic over a long session");
            Check(std::hypot(turtle.x - home.x, turtle.z - home.z)
                          <= home.radius + 0.01f
                      && sample.signedShoreDistance
                          >= island::SeaTurtleGroup::MinimumShoreDistance - 0.01f
                      && sample.signedShoreDistance
                          <= island::SeaTurtleGroup::MaximumShoreDistance + 0.01f
                      && sample.height <= -0.719f
                      && turtle.y >= sample.height + 0.38f
                      && turtle.y <= -0.09f,
                  "sea turtles remain between seabed and surface inside near-shore ranges");
            Check(island::IslandEnvironmentField::AquaticHabitatSuitability(
                      turtle.x, turtle.z, sample.signedShoreDistance, sample.height) >= .025f,
                  "sea turtles remain inside shared reef or seagrass habitat");
            Check(std::isfinite(turtle.speed) && turtle.speed >= 0.0f
                      && std::isfinite(turtle.flipperStroke)
                      && std::fabs(turtle.flipperStroke) <= 1.001f
                      && std::fabs(turtle.pitchRadians) <= 0.241f
                      && std::fabs(turtle.bankRadians) <= 0.221f,
                  "sea-turtle motion and articulated flipper pose remain finite and bounded");
            observedTurtleCruise = observedTurtleCruise
                || turtle.behavior == island::SeaTurtleBehavior::Cruise;
            observedTurtleForage = observedTurtleForage
                || turtle.behavior == island::SeaTurtleBehavior::Forage;
            observedTurtleSurface = observedTurtleSurface
                || turtle.behavior == island::SeaTurtleBehavior::Surface;
            observedNearSurfaceTurtle = observedNearSurfaceTurtle
                || (turtle.behavior == island::SeaTurtleBehavior::Surface
                    && turtle.y > -0.18f);
        }
    }
    {
        // Shared turtle shape reference: bounded inside the culling sphere, teardrop outline,
        // a dome above the rim, paddles that bend and lift with the stroke, invalid input refused.
        using namespace island::sea_turtle;
        float reach = 0.0f, widest = 0.0f, widestU = 0.0f, tallest = 0.0f, tallestU = 0.0f;
        for (int i = 0; i <= 40; ++i) {
            const float u = -1.0f + 0.05f * static_cast<float>(i);
            const float w = HalfWidth(u);
            if (w > widest) { widest = w; widestU = u; }
            for (int j = 0; j <= 20; ++j) {
                const float sPos = -1.0f + 0.1f * static_cast<float>(j);
                const float h = CarapaceHeight(u, sPos), b = PlastronY(u, sPos);
                Check(std::isfinite(h) && h >= CarapaceRimY && b <= CarapaceRimY,
                      "turtle carapace stays above its rim and the plastron below it");
                if (h > tallest) { tallest = h; tallestU = u; }
                reach = std::max(reach, std::hypot(sPos * w, h, LengthToZ(u)));
            }
        }
        Check(widest > 0.35f && widestU > 0.0f && widestU < 0.5f && HalfWidth(-1.0f) == 0.0f
                  && HalfWidth(1.0f) == 0.0f,
              "turtle carapace is a closed teardrop widest ahead of its middle");
        Check(tallest > 0.18f && tallest < 0.26f && std::abs(tallestU) < 0.3f,
              "turtle carapace dome peaks near the centre at a realistic height");
        float maximumFrontPaddleHalfWidth = 0.0f, maximumRearPaddleHalfWidth = 0.0f;
        for (const float stroke : {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f}) {
            float rootWidth = 0.0f, tipWidth = 0.0f;
            const auto root = FrontFlipper(1.0f, 0.0f, stroke, rootWidth);
            const auto tip = FrontFlipper(1.0f, 1.0f, stroke, tipWidth);
            float rearWidth = 0.0f;
            const auto rearTip = RearFlipper(-1.0f, 1.0f, stroke, rearWidth);
            Check(rootWidth > tipWidth && tipWidth > 0.02f && rearWidth > 0.02f,
                  "turtle paddles taper to a nondegenerate tip");
            Check(std::hypot(tip.x - root.x, tip.y - root.y, tip.z - root.z) > 0.6f
                      && std::hypot(rearTip.x, rearTip.z) < 0.9f,
                  "front paddles are long and rear paddles short");
            for (int i = 0; i <= 10; ++i) {
                float frontWidth = 0.0f, rearSampleWidth = 0.0f;
                const auto f = FrontFlipper(-1.0f, 0.1f * static_cast<float>(i), stroke, frontWidth);
                const auto r = RearFlipper(1.0f, 0.1f * static_cast<float>(i), stroke, rearSampleWidth);
                maximumFrontPaddleHalfWidth=std::max(maximumFrontPaddleHalfWidth,frontWidth);
                maximumRearPaddleHalfWidth=std::max(maximumRearPaddleHalfWidth,rearSampleWidth);
                reach = std::max({reach, std::hypot(f.x, f.y, std::abs(f.z) + frontWidth),
                                  std::hypot(r.x, r.y, std::abs(r.z) + rearSampleWidth)});
            }
        }
        Check(maximumFrontPaddleHalfWidth>0.11f&&maximumFrontPaddleHalfWidth<0.135f
                  &&maximumRearPaddleHalfWidth>0.07f&&maximumRearPaddleHalfWidth<0.09f,
              "turtle paddles retain a narrow hydrodynamic planform rather than broad leaves");
        float upWidth = 0.0f, downWidth = 0.0f;
        const auto up = FrontFlipper(1.0f, 1.0f, 1.0f, upWidth);
        const auto down = FrontFlipper(1.0f, 1.0f, -1.0f, downWidth);
        Check(up.y > down.y + 0.3f && down.z < up.z,
              "front paddle tip lifts on the recovery stroke and sweeps back on the power stroke");
        Check(reach <= ReachLimit, "every turtle vertex stays inside the culling reach limit");
        Check(ScuteAt(0.0f) == Scute::Vertebral && ScuteAt(0.5f) == Scute::Costal
                  && ScuteAt(-0.9f) == Scute::Marginal && ScuteRow(-1.0f) == 0 && ScuteRow(1.0f) == 4,
              "turtle scute bands and rows are assigned as authored");
        for (int boundary = 1; boundary <= 4; ++boundary) {
            const float left = ScuteRowBoundaryU(boundary, -0.35f);
            const float centre = ScuteRowBoundaryU(boundary, 0.0f);
            const float right = ScuteRowBoundaryU(boundary, 0.35f);
            CheckNear(left, right, 1.0e-6f,
                      "turtle transverse scute boundaries remain bilaterally symmetric");
            Check(std::abs(centre - left) > 0.015f,
                  "turtle transverse scute boundaries form visible alternating points");
        }
        for (const int column : {1, 3, 5, 7}) {
            const float a = ScuteColumnBoundaryS(column, -0.85f);
            const float b = ScuteColumnBoundaryS(column, -0.75f);
            Check(std::abs(a - b) > 0.01f,
                  "turtle longitudinal scute boundaries visibly interlock between rows");
            CheckNear(ScuteColumnBoundaryS(column, 0.25f),
                      -ScuteColumnBoundaryS(8 - column, 0.25f), 1.0e-6f,
                      "turtle longitudinal scute boundaries remain bilaterally symmetric");
        }
        for (const float invalid : {1.5f, -1.5f, std::numeric_limits<float>::quiet_NaN()}) {
            bool rejected = 0;
            try { (void)HalfWidth(invalid); } catch (const std::invalid_argument&) { rejected = true; }
            Check(rejected, "turtle profile rejects out-of-range coordinates");
            rejected = false;
            float w = 0.0f;
            try { (void)FrontFlipper(1.0f, 0.5f, invalid, w); } catch (const std::invalid_argument&) { rejected = true; }
            Check(rejected, "turtle paddle rejects an invalid stroke");
        }
        for (const int invalidColumn : {0, 2, 4, 6, 8}) {
            bool rejected = false;
            try { (void)ScuteColumnBoundaryS(invalidColumn, 0.0f); }
            catch (const std::invalid_argument&) { rejected = true; }
            Check(rejected, "turtle scute profile rejects non-boundary columns");
        }
    }
    Check(observedTurtleCruise && observedTurtleForage && observedTurtleSurface
              && observedNearSurfaceTurtle,
          "sea turtles reach cruise, forage, and visibly near-surface states");
    Check(seaTurtles.FullSimulationCount() + seaTurtles.ReducedSimulationCount()
              + seaTurtles.DormantSimulationCount()
              == static_cast<int>(island::SeaTurtleGroup::TurtleCount),
          "sea-turtle simulation tiers account for the complete population");
    island::SeaTurtleGroup reactiveSeaTurtles(terrain);
    const island::SeaTurtleState initialReactiveTurtle = reactiveSeaTurtles.Turtles().front();
    for (int frame = 0; frame < 240; ++frame) {
        reactiveSeaTurtles.Update(
            1.0f / 60.0f, initialReactiveTurtle.x, initialReactiveTurtle.y,
            initialReactiveTurtle.z);
    }
    const island::SeaTurtleState& retreatingTurtle = reactiveSeaTurtles.Turtles().front();
    Check(std::hypot(retreatingTurtle.x - initialReactiveTurtle.x,
                     retreatingTurtle.z - initialReactiveTurtle.z) > 0.25f,
          "a nearby observer produces mild sea-turtle separation without hostility");

    {
        // After dark the crabs go home down their burrows and stay there.
        island::CrabColony nightly(terrain);
        nightly.SetNightness(1.0f);
        for (int step = 0; step < 30 * 90; ++step) nightly.Update(1.0f / 30.0f, 900.0f, 0.0f, 900.0f);
        std::size_t down = 0;
        for (const auto& crab : nightly.Crabs()) down += crab.hideAmount > 0.9f ? 1U : 0U;
        Check(down * 10U >= nightly.Crabs().size() * 9U, "after dark the beach crabs go home down their burrows");
    }
    {
        // Hermit crabs: far more time paused by day than by night.
        island::HermitCrabGroup day(terrain), night(terrain);
        day.SetNightness(0.0f);
        night.SetNightness(1.0f);
        int dayPaused = 0, nightPaused = 0, samples = 0;
        for (int step = 0; step < 30 * 300; ++step) {
            day.Update(1.0f / 30.0f, 900.0f, 0.0f, 900.0f);
            night.Update(1.0f / 30.0f, 900.0f, 0.0f, 900.0f);
            if (step % 30 != 0) continue;
            for (const auto& crab : day.Crabs()) dayPaused += crab.behavior == island::HermitCrabBehavior::Pause ? 1 : 0;
            for (const auto& crab : night.Crabs()) nightPaused += crab.behavior == island::HermitCrabBehavior::Pause ? 1 : 0;
            samples += static_cast<int>(day.Crabs().size());
        }
        Check(dayPaused > samples / 2 && nightPaused * 2 < dayPaused, "hermit crabs sit out the day and wander at night");
    }
    island::CrabColony crabs(terrain);
    island::CrabColony repeatedCrabs(terrain);
    island::CrabColony invalidInputCrabs(terrain);
    Check(crabs.Crabs().size() == island::CrabColony::CrabCount
              && crabs.Homes().size() == island::CrabColony::CrabCount,
          "beach-crab simulation exposes the intended bounded population and homes");
    for (std::size_t index = 0; index < crabs.Crabs().size(); ++index) {
        const island::CrabState& crab = crabs.Crabs()[index];
        const island::CrabState& repeated = repeatedCrabs.Crabs()[index];
        const island::CrabHomeRange& home = crabs.Homes()[index];
        const island::TerrainSample sample = terrain.Sample(crab.x, crab.z);
        Check(crab.x == repeated.x && crab.y == repeated.y && crab.z == repeated.z
                  && crab.bodyYawRadians == repeated.bodyYawRadians
                  && crab.colorVariant == repeated.colorVariant,
              "beach-crab construction is deterministic");
        Check(std::hypot(crab.x - home.x, crab.z - home.z) < 0.001f
                  && home.radius >= 5.8f && home.radius <= 8.0f,
              "each beach crab begins inside its bounded home range");
        Check(sample.signedShoreDistance >= island::CrabColony::MinimumShoreDistance
                  && sample.signedShoreDistance <= island::CrabColony::MaximumShoreDistance
                  && sample.height >= -0.02f && sample.slopeRadians < 0.38f,
              "each beach crab begins in suitable shoreline habitat");
    }
    invalidInputCrabs.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputCrabs.DormantSimulationCount()
              == static_cast<int>(island::CrabColony::CrabCount)
              && std::ranges::all_of(invalidInputCrabs.Crabs(), [](const auto& crab) {
                     return std::isfinite(crab.x) && std::isfinite(crab.y)
                         && std::isfinite(crab.z) && std::isfinite(crab.bodyYawRadians);
                 }),
          "beach crabs reject non-finite frame and observer input without state poisoning");
    {
        // The claw wave: the first crab, paused in the open, raises a claw a second into its
        // pause and sweeps it for a couple of seconds; no crab waves while it moves.
        island::CrabColony wavers(terrain);
        const auto first = wavers.Crabs()[0];
        bool movingWave = false;
        float longest = 0.0f, bout = 0.0f;
        for (int frame = 0; frame < 2400; ++frame) {
            wavers.Update(0.05f, first.x + 20.0f, first.y + 1.6f, first.z);
            for (const auto& crab : wavers.Crabs())
                if (crab.waveAmount > 0.9f && crab.behavior != island::CrabBehavior::Pause) movingWave = true;
            if (wavers.Crabs()[0].waveAmount > 0.5f) bout += 0.05f;
            else { longest = std::max(longest, bout); bout = 0.0f; }
        }
        longest = std::max(longest, bout);
        Check(wavers.WaveCount() >= 1U && longest > 1.0f && longest < 4.0f && !movingWave,
              "a crab paused in the open waves a claw for a couple of seconds and a moving crab never does");
    }
    bool observedPause = false;
    bool observedForage = false;
    for (int frame = 0; frame < 12000; ++frame) {
        crabs.Update(1.0f / 30.0f, 0.0f, 100.0f, 0.0f);
        repeatedCrabs.Update(1.0f / 30.0f, 0.0f, 100.0f, 0.0f);
        const auto states = crabs.Crabs();
        const auto repeatedStates = repeatedCrabs.Crabs();
        for (std::size_t index = 0; index < states.size(); ++index) {
            const island::CrabState& crab = states[index];
            const island::CrabState& repeated = repeatedStates[index];
            const island::CrabHomeRange& home = crabs.Homes()[index];
            const island::TerrainSample sample = terrain.Sample(crab.x, crab.z);
            Check(crab.x == repeated.x && crab.y == repeated.y && crab.z == repeated.z
                      && crab.bodyYawRadians == repeated.bodyYawRadians
                      && crab.travelHeadingRadians == repeated.travelHeadingRadians
                      && crab.behavior == repeated.behavior,
                  "beach-crab simulation remains deterministic over a long session");
            Check(std::hypot(crab.x - home.x, crab.z - home.z) <= home.radius + 0.01f
                      && sample.signedShoreDistance
                          >= island::CrabColony::MinimumShoreDistance - 0.01f
                      && sample.signedShoreDistance
                          <= island::CrabColony::MaximumShoreDistance + 0.01f
                      && sample.height >= -0.021f && sample.slopeRadians < 0.381f,
                  "beach crabs remain inside bounded dry shoreline habitat");
            Check(std::isfinite(crab.speed) && std::isfinite(crab.legPhase)
                      && std::isfinite(crab.clawPose)
                      && crab.speed >= 0.0f && std::fabs(crab.legPhase) <= 1.001f
                      && crab.clawPose >= 0.0f && crab.clawPose <= 1.0f,
                  "beach-crab animation remains finite and bounded");
            observedPause = observedPause || crab.behavior == island::CrabBehavior::Pause;
            observedForage = observedForage || crab.behavior == island::CrabBehavior::Forage;
        }
        if (frame % 30 == 0) {
            for (std::size_t first = 0; first < states.size(); ++first) {
                for (std::size_t second = first + 1; second < states.size(); ++second) {
                    Check(std::hypot(states[first].x - states[second].x,
                                     states[first].z - states[second].z)
                              >= island::CrabColony::MinimumAgentSeparation - 0.001f,
                          "live beach crabs overlap while foraging");
                }
            }
        }
    }
    Check(observedPause && observedForage,
          "beach-crab colonies alternate between quiet and foraging behaviour");
    Check(crabs.FullSimulationCount() + crabs.ReducedSimulationCount()
              + crabs.DormantSimulationCount()
              == static_cast<int>(island::CrabColony::CrabCount),
          "beach-crab simulation tiers account for the complete population");
    island::CrabColony reactiveCrabs(terrain);
    const island::CrabState initialReactiveCrab = reactiveCrabs.Crabs().front();
    for (int frame = 0; frame < 90; ++frame) {
        reactiveCrabs.Update(1.0f / 60.0f,
                             initialReactiveCrab.x, initialReactiveCrab.y,
                             initialReactiveCrab.z);
    }
    const island::CrabState& escapedCrab = reactiveCrabs.Crabs().front();
    Check(escapedCrab.behavior == island::CrabBehavior::Scuttle
              && std::hypot(escapedCrab.x - initialReactiveCrab.x,
                            escapedCrab.z - initialReactiveCrab.z) > 0.5f,
          "a nearby observer provokes a visible non-hostile crab scuttle response");
    {
        // The resident walking the beach scatters the crabs too: with the observer 40 m off
        // (near enough for full simulation) and the walker on the crab, it scuttles away
        // from the walker; with the walker withdrawn it goes back to its bouts.
        island::CrabColony walkedCrabs(terrain);
        const island::CrabState startled = walkedCrabs.Crabs().front();
        walkedCrabs.SetWalker(startled.x + 1.0f, startled.z, true);
        for (int frame = 0; frame < 90; ++frame)
            walkedCrabs.Update(1.0f / 60.0f, startled.x, startled.y + 1.5f, startled.z + 40.0f);
        const island::CrabState& fled = walkedCrabs.Crabs().front();
        Check(fled.behavior == island::CrabBehavior::Scuttle && fled.x < startled.x - 0.3f,
              "the resident's walk beside a crab sends it scuttling away from the walker");
        walkedCrabs.SetWalker(0.0f, 0.0f, false);
        bool settled = false;
        for (int frame = 0; frame < 600 && !settled; ++frame) {
            walkedCrabs.Update(1.0f / 60.0f, startled.x, startled.y + 1.5f, startled.z + 40.0f);
            settled = walkedCrabs.Crabs().front().behavior != island::CrabBehavior::Scuttle;
        }
        Check(settled, "with the walker gone the crab returns to its bouts");
    }
    {
        // A crab within reach of its colony's burrow, frightened from the far side, bolts for
        // the hole and goes down it; once the beach is clear for a few seconds it comes up
        // and goes back to its bouts, leaving the hole.
        island::CrabColony burrowCrabs(terrain);
        std::size_t subject = burrowCrabs.Crabs().size();
        for (std::size_t i = 0; i < burrowCrabs.Crabs().size() && subject == burrowCrabs.Crabs().size(); ++i) {
            const auto& hole = burrowCrabs.Burrows()[i / 5U];
            const float d = std::hypot(burrowCrabs.Crabs()[i].x - hole.x, burrowCrabs.Crabs()[i].z - hole.z);
            if (d > 0.8f && d < 2.5f) subject = i;
        }
        Check(subject < burrowCrabs.Crabs().size(), "a crab starts within reach of its colony's burrow");
        const island::CrabBurrow hole = burrowCrabs.Burrows()[subject / 5U];
        const island::CrabState start = burrowCrabs.Crabs()[subject];
        const float dx = start.x - hole.x, dz = start.z - hole.z, d = std::max(std::hypot(dx, dz), 0.001f);
        const float threatX = start.x + dx / d * 2.0f, threatZ = start.z + dz / d * 2.0f;
        bool hid = false;
        for (int frame = 0; frame < 360 && !hid; ++frame) {
            burrowCrabs.Update(1.0f / 60.0f, threatX, start.y + 1.5f, threatZ);
            const auto& crab = burrowCrabs.Crabs()[subject];
            hid = crab.behavior == island::CrabBehavior::Hide && crab.hideAmount > 0.9f
                && std::hypot(crab.x - hole.x, crab.z - hole.z) < 0.3f
                && crab.y < terrain.Height(crab.x, crab.z) - 0.05f;
        }
        Check(hid && burrowCrabs.HideCount() >= 1U && burrowCrabs.HiddenCount() >= 1U,
              "a frightened crab bolts for its colony's burrow and goes down the hole within six seconds");
        // The eyestalks: down the hole they lie folded flat.
        Check(burrowCrabs.Crabs()[subject].eyeFold > 0.9f && burrowCrabs.EyeFoldedCount() >= 1U,
              "a crab down its burrow does not fold its eyestalks");
        bool emerged = false;
        for (int frame = 0; frame < 900 && !emerged; ++frame) {
            burrowCrabs.Update(1.0f / 60.0f, start.x, start.y + 1.5f, start.z + 40.0f);
            const auto& crab = burrowCrabs.Crabs()[subject];
            emerged = crab.behavior != island::CrabBehavior::Hide && crab.hideAmount <= 0.0f;
        }
        Check(emerged, "with the beach clear the hidden crab comes up again within fifteen seconds");
        // Up again and standing a while, the eyestalks are raised.
        for (int frame = 0; frame < 120; ++frame) burrowCrabs.Update(1.0f / 60.0f, start.x, start.y + 1.5f, start.z + 40.0f);
        Check(burrowCrabs.Crabs()[subject].hideAmount < 0.05f && burrowCrabs.Crabs()[subject].eyeFold < 0.1f,
              "a crab up from its burrow does not raise its eyestalks again");
        // The feed and its bubbles: watched from twenty metres, within four minutes a crab
        // pauses among the strandings to feed, and the bubble field blows a bubble at its
        // mouth that rises and pops within the second.
        {
            island::CrabColony feeders(terrain);
            island::CrabBubbleField bubbles;
            const auto& watchHome = feeders.Homes()[0];
            bool fed = false, blown = false, popped = false; std::size_t feeder = 0;
            for (int step = 0; step < 4800 && !(fed && blown); ++step) {
                feeders.Update(0.05f, watchHome.x + 20.0f, terrain.Height(watchHome.x, watchHome.z) + 1.6f, watchHome.z);
                bubbles.Update(0.05f, feeders.Crabs());
                for (std::size_t i = 0; i < feeders.Crabs().size(); ++i)
                    if (feeders.Crabs()[i].feeding > 0.9f && feeders.Crabs()[i].behavior == island::CrabBehavior::Pause) { fed = true; feeder = i; }
                if (fed && bubbles.BubbleCount() >= 1U && bubbles.ActiveCount() >= 1U) {
                    for (const auto& puff : bubbles.Puffs())
                        if (puff.active && std::hypot(puff.x - feeders.Crabs()[feeder].x, puff.z - feeders.Crabs()[feeder].z) < 0.6f) blown = true;
                }
            }
            Check(fed && feeders.FeedingCount() >= 1U && blown, "a crab feeding among the strandings blows no bubble at its mouth");
            for (int step = 0; step < 24; ++step) bubbles.Update(0.05f, std::span<const island::CrabState>());
            popped = bubbles.ActiveCount() == 0U;
            Check(popped, "a crab's bubble does not pop within the second");
        }
        bool left = false;
        for (int frame = 0; frame < 1800 && !left; ++frame) {
            burrowCrabs.Update(1.0f / 60.0f, start.x, start.y + 1.5f, start.z + 40.0f);
            const auto& crab = burrowCrabs.Crabs()[subject];
            left = std::hypot(crab.x - hole.x, crab.z - hole.z) > 0.8f;
        }
        Check(left, "the emerged crab leaves the burrow mouth for its bouts within half a minute");
    }

    island::FishSchools fishSchools(terrain);
    island::FishSchools repeatedFishSchools(terrain);
    island::FishSchools invalidInputFish(terrain);
    invalidInputFish.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputFish.DormantSimulationCount()
              == static_cast<int>(island::FishSchools::FishCount)
              && std::ranges::all_of(invalidInputFish.Fish(), [](const auto& state) {
                     return std::isfinite(state.x) && std::isfinite(state.y)
                         && std::isfinite(state.z) && std::isfinite(state.headingRadians);
                 }),
          "fish simulation rejects non-finite frame and observer input without state poisoning");
    Check(fishSchools.Fish().size() == island::FishSchools::FishCount,
          "fish schools have the intended bounded population");
    Check(fishSchools.Schools().size() == island::FishSchools::SchoolCount,
          "fish navigation exposes every authoritative school centre");
    for (std::size_t index = 0; index < fishSchools.Schools().size(); ++index) {
        const island::FishSchoolState& school = fishSchools.Schools()[index];
        const island::FishSchoolState& repeated = repeatedFishSchools.Schools()[index];
        const island::TerrainSample sample = terrain.Sample(school.centreX, school.centreZ);
        Check(school.centreX == repeated.centreX && school.centreY == repeated.centreY
                  && school.centreZ == repeated.centreZ
                  && school.direction == repeated.direction,
              "fish school centres initialize deterministically");
        Check(std::isfinite(school.centreX) && std::isfinite(school.centreY)
                  && std::isfinite(school.centreZ),
              "fish school centre diagnostics remain finite");
        Check((school.direction == -1 || school.direction == 1)
                  && sample.signedShoreDistance <= -2.8f
                  && sample.signedShoreDistance >= -48.2f,
              "fish school centres stay in the authored water belt");
        Check(island::IslandEnvironmentField::AquaticHabitatSuitability(
                  school.centreX, school.centreZ,
                  sample.signedShoreDistance, sample.height) >= .04f,
              "fish school centres initialize in shared reef or seagrass habitat");
    }
    std::array<int, island::FishSchools::SchoolCount> fishPerSchool{};
    for (std::size_t index = 0; index < fishSchools.Fish().size(); ++index) {
        const island::FishState& fish = fishSchools.Fish()[index];
        const island::FishState& repeated = repeatedFishSchools.Fish()[index];
        const island::TerrainSample sample = terrain.Sample(fish.x, fish.z);
        Check(fish.x == repeated.x && fish.y == repeated.y && fish.z == repeated.z
                  && fish.headingRadians == repeated.headingRadians
                  && fish.pitchRadians == repeated.pitchRadians
                  && fish.bankRadians == repeated.bankRadians
                  && fish.bodyWave == repeated.bodyWave && fish.speed == repeated.speed
                  && fish.scale == repeated.scale && fish.schoolIndex == repeated.schoolIndex
                  && fish.colorVariant == repeated.colorVariant,
              "fish initialization is deterministic");
        Check(fish.schoolIndex < island::FishSchools::SchoolCount,
              "fish school indices stay in range");
        ++fishPerSchool[fish.schoolIndex];
        Check(sample.signedShoreDistance <= -2.8f
                  && sample.signedShoreDistance >= -48.2f,
              "fish initialize inside the authored near-shore water belt");
        Check(sample.height <= -0.5f && fish.y >= sample.height + 0.20f
                  && fish.y <= -0.20f,
              "fish initialize between the seabed and water surface");
    }
    for (const int population : fishPerSchool) {
        Check(population == static_cast<int>(island::FishSchools::FishPerSchool),
              "each deterministic fish school has the intended population");
    }

    const island::FishState initialLeadFish = fishSchools.Fish().front();
    bool observedFishTailLeft = false;
    bool observedFishTailRight = false;
    bool observedFishPitchUp = false;
    bool observedFishPitchDown = false;
    bool observedFishLeftBank = false;
    bool observedFishRightBank = false;
    constexpr int fishSimulationFrames = 18000;
    for (int frame = 0; frame < fishSimulationFrames; ++frame) {
        const island::FishState observerFish = fishSchools.Fish().front();
        fishSchools.Update(1.0f / 30.0f, observerFish.x, observerFish.y, observerFish.z);
        repeatedFishSchools.Update(1.0f / 30.0f, observerFish.x,
                                   observerFish.y, observerFish.z);
        const auto fishStates = fishSchools.Fish();
        const auto repeatedStates = repeatedFishSchools.Fish();
        const auto schoolStates = fishSchools.Schools();
        const auto repeatedSchoolStates = repeatedFishSchools.Schools();
        for (std::size_t index = 0; index < schoolStates.size(); ++index) {
            const island::FishSchoolState& school = schoolStates[index];
            const island::FishSchoolState& repeated = repeatedSchoolStates[index];
            Check(school.centreX == repeated.centreX
                      && school.centreY == repeated.centreY
                      && school.centreZ == repeated.centreZ
                      && school.direction == repeated.direction
                      && school.simulationTier == repeated.simulationTier,
                  "fish school navigation remains deterministic over a long session");
            const auto schoolSample = terrain.Sample(school.centreX, school.centreZ);
            Check(island::IslandEnvironmentField::AquaticHabitatSuitability(
                      school.centreX, school.centreZ, schoolSample.signedShoreDistance,
                      schoolSample.height) >= .04f,
                  "fish school routes remain tied to shared reef or seagrass habitat");
        }
        for (std::size_t index = 0; index < fishStates.size(); ++index) {
            const island::FishState& fish = fishStates[index];
            const island::FishState& repeated = repeatedStates[index];
            const island::TerrainSample sample = terrain.Sample(fish.x, fish.z);
            Check(fish.x == repeated.x && fish.y == repeated.y && fish.z == repeated.z
                      && fish.headingRadians == repeated.headingRadians
                      && fish.pitchRadians == repeated.pitchRadians
                      && fish.bankRadians == repeated.bankRadians
                      && fish.bodyWave == repeated.bodyWave
                      && fish.simulationTier == repeated.simulationTier,
                  "fish simulation remains deterministic over a long session");
            Check(std::isfinite(fish.x) && std::isfinite(fish.y)
                      && std::isfinite(fish.z) && std::isfinite(fish.headingRadians)
                      && std::isfinite(fish.pitchRadians)
                      && std::isfinite(fish.bankRadians),
                  "fish pose remains finite over a long session");
            Check(std::fabs(fish.pitchRadians) <= 0.221f
                      && std::fabs(fish.bankRadians) <= 0.281f,
                  "fish pitch and bank remain inside restrained pose bounds");
            Check(sample.signedShoreDistance <= -2.8f
                      && sample.signedShoreDistance >= -48.2f,
                  "fish remain inside the authored near-shore water belt");
            Check(sample.height <= -0.5f && fish.y >= sample.height + 0.18f
                      && fish.y <= -0.18f,
                  "fish remain below the surface and above the seabed");
            observedFishTailLeft = observedFishTailLeft || fish.bodyWave > 0.65f;
            observedFishTailRight = observedFishTailRight || fish.bodyWave < -0.65f;
            observedFishPitchUp = observedFishPitchUp || fish.pitchRadians > 0.035f;
            observedFishPitchDown = observedFishPitchDown || fish.pitchRadians < -0.035f;
            observedFishLeftBank = observedFishLeftBank || fish.bankRadians < -0.035f;
            observedFishRightBank = observedFishRightBank || fish.bankRadians > 0.035f;
        }
    }
    const island::FishState finalLeadFish = fishSchools.Fish().front();
    Check(std::hypot(finalLeadFish.x - initialLeadFish.x,
                     finalLeadFish.z - initialLeadFish.z) > 20.0f,
          "fish schools traverse a meaningful section of shoreline over time");
    Check(observedFishTailLeft && observedFishTailRight,
          "fish animation visits both tail-stroke extremes");
    Check(observedFishPitchUp && observedFishPitchDown,
          "fish schools pitch into both rising and descending depth changes");
    Check(observedFishLeftBank && observedFishRightBank,
          "fish schools bank through turns in both directions");
    Check(fishSchools.FullSimulationCount() + fishSchools.ReducedSimulationCount()
              + fishSchools.DormantSimulationCount()
              == static_cast<int>(island::FishSchools::FishCount),
          "fish simulation tiers account for every school member");

    island::SharkPatrol shark(terrain);
    island::SharkPatrol repeatedShark(terrain);
    island::SharkPatrol invalidInputShark(terrain);
    invalidInputShark.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputShark.State().simulationTier
              == island::WildlifeSimulationTier::Dormant
              && std::isfinite(invalidInputShark.State().x)
              && std::isfinite(invalidInputShark.State().y)
              && std::isfinite(invalidInputShark.State().z)
              && std::isfinite(invalidInputShark.State().headingRadians),
          "shark simulation rejects non-finite frame and observer input without state poisoning");
    Check(shark.State().present && shark.State().visible
              && shark.State().activity == 1.0f,
          "shark begins with a bounded visible observation window");
    Check(shark.State().x == repeatedShark.State().x
              && shark.State().y == repeatedShark.State().y
              && shark.State().z == repeatedShark.State().z
              && shark.State().headingRadians == repeatedShark.State().headingRadians
              && shark.State().pitchRadians == repeatedShark.State().pitchRadians
              && shark.State().bankRadians == repeatedShark.State().bankRadians
              && shark.State().targetX == repeatedShark.State().targetX
              && shark.State().targetZ == repeatedShark.State().targetZ,
          "shark patrol initialization is deterministic");
    {
        // The fin's wake: a shark riding just under the surface lays foam at the fin every
        // fifth of a second, the puffs on the surface behind it, gone within three seconds
        // once it dives; the patrol's own first nine seconds ride high enough to cut it.
        island::FinWakeField wake;
        island::SharkState rider;
        rider.y = -0.85f; rider.scale = 2.8f;
        for (int step = 0; step < 40; ++step) { rider.x += 2.0f * 0.05f; wake.Update(0.05f, rider); }
        Check(wake.ReleaseCount() >= 8U && wake.ReleaseCount() <= 12U && wake.ActiveCount() >= 8U, "the riding shark lays a foam puff every fifth of a second");
        bool behind = true, afloat = true;
        for (const auto& puff : wake.Puffs()) if (puff.active) {
            if (puff.x > rider.x) behind = false;
            if (std::abs(puff.y) > 0.1f) afloat = false;
        }
        Check(behind && afloat, "the wake lies on the surface behind the fin");
        rider.y = -2.4f;
        const std::uint32_t laid = wake.ReleaseCount();
        for (int step = 0; step < 70; ++step) { rider.x += 2.0f * 0.05f; wake.Update(0.05f, rider); }
        Check(wake.ReleaseCount() == laid && wake.ActiveCount() == 0U, "a shark cruising deep cuts no wake and the foam is gone within three and a half seconds");
        island::SharkPatrol patrol(terrain);
        island::FinWakeField patrolWake;
        bool cut = false;
        for (int step = 0; step < 160; ++step) {
            patrol.Update(0.05f, patrol.State().x + 20.0f, 0.0f, patrol.State().z);
            patrolWake.Update(0.05f, patrol.State());
            cut = cut || island::FinWakeField::Cutting(patrol.State());
        }
        Check(cut && patrolWake.ReleaseCount() >= 1U, "the patrol's first seconds ride high enough to cut the water and lay a wake");
    }
    {
        // Seabed shadows: a fish a metre over the sandy bottom casts a shadow down the sun's
        // line onto ground under the sea; one on the bottom, one far above it, and one over
        // land cast none here.
        island::FishSchools shoal(terrain);
        const auto& fish = shoal.Fish()[0];
        const float bottom = terrain.Height(fish.x, fish.z);
        const auto cast = island::SeabedShadowProjection::Project(terrain, fish.x, bottom + 1.0f, fish.z);
        Check(bottom < 0.0f && cast.valid && std::abs(cast.height - 1.0f) < 0.4f
                  && terrain.Height(cast.x, cast.z) < 0.0f && std::hypot(cast.x - fish.x, cast.z - fish.z) > 0.3f,
              "a fish a metre over the seabed casts a shadow down the sun's line onto the bottom");
        Check(!island::SeabedShadowProjection::Project(terrain, fish.x, bottom + 0.05f, fish.z).valid
                  && !island::SeabedShadowProjection::Project(terrain, fish.x, bottom + 12.0f, fish.z).valid,
              "a fish on the bottom or far above it casts no seabed shadow");
        const float landX = 0.0f, landZ = 0.0f;
        Check(terrain.Height(landX, landZ) > 0.0f && !island::SeabedShadowProjection::Project(terrain, landX, terrain.Height(landX, landZ) + 1.0f, landZ).valid,
              "ground above the sea takes no seabed shadow");
        // Under the water the light bends toward the vertical: the afternoon sun's 0.69 m of
        // run per metre of fall becomes about 0.47 m, a fifteen-degree sun's 3.7 m about 1.05 m,
        // and the seabed shadow falls nearer under the fish than the unrefracted line put it.
        const auto afternoon = island::SeabedShadowProjection::Refracted(-0.45f, 0.82f, -0.34f);
        const float lowElevation = 15.0f * std::numbers::pi_v<float> / 180.0f;
        const auto evening = island::SeabedShadowProjection::Refracted(std::cos(lowElevation), std::sin(lowElevation), 0.0f);
        const float afternoonRun = std::hypot(afternoon[0], afternoon[1]);
        const auto refracted = island::SeabedShadowProjection::Project(terrain, fish.x, bottom + 1.0f, fish.z, afternoon[0], afternoon[1]);
        Check(std::abs(afternoonRun - 0.47f) < 0.02f && afternoon[0] < 0.0f && afternoon[1] < 0.0f
                  && evening[0] > 1.0f && evening[0] < 1.4f && std::abs(evening[1]) < 1e-5f
                  && refracted.valid && std::hypot(refracted.x - fish.x, refracted.z - fish.z) < std::hypot(cast.x - fish.x, cast.z - fish.z),
              "the light bends toward the vertical under the water, and the seabed shadow falls nearer under the fish");
    }
    bool observedPresentShark = false;
    bool observedHiddenShark = false;
    bool observedSharkTailLeft = false;
    bool observedSharkTailRight = false;
    bool observedSharkPitchUp = false;
    bool observedSharkPitchDown = false;
    bool observedSharkBank = false;
    float sharkTravelDistance = 0.0f;
    island::SharkState previousShark = shark.State();
    constexpr int sharkSimulationFrames = 36000;
    for (int frame = 0; frame < sharkSimulationFrames; ++frame) {
        const island::SharkState observerShark = shark.State();
        shark.Update(1.0f / 30.0f, observerShark.x, observerShark.y, observerShark.z);
        repeatedShark.Update(1.0f / 30.0f, observerShark.x,
                             observerShark.y, observerShark.z);
        const island::SharkState& state = shark.State();
        const island::SharkState& repeated = repeatedShark.State();
        const island::TerrainSample sample = terrain.Sample(state.x, state.z);
        Check(state.x == repeated.x && state.y == repeated.y && state.z == repeated.z
                  && state.headingRadians == repeated.headingRadians
                  && state.pitchRadians == repeated.pitchRadians
                  && state.bankRadians == repeated.bankRadians
                  && state.bodyWave == repeated.bodyWave
                  && state.activity == repeated.activity
                  && state.targetX == repeated.targetX
                  && state.targetZ == repeated.targetZ
                  && state.present == repeated.present && state.visible == repeated.visible
                  && state.simulationTier == repeated.simulationTier,
              "shark patrol remains deterministic over a long session");
        Check(std::isfinite(state.x) && std::isfinite(state.y)
                  && std::isfinite(state.z) && std::isfinite(state.headingRadians)
                  && std::isfinite(state.pitchRadians)
                  && std::isfinite(state.bankRadians)
                  && std::isfinite(state.targetX) && std::isfinite(state.targetZ),
              "shark pose remains finite over a long session");
        Check(std::fabs(state.pitchRadians) <= 0.181f
                  && std::fabs(state.bankRadians) <= 0.241f,
              "shark pitch and bank remain inside restrained pose bounds");
        const island::TerrainSample targetSample = terrain.Sample(
            state.targetX, state.targetZ);
        Check(targetSample.signedShoreDistance <= -33.8f
                  && targetSample.signedShoreDistance >= -88.2f,
              "shark route target remains in the authored offshore patrol belt");
        Check(sample.signedShoreDistance <= -33.8f
                  && sample.signedShoreDistance >= -88.2f,
              "shark remains in the authored offshore patrol belt");
        // The body stays under the surface even in the finning window, when the centre
        // rides at -0.85 m so only the dorsal fin (tip 1.12 m above centre) cuts the water.
        Check(sample.height <= -2.95f && state.y >= sample.height + 0.70f
                  && state.y <= -0.80f,
              "shark remains below the surface and above the offshore seabed");
        Check(state.activity >= 0.0f && state.activity <= 1.0f,
              "shark activity blend remains normalized");
        sharkTravelDistance += std::hypot(state.x - previousShark.x,
                                          state.z - previousShark.z);
        previousShark = state;
        observedPresentShark = observedPresentShark
            || (state.present && state.activity > 0.95f && state.visible);
        observedHiddenShark = observedHiddenShark
            || (!state.present && state.activity < 0.035f && !state.visible);
        observedSharkTailLeft = observedSharkTailLeft || state.bodyWave > 0.65f;
        observedSharkTailRight = observedSharkTailRight || state.bodyWave < -0.65f;
        observedSharkPitchUp = observedSharkPitchUp || state.pitchRadians > 0.035f;
        observedSharkPitchDown = observedSharkPitchDown || state.pitchRadians < -0.035f;
        observedSharkBank = observedSharkBank || std::fabs(state.bankRadians) > 0.018f;
    }
    Check(shark.PresenceCycleCount() >= 8U,
          "shark alternates through several bounded presence cycles in twenty minutes");
    Check(observedPresentShark && observedHiddenShark,
          "shark reaches both fully present and fully hidden states");
    Check(observedSharkTailLeft && observedSharkTailRight,
          "shark animation visits both tail-stroke extremes");
    Check(observedSharkPitchUp && observedSharkPitchDown,
          "shark pitches through both dive and return transitions");
    Check(observedSharkBank,
          "shark visibly banks while following its varied offshore patrol");
    Check(sharkTravelDistance > 700.0f,
          "shark patrol covers a meaningful offshore route over time");

    const island::VegetationLayout vegetation(terrain);
    const island::JungleLayout jungle(terrain);
    const island::CanopyLightField canopyLight(terrain, island::MakeCanopyOccluders(jungle.Canopy()));
    {
        // The sun shade field: the jungle's solid casters land their shadows down the sun
        // line; a receiver standing in one is darkened through the canopy light field's
        // receiver sample while the ground sample stays untouched.
        island::SunShadeField shade(terrain, jungle);
        Check(shade.CasterCount() >= jungle.StandingSnags().size()
                  && shade.CasterCount() <= jungle.StandingSnags().size() + jungle.Understory().size() + jungle.Bushes().size(),
              "the sun shade field bakes the jungle's snags, understory and bushes");
        const float slopeX = island::environment::SunDirectionX / island::environment::SunDirectionY;
        const float slopeZ = island::environment::SunDirectionZ / island::environment::SunDirectionY;
        const float groundY = terrain.Height(0.0f, 0.0f);
        const float before = shade.Cover(-slopeX * 5.0f, -slopeZ * 5.0f);
        shade.AddDisc(0.0f, groundY + 5.0f, 0.0f, 3.0f, 1.0f);
        const float landX = -slopeX * 5.0f, landZ = -slopeZ * 5.0f;
        Check(shade.Cover(landX, landZ) > 0.9f && shade.Cover(landX, landZ) >= before,
              "a disc five metres up shades the ground where its sun line lands");
        Check(shade.Cover(landX + 12.0f, landZ + 12.0f) < 0.6f || before > 0.5f,
              "the disc's shadow stays within its reach");
        Check(shade.Cover(-290.0f, -290.0f) == 0.0f && shade.Cover(1.0e9f, 0.0f) == 0.0f,
              "open sea and points off the grid carry no shade");
        Check(shade.CoverAt(0.0f, groundY + 2.5f, 0.0f, groundY) > 0.9f,
              "a receiver under the disc reads its shade through its own sun line");
        island::CanopyLightField lit(terrain, {}, 64);
        const float open = lit.SampleReceiver(landX, groundY + 0.5f, landZ, groundY);
        Check(open > 0.99f, "with no crowns and no shade field the receiver is fully lit");
        lit.SetSunShade(&shade);
        const float shaded = lit.SampleReceiver(landX, groundY + 0.5f, landZ, groundY);
        Check(shaded < 0.45f && shaded > 0.2f, "with the shade field set a receiver in the disc's shadow darkens by the shade depth");
        Check(lit.Sample(landX, landZ) > 0.99f, "the ground sample ignores the solid casters' shade");
        lit.SetSunShade(nullptr);
        Check(lit.SampleReceiver(landX, groundY + 0.5f, landZ, groundY) > 0.99f, "clearing the shade field restores the open reading");
    }
    const island::GroundCoverField groundCover;
    const island::NaturalPropLayout props(terrain, vegetation, jungle);
    std::vector<island::CrabHabitatCue> crabDebrisCues;
    for (const auto& wood : props.Driftwood()) crabDebrisCues.push_back({wood.x, wood.z});
    for (const auto& detail : props.BeachDetails())
        if (detail.variant == 3U) crabDebrisCues.push_back({detail.x, detail.z});
    island::CrabColony debrisCrabs(terrain), repeatedDebrisCrabs(terrain);
    debrisCrabs.SetDebrisHabitats(crabDebrisCues);
    repeatedDebrisCrabs.SetDebrisHabitats(crabDebrisCues);
    Check(debrisCrabs.Burrows().size() == 6U
              && repeatedDebrisCrabs.Burrows().size() == 6U,
          "six crab colonies expose one visible shared burrow each");
    for (std::size_t colony = 0; colony < debrisCrabs.Burrows().size(); ++colony) {
        const auto& burrow = debrisCrabs.Burrows()[colony];
        const auto& repeated = repeatedDebrisCrabs.Burrows()[colony];
        Check(burrow.x == repeated.x && burrow.y == repeated.y
                  && burrow.z == repeated.z && burrow.radius == repeated.radius,
              "crab burrows derive deterministically from the debris-linked homes");
        Check(std::isfinite(burrow.x) && std::isfinite(burrow.y)
                  && std::isfinite(burrow.z) && burrow.radius >= .24f
                  && burrow.radius <= .31f
                  && std::fabs(burrow.y - terrain.Height(burrow.x, burrow.z) - .012f)
                      < .001f,
              "crab burrows remain small and grounded on their beach section");
        for (std::size_t member = 0; member < 5U; ++member) {
            const auto& home = debrisCrabs.Homes()[colony * 5U + member];
            Check(std::hypot(home.x - burrow.x, home.z - burrow.z) < 2.5f,
                  "each crab home remains beside its colony burrow");
        }
    }
    for (std::size_t index = 0; index < debrisCrabs.Homes().size(); ++index) {
        const auto home = debrisCrabs.Homes()[index];
        Check(home.x == repeatedDebrisCrabs.Homes()[index].x
                  && home.z == repeatedDebrisCrabs.Homes()[index].z,
              "debris-guided crab colony placement is deterministic");
        Check(island::CrabColony::IsNearDebris(home, crabDebrisCues),
              "crab colony home is not beside driftwood or dry wrack");
        const std::size_t first=(index/5U)*5U;
        for(std::size_t previous=first;previous<index;++previous)
            Check(std::hypot(home.x-debrisCrabs.Homes()[previous].x,
                             home.z-debrisCrabs.Homes()[previous].z)>=1.119f,
                  "debris-guided crab homes keep broad carapaces separated");
    }
    const island::ShipwreckLayout shipwreck(terrain);
    const island::ShipwreckLayout repeatedShipwreck(terrain);
    const island::BirdPerchLayout birdPerches(terrain, jungle, props, shipwreck);
    const island::BirdPerchLayout repeatedBirdPerches(
        terrain, jungle, props, repeatedShipwreck);
    Check(birdPerches.Sites().size() == island::BirdPerchLayout::SiteCount
              && repeatedBirdPerches.Sites().size() == birdPerches.Sites().size(),
          "bird perch layout derives the bounded requested world-anchor inventory");
    std::array<int, 3> perchKinds{};
    for (std::size_t index = 0; index < birdPerches.Sites().size(); ++index) {
        const auto& site = birdPerches.Sites()[index];
        const auto& repeatedSite = repeatedBirdPerches.Sites()[index];
        Check(site.x == repeatedSite.x && site.y == repeatedSite.y
                  && site.z == repeatedSite.z
                  && site.headingRadians == repeatedSite.headingRadians
                  && site.kind == repeatedSite.kind,
              "bird world perch anchors derive deterministically");
        Check(site.y >= terrain.Height(site.x, site.z) + 0.35f,
              "bird world perch anchor remains above terrain");
        ++perchKinds[static_cast<std::size_t>(site.kind)];
    }
    Check(perchKinds[0] == 5 && perchKinds[1] == 2 && perchKinds[2] == 1,
          "bird perch layout includes five branches, two rocks and one wreck beam");
    const island::ShipwreckPlacement& ship = shipwreck.Ship();
    const island::ShipwreckPlacement& repeatedShip = repeatedShipwreck.Ship();
    std::vector<island::ContactFootprint> coastalFootprints;
    for (const auto& rock : props.Rocks()) {
        if (!rock.offshore) continue;
        const auto footprint = island::RockContactFootprint(rock);
        CheckNear(footprint.directionZ, -std::sin(rock.yawRadians), 0.00001f,
                  "rock shadow follows the mesh's local X rotation convention");
        coastalFootprints.push_back(footprint);
    }
    const auto hullFootprints = island::HullContactFootprints(ship);
    const auto& aftShadow = hullFootprints[0];
    const auto& bowShadow = hullFootprints[1];
    const float hullGap = std::hypot(aftShadow.x - bowShadow.x, aftShadow.z - bowShadow.z)
        - aftShadow.alongRadius - bowShadow.alongRadius;
    Check(hullGap > 2.0f && hullGap < 2.5f, "contact shadows preserve the open hull break");
    CheckNear(aftShadow.directionX, std::sin(ship.yawRadians), 0.00001f,
              "hull contact shadow follows the mesh's local Z convention");
    coastalFootprints.insert(coastalFootprints.end(), hullFootprints.begin(), hullFootprints.end());
    for (const auto& debris : shipwreck.Debris()) {
        coastalFootprints.push_back(island::DebrisContactFootprint(debris));
    }
    Check(coastalFootprints.size() == 28, "all 14 reef rocks, two hull halves and 12 fragments cast grounding patches");
    std::vector<island::ContactFootprint> landFootprints;
    for (const auto& rock : props.Rocks()) {
        if (!rock.offshore) landFootprints.push_back(island::LandRockContactFootprint(rock));
    }
    for (const auto& log : props.FallenLogs()) landFootprints.push_back(island::LogContactFootprint(log));
    for (const auto& wood : props.Driftwood()) landFootprints.push_back(island::DriftwoodContactFootprint(wood));
    Check(landFootprints.size() == static_cast<std::size_t>(std::count_if(
              props.Rocks().begin(), props.Rocks().end(), [](const auto& rock) { return !rock.offshore; }))
              + props.FallenLogs().size() + props.Driftwood().size(),
          "all land rocks, logs and driftwood pieces retain their shadows");
    for (float sign : {-1.0f, 1.0f}) {
        island::RockPlacement rock;
        island::FallenLogPlacement log;
        island::DriftwoodPlacement wood;
        rock.yawRadians = log.yawRadians = wood.yawRadians = sign*std::numbers::pi_v<float>/4;
        for (const auto& footprint : {island::LogContactFootprint(log), island::DriftwoodContactFootprint(wood)}) {
            CheckNear(footprint.directionX, std::sqrt(0.5f), 0.000001f,
                      "positive local-X shadow axis follows the model's 45-degree rotation");
            CheckNear(footprint.directionZ, sign*std::sqrt(0.5f), 0.000001f,
                      "wood shadows retain placement and collision heading");
            CheckNear(footprint.directionZ, -std::sin(island::WoodModelYaw(log.yawRadians)), 0.000001f,
                      "wood renderer converts its heading to the matrix rotation convention");
        }
        const auto rockFootprint = island::LandRockContactFootprint(rock);
        CheckNear(rockFootprint.directionZ, -sign*std::sqrt(0.5f), 0.000001f,
                  "land rock shadow retains its distinct model rotation convention");
        CheckNear(rockFootprint.z, -sign*std::sqrt(0.5f)*0.08f, 0.000001f,
                  "land rock shadow offset rotates with its corrected axis");
    }
    for (const auto& log : props.FallenLogs()) {
        const float modelYaw = island::WoodModelYaw(log.yawRadians);
        for (int sample = 0; sample < island::NaturalPropLayout::LogCollisionSamples; ++sample) {
            const float t = static_cast<float>(sample) / (island::NaturalPropLayout::LogCollisionSamples-1) - 0.5f;
            const float x = log.x + std::cos(modelYaw)*log.length*t;
            const float z = log.z - std::sin(modelYaw)*log.length*t;
            Check(std::any_of(props.Obstacles().begin(), props.Obstacles().end(), [&](const auto& obstacle) {
                return std::hypot(obstacle.x-x,obstacle.z-z) < 0.00001f
                    && std::abs(obstacle.radius-log.radius*0.92f) < 0.00001f;
            }), "every rendered log centreline collision sample matches an existing physical proxy");
        }
    }
    // Independent rendered-grid interpolation, including the top-right/bottom-left diagonal.
    const auto renderedHeight = [&](float x, float z) {
        const auto coordinate = [](float value) {
            return std::clamp(static_cast<int>(std::floor((value + island::TerrainMeshLayout::Extent)
                * island::TerrainMeshLayout::QuadsPerAxis / (2 * island::TerrainMeshLayout::Extent))),
                0, island::TerrainMeshLayout::QuadsPerAxis - 1);
        };
        const int column = coordinate(x), row = coordinate(z);
        const float x0 = island::TerrainMeshLayout::Coordinate(column);
        const float x1 = island::TerrainMeshLayout::Coordinate(column+1);
        const float z0 = island::TerrainMeshLayout::Coordinate(row);
        const float z1 = island::TerrainMeshLayout::Coordinate(row+1);
        const float u = (x-x0)/(x1-x0), v = (z-z0)/(z1-z0);
        if (u+v <= 1) return terrain.Height(x0,z0)*(1-u-v) + terrain.Height(x1,z0)*u + terrain.Height(x0,z1)*v;
        return terrain.Height(x1,z1)*(u+v-1) + terrain.Height(x1,z0)*(1-v) + terrain.Height(x0,z1)*(1-u);
    };
    const auto validateContactPatches = [&](const auto& footprints, const island::ContactPatchDetail detail) {
      std::size_t vertexCount = 0;
      for (const auto& footprint : footprints) {
        const auto patch = island::BuildContactPatch(terrain, footprint, detail);
        Check(!patch.empty() && patch.size() % 3 == 0 && patch.size() < 9000,
              "coastal contact geometry is nonempty, triangular and bounded");
        vertexCount += patch.size();
        double area = 0;
        float maximumOpacity = 0;
        for (const auto& vertex : patch) {
            Check(std::isfinite(vertex.y) && vertex.opacity >= 0 && vertex.opacity <= footprint.opacity + 0.000001f,
                  "coastal patch upload is finite with bounded opacity");
            maximumOpacity = std::max(maximumOpacity, vertex.opacity);
            const float dx = vertex.x - footprint.x, dz = vertex.z - footprint.z;
            const float u = (dx*footprint.directionX + dz*footprint.directionZ) / footprint.alongRadius;
            const float v = (-dx*footprint.directionZ + dz*footprint.directionX) / footprint.acrossRadius;
            Check(u*u+v*v <= 1.001f, "clipped geometry stays inside its grounding footprint");
            CheckNear(vertex.y, renderedHeight(vertex.x,vertex.z) + island::ContactPatchSurfaceOffset,
                      0.0001f, "coastal patch vertices follow the rendered terrain, not analytic-height mismatches");
        }
        for (std::size_t index = 0; index < patch.size(); index += 3) {
            const auto& a = patch[index]; const auto& b = patch[index+1]; const auto& c = patch[index+2];
            area += std::abs((static_cast<double>(b.x)-a.x)*(c.z-a.z) - (static_cast<double>(b.z)-a.z)*(c.x-a.x)) * 0.5;
            CheckNear((a.y+b.y+c.y)/3, renderedHeight((a.x+b.x+c.x)/3,(a.z+b.z+c.z)/3)
                          + island::ContactPatchSurfaceOffset, 0.00015f,
                      "shadow triangle interiors remain on one terrain plane");
        }
        const int sectors = detail == island::ContactPatchDetail::Compact ? 12 : 24;
        const double expectedArea = sectors * 0.5 * std::sin(2 * std::numbers::pi / sectors)
            * footprint.alongRadius * footprint.acrossRadius;
        Check(std::abs(area - expectedArea) < expectedArea * 0.001,
              "terrain clipping preserves footprint area without holes or duplicate coverage");
        CheckNear(maximumOpacity, footprint.opacity, 0.000001f, "every grounding patch retains its contact core");
      }
      return vertexCount;
    };
    const auto coastalVertexCount = validateContactPatches(coastalFootprints, island::ContactPatchDetail::Coastal);
    const auto landVertexCount = validateContactPatches(landFootprints, island::ContactPatchDetail::Compact);
    Check(landVertexCount < 70000, "the complete clipped land-shadow geometry stays within its static budget");
    Check(coastalVertexCount == 23772, "coastal patch geometry matches the authored shoreline layout");
    std::cout << "Land contact patches: " << landFootprints.size() << ", vertices: " << landVertexCount << '\n';
    Check(coastalVertexCount < 50000, "the complete coastal shadow addition fits the static vertex budget");
    std::cout << "Coastal contact patches: " << coastalFootprints.size() << ", vertices: " << coastalVertexCount << '\n';
    for (int invalidCase = 0; invalidCase < 5; ++invalidCase) {
        auto invalid = aftShadow;
        if (invalidCase == 0) invalid.x = std::numeric_limits<float>::quiet_NaN();
        if (invalidCase == 1) invalid.alongRadius = 8.1f;
        if (invalidCase == 2) invalid.acrossRadius = 0;
        if (invalidCase == 3) invalid.directionX = 4;
        if (invalidCase == 4) invalid.opacity = -1;
        bool rejected = false;
        try { (void)island::BuildContactPatch(terrain, invalid); }
        catch (const std::invalid_argument&) { rejected = true; }
        Check(rejected, "invalid contact footprint is rejected before GPU upload");
    }
    Check(ship.length == 20.0f && ship.beam == 5.2f && ship.hullHeight == 4.4f,
          "shipwreck retains the authored large-ship dimensions");
    Check(ship.x == repeatedShip.x && ship.y == repeatedShip.y
              && ship.z == repeatedShip.z && ship.yawRadians == repeatedShip.yawRadians
              && ship.heelRadians == repeatedShip.heelRadians
              && ship.pitchRadians == repeatedShip.pitchRadians,
          "shipwreck placement is bitwise deterministic");
    Check(std::isfinite(ship.x) && std::isfinite(ship.y) && std::isfinite(ship.z)
              && std::isfinite(ship.yawRadians) && std::isfinite(ship.heelRadians)
              && std::isfinite(ship.pitchRadians),
          "shipwreck placement remains finite");
    const float shipShoreDistance = terrain.SignedShoreDistance(ship.x, ship.z);
    Check(shipShoreDistance < -3.0f && shipShoreDistance > -12.0f,
          "large shipwreck is grounded in accessible beach shallows");
    Check(shipwreck.Debris().size() == island::ShipwreckLayout::DebrisCount
              && shipwreck.Obstacles().size()
                  == island::ShipwreckLayout::CollisionProxyCount,
          "shipwreck exposes its complete bounded debris and collision inventory");
    std::array<int, 3> shipwreckDebrisKinds{};
    bool shipwreckDebrisReachesWater = false;
    bool shipwreckDebrisReachesBeach = false;
    int stronglyAngledShipwreckDebris = 0;
    bool shipwreckDebrisAnglesClockwise = false;
    bool shipwreckDebrisAnglesCounterClockwise = false;
    for (std::size_t index = 0; index < shipwreck.Debris().size(); ++index) {
        const island::ShipwreckDebrisPlacement& debris = shipwreck.Debris()[index];
        const island::ShipwreckDebrisPlacement& repeated = repeatedShipwreck.Debris()[index];
        Check(debris.x == repeated.x && debris.y == repeated.y && debris.z == repeated.z
                  && debris.yawRadians == repeated.yawRadians
                  && debris.length == repeated.length && debris.width == repeated.width
                  && debris.kind == repeated.kind,
              "shipwreck debris placement is bitwise deterministic");
        Check(std::isfinite(debris.x) && std::isfinite(debris.y)
                  && std::isfinite(debris.z) && std::isfinite(debris.yawRadians)
                  && debris.length >= 2.0f && debris.length <= 5.6f
                  && debris.width >= 0.20f && debris.width <= 0.50f,
              "shipwreck debris remains finite and visibly substantial");
        const auto kind = static_cast<std::size_t>(debris.kind);
        Check(kind < shipwreckDebrisKinds.size(),
              "shipwreck debris kind remains in renderer range");
        if (kind < shipwreckDebrisKinds.size()) ++shipwreckDebrisKinds[kind];
        const float shoreDistance = terrain.SignedShoreDistance(debris.x, debris.z);
        shipwreckDebrisReachesWater = shipwreckDebrisReachesWater || shoreDistance < -1.0f;
        shipwreckDebrisReachesBeach = shipwreckDebrisReachesBeach || shoreDistance > 2.0f;
        Check(std::hypot(debris.x - ship.x, debris.z - ship.z) < 30.0f,
              "shipwreck debris stays in a coherent beach-directed trail");
        const float debrisAngle = debris.yawRadians - ship.yawRadians;
        Check(std::abs(debrisAngle) <= 0.85f,
              "shipwreck debris remains plausibly aligned with the wreck event");
        if (std::abs(debrisAngle) >= 0.40f) ++stronglyAngledShipwreckDebris;
        shipwreckDebrisAnglesClockwise = shipwreckDebrisAnglesClockwise
            || debrisAngle <= -0.40f;
        shipwreckDebrisAnglesCounterClockwise = shipwreckDebrisAnglesCounterClockwise
            || debrisAngle >= 0.40f;
    }
    Check(shipwreckDebrisKinds[0] == 9 && shipwreckDebrisKinds[1] == 2
              && shipwreckDebrisKinds[2] == 1,
          "shipwreck debris mixes planks, major hull fragments, and one spar");
    Check(shipwreckDebrisReachesWater && shipwreckDebrisReachesBeach,
          "shipwreck debris trail connects the shallow wreck to the beach");
    Check(stronglyAngledShipwreckDebris >= 8 && shipwreckDebrisAnglesClockwise
              && shipwreckDebrisAnglesCounterClockwise,
          "shipwreck debris breaks parallel repetition in both directions");
    for (std::size_t index = 0; index < shipwreck.Obstacles().size(); ++index) {
        const island::CircularObstacle& obstacle = shipwreck.Obstacles()[index];
        const island::CircularObstacle& repeated = repeatedShipwreck.Obstacles()[index];
        Check(obstacle.x == repeated.x && obstacle.z == repeated.z
                  && obstacle.radius == repeated.radius,
              "shipwreck collision proxies are bitwise deterministic");
        Check(std::isfinite(obstacle.x) && std::isfinite(obstacle.z)
                  && obstacle.radius >= 1.3f && obstacle.radius <= 1.8f
                  && std::hypot(obstacle.x - ship.x, obstacle.z - ship.z) <= 8.1f,
              "shipwreck collision proxies cover only the major grounded hull");
    }
    std::vector<island::CircularObstacle> worldObstacles;
    worldObstacles.reserve(vegetation.Obstacles().size() + jungle.Obstacles().size()
                           + props.Obstacles().size() + shipwreck.Obstacles().size());
    worldObstacles.insert(worldObstacles.end(), vegetation.Obstacles().begin(),
                          vegetation.Obstacles().end());
    worldObstacles.insert(worldObstacles.end(), jungle.Obstacles().begin(),
                          jungle.Obstacles().end());
    worldObstacles.insert(worldObstacles.end(), props.Obstacles().begin(),
                          props.Obstacles().end());
    worldObstacles.insert(worldObstacles.end(), shipwreck.Obstacles().begin(),
                          shipwreck.Obstacles().end());
    const island::WorldCollisionGrid worldCollisionGrid(worldObstacles);
    // Narrowing the vegetation reserve must preserve a continuous walking corridor,
    // including collision footprints of rocks, logs and promoted emergent trunks.
    for (int z = -215; z <= 180; ++z) {
        const float centre = island::VegetationLayout::PrimaryPathCenterX(static_cast<float>(z));
        for (const float offset : {-0.65f, 0.0f, 0.65f}) {
            const auto trailSurface = terrain.Sample(centre + offset, static_cast<float>(z));
            if (trailSurface.slopeRadians >= island::PlayerMotion::MaximumWalkSlopeRadians - 0.025f)
                std::cerr << "Trail slope at z=" << z << " offset=" << offset
                          << " is " << trailSurface.slopeRadians << '\n';
            Check(trailSurface.slopeRadians < island::PlayerMotion::MaximumWalkSlopeRadians - 0.025f,
                  "primary trail stays below the player's slope limit with contact margin");
            for (const auto& obstacle : worldObstacles) {
                const bool clear = std::hypot(centre + offset - obstacle.x,
                                              static_cast<float>(z) - obstacle.z)
                    > obstacle.radius + island::PlayerMotion::CapsuleRadius;
                if (!clear) std::cerr << "Trail blocked at z=" << z << " offset=" << offset
                    << " by obstacle " << obstacle.x << ',' << obstacle.z
                    << " radius=" << obstacle.radius << '\n';
                Check(clear,
                      "narrow trail leaves a continuous player-width corridor beside its centre");
            }
        }
    }
    Check(worldCollisionGrid.Obstacles().size() == worldObstacles.size()
              && worldCollisionGrid.CellCount() > 0U,
          "world collision grid indexes the complete static obstacle set");
    bool collisionGridMatchesBruteForce = true;
    std::size_t collisionCandidateTotal = 0U;
    std::size_t collisionQueryCount = 0U;
    for (int row = -20; row <= 20; ++row) {
        for (int column = -20; column <= 20; ++column) {
            const float queryX = static_cast<float>(column) * 13.7f + 0.19f;
            const float queryZ = static_cast<float>(row) * 13.1f - 0.23f;
            const float queryRadius = island::PlayerMotion::CapsuleRadius;
            const bool bruteForceHit = std::ranges::any_of(
                worldObstacles, [queryX, queryZ, queryRadius](const auto& obstacle) {
                    const float combinedRadius = queryRadius
                        + std::max(0.0f, obstacle.radius);
                    const float dx = queryX - obstacle.x;
                    const float dz = queryZ - obstacle.z;
                    return dx * dx + dz * dz < combinedRadius * combinedRadius;
                });
            collisionGridMatchesBruteForce = collisionGridMatchesBruteForce
                && worldCollisionGrid.IntersectsCircle(queryX, queryZ, queryRadius)
                    == bruteForceHit;
            collisionCandidateTotal += worldCollisionGrid.CandidateCount(
                queryX, queryZ, queryRadius);
            ++collisionQueryCount;
        }
    }
    Check(collisionGridMatchesBruteForce,
          "collision broad phase exactly matches exhaustive circle tests across the island");
    Check(collisionCandidateTotal / collisionQueryCount < worldObstacles.size() / 8U,
          "collision broad phase materially reduces the average candidate population");
    {
        // A monkey idle on the ground scratches now and then: with the observer twelve
        // metres off, within a minute one of the troop brings a forelimb up for a bout of
        // about a second and a half, and never while it walks or climbs.
        island::MonkeyTroop scratchers(terrain);
        const auto first = scratchers.Monkeys().front();
        float longest = 0.0f, bout = 0.0f;
        bool movingScratch = false;
        for (int step = 0; step < 1800; ++step) {
            scratchers.Update(1.0f / 30.0f, first.x + 12.0f, first.y, first.z);
            float best = 0.0f;
            for (const auto& monkey : scratchers.Monkeys()) {
                best = std::max(best, monkey.scratchAmount);
                if (monkey.scratchAmount > 0.5f && (monkey.speed > 0.05f || monkey.treeExcursion)) movingScratch = true;
            }
            if (best > 0.5f) bout += 1.0f / 30.0f;
            else { longest = std::max(longest, bout); bout = 0.0f; }
        }
        longest = std::max(longest, bout);
        Check(scratchers.ScratchCount() >= 1U && longest > 0.8f && longest < 3.0f,
              "a monkey idle on the ground scratches for a bout of about a second and a half within a minute");
        Check(!movingScratch, "a walking or climbing monkey never scratches");
        scratchers.PreviewScratch(0);
        Check(scratchers.Monkeys()[0].scratchAmount == 1.0f && scratchers.ScratchingCount() >= 1U,
              "the scratch view's preview holds the subject mid-scratch");
    }
    {
        // After dark the troop finishes what it is doing and settles to rest: within three
        // minutes no monkey is still wandering or walking about (the observer far off).
        island::MonkeyTroop sleepy(terrain);
        sleepy.SetNightness(1.0f);
        for (int step = 0; step < 30 * 180; ++step) sleepy.Update(1.0f / 30.0f, 400.0f, 0.0f, 400.0f);
        bool restless = false;
        for (const auto& monkey : sleepy.Monkeys())
            restless = restless || monkey.behavior == island::MonkeyBehavior::Wander || monkey.behavior == island::MonkeyBehavior::Walk;
        Check(!restless, "after dark the monkeys settle to rest instead of wandering");
    }
    island::MonkeyTroop monkeys(terrain);
    const island::RenderedTerrainSurface monkeyAttachmentTerrain(terrain);
    std::array<bool,island::MonkeyTroop::MonkeyCount> attachmentAttempted{};
    unsigned attachmentCandidates=0,attachmentAccepted=0;
    unsigned excursionAccepted=0;
    unsigned excursionClear=0;
    double attachmentMillis=0,attachmentMaxMillis=0,clearanceMillis=0,clearanceMaxMillis=0;
    double clearanceChunkMaxMillis=0;
    double attachmentChunkMaxMillis=0;
    bool observedPreparing=false;
    std::array<unsigned,island::MonkeyTroop::MonkeyCount> preparationFrames{};
    bool observedLiveClimb=false,observedLiveSummit=false,checkedLiveReturn=false;
    std::array<unsigned,11> attachmentFailures{};
    island::MonkeyTroop repeatedMonkeys(terrain);
    island::MonkeyTroop invalidInputMonkeys(terrain);
    invalidInputMonkeys.Update(quietNan, quietNan, quietNan, quietNan);
    Check(invalidInputMonkeys.DormantSimulationCount()
              == static_cast<int>(island::MonkeyTroop::MonkeyCount)
              && std::ranges::all_of(invalidInputMonkeys.Monkeys(), [](const auto& state) {
                     return std::isfinite(state.x) && std::isfinite(state.y)
                         && std::isfinite(state.z) && std::isfinite(state.animationSeconds);
                 }),
          "monkey simulation rejects non-finite frame and observer input without state poisoning");
    monkeys.SetObstacles(worldObstacles);
    repeatedMonkeys.SetObstacles(worldObstacles);
    monkeys.SetCanopy(jungle.Canopy());
    repeatedMonkeys.SetCanopy(jungle.Canopy());
    Check(monkeys.Monkeys().size() == island::MonkeyTroop::MonkeyCount,
          "monkey troop has the intended bounded population");
    Check(monkeys.Monkeys().size() == repeatedMonkeys.Monkeys().size(),
          "repeated monkey troop has the same population");
    std::array<bool, 4> observedMonkeyVariants{};
    float minimumMonkeyScale = 10.0f;
    float maximumMonkeyScale = 0.0f;
    for (const island::MonkeyState& monkey : monkeys.Monkeys()) {
        Check(island::MonkeyTroop::HomeHasClosedCanopy(
                  jungle.Canopy(),monkey.homeX,monkey.homeZ),
              "monkey home is not beside a clearance-qualified closed-canopy tree");
        Check(terrain.SignedShoreDistance(monkey.homeX,monkey.homeZ)>=90.0f,
              "monkey home is not in the deep interior canopy");
        Check(monkey.colorVariant < observedMonkeyVariants.size(),
              "monkey material variant remains inside the renderer palette");
        if (monkey.colorVariant < observedMonkeyVariants.size()) {
            observedMonkeyVariants[monkey.colorVariant] = true;
        }
        minimumMonkeyScale = std::min(minimumMonkeyScale, monkey.scale);
        maximumMonkeyScale = std::max(maximumMonkeyScale, monkey.scale);
    }
    for (std::size_t first=0;first<monkeys.Monkeys().size();++first) {
        for (std::size_t second=first+1;second<monkeys.Monkeys().size();++second) {
            const auto& a=monkeys.Monkeys()[first];
            const auto& b=monkeys.Monkeys()[second];
            Check(std::hypot(a.homeX-b.homeX,a.homeZ-b.homeZ)>=7.0f,
                  "deep-interior monkey homes are not sufficiently separated");
        }
    }
    Check(std::ranges::all_of(observedMonkeyVariants, [](const bool value) { return value; }),
          "monkey troop exercises all four material palettes");
    Check(maximumMonkeyScale - minimumMonkeyScale >= 0.20f,
          "monkey troop has meaningful individual scale variation");

    island::MonkeyTroop attentiveMonkeys(terrain);
    attentiveMonkeys.SetObstacles(worldObstacles);
    const island::MonkeyState attentionStart = attentiveMonkeys.Monkeys().front();
    const float leftObserverAngle = attentionStart.headingRadians + 0.90f;
    for (int frame = 0; frame < 30; ++frame) {
        attentiveMonkeys.Update(
            1.0f / 30.0f,
            attentionStart.x + std::cos(leftObserverAngle) * 12.0f,
            attentionStart.y + 1.6f,
            attentionStart.z + std::sin(leftObserverAngle) * 12.0f);
    }
    Check(attentiveMonkeys.Monkeys().front().headLookRadians > 0.20f,
          "a calm monkey gradually attends toward a nearby observer on its left");
    const float rightObserverAngle = attentionStart.headingRadians - 0.90f;
    for (int frame = 0; frame < 50; ++frame) {
        attentiveMonkeys.Update(
            1.0f / 30.0f,
            attentionStart.x + std::cos(rightObserverAngle) * 12.0f,
            attentionStart.y + 1.6f,
            attentionStart.z + std::sin(rightObserverAngle) * 12.0f);
    }
    Check(attentiveMonkeys.Monkeys().front().headLookRadians < -0.20f,
          "a calm monkey smoothly redirects attention toward the opposite side");

    island::MonkeyTroop retreatingMonkeys(terrain);
    retreatingMonkeys.SetObstacles(worldObstacles);
    const auto retreatStart = retreatingMonkeys.Monkeys().front();
    float retreatHeading = retreatStart.headingRadians;
    float retreatAngularVelocity = 0.0f;
    bool observedRetreatTurn = false;
    for (int frame = 0; frame < 120; ++frame) {
        const auto before = retreatingMonkeys.Monkeys().front();
        retreatingMonkeys.Update(
            1.0f / 30.0f,
            before.x + std::cos(retreatStart.headingRadians) * 2.0f,
            before.y + 1.2f,
            before.z + std::sin(retreatStart.headingRadians) * 2.0f);
        const auto after = retreatingMonkeys.Monkeys().front();
        const float signedTurn = std::remainder(
            after.headingRadians-retreatHeading,2.0f*std::numbers::pi_v<float>);
        const float turn = std::fabs(signedTurn);
        const float angularVelocity=signedTurn*30.0f;
        Check(turn <= island::MonkeyTroop::MaximumRetreatTurnRate / 30.0f + 0.0001f,
              "a close observer cannot make a ground monkey snap beyond its retreat turn rate");
        const float angularAccelerationStep=std::fabs(
            angularVelocity-retreatAngularVelocity);
        Check(angularAccelerationStep
                  <= island::MonkeyTroop::MaximumGroundTurnAcceleration/30.0f+.001f,
              "a close observer cannot make a ground monkey exceed its angular acceleration bound");
        observedRetreatTurn = observedRetreatTurn || turn > 0.01f;
        retreatAngularVelocity=angularVelocity;
        retreatHeading = after.headingRadians;
    }
    Check(observedRetreatTurn
              && retreatingMonkeys.Monkeys().front().behavior
                  == island::MonkeyBehavior::RunAway,
          "a close observer exercises the monkey's bounded retreat turn");
    // The flight is noted where it began, so the soundscape can give the alarm there.
    Check(retreatingMonkeys.FleeCount() >= 1U && retreatingMonkeys.LastFlee().valid
              && std::isfinite(retreatingMonkeys.LastFlee().x) && std::isfinite(retreatingMonkeys.LastFlee().z)
              && std::hypot(retreatingMonkeys.LastFlee().x - retreatingMonkeys.Monkeys().front().x,
                            retreatingMonkeys.LastFlee().z - retreatingMonkeys.Monkeys().front().z) < 30.0f,
          "a monkey's flight from the walker is noted where it broke into it");
    island::MonkeyTroop calmMonkeys(terrain);
    for (int frame = 0; frame < 60; ++frame) calmMonkeys.Update(1.0f / 30.0f, 1000.0f, 0.0f, 1000.0f, 0.0f);
    Check(calmMonkeys.FleeCount() == 0U, "with the walker far off no monkey breaks into flight");
    {
        // A snake's head two metres from a monkey sends it off with the alarm; one eight
        // metres off is left alone.
        island::MonkeyTroop wary(terrain);
        const auto& first = wary.Monkeys()[0];
        std::vector<island::SnakeState> snakes(1);
        for (auto& point : snakes[0].body) point = {first.x + 8.0f, first.y, first.z};
        wary.SetSnakes(snakes);
        for (int frame = 0; frame < 60; ++frame) wary.Update(1.0f / 30.0f, first.x + 20.0f, first.y + 1.0f, first.z, 0.0f);
        Check(wary.SnakeAlarmCount() == 0U, "a snake eight metres off does not alarm the monkey");
        for (auto& point : snakes[0].body) point = {first.x + 2.0f, first.y, first.z};
        bool fled = false;
        for (int frame = 0; frame < 60 && !fled; ++frame) {
            wary.Update(1.0f / 30.0f, first.x + 20.0f, first.y + 1.0f, first.z, 0.0f);
            fled = first.behavior == island::MonkeyBehavior::RunAway && wary.SnakeAlarmCount() >= 1U;
        }
        Check(fled && wary.FleeCount() >= 1U && wary.LastFlee().valid, "a snake two metres from a monkey sends it off with the alarm within two seconds");
    }

    bool observedMonkeyIdle = false;
    bool observedMonkeyObserve = false;
    bool observedMonkeyWalk = false;
    bool observedMonkeyRunAway = false;
    bool observedMonkeyTreeApproach = false;
    bool observedMonkeyTreeArrival = false;
    bool observedMonkeyFullTier = false;
    bool observedMonkeyReducedTier = false;
    bool observedMonkeyDormantTier = false;
    bool observedMonkeyContinuousClipClock = false;
    float monkeyTravelDistance = 0.0f;
    std::vector<island::MonkeyState> previousMonkeys(
        monkeys.Monkeys().begin(), monkeys.Monkeys().end());
    constexpr int monkeySimulationFrames = 36000;
    for (int frame = 0; frame < monkeySimulationFrames; ++frame) {
        const island::MonkeyState followed = monkeys.Monkeys().front();
        const float observerX = frame < 240 ? followed.x
            : (frame < 12000 ? 0.0f : (frame < 18000 ? 1000.0f : -40.0f));
        const float observerY = frame < 240 ? followed.y + 1.0f : 4.0f;
        const float observerZ = frame < 240 ? followed.z
            : (frame < 12000 ? 0.0f : (frame < 18000 ? 1000.0f : 35.0f));
        monkeys.Update(1.0f / 30.0f, observerX, observerY, observerZ);
        Check(monkeys.PreflightSamplesLastUpdate()<=32,"full-excursion preflight has a troop-wide per-update work bound");
        repeatedMonkeys.Update(1.0f / 30.0f, observerX, observerY, observerZ);
        Check(monkeys.FullSimulationCount() + monkeys.ReducedSimulationCount()
                  + monkeys.DormantSimulationCount()
                  == static_cast<int>(island::MonkeyTroop::MonkeyCount),
              "monkey simulation tiers account for the whole troop");
        observedMonkeyFullTier = observedMonkeyFullTier
            || monkeys.FullSimulationCount() > 0;
        observedMonkeyReducedTier = observedMonkeyReducedTier
            || monkeys.ReducedSimulationCount() > 0;
        observedMonkeyDormantTier = observedMonkeyDormantTier
            || monkeys.DormantSimulationCount() > 0;
        for (std::size_t index = 0; index < monkeys.Monkeys().size(); ++index) {
            const island::MonkeyState& state = monkeys.Monkeys()[index];
            const island::MonkeyState& repeated = repeatedMonkeys.Monkeys()[index];
            if (state.treePreparing) {
                observedPreparing=true;
                ++preparationFrames[index];
                Check(!state.treeExcursion,"pending collision preflight cannot activate tree climbing early");
            } else if (!state.treeExcursion) preparationFrames[index]=0;
            if (state.treeExcursion) {
                Check(preparationFrames[index]>=215,"live climb waits for all attachment and excursion samples");
                observedLiveClimb=true;
                observedLiveSummit=observedLiveSummit||state.treeClock.seconds>=18;
                Check(state.treeIndex>=0 && repeated.treeExcursion
                    && state.treeClock.seconds==repeated.treeClock.seconds,
                    "live monkey climbing preserves tree identity and deterministic clock");
                if (!checkedLiveReturn && state.treeClock.seconds>=12) {
                    checkedLiveReturn=true;
                    auto interrupted=monkeys;
                    interrupted.Update(.1f,state.x,state.y,state.z);
                    Check(interrupted.Monkeys()[index].treeExcursion
                        && interrupted.Monkeys()[index].treeClock.returning,
                        "player proximity requests descent instead of clearing airborne tree grips");
                    bool refused=false;
                    try { interrupted.SetCanopy({}); } catch (const std::logic_error&) { refused=true; }
                    Check(refused && interrupted.Monkeys()[index].treeExcursion,
                        "canopy replacement cannot delete an attached monkey's supporting tree");
                    for (int step=0;step<500 && interrupted.Monkeys()[index].treeIndex>=0;++step)
                        interrupted.Update(.1f,state.x,state.y,state.z);
                    Check(!interrupted.Monkeys()[index].treeExcursion
                        && !interrupted.Monkeys()[index].treeReleasing
                        && interrupted.Monkeys()[index].treeIndex<0,
                        "interrupted live monkey descends and releases grounded grips before fleeing");
                }
            }
            if (state.behavior!=island::MonkeyBehavior::InspectTree) attachmentAttempted[index]=false;
            if (state.behavior==island::MonkeyBehavior::InspectTree && state.treeInspectionSeconds>=3.2f
                && !attachmentAttempted[index] && attachmentCandidates<64) {
                attachmentAttempted[index]=true; ++attachmentCandidates;
                const island::MonkeyAttachmentSite::GroundFrame frame{{state.x,state.y,state.z},state.headingRadians,state.scale};
                const auto attachmentStart=std::chrono::steady_clock::now();
                const auto prepared=island::MonkeyAttachmentSite::Prepare(state.selectedTree,frame,
                    monkeyAttachmentTerrain,worldObstacles,3.0f);
                const double attachmentTime=std::chrono::duration<double,std::milli>(
                    std::chrono::steady_clock::now()-attachmentStart).count();
                attachmentMillis+=attachmentTime;
                attachmentMaxMillis=std::max(attachmentMaxMillis,attachmentTime);
                island::MonkeyAttachmentSite::Result ranged{};
                int siteChecked=0;
                while (siteChecked<island::MonkeyAttachmentSite::SampleCount) {
                    const int count=std::min(16,island::MonkeyAttachmentSite::SampleCount-siteChecked);
                    const auto chunkStart=std::chrono::steady_clock::now();
                    ranged=island::MonkeyAttachmentSite::PrepareRange(state.selectedTree,frame,
                        monkeyAttachmentTerrain,worldObstacles,3.0f,false,siteChecked,count);
                    attachmentChunkMaxMillis=std::max(attachmentChunkMaxMillis,
                        std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-chunkStart).count());
                    siteChecked+=count;
                    if (!ranged.plan) break;
                }
                Check(ranged.failure==prepared.failure && ranged.plan.has_value()==prepared.plan.has_value(),
                    "incremental attachment preserves full validator decisions and rejection categories");
                if (ranged.plan) {
                    Check(siteChecked==1502,"attachment chunks retain 1001 reach and 501 fixed-wind clearance samples");
                    for (std::size_t key=0;key<prepared.plan->keys.size();++key)
                        for (std::size_t limb=0;limb<4;++limb) {
                            const auto a=ranged.plan->keys[key].contacts[limb];
                            const auto b=prepared.plan->keys[key].contacts[limb];
                            Check(a.x==b.x && a.y==b.y && a.z==b.z,
                                "provisional attachment chunks preserve exactly the same plan contacts");
                        }
                }
                if (prepared.plan) {
                    ++attachmentAccepted;
                    const auto& plan=*prepared.plan;
                    const auto excursion=island::MonkeyTreeExcursion::Build(plan,frame,
                        island::GroundJunglePlacement(state.selectedTree,monkeyAttachmentTerrain),3.0f);
                    const auto clearanceStart=std::chrono::steady_clock::now();
                    const auto clearance=island::MonkeyExcursionClearance::Validate(excursion,
                        monkeyAttachmentTerrain,worldObstacles,3.0f);
                    const double clearanceTime=std::chrono::duration<double,std::milli>(
                        std::chrono::steady_clock::now()-clearanceStart).count();
                    clearanceMillis+=clearanceTime;
                    clearanceMaxMillis=std::max(clearanceMaxMillis,clearanceTime);
                    if (clearance.failure==island::MonkeyExcursionClearance::Failure::None) {
                        ++excursionClear;
                        if (excursionClear==1) {
                            int checkedSamples=0;
                            while (checkedSamples<island::MonkeyExcursionClearance::SampleCount) {
                                const int count=std::min(16,island::MonkeyExcursionClearance::SampleCount-checkedSamples);
                                const auto chunkStart=std::chrono::steady_clock::now();
                                const auto chunk=island::MonkeyExcursionClearance::ValidateRange(excursion,
                                    monkeyAttachmentTerrain,worldObstacles,3.0f,checkedSamples,count);
                                clearanceChunkMaxMillis=std::max(clearanceChunkMaxMillis,
                                    std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-chunkStart).count());
                                Check(chunk.failure==island::MonkeyExcursionClearance::Failure::None,
                                    "incremental clearance checks exactly the same passing poses as full validation");
                                checkedSamples+=count;
                            }
                            Check(checkedSamples==1951,"incremental clearance retains every original sample");
                            const auto summit=island::MonkeyTreeExcursion::Sample(excursion,19.0f,22.0f,false);
                            const auto foot=frame.World(summit.body.contacts[2]);
                            auto obstructed=worldObstacles;
                            obstructed.push_back({foot.x,foot.z,.025f});
                            Check(island::MonkeyExcursionClearance::Validate(excursion,monkeyAttachmentTerrain,
                                obstructed,3.0f).failure==island::MonkeyExcursionClearance::Failure::Obstacle,
                                "full excursion preflight rejects an additional obstacle along its bark-contact path");
                            auto sunken=excursion;
                            sunken.frame.origin.y-=.02f; sunken.tree.y-=.02f;
                            Check(island::MonkeyExcursionClearance::Validate(sunken,monkeyAttachmentTerrain,
                                worldObstacles,3.0f).failure==island::MonkeyExcursionClearance::Failure::Terrain,
                                "full excursion preflight rejects sole footprints penetrating actual terrain");
                            auto invalid=excursion; invalid.frame.scale=0;
                            Check(island::MonkeyExcursionClearance::Validate(invalid,monkeyAttachmentTerrain,
                                worldObstacles,3.0f).failure==island::MonkeyExcursionClearance::Failure::InvalidInput,
                                "full excursion clearance rejects invalid frames before querying geometry");
                        }
                    }
                    else std::cout << "Monkey excursion clearance rejected reason=" << static_cast<int>(clearance.failure)
                        << " time=" << clearance.seconds << " probe=" << clearance.probe << '\n';
                    if (island::MonkeyTreeExcursion::IsReachable(excursion)) {
                        ++excursionAccepted;
                        const auto distance=[](auto a,auto b) {
                            return std::hypot(a.y-b.y,std::hypot(a.x-b.x,a.z-b.z));
                        };
                        if (excursionAccepted==1) {
                            bool movingBarkObserved=false;
                            for (float wind:{0.0f,3.0f,17.0f}) for (bool far:{false,true}) {
                                // A coarse trunk can move the requested palm beyond arm
                                // reach. Establish that independently before retargeting;
                                // refusal is correct, clamping and reporting a grip is not.
                                bool initialLimited=false;
                                auto initial=plan.ground;
                                for (std::size_t hand=0;hand<2;++hand) {
                                    const auto target=island::MonkeyTreeReach::Target(excursion.tree,frame.origin,
                                        frame.heading,frame.scale,initial.limbs[hand].root.y,hand==0,wind,far);
                                    Check(target.has_value(),"retarget fixture has defined bark coordinates");
                                    if (target) initial.limbs[hand]=island::MonkeyGroundGait::SolveLimb(
                                        initial.limbs[hand].root,target->position,island::MonkeyTreeReach::BendHint(1),
                                        true,initialLimited);
                                }
                                initialLimited=initialLimited||!island::MonkeyTreeReach::RaiseTorso(initial,.22f);
                                if (initialLimited) {
                                    bool refused=false;
                                    try { (void)island::MonkeyTreeExcursion::Retarget(excursion,wind,far); }
                                    catch (const std::invalid_argument&) { refused=true; }
                                    Check(refused,"unreachable mesh/wind retarget is rejected instead of clamping bark grips");
                                    continue;
                                }
                                const auto retargeted=island::MonkeyTreeExcursion::Retarget(excursion,wind,far);
                                for (std::size_t limb=2;limb<4;++limb) {
                                    Check(distance(retargeted.attachment.keys[0].contacts[limb],plan.keys[0].contacts[limb])<1e-6f
                                        && distance(retargeted.attachment.keys[5].contacts[limb],plan.keys[5].contacts[limb])<1e-6f,
                                        "wind/mesh retargeting leaves planted and staged ground feet fixed");
                                }
                                for (std::size_t hand=0;hand<2;++hand) {
                                    const auto expected=island::MonkeyTreeReach::Target(excursion.tree,frame.origin,
                                        frame.heading,frame.scale,plan.ground.limbs[hand].root.y,hand==0,wind,far);
                                    Check(expected && distance(expected->position,retargeted.attachment.keys[0].contacts[hand])<1e-6f,
                                        "retargeted forepalms use actual current wind and trunk mesh queries");
                                    if (!far && wind!=3)
                                        movingBarkObserved=movingBarkObserved
                                            ||distance(retargeted.attachment.keys[0].contacts[hand],plan.keys[0].contacts[hand])>1e-6f;
                                }
                                for (float boundary:{10.0f,18.0f,21.0f,29.0f}) {
                                    const auto before=island::MonkeyTreeExcursion::Sample(excursion,boundary-.0001f,wind-.0001f,far);
                                    const auto after=island::MonkeyTreeExcursion::Sample(excursion,boundary+.0001f,wind+.0001f,far);
                                    for (std::size_t limb=0;limb<4;++limb) {
                                        const auto& a=before.body.limbs.limbs[limb];
                                        const auto& b=after.body.limbs.limbs[limb];
                                        Check(distance(a.root,b.root)<.001f && distance(a.joint,b.joint)<.001f
                                            && distance(a.sole,b.sole)<.001f,
                                            "stage transitions stay continuous with advancing wind on either trunk mesh");
                                    }
                                }
                            }
                            Check(movingBarkObserved,"wind test must move the bark, not silently reuse frozen grips");
                            for (int step=0;step<=390;++step) {
                                const float time=step*.1f;
                                const auto moving=island::MonkeyTreeExcursion::Sample(excursion,time,time+3,false);
                                Check(!moving.body.limbs.reachLimited,"selected real excursion stays reachable with advancing wind");
                                unsigned supports=0;
                                for (const auto& limb:moving.body.limbs.limbs) {
                                    supports+=limb.supporting;
                                    Check(std::abs(distance(limb.root,limb.joint)-.31f)<1e-4f
                                        && std::abs(distance(limb.joint,limb.sole)-.33f)<1e-4f,
                                        "moving-wind excursion preserves limb lengths");
                                }
                                Check(supports>=3,"moving-wind excursion retains three supports");
                            }
                            for (float interruptAt:{.25f,5.0f,12.0f,17.0f,19.0f,24.0f,33.0f}) {
                                island::MonkeyTreeExcursion::Clock clock;
                                while (clock.seconds<interruptAt) clock.Advance(.05f);
                                const auto before=island::MonkeyTreeExcursion::Sample(excursion,clock.seconds);
                                clock.Advance(.001f,true);
                                const auto after=island::MonkeyTreeExcursion::Sample(excursion,clock.seconds);
                                for (std::size_t limb=0;limb<4;++limb)
                                    Check(distance(before.body.contacts[limb],after.body.contacts[limb])<.005f,
                                        "an excursion interruption starts returning without teleporting contacts");
                                for (int step=0;step<1000 && !clock.finished;++step) clock.Advance(.05f);
                                Check(clock.finished && clock.returning,"interrupted excursion returns in bounded time");
                                const auto returned=island::MonkeyTreeExcursion::Sample(excursion,clock.seconds);
                                const auto planted=island::MonkeyTreeExcursion::Sample(excursion,0);
                                for (std::size_t limb=0;limb<4;++limb)
                                    Check(distance(returned.body.contacts[limb],planted.body.contacts[limb])<1e-5f,
                                        "interruption finishes on the original planted ground/bark grips before release");
                            }
                            island::MonkeyTreeExcursion::Clock clock;
                            clock.Advance(quietNan); clock.Advance(-1);
                            Check(clock.seconds==0 && !clock.finished,"invalid excursion time step does not advance the clock");
                        }
                        for (float boundary:{10.0f,18.0f,21.0f,29.0f}) {
                            const auto before=island::MonkeyTreeExcursion::Sample(excursion,boundary-.0001f);
                            const auto after=island::MonkeyTreeExcursion::Sample(excursion,boundary+.0001f);
                            for (std::size_t limb=0;limb<4;++limb) {
                                const auto& a=before.body.limbs.limbs[limb];
                                const auto& b=after.body.limbs.limbs[limb];
                                Check(distance(a.root,b.root)<.001f && distance(a.joint,b.joint)<.001f
                                    && distance(a.sole,b.sole)<.001f,
                                    "whole tree excursion has continuous roots, joints and soles at stage boundaries");
                            }
                        }
                        for (int step=0;step<=390;++step) {
                            const auto pose=island::MonkeyTreeExcursion::Sample(excursion,step*.1f);
                            const auto windy=island::MonkeyTreeExcursion::Sample(excursion,step*.1f,3+step*.1f,false);
                            Check(!windy.body.limbs.reachLimited,
                                "every accepted natural site retains full-cycle reach with advancing wind");
                            const auto reverse=island::MonkeyTreeExcursion::Sample(excursion,39-step*.1f);
                            unsigned supports=0;
                            for (std::size_t limb=0;limb<4;++limb) {
                                const auto& current=pose.body.limbs.limbs[limb];
                                const auto& windLimb=windy.body.limbs.limbs[limb];
                                Check(std::abs(distance(windLimb.root,windLimb.joint)-.31f)<1e-4f
                                    && std::abs(distance(windLimb.joint,windLimb.sole)-.33f)<1e-4f,
                                    "moving-wind contacts do not stretch limbs at natural sites");
                                supports+=current.supporting;
                                Check(distance(current.sole,reverse.body.limbs.limbs[limb].sole)<.0001f,
                                    "descent and detachment retrace the ascent contacts without ground teleport");
                                Check(std::abs(distance(current.root,current.joint)-.31f)<1e-4f
                                    && std::abs(distance(current.joint,current.sole)-.33f)<1e-4f,
                                    "whole excursion preserves fixed limb lengths");
                            }
                            Check(supports>=3,"whole excursion retains at least three supports");
                            if (pose.stage==island::MonkeyTreeExcursion::Stage::Rest)
                                Check(supports==4,"summit rest holds all four bark contacts");
                        }
                    }
                    if (attachmentAccepted==1) {
                        for (const auto& [first,count]:std::array<std::pair<int,int>,5>{{
                                {-1,1},{0,0},{0,-1},{1502,1},{1501,2}}})
                            Check(island::MonkeyAttachmentSite::PrepareRange(state.selectedTree,frame,
                                monkeyAttachmentTerrain,worldObstacles,3.0f,false,first,count).failure
                                    ==island::MonkeyAttachmentSite::Failure::InvalidInput,
                                "attachment range rejects negative, empty and overflowing sample intervals");
                        for (const auto& [first,count]:std::array<std::pair<int,int>,5>{{
                                {-1,1},{0,0},{0,-1},{1001,1},{1000,2}}})
                            Check(!island::MonkeyTreeAttachment::IsReachableRange(plan,first,count),
                                "reach range rejects negative, empty and overflowing sample intervals");
                        auto displacedTree=state.selectedTree;
                        displacedTree.y+=100;
                        const auto regrounded=island::MonkeyAttachmentSite::Prepare(displacedTree,frame,
                            monkeyAttachmentTerrain,worldObstacles,3.0f);
                        Check(regrounded.plan.has_value() && regrounded.plan->keys[10].contacts[0].y==plan.keys[10].contacts[0].y,
                            "attachment uses rendered tree grounding rather than stale layout Y");
                        auto wrongFrame=frame; wrongFrame.heading+=.2f;
                        Check(island::MonkeyAttachmentSite::Prepare(state.selectedTree,wrongFrame,
                            monkeyAttachmentTerrain,worldObstacles,3.0f).failure==island::MonkeyAttachmentSite::Failure::Heading,
                            "attachment does not silently snap a monkey that is not facing its tree");
                        wrongFrame=frame; wrongFrame.scale=0;
                        Check(island::MonkeyAttachmentSite::Prepare(state.selectedTree,wrongFrame,
                            monkeyAttachmentTerrain,worldObstacles,3.0f).failure==island::MonkeyAttachmentSite::Failure::InvalidInput,
                            "invalid monkey scale is rejected before surface queries");
                    }
                    for (std::size_t foot=2;foot<4;++foot) {
                        const auto point=frame.World(plan.keys[5].contacts[foot]);
                        const auto ground=monkeyAttachmentTerrain.Sample(point.x,point.z);
                        Check(std::abs(point.y-ground.height-island::MonkeyGroundGait::SoleHeight*state.scale/ground.normal.y)<1e-4f,
                            "attachment staging feet follow actual rendered terrain, not a flat plane");
                    }
                    const auto blocker=frame.World(plan.keys[5].contacts[2]);
                    auto blocked=worldObstacles;
                    blocked.push_back({blocker.x,blocker.z,.09f});
                    const auto denied=island::MonkeyAttachmentSite::Prepare(state.selectedTree,frame,
                        monkeyAttachmentTerrain,blocked,3.0f);
                    Check(!denied.plan && denied.failure==island::MonkeyAttachmentSite::Failure::Obstacle,
                        "an extra obstacle at a staging foot rejects attachment before lifting supports");
                } else ++attachmentFailures[static_cast<std::size_t>(prepared.failure)];
            }
            Check(state.treeIndex == repeated.treeIndex,
                  "repeated monkey simulation selects the same actual canopy tree");
            if (state.behavior == island::MonkeyBehavior::ApproachTree
                || state.behavior == island::MonkeyBehavior::InspectTree) {
                Check(state.treeIndex >= 0 && static_cast<std::size_t>(state.treeIndex) < jungle.Canopy().size(),
                      "tree behaviour references an installed canopy placement");
                if (state.treeIndex >= 0) {
                    Check(island::MonkeyTroop::IsClosedCanopyTree(
                              jungle.Canopy(), static_cast<std::size_t>(state.treeIndex)),
                          "monkey selected an isolated or mangrove tree outside closed canopy");
                }
                observedMonkeyTreeApproach = true;
                if (state.behavior == island::MonkeyBehavior::InspectTree) {
                    if (!observedMonkeyTreeArrival) {
                        auto interrupted = monkeys;
                        interrupted.Update(.1f,state.x,state.y,state.z);
                        Check(interrupted.Monkeys()[index].behavior == island::MonkeyBehavior::RunAway
                            && interrupted.Monkeys()[index].treeIndex == -1,
                            "player proximity interrupts tree inspection and releases the tree");
                        auto removed = monkeys;
                        removed.SetCanopy({});
                        Check(removed.Monkeys()[index].treeIndex == -1
                            && removed.Monkeys()[index].behavior == island::MonkeyBehavior::Idle,
                            "replacing canopy placements clears obsolete tree identities");
                        auto invalidTree = jungle.Canopy().front();
                        invalidTree.scale = quietNan;
                        bool rejected = false;
                        try { removed.SetCanopy(std::span<const island::CanopyPlacement>(&invalidTree,1)); }
                        catch (const std::invalid_argument&) { rejected = true; }
                        Check(rejected,"non-finite canopy placement is rejected before installation");
                    }
                    observedMonkeyTreeArrival = true;
                    const auto& tree = jungle.Canopy()[static_cast<std::size_t>(state.treeIndex)];
                    Check(std::hypot(state.x-tree.x,state.z-tree.z)
                        >= island::MonkeyTreeReach::StandOff(tree.scale,state.scale)-.121f,
                        "tree staging remains outside the flared root with body clearance");
                    Check(std::hypot(state.x-state.targetX,state.z-state.targetZ) < .121f
                        && state.speed == 0,
                        "tree inspection starts only after reaching the staging point and stopping");
                }
            } else {
                Check(state.treeIndex == -1,"ordinary ground behaviour releases its selected tree");
            }
            const island::TerrainSample sample = terrain.Sample(state.x, state.z);
            Check(state.x == repeated.x && state.y == repeated.y
                      && state.z == repeated.z
                      && state.headingRadians == repeated.headingRadians
                      && state.headLookRadians == repeated.headLookRadians
                      && state.speed == repeated.speed
                      && state.animationSeconds == repeated.animationSeconds
                      && state.gaitPhase == repeated.gaitPhase
                      && state.gaitAmount == repeated.gaitAmount
                      && state.homeX == repeated.homeX && state.homeZ == repeated.homeZ
                      && state.targetX == repeated.targetX
                      && state.targetZ == repeated.targetZ
                      && state.behavior == repeated.behavior
                      && state.simulationTier == repeated.simulationTier,
                  "monkey troop remains deterministic over a long session");
            Check(std::isfinite(state.x) && std::isfinite(state.y)
                      && std::isfinite(state.z) && std::isfinite(state.headingRadians)
                      && std::isfinite(state.headLookRadians)
                      && std::isfinite(state.animationSeconds)
                      && std::isfinite(state.homeX) && std::isfinite(state.homeZ)
                      && std::isfinite(state.targetX) && std::isfinite(state.targetZ),
                  "monkey pose remains finite over a long session");
            Check(state.gaitPhase >= 0.0f && state.gaitPhase < 1.0f
                      && state.gaitAmount >= 0.0f && state.gaitAmount <= 1.0f,
                  "distance-driven monkey gait remains bounded over all simulation tiers");
            if (state.x == previousMonkeys[index].x && state.z == previousMonkeys[index].z) {
                Check(state.gaitPhase == previousMonkeys[index].gaitPhase,
                      "stationary monkeys do not advance the ground stride");
            }
            Check(std::fabs(state.headLookRadians)
                      <= island::MonkeyTroop::MaximumHeadLookRadians + 0.001f,
                  "monkey head attention remains inside its restrained skeletal bound");
            Check(std::hypot(state.x - state.homeX, state.z - state.homeZ)
                      <= island::MonkeyTroop::MaximumHomeDistance + 0.001f
                      && std::hypot(state.targetX - state.homeX,
                                    state.targetZ - state.homeZ)
                          <= island::MonkeyTroop::MaximumHomeDistance + 0.001f,
                  "monkey pose and active target remain inside the authoritative home range");
            Check(state.animationSeconds >= previousMonkeys[index].animationSeconds,
                  "monkey animation clock remains monotonic across behaviour changes");
            observedMonkeyContinuousClipClock = observedMonkeyContinuousClipClock
                || (state.behavior != previousMonkeys[index].behavior
                    && state.animationSeconds > previousMonkeys[index].animationSeconds);
            Check(sample.signedShoreDistance >= 47.8f && sample.height >= 2.75f
                      && sample.slopeRadians <= 0.465f
                      && std::fabs(state.y - sample.height) < 0.001f,
                  "monkeys remain on supported traversable interior terrain");
            if (frame % 30 == 0) {
                for (const island::CircularObstacle& obstacle : worldObstacles) {
                    const float minimumDistance = island::MonkeyTroop::CollisionRadius
                        + std::max(0.0f, obstacle.radius);
                    Check(std::hypot(state.x - obstacle.x, state.z - obstacle.z)
                              >= minimumDistance - 0.001f,
                          "monkeys remain outside trunk, rock, and log collision proxies");
                    const auto& previous = previousMonkeys[index];
                    const float dx = state.x-previous.x, dz = state.z-previous.z;
                    const float lengthSquared = dx*dx+dz*dz;
                    const float t = lengthSquared > 0 ? std::clamp(
                        ((obstacle.x-previous.x)*dx+(obstacle.z-previous.z)*dz)/lengthSquared,
                        0.0f,1.0f) : 0.0f;
                    Check(std::hypot(previous.x+t*dx-obstacle.x,previous.z+t*dz-obstacle.z)
                              >= minimumDistance-.001f,
                          "sampled monkey movement segments do not cross solid proxies");
                }
            }
            monkeyTravelDistance += std::hypot(
                state.x - previousMonkeys[index].x,
                state.z - previousMonkeys[index].z);
            previousMonkeys[index] = state;
            observedMonkeyIdle = observedMonkeyIdle
                || state.behavior == island::MonkeyBehavior::Idle;
            observedMonkeyObserve = observedMonkeyObserve
                || state.behavior == island::MonkeyBehavior::Observe;
            observedMonkeyWalk = observedMonkeyWalk
                || state.behavior == island::MonkeyBehavior::Walk;
            observedMonkeyRunAway = observedMonkeyRunAway
                || state.behavior == island::MonkeyBehavior::RunAway;
        }
    }
    Check(observedMonkeyIdle && observedMonkeyObserve && observedMonkeyWalk
              && observedMonkeyRunAway,
          "monkey troop visits idle, observe, walk, and mild avoidance states");
    Check(observedMonkeyFullTier && observedMonkeyReducedTier && observedMonkeyDormantTier,
          "monkey troop exercises every simulation quality tier");
    Check(monkeyTravelDistance > 900.0f,
          "monkey troop covers meaningful valid ground during a long session");
    Check(monkeys.ObstacleDeflectionCount() > 0U
              && monkeys.ObstacleDeflectionCount()
                  == repeatedMonkeys.ObstacleDeflectionCount(),
          "monkey obstacle steering is exercised and remains deterministic");
    Check(observedMonkeyContinuousClipClock,
          "monkey behaviour changes preserve a continuous renderer animation clock");
    std::cout << "Monkey attachment sites " << attachmentAccepted << '/' << attachmentCandidates << "; rejections";
    for (std::size_t reason=0;reason<attachmentFailures.size();++reason)
        std::cout << ' ' << reason << ':' << attachmentFailures[reason];
    std::cout << '\n';
    Check(attachmentAccepted>0,"naturally reached real-island trees include a candidate passing terrain/contact/body-probe checks");
    std::cout << "Monkey full excursion reach " << excursionAccepted << '/' << attachmentAccepted << '\n';
    Check(excursionAccepted>0,"actual island attachment sites include reachable ascent and descent, not only a first grip");
    std::cout << "Monkey full excursion clearance " << excursionClear << '/' << attachmentAccepted << '\n';
    std::cout << "Monkey preflight milliseconds: attachment mean/max "
        << attachmentMillis/std::max(1U,attachmentCandidates) << '/' << attachmentMaxMillis
        << "; excursion mean/max " << clearanceMillis/std::max(1U,attachmentAccepted)
        << '/' << clearanceMaxMillis << '\n';
    std::cout << "Monkey preflight 16-pose chunk maximum milliseconds " << clearanceChunkMaxMillis << '\n';
    std::cout << "Monkey attachment 16-pose chunk maximum milliseconds " << attachmentChunkMaxMillis << '\n';
    Check(observedPreparing,"live preparation is exercised across updates instead of blocking on the whole trajectory");
    Check(excursionClear>0,"natural island arrivals include an excursion passing full-cycle clearance probes");
    {
        // At night each monkey near the walker makes for a tree and climbs it, and stays up at
        // the top of its climb, asleep, until morning; then it climbs down.
        island::MonkeyTroop roosting(terrain);
        roosting.SetObstacles(worldObstacles);
        roosting.SetCanopy(jungle.Canopy());
        roosting.SetNightness(1.0f);
        float cx = 0.0f, cz = 0.0f;
        for (const auto& monkey : roosting.Monkeys()) { cx += monkey.x; cz += monkey.z; }
        cx /= static_cast<float>(roosting.Monkeys().size());
        cz /= static_cast<float>(roosting.Monkeys().size());
        const auto restingUp = [&]() {
            int count = 0;
            for (const auto& monkey : roosting.Monkeys())
                if (monkey.treeExcursion && monkey.treeClock.seconds >= island::MonkeyTreeExcursion::AttachSeconds
                        + island::MonkeyTreeExcursion::TravelSeconds
                    && monkey.treeClock.seconds < island::MonkeyTreeExcursion::DescendStart) ++count;
            return count;
        };
        int upAtThreeMinutes = 0, upAtNineMinutes = 0;
        for (int frame = 0; frame < 30 * 540; ++frame) {
            roosting.Update(1.0f / 30.0f, cx + 12.0f, 4.0f, cz + 9.0f);
            if (frame == 30 * 180) upAtThreeMinutes = restingUp();
        }
        upAtNineMinutes = restingUp();
        roosting.SetNightness(0.0f);
        for (int frame = 0; frame < 30 * 90; ++frame) roosting.Update(1.0f / 30.0f, cx + 12.0f, 4.0f, cz + 9.0f);
        const int upInTheMorning = restingUp();
        std::cout << "monkeys asleep up their trees: " << upAtThreeMinutes << " after three minutes of night, "
                  << upAtNineMinutes << " after nine, " << upInTheMorning << " a minute and a half into the morning\n";
        Check(upAtNineMinutes >= 1 && upAtNineMinutes >= upAtThreeMinutes && upInTheMorning < upAtNineMinutes,
              "at night monkeys climb a tree and sleep up it until morning, then come down");
    }
    Check(observedLiveClimb && observedLiveSummit && checkedLiveReturn,
        "ordinary island simulation actually climbs, reaches summit and exercises safe interruption");
    Check(observedMonkeyTreeApproach && observedMonkeyTreeArrival,
          "real-island simulation approaches and reaches actual canopy trees");

    Check(island::OceanMeshLayout::VertexCount == 25921U,
          "ocean grid remains within the portable 16-bit vertex budget");
    Check(island::OceanMeshLayout::TriangleCount == 51200U,
          "ocean grid triangle count is deterministic");
    Check(island::OceanMeshLayout::IsPortable(fullQuality.oceanQuadsPerAxis)
              && island::OceanMeshLayout::IsPortable(mobileQuality.oceanQuadsPerAxis),
          "both runtime ocean grids fit portable 16-bit indices");
    Check(!island::OceanMeshLayout::IsPortable(0)
              && island::OceanMeshLayout::IsPortable(255)
              && !island::OceanMeshLayout::IsPortable(256)
              && !island::OceanMeshLayout::IsPortable(std::numeric_limits<int>::max()),
          "runtime ocean validation accepts the exact 16-bit grid boundary");
    Check(island::OceanMeshLayout::VertexCountFor(mobileQuality.oceanQuadsPerAxis) == 16641U
              && island::OceanMeshLayout::TriangleCountFor(
                     mobileQuality.oceanQuadsPerAxis) == 32768U,
          "mobile ocean grid has the audited reduced geometry budget");
    CheckNear(island::OceanMeshLayout::Coordinate(
                  mobileQuality.oceanQuadsPerAxis, mobileQuality.oceanQuadsPerAxis),
              island::OceanMeshLayout::Extent, 0.0001f,
              "mobile ocean grid reaches the shared outer extent");
    CheckNear(island::OceanMeshLayout::Coordinate(0), -island::OceanMeshLayout::Extent,
              0.0001f, "ocean grid begins at its documented extent");
    CheckNear(island::OceanMeshLayout::Coordinate(island::OceanMeshLayout::QuadsPerAxis),
              island::OceanMeshLayout::Extent, 0.0001f,
              "ocean grid ends at its documented extent");
    Check(island::ShorelineMeshLayout::VertexCount == 2695U,
          "shoreline ribbon stays inside its compact portable vertex budget");
    Check(island::ShorelineMeshLayout::TriangleCount == 4608U,
          "shoreline ribbon triangle count is deterministic");
    CheckNear(island::ShorelineMeshLayout::RadialOffset(0), -2.5f, 0.0001f,
              "shoreline ribbon begins landward of the authoritative contour");
    CheckNear(island::ShorelineMeshLayout::RadialOffset(
                  island::ShorelineMeshLayout::AcrossQuadCount),
              5.5f, 0.0001f,
              "shoreline ribbon ends in the shallow offshore wash band");

    Check(island::TerrainMeshLayout::CellCount == 64U,
          "terrain layout has the expected bounded cell count");
    std::size_t partitionTriangleCount = 0;
    for (std::size_t index = 0; index < island::TerrainMeshLayout::CellCount; ++index) {
        const island::TerrainCellLayout cell = island::TerrainMeshLayout::Cell(index);
        Check(cell.quadColumns == island::TerrainMeshLayout::QuadsPerCell
                  && cell.quadRows == island::TerrainMeshLayout::QuadsPerCell,
              "terrain cells use a uniform portable grid");
        Check(cell.minimumX < cell.maximumX && cell.minimumZ < cell.maximumZ,
              "terrain cell bounds have positive area");
        partitionTriangleCount += static_cast<std::size_t>(cell.quadColumns * cell.quadRows * 2);
        if (cell.column + 1 < island::TerrainMeshLayout::CellsPerAxis) {
            const island::TerrainCellLayout right = island::TerrainMeshLayout::Cell(index + 1U);
            CheckNear(cell.maximumX, right.minimumX, 0.0001f,
                      "horizontal terrain cell boundaries share an exact seam");
        }
        if (cell.row + 1 < island::TerrainMeshLayout::CellsPerAxis) {
            const island::TerrainCellLayout front = island::TerrainMeshLayout::Cell(
                index + island::TerrainMeshLayout::CellsPerAxis);
            CheckNear(cell.maximumZ, front.minimumZ, 0.0001f,
                      "vertical terrain cell boundaries share an exact seam");
        }
    }
    Check(partitionTriangleCount
              == static_cast<std::size_t>(island::TerrainMeshLayout::QuadsPerAxis
                                           * island::TerrainMeshLayout::QuadsPerAxis * 2),
          "terrain cells cover every source-grid triangle exactly once");
    Check(island::TerrainMeshLayout::Resolution * island::TerrainMeshLayout::Resolution
              == 58081,
          "the shared terrain vertex grid remains inside the portable 16-bit index budget");
    std::array<bool, island::TerrainMeshLayout::CellCount> noVisibleTerrainCells{};
    Check(island::TerrainMeshLayout::CoalesceVisibleCells(noVisibleTerrainCells).count == 0U,
          "terrain batching emits no range when every cell is culled");
    std::array<bool, island::TerrainMeshLayout::CellCount> allVisibleTerrainCells{};
    allVisibleTerrainCells.fill(true);
    const island::TerrainMeshLayout::CellRuns oneTerrainRun =
        island::TerrainMeshLayout::CoalesceVisibleCells(allVisibleTerrainCells);
    Check(oneTerrainRun.count == 1U && oneTerrainRun.values[0].firstCell == 0U
              && oneTerrainRun.values[0].cellCount == island::TerrainMeshLayout::CellCount,
          "terrain batching merges one fully visible island into one index range");
    std::array<bool, island::TerrainMeshLayout::CellCount> sparseTerrainCells{};
    sparseTerrainCells[0] = true;
    sparseTerrainCells[1] = true;
    sparseTerrainCells[3] = true;
    sparseTerrainCells[4] = true;
    sparseTerrainCells[5] = true;
    sparseTerrainCells.back() = true;
    const island::TerrainMeshLayout::CellRuns sparseTerrainRuns =
        island::TerrainMeshLayout::CoalesceVisibleCells(sparseTerrainCells);
    Check(sparseTerrainRuns.count == 3U
              && sparseTerrainRuns.values[0].firstCell == 0U
              && sparseTerrainRuns.values[0].cellCount == 2U
              && sparseTerrainRuns.values[1].firstCell == 3U
              && sparseTerrainRuns.values[1].cellCount == 3U
              && sparseTerrainRuns.values[2].firstCell
                  == island::TerrainMeshLayout::CellCount - 1U
              && sparseTerrainRuns.values[2].cellCount == 1U,
          "terrain batching preserves gaps while merging adjacent visible cells");

    Check(terrain.IsLand(0.0f, 0.0f), "island centre is land");
    Check(!terrain.IsLand(420.0f, 0.0f), "far offshore point is water");
    Check(terrain.Height(420.0f, 0.0f) < -3.0f, "offshore seabed descends");
    Check(terrain.Height(0.0f, 0.0f) > 8.0f, "interior has useful elevation");
    const island::TerrainSample invalidTerrainSample = terrain.Sample(
        quietNan, std::numeric_limits<float>::infinity());
    Check(invalidTerrainSample.height == -22.0f
              && invalidTerrainSample.signedShoreDistance
                  == -island::IslandTerrain::MaximumQueryCoordinate
              && invalidTerrainSample.normal.x == 0.0f
              && invalidTerrainSample.normal.y == 1.0f
              && invalidTerrainSample.normal.z == 0.0f
              && invalidTerrainSample.slopeRadians == 0.0f
              && invalidTerrainSample.surface == island::TerrainSurface::DeepSeabed,
          "non-finite terrain coordinates fail closed to a finite deep-seabed sample");
    Check(terrain.Height(std::numeric_limits<float>::max(), 0.0f) == -22.0f
              && terrain.SignedShoreDistance(
                     0.0f, -island::IslandTerrain::MaximumQueryCoordinate - 1.0f)
                  == -island::IslandTerrain::MaximumQueryCoordinate
              && !terrain.IsLand(quietNan, 0.0f),
          "out-of-domain terrain queries avoid procedural-noise conversion and remain water");
    CheckNear(terrain.ShoreRadius(quietNan), terrain.ShoreRadius(0.0f), 0.0001f,
              "non-finite shoreline angles use the deterministic zero-angle profile");
    {
        float leastShoreRadius = std::numeric_limits<float>::max();
        for (int step = 0; step < 200000; ++step) {
            const float angle = -std::numbers::pi_v<float> + 2.0f * std::numbers::pi_v<float> * static_cast<float>(step) / 200000.0f;
            leastShoreRadius = std::min(leastShoreRadius, terrain.ShoreRadius(angle));
        }
        Check(leastShoreRadius >= island::IslandTerrain::MinimumShoreRadius,
              "the shoreline never comes inside the documented least shore radius");
        bool inlandAgrees = true;
        for (int step = 0; step < 40000 && inlandAgrees; ++step) {
            const float angle = 0.000157f * static_cast<float>(step * 7);
            const float radius = 300.0f * static_cast<float>((step * 37) % 1000) / 1000.0f;
            const float x = radius * std::cos(angle), z = radius * std::sin(angle);
            for (const float distance : {0.0f, 10.0f, 20.0f, 35.0f, 90.0f})
                inlandAgrees = inlandAgrees
                    && terrain.IsInlandBy(x, z, distance) == (terrain.SignedShoreDistance(x, z) >= distance);
        }
        Check(inlandAgrees, "IsInlandBy answers exactly as the evaluated shore distance does");
    }

    const island::TerrainSample centreSample = terrain.Sample(0.0f, 0.0f);
    const float centreNormalLength = std::hypot(
        centreSample.normal.y, std::hypot(centreSample.normal.x, centreSample.normal.z));
    CheckNear(centreNormalLength, 1.0f, 0.0001f, "terrain sample normal is normalized");
    Check(centreSample.normal.y > 0.0f, "terrain sample normal points upward");
    Check(centreSample.surface == island::TerrainSurface::InteriorGround
              || centreSample.surface == island::TerrainSurface::SteepGround,
          "island centre has an interior ground classification");

    const auto regionalSummit = [&](int minX, int maxX, int minZ, int maxZ) {
        float highest = 0.0f;
        for (int z = minZ; z <= maxZ; z += 2)
            for (int x = minX; x <= maxX; x += 2)
                highest = std::max(highest, terrain.Height(static_cast<float>(x), static_cast<float>(z)));
        return highest;
    };
    const float mainSummit = regionalSummit(-90, -20, 20, 86);
    const float secondarySummit = regionalSummit(44, 104, -60, 0);
    const float saddleHeight = terrain.Height(10.0f, 5.0f);
    std::cout << "Terrain summits/saddle: " << mainSummit << '/' << secondarySummit
              << '/' << saddleHeight << '\n';
    Check(mainSummit >= 35.0f && mainSummit <= 45.0f,
          "main ridge has an island-scale 35 to 45 metre summit");
    Check(secondarySummit >= 25.0f && secondarySummit <= 30.0f,
          "second ridge has a distinct 25 to 30 metre summit");
    Check(saddleHeight > 6.0f && saddleHeight < secondarySummit - 6.0f,
          "a lower inland saddle separates the two ridges");
    std::cout << "South valley left/bed/right: " << terrain.Height(-31.0f, -70.0f)
              << '/' << terrain.Height(-6.0f, -70.0f) << '/' << terrain.Height(19.0f, -70.0f) << '\n';
    Check(terrain.Height(-6.0f, -70.0f) + 1.0f < terrain.Height(-31.0f, -70.0f)
              && terrain.Height(-6.0f, -70.0f) + 1.0f < terrain.Height(19.0f, -70.0f),
          "southern drainage has shoulders on both sides rather than a uniform dome");
    Check(terrain.Height(-6.0f, -70.0f) > terrain.Height(5.0f, -175.0f) + 2.0f,
          "southern drainage descends toward the coastal plain");

    const island::TerrainSample deepWaterSample = terrain.Sample(420.0f, 0.0f);
    Check(deepWaterSample.IsSubmerged(), "deep-water sample reports submerged terrain");
    Check(deepWaterSample.surface == island::TerrainSurface::DeepSeabed,
          "far offshore terrain is classified as deep seabed");

    float minimumRadius = 1000.0f;
    float maximumRadius = 0.0f;
    for (int step = 0; step < 720; ++step) {
        const float angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(step) / 720.0f;
        const float radius = terrain.ShoreRadius(angle);
        minimumRadius = std::min(minimumRadius, radius);
        maximumRadius = std::max(maximumRadius, radius);
        const float x = std::cos(angle) * radius;
        const float z = std::sin(angle) * radius;
        CheckNear(terrain.SignedShoreDistance(x, z), 0.0f, 0.001f,
                  "shore parameterization agrees with shore distance");
        Check(std::isfinite(terrain.Height(x, z)), "shore height is finite");
    }
    Check(minimumRadius > 150.0f, "sheltered bay preserves a broad connected island interior");
    Check(maximumRadius < 275.0f, "shoreline remains near the 500 metre target");
    Check(maximumRadius - minimumRadius > 20.0f, "shoreline silhouette has variation");
    const float westBayRadius = terrain.ShoreRadius(std::numbers::pi_v<float>);
    Check(terrain.ShoreRadius(2.48f) > westBayRadius + 35.0f
              && terrain.ShoreRadius(-2.48f) > westBayRadius + 35.0f,
          "western sheltered bay sits between two pronounced headlands");
    Check(terrain.ShoreRadius(0.35f) > terrain.ShoreRadius(1.0f) + 12.0f,
          "windward coast has a separate eastern promontory");
    CheckNear(terrain.ShoreRadius(-std::numbers::pi_v<float>), westBayRadius, 0.001f,
              "sheltered bay is continuous across the polar seam");
    for (const float headlandAngle : {2.48f, -2.48f}) {
        const float tipX = std::cos(headlandAngle) * terrain.ShoreRadius(headlandAngle);
        Check(-westBayRadius > tipX + 25.0f,
              "bay shore is recessed at least 25 metres behind both headland tips in world space");
    }
    std::cout << "Coast radius min/max/bay: " << minimumRadius << '/' << maximumRadius
              << '/' << westBayRadius << '\n';
    const auto coastalStation = [&](float angle, float inland) {
        const float radius = terrain.ShoreRadius(angle) - inland;
        return terrain.Sample(std::cos(angle) * radius, std::sin(angle) * radius);
    };
    const auto rockyStation = coastalStation(0.35f, 12.0f);
    const auto bayStation = coastalStation(std::numbers::pi_v<float>, 12.0f);
    Check(rockyStation.coastalRockiness > 0.90f && bayStation.coastalRockiness < 0.05f,
          "coastal environment distinguishes headland and bay at the same shore distance");
    Check(rockyStation.height > bayStation.height + 6.0f,
          "rocky headland has actual elevated relief, not only a material tint");
    Check(rockyStation.surface == island::TerrainSurface::RockyGround,
          "rocky headland is not classified as sandy habitat");
    Check(rockyStation.beachSuitability < 0.1f && bayStation.beachSuitability > 0.9f,
          "sheltered bay and exposed rock have distinct beach suitability");
    const float bayWidth = island::IslandEnvironmentField::BeachWidth(std::numbers::pi_v<float>);
    const float easternWidth = island::IslandEnvironmentField::BeachWidth(0.35f);
    Check(bayWidth > 34.0f && bayWidth < 37.0f && easternWidth < 8.0f,
          "dominant bay beach is substantially wider than the exposed rocky margin");
    const auto backBayBeach = coastalStation(std::numbers::pi_v<float>, 25.0f);
    Check(backBayBeach.beachSuitability > 0.45f && backBayBeach.height < 2.0f,
          "wide bay beach has both sandy material potential and low physical relief");
    const auto coastalHeight = [&](const float angle, const float inlandDistance) {
        const float radius = terrain.ShoreRadius(angle) - inlandDistance;
        return terrain.Height(std::cos(angle) * radius, std::sin(angle) * radius);
    };
    const float southBeachAngle = -std::numbers::pi_v<float> * 0.5f;
    const float lowerSouthBeach = coastalHeight(southBeachAngle, 3.0f);
    const float bermSouthBeach = coastalHeight(southBeachAngle, 9.0f);
    const float upperSouthBeach = coastalHeight(southBeachAngle, 15.0f);
    Check(bermSouthBeach > 0.5f * (lowerSouthBeach + upperSouthBeach) + 0.25f,
          "broad beach has a physical sub-metre berm and dune shoulder");
    const float lowerBay = coastalHeight(std::numbers::pi_v<float>, 1.0f);
    const float washBay = coastalHeight(std::numbers::pi_v<float>, 3.0f);
    const float upperBay = coastalHeight(std::numbers::pi_v<float>, 5.0f);
    Check(washBay + 0.01f < std::min(lowerBay, upperBay),
          "sheltered beach has a shallow terrain-authored wash channel");
    Check(coastalHeight(0.35f, 12.0f) > coastalHeight(0.35f, 15.0f) + 0.04f,
          "rocky headland has a walkable sub-metre shelf step");
    CheckNear(coastalHeight(southBeachAngle, 0.0f), 0.0f, 0.0001f,
              "coastal micro-relief preserves the exact swimming shoreline");
    CheckNear(island::IslandEnvironmentField::BeachWidth(-std::numbers::pi_v<float>), bayWidth,
              0.0001f, "bay beach width stays continuous across the polar seam");
    const auto coastalExperienceStation = [&](const float angle, const float inland) {
        const float radius=terrain.ShoreRadius(angle)-inland;
        const float x=std::cos(angle)*radius,z=std::sin(angle)*radius;
        return island::IslandEnvironmentField::SampleCoastalExperience(
            x,z,terrain.SignedShoreDistance(x,z));
    };
    const auto broadBeachExperience=coastalExperienceStation(
        -std::numbers::pi_v<float>*.5f,12.0f);
    const auto bayExperience=coastalExperienceStation(std::numbers::pi_v<float>,12.0f);
    const auto headlandExperience=coastalExperienceStation(.35f,12.0f);
    const float mouthX=island::StreamRoute::SourceX-island::StreamRoute::NominalLength;
    const float mouthZ=island::StreamRoute::CentreZ(mouthX);
    const auto mouthExperience=island::IslandEnvironmentField::SampleCoastalExperience(
        mouthX,mouthZ,terrain.SignedShoreDistance(mouthX,mouthZ));
    const auto broadBeachDebris=island::IslandEnvironmentField::SampleCoastalDebris(
        0.0f,-terrain.ShoreRadius(-std::numbers::pi_v<float>*.5f)+12.0f,
        coastalStation(-std::numbers::pi_v<float>*.5f,12.0f).signedShoreDistance);
    const float bayDebrisRadius=terrain.ShoreRadius(std::numbers::pi_v<float>)-12.0f;
    const auto bayDebris=island::IslandEnvironmentField::SampleCoastalDebris(
        -bayDebrisRadius,0.0f,bayStation.signedShoreDistance);
    const float headlandDebrisRadius=terrain.ShoreRadius(.35f)-12.0f;
    const auto headlandDebris=island::IslandEnvironmentField::SampleCoastalDebris(
        std::cos(.35f)*headlandDebrisRadius,std::sin(.35f)*headlandDebrisRadius,
        rockyStation.signedShoreDistance);
    Check(broadBeachExperience.broadBeach>.90f
            && broadBeachExperience.broadBeach>broadBeachExperience.shelteredBay,
        "southern Robinson beach has a dominant shared coastal identity");
    Check(bayExperience.shelteredBay>.75f
            && bayExperience.shelteredBay>bayExperience.mangroveMouth,
        "protected western bay remains distinct from its localized stream mouth");
    Check(headlandExperience.rockyShore>.90f,
        "exposed eastern headland has a dominant rocky-shore identity");
    Check(mouthExperience.mangroveMouth>.80f
            && mouthExperience.mangroveMouth>mouthExperience.shelteredBay,
        "stream outlet has a localized mangrove-mouth coastal identity");
    Check(bayDebris.windExposure<headlandDebris.windExposure*.45f
            && bayDebris.shelter>.70f && headlandDebris.shelter<.10f,
        "shared debris field distinguishes leeward bay shelter from windward headland exposure");
    Check(bayDebris.accumulation>.70f
            && broadBeachDebris.accumulation>.10f
            && broadBeachDebris.accumulation<bayDebris.accumulation
            && headlandDebris.accumulation<.05f,
        "shared debris accumulation is strongest in the bay and minimal on rocky headlands");
    Check(island::TidePoolLayout::Definitions.size()==5U,
        "rocky shore has a small bounded tide-pool set");
    for (std::size_t index=0;index<island::TidePoolLayout::Definitions.size();++index) {
        const auto& pool=island::TidePoolLayout::Definitions[index];
        const auto [x,z]=island::TidePoolLayout::Centre(terrain,index);
        const auto centre=terrain.Sample(x,z);
        const float water=island::TidePoolLayout::WaterLevel(terrain,index);
        Check(centre.signedShoreDistance>=23.5f && centre.signedShoreDistance<=34.5f
                && centre.coastalRockiness>.70f,
            "tide-pool basin remains on the authored rocky shore");
        CheckNear(island::TidePoolLayout::Depression(terrain,x,z),pool.depth,.0001f,
            "tide-pool centre receives its complete authored depression");
        Check(water>centre.height+.15f && water<centre.height+pool.depth-.12f,
            "tide-pool water lies above its floor and below the undepressed rim");
        Check(island::TidePoolLayout::WaterRadius(index)>1.0f
                && island::TidePoolLayout::WaterRadius(index)<pool.radius,
            "tide-pool water disc remains visibly broad and inside its basin");
        float narrowest=2.0f,widest=0.0f;
        for (int step=0;step<72;++step) {
            const float bearing=static_cast<float>(step)*0.0872665f;
            const float scale=island::TidePoolLayout::OutlineScale(index,bearing);
            narrowest=std::min(narrowest,scale);
            widest=std::max(widest,scale);
            // the depression follows the lobes: at the lobed waterline it stands where the round
            // basin's did
            const float reach=pool.radius*scale*.5f;
            CheckNear(island::TidePoolLayout::Depression(terrain,x+std::cos(bearing)*reach,z+std::sin(bearing)*reach),
                pool.depth*.5625f,.002f,"the tide-pool bowl follows its lobed outline");
        }
        Check(narrowest>=.77f && widest<=1.23f && widest-narrowest>.18f,
            "each tide pool's outline is lobed within bounds rather than round");
        for (std::size_t other=index+1;other<island::TidePoolLayout::Definitions.size();++other) {
            const auto [otherX,otherZ]=island::TidePoolLayout::Centre(terrain,other);
            Check(std::hypot(x-otherX,z-otherZ)>pool.radius
                    +island::TidePoolLayout::Definitions[other].radius+3.0f,
                "tide-pool basins remain separated rocky-shore landmarks");
        }
    }
    for (const auto& palm : vegetation.Palms())
        Check(terrain.Sample(palm.x, palm.z).coastalRockiness <= 0.45f,
              "adult and juvenile palms avoid exposed coastal rock");
    const auto checkCoastalRoots = [&](const auto& plants, float maximumRockiness) {
        for (const auto& plant : plants)
            Check(terrain.Sample(plant.x, plant.z).coastalRockiness <= maximumRockiness,
                  "jungle roots respect shared coastal rock exclusions");
    };
    checkCoastalRoots(jungle.Canopy(), 0.45f);
    checkCoastalRoots(jungle.Understory(), 0.60f);
    checkCoastalRoots(jungle.Bushes(), 0.60f);
    checkCoastalRoots(jungle.GroundPlants(), 0.60f);
    const auto invalidCoast = island::IslandEnvironmentField::SampleCoast(quietNan, 0.0f, 0.0f);
    Check(invalidCoast.rockiness == 0.0f && invalidCoast.beachSuitability == 0.0f,
          "non-finite environment query has safe empty controls");
    const auto invalidExperience=island::IslandEnvironmentField::SampleCoastalExperience(
        quietNan,0.0f,0.0f);
    Check(invalidExperience.broadBeach==0.0f && invalidExperience.shelteredBay==0.0f
            && invalidExperience.rockyShore==0.0f && invalidExperience.mangroveMouth==0.0f,
        "non-finite coastal-experience query has safe empty controls");
    const auto invalidDebris=island::IslandEnvironmentField::SampleCoastalDebris(
        quietNan,0.0f,0.0f);
    Check(invalidDebris.windExposure==0.0f && invalidDebris.shelter==0.0f
            && invalidDebris.accumulation==0.0f,
        "non-finite coastal-debris query has safe empty controls");
    const float streamX=-110.0f,streamZ=island::StreamRoute::CentreZ(streamX);
    CheckNear(island::StreamRoute::CentreZ(island::StreamRoute::SourceX), -40.05f,
        .0001f, "stream meander preserves the authored source connection");
    Check(std::abs(streamZ-(-55.0f-.23f*streamX))>1.0f,
        "shared stream centreline has a material mid-course meander");
    CheckNear(island::StreamRoute::HalfWidth(island::StreamRoute::SourceX), .45f,
        .0001f, "stream source retains its narrow authored connection");
    CheckNear(island::StreamRoute::HalfWidth(
        island::StreamRoute::SourceX-island::StreamRoute::NominalLength), 1.5f,
        .0001f, "stream mouth retains its broad authored connection");
    float positiveWidthVariation=0.0f,negativeWidthVariation=0.0f;
    float positiveMeander=0.0f,negativeMeander=0.0f;
    float minimumLeftBankScale=1.0f,maximumLeftBankScale=1.0f;
    float minimumRightBankScale=1.0f,maximumRightBankScale=1.0f;
    for(int along=0;along<=98;++along) {
        const float routeX=island::StreamRoute::SourceX-static_cast<float>(along);
        const float routeOffset=island::StreamRoute::CentreZ(routeX)-(-55.0f-.23f*routeX);
        positiveMeander=std::max(positiveMeander,routeOffset);
        negativeMeander=std::min(negativeMeander,routeOffset);
        const float t=static_cast<float>(along)/island::StreamRoute::NominalLength;
        const float shaped=t*t*(3.0f-2.0f*t);
        const float baseline=std::lerp(.45f,1.5f,shaped);
        const float width=island::StreamRoute::HalfWidth(
            island::StreamRoute::SourceX-static_cast<float>(along));
        const float leftScale=island::StreamRoute::BankWidthScale(
            static_cast<float>(along),-1.0f);
        const float rightScale=island::StreamRoute::BankWidthScale(
            static_cast<float>(along),1.0f);
        Check(std::isfinite(width)&&width>=.45f&&width<=1.56f,
            "stream pools and pinches stay inside the authored width budget");
        Check(std::isfinite(leftScale)&&std::isfinite(rightScale)
                &&leftScale>=.78f&&leftScale<=1.22f
                &&rightScale>=.78f&&rightScale<=1.22f,
            "independent stream-bank width rhythms stay bounded");
        positiveWidthVariation=std::max(positiveWidthVariation,width-baseline);
        negativeWidthVariation=std::min(negativeWidthVariation,width-baseline);
        minimumLeftBankScale=std::min(minimumLeftBankScale,leftScale);
        maximumLeftBankScale=std::max(maximumLeftBankScale,leftScale);
        minimumRightBankScale=std::min(minimumRightBankScale,rightScale);
        maximumRightBankScale=std::max(maximumRightBankScale,rightScale);
    }
    Check(positiveWidthVariation>.16f&&negativeWidthVariation<-.31f,
        "stream profile contains material low-frequency pools and pinches");
    Check(positiveMeander>2.0f&&negativeMeander<-2.8f,
        "stream route contains broad visible bends on both sides of its valley axis");
    Check(minimumLeftBankScale<.90f&&maximumLeftBankScale>1.09f
              &&minimumRightBankScale<.91f&&maximumRightBankScale>1.08f
              &&std::abs(island::StreamRoute::BankWidthScale(42.0f,-1.0f)
                          -island::StreamRoute::BankWidthScale(42.0f,1.0f))>.02f,
        "stream banks retain independent width rhythms instead of mirrored edges");
    const auto streamCentre=island::IslandEnvironmentField::SampleInland(
        streamX,streamZ,terrain.SignedShoreDistance(streamX,streamZ));
    const auto streamBank=island::IslandEnvironmentField::SampleInland(
        streamX,streamZ+14.0f,terrain.SignedShoreDistance(streamX,streamZ+14.0f));
    const auto dryInterior=island::IslandEnvironmentField::SampleInland(
        105.0f,80.0f,terrain.SignedShoreDistance(105.0f,80.0f));
    const float wetlandShore=terrain.SignedShoreDistance(
        island::IslandEnvironmentField::SeasonalWetlandX,
        island::IslandEnvironmentField::SeasonalWetlandZ);
    const auto seasonalWetland=island::IslandEnvironmentField::SampleInland(
        island::IslandEnvironmentField::SeasonalWetlandX,
        island::IslandEnvironmentField::SeasonalWetlandZ,wetlandShore);
    Check(streamCentre.drainage>0.95f && streamCentre.moisture>0.95f
        && streamBank.moisture>streamBank.drainage && streamBank.moisture>0.35f
        && dryInterior.moisture<0.15f && dryInterior.drainage<0.15f,
        "shared inland field distinguishes wet channel, damp bank and dry interior");
    Check(island::IslandEnvironmentField::SeasonalWetlandPotential(
              island::IslandEnvironmentField::SeasonalWetlandX,
              island::IslandEnvironmentField::SeasonalWetlandZ,wetlandShore)>.95f
            && seasonalWetland.moisture>.90f
            && seasonalWetland.drainage>.55f&&seasonalWetland.drainage<.70f,
        "seasonal inland depression has damp non-flowing environmental controls");
    Check(island::IslandEnvironmentField::SeasonalWetlandPotential(
              island::IslandEnvironmentField::SeasonalWetlandX+26.0f,
              island::IslandEnvironmentField::SeasonalWetlandZ,wetlandShore)==0.0f,
        "seasonal wetland stays local instead of becoming a second valley");
    const auto invalidInland=island::IslandEnvironmentField::SampleInland(quietNan,0,0);
    Check(invalidInland.moisture==0 && invalidInland.drainage==0
        && invalidInland.saltExposure==0,"non-finite inland environment query is safely empty");
    for(int x=-170;x<=130;x+=10)for(int z=-190;z<=170;z+=10) {
        const auto field=island::IslandEnvironmentField::SampleInland(
            static_cast<float>(x),static_cast<float>(z),terrain.SignedShoreDistance(x,z));
        Check(std::isfinite(field.moisture)&&field.moisture>=0&&field.moisture<=1
            && std::isfinite(field.drainage)&&field.drainage>=0&&field.drainage<=1
            && std::isfinite(field.saltExposure)&&field.saltExposure>=0&&field.saltExposure<=1,
            "inland environment controls stay finite and normalized across the island");
    }
    for (int angularIndex = 0; angularIndex < 180; ++angularIndex) {
        const float angle = angularIndex * std::numbers::pi_v<float> / 90.0f;
        const float width = island::IslandEnvironmentField::BeachWidth(angle);
        Check(std::isfinite(width) && width >= 6.0f && width <= 36.0f,
              "authored beach width stays finite and within coastal scale");
        for (float inland : {-20.0f, 0.0f, 12.0f, 40.0f, 80.0f}) {
            const auto station = coastalStation(angle, inland);
            Check(std::isfinite(station.coastalRockiness) && station.coastalRockiness >= 0.0f
                      && station.coastalRockiness <= 1.0f && station.beachSuitability >= 0.0f
                      && station.beachSuitability <= 1.0f,
                  "coastal environment controls remain finite and normalized");
            if (inland == 80.0f)
                Check(station.coastalRockiness == 0.0f,
                      "coastal rock field does not override interior ridge geology");
            const float radius=terrain.ShoreRadius(angle)-inland;
            const auto experience=island::IslandEnvironmentField::SampleCoastalExperience(
                std::cos(angle)*radius,std::sin(angle)*radius,
                station.signedShoreDistance);
            for (const float weight : {experience.broadBeach,experience.shelteredBay,
                                      experience.rockyShore,experience.mangroveMouth}) {
                Check(std::isfinite(weight) && weight>=0.0f && weight<=1.0f,
                    "coastal experience weights remain finite and normalized");
            }
        }
    }

    const island::VegetationLayout repeatedVegetation(terrain);
    Check(vegetation.Palms().size() >= 180U && vegetation.Palms().size() <= 520U,
          "palm population stays within the intended first performance budget");
    Check(vegetation.Palms().size() == repeatedVegetation.Palms().size(),
          "palm population is deterministic");
    Check(vegetation.Obstacles().size() >= vegetation.Palms().size(),
          "every palm has a trunk collision proxy and fallen stems may add visible-log proxies");
    CheckNear(island::VegetationLayout::PrimaryPathCenterX(
                  -island::VegetationLayout::PrimaryPathZOffset),
              island::VegetationLayout::PrimaryPathBaseX, 0.0001f,
              "primary path shape constants identify the authored centre crossing");
    Check(island::VegetationLayout::PrimaryPathMinimumZ
                  < island::VegetationLayout::PrimaryPathMaximumZ
              && island::VegetationLayout::PrimaryPathReservedHalfWidth >= 0.75f
              && island::VegetationLayout::PrimaryPathReservedHalfWidth <= 1.5f,
          "primary path keeps an ordered range and a 1.5 to 3 metre walking reserve");
    Check(island::VegetationLayout::IsReservedRoute(
              island::VegetationLayout::PlayerSpawnX,
              island::VegetationLayout::PlayerSpawnZ)
              && island::VegetationLayout::PlayerSpawnReserveRadius >= 12.0f,
          "authored beach spawn retains a useful vegetation and prop clearance");
    Check(island::VegetationLayout::EastClearing.radius > 20.0f
              && island::VegetationLayout::WestClearing.radius > 20.0f,
          "authored clearings retain useful first-person room dimensions");
    Check(island::VegetationLayout::IsReservedRoute(
              island::VegetationLayout::EastClearing.x,
              island::VegetationLayout::EastClearing.z)
              && island::VegetationLayout::IsReservedRoute(
                  island::VegetationLayout::WestClearing.x,
                  island::VegetationLayout::WestClearing.z),
          "authored clearing material centres remain vegetation-reserved");
    for (int z = -216; z <= 180; z += 18) {
        const float pathX = island::VegetationLayout::PrimaryPathCenterX(static_cast<float>(z));
        Check(island::VegetationLayout::IsReservedRoute(pathX, static_cast<float>(z)),
              "the authored primary path centre remains reserved from vegetation");
    }
    std::size_t youngPalms = 0U, fallenPalms = 0U, brokenPalms = 0U;
    for (std::size_t index = 0; index < vegetation.Palms().size(); ++index) {
        const island::PalmPlacement& palm = vegetation.Palms()[index];
        const island::PalmPlacement& repeated = repeatedVegetation.Palms()[index];
        const island::TerrainSample sample = terrain.Sample(palm.x, palm.z);
        Check(palm.x == repeated.x && palm.y == repeated.y && palm.z == repeated.z
                  && palm.yawRadians == repeated.yawRadians && palm.scale == repeated.scale
                  && palm.damageYawRadians == repeated.damageYawRadians
                  && palm.variant == repeated.variant && palm.condition == repeated.condition,
              "palm placement data is bitwise deterministic for one seed");
        Check(sample.signedShoreDistance >= 7.0f && sample.signedShoreDistance <= 158.0f,
              "palms remain inside the authored coastal/interior belt");
        Check(sample.slopeRadians <= 0.4712389f,
              "palms reject terrain steeper than the placement limit");
        Check(!island::VegetationLayout::IsReservedRoute(palm.x, palm.z),
              "palms keep paths, clearings, and player spawn open");
        Check(island::IslandEnvironmentField::SampleInland(
                  palm.x,palm.z,sample.signedShoreDistance).drainage<=0.68f,
              "palm trunks stay outside the active stream channel");
        Check(palm.variant < island::VegetationLayout::PalmVariantCount,
              "palm variant index remains in range");
        const bool youngPalm = palm.scale < island::VegetationLayout::PalmMinimumScale;
        Check(youngPalm
                  ? palm.scale >= island::VegetationLayout::PalmJuvenileMinimumScale
                      && palm.scale <= island::VegetationLayout::PalmJuvenileMaximumScale
                  : palm.scale <= island::VegetationLayout::PalmMaximumScale,
              "palm scale remains in the authored adult or juvenile range");
        youngPalms += youngPalm ? 1U : 0U;
        fallenPalms += palm.condition == island::PalmCondition::Fallen ? 1U : 0U;
        brokenPalms += palm.condition == island::PalmCondition::Broken ? 1U : 0U;
        Check(!youngPalm || palm.condition == island::PalmCondition::Living,
              "juvenile palms remain upright while only established adults carry storm history");
        Check(std::isfinite(palm.damageYawRadians),
              "palm storm orientation remains finite");
        if (palm.condition == island::PalmCondition::Fallen) {
            const float length = (8.6f + 0.45f * static_cast<float>(palm.variant)) * palm.scale;
            for (int step = 1; step <= 6; ++step) {
                const float distance = length * static_cast<float>(step) / 6.0f;
                const float x = palm.x + std::sin(palm.damageYawRadians) * distance;
                const float z = palm.z + std::cos(palm.damageYawRadians) * distance;
                const island::TerrainSample stemSample = terrain.Sample(x, z);
                Check(stemSample.signedShoreDistance >= 5.0f
                          && stemSample.slopeRadians <= 0.31f
                          && std::abs(stemSample.height - palm.y) <= 0.85f,
                      "fallen palm stem remains on stable dry terrain");
                Check(!island::VegetationLayout::OverlapsReservedRoute(x, z, 0.65f),
                      "fallen palm stem keeps the complete authored route open");
                for (const island::PalmPlacement& other : vegetation.Palms()) {
                    if (&other == &palm) continue;
                    const float clearance = step == 6 ? 2.2f : 0.85f;
                    Check(std::hypot(other.x - x, other.z - z) >= clearance,
                          "fallen palm stem and crown avoid standing palm trunks");
                }
            }
        }
        const island::CircularObstacle& obstacle = vegetation.Obstacles()[index];
        CheckNear(obstacle.x, palm.x, 0.0001f, "palm obstacle shares its visual x coordinate");
        CheckNear(obstacle.z, palm.z, 0.0001f, "palm obstacle shares its visual z coordinate");
        Check(obstacle.radius > (youngPalm ? 0.12f : 0.20f) && obstacle.radius < 0.45f,
              "palm collision radius stays close to the visible trunk");
    }
    Check(fallenPalms == island::VegetationLayout::FallenPalmTargetCount,
          "the default groves contain the bounded fallen-palm target");
    Check(brokenPalms == island::VegetationLayout::BrokenPalmTargetCount,
          "the default groves contain the bounded storm-broken-palm target");
    Check(vegetation.Obstacles().size()
              == vegetation.Palms().size() + fallenPalms * 5U,
          "each fallen stem adds five collision samples along its visible length");
    {
        // Groves on the coast, few palms inland, young palms beside adults.
        std::size_t grouped = 0U, inland = 0U;
        for (const island::PalmPlacement& palm : vegetation.Palms()) {
            int neighbours = 0;
            for (const island::PalmPlacement& other : vegetation.Palms()) {
                if (&other != &palm && std::hypot(other.x - palm.x, other.z - palm.z) < 12.0f) ++neighbours;
            }
            grouped += neighbours >= 3 ? 1U : 0U;
            inland += palm.shoreDistance > 90.0f ? 1U : 0U;
        }
        const double total = static_cast<double>(vegetation.Palms().size());
        Check(grouped / total >= 0.40 && inland / total <= 0.05,
              "palms form coastal groves with only a few old palms inland");
        Check(youngPalms >= 30U && youngPalms / total <= 0.35,
              "groves recruit young palms without replacing the adults");
        for (std::size_t a = 0; a < vegetation.Palms().size(); ++a) {
            for (std::size_t b = a + 1; b < vegetation.Palms().size(); ++b) {
                const auto& p = vegetation.Palms()[a];
                const auto& q = vegetation.Palms()[b];
                if (p.scale < island::VegetationLayout::PalmMinimumScale
                    || q.scale < island::VegetationLayout::PalmMinimumScale) {
                    Check(std::hypot(p.x - q.x, p.z - q.z) >= 2.2f, "young palms keep their trunks clear");
                }
            }
        }
    }

    {
        // Prevailing wind: exposure is bounded, strongest on the windward coast, fades inland,
        // never negative on the lee shore, and the palm lean stays within ten degrees.
        using namespace island::wind;
        Check(Exposure(230.0f, 0.0f, 5.0f) > 0.95f && Exposure(-230.0f, 0.0f, 5.0f) < 0.3f
                  && Exposure(-230.0f, 0.0f, 5.0f) > 0.2f && Exposure(0.0f, 0.0f, 120.0f) == 0.0f
                  && Exposure(0.0f, 230.0f, 5.0f) > Exposure(-230.0f, 0.0f, 5.0f),
              "wind exposure peaks on the windward east coast and vanishes inland");
        float leeMax = 0.0f, windwardMax = 0.0f;
        for (const island::PalmPlacement& palm : vegetation.Palms()) {
            const float lean = PalmLeanRadians(Exposure(palm.x, palm.z, palm.shoreDistance));
            Check(lean >= 0.0f && lean <= PalmMaximumLeanRadians, "palm lean stays within the authored limit");
            Check(std::abs(palm.shoreDistance - terrain.Sample(palm.x, palm.z).signedShoreDistance) < 0.001f,
                  "palm placements retain their shore distance");
            (palm.x > 0.0f ? windwardMax : leeMax) = std::max(palm.x > 0.0f ? windwardMax : leeMax, lean);
        }
        Check(windwardMax > leeMax && windwardMax > 0.12f, "east-coast palms lean more than west-coast palms");
        bool rejected = false;
        try { (void)Exposure(std::numeric_limits<float>::quiet_NaN(), 0.0f, 1.0f); }
        catch (const std::invalid_argument&) { rejected = true; }
        Check(rejected, "wind exposure rejects non-finite input");
        using island::sky_cloud::DriftAngle;
        CheckNear(DriftAngle(0.0f),0.0f,0.000001f,
                  "cloud drift begins at the authored fair-weather phase");
        CheckNear(DriftAngle(600.0f),0.21f,0.00001f,
                  "cloud drift remains very slow over a ten-minute walk");
        const float turnSeconds=2.0f*std::numbers::pi_v<float>
            / island::sky_cloud::AngularSpeedRadiansPerSecond;
        CheckNear(DriftAngle(turnSeconds),0.0f,0.00001f,
                  "cloud drift wraps on a shader-periodic complete turn");
        CheckNear(DriftAngle(std::numeric_limits<float>::quiet_NaN()),0.0f,0.000001f,
                  "cloud drift sanitizes non-finite clocks");
    }
    const island::GrassLayout grass(terrain, canopyLight, groundCover);
    const island::GrassLayout repeatedGrass(terrain, canopyLight, groundCover);
    {
        // The cached field bake and the direct reference agree exactly; cover stays bounded,
        // rises with light and patch potential, and vanishes on sand and steep slopes.
        const island::GroundCoverField repeatedCover;
        Check(std::ranges::equal(groundCover.PatchTexels(), repeatedCover.PatchTexels()),
              "ground-cover patch field is deterministic");
        int mismatches = 0;
        std::array<int, 2> extremes{};
        for (int row = 0; row < groundCover.Resolution(); row += 17) {
            for (int column = 0; column < groundCover.Resolution(); column += 13) {
                const float x = island::CanopyLightField::TexelCoordinate(column, groundCover.Resolution());
                const float z = island::CanopyLightField::TexelCoordinate(row, groundCover.Resolution());
                const auto expected = static_cast<std::uint8_t>(std::lround(255.0f
                    * island::GroundCover::PatchFromNoise(island::GroundCoverField::Noise(
                        x, z, island::GroundCoverField::DefaultSeed))));
                const auto texel = groundCover.PatchTexels()[static_cast<std::size_t>(row)
                    * groundCover.Resolution() + column];
                mismatches += texel != expected;
                extremes[0] += texel < 26;
                extremes[1] += texel > 229;
            }
        }
        Check(mismatches == 0, "cached ground-cover bake matches the direct noise reference");
        // GLSL ES 3.00 reserves identifiers that desktop Mesa GLSL 330 accepts; a reserved
        // parameter name compiled natively but sent the Web terrain to its stock fallback.
        for (const std::string& shader : {island::GroundCover::Shader(), island::TrunkSurface::Shader()}) {
            for (const char* reserved : {"patch", "sample", "filter", "input", "output", "active",
                                         "common", "partition", "resource", "subroutine"}) {
                const std::regex word("\\b" + std::string(reserved) + "\\b");
                Check(!std::regex_search(shader, word),
                      "generated GLSL avoids identifiers reserved by GLSL ES 3.00");
            }
        }
        Check(extremes[0] > 200 && extremes[1] > 200,
              "ground-cover patches include both sparse and rich ground");
        float previous = -1.0f;
        for (int step = 0; step <= 20; ++step) {
            const float value = static_cast<float>(step) / 20.0f;
            const float lit = island::GroundCover::Density(value, 1.0f, 80.0f, 0.1f);
            const float rich = island::GroundCover::Density(1.0f, value, 80.0f, 0.1f);
            Check(lit >= previous && lit >= 0.0f && lit <= 1.0f && rich >= 0.0f && rich <= 1.0f,
                  "ground cover is bounded and rises with canopy transmission");
            previous = lit;
        }
        Check(island::GroundCover::Density(1.0f, 1.0f, 10.0f, 0.1f) == 0.0f
                  && island::GroundCover::Density(1.0f, 1.0f, 80.0f, 0.6f) == 0.0f
                  && island::GroundCover::Density(0.0f, 1.0f, 80.0f, 0.1f)
                      < 0.3f * island::GroundCover::Density(1.0f, 1.0f, 80.0f, 0.1f),
              "ground cover vanishes on sand and steep slopes and thins in deep shade");
        float previousCoastalCover = 1.0f;
        for (int step = 0; step <= 20; ++step) {
            const float beach = static_cast<float>(step) / 20.0f;
            const float density = island::GroundCover::Density(1.0f, 1.0f, 40.0f, 0.1f, 0.0f, beach);
            Check(density >= 0.0f && density <= previousCoastalCover,
                  "shared grass and turf response decreases monotonically with sandy substrate");
            previousCoastalCover = density;
        }
        Check(island::GroundCover::Density(1.0f, 1.0f, 40.0f, 0.1f, 0.0f, 0.8f) == 0.0f
                  && island::GroundCover::Density(1.0f, 1.0f, 40.0f, 0.1f, 1.0f, 0.0f) == 0.0f
                  && island::GroundCover::Density(1.0f, 1.0f, 40.0f, 0.1f, 0.0f, 0.0f) == 1.0f,
              "sandy apron and bare rock suppress turf independently of distance from shore");
        const float dryCover=island::GroundCover::Density(1,1,80,.1f,0,0,0,0);
        const float dampCover=island::GroundCover::Density(1,1,80,.1f,0,0,1,0);
        const float channelCover=island::GroundCover::Density(1,1,80,.1f,0,0,1,1);
        Check(dampCover>=dryCover && dampCover<=1
            && channelCover<dryCover*.25f,
            "shared moisture supports bank cover while concentrated drainage suppresses channel grass");
        const auto wideBeach = coastalStation(std::numbers::pi_v<float>, 25.0f);
        const auto ordinaryBeach = coastalStation(-1.5f, 25.0f);
        const float bayCover = island::GroundCover::Density(1.0f, 1.0f, 25.0f, 0.1f,
            wideBeach.coastalRockiness, wideBeach.beachSuitability);
        const float ordinaryCover = island::GroundCover::Density(1.0f, 1.0f, 25.0f, 0.1f,
            ordinaryBeach.coastalRockiness, ordinaryBeach.beachSuitability);
        Check(bayCover < ordinaryCover * 0.5f,
              "wide bay suppresses turf more than ordinary coast at the same shore distance");
        for (const float invalid : {std::numeric_limits<float>::quiet_NaN(), -0.1f, 1.1f}) {
            bool rejected = false;
            try { (void)island::GroundCover::Density(invalid, 0.5f, 50.0f, 0.1f); }
            catch (const std::invalid_argument&) { rejected = true; }
            Check(rejected, "ground cover rejects invalid transmission");
            for (bool invalidRock : {false, true}) {
                rejected = false;
                try {
                    (void)island::GroundCover::Density(0.5f, 0.5f, 50.0f, 0.1f,
                        invalidRock ? invalid : 0.0f, invalidRock ? 0.0f : invalid);
                } catch (const std::invalid_argument&) { rejected = true; }
                Check(rejected, "ground cover rejects invalid coastal controls");
            }
        }
    }
    {
        // Per-patch grass tiers (loop pass 209): each patch morphs between meshes by its own
        // distance, the weights stay continuous and complete, a cell's draw ranges cover every
        // patch that carries weight in a tier, and nothing inside NearOnly - morph leaves the
        // near mesh, where the old split by the cell centre drew patches 51 m away as middle.
        using island::GrassLod;
        bool complete = true, covered = true, continuous = true, nearHeld = true;
        for (const int count : {1, 7, 64, 333}) {
            for (float nearest = 0.0f; nearest <= 130.0f; nearest += 3.7f) {
                for (const float span : {0.0f, 9.0f, 26.5f, 53.0f}) {
                    const float farthest = nearest + span;
                    const auto ranges = GrassLod::Ranges(count, nearest, farthest);
                    for (int i = 0; i < count; ++i) {
                        const float rank = (static_cast<float>(i) + 0.5f) / static_cast<float>(count);
                        for (float d = nearest; d <= farthest; d += 0.5f) {
                            const auto w = GrassLod::PatchWeights(rank, d);
                            if (std::abs(w.nearWeight + w.middleWeight + w.farWeight - 1.0f) > 1e-4f
                                || w.nearWeight < 0 || w.middleWeight < 0 || w.farWeight < 0) complete = false;
                            if (w.nearWeight > 0 && (i < ranges.nearBegin || i >= ranges.nearEnd)) covered = false;
                            if (w.middleWeight > 0 && (i < ranges.middleBegin || i >= ranges.middleEnd)) covered = false;
                            if (w.farWeight > 0 && (i < ranges.farBegin || i >= ranges.farEnd)) covered = false;
                        }
                    }
                }
            }
        }
        float largestStep = 0.0f;
        for (float rank = 0.005f; rank < 1.0f; rank += 0.01f) {
            auto previous = GrassLod::PatchWeights(rank, 0.0f);
            for (float d = 0.01f; d <= 130.0f; d += 0.01f) {
                const auto w = GrassLod::PatchWeights(rank, d);
                largestStep = std::max({largestStep, std::abs(w.nearWeight - previous.nearWeight),
                                        std::abs(w.middleWeight - previous.middleWeight),
                                        std::abs(w.farWeight - previous.farWeight)});
                previous = w;
            }
            if (GrassLod::PatchWeights(rank, GrassLod::NearOnlyDistance - GrassLod::MorphHalfWidth).nearWeight != 1.0f)
                nearHeld = false;
        }
        continuous = largestStep <= 0.01f;
        // the old split: a 37.5 m cell centred 70 m away put a share of its patches, the
        // nearest 51.25 m away, on the middle mesh
        const auto oldCentre = GrassLod::Split(100, 70.0f);
        bool newNear = true;
        for (int i = 0; i < 100; ++i)
            if (GrassLod::PatchWeights((static_cast<float>(i) + 0.5f) / 100.0f, 51.25f).nearWeight != 1.0f) newNear = false;
        const auto close = GrassLod::Ranges(100, 10.0f, 50.0f);
        const auto distant = GrassLod::Ranges(100, 105.0f, 130.0f);
        Check(complete, "grass patch tier weights lie in 0..1 and sum to 1");
        Check(covered, "a grass cell's tier draw ranges cover every patch with weight in that tier");
        Check(continuous, "a grass patch morphs between tiers by at most 1 % per centimetre of distance");
        Check(nearHeld, "no grass patch leaves the near mesh nearer than NearOnly minus the morph");
        Check(oldCentre.middleCount > 0 && newNear,
              "per-patch tiers keep near patches near where the cell-centre split thinned them");
        Check(close.nearEnd == 100 && close.middleEnd == close.middleBegin && close.farEnd == close.farBegin
                  && distant.nearEnd == distant.nearBegin,
              "grass cells draw only the tiers their distances can reach");
        std::cout << "grass per-patch tiers: largest morph step " << largestStep << " per cm; old cell split at 70 m drew "
                  << oldCentre.middleCount << " of 100 patches as middle\n";
    }
    {
        // The jungle floor's fades (loop pass 210): staggered per-plant swap distances spread
        // over +-12 % and are deterministic; the far shrink is 1 inside its band, 0 at the
        // maximum distance and continuous between.
        namespace fade = island::vegetation_fade;
        float lowest = 1e9f, highest = 0.0f;
        bool deterministic = true;
        for (float x = -200.0f; x <= 200.0f; x += 3.1f)
            for (float z = -200.0f; z <= 200.0f; z += 2.9f) {
                const float d = fade::StaggeredNearDistance(44.0f, x, z);
                lowest = std::min(lowest, d);
                highest = std::max(highest, d);
                if (d != fade::StaggeredNearDistance(44.0f, x, z)) deterministic = false;
            }
        float largestStep = 0.0f, previous = 1.0f;
        for (float d = 0.0f; d <= 60.0f; d += 0.01f) {
            const float s = fade::FarShrink(d, 56.1f);
            largestStep = std::max(largestStep, std::abs(s - previous));
            previous = s;
        }
        Check(deterministic && lowest >= 44.0f * 0.88f - 1e-3f && highest <= 44.0f * 1.12f + 1e-3f
                  && highest - lowest > 44.0f * 0.2f,
              "jungle floor plants swap meshes at their own distances spread over +-12 %");
        Check(fade::FarShrink(56.1f * 0.78f, 56.1f) == 1.0f && fade::FarShrink(56.1f, 56.1f) == 0.0f
                  && largestStep < 0.002f,
              "jungle floor plants shrink smoothly to nothing over the last 22 % of their range");
        float treeStep = 0.0f, treePrevious = 1.0f;
        for (float d = 0.0f; d <= 300.0f; d += 0.01f) {
            const float s = fade::FarShrink(d, 286.0f, fade::TreeShrinkFraction);
            treeStep = std::max(treeStep, std::abs(s - treePrevious));
            treePrevious = s;
        }
        Check(fade::FarShrink(286.0f * 0.88f, 286.0f, fade::TreeShrinkFraction) == 1.0f
                  && fade::FarShrink(286.0f, 286.0f, fade::TreeShrinkFraction) == 0.0f && treeStep < 0.001f,
              "trees and palms shrink smoothly to nothing over the last 12 % of their range");
    }
    {
        // Coastal scrub on the bare banks (loop pass 223): deterministic, only where the other
        // plant rules leave the ground bare (a steep bank or the strip behind the beach), clear
        // of the reserved routes, the hut's clearing and the given circles.
        const island::IslandTerrain terrain;
        const island::SlopeScrubLayout::ClearCircle circle{0.0f, 0.0f, 30.0f};
        const auto scrub = island::SlopeScrubLayout::Generate(terrain, std::span(&circle, 1));
        const auto again = island::SlopeScrubLayout::Generate(terrain, std::span(&circle, 1));
        bool same = scrub.size() == again.size(), ruled = true, clear = true, steepShare = false;
        std::size_t steep = 0;
        for (std::size_t i = 0; i < scrub.size(); ++i) {
            const auto& b = scrub[i];
            if (same && (b.x != again[i].x || b.z != again[i].z || b.variant != again[i].variant)) same = false;
            const auto sample = terrain.Sample(b.x, b.z);
            if (!island::SlopeScrubLayout::Accepts(sample.height, sample.slopeRadians, sample.signedShoreDistance,
                                                   sample.coastalRockiness)) ruled = false;
            if (island::VegetationLayout::IsReservedRoute(b.x, b.z) || std::hypot(b.x, b.z) < 30.0f) clear = false;
            if (sample.slopeRadians >= island::SlopeScrubLayout::SteepSlope) ++steep;
        }
        steepShare = steep * 4 >= scrub.size();
        Check(same, "the coastal scrub is deterministic");
        Check(scrub.size() >= 300 && scrub.size() <= 3000, "the coastal scrub covers the bare banks with a bounded count");
        Check(ruled, "the coastal scrub grows only on steep banks and beach backs above the dry sand");
        Check(clear, "the coastal scrub stays off the reserved routes and the clear circles");
        Check(steepShare, "at least a quarter of the coastal scrub holds the steep banks");
        std::cout << "coastal scrub: " << scrub.size() << " bushes, " << steep << " on banks steeper than "
                  << island::SlopeScrubLayout::SteepSlope << " rad\n";
    }
    {
        // The day/night clock (operator request 2026-10-20): 24 game hours in 24 real minutes,
        // the sun's path fitted to the former fixed sun at 14:00, sunrise and sunset at six,
        // a dark night with stars, a moon opposite the sun at full, and no jumps in the light.
        using island::DayNightCycle;
        const float length = std::sqrt(island::environment::SunDirectionX * island::environment::SunDirectionX
            + island::environment::SunDirectionY * island::environment::SunDirectionY
            + island::environment::SunDirectionZ * island::environment::SunDirectionZ);
        const auto at14 = DayNightCycle::SunDirectionAt(DayNightCycle::DefaultHour);
        Check(std::abs(at14.x - island::environment::SunDirectionX / length) < 1e-3f
                  && std::abs(at14.y - island::environment::SunDirectionY / length) < 1e-3f
                  && std::abs(at14.z - island::environment::SunDirectionZ / length) < 1e-3f,
              "at 14:00 the sun stands where the former fixed sun was");
        const auto sunrise = DayNightCycle::LightingAt(6.0f, 2.0f), sunset = DayNightCycle::LightingAt(18.0f, 2.0f);
        const auto noon = DayNightCycle::LightingAt(12.0f, 2.0f), midnight = DayNightCycle::LightingAt(0.0f, 2.0f);
        const auto afternoon = DayNightCycle::LightingAt(DayNightCycle::DefaultHour, 2.0f);
        Check(std::abs(sunrise.sunElevationDegrees) < 0.5f && std::abs(sunset.sunElevationDegrees) < 0.5f
                  && sunrise.toSun.x * sunset.toSun.x + sunrise.toSun.z * sunset.toSun.z < -0.9f,
              "the sun rises at six on one side and sets at six on the other");
        Check(std::abs(noon.sunElevationDegrees - (90.0f - DayNightCycle::LatitudeDegrees())) < 0.5f,
              "the noon sun stands at ninety degrees less the latitude");
        Check(afternoon.grade.x == 1.0f && afternoon.grade.y == 1.0f && afternoon.grade.z == 1.0f
                  && afternoon.shadowStrength == 1.0f && afternoon.starVisibility == 0.0f,
              "the afternoon is full daylight with full shadows and no stars");
        Check(std::max({midnight.grade.x, midnight.grade.y, midnight.grade.z}) < 0.30f
                  && midnight.grade.z > midnight.grade.x && midnight.starVisibility > 0.99f
                  && midnight.sunVisibility == 0.0f && midnight.shadowStrength == 0.0f,
              "midnight is dark and blue, starry, without sun or shadows");
        {
            // The stars wheel with the hour: the sky turns about the celestial pole with the sun
            // (midnight's sun carried to every hour's), the moon too (on its day), the pole stays
            // put at the latitude's height over the horizon, and nothing is stretched.
            const auto close = [](const island::DayVec3& a, const island::DayVec3& b) {
                return std::abs(a.x - b.x) < 1e-4f && std::abs(a.y - b.y) < 1e-4f && std::abs(a.z - b.z) < 1e-4f;
            };
            const auto moonAt = [](const float hour) { return DayNightCycle::LightingAt(hour, 3.0f).toMoon; };
            bool withTheSun = true, withTheMoon = true, rigid = true;
            for (const float hour : {0.0f, 3.0f, 7.5f, 12.0f, 18.25f, 23.9f}) {
                withTheSun = withTheSun && close(DayNightCycle::SkyAt(DayNightCycle::SunDirectionAt(0.0f), hour),
                                                 DayNightCycle::SunDirectionAt(hour));
                withTheMoon = withTheMoon && close(DayNightCycle::SkyAt(moonAt(0.0f), hour), moonAt(hour));
                const auto star = DayNightCycle::SkyAt({0.6f, 0.0f, 0.8f}, hour);
                rigid = rigid && std::abs(star.x * star.x + star.y * star.y + star.z * star.z - 1.0f) < 1e-4f;
            }
            const float latitude = DayNightCycle::LatitudeDegrees() * std::numbers::pi_v<float> / 180.0f;
            const float east = DayNightCycle::EastAxisDegrees() * std::numbers::pi_v<float> / 180.0f;
            const island::DayVec3 pole{-std::cos(latitude) * std::sin(east), std::sin(latitude), std::cos(latitude) * std::cos(east)};
            Check(withTheSun && withTheMoon && rigid && close(DayNightCycle::SkyAt(pole, 5.0f), pole)
                      && close(DayNightCycle::SkyAt(pole, 17.0f), pole),
                  "the night sky wheels about the celestial pole with the sun and the moon, rigidly");
            Check(midnight.hour == 0.0f && afternoon.hour == DayNightCycle::DefaultHour,
                  "the hour's light carries its hour (the stars turn with it)");
        }
        const auto fullMoon = DayNightCycle::LightingAt(0.0f, 0.5f * DayNightCycle::LunarDays);
        const auto newMoon = DayNightCycle::LightingAt(0.0f, 0.0f);
        Check(fullMoon.moonIllumination > 0.99f && newMoon.moonIllumination < 0.01f
                  && fullMoon.moonElevationDegrees > 60.0f && fullMoon.moonLight > newMoon.moonLight + 0.5f
                  && fullMoon.grade.z > newMoon.grade.z,
              "a full moon stands high at midnight and lightens the night; a new moon does not");
        float largestStep = 0.0f;
        auto previous = DayNightCycle::LightingAt(0.0f, 2.0f);
        for (int minute = 1; minute <= 24 * 60; ++minute) {
            const auto now = DayNightCycle::LightingAt(static_cast<float>(minute) / 60.0f, 2.0f + minute / 1440.0f);
            largestStep = std::max({largestStep, std::abs(now.grade.x - previous.grade.x),
                                    std::abs(now.grade.y - previous.grade.y), std::abs(now.grade.z - previous.grade.z),
                                    std::abs(now.skyHorizon.x - previous.skyHorizon.x),
                                    std::abs(now.shadowStrength - previous.shadowStrength)});
            previous = now;
        }
        Check(largestStep < 0.04f, "the light changes smoothly, under 4 % per game minute");
        DayNightCycle clock;
        const float startDay = clock.Day();
        for (int second = 0; second < 1440; ++second) clock.Advance(1.0f);
        Check(std::abs(clock.Day() - startDay - 1.0f) < 1e-3f && std::abs(clock.Hour() - DayNightCycle::DefaultHour) < 0.05f,
              "twenty-four real minutes make one game day");
        clock.SetFrozen(true);
        const float held = clock.Hour();
        clock.Advance(300.0f);
        Check(clock.Hour() == held, "a frozen clock holds its hour");
        std::cout << "day/night: latitude " << DayNightCycle::LatitudeDegrees() << " deg, east axis "
                  << DayNightCycle::EastAxisDegrees() << " deg, largest light step " << largestStep << " per game minute\n";
    }
    {
        // The weather (operator request 2026-10-20): reproducible spells of 1.5-5 game hours
        // following the transition table, eased; the former look is partly cloudy and changes
        // nothing; rain wets the ground and it dries; a storm throws lightning; overcast weakens
        // the sun's shadows and greys the scene.
        using island::WeatherKind;
        using island::WeatherSystem;
        island::WeatherSystem a(1234U), b(1234U);
        bool repeatable = true, spellsBounded = true, legal = true, sawRain = false, sawStorm = false;
        WeatherKind previous = a.State().kind;
        float spellHours = 0.0f;
        for (int step = 0; step < 24 * 60 * 20; ++step) {   // twenty game days, one game minute a step
            a.Advance(1.0f, 1.0f / 60.0f);
            b.Advance(1.0f, 1.0f / 60.0f);
            if (a.State().kind != b.State().kind || a.State().rain != b.State().rain) repeatable = false;
            spellHours += 1.0f / 60.0f;
            if (a.State().kind != previous) {
                if (spellHours < WeatherSystem::MinimumSpellHours - 0.02f || spellHours > WeatherSystem::MaximumSpellHours + 0.02f)
                    spellsBounded = false;
                if ((previous == WeatherKind::Clear && a.State().kind == WeatherKind::Thunderstorm)
                    || (previous == WeatherKind::Thunderstorm && a.State().kind == WeatherKind::Clear)) legal = false;
                previous = a.State().kind;
                spellHours = 0.0f;
            }
            sawRain = sawRain || a.State().rain > 0.3f;
            sawStorm = sawStorm || a.State().kind == WeatherKind::Thunderstorm;
        }
        Check(repeatable, "the same seed repeats the same weather");
        Check(spellsBounded, "weather spells last 1.5 to 5 game hours");
        Check(legal, "fair weather never turns straight into a storm or back");
        Check(sawRain && sawStorm, "twenty game days bring rain and a thunderstorm");
        island::WeatherSystem wet(7U);
        wet.Force(WeatherKind::PartlyCloudy);
        auto day = island::DayNightCycle::LightingAt(14.0f, 2.0f);
        auto same = day;
        WeatherSystem::ApplyTo(same, wet.State());
        Check(same.grade.x == day.grade.x && same.shadowStrength == day.shadowStrength && same.skyZenith.z == day.skyZenith.z,
              "the former partly cloudy weather leaves the light unchanged");
        wet.Force(WeatherKind::Showers);
        wet.Release();
        island::WeatherSystem storm(9U);
        storm.Force(WeatherKind::Thunderstorm);
        float soaked = 0.0f;
        for (int s = 0; s < 120; ++s) { storm.Advance(1.0f, 0.0f); soaked = storm.State().wetness; }
        std::uint32_t strikes = storm.State().strikes;
        bool flashed = false;
        for (int s = 0; s < 600; ++s) { storm.Advance(0.05f, 0.0f); flashed = flashed || storm.State().flash > 0.9f; }
        Check(soaked > 0.95f && storm.State().strikes > strikes && flashed, "a storm soaks the ground and throws lightning");
        storm.Force(WeatherKind::Clear);
        storm.Release();
        island::WeatherSystem drying(9U);
        drying.Force(WeatherKind::Thunderstorm);
        for (int s = 0; s < 120; ++s) drying.Advance(1.0f, 0.0f);
        drying.Force(WeatherKind::Clear);
        drying.Advance(1.0f, 0.0f);
        Check(drying.State().wetness == 0.0f, "forcing clear weather leaves the ground dry");
        island::WeatherSystem dryer(9U);
        dryer.Force(WeatherKind::Overcast);
        auto overcast = day;
        WeatherSystem::ApplyTo(overcast, dryer.State());
        Check(overcast.shadowStrength < 0.3f && overcast.grade.x < day.grade.x && overcast.sunVisibility < 0.3f,
              "overcast weakens the sun's shadows, hides its disc and dims the scene");
        Check(island::WeatherFromName("storm") == WeatherKind::Thunderstorm && !island::WeatherFromName("snow"),
              "the weather names parse, and there is no snow on a tropical island");
        {
            // The sea runs higher with the storm's wind (and never lower than the trade wind's).
            const auto saved = island::weather_now::Current();
            island::WeatherState calm, storm;
            calm.wind = 0.8f;
            storm.wind = 2.0f;
            island::weather_now::Set(calm);
            const float calmSea = island::weather_now::SeaState();
            island::weather_now::Set(storm);
            const float stormSea = island::weather_now::SeaState();
            island::weather_now::Set(saved);
            Check(calmSea == 1.0f && std::abs(stormSea - 1.7f) < 1e-4f, "the sea runs 1.7 times as high in a storm's wind, never below the trade wind's");
        }
    }
    {
        // Fireflies: none by day or in the rain; at night some flash at any moment, low over
        // the ground, and over a few seconds every one of them flashes.
        const island::IslandTerrain fireflyTerrain;
        island::FireflySwarm day(fireflyTerrain), night(fireflyTerrain), wet(fireflyTerrain);
        std::size_t dayGlow = 0, wetGlow = 0, nightPeak = 0;
        std::array<bool, island::FireflySwarm::Count> flashed{};
        bool low = true;
        for (int i = 0; i < 60 * 8; ++i) {
            day.Update(1.0f / 60.0f, 0.0f, 0.0f);
            wet.Update(1.0f / 60.0f, 1.0f, 0.9f);
            night.Update(1.0f / 60.0f, 1.0f, 0.0f);
            dayGlow += day.GlowingCount();
            wetGlow += wet.GlowingCount();
            nightPeak = std::max(nightPeak, night.GlowingCount());
            for (std::size_t f = 0; f < island::FireflySwarm::Count; ++f) {
                const auto& fly = night.Flies()[f];
                if (fly.glow > 0.05f) flashed[f] = true;
                low = low && fly.y - std::max(fireflyTerrain.Height(fly.x, fly.z), 0.0f) < 3.5f;
            }
        }
        const bool all = std::all_of(flashed.begin(), flashed.end(), [](bool b) { return b; });
        Check(dayGlow == 0 && wetGlow == 0 && nightPeak >= 6 && all && low,
              "fireflies only on dry nights, every one flashing within seconds, low over the ground");
        // Storm gusts: on the live clock the fronts come about twice as often in a storm as in
        // the trade wind, and a change of wind never makes a front jump.
        {
            namespace gust = island::WindGust;
            const auto passes = [](const float wind) {
                gust::Live() = gust::Clock{};
                int fronts = 0;
                bool inFront = false;
                for (int i = 0; i < 60 * 120; ++i) {
                    gust::Advance(1.0f / 60.0f, wind);
                    const bool now = gust::Strength(0.0f, 0.0f, static_cast<float>(i) / 60.0f) > 0.3f;
                    if (now && !inFront) ++fronts;
                    inFront = now;
                }
                return fronts;
            };
            const int trade = passes(1.0f), storm = passes(2.0f);
            gust::Live() = gust::Clock{};
            float worstStep = 0.0f, previous = -1.0f;
            for (int i = 0; i < 60 * 60; ++i) {
                gust::Advance(1.0f / 60.0f, 1.0f + std::min(1.0f, static_cast<float>(i) / 1800.0f));   // the wind rising over 30 s
                const float now = gust::Strength(-30.0f, 0.0f, static_cast<float>(i) / 60.0f);
                if (previous >= 0.0f) worstStep = std::max(worstStep, std::abs(now - previous));
                previous = now;
            }
            gust::Live() = gust::Clock{};
            Check(storm >= trade * 2 && trade >= 4 && worstStep < 0.08f,
                  "storm gust fronts come twice as often as the trade wind's, and a rising wind never makes them jump");
        }
        // Morning mist: banks at first light, gone by mid-morning, none at noon, in a storm or
        // in the rain, lying low on damp ground.
        {
            using island::MorningMistField;
            const bool timing = MorningMistField::Thickness(6.0f, 1.0f, 0.0f) > 0.95f
                && MorningMistField::Thickness(4.0f, 1.0f, 0.0f) < 0.01f
                && MorningMistField::Thickness(9.0f, 1.0f, 0.0f) < 0.01f
                && MorningMistField::Thickness(14.0f, 1.0f, 0.0f) < 0.01f
                && MorningMistField::Thickness(6.0f, 2.0f, 0.0f) < 0.01f
                && MorningMistField::Thickness(6.0f, 1.0f, 0.8f) < 0.01f;
            const island::IslandTerrain mistTerrain;
            MorningMistField mist(mistTerrain);
            mist.Update(1.0f / 60.0f, 1.0f, 1.0f, 0.0f);
            bool low = true;
            std::size_t thick = 0;
            for (const auto& puff : mist.Puffs()) {
                low = low && puff.y - mistTerrain.Height(puff.x, puff.z) < 4.0f;
                thick += puff.active ? 1U : 0U;
            }
            Check(timing && mist.SiteCount() >= 60 && thick == mist.SiteCount() && low,
                  "morning mist lies low on the damp ground at first light only, and not in wind or rain");
        }
        // Shooting stars: about one a minute, each under a second, sliding down the sky.
        {
            int streaks = 0;
            bool shortLived = true, inSky = true;
            double started = -1.0;
            for (int i = 0; i < 60 * 600; ++i) {
                const double t = static_cast<double>(i) / 60.0;
                const auto m = island::shooting_stars::At(t);
                if (m.visible && started < 0.0) { started = t; ++streaks; inSky = inSky && m.elevation > 0.4f && m.elevation < 1.2f; }
                if (!m.visible && started >= 0.0) { shortLived = shortLived && t - started < 0.8; started = -1.0; }
            }
            Check(streaks >= 6 && streaks <= 13 && shortLived && inSky, "about one shooting star a minute, each under a second, high in the sky");
        }
        // Satellites: in the twilight hours (the sun 5-20 degrees down) a steady point glides
        // across the sky now and then, always above the horizon, moving smoothly; none in the
        // deep night or by day.
        {
            int twilightFrames = 0, passes = 0;
            bool aboveHorizon = true, smooth = true, was = false;
            float lastX = 0.0f, lastY = 0.0f, lastZ = 0.0f;
            for (int step = 0; step < 20 * 240; ++step) {   // twenty windows, a second apart
                const auto pass = island::satellites::At(static_cast<double>(step), -10.0f);
                if (pass.visible) {
                    ++twilightFrames;
                    aboveHorizon = aboveHorizon && pass.y > 0.0f;
                    if (!was) ++passes;
                    else smooth = smooth && std::hypot(pass.x - lastX, pass.y - lastY, pass.z - lastZ) < 0.04f;
                    lastX = pass.x; lastY = pass.y; lastZ = pass.z;
                }
                was = pass.visible;
            }
            bool deepNight = false, day = false;
            for (int step = 0; step < 20 * 240; step += 7) {
                deepNight = deepNight || island::satellites::At(static_cast<double>(step), -30.0f).visible;
                day = day || island::satellites::At(static_cast<double>(step), 10.0f).visible;
            }
            Check(passes >= 6 && passes <= 18 && twilightFrames > 600 && aboveHorizon && smooth && !deepNight && !day,
                  "satellites glide across the twilight sky now and then, above the horizon, none in the deep night or by day");
        }
        // Whales offshore: a surfacing every 35-85 s, 330-620 m out, its blow standing up
        // several metres and gone within six seconds.
        {
            island::WhalePod whales;
            int surfacings = 0;
            bool farOut = true, blowRose = false, blowGone = true;
            bool was = false;
            float sinceStart = 0.0f;
            for (int step = 0; step < 60 * 600; ++step) {
                whales.Update(1.0f / 60.0f, 1.0f, 0.0f);
                const auto& w = whales.Surfacing();
                if (w.active && !was) {
                    ++surfacings;
                    const float r = std::hypot(w.x, w.z);
                    farOut = farOut && r > 320.0f && r < 640.0f;
                    sinceStart = 0.0f;
                }
                was = w.active;
                sinceStart += 1.0f / 60.0f;
                for (const auto& puff : whales.Blow()) {
                    if (puff.active && puff.y > 4.0f) blowRose = true;
                    if (puff.active && sinceStart > 6.0f && surfacings > 0) blowGone = false;
                }
            }
            Check(surfacings >= 7 && surfacings <= 16 && farOut && blowRose && blowGone,
                  "whales surface every minute or so far offshore, their blows standing up and thinning away");
        }
        // A yacht now and then: out part of the time, always 560-720 m off the island's
        // centre, sailing a straight line at its five knots; a fresh line each passage.
        {
            island::SailboatPassage passage;
            int outFrames = 0, passages = 0;
            bool offshore = true, steady = true, was = false;
            float lastX = 0.0f, lastZ = 0.0f, firstHeading = 0.0f, secondHeading = 0.0f;
            for (int step = 0; step < 2 * 3 * 540; ++step) {   // three periods at half a second
                passage.Update(0.5f);
                const auto& boat = passage.State();
                if (boat.active) {
                    ++outFrames;
                    const float r = std::hypot(boat.x, boat.z);
                    offshore = offshore && r > 555.0f && r < 725.0f;
                    if (!was) { ++passages; (passages == 1 ? firstHeading : secondHeading) = boat.headingRadians; }
                    else steady = steady && std::abs(std::hypot(boat.x - lastX, boat.z - lastZ) - 0.5f * island::SailboatPassage::Speed) < 0.01f;
                    lastX = boat.x; lastZ = boat.z;
                }
                was = boat.active;
            }
            Check(passages == 3 && outFrames > 3 * 300 && outFrames < 3 * 1000 && offshore && steady
                      && std::abs(std::remainder(firstHeading - secondHeading, 6.2831853f)) > 0.05f,
                  "a yacht passes well offshore once a period, sailing a straight line at its speed, a fresh line each time");
        }
        // A swimmer at night stirs the plankton into light: sparks round them, many more swimming
        // hard than treading water, each fading within two seconds; none by day or ashore.
        {
            const auto lit = [](const island::SwimGlowField& field) {
                int count = 0;
                for (const auto& speck : field.Specks()) count += speck.Brightness() > 0.01f ? 1 : 0;
                return count;
            };
            island::SwimGlowField treading, swimming, day, ashore;
            for (int step = 0; step < 40; ++step) {
                treading.Update(0.05f, 10.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, true, 1.0f);
                swimming.Update(0.05f, 10.0f, 0.0f, 5.0f, 1.5f, 1.0f, 0.0f, true, 1.0f);
                day.Update(0.05f, 10.0f, 0.0f, 5.0f, 1.5f, 1.0f, 0.0f, true, 0.0f);
                ashore.Update(0.05f, 10.0f, 0.0f, 5.0f, 1.5f, 1.0f, 0.0f, false, 1.0f);
            }
            bool near = true;
            for (const auto& speck : swimming.Specks())
                if (speck.active) near = near && std::hypot(speck.x - 10.0f, speck.z - 5.0f) < 2.5f;
            island::SwimGlowField stopped = swimming;
            for (int step = 0; step < 60; ++step) stopped.Update(0.05f, 10.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, false, 1.0f);
            Check(lit(swimming) > 3 * lit(treading) && lit(treading) > 0 && lit(day) == 0 && lit(ashore) == 0 && near && lit(stopped) == 0,
                  "a swimmer at night stirs plankton light round them, more swimming hard, gone within two seconds");
        }
        // Waterspouts: in a storm one now and then, offshore, for its few minutes; in settled
        // weather none, and one already standing fades away when the storm passes.
        {
            island::Waterspout stormy, settled;
            int spouts = 0, stormyFrames = 0;
            bool offshore = true, was = false;
            for (int step = 0; step < 2 * 10 * 600; ++step) {   // ten windows at half a second
                stormy.Update(0.5f, 1.0f, -1.0f, 0.0f);
                settled.Update(0.5f, 0.0f, -1.0f, 0.0f);
                const auto& s = stormy.State();
                if (s.active) {
                    ++stormyFrames;
                    const float r = std::hypot(s.x, s.z);
                    offshore = offshore && r > 600.0f - 1.0f && r < 780.0f + island::Waterspout::DriftSpeed * island::Waterspout::LifeSeconds + 1.0f;
                    if (!was) ++spouts;
                }
                was = s.active;
                Check(!settled.State().active, "no waterspout in settled weather");
            }
            island::Waterspout passing;
            bool faded = false;
            for (int step = 0; step < 2 * 10 * 600 && !faded; ++step) {
                passing.Update(0.5f, step < 2 * 600 * 10 / 2 ? 1.0f : 0.0f, -1.0f, 0.0f);
                if (step > 2 * 600 * 10 / 2 + 2 * 120) faded = !passing.State().active;
            }
            Check(spouts >= 2 && spouts <= 10 && stormyFrames > 2 * 200 && offshore && faded,
                  "in a storm waterspouts stand offshore now and then for a few minutes, and go when the storm passes");
        }
        // Rain rings on the sea: world-anchored and repeatable, each inside its own cell,
        // more cells striking in heavier rain, none in no rain.
        int light = 0, heavy = 0, none = 0;
        bool inside = true, repeat = true;
        for (int cx = -20; cx < 20; ++cx) {
            for (int cz = -20; cz < 20; ++cz) {
                for (const double t : {3.0, 7.25, 11.5}) {
                    const auto a = island::rain_rings::At(cx, cz, t, 0.25f), b = island::rain_rings::At(cx, cz, t, 1.0f);
                    const auto c = island::rain_rings::At(cx, cz, t, 0.0f), again = island::rain_rings::At(cx, cz, t, 1.0f);
                    light += a.active ? 1 : 0;
                    heavy += b.active ? 1 : 0;
                    none += c.active ? 1 : 0;
                    repeat = repeat && again.active == b.active && again.x == b.x && again.progress == b.progress;
                    if (b.active) {
                        inside = inside && b.x >= cx * island::rain_rings::CellSize && b.x <= (cx + 1) * island::rain_rings::CellSize
                            && b.z >= cz * island::rain_rings::CellSize && b.z <= (cz + 1) * island::rain_rings::CellSize
                            && b.progress >= 0.0f && b.progress < 1.0f;
                    }
                }
            }
        }
        Check(none == 0 && light > 0 && heavy > light * 3 && inside && repeat,
              "rain rings stay in their cells, repeat, and grow many with the rain");
    }
    {
        // The quad bike (operator request 2026-10-20): settles on its springs, reaches a real
        // sport quad's 80 km/h and no more, accelerates and brakes like one, turns, rolls over on
        // a steep side slope but not on a gentle one, is righted by Right(), and repeats.
        using island::QuadBike;
        using island::BikeGround;
        const auto flat = [](float, float) { BikeGround g; g.height = 0.0f; return g; };
        const auto sideSlope = [](const float degrees, const float grip = 1.0f) {
            const float t = std::tan(degrees * 3.14159265f / 180.0f), length = std::sqrt(1.0f + t * t);
            return [t, length, grip](const float x, float) {
                BikeGround g; g.height = x * t; g.normal = {-t / length, 1.0f / length, 0.0f}; g.grip = grip; return g; };
        };
        QuadBike bike({0.0f, 1.0f, 0.0f}, 0.0f);
        bike.Place({0.0f, 0.0f, 0.0f}, 0.0f, flat);
        for (int i = 0; i < 180; ++i) bike.Step(1.0f / 60.0f, {}, flat);
        bool allDown = true;
        for (int w = 0; w < 4; ++w) allDown = allDown && bike.WheelContact(w) && bike.Compression(w) > 0.05f && bike.Compression(w) < 0.6f;
        Check(bike.Up().y > 0.995f && std::abs(bike.Velocity().y) < 0.02f && allDown,
              "the quad settles upright on its four sprung wheels");
        float timeTo50 = -1.0f, topSpeed = 0.0f;
        for (int i = 0; i < 60 * 40; ++i) {
            bike.Step(1.0f / 60.0f, {1.0f, 0.0f, false}, flat);
            if (timeTo50 < 0.0f && bike.Speed() >= 50.0f / 3.6f) timeTo50 = static_cast<float>(i) / 60.0f;
            topSpeed = std::max(topSpeed, bike.Speed());
        }
        Check(topSpeed > 78.0f / 3.6f && topSpeed < 81.0f / 3.6f, "the quad's top speed is a sport quad's 80 km/h");
        Check(timeTo50 > 2.0f && timeTo50 < 6.0f, "the quad reaches 50 km/h in two to six seconds");
        const float brakeStart = bike.Position().z;
        for (int i = 0; i < 60 * 10 && bike.Speed() > 0.3f; ++i) bike.Step(1.0f / 60.0f, {-1.0f, 0.0f, false}, flat);
        const float brakeDistance = bike.Position().z - brakeStart;
        Check(bike.Speed() < 0.5f && brakeDistance > 15.0f && brakeDistance < 50.0f,
              "full brakes stop it from 80 km/h in 15 to 50 metres");
        QuadBike turning({0.0f, 0.0f, 0.0f}, 0.0f);
        turning.Place({0.0f, 0.0f, 0.0f}, 0.0f, flat);
        for (int i = 0; i < 60 * 3; ++i) turning.Step(1.0f / 60.0f, {0.5f, 0.0f, false}, flat);
        const float yawBefore = turning.YawRadians();
        for (int i = 0; i < 60 * 2; ++i) turning.Step(1.0f / 60.0f, {0.35f, 0.6f, false}, flat);
        Check(std::remainder(turning.YawRadians() - yawBefore, 6.2831853f) < -0.5f && turning.Up().y > 0.9f,
              "steering right turns the quad to its right (heading +z, toward -x), on its wheels in a moderate turn");
        const auto turnedRight = turning.Right(), turnedForward = turning.Forward();
        Check(turnedRight.z * turnedForward.x - turnedRight.x * turnedForward.z > 0.99f,
              "the quad's right crossed with its heading points up: its right is the rider's right");
        // on loose dirt a steep side slope slides the quad; on grippy rock it rolls it
        const auto gentle = sideSlope(14.0f), steep = sideSlope(44.0f, 1.3f);   // tips at 42.7, slides at 45.3
        QuadBike across({0.0f, 0.0f, 0.0f}, 0.0f), tipping({0.0f, 0.0f, 0.0f}, 0.0f);
        across.Place({0.0f, 0.0f, 0.0f}, 0.0f, gentle);
        tipping.Place({0.0f, 0.0f, 0.0f}, 0.0f, steep);
        for (int i = 0; i < 60 * 6; ++i) {
            across.Step(1.0f / 60.0f, {0.4f, 0.0f, false}, gentle);
            tipping.Step(1.0f / 60.0f, {0.4f, 0.0f, false}, steep);
        }
        Check(across.Rollovers() == 0 && across.Up().y > 0.9f, "the quad crosses a 14 degree side slope on its wheels");
        Check(tipping.Rollovers() >= 1 && tipping.Overturned(), "on a 44 degree rocky side slope the quad rolls over");
        tipping.Right(flat);
        for (int i = 0; i < 120; ++i) tipping.Step(1.0f / 60.0f, {}, flat);
        Check(tipping.Up().y > 0.99f && !tipping.Overturned(), "Right() sets an overturned quad back on its wheels");
        QuadBike a({0.0f, 0.0f, 0.0f}, 0.3f), b({0.0f, 0.0f, 0.0f}, 0.3f);
        a.Place({0.0f, 0.0f, 0.0f}, 0.3f, gentle); b.Place({0.0f, 0.0f, 0.0f}, 0.3f, gentle);
        for (int i = 0; i < 300; ++i) {
            a.Step(1.0f / 60.0f, {0.8f, 0.3f, false}, gentle);
            b.Step(1.0f / 60.0f, {0.8f, 0.3f, false}, gentle);
        }
        Check(a.Position().x == b.Position().x && a.Position().z == b.Position().z, "the quad's motion repeats exactly");
        std::cout << "quad bike: top " << topSpeed * 3.6f << " km/h, 0-50 km/h in " << timeTo50 << " s, brakes from top in "
                  << brakeDistance << " m\n";

        // Knocks: driving on the flat makes none; a tree at speed makes a hard one.
        QuadBike rider({0.0f, 0.0f, 0.0f}, 0.0f);
        rider.Place({0.0f, 0.0f, 0.0f}, 0.0f, flat);
        const auto tree = [](float x, float z, float radius, float& pushX, float& pushZ) {
            const float dx = x - 0.0f, dz = z - 30.0f, distance = std::hypot(dx, dz), reach = radius + 0.4f;
            if (distance >= reach || distance < 1e-4f) return false;
            pushX = dx / distance * (reach - distance);
            pushZ = dz / distance * (reach - distance);
            return true;
        };
        float flatKnock = 0.0f, treeKnock = 0.0f;
        for (int i = 0; i < 60 * 8; ++i) {
            rider.Step(1.0f / 60.0f, {0.7f, 0.0f, false}, flat, tree);
            if (i > 60 && rider.Position().z < 27.0f) flatKnock = std::max(flatKnock, rider.LastImpact());
            if (rider.Position().z >= 27.0f) treeKnock = std::max(treeKnock, rider.LastImpact());
        }
        Check(flatKnock < 1.5f && treeKnock > 4.0f, "a tree at speed knocks the quad hard; the flat does not");
        // Sliding: rolling straight the tyres hardly slide; a full-lock turn at speed skids them.
        QuadBike skid({0.0f, 0.0f, 0.0f}, 0.0f);
        skid.Place({0.0f, 0.0f, 0.0f}, 0.0f, flat);
        float straightSlide = 0.0f, turnSlide = 0.0f;
        for (int i = 0; i < 60 * 4; ++i) {
            skid.Step(1.0f / 60.0f, {1.0f, 0.0f, false}, flat);
            if (i > 60) straightSlide = std::max(straightSlide, skid.SlideSpeed());
        }
        for (int i = 0; i < 60; ++i) {
            skid.Step(1.0f / 60.0f, {0.6f, 1.0f, false}, flat);
            turnSlide = std::max(turnSlide, skid.SlideSpeed());
        }
        Check(straightSlide < 0.3f && turnSlide > 1.0f, "the tyres slide in a hard turn at speed, not rolling straight");
        // The airbox: water over the seat floods it, wading a stream does not.
        {
            BikeGround deep; deep.height = -1.0f; deep.waterDepth = 1.0f;
            BikeGround ford; ford.height = 4.7f; ford.waterDepth = 0.29f;
            Check(QuadBike::AirboxFlooded({0.0f, -0.40f, 0.0f}, deep) && !QuadBike::AirboxFlooded({0.0f, 5.3f, 0.0f}, ford)
                      && !QuadBike::AirboxFlooded({0.0f, 1.0f, 0.0f}, BikeGround{}),
                  "water over the seat floods the quad's airbox; a stream's ford and dry ground do not");
        }
        // Wading: the water against the tyres takes the speed of a quad running into a stream.
        const auto wading = [](float, float) { BikeGround g; g.height = 0.0f; g.waterDepth = 0.25f; return g; };
        QuadBike dryRun({0.0f, 0.0f, 0.0f}, 0.0f), wet({0.0f, 0.0f, 0.0f}, 0.0f);
        dryRun.Place({0.0f, 0.0f, 0.0f}, 0.0f, flat);
        wet.Place({0.0f, 0.0f, 0.0f}, 0.0f, flat);
        for (int i = 0; i < 60 * 3; ++i) {
            dryRun.Step(1.0f / 60.0f, {1.0f, 0.0f, false}, flat);
            wet.Step(1.0f / 60.0f, {1.0f, 0.0f, false}, flat);
        }
        const float wadeEntry = wet.Speed();
        for (int i = 0; i < 60; ++i) {
            dryRun.Step(1.0f / 60.0f, {1.0f, 0.0f, false}, flat);
            wet.Step(1.0f / 60.0f, {1.0f, 0.0f, false}, wading);
        }
        Check(wet.Speed() < wadeEntry - 1.5f && wet.Speed() > 1.0f && dryRun.Speed() > wadeEntry && wet.Up().y > 0.9f,
              "wading a stream slows the quad at full throttle without stopping it dead or tipping it");
        // The quad in water: its churning a three-second loop, its splash loud at once and dying
        // away, silent at its ends.
        {
            const auto wade = island::GenerateQuadWadeLoop();
            double wadeEnergy = 0.0;
            for (const auto sample : wade.samples) wadeEnergy += static_cast<double>(sample) * sample;
            Check(wade.sampleRate > 0 && wade.samples.size() == static_cast<std::size_t>(wade.sampleRate) * 3U
                      && wadeEnergy / static_cast<double>(wade.samples.size()) > 1.0e5,
                  "the quad's tyres churning water are an audible three-second loop");
            const auto splash = island::GenerateQuadSplash();
            const std::size_t fifth = splash.samples.size() / 5U;
            double head = 0.0, tail = 0.0;
            for (std::size_t i = 0; i < fifth; ++i) {
                head += static_cast<double>(splash.samples[i]) * splash.samples[i];
                const auto last = splash.samples[splash.samples.size() - 1U - i];
                tail += static_cast<double>(last) * last;
            }
            Check(head > tail * 8.0 && head / static_cast<double>(fifth) > 1.0e5 && splash.samples.front() == 0
                      && splash.samples.back() == 0,
                  "the quad's splash is loud at once, dies away and is silent at its ends");
        }
        // The rain where the listener stands: louder and deeper under the crowns, all but gone in
        // the cave, nothing without rain.
        {
            const auto open = island::RainUnderShelter(0.8f, 0.0f, 0.0f);
            const auto forest = island::RainUnderShelter(0.8f, 1.0f, 0.0f);
            const auto cave = island::RainUnderShelter(0.8f, 0.0f, 1.0f);
            Check(forest.volume > open.volume * 1.25f && forest.pitch < open.pitch - 0.15f
                      && cave.volume < open.volume * 0.2f && island::RainUnderShelter(0.0f, 1.0f, 0.0f).volume == 0.0f
                      && std::abs(open.volume - 0.8f * 0.62f) < 1e-6f && open.pitch == 0.0f,
                  "the rain drums deeper under the crowns, is far off in the cave and unchanged in the open");
            const auto cabin = island::RainUnderShelter(0.8f, 0.0f, 0.0f, 1.0f);
            Check(cabin.volume > open.volume * 1.5f && cabin.pitch > open.pitch + 0.2f
                      && island::RainUnderShelter(0.0f, 0.0f, 0.0f, 1.0f).volume == 0.0f,
                  "in the helicopter's cabin the rain drums on the metal skin, loud and tinny");
        }
        // The night chorus: an audible eight-second loop whose crickets live in the treble.
        const auto chorus = island::GenerateNightChorusLoop();
        double chorusEnergy = 0.0, chorusHigh = 0.0;
        for (std::size_t i = 1; i < chorus.samples.size(); ++i) {
            const double s = chorus.samples[i], d = static_cast<double>(chorus.samples[i]) - chorus.samples[i - 1];
            chorusEnergy += s * s;
            chorusHigh += d * d;
        }
        Check(chorus.sampleRate > 0 && chorus.samples.size() == static_cast<std::size_t>(chorus.sampleRate) * 8U
                  && chorusEnergy / static_cast<double>(chorus.samples.size()) > 1.0e5 && chorusHigh > chorusEnergy * 0.3,
              "the night chorus is an audible eight-second loop with its crickets high in the treble");
        // The reef heron's croak: a short harsh call, bright with harmonics, silent at its ends.
        {
            const auto croak = island::GenerateHeronCroak();
            const auto again = island::GenerateHeronCroak();
            double energy = 0.0, rough = 0.0;
            for (std::size_t i = 1; i < croak.samples.size(); ++i) {
                const double s = croak.samples[i], d = static_cast<double>(croak.samples[i]) - croak.samples[i - 1];
                energy += s * s;
                rough += d * d;
            }
            const double seconds = static_cast<double>(croak.samples.size()) / croak.sampleRate;
            Check(croak.sampleRate > 0 && seconds > 0.3 && seconds < 0.6 && croak.samples == again.samples
                      && std::abs(static_cast<int>(croak.samples.front())) < 600
                      && std::abs(static_cast<int>(croak.samples.back())) < 600
                      && energy / static_cast<double>(croak.samples.size()) > 2.0e6 && rough > energy * 0.15,
                  "the reef heron croaks a short, harsh, deterministic call that starts and ends quiet");
        }
        // The scops owl: three soft toots, each a fifth of a second, a couple of seconds apart.
        {
            const auto owl = island::GenerateScopsOwlCall();
            int toots = 0;
            bool inToot = false;
            const std::size_t window = static_cast<std::size_t>(owl.sampleRate / 100);
            for (std::size_t i = 0; i + window < owl.samples.size(); i += window) {
                int peak = 0;
                for (std::size_t k = 0; k < window; ++k) peak = std::max(peak, std::abs(static_cast<int>(owl.samples[i + k])));
                const bool loud = peak > 4000;
                if (loud && !inToot) ++toots;
                inToot = loud;
            }
            Check(owl.sampleRate > 0 && toots == 3, "the scops owl calls three soft toots");
        }
        const auto tyres = island::GenerateTyreRollLoop();
        std::int64_t tyreEnergy = 0;
        for (const auto sample : tyres.samples) tyreEnergy += std::abs(static_cast<int>(sample));
        Check(tyres.sampleRate > 0 && tyres.samples.size() == static_cast<std::size_t>(tyres.sampleRate) * 2U
                  && tyreEnergy / static_cast<std::int64_t>(std::max<std::size_t>(tyres.samples.size(), 1U)) > 400,
              "the tyres' roll is an audible two-second loop");

        // The engine's two loops: whole firing cycles of a single near two seconds, audible,
        // the high one firing faster (more barks a second).
        const auto low = island::GenerateEngineLoop(0), high = island::GenerateEngineLoop(1);
        const auto barks = [](const island::AmbientLoop& loop) {
            // firings counted as rises of the smoothed loudness through its mean
            const std::size_t window = static_cast<std::size_t>(loop.sampleRate / 400);
            std::vector<float> envelope;
            float sum = 0.0f;
            for (std::size_t i = 0; i + window < loop.samples.size(); i += window) {
                float e = 0.0f;
                for (std::size_t k = 0; k < window; ++k) e += std::abs(static_cast<float>(loop.samples[i + k]));
                envelope.push_back(e / static_cast<float>(window));
                sum += envelope.back();
            }
            const float mean = sum / static_cast<float>(std::max<std::size_t>(envelope.size(), 1U));
            int rises = 0;
            for (std::size_t i = 1; i < envelope.size(); ++i) rises += envelope[i - 1] < mean && envelope[i] >= mean ? 1 : 0;
            return static_cast<float>(rises) * static_cast<float>(loop.sampleRate) / static_cast<float>(loop.samples.size());
        };
        const float lowSeconds = static_cast<float>(low.samples.size()) / static_cast<float>(std::max(low.sampleRate, 1));
        const float lowBarks = barks(low), highBarks = barks(high);
        Check(low.sampleRate > 0 && lowSeconds > 1.8f && lowSeconds < 2.2f && highBarks > lowBarks * 1.5f
                  && lowBarks > island::EngineLowRpm / 120.0f * 0.6f && lowBarks < island::EngineLowRpm / 120.0f * 1.6f,
              "the engine loops fire at their revs, the high one faster");

        // The exhaust: puffs while the engine runs, more with the throttle, carried along and
        // downwind, and none left a few seconds after it stops.
        island::ExhaustSmokeField idle, open;
        const island::BikeVec tip{0.0f, 0.5f, 0.0f}, back{0.0f, 0.0f, -1.0f}, still{0.0f, 0.0f, 0.0f};
        for (int i = 0; i < 120; ++i) {
            idle.Update(1.0f / 60.0f, true, 0.0f, tip, back, still, 1.0f, 0.0f);
            open.Update(1.0f / 60.0f, true, 1.0f, tip, back, still, 1.0f, 0.0f);
        }
        float drift = 0.0f;
        int live = 0;
        for (const auto& puff : open.Puffs()) {
            if (!puff.active) continue;
            ++live;
            drift += puff.x;
        }
        Check(idle.Emitted() > 0U && open.Emitted() > idle.Emitted() && live > 0 && drift / static_cast<float>(live) > 0.05f,
              "the exhaust smokes at idle, thicker with the throttle, and drifts downwind");
        for (int i = 0; i < 60 * 4; ++i) open.Update(1.0f / 60.0f, false, 0.0f, tip, back, still, 1.0f, 0.0f);
        bool cleared = true;
        for (const auto& puff : open.Puffs()) cleared = cleared && !puff.active;
        Check(cleared, "the exhaust clears within its life once the engine stops");

        // The wheels' traces: ruts on sand every third of a metre, none on rock; the wet sand's
        // go sooner; dust off dry ground at speed, none at a walk; spray only in water.
        std::array<island::WheelOnGround, 4> wheels{};
        for (auto& wheel : wheels) { wheel.down = true; wheel.soft = true; wheel.dusty = true; wheel.load = 0.4f; }
        island::TyreTrackField ruts;
        for (int i = 0; i <= 100; ++i) {
            for (std::size_t w = 0; w < wheels.size(); ++w) {
                wheels[w].x = (w % 2 == 0 ? -1.0f : 1.0f) * (w < 2 ? 0.59f : 0.80f);
                wheels[w].z = static_cast<float>(i) * 0.1f + (w < 2 ? 0.625f : -0.625f);
                wheels[w].soft = w != 3;   // the rear right on rock
                wheels[w].wet = w == 1;
            }
            ruts.Update(1.0f / 60.0f, wheels);
        }
        std::size_t perWheel[4] = {0, 0, 0, 0};
        float wetLife = 0.0f;
        for (const auto& track : ruts.Tracks()) {
            if (!track.active) continue;
            const std::size_t w = (track.x0 < 0.0f ? 0U : 1U) + (std::abs(track.x0) > 0.7f ? 2U : 0U);
            ++perWheel[w];
            if (w == 1U) wetLife = track.life;
        }
        Check(perWheel[0] >= 20 && perWheel[0] <= 32 && perWheel[3] == 0 && wetLife == island::TyreTrackField::WetLifeSeconds,
              "tyres leave a rut piece every third of a metre on sand, none on rock, short-lived on wet sand");
        {
            // a tyre wet from a ford prints even rock, fainter and briefly; a dry one does not
            std::array<island::WheelOnGround, 4> onRock = wheels;
            island::TyreTrackField damp;
            for (int i = 0; i <= 100; ++i) {
                for (std::size_t w = 0; w < onRock.size(); ++w) {
                    onRock[w].z = static_cast<float>(i) * 0.1f + (w < 2 ? 0.625f : -0.625f);
                    onRock[w].soft = false;
                    onRock[w].wet = false;
                    onRock[w].wetTread = w == 0 ? 1.0f : 0.0f;
                }
                damp.Update(1.0f / 60.0f, onRock);
            }
            std::size_t dampPrints[4] = {0, 0, 0, 0};
            bool briefAndFaint = true;
            for (const auto& track : damp.Tracks()) {
                if (!track.active) continue;
                ++dampPrints[(track.x0 < 0.0f ? 0U : 1U) + (std::abs(track.x0) > 0.7f ? 2U : 0U)];
                briefAndFaint = briefAndFaint && track.life == island::TyreTrackField::DampPrintLifeSeconds && track.strength < 0.7f;
            }
            Check(dampPrints[0] >= 20 && dampPrints[1] + dampPrints[2] + dampPrints[3] == 0 && briefAndFaint,
                  "a tyre wet from the water prints even rock, faintly and briefly; dry ones do not");
        }
        for (auto& wheel : wheels) wheel.soft = true;
        island::WheelThrowField dust(island::WheelThrowField::Kind::Dust), walk(island::WheelThrowField::Kind::Dust),
            spray(island::WheelThrowField::Kind::Spray), dry(island::WheelThrowField::Kind::Spray);
        for (int i = 0; i < 60; ++i) {
            dust.Update(1.0f / 60.0f, wheels, {0.0f, 0.0f, 12.0f}, 1.0f, 0.0f);
            walk.Update(1.0f / 60.0f, wheels, {0.0f, 0.0f, 1.0f}, 1.0f, 0.0f);
            dry.Update(1.0f / 60.0f, wheels, {0.0f, 0.0f, 8.0f}, 1.0f, 0.0f);
        }
        for (auto& wheel : wheels) wheel.waterDepth = 0.2f;
        for (int i = 0; i < 60; ++i) spray.Update(1.0f / 60.0f, wheels, {0.0f, 0.0f, 8.0f}, 1.0f, 0.0f);
        Check(dust.Emitted() > 10U && walk.Emitted() == 0U && spray.Emitted() > 10U && dry.Emitted() == 0U,
              "the wheels throw dust off dry ground at speed (not at a walk) and spray only in water");
    }
    const island::GrassLodSplit emptyGrassLod = island::GrassLod::Split(0, 82.0f);
    Check(emptyGrassLod.nearCount == 0 && emptyGrassLod.middleCount == 0
              && emptyGrassLod.farCount == 0,
          "grass LOD ignores an empty population");
    const island::GrassLodSplit invalidGrassLod = island::GrassLod::Split(
        3, std::numeric_limits<float>::quiet_NaN());
    const island::GrassLodSplit infiniteGrassLod = island::GrassLod::Split(
        3, std::numeric_limits<float>::infinity());
    Check(invalidGrassLod.nearCount == 3 && invalidGrassLod.middleCount == 0
              && invalidGrassLod.farCount == 0 && infiniteGrassLod.nearCount == 0
              && infiniteGrassLod.middleCount == 0 && infiniteGrassLod.farCount == 3,
          "grass LOD handles invalid and unbounded distances conservatively");
    const island::GrassLodSplit nearGrassLod = island::GrassLod::Split(
        101, island::GrassLod::NearOnlyDistance);
    const island::GrassLodSplit nearMiddleGrassLod = island::GrassLod::Split(101, 70.0f);
    const island::GrassLodSplit middleGrassLod = island::GrassLod::Split(101, 82.0f);
    const island::GrassLodSplit middleFarGrassLod = island::GrassLod::Split(101, 94.0f);
    const island::GrassLodSplit farGrassLod = island::GrassLod::Split(
        101, island::GrassLod::FarOnlyDistance);
    Check(nearGrassLod.nearCount == 101 && nearGrassLod.middleCount == 0
              && nearGrassLod.farCount == 0 && nearMiddleGrassLod.nearCount == 50
              && nearMiddleGrassLod.middleCount == 51 && nearMiddleGrassLod.farCount == 0
              && middleGrassLod.nearCount == 0 && middleGrassLod.middleCount == 101
              && middleGrassLod.farCount == 0 && middleFarGrassLod.nearCount == 0
              && middleFarGrassLod.middleCount == 50 && middleFarGrassLod.farCount == 51
              && farGrassLod.nearCount == 0 && farGrassLod.middleCount == 0
              && farGrassLod.farCount == 101,
          "grass LOD covers near, middle, far, and adjacent mixed populations");
    int previousNearGrassCount = 101;
    int previousFarGrassCount = 0;
    for (int step = 0; step <= 160; ++step) {
        const float distance = island::GrassLod::NearOnlyDistance
            + static_cast<float>(step) * 0.25f;
        const island::GrassLodSplit split = island::GrassLod::Split(101, distance);
        Check(split.nearCount >= 0 && split.middleCount >= 0 && split.farCount >= 0
                  && split.nearCount + split.middleCount + split.farCount == 101,
              "grass LOD conserves every patch through both transitions");
        Check(split.nearCount <= previousNearGrassCount,
              "grass near-LOD population decreases monotonically with distance");
        Check(split.farCount >= previousFarGrassCount,
              "grass far-LOD population increases monotonically with distance");
        Check(!(split.nearCount > 0 && split.farCount > 0),
              "grass never mixes non-adjacent near and far meshes");
        previousNearGrassCount = split.nearCount;
        previousFarGrassCount = split.farCount;
    }
    Check(grass.Patches().size() >= 10000U && grass.Patches().size() <= 40000U,
          "grass patch population stays inside the first geometry budget");
    Check(grass.Patches().size() == repeatedGrass.Patches().size(),
          "grass population is deterministic");
    {
        // Redistribution, not inflation: rich sunlit ground carries several times the tufts
        // per candidate of patch-poor or shaded ground.
        double richSites = 0.0, poorSites = 0.0;
        int richPatches = 0, poorPatches = 0;
        for (float z = -240.0f; z <= 240.0f; z += 2.0f) {
            for (float x = -240.0f; x <= 240.0f; x += 2.0f) {
                const auto sample = terrain.Sample(x, z);
                if (sample.signedShoreDistance < 40.0f || sample.slopeRadians > 0.3f) continue;
                const auto environment=island::IslandEnvironmentField::SampleInland(
                    x,z,sample.signedShoreDistance);
                const float cover = island::GroundCover::Density(canopyLight.Sample(x, z),
                    groundCover.SamplePatch(x, z), sample.signedShoreDistance, sample.slopeRadians,
                    sample.coastalRockiness,sample.beachSuitability,
                    environment.moisture,environment.drainage);
                if (cover > 0.7f) richSites += 4.0;
                else if (cover < 0.2f) poorSites += 4.0;
            }
        }
        for (const auto& patch : grass.Patches()) {
            const auto sample = terrain.Sample(patch.x, patch.z);
            Check(sample.beachSuitability < island::GroundCover::BeachSuppressionEnd,
                  "grass parents and companions avoid the sandy apron even on reserved routes");
            const auto environment=island::IslandEnvironmentField::SampleInland(
                patch.x,patch.z,sample.signedShoreDistance);
            Check(environment.drainage<0.82f,"grass roots stay outside flowing stream water");
            if (sample.signedShoreDistance < 40.0f || sample.slopeRadians > 0.3f) continue;
            const float cover = island::GroundCover::Density(canopyLight.Sample(patch.x, patch.z),
                groundCover.SamplePatch(patch.x, patch.z), sample.signedShoreDistance, sample.slopeRadians,
                sample.coastalRockiness,sample.beachSuitability,
                environment.moisture,environment.drainage);
            if (cover > 0.7f) ++richPatches;
            else if (cover < 0.2f) ++poorPatches;
        }
        Check(richSites > 1000.0 && poorSites > 1000.0
                  && richPatches / richSites > 4.0 * (poorPatches / poorSites),
              "grass forms dense swards on rich ground and isolated tufts on poor ground");
    }
    double eastClearingTintSum = 0.0;
    double westClearingTintSum = 0.0;
    std::size_t eastClearingPatchCount = 0U;
    std::size_t westClearingPatchCount = 0U;
    for (std::size_t index = 0; index < grass.Patches().size(); ++index) {
        const island::GrassPlacement& patch = grass.Patches()[index];
        const island::GrassPlacement& repeated = repeatedGrass.Patches()[index];
        const island::TerrainSample sample = terrain.Sample(patch.x, patch.z);
        Check(patch.x == repeated.x && patch.y == repeated.y && patch.z == repeated.z
                  && patch.yawRadians == repeated.yawRadians && patch.scale == repeated.scale
                  && patch.spreadScale == repeated.spreadScale
                  && patch.windPhase == repeated.windPhase
                  && patch.colorVariation == repeated.colorVariation
                  && patch.biomeTint == repeated.biomeTint
                  && patch.pathScale == repeated.pathScale,
              "grass placement data is bitwise deterministic for one seed");
        Check(sample.signedShoreDistance >= 20.0f && sample.height >= 2.1f,
              "grass remains above the sand transition");
        Check(sample.slopeRadians <= 0.4886922f,
              "grass rejects terrain steeper than its placement limit");
        Check(patch.windPhase >= 0.0f && patch.windPhase < 1.0f
                  && patch.colorVariation >= 0.0f && patch.colorVariation < 1.0f
                  && patch.biomeTint >= 0.0f && patch.biomeTint <= 1.0f,
              "grass shader variation values stay normalized");
        Check(patch.spreadScale >= island::GrassLayout::MinimumSpreadScale
                  && patch.spreadScale <= island::GrassLayout::MaximumSpreadScale,
              "grass horizontal spread remains in the authored range");
        if (patch.pathScale) {
            Check(island::VegetationLayout::IsReservedRoute(patch.x, patch.z),
                  "short grass is confined to authored route regions");
            Check(patch.scale < island::GrassLayout::MinimumScale,
                  "authored route grass remains shorter than ordinary grass");
        } else {
            Check(patch.scale >= island::GrassLayout::MinimumScale
                      && patch.scale <= island::GrassLayout::MaximumScale,
                  "ordinary grass scale remains in the authored range");
        }

        const auto accumulateClearingTint = [&patch](
            const island::ClearingDefinition& clearing, double& sum,
            std::size_t& count) {
            const float offsetX = patch.x - clearing.x;
            const float offsetZ = patch.z - clearing.z;
            if (offsetX * offsetX + offsetZ * offsetZ
                <= clearing.radius * clearing.radius) {
                sum += static_cast<double>(patch.biomeTint);
                ++count;
            }
        };
        accumulateClearingTint(island::VegetationLayout::EastClearing,
                               eastClearingTintSum, eastClearingPatchCount);
        accumulateClearingTint(island::VegetationLayout::WestClearing,
                               westClearingTintSum, westClearingPatchCount);
    }
    Check(eastClearingPatchCount >= 100U && westClearingPatchCount >= 100U,
          "both authored clearings retain enough grass to express their material identity");
    const double eastClearingAverageTint = eastClearingTintSum
        / static_cast<double>(eastClearingPatchCount);
    const double westClearingAverageTint = westClearingTintSum
        / static_cast<double>(westClearingPatchCount);
    Check(eastClearingAverageTint >= 0.70 && westClearingAverageTint <= 0.30
              && eastClearingAverageTint - westClearingAverageTint >= 0.45,
          "east and west clearing grass keep distinct warm and cool biome identities");

    const island::JungleLayout repeatedJungle(terrain);
    Check(jungle.ForestColonies() == repeatedJungle.ForestColonies()
        && jungle.ForestGroundPlantCount() == repeatedJungle.ForestGroundPlantCount()
        && jungle.StreamBankGroundPlantCount() == repeatedJungle.StreamBankGroundPlantCount()
        && jungle.SeasonalWetlandGroundPlantCount()
            == repeatedJungle.SeasonalWetlandGroundPlantCount(),
        "forest colony membership and shape are deterministic for one seed");
    Check(jungle.Bushes().size() - jungle.ClearingBushCount() == 557
        && jungle.GroundPlants().size() - jungle.ClearingGroundPlantCount()
            - jungle.ForestGroundPlantCount() - jungle.StreamBankGroundPlantCount()
            - jungle.SeasonalWetlandGroundPlantCount() == 2134,
        "wet-biome, cave, bunker and helicopter reservations preserve measured deterministic populations");
    Check(jungle.StreamBankGroundPlantCount() >= 20
        && jungle.StreamBankGroundPlantCount() <= static_cast<std::size_t>(
            island::JungleLayout::StreamBankColonyCount
                * island::JungleLayout::StreamBankPlantsPerColony),
        "stream banks retain a bounded mixed fern and broadleaf population");
    const auto checkColonies = [&](const island::JungleLayout& layout) {
        Check(layout.ClearingBushCount() > 0 && layout.ClearingBushCount() <= 24
            && layout.ClearingGroundPlantCount() > 0 && layout.ClearingGroundPlantCount() <= 144,
            "clearing accents stay within the authored local population budget");
        const auto checkLayer = [&](const auto& plants, std::size_t count, float radius, float separation) {
            std::array<int, 2> clearingCounts{};
            for (std::size_t i = plants.size() - count; i < plants.size(); ++i) {
                const auto& plant = plants[i];
                const float footprint = radius * plant.scale;
                Check(!island::VegetationLayout::OverlapsReservedRoute(plant.x, plant.z, footprint),
                    "full animated clearing-plant footprint avoids reserved walking space");
                for (int step = 0; step < 64; ++step) {
                    const float a = step * 2 * std::numbers::pi_v<float> / 64;
                    Check(!island::VegetationLayout::IsReservedRoute(plant.x + footprint * std::cos(a),
                        plant.z + footprint * std::sin(a)), "independent footprint samples remain outside routes");
                }
                for (std::size_t j = 0; j < i; ++j)
                    Check(std::hypot(plant.x - plants[j].x, plant.z - plants[j].z) >= separation,
                        "colony roots avoid duplicate roots within their layer");
                bool inBand = false;
                int index = 0;
                for (auto clearing : {island::VegetationLayout::EastClearing, island::VegetationLayout::WestClearing}) {
                    const float distance = std::hypot(plant.x - clearing.x, plant.z - clearing.z) - clearing.radius;
                    if (distance >= footprint && distance <= 15.4f) { inBand = true; ++clearingCounts[index]; }
                    ++index;
                }
                Check(inBand, "colony changes remain confined to clearing-edge bands");
                for (const auto& tree : layout.Obstacles())
                    Check(std::hypot(plant.x - tree.x, plant.z - tree.z) >= footprint + tree.radius,
                        "clearing plant footprints avoid canopy trunks");
            }
            Check(clearingCounts[0] > 0 && clearingCounts[1] > 0,
                "both clearings receive accents without filling their centres");
        };
        checkLayer(layout.Bushes(), layout.ClearingBushCount(), island::JungleLayout::BushFootprintRadius, 2.8f);
        checkLayer(layout.GroundPlants(), layout.ClearingGroundPlantCount(), island::JungleLayout::GroundPlantFootprintRadius, 0.72f);
    };
    const auto checkForestColonies = [&](const island::JungleLayout& layout) {
        Check(!layout.ForestColonies().empty() && layout.ForestColonies().size()<layout.Canopy().size(),
            "forest-floor recruitment leaves some canopy trees without a colony");
        Check(layout.ForestGroundPlantCount()>200
            && layout.ForestGroundPlantCount()<=layout.Canopy().size()*island::JungleLayout::ForestPlantsPerColony,
            "forest companions have useful density within their per-tree candidate budget");
        Check(layout.GroundPlants().size()<=5000,"forest companions preserve the existing global floor budget");
        std::size_t next=layout.GroundPlants().size()-layout.ClearingGroundPlantCount()-layout.ForestGroundPlantCount();
        std::size_t previousTree=0;
        std::array<int,3> species{};
        std::size_t groups=0;
        for (const auto& colony:layout.ForestColonies()) {
            Check(colony.firstPlant==next && colony.plantCount>0
                && colony.plantCount<=island::JungleLayout::ForestPlantsPerColony,
                "forest colony membership is contiguous, nonempty and bounded");
            Check(colony.canopyIndex<layout.Canopy().size()
                && (groups==0 || colony.canopyIndex>previousTree),
                "forest colonies have one unique ordered canopy parent each");
            previousTree=colony.canopyIndex;
            const auto& tree=layout.Canopy()[colony.canopyIndex];
            const float parentDistance=std::hypot(colony.x-tree.x,colony.z-tree.z)/tree.scale;
            Check(parentDistance>=3.1999f && parentDistance<=7.2001f
                && colony.majorRadius>=2.5f && colony.majorRadius<=3.8f
                && colony.minorRadius>=1.1f && colony.minorRadius<=2.4f,
                "forest colonies remain associated with their parent's nearby floor");
            for (std::size_t index=colony.firstPlant;index<colony.firstPlant+colony.plantCount;++index) {
                const auto& plant=layout.GroundPlants()[index];
                const float dx=plant.x-colony.x,dz=plant.z-colony.z;
                const float u=(std::cos(colony.angle)*dx+std::sin(colony.angle)*dz)/colony.majorRadius;
                const float v=(-std::sin(colony.angle)*dx+std::cos(colony.angle)*dz)/colony.minorRadius;
                Check(u*u+v*v<=1.0001f,"forest recruitment stays in its authored irregular ellipse");
                const float footprint=island::JungleLayout::GroundPlantFootprintRadius*plant.scale;
                const auto sample=terrain.Sample(plant.x,plant.z);
                Check(sample.signedShoreDistance>=43 && sample.signedShoreDistance<=238
                    && sample.slopeRadians<=0.41887902f && sample.height>=2.5f
                    && plant.scale>=island::JungleLayout::GroundPlantMinimumScale && plant.scale<=0.94f,
                    "forest companions have suitable inland ground and restrained stature");
                Check(!island::VegetationLayout::OverlapsReservedRoute(plant.x,plant.z,footprint),
                    "forest companion full footprint leaves reserved routes open");
                for (int step=0;step<64;++step) {
                    const float angle=step*2*std::numbers::pi_v<float>/64;
                    Check(!island::VegetationLayout::IsReservedRoute(plant.x+footprint*std::cos(angle),
                        plant.z+footprint*std::sin(angle)),"independent forest footprint samples avoid paths");
                }
                for (const auto& clearing:{island::VegetationLayout::EastClearing,island::VegetationLayout::WestClearing})
                    Check(std::hypot(plant.x-clearing.x,plant.z-clearing.z)>=clearing.radius+16+footprint,
                        "forest companions do not overwrite clearing-edge composition");
                for (std::size_t other=0;other<index;++other)
                    Check(std::hypot(plant.x-layout.GroundPlants()[other].x,plant.z-layout.GroundPlants()[other].z)
                        >=island::JungleLayout::ForestRootSeparation,"forest companion roots have minimum separation");
                for (const auto& trunk:layout.Obstacles())
                    Check(std::hypot(plant.x-trunk.x,plant.z-trunk.z)>=footprint+trunk.radius,
                        "forest companions avoid solid trunks with their full footprints");
                for (const auto& bush:layout.Bushes())
                    Check(std::hypot(plant.x-bush.x,plant.z-bush.z)>=0.9f,"forest companions avoid shrub roots");
                for (const auto& plantAbove:layout.Understory())
                    Check(std::hypot(plant.x-plantAbove.x,plant.z-plantAbove.z)>=0.85f,
                        "forest companions avoid medium-plant roots");
                ++species[plant.variant];
            }
            next+=colony.plantCount;
            ++groups;
        }
        Check(next==layout.GroundPlants().size()-layout.ClearingGroundPlantCount(),
            "forest colony metadata covers every added plant exactly once");
        Check(species[0]>species[1] && species[1]>0 && species[2]>0,
            "forest groups retain all species with low rosettes predominant");
    };
    checkColonies(jungle);
    checkForestColonies(jungle);
    Check(jungle.ForestGroundPlantCount() >= 780U
        && jungle.ForestGroundPlantCount() <= 900U,
        "default forest-floor recruitment has the reviewed local density budget");
    Check(jungle.ClearingBushCount()==12 && jungle.ClearingGroundPlantCount()==60,
        "forest recruitment preserves the existing default clearing accents");
    for (const auto seed:{0x12345U,0xa55aU}) {
        const island::JungleLayout alternate(terrain,seed);
        checkColonies(alternate);
        checkForestColonies(alternate);
    }
    for (float z = -230; z < 196; z += 0.75f) {
        for (float offset : {-9.0f, -7.5f, 7.5f, 9.0f}) {
            const float x = island::VegetationLayout::PrimaryPathCenterX(z) + offset;
            constexpr float radius = 2;
            if (island::VegetationLayout::OverlapsReservedRoute(x, z, radius)) continue;
            for (int i = 0; i < 64; ++i) {
                const float a = i * 2 * std::numbers::pi_v<float> / 64;
                Check(!island::VegetationLayout::IsReservedRoute(x + radius * std::cos(a), z + radius * std::sin(a)),
                    "conservative route envelope covers bends and path ends");
            }
        }
    }
    Check(island::VegetationLayout::OverlapsReservedRoute(0, 0, -1)
        && island::VegetationLayout::OverlapsReservedRoute(std::numeric_limits<float>::quiet_NaN(), 0, 1),
        "invalid placement footprints fail closed");
    Check(jungle.Canopy().size() >= 100U && jungle.Canopy().size() <= 600U,
          "broadleaf canopy population stays inside its first performance budget");
    Check(jungle.Understory().size() >= 800U && jungle.Understory().size() <= 4000U,
          "understory population stays inside its first performance budget");
    Check(jungle.Bushes().size() >= 150U && jungle.Bushes().size() <= 1400U,
          "low-bush population stays inside its first performance budget");
    Check(jungle.GroundPlants().size() >= 600U && jungle.GroundPlants().size() <= 5000U,
          "forest-floor plant population stays inside its first performance budget");
    Check(jungle.Canopy().size() == repeatedJungle.Canopy().size()
              && jungle.Understory().size() == repeatedJungle.Understory().size()
              && jungle.Bushes().size() == repeatedJungle.Bushes().size()
              && jungle.GroundPlants().size() == repeatedJungle.GroundPlants().size(),
          "jungle populations are deterministic");
    Check(jungle.Obstacles().size()
              == jungle.Canopy().size() + jungle.StandingSnags().size(),
          "every broadleaf canopy and standing snag has one collision proxy");
    std::array<std::size_t, island::JungleLayout::CanopyVariantCount> canopyFormCounts{};
    std::size_t emergentCount = 0U;
    std::size_t juvenileCount = 0U;
    std::size_t mangroveCount = 0U;
    for (std::size_t index = 0; index < jungle.Canopy().size(); ++index) {
        const island::CanopyPlacement& tree = jungle.Canopy()[index];
        const island::CanopyPlacement& repeated = repeatedJungle.Canopy()[index];
        const island::TerrainSample sample = terrain.Sample(tree.x, tree.z);
        const bool mangrove = tree.variant == island::MangroveCanopyVariant;
        Check(tree.x == repeated.x && tree.y == repeated.y && tree.z == repeated.z
                  && tree.yawRadians == repeated.yawRadians && tree.scale == repeated.scale
                  && tree.variant == repeated.variant,
              "broadleaf canopy placement is bitwise deterministic for one seed");
        Check(sample.signedShoreDistance >= (mangrove ? .4f : 43.0f)
                  && sample.signedShoreDistance <= (mangrove ? 24.0f : 238.0f),
              "canopy trees remain in their authored coastal or interior belt");
        Check(sample.height >= (mangrove ? -.05f : 3.8f)
                  && sample.height <= (mangrove ? 3.4f : 100.0f)
                  && sample.slopeRadians <= (mangrove ? .38f : .4712389f),
              "canopy trees remain on suitable ground for their habitat");
        Check(!island::VegetationLayout::IsReservedRoute(tree.x, tree.z),
              "broadleaf trees keep paths, clearings, and player spawn open");
        const auto environment = island::IslandEnvironmentField::SampleInland(
            tree.x, tree.z, sample.signedShoreDistance);
        Check(environment.drainage <= (mangrove ? .76f : .68f),
              "canopy trunks stay outside the active stream channel");
        if (mangrove) {
            Check(environment.moisture >= .30f && environment.saltExposure >= .06f
                      && sample.coastalRockiness <= .38f,
                  "mangroves remain in the localized damp brackish mouth habitat");
            for (const island::CircularObstacle& palm : vegetation.Obstacles()) {
                Check(std::hypot(tree.x - palm.x, tree.z - palm.z)
                          >= jungle.Obstacles()[index].radius + palm.radius,
                      "mangrove stilt roots do not intersect a palm collision footprint");
            }
            ++mangroveCount;
        }
        Check(tree.variant < island::JungleLayout::CanopyVariantCount,
              "broadleaf canopy variant index remains in range");
        ++canopyFormCounts.at(tree.variant);
        const bool emergent = !mangrove
            && tree.scale > island::JungleLayout::CanopyMaximumScale;
        const bool juvenile = !mangrove
            && tree.scale < island::JungleLayout::CanopyMinimumScale;
        Check(mangrove
                  ? tree.scale >= island::JungleLayout::MangroveMinimumScale
                      && tree.scale <= island::JungleLayout::MangroveMaximumScale
                  : juvenile
                      ? tree.scale >= island::JungleLayout::CanopyJuvenileMinimumScale
                          && tree.scale <= island::JungleLayout::CanopyJuvenileMaximumScale
                      : tree.scale >= island::JungleLayout::CanopyMinimumScale
                          && (tree.scale <= island::JungleLayout::CanopyMaximumScale
                              || (tree.scale >= island::JungleLayout::CanopyEmergentMinimumScale
                                  && tree.scale <= island::JungleLayout::CanopyEmergentScale)),
              "canopy scale remains in its authored juvenile, adult or emergent range");
        emergentCount += emergent ? 1U : 0U;
        juvenileCount += juvenile ? 1U : 0U;
        if (juvenile) {
            const float nominalHeight = island::CanopyNominalHeight(tree.variant) * tree.scale;
            Check(nominalHeight >= 2.0f && nominalHeight <= 5.0f,
                  "juvenile full crown envelope remains inside the requested 2-5m stature band");
        }
        if (emergent) {
            int standNeighbours = 0;
            for (const island::CanopyPlacement& other : jungle.Canopy()) {
                if (&other != &tree && std::hypot(other.x - tree.x, other.z - tree.z) < 20.0f) ++standNeighbours;
            }
            Check(standNeighbours >= 2, "emergent trees rise only inside closed stands");
        }
        const island::CircularObstacle& obstacle = jungle.Obstacles()[index];
        CheckNear(obstacle.x, tree.x, 0.0001f,
                  "broadleaf obstacle shares its visual x coordinate");
        CheckNear(obstacle.z, tree.z, 0.0001f,
                  "broadleaf obstacle shares its visual z coordinate");
        if (mangrove) {
            CheckNear(obstacle.radius, 1.08f * tree.scale, .0001f,
                "mangrove collision covers its visible stilt-root spread");
        } else {
            Check(obstacle.radius >= (juvenile ? .12f : .30f)
                      && obstacle.radius < 0.43f * island::JungleLayout::CanopyEmergentScale + 0.01f
                      && (emergent || obstacle.radius < 0.56f),
                  "broadleaf collision radius stays close to the visible trunk");
        }
    }
    Check(mangroveCount == jungle.MangroveCount() && mangroveCount >= 15
              && mangroveCount <= 30,
          "one localized mangrove colony stays inside its requested population budget");
    Check(jungle.StandingSnags().size()
              == static_cast<std::size_t>(island::JungleLayout::StandingSnagTargetCount)
              && jungle.StandingSnags().size() == repeatedJungle.StandingSnags().size(),
          "standing dead and broken trunks meet their bounded deterministic population");
    for (std::size_t index = 0; index < jungle.StandingSnags().size(); ++index) {
        const auto& snag = jungle.StandingSnags()[index];
        const auto& repeated = repeatedJungle.StandingSnags()[index];
        const auto sample = terrain.Sample(snag.x, snag.z);
        const auto environment = island::IslandEnvironmentField::SampleInland(
            snag.x, snag.z, sample.signedShoreDistance);
        const float rootRadius = .76f * snag.scale;
        Check(snag.x == repeated.x && snag.y == repeated.y && snag.z == repeated.z
                  && snag.yawRadians == repeated.yawRadians && snag.scale == repeated.scale
                  && snag.variant == repeated.variant,
              "standing snag placement is bitwise deterministic for one seed");
        Check(snag.variant < island::JungleLayout::StandingSnagVariantCount
                  && snag.scale >= island::JungleLayout::StandingSnagMinimumScale
                  && snag.scale <= island::JungleLayout::StandingSnagMaximumScale,
              "standing snag silhouette and scale remain inside the authored range");
        Check(sample.signedShoreDistance >= 58.0f && sample.signedShoreDistance <= 232.0f
                  && sample.height >= 4.0f && sample.slopeRadians <= .42f
                  && sample.coastalRockiness <= .46f && environment.drainage <= .67f,
              "standing snags remain on stable dry inland forest ground");
        Check(!island::VegetationLayout::OverlapsReservedRoute(
                  snag.x, snag.z, rootRadius),
              "standing snag collision footprint leaves routes and clearings open");
        int matureNeighbours = 0;
        for (const auto& tree : jungle.Canopy()) {
            const float distance = std::hypot(snag.x - tree.x, snag.z - tree.z);
            if (tree.variant != island::MangroveCanopyVariant
                && tree.scale >= island::JungleLayout::CanopyMinimumScale
                && distance >= 3.0f && distance <= 25.0f) ++matureNeighbours;
        }
        Check(matureNeighbours >= 2,
              "standing snags occur inside established multi-tree stands");
        for (std::size_t treeIndex = 0; treeIndex < jungle.Canopy().size(); ++treeIndex) {
            const auto& treeObstacle = jungle.Obstacles()[treeIndex];
            Check(std::hypot(snag.x - treeObstacle.x, snag.z - treeObstacle.z)
                      >= rootRadius + treeObstacle.radius + .85f,
                  "standing snag roots do not intersect living canopy trunks");
        }
        const auto& obstacle = jungle.Obstacles()[jungle.Canopy().size() + index];
        CheckNear(obstacle.x, snag.x, .0001f,
                  "standing snag obstacle shares its visual x coordinate");
        CheckNear(obstacle.z, snag.z, .0001f,
                  "standing snag obstacle shares its visual z coordinate");
        CheckNear(obstacle.radius, rootRadius, .0001f,
                  "standing snag collision covers its visible root flare");
    }
    {
        // Forest masses, not a park: most trees stand in closed groups, a few stand alone.
        std::size_t grouped = 0U, solitary = 0U;
        for (const island::CanopyPlacement& tree : jungle.Canopy()) {
            int neighbours = 0;
            for (const island::CanopyPlacement& other : jungle.Canopy()) {
                if (&other != &tree && std::hypot(other.x - tree.x, other.z - tree.z) < 20.0f) ++neighbours;
            }
            grouped += neighbours >= 3 ? 1U : 0U;
            solitary += neighbours == 0 ? 1U : 0U;
        }
        const double total = static_cast<double>(jungle.Canopy().size());
        Check(grouped / total >= 0.60 && solitary >= 3U && solitary / total <= 0.15,
              "canopy trees form closed stands with a sparse scatter of solitary trees");
        Check(emergentCount / total >= 0.03 && emergentCount / total <= 0.15,
              "a small fraction of canopy trees is emergent");
        Check(juvenileCount / total >= 0.04 && juvenileCount / total <= 0.15
                  && juvenileCount == jungle.JuvenileCanopyCount()
                  && juvenileCount == repeatedJungle.JuvenileCanopyCount(),
              "a small deterministic fraction of canopy trees is juvenile");
        for (const island::CanopyPlacement& tree : jungle.Canopy()) {
            if (tree.variant == island::MangroveCanopyVariant
                || tree.scale >= island::JungleLayout::CanopyMinimumScale) continue;
            const bool nearAdult = std::ranges::any_of(jungle.Canopy(), [&](const auto& other) {
                return &other != &tree && other.variant != island::MangroveCanopyVariant
                    && other.scale >= island::JungleLayout::CanopyMinimumScale
                    && std::hypot(other.x-tree.x,other.z-tree.z)<18.0f;
            });
            Check(nearAdult, "juvenile canopy trees remain beside established forest");
        }
    }
    Check(std::ranges::all_of(canopyFormCounts, [](std::size_t count) { return count >= 10; }),
          "every shared canopy form is represented across the island");
    for (std::size_t index = 0; index < jungle.Understory().size(); ++index) {
        const island::UnderstoryPlacement& plant = jungle.Understory()[index];
        const island::UnderstoryPlacement& repeated = repeatedJungle.Understory()[index];
        const island::TerrainSample sample = terrain.Sample(plant.x, plant.z);
        Check(plant.x == repeated.x && plant.y == repeated.y && plant.z == repeated.z
                  && plant.yawRadians == repeated.yawRadians && plant.scale == repeated.scale
                  && plant.variant == repeated.variant,
              "understory placement is bitwise deterministic for one seed");
        if (plant.variant == island::JungleLayout::BananaUnderstoryVariant) {
            // Banana stools are the stream-bank exception: lower, moister ground by the water.
            const auto environment = island::IslandEnvironmentField::SampleInland(
                plant.x, plant.z, sample.signedShoreDistance);
            Check(sample.height >= 0.7f && sample.slopeRadians <= 0.5f
                      && environment.moisture >= 0.38f && sample.signedShoreDistance >= 8.0f,
                  "banana stools stand on moist low stream-bank ground");
            continue;
        }
        Check(sample.signedShoreDistance >= 31.0f
                  && sample.signedShoreDistance <= 242.0f,
              "understory remains inside its interior placement belt");
        Check(sample.height >= 2.8f && sample.slopeRadians <= 0.5410521f,
              "understory remains on suitable interior ground");
        Check(!island::VegetationLayout::IsReservedRoute(plant.x, plant.z),
              "understory keeps paths, clearings, and player spawn open");
        Check(island::IslandEnvironmentField::SampleInland(
                  plant.x,plant.z,sample.signedShoreDistance).drainage<=0.82f,
              "understory stays outside flowing stream water");
        Check(plant.variant < island::JungleLayout::UnderstoryVariantCount,
              "understory variant index remains in range");
        Check(plant.scale >= island::JungleLayout::UnderstoryMinimumScale
                  && plant.scale <= island::JungleLayout::UnderstoryMaximumScale,
              "understory scale remains in the authored range");
    }
    for (std::size_t index = 0; index < jungle.Bushes().size(); ++index) {
        const island::BushPlacement& bush = jungle.Bushes()[index];
        const island::BushPlacement& repeated = repeatedJungle.Bushes()[index];
        const island::TerrainSample sample = terrain.Sample(bush.x, bush.z);
        Check(bush.x == repeated.x && bush.y == repeated.y && bush.z == repeated.z
                  && bush.yawRadians == repeated.yawRadians && bush.scale == repeated.scale
                  && bush.variant == repeated.variant,
              "low-bush placement is bitwise deterministic for one seed");
        Check(sample.signedShoreDistance >= 38.0f
                  && sample.signedShoreDistance <= 232.0f,
              "low bushes remain inside their interior placement belt");
        Check(sample.height >= 3.0f && sample.slopeRadians <= 0.5235988f,
              "low bushes remain on suitable interior ground");
        Check(!island::VegetationLayout::IsReservedRoute(bush.x, bush.z),
              "low bushes keep paths, clearings, and player spawn open");
        Check(island::IslandEnvironmentField::SampleInland(
                  bush.x,bush.z,sample.signedShoreDistance).drainage<=0.78f,
              "low bushes stay outside flowing stream water");
        Check(bush.variant < island::JungleLayout::BushVariantCount,
              "low-bush variant index remains in range");
        Check(bush.scale >= island::JungleLayout::BushMinimumScale
                  && bush.scale <= island::JungleLayout::BushMaximumScale,
              "low-bush scale remains in the authored range");
    }
    std::size_t wetBankPlantCount = 0;
    std::size_t seasonalWetlandPlantCount = 0;
    std::size_t wetBankNegativeSideCount = 0;
    std::size_t wetBankPositiveSideCount = 0;
    for (std::size_t index = 0; index < jungle.GroundPlants().size(); ++index) {
        const island::GroundPlantPlacement& plant = jungle.GroundPlants()[index];
        const island::GroundPlantPlacement& repeated = repeatedJungle.GroundPlants()[index];
        const island::TerrainSample sample = terrain.Sample(plant.x, plant.z);
        Check(plant.x == repeated.x && plant.y == repeated.y && plant.z == repeated.z
                  && plant.yawRadians == repeated.yawRadians && plant.scale == repeated.scale
                  && plant.variant == repeated.variant && plant.wetBank == repeated.wetBank
                  && plant.seasonalWetland == repeated.seasonalWetland,
              "forest-floor plant placement is bitwise deterministic for one seed");
        Check(sample.signedShoreDistance >= (plant.wetBank ? 8.0f : 27.0f)
                  && sample.signedShoreDistance <= 238.0f,
              "forest-floor plants remain inside their coastal-to-interior belt");
        Check(sample.height >= (plant.wetBank ? .7f : 2.5f)
                  && sample.slopeRadians <= 0.5585054f,
              "forest-floor plants remain on suitable ground");
        Check(!island::VegetationLayout::IsReservedRoute(plant.x, plant.z),
              "forest-floor plants keep paths, clearings, and player spawn open");
        Check(island::IslandEnvironmentField::SampleInland(
                  plant.x,plant.z,sample.signedShoreDistance).drainage<=0.84f,
              "forest-floor plants stay outside flowing stream water");
        if (plant.wetBank) {
            const auto field = island::IslandEnvironmentField::SampleInland(
                plant.x, plant.z, sample.signedShoreDistance);
            Check(field.moisture >= .38f && field.drainage <= .72f,
                "authored stream-bank plants remain damp without entering water");
            if (plant.z < island::StreamRoute::CentreZ(plant.x)) ++wetBankNegativeSideCount;
            else ++wetBankPositiveSideCount;
            ++wetBankPlantCount;
        }
        if (plant.seasonalWetland) {
            const float potential=island::IslandEnvironmentField::SeasonalWetlandPotential(
                plant.x,plant.z,sample.signedShoreDistance);
            const auto field=island::IslandEnvironmentField::SampleInland(
                plant.x,plant.z,sample.signedShoreDistance);
            Check(!plant.wetBank&&potential>=.18f&&field.moisture>=.42f
                    &&field.drainage<=.72f,
                "seasonal-wetland plants remain in the damp non-flowing basin");
            ++seasonalWetlandPlantCount;
        }
        Check(plant.variant < island::JungleLayout::GroundPlantVariantCount,
              "forest-floor plant variant index remains in range");
        Check(plant.scale >= island::JungleLayout::GroundPlantMinimumScale
                  && plant.scale <= island::JungleLayout::GroundPlantMaximumScale,
              "forest-floor plant scale remains in the authored range");
    }
    Check(wetBankPlantCount == jungle.StreamBankGroundPlantCount(),
        "stream-bank plant inventory matches marked ground plants");
    Check(seasonalWetlandPlantCount==jungle.SeasonalWetlandGroundPlantCount()
            &&seasonalWetlandPlantCount>=30
            &&seasonalWetlandPlantCount<=static_cast<std::size_t>(
                island::JungleLayout::SeasonalWetlandPlantTargetCount),
        "seasonal depression retains a bounded fern-rich local population");
    Check(wetBankNegativeSideCount >= 5 && wetBankPositiveSideCount >= 5,
        "stream-bank colonies occupy both sides without a single-sided strip");

    const island::NaturalPropLayout repeatedProps(terrain, vegetation, jungle);
    Check(props.Rocks().size() >= 30U && props.Rocks().size() <= 500U,
          "rock population stays inside its first performance budget");
    Check(props.FallenLogs().size() >= 6U && props.FallenLogs().size() <= 80U,
          "fallen-log population stays inside its first composition budget");
    Check(props.Driftwood().size() >= 12U && props.Driftwood().size() <= 100U,
          "driftwood population stays inside its sparse beach-composition budget");
    Check(props.BeachDetails().size() >= 120U && props.BeachDetails().size() <= 600U,
          "shell/pebble/wrack clusters stay inside their near-field beach-detail budget");
    Check(props.FallenCoconuts().size() >= 20U && props.FallenCoconuts().size() <= 400U
              && props.FallenCoconuts().size() == repeatedProps.FallenCoconuts().size(),
          "fallen coconuts stay inside their bounded deterministic budget");
    for (const auto& coconut : props.FallenCoconuts()) {
        const auto sample = terrain.Sample(coconut.x, coconut.z);
        Check(sample.height >= 0.15f && coconut.y > sample.height && coconut.y < sample.height + 0.12f,
              "fallen coconuts rest on dry supported ground");
    }
    Check(props.AnimalTracks().size() >= 8U && props.AnimalTracks().size() <= 40U
              && props.AnimalTracks().size() == repeatedProps.AnimalTracks().size(),
          "deer and boar track lines stay inside their bounded deterministic budget");
    {
        bool bankTracks = false, shoreTracks = false, deerTracks = false, boarTracks = false;
        for (const auto& track : props.AnimalTracks()) {
            const auto sample = terrain.Sample(track.x, track.z);
            Check(sample.height >= 0.10f && sample.slopeRadians <= 0.40f
                      && std::abs(track.y - sample.height - 0.008f) < 0.001f,
                  "animal track lines rest on supported low-slope ground above the water");
            bankTracks |= track.streamBank;
            shoreTracks |= !track.streamBank;
            deerTracks |= track.variant == 0U;
            boarTracks |= track.variant == 1U;
        }
        Check(bankTracks && shoreTracks && deerTracks && boarTracks,
              "both species leave prints on both stream banks and wet sand");
    }
    Check(props.FallenFronds().size() >= 30U && props.FallenFronds().size() <= 180U,
          "fallen palm fronds stay inside their near-field composition budget");
    {
        // Beach morning glory crowns root on the dry upper beach, lie along the sand, keep clear
        // of the turtles' crawls and nests, and come out the same each time.
        bool onUpperBeach = true, alongSand = true, clearOfTurtles = true;
        for (const auto& vine : props.BeachVines()) {
            const auto sample = terrain.Sample(vine.x, vine.z);
            onUpperBeach = onUpperBeach && sample.surface == island::TerrainSurface::DrySand
                && sample.signedShoreDistance >= 4.5f && sample.signedShoreDistance <= 32.0f
                && vine.scale >= island::NaturalPropLayout::BeachVineMinimumScale
                && vine.scale <= island::NaturalPropLayout::BeachVineMaximumScale;
            alongSand = alongSand && vine.normalY > 0.85f && std::abs(vine.y - sample.height) < 0.35f;
            for (const auto& detail : props.BeachDetails())
                if (detail.turtleTrace && std::hypot(detail.x - vine.x, detail.z - vine.z) < 3.0f) clearOfTurtles = false;
        }
        Check(props.BeachVines().size() >= 80U && props.BeachVines().size() <= 360U && onUpperBeach && alongSand,
              "beach morning glory crowns root on the dry upper beach and lie along the sand");
        Check(clearOfTurtles && props.BeachVines().size() == repeatedProps.BeachVines().size(),
              "beach morning glory keeps clear of the turtles' marks and is deterministic");
    }
    Check(props.ReefHabitat().size() >= 700U && props.ReefHabitat().size() <= 1800U,
          "spatial reef habitat stays inside its bounded instance budget");
    Check(props.Rocks().size() == repeatedProps.Rocks().size()
              && props.FallenLogs().size() == repeatedProps.FallenLogs().size()
              && props.Driftwood().size() == repeatedProps.Driftwood().size()
              && props.BeachDetails().size() == repeatedProps.BeachDetails().size()
              && props.FallenFronds().size() == repeatedProps.FallenFronds().size()
              && props.ReefHabitat().size() == repeatedProps.ReefHabitat().size(),
          "natural-prop populations are deterministic");
    std::array<std::size_t, island::NaturalPropLayout::ReefHabitatVariantCount>
        reefHabitatVariants{};
    for (std::size_t index = 0; index < props.ReefHabitat().size(); ++index) {
        const auto& habitat = props.ReefHabitat()[index];
        const auto& repeated = repeatedProps.ReefHabitat()[index];
        const auto support = terrain.Sample(habitat.x, habitat.z);
        Check(habitat.x == repeated.x && habitat.y == repeated.y
                  && habitat.z == repeated.z && habitat.yawRadians == repeated.yawRadians
                  && habitat.scale == repeated.scale && habitat.variant == repeated.variant,
              "reef habitat placement is bitwise deterministic for one seed");
        CheckNear(habitat.y, support.height + .025f, .0001f,
                  "reef habitat roots remain fitted to the seabed");
        Check(island::IslandEnvironmentField::ReefSuitability(
                  habitat.x, habitat.z, support.signedShoreDistance, support.height) >= .12f,
              "reef habitat remains inside the shared shallow-water suitability field");
        Check(habitat.scale >= island::NaturalPropLayout::ReefHabitatMinimumScale
                  && habitat.scale <= island::NaturalPropLayout::ReefHabitatMaximumScale,
              "reef habitat scale remains inside the portable geometry budget");
        Check(habitat.variant < island::NaturalPropLayout::ReefHabitatVariantCount,
              "reef habitat variant index remains in range");
        ++reefHabitatVariants.at(habitat.variant);
    }
    {
        // The reef's small fish: clouds over the coral heads near the observer, under the water,
        // hiding in the coral from a swimmer at arm's length and out again some seconds after.
        island::ReefFishField reefFish(props.ReefHabitat());
        const auto& coral = props.ReefHabitat()[0];
        for (int step = 0; step < 20; ++step) reefFish.Update(0.1f, coral.x + 10.0f, -1.0f, coral.z);
        bool underWater = true;
        for (const auto& fish : reefFish.Fish()) underWater = underWater && fish.y < -0.1f && fish.scale < 0.25f;
        Check(reefFish.HeadPool() > 100U && reefFish.HeadsPeopled() > 0U && !reefFish.Fish().empty()
                  && reefFish.Fish().size() <= island::ReefFishField::Capacity && underWater && reefFish.HidingCount() == 0U,
              "small reef fish people the coral heads near the observer, under the water");
        const auto& first = reefFish.Fish().front();
        for (int step = 0; step < 30; ++step) reefFish.Update(0.1f, first.x, first.y, first.z);
        const std::size_t hiding = reefFish.HidingCount();
        for (int step = 0; step < 20; ++step) reefFish.Update(0.1f, first.x + 12.0f, first.y, first.z);
        const std::size_t stillHiding = reefFish.HidingCount();
        for (int step = 0; step < 80; ++step) reefFish.Update(0.1f, first.x + 12.0f, first.y, first.z);
        Check(hiding >= 1U && stillHiding >= 1U && reefFish.HidingCount() < hiding,
              "a cloud hides in its coral from a close swimmer and comes out some seconds after");
        // About one coral head in six has an anemone at its foot, and a pair of clownfish in it.
        const auto anemones = reefFish.Anemones();
        Check(anemones.size() * 10U > reefFish.HeadPool() && anemones.size() * 4U < reefFish.HeadPool(),
              "about one coral head in six has a sea anemone beside it");
        const auto& home = anemones.front();
        island::ReefFishField clowns(props.ReefHabitat());
        for (int step = 0; step < 20; ++step) clowns.Update(0.1f, home.x + 6.0f, home.y + 1.0f, home.z);
        std::size_t clownfish = 0;
        bool atHome = true;
        for (const auto& fish : clowns.Fish())
            if (fish.colorVariant == island::ReefFishField::ClownfishVariant) {
                ++clownfish;
                atHome = atHome && std::hypot(fish.x - home.x, fish.z - home.z) < 0.6f && fish.y > home.y && fish.y < -0.1f;
            }
        Check(clownfish >= 2U && clownfish % 2U == 0U && atHome,
              "a pair of clownfish hovers in each anemone near the observer");
    }
    Check(std::all_of(reefHabitatVariants.begin(), reefHabitatVariants.end(),
                      [](const std::size_t count) { return count >= 30U; }),
          "branching, rounded, plate coral and underwater-rock forms all populate the reef");
    std::size_t solidRockCount = 0U;
    std::size_t reefRockCount = 0U;
    std::size_t clearingLandmarkRockCount = 0U;
    std::size_t rockyShoreRockCount = 0U;
    std::size_t tidePoolRockCount = 0U;
    std::size_t streamBankRockCount = 0U;
    std::size_t cliffTalusRockCount = 0U;
    for (std::size_t index = 0; index < props.Rocks().size(); ++index) {
        const island::RockPlacement& rock = props.Rocks()[index];
        const island::RockPlacement& repeated = repeatedProps.Rocks()[index];
        const island::TerrainSample sample = terrain.Sample(rock.x, rock.z);
        Check(rock.x == repeated.x && rock.y == repeated.y && rock.z == repeated.z
                  && rock.yawRadians == repeated.yawRadians
                  && rock.scaleX == repeated.scaleX && rock.scaleY == repeated.scaleY
                  && rock.scaleZ == repeated.scaleZ && rock.variant == repeated.variant
                  && rock.solid == repeated.solid && rock.offshore == repeated.offshore
                  && rock.clearingLandmark == repeated.clearingLandmark
                  && rock.tidePoolLandmark == repeated.tidePoolLandmark
                  && rock.streamBankLandmark == repeated.streamBankLandmark
                  && rock.cliffTalus == repeated.cliffTalus,
              "rock placement is bitwise deterministic for one seed");
        if (rock.offshore) {
            ++reefRockCount;
            Check(sample.signedShoreDistance <= -1.8f
                      && sample.signedShoreDistance >= -9.4f,
                  "reef outcrops remain inside the authored shallow-water belt");
            Check(sample.height <= -0.04f && rock.solid,
                  "reef outcrops originate on submerged support and remain solid");
            Check(rock.y + rock.scaleY * 1.03f >= 0.39f,
                  "reef outcrops remain visibly emergent above the ocean surface");
            Check(rock.scaleX > 0.90f && rock.scaleX < 3.55f
                      && rock.scaleY > 0.70f && rock.scaleY < 3.40f
                      && rock.scaleZ > 0.90f && rock.scaleZ < 3.60f,
                  "reef-outcrop axis scales remain inside their authored bounds");
        } else {
            if (rock.tidePoolLandmark) {
                ++tidePoolRockCount;
                Check(!rock.solid && !rock.clearingLandmark
                        && sample.coastalRockiness>.50f,
                    "tide-pool accent rocks remain non-solid on rocky support");
            } else if (rock.streamBankLandmark) {
                ++streamBankRockCount;
                const float across=std::abs(rock.z-island::StreamRoute::CentreZ(rock.x));
                Check(!rock.solid&&!rock.clearingLandmark
                        && across>island::StreamRoute::HalfWidth(rock.x)+.45f
                        && across<island::StreamRoute::HalfWidth(rock.x)+2.4f,
                    "stream stones remain non-solid and just beyond the carved bank");
                Check(island::IslandEnvironmentField::SampleInland(
                        rock.x,rock.z,sample.signedShoreDistance).moisture>.55f,
                    "stream stones remain inside the damp bank microhabitat");
            } else if (rock.clearingLandmark) {
                Check(sample.signedShoreDistance >= 30.0f,
                      "authored clearing rocks remain on deep interior ground");
            } else if (rock.cliffTalus) {
                ++cliffTalusRockCount;
                Check(rock.solid && sample.coastalRockiness > .90f
                          && sample.signedShoreDistance >= 1.4f
                          && sample.signedShoreDistance <= 2.8f,
                      "authored talus blocks remain solid at the foot of the rocky headland");
            } else {
                Check(sample.signedShoreDistance >= 1.5f
                          && sample.signedShoreDistance <= 202.0f,
                      "procedural land rocks stay inside their placement belt");
            }
            Check(sample.height >= 0.15f && sample.slopeRadians <= 0.5759587f,
                  "land rocks remain on suitable unsubmerged ground");
            if (rock.clearingLandmark) {
                ++clearingLandmarkRockCount;
                const float eastDistance = std::hypot(
                    rock.x - island::VegetationLayout::EastClearing.x,
                    rock.z - island::VegetationLayout::EastClearing.z);
                const float westDistance = std::hypot(
                    rock.x - island::VegetationLayout::WestClearing.x,
                    rock.z - island::VegetationLayout::WestClearing.z);
                const island::ClearingDefinition& clearing = eastDistance < westDistance
                    ? island::VegetationLayout::EastClearing
                    : island::VegetationLayout::WestClearing;
                const float clearingDistance = std::min(eastDistance, westDistance);
                Check(clearingDistance > clearing.radius * 0.70f
                          && clearingDistance < clearing.radius * 0.92f,
                      "authored clearing rocks stay in a traversable outer landmark ring");
                Check(island::VegetationLayout::IsInsideAuthoredClearing(rock.x, rock.z),
                      "authored clearing rocks remain associated with a clearing");
            } else {
                Check(!island::VegetationLayout::IsReservedRoute(rock.x, rock.z),
                      "procedural land rocks keep authored routes and clearings open");
                if (island::IslandEnvironmentField::SampleCoastalExperience(
                        rock.x,rock.z,sample.signedShoreDistance).rockyShore>.50f) {
                    ++rockyShoreRockCount;
                }
            }
            if (rock.cliffTalus) {
                Check(rock.scaleX >= 1.5f && rock.scaleX <= 3.4f
                          && rock.scaleY >= 1.1f && rock.scaleY <= 2.3f
                          && rock.scaleZ >= 1.8f && rock.scaleZ <= 3.0f,
                      "cliff talus retains varied large-block proportions");
            } else {
                Check(rock.scaleX > 0.35f && rock.scaleX < 2.30f
                          && rock.scaleY > 0.24f && rock.scaleY < 1.70f
                          && rock.scaleZ > 0.35f && rock.scaleZ < 2.35f,
                      "land-rock axis scales remain inside conservative visual bounds");
            }
        }
        Check(rock.variant < island::NaturalPropLayout::RockVariantCount,
              "rock variant index remains in range");
        if (rock.solid) ++solidRockCount;
    }
    Check(reefRockCount >= 12U && reefRockCount <= 34U,
          "reef outcrops stay inside their sparse shoreline-composition budget");
    Check(clearingLandmarkRockCount
              == static_cast<std::size_t>(
                  island::NaturalPropLayout::AuthoredClearingRockCount),
          "both clearing landmark rock groups remain complete");
    Check(rockyShoreRockCount>=8U,
        "exposed rocky-shore experience retains a visible procedural rock composition");
    Check(tidePoolRockCount==island::TidePoolLayout::Definitions.size()*2U,
        "each tide pool retains two lightweight rim-rock accents");
    Check(streamBankRockCount==props.StreamBankRockCount()&&streamBankRockCount>=10U
            &&streamBankRockCount<=island::NaturalPropLayout::StreamBankRockCandidateCount,
        "stream retains a bounded deterministic non-solid bank-stone composition");
    Check(cliffTalusRockCount
              == static_cast<std::size_t>(island::NaturalPropLayout::AuthoredCliffTalusCount),
          "the eastern cliff retains its complete authored talus group");
    std::size_t clearingLandmarkLogCount = 0U;
    std::size_t wetBankLogCount = 0U;
    bool eastClearingLogObserved = false;
    bool westClearingLogObserved = false;
    for (std::size_t index = 0; index < props.FallenLogs().size(); ++index) {
        const island::FallenLogPlacement& log = props.FallenLogs()[index];
        const island::FallenLogPlacement& repeated = repeatedProps.FallenLogs()[index];
        Check(log.x == repeated.x && log.y == repeated.y && log.z == repeated.z
                  && log.yawRadians == repeated.yawRadians && log.length == repeated.length
                  && log.radius == repeated.radius && log.variant == repeated.variant
                  && log.clearingLandmark == repeated.clearingLandmark
                  && log.wetBank == repeated.wetBank,
              "fallen-log placement is bitwise deterministic for one seed");
        Check(log.length >= island::NaturalPropLayout::LogMinimumLength
                  && log.length <= island::NaturalPropLayout::LogMaximumLength,
              "fallen-log length remains in the authored range");
        Check(log.radius >= 0.24f && log.radius <= 0.43f,
              "fallen-log radius remains in the authored range");
        Check(log.variant < island::NaturalPropLayout::LogVariantCount,
              "fallen-log variant index remains in range");
        if (log.wetBank) {
            const auto sample = terrain.Sample(log.x, log.z);
            const auto field = island::IslandEnvironmentField::SampleInland(
                log.x, log.z, sample.signedShoreDistance);
            Check(!log.clearingLandmark && field.moisture >= .38f
                      && field.drainage <= .72f,
                "damp-bank logs stay beside rather than across the stream");
            ++wetBankLogCount;
        }
        const float directionX = std::cos(log.yawRadians);
        const float directionZ = std::sin(log.yawRadians);
        for (int sampleIndex = 0; sampleIndex < 7; ++sampleIndex) {
            const float t = static_cast<float>(sampleIndex) / 6.0f - 0.5f;
            const float sampleX = log.x + directionX * log.length * t;
            const float sampleZ = log.z + directionZ * log.length * t;
            if (log.clearingLandmark) {
                Check(island::VegetationLayout::IsInsideAuthoredClearing(sampleX, sampleZ),
                      "the complete landmark-log footprint stays inside its clearing");
                const float eastDistance = std::hypot(
                    log.x - island::VegetationLayout::EastClearing.x,
                    log.z - island::VegetationLayout::EastClearing.z);
                const float westDistance = std::hypot(
                    log.x - island::VegetationLayout::WestClearing.x,
                    log.z - island::VegetationLayout::WestClearing.z);
                const bool usesEastClearing = eastDistance < westDistance;
                const island::ClearingDefinition& clearing = usesEastClearing
                    ? island::VegetationLayout::EastClearing
                    : island::VegetationLayout::WestClearing;
                Check(std::hypot(sampleX - clearing.x, sampleZ - clearing.z)
                          > clearing.radius * 0.56f,
                      "the landmark log preserves the clearing's central walking room");
                eastClearingLogObserved = eastClearingLogObserved || usesEastClearing;
                westClearingLogObserved = westClearingLogObserved || !usesEastClearing;
            } else {
                Check(!island::VegetationLayout::IsReservedRoute(sampleX, sampleZ),
                      "procedural fallen logs stay outside authored routes");
            }
            Check(terrain.Sample(sampleX, sampleZ).signedShoreDistance
                      >= (log.wetBank ? 8.0f : 30.0f),
                  "the complete fallen-log footprint stays on suitable island ground");
        }
        if (log.clearingLandmark) {
            ++clearingLandmarkLogCount;
            // a landmark log lies along the contour, not bridged out over the slope
            float highest = -1.0e9f, lowest = 1.0e9f;
            for (int sampleIndex = 0; sampleIndex < 7; ++sampleIndex) {
                const float t = static_cast<float>(sampleIndex) / 6.0f - 0.5f;
                const float height = terrain.Height(log.x + std::cos(log.yawRadians) * log.length * t,
                                                    log.z + std::sin(log.yawRadians) * log.length * t);
                highest = std::max(highest, height);
                lowest = std::min(lowest, height);
            }
            Check(highest - lowest < 0.45f, "a clearing landmark log lies along the slope's contour");
        }
    }
    Check(wetBankLogCount == props.StreamBankLogCount() && wetBankLogCount >= 2
        && wetBankLogCount <= island::NaturalPropLayout::StreamBankLogCandidateCount,
        "stream has a bounded deterministic fallen-wood composition");
    Check(clearingLandmarkLogCount
              == static_cast<std::size_t>(
                  island::NaturalPropLayout::AuthoredClearingLogCount),
          "both clearing fallen-log landmarks remain present");
    Check(eastClearingLogObserved && westClearingLogObserved,
          "each authored clearing retains one fallen-log landmark");
    std::array<bool, island::NaturalPropLayout::DriftwoodVariantCount>
        observedDriftwoodVariants{};
    std::size_t coveLandmarkDriftwoodCount = 0U;
    std::size_t shelteredDriftwoodCount=0U,exposedDriftwoodCount=0U;
    std::array<bool, island::NaturalPropLayout::DriftwoodVariantCount>
        coveLandmarkVariants{};
    for (std::size_t index = 0; index < props.Driftwood().size(); ++index) {
        const island::DriftwoodPlacement& wood = props.Driftwood()[index];
        const island::DriftwoodPlacement& repeated = repeatedProps.Driftwood()[index];
        Check(wood.x == repeated.x && wood.y == repeated.y && wood.z == repeated.z
                  && wood.yawRadians == repeated.yawRadians && wood.length == repeated.length
                  && wood.radius == repeated.radius && wood.variant == repeated.variant
                  && wood.coveLandmark == repeated.coveLandmark,
              "driftwood placement is bitwise deterministic for one seed");
        Check(wood.length >= island::NaturalPropLayout::DriftwoodMinimumLength
                  && wood.length <= island::NaturalPropLayout::DriftwoodMaximumLength,
              "driftwood length remains in the authored range");
        Check(wood.radius >= 0.09f && wood.radius <= 0.18f,
              "driftwood radius remains in the authored range");
        Check(wood.variant < island::NaturalPropLayout::DriftwoodVariantCount,
              "driftwood variant index remains in range");
        if (wood.variant < observedDriftwoodVariants.size()) {
            observedDriftwoodVariants[wood.variant] = true;
        }
        const auto woodExperience=island::IslandEnvironmentField::SampleCoastalExperience(
            wood.x,wood.z,terrain.SignedShoreDistance(wood.x,wood.z));
        if (std::max(woodExperience.shelteredBay,woodExperience.mangroveMouth)>.35f)
            ++shelteredDriftwoodCount;
        if (woodExperience.rockyShore>.35f) ++exposedDriftwoodCount;
        if (wood.coveLandmark) {
            ++coveLandmarkDriftwoodCount;
            const float angle = std::atan2(wood.z, wood.x);
            Check(std::abs(std::remainder(
                      angle - island::NaturalPropLayout::AuthoredCoveAngle,
                      2.0f * std::numbers::pi_v<float>)) < 0.05f,
                  "authored driftwood remains inside its sheltered cove arc");
            Check(terrain.SignedShoreDistance(wood.x, wood.z) >= 6.8f
                      && terrain.SignedShoreDistance(wood.x, wood.z) <= 10.7f,
                  "authored cove driftwood centres remain on dry beach");
            const auto centre = terrain.Sample(wood.x, wood.z);
            const auto coast = island::IslandEnvironmentField::SampleCoast(
                wood.x, wood.z, centre.signedShoreDistance);
            Check(coast.beachSuitability >= 0.72f,
                  "authored cove driftwood remains on visibly sandy coast");
            if (wood.variant < coveLandmarkVariants.size()) {
                coveLandmarkVariants[wood.variant] = true;
            }
            for (const island::PalmPlacement& palm : vegetation.Palms()) {
                Check(std::hypot(wood.x - palm.x, wood.z - palm.z) >= 2.0f,
                      "authored cove driftwood remains clear of palm trunks");
            }
            for (const island::CanopyPlacement& tree : jungle.Canopy()) {
                Check(std::hypot(wood.x - tree.x, wood.z - tree.z) >= 2.0f,
                      "authored cove driftwood remains clear of canopy trunks");
            }
            for (const island::DriftwoodPlacement& other : props.Driftwood()) {
                if (&other == &wood) continue;
                Check(std::hypot(wood.x - other.x, wood.z - other.z) >= 2.4f,
                      "authored cove driftwood remains visually separated");
            }
        }
        const float directionX = std::cos(wood.yawRadians);
        const float directionZ = std::sin(wood.yawRadians);
        float minimumSupportHeight = terrain.Height(wood.x, wood.z);
        float maximumSupportHeight = minimumSupportHeight;
        float maximumSupportSlope = terrain.Sample(wood.x, wood.z).slopeRadians;
        for (int sampleIndex = 0; sampleIndex < 5; ++sampleIndex) {
            const float t = static_cast<float>(sampleIndex) / 4.0f - 0.5f;
            const float sampleX = wood.x + directionX * wood.length * t;
            const float sampleZ = wood.z + directionZ * wood.length * t;
            const island::TerrainSample support = terrain.Sample(sampleX, sampleZ);
            minimumSupportHeight = std::min(minimumSupportHeight, support.height);
            maximumSupportHeight = std::max(maximumSupportHeight, support.height);
            maximumSupportSlope = std::max(maximumSupportSlope, support.slopeRadians);
            Check(support.signedShoreDistance >= 1.5f
                      && support.signedShoreDistance <= 27.0f,
                  "the complete driftwood footprint stays in the dry beach belt");
            Check(!island::VegetationLayout::IsReservedRoute(sampleX, sampleZ),
                  "the complete driftwood footprint stays outside authored routes");
        }
        if (wood.coveLandmark) {
            Check(maximumSupportSlope <= 0.17f,
                  "authored cove driftwood lies on a level beach footprint");
            Check(maximumSupportHeight - minimumSupportHeight <= 0.22f,
                  "authored cove driftwood has no buried or hovering end");
        }
    }
    for (const bool observed : observedDriftwoodVariants) {
        Check(observed, "every authored driftwood silhouette remains represented");
    }
    Check(coveLandmarkDriftwoodCount
              == static_cast<std::size_t>(
                  island::NaturalPropLayout::AuthoredCoveDriftwoodCount),
          "the authored cove driftwood landmark remains complete");
    Check(std::ranges::all_of(coveLandmarkVariants,
                              [](const bool value) { return value; }),
          "the authored cove landmark uses all three driftwood silhouettes");
    std::array<bool, island::NaturalPropLayout::BeachDetailVariantCount>
        observedBeachDetailVariants{};
    std::size_t broadBeachShellDetailCount=0U,broadBeachWrackDetailCount=0U;
    std::size_t shelteredBayWrackDetailCount=0U,mangroveMouthWrackDetailCount=0U;
    std::size_t tidePoolShellDetailCount=0U;
    std::size_t shelteredBeachDetailCount=0U,exposedBeachDetailCount=0U;
    std::size_t turtleTrackMarkCount=0U,turtleNestCount=0U;
    std::vector<std::array<float,2>> turtleNestPositions;
    const island::RenderedTerrainSurface drawnGround(terrain);
    for (std::size_t index = 0; index < props.BeachDetails().size(); ++index) {
        const island::BeachDetailPlacement& detail = props.BeachDetails()[index];
        const island::BeachDetailPlacement& repeated = repeatedProps.BeachDetails()[index];
        const island::TerrainSample sample = terrain.Sample(detail.x, detail.z);
        Check(detail.x == repeated.x && detail.y == repeated.y
                  && detail.z == repeated.z
                  && detail.yawRadians == repeated.yawRadians
                  && detail.scale == repeated.scale && detail.variant == repeated.variant
                  && detail.tidePoolLandmark == repeated.tidePoolLandmark
                  && detail.turtleTrace == repeated.turtleTrace
                  && detail.turtleNest == repeated.turtleNest,
              "shell/pebble/wrack placement is bitwise deterministic for one seed");
        if (detail.tidePoolLandmark) {
            ++tidePoolShellDetailCount;
            Check(sample.signedShoreDistance>=20.0f
                    && sample.signedShoreDistance<=40.0f
                    && sample.coastalRockiness>.50f && detail.variant<3U,
                "tide-pool shell accents remain on rocky rims and never use wrack");
        } else if (detail.turtleTrace) {
            Check(sample.signedShoreDistance >= 3.0f
                      && sample.signedShoreDistance <= 18.0f
                      && sample.height >= 0.22f
                      && sample.slopeRadians <= 0.20943951f
                      && detail.x < -100.0f,
                  "turtle nesting traces stay on one dry low-slope western beach");
            Check((detail.turtleNest && detail.variant == 5U)
                      || (!detail.turtleNest && detail.variant == 4U),
                  "turtle trace kind selects its dedicated track or nest silhouette");
            if (detail.turtleNest) {
                ++turtleNestCount;
                turtleNestPositions.push_back({detail.x,detail.z});
            } else {
                ++turtleTrackMarkCount;
            }
        } else {
            Check(sample.signedShoreDistance >= 3.0f
                      && sample.signedShoreDistance <= 18.0f,
                  "shell/pebble/wrack clusters remain in the authored dry-sand belt");
            Check(sample.height >= 0.22f && sample.slopeRadians <= 0.2617994f,
                  "shell/pebble/wrack clusters stay on dry low-slope support");
        }
        CheckNear(detail.y, drawnGround.Sample(detail.x, detail.z).height
                      + (detail.turtleNest ? 0.02f : detail.turtleTrace ? 0.008f : 0.012f),
                  0.0001f,
                  "shell/pebble/wrack clusters rest just above the drawn ground");
        if (detail.turtleTrace)
            Check(std::abs(detail.normalX - sample.normal.x) < 1e-5f && std::abs(detail.normalY - sample.normal.y) < 1e-5f
                      && std::abs(detail.normalZ - sample.normal.z) < 1e-5f,
                  "the turtle traces are laid along the ground's slope");
        Check(!island::VegetationLayout::IsReservedRoute(detail.x, detail.z),
              "shell/pebble/wrack clusters keep authored routes and spawn clear");
        Check(detail.variant < observedBeachDetailVariants.size(),
              "shell/pebble/wrack cluster variant index remains in range");
        if (detail.variant < observedBeachDetailVariants.size()) {
            observedBeachDetailVariants[detail.variant] = true;
        }
        const auto experience=island::IslandEnvironmentField::SampleCoastalExperience(
            detail.x,detail.z,sample.signedShoreDistance);
        if (!detail.turtleTrace && experience.broadBeach>.50f) {
            if (detail.variant==3U) ++broadBeachWrackDetailCount;
            else ++broadBeachShellDetailCount;
        }
        if (!detail.turtleTrace && experience.shelteredBay>.50f && detail.variant==3U)
            ++shelteredBayWrackDetailCount;
        if (!detail.turtleTrace && experience.mangroveMouth>.35f && detail.variant==3U)
            ++mangroveMouthWrackDetailCount;
        if (!detail.tidePoolLandmark && !detail.turtleTrace
                && std::max(experience.shelteredBay,experience.mangroveMouth)>.35f)
            ++shelteredBeachDetailCount;
        if (!detail.tidePoolLandmark && !detail.turtleTrace && experience.rockyShore>.35f)
            ++exposedBeachDetailCount;
        Check(detail.scale >= island::NaturalPropLayout::BeachDetailMinimumScale
                  && detail.scale <= island::NaturalPropLayout::BeachDetailMaximumScale,
              "shell/pebble/wrack cluster scale remains in the authored range");
    }
    Check(std::ranges::all_of(observedBeachDetailVariants,
                              [](const bool value) { return value; }),
          "beach detail population exercises shell, wrack, track and nest variants");
    Check(turtleTrackMarkCount == static_cast<std::size_t>(
              island::NaturalPropLayout::TurtleNestingTrackCount
                  * island::NaturalPropLayout::TurtleTrackSegmentsPerNest)
              && turtleNestCount
                  == static_cast<std::size_t>(island::NaturalPropLayout::TurtleNestingTrackCount),
          "one western nesting beach has two complete turtle approaches and nests");
    Check(turtleNestPositions.size()==2U
              && std::hypot(turtleNestPositions[0][0]-turtleNestPositions[1][0],
                            turtleNestPositions[0][1]-turtleNestPositions[1][1])>=18.0f,
          "the two subtle turtle nests remain separately readable on the same beach");
    std::cout << "Coastal identity props rocky/south-shell/south-wrack/bay-wrack/mouth-wrack: "
        << rockyShoreRockCount << '/' << broadBeachShellDetailCount << '/'
        << broadBeachWrackDetailCount << '/' << shelteredBayWrackDetailCount << '/'
        << mangroveMouthWrackDetailCount << '\n';
    Check(broadBeachShellDetailCount>=12U
            && broadBeachShellDetailCount>broadBeachWrackDetailCount*3U,
        "broad Robinson beach is visually biased toward shell and pebble detail");
    Check(shelteredBayWrackDetailCount>=3U && mangroveMouthWrackDetailCount>=1U,
        "sheltered bay and localized mangrove mouth retain distinct wrack detail");
    Check(tidePoolShellDetailCount==island::TidePoolLayout::Definitions.size(),
        "each tide pool retains one lightweight shell/pebble rim accent");
    std::array<bool, island::NaturalPropLayout::FallenFrondVariantCount>
        observedFallenFrondVariants{};
    std::size_t shelteredCoastalFrondCount=0U,exposedCoastalFrondCount=0U;
    for (std::size_t index = 0; index < props.FallenFronds().size(); ++index) {
        const island::FallenFrondPlacement& frond = props.FallenFronds()[index];
        const island::FallenFrondPlacement& repeated = repeatedProps.FallenFronds()[index];
        Check(frond.x == repeated.x && frond.y == repeated.y && frond.z == repeated.z
                  && frond.yawRadians == repeated.yawRadians
                  && frond.scale == repeated.scale && frond.variant == repeated.variant,
              "fallen-frond placement is bitwise deterministic for one seed");
        Check(frond.scale >= island::NaturalPropLayout::FallenFrondMinimumScale
                  && frond.scale <= island::NaturalPropLayout::FallenFrondMaximumScale,
              "fallen-frond scale remains in the authored range");
        Check(frond.variant < observedFallenFrondVariants.size(),
              "fallen-frond variant index remains in range");
        if (frond.variant < observedFallenFrondVariants.size()) {
            observedFallenFrondVariants[frond.variant] = true;
        }
        const auto frondSample=terrain.Sample(frond.x,frond.z);
        const auto frondExperience=island::IslandEnvironmentField::SampleCoastalExperience(
            frond.x,frond.z,frondSample.signedShoreDistance);
        if (frondSample.signedShoreDistance<68.0f
                && std::max(frondExperience.shelteredBay,frondExperience.mangroveMouth)>.35f)
            ++shelteredCoastalFrondCount;
        if (frondSample.signedShoreDistance<68.0f && frondExperience.rockyShore>.35f)
            ++exposedCoastalFrondCount;
        const float directionX = std::cos(frond.yawRadians);
        const float directionZ = std::sin(frond.yawRadians);
        float maximumSupportHeight = -std::numeric_limits<float>::infinity();
        for (int sampleIndex = 0; sampleIndex < 5; ++sampleIndex) {
            const float t = static_cast<float>(sampleIndex) / 4.0f - 0.5f;
            const float sampleX = frond.x + directionX * frond.scale * t;
            const float sampleZ = frond.z + directionZ * frond.scale * t;
            const island::TerrainSample support = terrain.Sample(sampleX, sampleZ);
            maximumSupportHeight = std::max(maximumSupportHeight, support.height);
            Check(support.signedShoreDistance >= 6.0f
                      && support.signedShoreDistance <= 190.0f,
                  "the complete fallen-frond footprint stays on coastal/interior ground");
            Check(support.slopeRadians <= 0.34906585f,
                  "the complete fallen-frond footprint stays on gentle support");
            Check(!island::VegetationLayout::IsReservedRoute(sampleX, sampleZ),
                  "the complete fallen-frond footprint keeps authored routes open");
        }
        CheckNear(frond.y, maximumSupportHeight + 0.022f, 0.0001f,
                  "fallen fronds rest on the highest sampled terrain support");
        for (std::size_t other = 0; other < index; ++other) {
            const island::FallenFrondPlacement& previous = props.FallenFronds()[other];
            Check(std::hypot(frond.x - previous.x, frond.z - previous.z) >= 2.899f,
                  "fallen fronds preserve their minimum centre spacing");
        }
    }
    Check(std::ranges::all_of(observedFallenFrondVariants,
                              [](const bool value) { return value; }),
          "fallen-frond population exercises all three colour/shape variants");
    std::cout << "Wind-sorted coastal debris sheltered/exposed wood,detail,fronds: "
        << shelteredDriftwoodCount << '/' << exposedDriftwoodCount << ", "
        << shelteredBeachDetailCount << '/' << exposedBeachDetailCount << ", "
        << shelteredCoastalFrondCount << '/' << exposedCoastalFrondCount << '\n';
    Check(shelteredDriftwoodCount>=island::NaturalPropLayout::AuthoredCoveDriftwoodCount
            && shelteredDriftwoodCount>=exposedDriftwoodCount*2U,
        "driftwood is more frequent in the leeward bay than on exposed rocky headlands");
    Check(shelteredBeachDetailCount*3U>=exposedBeachDetailCount*5U,
        "shell, pebble and wrack detail accumulates more strongly in sheltered shore sectors");
    Check(shelteredCoastalFrondCount>exposedCoastalFrondCount,
        "coastal fallen fronds accumulate more strongly in shelter than on rocky headlands");
    Check(props.Obstacles().size() == solidRockCount + props.FallenLogs().size() * 7U,
          "natural-prop collision excludes small decorative props and matches solid spans");

    for (int z = -360; z <= 360; z += 12) {
        for (int x = -360; x <= 360; x += 12) {
            const float first = terrain.Height(static_cast<float>(x), static_cast<float>(z));
            const float second = terrain.Height(static_cast<float>(x), static_cast<float>(z));
            Check(std::isfinite(first), "terrain height is finite across the world grid");
            Check(first == second, "terrain generation is deterministic");
            Check(first >= -22.01f && first <= 45.0f, "terrain height stays in the authored scale");
            const island::TerrainSample sample = terrain.Sample(static_cast<float>(x),
                                                                 static_cast<float>(z));
            Check(std::isfinite(sample.slopeRadians), "terrain slope is finite across the world grid");
            Check(sample.slopeRadians >= 0.0f
                      && sample.slopeRadians <= std::numbers::pi_v<float> * 0.5f,
                  "terrain slope remains in the upward-normal range");
        }
    }

    island::PlayerMotion player;
    player.SetPosition(terrain, 0.0f, 0.0f);
    const island::PlayerPosition start = player.Position();
    Check(island::PlayerMotion::GroundContactOffsets.size() == 9U,
          "capsule ground query exposes all nine diagnostic samples");
    CheckNear(island::PlayerMotion::GroundContactOffsets.front().x, 0.0f, 0.0001f,
              "capsule ground query begins at the footprint centre x");
    CheckNear(island::PlayerMotion::GroundContactOffsets.front().z, 0.0f, 0.0001f,
              "capsule ground query begins at the footprint centre z");
    float maximumGroundSampleHeight = -std::numeric_limits<float>::infinity();
    for (const island::PlayerGroundOffset& offset
         : island::PlayerMotion::GroundContactOffsets) {
        Check(std::hypot(offset.x, offset.z) <= 1.0001f,
              "capsule ground sample stays inside the unit footprint");
        const island::TerrainSample sample = terrain.Sample(
            start.x + offset.x * island::PlayerMotion::CapsuleRadius,
            start.z + offset.z * island::PlayerMotion::CapsuleRadius);
        Check(std::isfinite(sample.height) && std::isfinite(sample.slopeRadians),
              "capsule diagnostic sample is finite");
        maximumGroundSampleHeight = std::max(maximumGroundSampleHeight, sample.height);
    }
    CheckNear(maximumGroundSampleHeight, player.GroundHeight(), 0.0001f,
              "capsule diagnostics reproduce the authoritative support height");
    Check(player.GroundHeight() >= terrain.Height(0.0f, 0.0f),
          "capsule footprint resolves at or above its centre terrain height");
    Check(player.Position().y >= player.GroundHeight() + island::PlayerMotion::EyeHeight - 0.0001f,
          "walking eye never starts below its resolved ground support");
    Check(player.GroundSlopeRadians() >= terrain.Sample(0.0f, 0.0f).slopeRadians,
          "capsule ground slope includes the centre sample");
    player.Update(terrain, 1.0f, 0.0f, 1.0f / 60.0f);
    Check(player.Position().x > start.x, "walking input accelerates the player");
    const island::PlayerMovementTrace firstMovementTrace = player.MovementTrace();
    Check(firstMovementTrace.valid && firstMovementTrace.startX == start.x
              && firstMovementTrace.attemptedX > firstMovementTrace.startX,
          "movement diagnostics retain the exact nonzero attempted step");
    CheckNear(firstMovementTrace.resolvedX, player.Position().x, 0.0001f,
              "movement diagnostics retain the exact resolved player position");
    Check(firstMovementTrace.obstacleRejections == 0
              && firstMovementTrace.terrainRejections == 0
              && firstMovementTrace.slideSubsteps == 0
              && firstMovementTrace.obstacleDepenetrations == 0
              && !firstMovementTrace.safetyClamped,
          "unobstructed movement diagnostics report no rejection or clamp");
    Check(player.VelocityX() > 0.0f && player.VelocityX() < island::PlayerMotion::WalkSpeed,
          "walking acceleration does not jump to full speed");
    Check(player.Mode() == island::LocomotionMode::Walking,
          "interior movement stays in walking mode");
    CheckNear(player.OceanCurrentStrength(terrain), 0.0f, 0.0001f,
              "ocean current remains inactive while walking");

    island::PlayerMotion invalidInputPlayer;
    invalidInputPlayer.SetPosition(terrain, 12.0f, -9.0f);
    const island::PlayerPosition validPlayerPosition = invalidInputPlayer.Position();
    invalidInputPlayer.Update(terrain, quietNan,
                              std::numeric_limits<float>::infinity(), quietNan);
    Check(invalidInputPlayer.Position().x == validPlayerPosition.x
              && invalidInputPlayer.Position().y == validPlayerPosition.y
              && invalidInputPlayer.Position().z == validPlayerPosition.z
              && invalidInputPlayer.VelocityX() == 0.0f
              && invalidInputPlayer.VelocityZ() == 0.0f,
          "player movement rejects non-finite frame and direction input without state poisoning");
    invalidInputPlayer.SetPosition(
        terrain, quietNan, std::numeric_limits<float>::infinity());
    Check(invalidInputPlayer.Position().x == validPlayerPosition.x
              && invalidInputPlayer.Position().z == validPlayerPosition.z
              && std::isfinite(invalidInputPlayer.Position().y),
          "player placement retains the last valid coordinates for non-finite input");

    island::PlayerMotion cardinalAccelerationPlayer;
    island::PlayerMotion diagonalAccelerationPlayer;
    cardinalAccelerationPlayer.SetPosition(terrain, 0.0f, 0.0f);
    diagonalAccelerationPlayer.SetPosition(terrain, 0.0f, 0.0f);
    cardinalAccelerationPlayer.Update(terrain, 0.0f, 1.0f, 1.0f / 60.0f);
    diagonalAccelerationPlayer.Update(terrain, 1.0f, 1.0f, 1.0f / 60.0f);
    const float cardinalAccelerationSpeed = std::hypot(
        cardinalAccelerationPlayer.VelocityX(), cardinalAccelerationPlayer.VelocityZ());
    const float diagonalAccelerationSpeed = std::hypot(
        diagonalAccelerationPlayer.VelocityX(), diagonalAccelerationPlayer.VelocityZ());
    CheckNear(diagonalAccelerationSpeed, cardinalAccelerationSpeed, 0.0001f,
              "walking acceleration is independent of movement direction");
    cardinalAccelerationPlayer.Update(terrain, 0.0f, 0.0f, 1.0f / 60.0f);
    diagonalAccelerationPlayer.Update(terrain, 0.0f, 0.0f, 1.0f / 60.0f);
    CheckNear(std::hypot(diagonalAccelerationPlayer.VelocityX(),
                         diagonalAccelerationPlayer.VelocityZ()),
              std::hypot(cardinalAccelerationPlayer.VelocityX(),
                         cardinalAccelerationPlayer.VelocityZ()),
              0.0001f,
              "walking deceleration is independent of movement direction");

    island::PlayerMotion runningPlayer;
    // Speed selection needs an unobstructed gentle slope; the island centre is now a saddle
    // approach whose bank correctly rejects some northward movement. Steep-bank behaviour
    // has its own independent test below.
    for (int z = -170; z <= -140; ++z) {
        const auto runningSurface = terrain.Sample(0.0f, static_cast<float>(z));
        Check(runningSurface.height > 0.5f && runningSurface.slopeRadians < 0.25f,
              "run-speed fixture remains dry and gently sloped");
    }
    runningPlayer.SetPosition(terrain, 0.0f, -170.0f);
    runningPlayer.SetRunning(true);
    for (int frame = 0; frame < 120; ++frame) {
        runningPlayer.Update(terrain, 0.0f, 1.0f, 1.0f / 60.0f);
    }
    Check(island::PlayerMotion::RunSpeed >= 8.0f,
          "selected run mode provides the faster requested land pace");
    Check(runningPlayer.IsRunning(), "selected run mode activates the running land pace");
    CheckNear(std::hypot(runningPlayer.VelocityX(), runningPlayer.VelocityZ()),
              island::PlayerMotion::RunSpeed, 0.001f,
              "running acceleration reaches the bounded running speed");
    runningPlayer.SetRunning(false);
    for (int frame = 0; frame < 120; ++frame) {
        runningPlayer.Update(terrain, 0.0f, 1.0f, 1.0f / 60.0f);
    }
    Check(!runningPlayer.IsRunning(), "selecting walk mode restores the walking land pace");
    CheckNear(std::hypot(runningPlayer.VelocityX(), runningPlayer.VelocityZ()),
              island::PlayerMotion::WalkSpeed, 0.001f,
              "selected walk mode settles at the bounded walking speed");

    float coveAngle = 0.0f;
    float headlandAngle = 0.0f;
    float minimumShoreRadius = std::numeric_limits<float>::infinity();
    float maximumShoreRadius = 0.0f;
    for (int sampleIndex = 0; sampleIndex < 720; ++sampleIndex) {
        const float angle = -std::numbers::pi_v<float>
            + 2.0f * std::numbers::pi_v<float>
                * static_cast<float>(sampleIndex) / 720.0f;
        const float radius = terrain.ShoreRadius(angle);
        if (radius < minimumShoreRadius) {
            minimumShoreRadius = radius;
            coveAngle = angle;
        }
        if (radius > maximumShoreRadius) {
            maximumShoreRadius = radius;
            headlandAngle = angle;
        }
    }
    const std::array<float, 6> shorelineTransects = {
        coveAngle, headlandAngle, -2.35f, -0.78f, 0.69f, 2.18f};
    int completedShorelineRoundTrips = 0;
    bool shorelineTraversalStayedFinite = true;
    for (const float authoredAngle : shorelineTransects) {
        // Rocky headlands now intentionally block climbing. Test their adjacent sandy
        // access here, and the actual cliff barrier independently below.
        const float angle = island::IslandEnvironmentField::CoastalRockPotential(authoredAngle) > 0.55f
            ? authoredAngle + 0.65f : authoredAngle;
        const float outwardX = std::cos(angle);
        const float outwardZ = std::sin(angle);
        const float shoreRadius = terrain.ShoreRadius(angle);
        island::PlayerMotion transectPlayer;
        transectPlayer.SetPosition(
            terrain, outwardX * (shoreRadius - 14.0f),
            outwardZ * (shoreRadius - 14.0f));
        Check(transectPlayer.Mode() == island::LocomotionMode::Walking,
              "shoreline traversal begins on walkable dry ground");

        int outwardWaterEntries = 0;
        float maximumOffshoreDistance = 0.0f;
        island::LocomotionMode previousMode = transectPlayer.Mode();
        for (int step = 0; step < 900; ++step) {
            transectPlayer.Update(terrain, outwardX, outwardZ, 1.0f / 60.0f);
            if (previousMode == island::LocomotionMode::Walking
                && transectPlayer.Mode() == island::LocomotionMode::Swimming) {
                ++outwardWaterEntries;
            }
            previousMode = transectPlayer.Mode();
            maximumOffshoreDistance = std::max(
                maximumOffshoreDistance, transectPlayer.OffshoreDistance(terrain));
            shorelineTraversalStayedFinite = shorelineTraversalStayedFinite
                && std::isfinite(transectPlayer.Position().x)
                && std::isfinite(transectPlayer.Position().y)
                && std::isfinite(transectPlayer.Position().z);
            if (transectPlayer.Mode() == island::LocomotionMode::Swimming
                && maximumOffshoreDistance >= 20.0f) {
                break;
            }
        }

        int inwardLandExits = 0;
        previousMode = transectPlayer.Mode();
        for (int step = 0; step < 1200; ++step) {
            transectPlayer.Update(terrain, -outwardX, -outwardZ, 1.0f / 60.0f);
            if (previousMode == island::LocomotionMode::Swimming
                && transectPlayer.Mode() == island::LocomotionMode::Walking) {
                ++inwardLandExits;
            }
            previousMode = transectPlayer.Mode();
            shorelineTraversalStayedFinite = shorelineTraversalStayedFinite
                && std::isfinite(transectPlayer.Position().x)
                && std::isfinite(transectPlayer.Position().y)
                && std::isfinite(transectPlayer.Position().z);
            if (transectPlayer.Mode() == island::LocomotionMode::Walking
                && terrain.SignedShoreDistance(
                    transectPlayer.Position().x, transectPlayer.Position().z) >= 5.0f) {
                break;
            }
        }

        Check(outwardWaterEntries == 1 && maximumOffshoreDistance >= 20.0f,
              "each authored shoreline transect enters stable near-shore swimming once");
        Check(inwardLandExits == 1
                  && transectPlayer.Mode() == island::LocomotionMode::Walking
                  && terrain.SignedShoreDistance(
                      transectPlayer.Position().x, transectPlayer.Position().z) >= 5.0f,
              "each authored shoreline transect returns to stable dry-ground walking once");
        if (outwardWaterEntries == 1 && inwardLandExits == 1) {
            ++completedShorelineRoundTrips;
        }
    }
    Check(completedShorelineRoundTrips
              == static_cast<int>(shorelineTransects.size()),
          "cove, headland, and varied-coast traversal scenarios all complete");
    Check(shorelineTraversalStayedFinite,
          "all shoreline traversal positions remain finite");

    island::PlayerMotion cliffPlayer;
    constexpr float cliffAngle = 0.35f;
    const float cliffRadius = terrain.ShoreRadius(cliffAngle);
    cliffPlayer.SetPosition(terrain, std::cos(cliffAngle) * (cliffRadius - 0.5f),
                           std::sin(cliffAngle) * (cliffRadius - 0.5f));
    for (int frame = 0; frame < 600; ++frame)
        cliffPlayer.Update(terrain, -std::cos(cliffAngle), -std::sin(cliffAngle), 1.0f / 60.0f);
    const float cliffReached = terrain.SignedShoreDistance(cliffPlayer.Position().x, cliffPlayer.Position().z);
    Check(cliffReached > 1.0f && cliffReached < 7.0f,
          "player can approach rocky headland but cannot climb through its steep face");
    Check(cliffPlayer.Mode() == island::LocomotionMode::Walking,
          "cliff rejection leaves player on stable dry footing");

    bool foundSteepBankScenario = false;
    float steepBankSlope = 0.0f;
    float steepBankStartX = 0.0f;
    float steepBankStartZ = 0.0f;
    float steepBankUphillX = 0.0f;
    float steepBankUphillZ = 0.0f;
    for (int z = -220; z <= 220; z += 2) {
        for (int x = -220; x <= 220; x += 2) {
            const float targetX = static_cast<float>(x);
            const float targetZ = static_cast<float>(z);
            const island::TerrainSample target = terrain.Sample(targetX, targetZ);
            if (target.signedShoreDistance < 24.0f
                || target.height < 0.2f
                || target.slopeRadians <= steepBankSlope) {
                continue;
            }
            const float gradientX = terrain.Height(targetX + 0.5f, targetZ)
                - terrain.Height(targetX - 0.5f, targetZ);
            const float gradientZ = terrain.Height(targetX, targetZ + 0.5f)
                - terrain.Height(targetX, targetZ - 0.5f);
            const float gradientLength = std::hypot(gradientX, gradientZ);
            if (gradientLength < 0.05f) continue;
            const float uphillX = gradientX / gradientLength;
            const float uphillZ = gradientZ / gradientLength;
            const float startX = targetX - uphillX * 2.4f;
            const float startZ = targetZ - uphillZ * 2.4f;
            const island::TerrainSample bankStart = terrain.Sample(startX, startZ);
            if (bankStart.height < 0.2f
                || bankStart.slopeRadians > island::PlayerMotion::MaximumWalkSlopeRadians
                || bankStart.signedShoreDistance < 20.0f) {
                continue;
            }
            foundSteepBankScenario = true;
            steepBankSlope = target.slopeRadians;
            steepBankStartX = startX;
            steepBankStartZ = startZ;
            steepBankUphillX = uphillX;
            steepBankUphillZ = uphillZ;
        }
    }
    Check(foundSteepBankScenario && steepBankSlope >= 0.30f,
          "deterministic interior search finds a representative steep-bank approach");
    if (foundSteepBankScenario) {
        island::PlayerMotion bankWalker;
        bankWalker.SetPosition(terrain, steepBankStartX, steepBankStartZ);
        const float initialGroundHeight = bankWalker.GroundHeight();
        float minimumGroundHeight = initialGroundHeight;
        float maximumGroundHeight = initialGroundHeight;
        int rejectionCount = 0;
        bool bankTraversalStayedFinite = true;
        for (int step = 0; step < 360; ++step) {
            bankWalker.Update(
                terrain, steepBankUphillX, steepBankUphillZ, 1.0f / 60.0f);
            rejectionCount += bankWalker.MovementTrace().terrainRejections;
            minimumGroundHeight = std::min(minimumGroundHeight, bankWalker.GroundHeight());
            maximumGroundHeight = std::max(maximumGroundHeight, bankWalker.GroundHeight());
            bankTraversalStayedFinite = bankTraversalStayedFinite
                && std::isfinite(bankWalker.Position().x)
                && std::isfinite(bankWalker.Position().y)
                && std::isfinite(bankWalker.Position().z);
        }
        const float uphillProgress =
            (bankWalker.Position().x - steepBankStartX) * steepBankUphillX
            + (bankWalker.Position().z - steepBankStartZ) * steepBankUphillZ;
        Check(uphillProgress > 0.5f || rejectionCount > 0,
              "steep-bank traversal either climbs valid support or exercises slope rejection");
        Check(maximumGroundHeight - minimumGroundHeight > 0.20f || rejectionCount > 0,
              "steep-bank traversal observes elevation change or an authoritative limit");
        Check(bankWalker.Mode() == island::LocomotionMode::Walking,
              "steep-bank traversal cannot spuriously enter swimming mode");
        Check(bankTraversalStayedFinite,
              "steep-bank traversal remains numerically stable");
    }

    const float eastShore = terrain.ShoreRadius(0.0f);
    player.SetPosition(terrain, eastShore + 24.0f, 0.0f);
    Check(player.Mode() == island::LocomotionMode::Swimming,
          "sufficiently deep near-shore water enters swimming mode");
    CheckNear(player.Position().y, 0.58f, 0.001f, "swimming eye rests near the water surface");
    CheckNear(player.OceanCurrentStrength(terrain), 0.0f, 0.0001f,
              "ocean current remains inactive inside the near-shore swimming area");

    const island::CircularObstacle testReef{eastShore + 25.4f, 0.0f, 0.62f};
    island::PlayerMotion reefSwimmer;
    reefSwimmer.SetPosition(terrain, eastShore + 24.0f, 0.0f);
    for (int step = 0; step < 180; ++step) {
        reefSwimmer.Update(
            terrain, 1.0f, 0.0f, 1.0f / 60.0f,
            std::span<const island::CircularObstacle>(&testReef, 1U));
    }
    const float reefClearance = island::PlayerMotion::CapsuleRadius + testReef.radius;
    Check(reefSwimmer.Position().x <= testReef.x - reefClearance + 0.001f,
          "a near-shore swimmer cannot pass through an emergent-rock proxy");
    Check(reefSwimmer.Mode() == island::LocomotionMode::Swimming,
          "emergent-rock collision is exercised while the controller remains in swimming mode");
    const island::WorldCollisionGrid reefCollisionGrid(
        std::span<const island::CircularObstacle>(&testReef, 1U));
    island::PlayerMotion indexedReefSwimmer;
    indexedReefSwimmer.SetPosition(terrain, eastShore + 24.0f, 0.0f);
    for (int step = 0; step < 180; ++step) {
        indexedReefSwimmer.Update(terrain, 1.0f, 0.0f, 1.0f / 60.0f,
                                  reefCollisionGrid);
    }
    CheckNear(indexedReefSwimmer.Position().x, reefSwimmer.Position().x, 0.0001f,
              "indexed and exhaustive swimming collision stop at the same position");
    CheckNear(indexedReefSwimmer.Position().z, reefSwimmer.Position().z, 0.0001f,
              "indexed and exhaustive swimming collision preserve identical axis sliding");

    player.SetPosition(terrain, eastShore + island::PlayerMotion::FullCurrentDistance, 0.0f);
    CheckNear(player.OceanCurrentStrength(terrain), 1.0f, 0.0001f,
              "ocean current reaches full normalized strength at its documented distance");
    const island::OceanCurrentDrift fullCurrent = player.OceanCurrentVelocity(terrain);
    Check(fullCurrent.x < -2.05f && fullCurrent.z > 0.35f
              && std::hypot(fullCurrent.x, fullCurrent.z) < 2.25f,
          "full current remains strongly shoreward with restrained coherent circulation");
    bool currentFieldRemainsSafe = true;
    for (int step = 0; step < 16; ++step) {
        const float angle = 2.0f * std::numbers::pi_v<float>
            * static_cast<float>(step) / 16.0f;
        const float outwardX = std::cos(angle);
        const float outwardZ = std::sin(angle);
        const float currentRadius = terrain.ShoreRadius(angle)
            + island::PlayerMotion::FullCurrentDistance;
        player.SetPosition(terrain, outwardX * currentRadius, outwardZ * currentRadius);
        const island::OceanCurrentDrift current = player.OceanCurrentVelocity(terrain);
        const float inwardSpeed = -(current.x * outwardX + current.z * outwardZ);
        const float circulationSpeed = -current.x * outwardZ + current.z * outwardX;
        currentFieldRemainsSafe = currentFieldRemainsSafe
            && std::isfinite(current.x) && std::isfinite(current.z)
            && inwardSpeed > 2.09f && inwardSpeed < 2.11f
            && circulationSpeed > 0.25f && circulationSpeed < 0.61f
            && inwardSpeed > circulationSpeed;
    }
    Check(currentFieldRemainsSafe,
          "full current stays finite, shoreward-dominant, and coherently circulating "
          "around the island");
    player.SetPosition(terrain, eastShore + island::PlayerMotion::SoftCurrentStart + 0.25f,
                       0.0f);
    const island::OceanCurrentDrift thresholdCurrent =
        player.OceanCurrentVelocity(terrain);
    Check(std::hypot(thresholdCurrent.x, thresholdCurrent.z) < 0.002f,
          "current velocity eases continuously from zero at the soft boundary");

    player.SetPosition(terrain, eastShore + 80.0f, 0.0f);
    const float offshoreBeforeCurrent = player.OffshoreDistance(terrain);
    for (int step = 0; step < 120; ++step) {
        player.Update(terrain, 0.0f, 0.0f, 1.0f / 60.0f);
    }
    Check(player.OffshoreDistance(terrain) < offshoreBeforeCurrent,
          "outer-ocean current gently carries an idle swimmer shoreward");
    Check(player.Position().z > 0.20f,
          "outer-ocean current gives an idle swimmer a mild coast-parallel drift");

    player.SetPosition(terrain, eastShore + 88.0f, 0.0f);
    for (int step = 0; step < 900; ++step) {
        player.Update(terrain, 1.0f, 0.0f, 1.0f / 60.0f);
        Check(player.OffshoreDistance(terrain) <= island::PlayerMotion::SafetyDistance + 0.01f,
              "swimmer cannot escape the ocean safety boundary");
    }
    Check(player.VelocityX() <= 0.01f,
          "safety boundary removes residual outward velocity");

    island::PlayerMotion safetyTracePlayer;
    safetyTracePlayer.SetPosition(
        terrain, eastShore + island::PlayerMotion::SafetyDistance + 4.0f, 0.0f);
    safetyTracePlayer.Update(terrain, 0.0f, 0.0f, 1.0f / 60.0f);
    Check(safetyTracePlayer.MovementTrace().safetyClamped
              && safetyTracePlayer.OffshoreDistance(terrain)
                  <= island::PlayerMotion::SafetyDistance + 0.01f,
          "movement diagnostics record an explicit ocean safety-boundary correction");

    player.SetPosition(terrain, 0.0f, 0.0f);
    player.Update(terrain, 1.0f, 0.0f, 5.0f);
    Check(player.Position().x - 0.0f < 0.2f,
          "long frame is clamped and cannot teleport the player");

    player.SetPosition(terrain, 35.0f, -20.0f);
    for (int step = 0; step < 600; ++step) {
        const float inputX = std::sin(static_cast<float>(step) * 0.071f);
        const float inputZ = std::cos(static_cast<float>(step) * 0.053f);
        player.Update(terrain, inputX, inputZ, step % 37 == 0 ? 0.1f : 1.0f / 60.0f);
        if (player.Mode() == island::LocomotionMode::Walking) {
            Check(player.Position().y
                      >= player.GroundHeight() + island::PlayerMotion::EyeHeight - 0.0001f,
                  "walking camera remains above the capsule terrain support");
        }
    }

    player.SetPosition(terrain, 0.0f, 0.0f);
    const island::CircularObstacle testTrunk{1.2f, 0.0f, 0.42f};
    for (int step = 0; step < 180; ++step) {
        player.Update(terrain, 1.0f, 0.0f, 1.0f / 60.0f,
                      std::span<const island::CircularObstacle>(&testTrunk, 1U));
    }
    const float trunkDistance = std::hypot(player.Position().x - testTrunk.x,
                                            player.Position().z - testTrunk.z);
    Check(trunkDistance >= island::PlayerMotion::CapsuleRadius + testTrunk.radius - 0.001f,
          "player capsule cannot enter a palm trunk proxy");
    Check(player.Position().x < testTrunk.x,
          "blocked movement does not tunnel through a trunk");
    Check(player.MovementTrace().obstacleRejections > 0
              && player.MovementTrace().terrainRejections == 0
              && player.MovementTrace().slideSubsteps > 0
              && player.MovementTrace().attemptedX > player.MovementTrace().resolvedX,
          "movement diagnostics distinguish blocked direct motion and axis sliding");

    const island::WorldCollisionGrid testCollisionGrid(
        std::span<const island::CircularObstacle>(&testTrunk, 1U));
    island::PlayerMotion indexedPlayer;
    indexedPlayer.SetPosition(terrain, 0.0f, 0.0f);
    for (int step = 0; step < 180; ++step) {
        indexedPlayer.Update(terrain, 1.0f, 0.0f, 1.0f / 60.0f,
                             testCollisionGrid);
    }
    CheckNear(indexedPlayer.Position().x, player.Position().x, 0.0001f,
              "indexed and exhaustive obstacle paths stop at the same position");
    CheckNear(indexedPlayer.Position().z, player.Position().z, 0.0001f,
              "indexed and exhaustive obstacle paths preserve identical axis sliding");

    const island::CircularObstacle embeddedObstacle{0.0f, 0.0f, 0.42f};
    const std::span<const island::CircularObstacle> embeddedObstacles(
        &embeddedObstacle, 1U);
    island::PlayerMotion embeddedPlayer;
    embeddedPlayer.SetPosition(terrain, embeddedObstacle.x, embeddedObstacle.z);
    embeddedPlayer.Update(terrain, 0.0f, 0.0f, 1.0f / 60.0f, embeddedObstacles);
    const float embeddedClearance = island::PlayerMotion::CapsuleRadius
        + embeddedObstacle.radius;
    const float recoveredDistance = std::hypot(
        embeddedPlayer.Position().x - embeddedObstacle.x,
        embeddedPlayer.Position().z - embeddedObstacle.z);
    Check(std::isfinite(embeddedPlayer.Position().x)
              && std::isfinite(embeddedPlayer.Position().z)
              && recoveredDistance >= embeddedClearance,
          "placement inside a static obstacle recovers to finite capsule clearance");
    Check(embeddedPlayer.Position().x > embeddedObstacle.x
              && std::abs(embeddedPlayer.Position().z - embeddedObstacle.z) < 0.0001f,
          "exact-centre placement recovery uses its deterministic outward direction");
    Check(embeddedPlayer.MovementTrace().valid
              && embeddedPlayer.MovementTrace().obstacleDepenetrations == 1
              && embeddedPlayer.MovementTrace().obstacleRejections == 0,
          "movement diagnostics distinguish placement recovery from blocked movement");
    const island::PlayerPosition recoveredPosition = embeddedPlayer.Position();
    embeddedPlayer.Update(terrain, 0.0f, 0.0f, 1.0f / 60.0f, embeddedObstacles);
    CheckNear(embeddedPlayer.Position().x, recoveredPosition.x, 0.0001f,
              "static placement recovery does not drift on later idle updates along x");
    CheckNear(embeddedPlayer.Position().z, recoveredPosition.z, 0.0001f,
              "static placement recovery does not drift on later idle updates along z");

    const island::WorldCollisionGrid embeddedCollisionGrid(embeddedObstacles);
    island::PlayerMotion indexedEmbeddedPlayer;
    indexedEmbeddedPlayer.SetPosition(terrain, embeddedObstacle.x, embeddedObstacle.z);
    indexedEmbeddedPlayer.Update(
        terrain, 0.0f, 0.0f, 1.0f / 60.0f, embeddedCollisionGrid);
    CheckNear(indexedEmbeddedPlayer.Position().x, recoveredPosition.x, 0.0001f,
              "indexed and exhaustive placement recovery agree along x");
    CheckNear(indexedEmbeddedPlayer.Position().z, recoveredPosition.z, 0.0001f,
              "indexed and exhaustive placement recovery agree along z");
    Check(indexedEmbeddedPlayer.MovementTrace().obstacleDepenetrations == 1
              && !embeddedCollisionGrid.IntersectsCircle(
                  indexedEmbeddedPlayer.Position().x, indexedEmbeddedPlayer.Position().z,
                  island::PlayerMotion::CapsuleRadius),
          "indexed placement recovery ends outside the obstacle and reports its correction");

    const island::CircularObstacle* selectedRockObstacle = nullptr;
    float rockApproachX = 0.0f;
    float rockApproachZ = 0.0f;
    float rockStartX = 0.0f;
    float rockStartZ = 0.0f;
    for (const island::RockPlacement& rock : props.Rocks()) {
        if (!rock.solid || rock.offshore
            || terrain.SignedShoreDistance(rock.x, rock.z) < 28.0f
            || terrain.Sample(rock.x, rock.z).slopeRadians > 0.30f) {
            continue;
        }
        const auto obstacle = std::ranges::find_if(
            props.Obstacles(), [&rock](const island::CircularObstacle& candidate) {
                return std::abs(candidate.x - rock.x) < 0.0001f
                    && std::abs(candidate.z - rock.z) < 0.0001f;
            });
        if (obstacle == props.Obstacles().end()) continue;
        const float radiusFromCentre = std::hypot(rock.x, rock.z);
        if (radiusFromCentre < 1.0f) continue;
        const float outwardX = rock.x / radiusFromCentre;
        const float outwardZ = rock.z / radiusFromCentre;
        const float startX = rock.x - outwardX * (obstacle->radius + 2.4f);
        const float startZ = rock.z - outwardZ * (obstacle->radius + 2.4f);
        const island::TerrainSample approachStart = terrain.Sample(startX, startZ);
        if (approachStart.height < 0.2f
            || approachStart.slopeRadians > island::PlayerMotion::MaximumWalkSlopeRadians
            || approachStart.signedShoreDistance < 18.0f) {
            continue;
        }
        selectedRockObstacle = &*obstacle;
        rockApproachX = outwardX;
        rockApproachZ = outwardZ;
        rockStartX = startX;
        rockStartZ = startZ;
        break;
    }
    Check(selectedRockObstacle != nullptr,
          "deterministic natural-prop layout supplies a walkable land-rock approach");
    if (selectedRockObstacle != nullptr) {
        island::PlayerMotion rockWalker;
        rockWalker.SetPosition(terrain, rockStartX, rockStartZ);
        int rockObstacleRejections = 0;
        int rockTerrainRejections = 0;
        for (int step = 0; step < 360; ++step) {
            rockWalker.Update(
                terrain, rockApproachX, rockApproachZ, 1.0f / 60.0f,
                std::span<const island::CircularObstacle>(selectedRockObstacle, 1U));
            rockObstacleRejections += rockWalker.MovementTrace().obstacleRejections;
            rockTerrainRejections += rockWalker.MovementTrace().terrainRejections;
        }
        const float finalClearance = std::hypot(
            rockWalker.Position().x - selectedRockObstacle->x,
            rockWalker.Position().z - selectedRockObstacle->z);
        const float requiredClearance = island::PlayerMotion::CapsuleRadius
            + selectedRockObstacle->radius;
        const float approachProgress =
            (rockWalker.Position().x - rockStartX) * rockApproachX
            + (rockWalker.Position().z - rockStartZ) * rockApproachZ;
        Check(approachProgress > 0.5f,
              "player advances along the deterministic land-rock approach");
        Check(finalClearance >= requiredClearance - 0.001f,
              "player capsule cannot tunnel through an authored land-rock proxy");
        Check(rockObstacleRejections > 0 && rockTerrainRejections == 0,
              "authored land-rock traversal is rejected by collision rather than terrain");
    }

    {
        // Boars whose range reaches the stream bank walk there to wallow and come back muddy;
        // the mud then dries off.
        const island::IslandTerrain terrain;
        island::BoarHerd herd(terrain);
        Check(herd.WallowerCount() >= 1U, "at least one boar range reaches a stream bank");
        float wettest = 0.0f;
        bool wallowed = false;
        // The shake on leaving the mud: it begins within a second of the wallow ending with
        // the boar standing still, the shake rising to full and its beat advancing, noted
        // where the boar stands; the mud field flings flecks that fall within the second.
        float shakePeak = 0.0f, shakeSpeed = 0.0f; bool shakeNear = true, beat = false;
        island::MudFleckField flecks;
        std::size_t flungAtOnce = 0; float highestFleck = -1.0f;
        std::uint32_t seenShakes = 0U;
        for (int step = 0; step < 12000; ++step) {
            herd.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
            flecks.Update(0.05f, herd.LastShake());
            flungAtOnce = std::max(flungAtOnce, flecks.ActiveCount());
            for (const auto& puff : flecks.Puffs()) if (puff.active) highestFleck = std::max(highestFleck, puff.y - herd.LastShake().y);
            if (herd.ShakeCount() != seenShakes) {
                seenShakes = herd.ShakeCount();
                bool found = false;
                for (const auto& boar : herd.Boars())
                    if (boar.mudAmount > 0.5f && std::hypot(boar.x - herd.LastShake().x, boar.z - herd.LastShake().z) < 0.3f) found = true;
                if (!found) shakeNear = false;
            }
            for (const auto& boar : herd.Boars()) {
                wettest = std::max(wettest, boar.mudAmount);
                wallowed |= boar.behavior == island::BoarBehavior::Wallow && boar.mudAmount > 0.5f;
                if (boar.shake > 0.5f) {
                    shakePeak = std::max(shakePeak, boar.shake);
                    if (boar.shake > 0.95f) shakeSpeed = std::max(shakeSpeed, boar.speed);
                    if (boar.shakePhase > 0.0f) beat = true;
                }
            }
        }
        Check(wallowed && wettest > 0.9f, "a boar wallows at the bank within ten minutes and comes out muddy");
        Check(herd.ShakeCount() >= 1U && shakePeak > 0.95f && shakeSpeed < 0.05f && beat && shakeNear,
              "a boar leaving its wallow stands and shakes where it stands");
        Check(flecks.ShakeCount() == herd.ShakeCount() && flungAtOnce >= island::MudFleckField::FlecksPerShake
                  && highestFleck > 0.3f && highestFleck < 1.2f,
              "the shake flings a ring of mud flecks from the flanks that fall within the second");
        // The snout: through those ten minutes a boar with its snout down works it to either
        // side past eight tenths, and a boar with its head up holds the snout still.
        {
            island::BoarHerd rooters(terrain);
            bool swayLeft = false, swayRight = false, headUpSway = false;
            for (int step = 0; step < 6000 && !(swayLeft && swayRight); ++step) {
                rooters.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
                for (const auto& boar : rooters.Boars()) {
                    if (boar.rootingPose > 0.9f && boar.snoutSway > 0.8f) swayRight = true;
                    if (boar.rootingPose > 0.9f && boar.snoutSway < -0.8f) swayLeft = true;
                    if (boar.rootingPose < 0.1f && std::abs(boar.snoutSway) > 0.01f) headUpSway = true;
                }
            }
            Check(swayLeft && swayRight && !headUpSway, "a rooting boar does not work its snout side to side, or works it with the head up");
        }
    }

    {
        // Gust fronts: bounded, periodic at a fixed point, and travelling downwind (toward -x).
        float peak = 0.0f, low = 1.0f;
        for (float t = 0.0f; t < 30.0f; t += 0.1f) {
            const float g = island::WindGust::Strength(12.0f, -40.0f, t);
            peak = std::max(peak, g); low = std::min(low, g);
        }
        Check(peak > 0.5f && low <= 0.0f, "a gust front passes a fixed point within half a minute and clears again");
        const float period = island::WindGust::FrontSpacing / island::WindGust::FrontSpeed;
        Check(std::abs(island::WindGust::Strength(12.0f, -40.0f, 3.0f)
                       - island::WindGust::Strength(12.0f, -40.0f, 3.0f + period)) < 0.3f,
              "gust fronts repeat with the front spacing over the front speed");
        // The front that peaks at the origin peaks 20 m downwind (smaller x) about three
        // seconds later: fronts are spatially periodic, so compare peak times, not first sightings.
        float peakHere = 0.0f, peakHereStrength = -1.0f;
        for (float t = 0.0f; t < period; t += 0.05f) {
            const float g = island::WindGust::Strength(0.0f, 0.0f, t);
            if (g > peakHereStrength) { peakHereStrength = g; peakHere = t; }
        }
        float peakDownwind = 0.0f, peakDownwindStrength = -1.0f;
        for (float t = peakHere; t < peakHere + period; t += 0.05f) {
            const float g = island::WindGust::Strength(-20.0f, 0.0f, t);
            if (g > peakDownwindStrength) { peakDownwindStrength = g; peakDownwind = t; }
        }
        const float expectedLag = 20.0f / island::WindGust::FrontSpeed;
        Check(std::abs((peakDownwind - peakHere) - expectedLag) < 1.0f,
              "gust fronts travel downwind at the front speed");
    }

    {
        // Seabirds leave their high orbit to circle low over the fish school they are given,
        // and climb back onto the route once it is withdrawn.
        const island::IslandTerrain terrain;
        island::BirdFlock flock(terrain);
        // A school point off the east shore at a typical offshore distance.
        const float shore = terrain.ShoreRadius(0.3f);
        const float forageX = std::cos(0.3f) * (shore + 25.0f), forageZ = std::sin(0.3f) * (shore + 25.0f);
        // The observer stands at the school so the hunting seabirds simulate at full rate.
        flock.SetSeaForage(forageX, forageZ, true);
        bool circledLow = false;
        for (int step = 0; step < 4800 && !circledLow; ++step) {
            flock.Update(0.05f, forageX, 2.0f, forageZ);
            for (const auto& bird : flock.Birds()) {
                if (bird.species != island::BirdSpecies::Seabird) continue;
                if (bird.huntAmount > 0.9f && bird.y < 14.0f && bird.y > 3.0f
                    && std::hypot(bird.x - forageX, bird.z - forageZ) < 22.0f) circledLow = true;
            }
        }
        Check(circledLow, "a seabird circles low over the fish school within four minutes");
        bool struck = false;
        for (int step = 0; step < 1200 && !struck; ++step) {
            flock.Update(0.05f, forageX, 2.0f, forageZ);
            const auto& splash = flock.DiveSplash();
            struck = splash.valid && splash.ageSeconds < 0.2f
                && std::hypot(splash.x - forageX, splash.z - forageZ) < 24.0f;
        }
        Check(struck && flock.DiveCount() >= 1U, "a circling seabird plunges and strikes the water near the school within a minute");
        // The strike brings the other seabirds within the joining distance in to the hunt.
        bool gathered = false;
        for (int step = 0; step < 200 && !gathered; ++step) {
            flock.Update(0.05f, forageX, 2.0f, forageZ);
            gathered = flock.HuntJoinCount() >= 1U && flock.HuntingCount() >= 2U;
        }
        Check(gathered, "a seabird's strike brings another in to hunt the same school within ten seconds");
        {
            // A dolphin's leap near a fresh school brings the seabirds in to hunt it at once,
            // ahead of their own times; a leap with no school in play draws none.
            island::BirdFlock drawn(terrain);
            drawn.SetSeaForage(forageX, forageZ, true);
            drawn.Update(0.05f, forageX, 2.0f, forageZ);
            Check(drawn.HuntingCount() == 0U, "the seabirds start on their routes");
            drawn.NoteDolphinLeap(forageX + 10.0f, forageZ);
            bool came = false;
            for (int step = 0; step < 40 && !came; ++step) {
                drawn.Update(0.05f, forageX, 2.0f, forageZ);
                came = drawn.DolphinJoinCount() >= 1U && drawn.HuntingCount() >= 1U;
            }
            Check(came, "a dolphin's leap near the school brings a seabird in to hunt within two seconds");
            island::BirdFlock idle(terrain);
            idle.NoteDolphinLeap(forageX + 10.0f, forageZ);
            for (int step = 0; step < 40; ++step) idle.Update(0.05f, forageX, 2.0f, forageZ);
            Check(idle.DolphinJoinCount() == 0U && idle.HuntingCount() == 0U, "a leap with no school in play draws no seabird");
        }
        // The hunt runs its course over deep water and the bird settles onto the sea: wings
        // folded (the perch pose), sitting at the surface, drifting downwind.
        bool rafted = false;
        float raftStartX = 0.0f, raftEndX = 0.0f;
        std::size_t rafter = 0U;
        for (int step = 0; step < 2400 && !rafted; ++step) {
            flock.Update(0.05f, forageX, 2.0f, forageZ);
            for (std::size_t i = 0; i < flock.Birds().size(); ++i) {
                const auto& bird = flock.Birds()[i];
                if (bird.species != island::BirdSpecies::Seabird) continue;
                if (bird.raftAmount > 0.9f && bird.y < 0.6f && bird.y > -0.1f && bird.perchAmount > 0.9f
                    && terrain.Height(bird.x, bird.z) < -0.5f) { rafted = true; raftStartX = bird.x; rafter = i; break; }
            }
        }
        Check(rafted && flock.RaftCount() >= 1U && flock.RaftingCount() >= 1U,
              "a seabird whose hunt has run its course settles onto the sea with folded wings within two minutes");
        for (int step = 0; step < 100; ++step)
            flock.Update(0.05f, forageX, 2.0f, forageZ);
        raftEndX = flock.Birds()[rafter].x;
        Check(rafted && raftEndX < raftStartX - 0.5f, "the rafting seabird drifts downwind on the water");
        // Lifting off the raft, the bird shakes the water from its feathers: the ruffle at
        // full within the seconds after the raft's end, and smoothed again as it climbs.
        {
            bool lifted = false; float shakePeak = 0.0f; int sinceLift = 0; bool smoothed = false;
            for (int step = 0; step < 800 && !(lifted && sinceLift > 80); ++step) {
                flock.Update(0.05f, forageX, 2.0f, forageZ);
                const auto& bird = flock.Birds()[rafter];
                if (!lifted && bird.raftAmount < 0.5f && flock.WaterShakeCount() >= 1U) lifted = true;
                if (lifted) { ++sinceLift; shakePeak = std::max(shakePeak, bird.ruffleAmount); if (shakePeak > 0.9f && bird.ruffleAmount < 0.1f) smoothed = true; }
            }
            Check(lifted && shakePeak > 0.9f && smoothed, "a seabird lifting off its raft does not shake the water from its feathers and smooth them again");
        }
        flock.SetSeaForage(0.0f, 0.0f, false);
        for (int step = 0; step < 1200; ++step)
            flock.Update(0.05f, forageX, 2.0f, forageZ);
        Check(flock.HuntingCount() == 0U, "seabirds return to their routes once the school is withdrawn");
        for (int step = 0; step < 1200; ++step)
            flock.Update(0.05f, forageX, 2.0f, forageZ);
        bool anyLow = false;
        for (const auto& bird : flock.Birds())
            if (bird.species == island::BirdSpecies::Seabird && (bird.raftAmount > 0.05f || bird.y < 3.0f)) anyLow = true;
        Check(flock.RaftingCount() == 0U && !anyLow, "rafting seabirds climb back to their routes within a minute of the raft's end");
    }

    {
        // Perched birds flush: the walker within 9 m of the perch, or a coconut's thud within
        // 30 m, sends the bird off at once; a distant walker and a far thud leave it sitting.
        const island::IslandTerrain terrain;
        const float perchX = 20.0f, perchZ = 20.0f;
        const std::vector<island::BirdPerchSite> sites{{perchX, terrain.Height(perchX, perchZ) + 2.0f, perchZ, 0.3f, island::BirdPerchKind::Wreck}};
        island::BirdFlock sitting(terrain);
        sitting.SetPerchSites(sites);
        Check(sitting.PerchedCount() == 0 && sitting.Birds()[0].behavior == island::BirdBehavior::Perched,
              "a bird is seeded onto the wreck perch");
        sitting.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(sitting.Birds()[0].behavior == island::BirdBehavior::Perched && sitting.FlushCount() == 0U,
              "a walker 25 m off leaves the perched bird sitting");
        sitting.NoteDisturbance(perchX + 45.0f, perchZ);
        sitting.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(sitting.Birds()[0].behavior == island::BirdBehavior::Perched && sitting.FlushCount() == 0U,
              "a thud 45 m off leaves the perched bird sitting");
        sitting.Update(0.05f, perchX + 6.0f, sites[0].y, perchZ);
        Check(sitting.Birds()[0].behavior == island::BirdBehavior::Depart && sitting.FlushCount() == 1U,
              "a walker 6 m off flushes the perched bird");
        Check(sitting.LastFlush().valid && sitting.AnyFlushCount() == 1U
                  && std::hypot(sitting.LastFlush().x - perchX, sitting.LastFlush().z - perchZ) < 0.5f,
              "the flush is noted at the perch for the wing clap");
        // The resident or an animal walking up under the perch flushes the bird as the walker
        // does; one passing twenty metres off leaves it sitting.
        island::BirdFlock underfoot(terrain);
        underfoot.SetPerchSites(sites);
        const std::array<island::GrassWalker, 1> farBody{{{perchX + 20.0f, perchZ, 0.6f, 1.0f}}};
        underfoot.SetWalkers(farBody);
        underfoot.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(underfoot.Birds()[0].behavior == island::BirdBehavior::Perched && underfoot.WalkerFlushCount() == 0U,
              "an animal twenty metres off leaves the perched bird sitting");
        const std::array<island::GrassWalker, 1> nearBody{{{perchX + 4.0f, perchZ, 0.6f, 1.0f}}};
        underfoot.SetWalkers(nearBody);
        underfoot.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(underfoot.Birds()[0].behavior == island::BirdBehavior::Depart && underfoot.WalkerFlushCount() == 1U && underfoot.FlushCount() == 1U,
              "an animal walking up four metres from the perch flushes the bird");
        // Between its looks around a perched bird preens: through its stay on the perch the
        // seeded bird turns its head back into a wing for a bout of a second or two, and
        // a flush drops the pose at once.
        island::BirdFlock preener(terrain);
        preener.SetPerchSites(sites);
        float longest = 0.0f, bout = 0.0f;
        for (int step = 0; step < 400; ++step) {
            preener.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
            const auto& bird = preener.Birds()[0];
            if (bird.behavior != island::BirdBehavior::Perched) break;
            if (bird.preenAmount > 0.5f) bout += 0.05f;
            else { longest = std::max(longest, bout); bout = 0.0f; }
        }
        longest = std::max(longest, bout);
        Check(preener.PreenCount() >= 1U && longest > 0.8f && longest < 4.0f,
              "a perched bird preens for a bout of a second or two during its stay");
        island::BirdFlock dropped(terrain);
        dropped.SetPerchSites(sites);
        for (int step = 0; step < 40; ++step) dropped.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(dropped.PreeningCount() == 1U && dropped.Birds()[0].preenAmount > 0.5f,
              "two seconds in, the seeded bird is mid-preen");
        dropped.SetWalkers(nearBody);
        dropped.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(dropped.Birds()[0].behavior == island::BirdBehavior::Depart && dropped.Birds()[0].preenAmount == 0.0f
                  && dropped.PreeningCount() == 0U,
              "a flush drops the preen at once");
        // Between its preens the perched bird stretches a wing: through a stay on the perch
        // the seeded bird opens one wing for a bout of a second or two, never while preening,
        // and a flush drops the stretch with the preen.
        island::BirdFlock stretcher(terrain);
        stretcher.SetPerchSites(sites);
        float longestStretch = 0.0f, stretchBout = 0.0f; bool together = false; float stretchPeak = 0.0f;
        for (int step = 0; step < 400; ++step) {
            stretcher.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
            const auto& bird = stretcher.Birds()[0];
            if (bird.behavior != island::BirdBehavior::Perched) break;
            stretchPeak = std::max(stretchPeak, bird.stretchAmount);
            if (bird.stretchAmount > 0.5f && bird.preenAmount > 0.5f) together = true;
            if (bird.stretchAmount > 0.5f) stretchBout += 0.05f;
            else { longestStretch = std::max(longestStretch, stretchBout); stretchBout = 0.0f; }
        }
        longestStretch = std::max(longestStretch, stretchBout);
        Check(stretcher.StretchCount() >= 1U && stretchPeak > 0.9f && longestStretch > 0.8f && longestStretch < 3.0f && !together,
              "a perched bird stretches one wing for a bout of a second or two between its preens");
        // The ruffle: as the seeded bird's first preen ends it shakes its feathers out, the
        // ruffle rising to full within a quarter second of the preen's end and smoothed again
        // within a second and a half; none before the preen ends.
        island::BirdFlock ruffler(terrain);
        ruffler.SetPerchSites(sites);
        bool preened = false, earlyRuffle = false; float rufflePeak = 0.0f; int ruffleFrames = 0, smoothAfter = -1;
        for (int step = 0; step < 400; ++step) {
            ruffler.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
            const auto& bird = ruffler.Birds()[0];
            if (bird.behavior != island::BirdBehavior::Perched) break;
            if (bird.preenAmount > 0.5f) preened = true;
            if (!preened && bird.ruffleAmount > 0.1f) earlyRuffle = true;
            rufflePeak = std::max(rufflePeak, bird.ruffleAmount);
            if (bird.ruffleAmount > 0.5f) ++ruffleFrames;
            if (rufflePeak > 0.9f && bird.ruffleAmount < 0.1f && smoothAfter < 0) smoothAfter = step;
        }
        Check(ruffler.RuffleCount() >= 1U && rufflePeak > 0.9f && ruffleFrames >= 8 && ruffleFrames <= 30 && smoothAfter > 0 && !earlyRuffle,
              "a perched bird shakes its feathers out as its preen ends and smooths them again");
        // The head scratch: through its stay the seeded bird brings a foot up to its head
        // for a bout of a second or so, never while preening or stretching.
        island::BirdFlock scratcher(terrain);
        scratcher.SetPerchSites(sites);
        float scratchPeak = 0.0f; bool scratchClash = false; int scratchFrames = 0, longestScratchBout = 0;
        for (int step = 0; step < 400; ++step) {
            scratcher.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
            const auto& bird = scratcher.Birds()[0];
            if (bird.behavior != island::BirdBehavior::Perched) break;
            scratchPeak = std::max(scratchPeak, bird.scratchAmount);
            if (bird.scratchAmount > 0.5f && (bird.preenAmount > 0.5f || bird.stretchAmount > 0.5f)) scratchClash = true;
            scratchFrames = bird.scratchAmount > 0.5f ? scratchFrames + 1 : 0; longestScratchBout = std::max(longestScratchBout, scratchFrames);
        }
        Check(scratcher.HeadScratchCount() >= 1U && scratchPeak > 0.9f && longestScratchBout >= 10 && longestScratchBout <= 40 && !scratchClash,
              "a perched bird does not scratch its head with a foot between its preens and stretches");
        float climbed = sitting.Birds()[0].y;
        for (int step = 0; step < 70; ++step) { sitting.Update(0.05f, perchX + 6.0f, sites[0].y, perchZ); climbed = std::max(climbed, sitting.Birds()[0].y); }
        Check(sitting.Birds()[0].behavior == island::BirdBehavior::Fly && climbed > sites[0].y + 2.0f,
              "the flushed bird climbs away and is flying within four seconds");
        island::BirdFlock startled(terrain);
        startled.SetPerchSites(sites);
        startled.NoteDisturbance(perchX + 12.0f, perchZ);
        startled.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        Check(startled.Birds()[0].behavior == island::BirdBehavior::Depart && startled.FlushCount() == 1U,
              "a thud 12 m off flushes the perched bird with no walker about");
        // A snake's head up at the perch flushes the bird; one on the ground five metres
        // below does not.
        island::BirdFlock coiled(terrain);
        coiled.SetPerchSites(sites);
        std::vector<island::SnakeState> snakes(1);
        for (auto& point : snakes[0].body) point = {perchX + 1.5f, sites[0].y - 5.0f, perchZ};
        coiled.SetSnakes(snakes);
        coiled.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(coiled.Birds()[0].behavior == island::BirdBehavior::Perched && coiled.SnakeFlushCount() == 0U,
              "a snake on the ground five metres below the perch leaves the bird sitting");
        for (auto& point : snakes[0].body) point = {perchX + 1.5f, sites[0].y, perchZ};
        coiled.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(coiled.Birds()[0].behavior == island::BirdBehavior::Depart && coiled.SnakeFlushCount() == 1U,
              "a snake's head up at the perch flushes the bird");
        // ...and mobs it: for the next eight seconds the bird stays a few metres over the
        // snake's head, chattering every couple of seconds, and only then takes its route.
        Check(coiled.MobCount() == 1U && coiled.MobbingCount() == 1U, "the snake-flushed bird begins mobbing the snake");
        float farthest = 0.0f;
        std::uint32_t chattersAtFour = 0U;
        for (int step = 0; step < 80; ++step) {
            coiled.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
            const auto& bird = coiled.Birds()[0];
            farthest = std::max(farthest, std::hypot(bird.x - (perchX + 1.5f), bird.z - perchZ));
            if (step == 79) chattersAtFour = coiled.LastMob().count;
        }
        Check(coiled.Birds()[0].behavior == island::BirdBehavior::Depart && coiled.MobbingCount() == 1U && farthest < 6.0f,
              "four seconds on the bird is still circling within six metres of the snake");
        Check(chattersAtFour >= 1U && chattersAtFour <= 3U && coiled.LastMob().valid
                  && std::hypot(coiled.LastMob().x - (perchX + 1.5f), coiled.LastMob().z - perchZ) < 0.1f,
              "the mobbing bird has chattered once or twice, at the snake");
        for (int step = 0; step < 120; ++step) coiled.Update(0.05f, perchX + 25.0f, sites[0].y, perchZ);
        Check(coiled.MobbingCount() == 0U && coiled.Birds()[0].behavior == island::BirdBehavior::Fly,
              "ten seconds on the mobbing is over and the bird is flying its route");
        Check(underfoot.MobCount() == 0U, "a walker's flush sends the bird straight off, no mobbing");
    }

    {
        // At a gust front a crown near the observer lets go a leaf: it starts in the crown,
        // descends steadily, drifts downwind and turns as it falls, lands on the floor and
        // lies there, and is gone after its rest.
        const island::IslandTerrain terrain;
        const island::JungleLayout jungle(terrain);
        island::CanopyLeafFallField leaves(terrain);
        leaves.SetCrowns(jungle.Canopy());
        Check(!jungle.Canopy().empty(), "the jungle has crowns to shed leaves");
        const auto& crown = jungle.Canopy().front();
        float seconds = 0.0f;
        bool released = false, startedHigh = false, fellAndDrifted = false, turned = false;
        float firstY = 0.0f, firstX = 0.0f, firstYaw = 0.0f;
        std::size_t slot = island::CanopyLeafFallField::Capacity;
        for (int step = 0; step < 4800 && !fellAndDrifted; ++step) {
            seconds += 0.05f;
            leaves.Update(seconds, 0.05f, crown.x, crown.z);
            if (!released && leaves.ReleaseCount() >= 1U) {
                released = true;
                for (std::size_t i = 0; i < leaves.Leaves().size(); ++i)
                    if (leaves.Leaves()[i].active) { slot = i; break; }
                const auto& leaf = leaves.Leaves()[slot];
                firstY = leaf.y; firstX = leaf.x; firstYaw = leaf.yawRadians;
                startedHigh = leaf.y > terrain.Height(leaf.x, leaf.z) + 2.5f;
            } else if (released) {
                const auto& leaf = leaves.Leaves()[slot];
                if (std::abs(leaf.yawRadians - firstYaw) > 0.3f) turned = true;
                if (leaf.active && leaf.y < firstY - 1.5f && leaf.x < firstX - 1.0f && leaves.FallingCount() >= 1U)
                    fellAndDrifted = true;
            }
        }
        Check(released && startedHigh, "a gust front has a crown near the observer let go a leaf from up in the crown within four minutes");
        Check(fellAndDrifted && turned, "the shed leaf descends, drifts downwind and turns as it falls");
        bool landed = false;
        for (int step = 0; step < 2400 && !landed; ++step) {
            seconds += 0.05f;
            leaves.Update(seconds, 0.05f, crown.x, crown.z);
            const auto& leaf = leaves.Leaves()[slot];
            landed = leaf.active && leaf.speed == 0.0f
                && std::abs(leaf.y - (std::max(terrain.Height(leaf.x, leaf.z), 0.0f) + 0.03f)) < 0.001f;
        }
        Check(landed, "the shed leaf lands on the floor and lies there");
        bool gone = false;
        for (int step = 0; step < 1000 && !gone; ++step) {
            seconds += 0.05f;
            leaves.Update(seconds, 0.05f, 1000.0f, 1000.0f);
            gone = !leaves.Leaves()[slot].active;
        }
        Check(gone, "a lying leaf is gone after its rest");
        {
            // How many shed leaves one view can hold (loop pass 221): the leaf renderer, sized
            // for the stream's 14 drifting leaves, also drew these and overflowed its buffer
            // (a fatal "data is not the correct size" crash) once more than 14 lay within its
            // 70 m draw radius. Ten minutes in the densest stand of crowns.
            std::size_t densest = 0, densestCount = 0;
            const auto crowns = jungle.Canopy();
            for (std::size_t i = 0; i < crowns.size(); ++i) {
                std::size_t near = 0;
                for (const auto& other : crowns)
                    if (std::hypot(other.x - crowns[i].x, other.z - crowns[i].z) < island::CanopyLeafFallField::Range) ++near;
                if (near > densestCount) { densestCount = near; densest = i; }
            }
            island::CanopyLeafFallField stand(terrain);
            stand.SetCrowns(crowns);
            std::size_t peak = 0;
            float clock = 0.0f;
            for (int step = 0; step < 12000; ++step) {
                clock += 0.05f;
                stand.Update(clock, 0.05f, crowns[densest].x, crowns[densest].z);
                std::size_t inView = 0;
                for (const auto& leaf : stand.Leaves())
                    if (leaf.active && std::hypot(leaf.x - crowns[densest].x, leaf.z - crowns[densest].z) < 70.0f) ++inView;
                peak = std::max(peak, inView);
            }
            Check(peak <= island::CanopyLeafFallField::Capacity, "the shed leaves never exceed the field's capacity");
            // In a storm's wind (2) the same fronts tear far more leaves loose and carry them
            // faster downwind.
            island::CanopyLeafFallField calm(terrain), storm(terrain);
            calm.SetCrowns(crowns); storm.SetCrowns(crowns); storm.SetWind(2.0f);
            float stormClock = 0.0f, calmFurthest = 0.0f, stormFurthest = 0.0f;
            for (int step = 0; step < 6000; ++step) {
                stormClock += 0.05f;
                calm.Update(stormClock, 0.05f, crowns[densest].x, crowns[densest].z);
                storm.Update(stormClock, 0.05f, crowns[densest].x, crowns[densest].z);
                for (const auto& leaf : calm.Leaves()) if (leaf.active) calmFurthest = std::max(calmFurthest, crowns[densest].x - leaf.x);
                for (const auto& leaf : storm.Leaves()) if (leaf.active) stormFurthest = std::max(stormFurthest, crowns[densest].x - leaf.x);
            }
            Check(storm.ReleaseCount() > calm.ReleaseCount() * 2U, "a storm's wind tears more than twice as many leaves loose");
            Check(stormFurthest > calmFurthest + 10.0f, "a storm's wind carries the leaves further downwind");
            std::cout << "canopy leaf fall in five minutes: " << calm.ReleaseCount() << " leaves in the trade wind, "
                      << storm.ReleaseCount() << " in a storm's; carried up to " << calmFurthest << " m and "
                      << stormFurthest << " m downwind\n";
            std::cout << "canopy leaf fall: up to " << peak << " leaves within 70 m of one view in ten minutes among "
                      << densestCount << " crowns (the stream field holds " << island::StreamDriftField::Capacity << ")\n";
        }
        // A shake at a point up in a crown lets a leaf go just below it; one at ground level
        // lets nothing go.
        island::CanopyLeafFallField shaken(terrain);
        const float shakeX = crown.x, shakeZ = crown.z, shakeY = terrain.Height(shakeX, shakeZ) + 7.0f;
        shaken.Shake(shakeX, shakeY, shakeZ, 5U);
        bool shakenLeaf = false;
        for (const auto& leaf : shaken.Leaves())
            if (leaf.active && std::hypot(leaf.x - shakeX, leaf.z - shakeZ) < 0.6f && std::abs(leaf.y - (shakeY - 0.3f)) < 0.001f) shakenLeaf = true;
        Check(shakenLeaf && shaken.ShakeCount() == 1U && shaken.ActiveCount() == 1U, "a shake up in a crown lets one leaf go just below the point");
        shaken.Shake(shakeX, terrain.Height(shakeX, shakeZ) + 0.5f, shakeZ, 6U);
        Check(shaken.ActiveCount() == 1U, "a shake at ground level lets no leaf go");
        // A feather from a flushing bird: pale, and it comes down at about half a leaf's pace.
        island::CanopyLeafFallField feathered(terrain);
        feathered.Shake(shakeX, shakeY, shakeZ, 5U);
        feathered.Feather(shakeX, shakeY, shakeZ, 9U);
        Check(feathered.FeatherCount() == 1U && feathered.ActiveCount() == 2U, "a flushing bird lets one feather go beside the leaf");
        float leafStart = 0.0f, featherStart = 0.0f;
        std::size_t leafSlot = 99, featherSlot = 99;
        for (std::size_t i = 0; i < feathered.Leaves().size(); ++i) {
            const auto& item = feathered.Leaves()[i];
            if (!item.active) continue;
            if (item.variant == island::CanopyLeafFallField::FeatherVariant) { featherSlot = i; featherStart = item.y; }
            else { leafSlot = i; leafStart = item.y; }
        }
        for (int step = 0; step < 40; ++step) feathered.Update(0.0f, 0.05f, 1000.0f, 1000.0f);
        const float leafDrop = leafStart - feathered.Leaves()[leafSlot].y;
        const float featherDrop = featherStart - feathered.Leaves()[featherSlot].y;
        Check(featherSlot < 99 && leafSlot < 99 && featherDrop > 0.4f && featherDrop < 0.7f && leafDrop > 1.0f,
              "the feather comes down at about half the leaf's pace");
        feathered.Feather(shakeX, terrain.Height(shakeX, shakeZ) + 0.1f, shakeZ, 10U);
        Check(feathered.FeatherCount() == 1U, "a bird flushing off the floor lets no feather go");
    }

    {
        // The cloud shadows and the skinks: a basking skink that a cloud's shadow covers gets
        // up and forages; a skink lies down to bask only where the ground is in the sun.
        const island::IslandTerrain terrain;
        island::SkinkPopulation skinks(terrain);
        const island::SkinkState start = skinks.Skinks()[1];   // index 1 starts basking
        Check(start.behavior == island::SkinkBehavior::Bask, "the second skink starts basking");
        float shadeClock = -1.0f, sunClock = -1.0f;
        for (float t = 0.0f; t < 3600.0f && (shadeClock < 0.0f || sunClock < 0.0f); t += 0.5f) {
            const float cover = island::CloudShadow::Cover(start.x, start.z, t);
            if (shadeClock < 0.0f && cover > 0.9f) shadeClock = t;
            if (sunClock < 0.0f && cover < 0.02f) sunClock = t;
        }
        Check(shadeClock >= 0.0f && sunClock >= 0.0f, "the cloud field both covers and clears the skink's spot within an hour");
        skinks.SetCloudClock(shadeClock);
        skinks.Update(0.05f, start.x + 12.0f, start.y + 1.0f, start.z);
        Check(skinks.Skinks()[1].behavior == island::SkinkBehavior::Forage && skinks.ShadeRouseCount() >= 1U,
              "a cloud's shadow over a basking skink has it up and foraging at once");
        bool baskedInShade = false;
        for (int step = 0; step < 600; ++step) {
            skinks.Update(0.05f, start.x + 12.0f, start.y + 1.0f, start.z);
            const auto& skink = skinks.Skinks()[1];
            if (skink.behavior == island::SkinkBehavior::Bask
                && island::CloudShadow::Cover(skink.x, skink.z, shadeClock) > island::SkinkPopulation::SunCover) baskedInShade = true;
        }
        Check(!baskedInShade, "under the cloud the skink keeps foraging and never lies down in the shade");
        skinks.SetCloudClock(sunClock);
        bool baskedInSun = false;
        for (int step = 0; step < 600 && !baskedInSun; ++step) {
            skinks.Update(0.05f, start.x + 12.0f, start.y + 1.0f, start.z);
            baskedInSun = skinks.Skinks()[1].behavior == island::SkinkBehavior::Bask && skinks.SunBaskingCount() >= 1U;
        }
        Check(baskedInSun, "with the sun back on its ground the skink lies down to bask within half a minute");
    }

    {
        // A fly hanging low within reach of a resting gecko: the gecko stalks it, lunges from
        // the last span and rests; the flies dart from the snap.
        const island::IslandTerrain terrain;
        island::GeckoPopulation geckos(terrain);
        const island::GeckoHome& home = geckos.Homes()[0];
        std::vector<island::FlyState> flies(1);
        flies[0].x = home.x + 1.6f; flies[0].z = home.z;
        flies[0].y = terrain.Height(flies[0].x, flies[0].z) + 0.3f;
        geckos.SetFlies(flies);
        bool stalked = false, lunged = false, wagged = false, restingWag = false;
        float lungeDistance = 100.0f;
        for (int step = 0; step < 1200 && !lunged; ++step) {
            geckos.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
            const auto& gecko = geckos.Geckos()[0];
            if (gecko.behavior == island::GeckoBehavior::Stalk && gecko.speed > 0.2f && gecko.speed < 0.6f) stalked = true;
            if (gecko.behavior == island::GeckoBehavior::Stalk && gecko.tailWag > 0.9f && geckos.WaggingCount() >= 1U) wagged = true;
            if (gecko.behavior == island::GeckoBehavior::Rest && gecko.tailWag > 0.5f && step < 5) restingWag = true;
            if (geckos.LungeCount() >= 1U) { lunged = true; lungeDistance = std::hypot(gecko.x - flies[0].x, gecko.z - flies[0].z); }
        }
        Check(stalked && lunged && lungeDistance < island::GeckoPopulation::LungeReach + 0.1f
                  && geckos.LastLunge().valid && std::abs(geckos.LastLunge().x - flies[0].x) < 0.01f,
              "a resting gecko stalks a low fly slowly and lunges from the last span within a minute");
        bool rested = false;
        for (int step = 0; step < 40 && !rested; ++step) {
            geckos.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
            rested = geckos.Geckos()[0].behavior == island::GeckoBehavior::Rest;
        }
        Check(rested, "after the lunge the gecko rests");
        // The lip lick: the hunt over, the gecko has licked its lips, the lick drawn in
        // again within a second and a half of the rest's start.
        Check(geckos.LipLickCount() >= 1U, "the gecko does not lick its lips as the hunt ends");
        {
            float lipPeakSeen = geckos.Geckos()[0].lipLick;
            for (int step = 0; step < 30; ++step) { geckos.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z); lipPeakSeen = std::max(lipPeakSeen, geckos.Geckos()[0].lipLick); }
            Check(lipPeakSeen > 0.5f && geckos.Geckos()[0].lipLick < 0.1f, "the lip lick is not drawn in again");
        }
        Check(wagged && !restingWag, "a stalking gecko wags its tail and a resting one holds it still");
        for (int step = 0; step < 20; ++step) geckos.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
        Check(geckos.Geckos()[0].tailWag < 0.2f, "the wag stills within a second of the lunge");
        for (int step = 0; step < 200; ++step) geckos.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
        Check(geckos.LungeCount() == 1U, "a gecko does not hunt again within the cooldown");
        // The eye lick: resting on through the cooldown, within half a minute the gecko licks
        // an eye at least twice, each lick to full and gone again, the eyes taking turns.
        const std::uint32_t licksBefore = geckos.LickCount();
        float lickPeak = 0.0f; bool lickSettled = false, leftEye = false, rightEye = false;
        for (int step = 0; step < 600; ++step) {
            geckos.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
            const auto& gecko = geckos.Geckos()[0];
            lickPeak = std::max(lickPeak, gecko.eyeLick);
            if (gecko.eyeLick > 0.9f) { if (gecko.lickSide < 0.0f) leftEye = true; else rightEye = true; }
            if (lickPeak > 0.9f && gecko.eyeLick < 0.05f) lickSettled = true;
        }
        Check(geckos.LickCount() >= licksBefore + 2U && lickPeak > 0.9f && lickSettled && leftEye && rightEye,
              "a resting gecko licks its eyes clean in turn every ten seconds or so");
        island::FlySwarms swarm(terrain);
        const auto& fly = swarm.Flies()[0];
        swarm.NoteLunge(fly.x, fly.z);
        swarm.Update(0.05f, fly.x + 8.0f, fly.y, fly.z);
        Check(swarm.LungeDartCount() >= 1U && swarm.Flies()[0].behavior == island::FlyBehavior::Dart,
              "flies dart from a gecko's snap among them");
    }

    {
        // Fermenting fruit on a fly swarm's ground: the swarm shifts over it and hangs low
        // there; with the fruit gone it drifts back to its own ground and height. Fresh fruit
        // does not draw it.
        const island::IslandTerrain terrain;
        island::FlySwarms flies(terrain);
        const island::FlyHome base = flies.Homes()[0];
        island::FallingFruitField field(terrain);
        field.DropFrom(island::CanopyPlacement{base.x + 3.0f, terrain.Height(base.x + 3.0f, base.z), base.z, 0.4f, 1.0f, 2U});
        const std::vector<island::MonkeyState> nobody;
        for (int step = 0; step < 300; ++step) field.Update(0.05f, nobody);
        std::size_t lying = field.Fruit().size();
        for (std::size_t i = 0; i < field.Fruit().size(); ++i)
            if (field.Fruit()[i].active && field.Fruit()[i].restSeconds >= 0.0f && !field.Fruit()[i].rolling) lying = i;
        Check(lying < field.Fruit().size() && std::hypot(field.Fruit()[lying].x - base.x, field.Fruit()[lying].z - base.z) < island::FlySwarms::FruitRange,
              "a fruit lies within the swarm's range");
        flies.SetLyingFruit(field.Fruit());
        for (int step = 0; step < 200; ++step) flies.Update(0.05f, base.x + 8.0f, terrain.Height(base.x, base.z) + 1.0f, base.z);
        Check(flies.FruitVisitCount() == 0U && flies.AtFruitCount() == 0U, "fresh fruit draws no flies");
        for (int step = 0; step < 500; ++step) field.Update(0.05f, nobody);
        Check(field.Fruit()[lying].active && field.Fruit()[lying].restSeconds >= island::FlySwarms::FermentSeconds, "the fruit has lain long enough to ferment");
        const float fruitX = field.Fruit()[lying].x, fruitZ = field.Fruit()[lying].z;
        bool over = false;
        for (int step = 0; step < 800 && !over; ++step) {
            flies.Update(0.05f, base.x + 8.0f, terrain.Height(base.x, base.z) + 1.0f, base.z);
            std::size_t near = 0;
            for (std::size_t i = 0; i < island::FlySwarms::PerSwarm; ++i) {
                const auto& fly = flies.Flies()[i];
                if (std::hypot(fly.x - fruitX, fly.z - fruitZ) < 2.0f && fly.y < terrain.Height(fly.x, fly.z) + 0.45f) ++near;
            }
            over = near >= island::FlySwarms::PerSwarm - 1U && flies.AtFruitCount() >= island::FlySwarms::PerSwarm - 1U;
        }
        Check(over && flies.FruitVisitCount() >= 1U, "the swarm shifts over the fermenting fruit and hangs low there within forty seconds");
        {
            // The landing: within twenty seconds at the fruit a fly sets down on it, landed at
            // full with its wings still and its forelegs rubbing, and lifts off again.
            bool landed = false, lifted = false; float wingBefore = -1.0f;
            for (int step = 0; step < 400; ++step) {
                flies.Update(0.05f, base.x + 8.0f, terrain.Height(base.x, base.z) + 1.0f, base.z);
                for (std::size_t i = 0; i < island::FlySwarms::PerSwarm; ++i) {
                    const auto& fly = flies.Flies()[i];
                    if (fly.landed > 0.9f && fly.y < terrain.Height(fly.x, fly.z) + 0.15f && fly.rubPhase > 0.5f) {
                        if (wingBefore >= 0.0f && std::abs(fly.wingPhase - wingBefore) < 0.01f) landed = true;
                        wingBefore = fly.wingPhase;
                    }
                    if (landed && fly.landed < 0.1f && fly.y > terrain.Height(fly.x, fly.z) + 0.2f) lifted = true;
                }
            }
            Check(flies.LandingCount() >= 1U && landed && lifted, "a fly at the fruit sets down on it, wings still, and lifts off again");
        }
        flies.SetLyingFruit({});
        bool home = false;
        for (int step = 0; step < 800 && !home; ++step) {
            flies.Update(0.05f, base.x + 8.0f, terrain.Height(base.x, base.z) + 1.0f, base.z);
            std::size_t back = 0;
            for (std::size_t i = 0; i < island::FlySwarms::PerSwarm; ++i) {
                const auto& fly = flies.Flies()[i];
                if (std::hypot(fly.x - base.x, fly.z - base.z) < 2.0f && fly.y > terrain.Height(fly.x, fly.z) + 0.4f) ++back;
            }
            home = back >= island::FlySwarms::PerSwarm - 1U && flies.AtFruitCount() == 0U;
        }
        Check(home, "with the fruit gone the swarm drifts back to its own ground and height");
    }

    {
        // Fruit lying inside a rodent's range: the rodent's next forage goes to it, it feeds
        // there (still, nibbling) for a while, then rests; fruit beyond its range it ignores.
        const island::IslandTerrain terrain;
        island::RodentPopulation rodents(terrain);
        island::FallingFruitField field(terrain);
        std::size_t subject = rodents.Rodents().size();
        for (std::size_t i = 0; i < rodents.Rodents().size() && subject == rodents.Rodents().size(); ++i) {
            const auto& home = rodents.Homes()[i];
            // A crown at the rodent's home drops fruit within a few metres of it.
            field.DropFrom(island::CanopyPlacement{home.x, terrain.Height(home.x, home.z), home.z, 0.4f, 1.0f, 2U});
            field.DropFrom(island::CanopyPlacement{home.x, terrain.Height(home.x, home.z), home.z, 1.9f, 1.0f, 4U});
            const std::vector<island::MonkeyState> nobody;
            for (int step = 0; step < 400; ++step) field.Update(0.05f, nobody);
            for (const auto& fruit : field.Fruit())
                if (fruit.active && fruit.restSeconds >= 0.0f && !fruit.rolling && rodents.IsValidPosition(i, fruit.x, fruit.z)
                    && std::hypot(fruit.x - home.x, fruit.z - home.z) < home.radius * 0.9f) subject = i;
        }
        Check(subject < rodents.Rodents().size(), "a rodent has lying fruit on ground it accepts inside its range");
        rodents.SetLyingFruit(field.Fruit());
        const auto& home = rodents.Homes()[subject];
        bool fed = false, still = false;
        for (int step = 0; step < 2400 && !fed; ++step) {
            rodents.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
            const auto& rodent = rodents.Rodents()[subject];
            for (const auto& fruit : field.Fruit())
                if (fruit.active && std::hypot(fruit.x - rodent.x, fruit.z - rodent.z) < island::RodentPopulation::FruitReach + 0.05f
                    && rodents.FeedingCount() >= 1U) fed = true;
        }
        Check(fed && rodents.FruitVisitCount() >= 1U, "a rodent goes to fruit lying in its range and feeds there within two minutes");
        {
            // Grooming: a rodent watched from twelve metres sits up to groom on a rest, the
            // paws' strokes advancing, and is down again by the next forage; one hidden
            // never grooms.
            island::RodentPopulation groomers(terrain);
            const auto& groomHome = groomers.Homes()[0];
            float peak = 0.0f, strokes = 0.0f; bool settled = false, hiddenGroom = false;
            for (int step = 0; step < 600; ++step) {
                groomers.Update(0.05f, groomHome.x + 12.0f, terrain.Height(groomHome.x, groomHome.z) + 1.0f, groomHome.z);
                const auto& rodent = groomers.Rodents()[0];
                peak = std::max(peak, rodent.groom);
                if (rodent.groom > 0.9f) strokes = std::max(strokes, rodent.groomPhase);
                if (peak > 0.9f && rodent.groom < 0.05f) settled = true;
                for (const auto& other : groomers.Rodents()) if (other.visibility < 0.3f && other.groom > 0.1f) hiddenGroom = true;
            }
            Check(groomers.GroomCount() >= 1U && peak > 0.9f && strokes > 1.0f && settled && !hiddenGroom,
                  "a resting rodent sits up to groom its face and settles again");
            // The whiskers: through those thirty seconds a foraging rodent's whiskers come
            // fully into play, a resting one's settle to a third, and a hidden one's lie still.
            float foragingWhisk = 0.0f, restingWhisk = 1.0f, hiddenWhisk = 0.0f;
            for (int step = 0; step < 600; ++step) {
                groomers.Update(0.05f, groomHome.x + 12.0f, terrain.Height(groomHome.x, groomHome.z) + 1.0f, groomHome.z);
                for (const auto& rodent : groomers.Rodents()) {
                    if (rodent.simulationTier != island::WildlifeSimulationTier::Full) continue;
                    if (rodent.behavior == island::RodentBehavior::Forage) foragingWhisk = std::max(foragingWhisk, rodent.whiskerTwitch);
                    if (rodent.behavior == island::RodentBehavior::Rest && rodent.groom < 0.05f) restingWhisk = std::min(restingWhisk, rodent.whiskerTwitch);
                    if (rodent.visibility < 0.2f) hiddenWhisk = std::max(hiddenWhisk, rodent.whiskerTwitch);
                }
            }
            Check(foragingWhisk > 0.9f && restingWhisk <= 0.35f && hiddenWhisk < 0.3f,
                  "a rodent's whiskers are not fully at work on the forage, eased at rest and still in hiding");
        }
        for (int step = 0; step < 20; ++step) {
            rodents.Update(0.05f, home.x + 12.0f, terrain.Height(home.x, home.z) + 1.0f, home.z);
            if (rodents.FeedingCount() >= 1U && rodents.Rodents()[subject].speed < 0.05f) still = true;
        }
        Check(still, "the feeding rodent comes to a stand at the fruit within a second");
        island::RodentPopulation unfed(terrain);
        const auto& farHome = unfed.Homes()[subject];
        island::FallingFruitField farField(terrain);
        farField.DropFrom(island::CanopyPlacement{farHome.x + 40.0f, terrain.Height(farHome.x + 40.0f, farHome.z), farHome.z, 0.4f, 1.0f, 2U});
        const std::vector<island::MonkeyState> nobody;
        for (int step = 0; step < 400; ++step) farField.Update(0.05f, nobody);
        unfed.SetLyingFruit(farField.Fruit());
        for (int step = 0; step < 1200; ++step) unfed.Update(0.05f, farHome.x + 12.0f, terrain.Height(farHome.x, farHome.z) + 1.0f, farHome.z);
        Check(unfed.FruitVisitCount() == 0U, "fruit lying forty metres off draws no rodent");
    }

    {
        // A fruit lets go from any crown handed to the fruit field, as from a picking
        // monkey's: it starts up in the crown and comes down to lie on the ground.
        const island::IslandTerrain terrain;
        island::FallingFruitField field(terrain);
        const island::CanopyPlacement tree{40.0f, terrain.Height(40.0f, 40.0f), 40.0f, 0.4f, 1.0f, 2U};
        field.DropFrom(tree);
        Check(field.ActiveCount() == 1U && field.DropCount() == 1U, "a crown handed to the fruit field lets one fruit go");
        const auto& fruit = field.Fruit()[0];
        Check(fruit.active && fruit.y > tree.y + 1.0f && std::hypot(fruit.x - tree.x, fruit.z - tree.z) < 4.0f * tree.scale,
              "the let-go fruit starts up in that crown");
        const std::vector<island::MonkeyState> nobody;
        bool lying = false;
        for (int step = 0; step < 400 && !lying; ++step) {
            field.Update(0.05f, nobody);
            lying = fruit.active && fruit.restSeconds >= 0.0f && !fruit.rolling;
        }
        Check(lying, "the let-go fruit comes down and lies on the ground within twenty seconds");
        island::FallingFruitField refused(terrain);
        refused.DropFrom(island::CanopyPlacement{std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f, 0.0f, 1.0f, 0U});
        Check(refused.ActiveCount() == 0U, "a crown with no finite position lets no fruit go");
    }

    {
        // The gape: the present shark, watched from beside it, drops its jaw within ten
        // seconds for under a second and a half and closes it again; a second gape follows
        // within another ten; a shark gone under holds its jaw shut.
        const island::IslandTerrain terrain;
        island::SharkPatrol gaper(terrain);
        Check(gaper.State().present, "the shark starts present for the gape check");
        float peak = 0.0f; int openFrames = 0, longest = 0; bool closed = false;
        for (int step = 0; step < 400; ++step) {
            const auto& s = gaper.State();
            gaper.Update(0.05f, s.x + 6.0f, s.y, s.z);
            peak = std::max(peak, s.jawOpen);
            openFrames = s.jawOpen > 0.5f ? openFrames + 1 : 0;
            longest = std::max(longest, openFrames);
            if (peak > 0.9f && s.jawOpen < 0.05f) closed = true;
        }
        Check(gaper.GapeCount() >= 2U && peak > 0.9f && longest <= 30 && closed, "the shark on patrol gapes for a moment every several seconds");
        island::SharkPatrol hidden(terrain);
        bool gone = false; float hiddenJaw = 0.0f;
        for (int step = 0; step < 12000 && !(gone && hidden.State().activity < 0.1f); ++step) {
            const auto& s = hidden.State();
            hidden.Update(0.05f, s.x + 6.0f, s.y, s.z);
            if (!s.present) { gone = true; if (s.activity < 0.5f) hiddenJaw = std::max(hiddenJaw, s.jawOpen); }
        }
        Check(gone && hiddenJaw < 0.3f, "a shark gone under keeps its jaw shut");
    }
    {
        // A fish's breach near the present shark draws it that way for a while; one far off
        // does not, and the patrol resumes once the interest has run out.
        const island::IslandTerrain terrain;
        island::SharkPatrol shark(terrain);
        const auto& state = shark.State();
        Check(state.present, "the shark starts present for the breach check");
        const float farX = state.x + 150.0f, farZ = state.z;
        shark.NoteBreach(farX, farZ);
        Check(shark.InvestigationCount() == 0U && !shark.Investigating(), "a breach a hundred and fifty metres off draws no shark");
        // A breach off the shark's bow, thirty metres on.
        const float breachX = state.x + std::cos(state.headingRadians) * 30.0f;
        const float breachZ = state.z + std::sin(state.headingRadians) * 30.0f;
        const float observerX = state.x + 20.0f, observerZ = state.z;
        shark.NoteBreach(breachX, breachZ);
        Check(shark.InvestigationCount() == 1U && shark.Investigating(), "a breach thirty metres off draws the shark");
        const float first = std::hypot(breachX - state.x, breachZ - state.z);
        float nearest = first;
        for (int step = 0; step < 200; ++step) {
            shark.Update(0.05f, observerX, 0.0f, observerZ);
            nearest = std::min(nearest, std::hypot(breachX - state.x, breachZ - state.z));
        }
        Check(nearest < first - 8.0f, "the shark closes eight metres and more on the breach within ten seconds");
        for (int step = 0; step < 300; ++step) shark.Update(0.05f, observerX, 0.0f, observerZ);
        Check(!shark.Investigating(), "the interest runs out within twenty-five seconds and the patrol goes on");
    }

    {
        // The shark near the turtles: a surfacing turtle dives at once and the turtles draw
        // away from it; the shark far off changes nothing.
        const island::IslandTerrain terrain;
        island::SeaTurtleGroup turtles(terrain);
        std::size_t surfacing = turtles.Turtles().size();
        for (std::size_t i = 0; i < turtles.Turtles().size(); ++i)
            if (turtles.Turtles()[i].behavior == island::SeaTurtleBehavior::Surface) { surfacing = i; break; }
        Check(surfacing < turtles.Turtles().size(), "a turtle starts surfacing");
        const auto up = turtles.Turtles()[surfacing];
        turtles.SetThreat(up.x + 50.0f, up.z, true);
        for (int step = 0; step < 20; ++step) turtles.Update(0.05f, up.x + 20.0f, 0.0f, up.z);
        Check(turtles.Turtles()[surfacing].behavior == island::SeaTurtleBehavior::Surface && turtles.ThreatDiveCount() == 0U,
              "the shark fifty metres off leaves the surfacing turtle up");
        const float sharkX = up.x + 6.0f, sharkZ = up.z;
        turtles.SetThreat(sharkX, sharkZ, true);
        turtles.Update(0.05f, up.x + 20.0f, 0.0f, up.z);
        Check(turtles.Turtles()[surfacing].behavior == island::SeaTurtleBehavior::Cruise && turtles.ThreatDiveCount() == 1U,
              "the shark six metres off sends the surfacing turtle under at once");
        const float first = std::hypot(turtles.Turtles()[surfacing].x - sharkX, turtles.Turtles()[surfacing].z - sharkZ);
        float farthest = first;
        for (int step = 0; step < 300; ++step) {
            turtles.Update(0.05f, up.x + 20.0f, 0.0f, up.z);
            farthest = std::max(farthest, std::hypot(turtles.Turtles()[surfacing].x - sharkX, turtles.Turtles()[surfacing].z - sharkZ));
        }
        Check(farthest > first + 2.0f, "the turtle draws two metres and more away from the shark within fifteen seconds");
    }

    {
        // The shark near a dolphin: it leaves its route and keeps its berth, drawing away
        // from the shark; the shark far off leaves the pod on its route.
        const island::IslandTerrain terrain;
        island::DolphinPod pod(terrain);
        const auto start = pod.Dolphins()[0];
        pod.SetThreat(start.x + 60.0f, start.z, true);
        for (int step = 0; step < 100; ++step) pod.Update(0.05f, start.x + 30.0f, 0.0f, start.z);
        Check(pod.BerthCount() == 0U && pod.BerthingCount() == 0U, "the shark sixty metres off leaves the pod on its route");
        const auto before = pod.Dolphins()[0];
        const float sharkX = before.x + 8.0f, sharkZ = before.z;
        pod.SetThreat(sharkX, sharkZ, true);
        float farthest = std::hypot(before.x - sharkX, before.z - sharkZ);
        const float first = farthest;
        for (int step = 0; step < 160; ++step) {
            pod.Update(0.05f, start.x + 30.0f, 0.0f, start.z);
            const auto& dolphin = pod.Dolphins()[0];
            farthest = std::max(farthest, std::hypot(dolphin.x - sharkX, dolphin.z - sharkZ));
        }
        Check(pod.BerthCount() >= 1U && farthest > first + 4.0f,
              "a dolphin eight metres from the shark draws four metres and more away within eight seconds");
        pod.SetThreat(0.0f, 0.0f, false);
        pod.Update(0.05f, start.x + 30.0f, 0.0f, start.z);
        Check(pod.BerthingCount() == 0U, "with the shark gone no dolphin keeps a berth");
    }

    {
        // The shark through a school: the fish part around it and hurry away; with the
        // threat withdrawn the parting ends.
        const island::IslandTerrain terrain;
        island::FishSchools schools(terrain);
        const island::FishSchoolState centre = schools.Schools()[0];
        const auto meanDistance = [&](float x, float z) {
            float sum = 0.0f; int count = 0;
            for (const auto& fish : schools.Fish())
                if (fish.schoolIndex == 0U) { sum += std::hypot(fish.x - x, fish.z - z); ++count; }
            return count > 0 ? sum / static_cast<float>(count) : 0.0f;
        };
        for (int step = 0; step < 40; ++step) schools.Update(0.05f, centre.centreX, 0.0f, centre.centreZ + 30.0f);
        const island::FishSchoolState now = schools.Schools()[0];
        const float before = meanDistance(now.centreX, now.centreZ);
        schools.SetThreat(now.centreX, now.centreY, now.centreZ, true);
        schools.Update(0.05f, centre.centreX, 0.0f, centre.centreZ + 30.0f);
        Check(schools.ThreatPartingCount() >= island::FishSchools::FishPerSchool / 2U, "the shark in a school has most of its fish parting");
        // The parting fish roll hard through their turns: within half a second a flank
        // flashes; every flash stays within 0..1.
        float brightest = 0.0f;
        bool bounded = true;
        for (int step = 0; step < 10; ++step) {
            schools.Update(0.05f, centre.centreX, 0.0f, centre.centreZ + 30.0f);
            for (const auto& fish : schools.Fish()) {
                if (fish.schoolIndex == 0U) brightest = std::max(brightest, fish.flash);
                if (fish.flash < 0.0f || fish.flash > 1.0f || !std::isfinite(fish.flash)) bounded = false;
            }
        }
        Check(brightest > 0.5f && bounded, "a fish parting from the shark flashes silver as it rolls through the turn");
        float farthest = before;
        for (int step = 0; step < 60; ++step) {
            schools.Update(0.05f, centre.centreX, 0.0f, centre.centreZ + 30.0f);
            farthest = std::max(farthest, meanDistance(now.centreX, now.centreZ));
        }
        Check(farthest > before + 1.0f, "the fish hurry a metre and more out of the shark's way within three seconds");
        schools.SetThreat(0.0f, 0.0f, 0.0f, false);
        schools.Update(0.05f, centre.centreX, 0.0f, centre.centreZ + 30.0f);
        Check(schools.ThreatPartingCount() == 0U, "with the shark gone no fish parts from it");
    }

    {
        // A fish school passing a shelter turns in and holds there, then goes on and does not
        // turn in again for a while.
        const island::IslandTerrain terrain;
        island::FishSchools schools(terrain);
        const island::FishSchoolState start = schools.Schools()[0];
        const float shelterX = start.centreX + 4.0f, shelterZ = start.centreZ;
        schools.SetShelter(shelterX, shelterZ, true);
        schools.Update(0.05f, start.centreX, 0.0f, start.centreZ);
        Check(schools.ShelteringCount() >= 1U && schools.ShelterCount() >= 1U,
              "a school within range of the shelter turns in at once");
        bool held = true;
        for (int step = 0; step < 560; ++step) {
            schools.Update(0.05f, start.centreX, 0.0f, start.centreZ);
            const auto& school = schools.Schools()[0];
            if (step > 100 && std::hypot(school.centreX - shelterX, school.centreZ - shelterZ) > 0.5f) held = false;
        }
        Check(held && schools.ShelteringCount() >= 1U, "the school eases to the shelter point and holds there for the hold's length");
        // Holding, the school's fish fan their pectorals, the fan at full with its phase
        // advancing.
        {
            float fanPeak = 0.0f, fanPhase = 0.0f;
            for (const auto& fish : schools.Fish()) { fanPeak = std::max(fanPeak, fish.finFan); if (fish.finFan > 0.9f) fanPhase = std::max(fanPhase, fish.finPhase); }
            Check(fanPeak > 0.9f && fanPhase > 0.5f && schools.FanningCount() >= 1U, "a holding school's fish do not fan their pectoral fins");
        }
        for (int step = 0; step < 60; ++step) schools.Update(0.05f, start.centreX, 0.0f, start.centreZ);
        const island::FishSchoolState released = schools.Schools()[0];
        bool movedOn = false;
        for (int step = 0; step < 400 && !movedOn; ++step) {
            schools.Update(0.05f, start.centreX, 0.0f, start.centreZ);
            const auto& school = schools.Schools()[0];
            movedOn = std::hypot(school.centreX - released.centreX, school.centreZ - released.centreZ) > 2.0f;
        }
        Check(movedOn && schools.ShelterCount() == 1U, "after the hold the school goes on its way and does not turn in again at once");
        // On its way, the fins fold back.
        {
            float fanLeft = 0.0f;
            for (const auto& fish : schools.Fish()) fanLeft = std::max(fanLeft, fish.finFan);
            Check(fanLeft < 0.2f && schools.FanningCount() == 0U, "a school on its way keeps its pectoral fins fanned");
        }
        bool fishFollow = true;
        for (const auto& fish : schools.Fish())
            if (fish.schoolIndex == 0U && !std::isfinite(fish.x + fish.y + fish.z)) fishFollow = false;
        Check(fishFollow, "the school's fish keep finite poses through the hold");
        // The world's jetty line reaches water deep enough for a school within forty metres,
        // and not under the deck itself (the water there is under half a metre).
        const island::JungleLayout jettyJungle(terrain);
        const std::vector<island::CircularObstacle> none;
        const auto hut = island::HutLayout::FindInWorld(terrain, jettyJungle, none);
        const auto jetty = hut ? island::JettyLayout::FindInWorld(terrain, *hut) : std::nullopt;
        float shadeAlong = -1.0f;
        if (jetty)
            for (float along = 5.0f; along <= 40.0f && shadeAlong < 0.0f; along += 1.0f)
                if (-terrain.Height(jetty->rootX + jetty->dirX * along, jetty->rootZ + jetty->dirZ * along) >= island::FishSchools::ShelterDepth) shadeAlong = along;
        Check(jetty.has_value() && shadeAlong > island::JettyPlacement::Length && shadeAlong < 30.0f,
              "the jetty's line reaches school-deep water a little off the end of the deck");
    }

    {
        // A bird settling on a branch perch is noted with the perch's position, so the
        // game can shake a leaf loose there; a wreck perch is not a crown.
        const island::IslandTerrain terrain;
        const float perchX = 60.0f, perchZ = 40.0f, perchY = terrain.Height(perchX, perchZ) + 6.0f;
        const std::vector<island::BirdPerchSite> sites{{perchX, perchY, perchZ, 0.3f, island::BirdPerchKind::Branch, false}};
        island::BirdFlock flock(terrain);
        flock.SetPerchSites(sites);
        bool landed = false;
        for (int step = 0; step < 6000 && !landed; ++step) {
            flock.Update(0.05f, perchX + 20.0f, perchY, perchZ);
            landed = flock.LandingCount() >= 1U;
        }
        const auto& landing = flock.LastLanding();
        Check(landed && landing.valid && std::abs(landing.x - perchX) < 0.01f && std::abs(landing.z - perchZ) < 0.01f
                  && std::abs(landing.y - perchY) < 0.01f,
              "a bird settling on a branch perch is noted at the perch within five minutes");
        const std::vector<island::BirdPerchSite> wreck{{20.0f, terrain.Height(20.0f, 20.0f) + 2.0f, 20.0f, 0.3f, island::BirdPerchKind::Wreck}};
        island::BirdFlock sitting(terrain);
        sitting.SetPerchSites(wreck);
        for (int step = 0; step < 1200; ++step) sitting.Update(0.05f, 45.0f, wreck[0].y, 20.0f);
        Check(sitting.LandingCount() == 0U, "a wreck perch landing is not a crown landing");
    }

    {
        // A parrot perched on a fruiting lobe feeds there in bouts (the head thrust) and stays
        // twice as long as on a bare branch; on a bare branch it never feeds.
        const island::IslandTerrain terrain;
        const float perchX = 60.0f, perchZ = 40.0f;
        const float perchY = terrain.Height(perchX, perchZ) + 6.0f;
        const auto runPerch = [&](const bool fruiting, float& feedSeen, float& perchSpan) {
            const std::vector<island::BirdPerchSite> sites{{perchX, perchY, perchZ, 0.3f, island::BirdPerchKind::Branch, fruiting}};
            island::BirdFlock flock(terrain);
            flock.SetPerchSites(sites);
            feedSeen = 0.0f; perchSpan = 0.0f;
            std::size_t parrot = flock.Birds().size();
            for (int step = 0; step < 6000; ++step) {
                flock.Update(0.05f, perchX + 20.0f, perchY, perchZ);
                if (parrot == flock.Birds().size()) {
                    for (std::size_t index = 10U; index < 14U; ++index)
                        if (flock.Birds()[index].behavior == island::BirdBehavior::Perched) parrot = index;
                    continue;
                }
                const auto& bird = flock.Birds()[parrot];
                if (bird.behavior != island::BirdBehavior::Perched) break;
                perchSpan += 0.05f;
                feedSeen = std::max(feedSeen, bird.feedAmount * bird.headBob);
            }
            return parrot < flock.Birds().size() ? flock.FeedCount() : 0U;
        };
        float fruitFeed = 0.0f, fruitSpan = 0.0f, bareFeed = 0.0f, bareSpan = 0.0f;
        const std::uint32_t fruitBouts = runPerch(true, fruitFeed, fruitSpan);
        const std::uint32_t bareBouts = runPerch(false, bareFeed, bareSpan);
        Check(fruitBouts >= 2U && fruitFeed > 0.6f, "a parrot on a fruiting lobe feeds there in bouts, reaching to the fruit");
        Check(fruitSpan > 15.0f && fruitSpan > bareSpan * 1.5f, "a parrot stays about twice as long on a fruiting lobe as on a bare branch");
        Check(bareBouts == 0U && bareFeed == 0.0f, "a parrot on a bare branch never feeds");
    }

    {
        // Fruit doves land beside the fallen fruit: with fruit lying on dove-grade ground
        // within range, a dove's next ground visit goes there and it forages around it.
        const island::IslandTerrain terrain;
        island::BirdFlock doves(terrain);
        const island::BirdState& dove = doves.Birds()[14U];
        float treeX = 0.0f, treeZ = 0.0f;
        bool spot = false;
        for (float radius = 8.0f; radius < 50.0f && !spot; radius += 4.0f)
            for (int step = 0; step < 12 && !spot; ++step) {
                const float angle = static_cast<float>(step) * 0.5236f;
                const float x = dove.x + std::cos(angle) * radius, z = dove.z + std::sin(angle) * radius;
                const auto sample = terrain.Sample(x, z);
                if (!sample.IsSubmerged() && sample.height > 0.8f && sample.signedShoreDistance > 24.0f
                    && sample.slopeRadians < 0.25f) { treeX = x; treeZ = z; spot = true; }
            }
        Check(spot, "dove-grade ground lies within fifty metres of the first dove");
        const std::vector<island::BirdPerchSite> sites{{treeX + 3.0f, terrain.Height(treeX + 3.0f, treeZ) + 2.5f, treeZ, 0.0f, island::BirdPerchKind::Branch}};
        doves.SetPerchSites(sites);
        island::FallingFruitField orchard(terrain);
        island::MonkeyState picker{};
        picker.behavior = island::MonkeyBehavior::InspectTree;
        picker.pickingFruit = true;
        picker.simulationTier = island::WildlifeSimulationTier::Full;
        picker.selectedTree = {treeX, terrain.Height(treeX, treeZ), treeZ, 0.4f, 1.0f, 2U};
        std::vector<island::MonkeyState> pickers{picker};
        for (int step = 0; step < 400; ++step) orchard.Update(0.05f, pickers);
        bool foragedAtFruit = false;
        for (int step = 0; step < 7200 && !foragedAtFruit; ++step) {
            doves.SetLyingFruit(orchard.Fruit());
            doves.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
            orchard.Update(0.05f, pickers);
            for (std::size_t index = 14U; index < 18U && index < doves.Birds().size(); ++index) {
                const auto& bird = doves.Birds()[index];
                if (bird.behavior != island::BirdBehavior::GroundForage) continue;
                for (const auto& fruit : orchard.Fruit())
                    if (fruit.active && std::hypot(bird.x - fruit.x, bird.z - fruit.z) < 3.0f) foragedAtFruit = true;
            }
        }
        Check(doves.FruitLandingCount() >= 1U && foragedAtFruit,
              "a fruit dove lands beside the fallen fruit and forages around it within six minutes");
    }

    {
        // Every fourth ground visit a fruit dove goes to the bath site (the plunge pool's
        // bank) and bathes there: standing at the site, wings flicking and head dipping in
        // bouts, then departs.
        const island::IslandTerrain terrain;
        island::BirdFlock doves(terrain);
        const island::StreamCourse course(terrain);
        const island::StreamStation& pool = course.PoolStation();
        const float reach = pool.halfWidth + 0.7f;
        const float side = terrain.Height(pool.x, pool.z + reach) <= terrain.Height(pool.x, pool.z - reach) ? 1.0f : -1.0f;
        const float bathX = pool.x, bathZ = pool.z + side * reach;
        Check(!terrain.Sample(bathX, bathZ).IsSubmerged() && terrain.Height(bathX, bathZ) > pool.water,
              "the pool's bath bank stands above the pool water on dry ground");
        const std::vector<island::BirdPerchSite> sites{{bathX + 6.0f, terrain.Height(bathX + 6.0f, bathZ) + 2.5f, bathZ, 0.0f, island::BirdPerchKind::Branch}};
        doves.SetPerchSites(sites);
        doves.SetBathSite(bathX, bathZ, true);
        bool bathed = false, flicked = false, dipped = false;
        for (int step = 0; step < 14400 && !(bathed && flicked && dipped); ++step) {
            doves.Update(0.05f, bathX, 2.0f, bathZ);
            for (std::size_t index = 0U; index < doves.Birds().size(); ++index) {
                const auto& bird = doves.Birds()[index];
                if (bird.species != island::BirdSpecies::FruitDove || bird.behavior != island::BirdBehavior::GroundForage) continue;
                if (std::hypot(bird.x - bathX, bird.z - bathZ) > 0.5f || bird.bathAmount < 0.9f) continue;
                bathed = true;
                if (bird.perchAmount < 0.75f) flicked = true;
                if (bird.headBob > 0.8f) dipped = true;
            }
        }
        Check(bathed && doves.BathCount() >= 1U, "a fruit dove comes down to the pool bank and bathes there within twelve minutes");
        Check(flicked && dipped, "the bathing dove flicks its wings open and dips its head in bouts");
        for (int step = 0; step < 600; ++step) doves.Update(0.05f, bathX, 2.0f, bathZ);
        bool stuck = false;
        for (const auto& bird : doves.Birds())
            if (bird.bathAmount > 0.0f && bird.behavior != island::BirdBehavior::GroundForage) stuck = true;
        Check(!stuck, "a dove leaving the bath carries no bathing pose into the air");
    }

    {
        // The stream's water level is the nearest stretch's in its channel (what the quad's
        // wheels wade), and none out on the ground beside it.
        const island::IslandTerrain terrain;
        const island::StreamCourse course(terrain);
        bool level = true, beside = true;
        for (std::size_t i = 4; i + 2 < course.Count(); ++i) {
            const auto& a = course.Station(i);
            const auto& b = course.Station(i + 1);
            const float midX = 0.5f * (a.x + b.x), midZ = 0.5f * (a.z + b.z);
            float water = 0.0f;
            if (!course.WaterLevel(midX, midZ, water) || std::abs(water - 0.5f * (a.water + b.water)) > 1e-3f) level = false;
            const float length = std::hypot(b.x - a.x, b.z - a.z), off = 2.0f * std::max(a.halfWidth, b.halfWidth);
            if (course.WaterLevel(midX - (b.z - a.z) / length * off, midZ + (b.x - a.x) / length * off, water)
                || course.WaterLevel(midX + (b.z - a.z) / length * off, midZ - (b.x - a.x) / length * off, water)) beside = false;
        }
        Check(level, "the stream's water level in its channel is its course's there");
        Check(beside, "two half widths off the stream's line there is no stream water");
    }

    {
        // The stream's rings: a wheel running through drops one each half metre, a point ringing
        // again close to a young ring adds none, the oldest gives way and they fade out.
        island::StreamRingField rings;
        for (int step = 0; step < 10; ++step) rings.Note(static_cast<float>(step) * 0.1f, 0.0f, 0.8f);
        Check(rings.Count() == 2U, "a wheel's run through the stream drops a ring each half metre");
        rings.Update(0.2f);
        rings.Update(0.2f);
        rings.Note(0.0f, 0.0f, 0.8f);
        Check(rings.Count() == 3U, "a point rings again once its ring has spread");
        for (int ring = 0; ring < 20; ++ring) rings.Note(10.0f + static_cast<float>(ring), 0.0f, 2.0f);
        std::size_t active = 0;
        bool firstGone = true;
        for (const auto& ring : rings.Rings()) {
            if (!ring.active) continue;
            ++active;
            if (ring.x < 5.0f) firstGone = false;
            if (ring.strength > 1.0f) firstGone = false;
        }
        Check(active == island::StreamRingField::Capacity && firstGone, "the oldest rings give way, strengths held to one");
        rings.Update(0.25f); rings.Update(0.25f); rings.Update(0.25f); rings.Update(0.25f);
        rings.Update(0.25f); rings.Update(0.25f); rings.Update(0.25f);
        bool faded = true;
        for (const auto& ring : rings.Rings()) faded = faded && !ring.active;
        Check(faded, "the stream's rings fade out within their life");
    }

    {
        // The drawn ground that marks and shadows lie on follows a local patch drawn in place of
        // the mesh (the stream's carved bed), and is the mesh itself away from it.
        const island::IslandTerrain terrain;
        const island::RenderedTerrainSurface mesh(terrain);
        const island::StreamCourse course(terrain);
        const island::StreamBedPatch bed(course, mesh);
        island::RenderedTerrainSurface drawn(terrain);
        drawn.SetLocalPatches({&bed, nullptr});
        const auto& station = course.Station(45);
        const float inBed = drawn.Height(station.x, station.z);
        Check(inBed == bed.SampleHeight(station.x, station.z) && inBed < mesh.Height(station.x, station.z) - 0.2f
              && drawn.Sample(station.x, station.z).height == inBed && drawn.Sample(station.x, station.z).normal.y > 0.5f,
              "the drawn ground in the stream's channel is its carved bed, below the mesh");
        Check(drawn.Height(40.0f, 60.0f) == mesh.Height(40.0f, 60.0f), "away from the patches the drawn ground is the mesh");
    }

    {
        // Butterflies alternate their nectar visits between the ground patch and a flowering
        // bush inside the habitat, hovering at blossom height over the bush.
        const island::IslandTerrain terrain;
        island::ButterflyMeadow meadow(terrain);
        const auto home = meadow.Homes()[0];
        const std::vector<island::BushPlacement> bushes{
            {home.x + 2.0f, 0.0f, home.z + 1.0f, 0.0f, 1.0f, 1U},
            {home.x - 1.5f, 0.0f, home.z + 2.0f, 0.0f, 1.2f, 0U},
            {home.x + 80.0f, 0.0f, home.z, 0.0f, 1.0f, 2U}};
        meadow.SetFlowerBushes(bushes);
        Check(meadow.FlowerBushCount() == 1U, "only flowering bushes within reach of a habitat become nectar sites");
        bool visitedBush = false;
        for (int step = 0; step < 2400 && !visitedBush; ++step) {
            meadow.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
            for (std::size_t index = 0; index < 6U; ++index) {
                float x = 0.0f, z = 0.0f, height = 0.0f;
                if (meadow.NectarTarget(index, x, z, height)
                    && meadow.Butterflies()[index].behavior == island::ButterflyBehavior::NectarHover
                    && std::hypot(x - (home.x + 2.0f), z - (home.z + 1.0f)) < 0.6f && height > 0.5f)
                    visitedBush = true;
            }
        }
        Check(visitedBush, "a butterfly hovers at blossom height over the flowering bush");
    }

    {
        // The wrack line is one function for the shader and the crabs, and the crabs reach it:
        // over four minutes a fair share of crab time is spent within a metre of the line.
        const island::IslandTerrain terrain;
        Check(std::abs(island::WrackLine::Centre(100.0f,0.0f)-island::WrackLine::BaseInland)<3.2f
              &&island::WrackLine::Centre(100.0f,0.0f)!=island::WrackLine::Centre(0.0f,100.0f),
              "the wrack line wanders a bounded distance about its base inland distance");
        island::CrabColony colony(terrain);
        std::size_t onLine=0,samples=0;
        for(int step=0;step<4800;++step) {
            colony.Update(0.05f,std::numeric_limits<float>::quiet_NaN(),0.0f,0.0f);
            if(step%20!=0) continue;
            for(const auto& crab:colony.Crabs()) {
                ++samples;
                if(std::abs(terrain.SignedShoreDistance(crab.x,crab.z)-island::WrackLine::Centre(crab.x,crab.z))<1.0f) ++onLine;
            }
        }
        Check(samples>0&&onLine*8>samples,"beach crabs spend part of their time working the wrack line");
    }

    {
        // Falling fruit: a picking monkey in a fruiting crown knocks fruit loose; it drops
        // from the crown, lands on the ground below and lies there.
        const island::IslandTerrain terrain;
        island::FallingFruitField field(terrain);
        island::MonkeyState monkey{};
        monkey.behavior=island::MonkeyBehavior::InspectTree;
        monkey.pickingFruit=true;
        monkey.simulationTier=island::WildlifeSimulationTier::Full;
        monkey.selectedTree={-60.0f,terrain.Height(-60.0f,-60.0f),-60.0f,0.4f,1.0f,2U};
        monkey.treeIndex=0;
        std::vector<island::MonkeyState> monkeys{monkey};
        float highest=-1e9f;
        bool landed=false;
        for(int step=0;step<600;++step) {
            field.Update(0.05f,monkeys);
            for(const auto& f:field.Fruit()) {
                if(!f.active) continue;
                highest=std::max(highest,f.y);
                if(f.restSeconds>=0.0f&&std::abs(f.y-(terrain.Height(f.x,f.z)+0.06f))<0.01f
                    &&std::hypot(f.x+60.0f,f.z+60.0f)<4.0f) landed=true;
            }
        }
        Check(field.DropCount()>=3U,"a picking monkey knocks several fruit loose within half a minute");
        // Gust fronts over coconut palms: with one living fruiting palm beside the observer,
        // the fronts passing over four minutes shake at least one nut loose, from the crown.
        const std::vector<island::PalmPlacement> palms{{-60.0f,terrain.Height(-60.0f,-60.0f),-60.0f,0.3f,1.0f,60.0f,0.0f,1U,island::PalmCondition::Living}};
        island::FallingFruitField grove(terrain);
        grove.SetPalms(palms);
        float nutTop=-1e9f;
        for(int step=0;step<4800;++step) {
            grove.UpdateCoconuts(static_cast<float>(step)*0.05f,-60.0f,-58.0f);
            grove.Update(0.05f,{});
            for(const auto& f:grove.Fruit()) if(f.active&&f.variant==island::FallingFruitField::CoconutVariant) nutTop=std::max(nutTop,f.y);
        }
        Check(grove.CoconutDropCount()>=1U&&nutTop>palms[0].y+6.0f,"gust fronts shake a coconut from the palm crown within four minutes");
        Check(grove.LastCoconutLanding().count>=1U&&std::abs(grove.LastCoconutLanding().y-(terrain.Height(grove.LastCoconutLanding().x,grove.LastCoconutLanding().z)+0.10f))<0.01f,
              "the coconut's landing is counted where it struck the ground");
        {
            // A storm's wind (2) shakes nuts loose at more of the same fronts: a row of
            // fruiting palms over twenty minutes, in the trade wind and in the storm.
            std::vector<island::PalmPlacement> row;
            for(int i=0;i<8;++i) {
                const float z=-70.0f+static_cast<float>(i)*3.0f;
                row.push_back({-60.0f,terrain.Height(-60.0f,z),z,0.3f,1.0f,60.0f,0.0f,1U,island::PalmCondition::Living});
            }
            island::FallingFruitField calmRow(terrain),stormRow(terrain);
            calmRow.SetPalms(row);stormRow.SetPalms(row);stormRow.SetWind(2.0f);
            for(int step=0;step<24000;++step) {
                calmRow.UpdateCoconuts(static_cast<float>(step)*0.05f,-60.0f,-60.0f);
                stormRow.UpdateCoconuts(static_cast<float>(step)*0.05f,-60.0f,-60.0f);
                calmRow.Update(0.05f,{});stormRow.Update(0.05f,{});
            }
            Check(stormRow.CoconutDropCount()>calmRow.CoconutDropCount()*2U,"a storm's wind shakes more than twice as many coconuts loose");
            std::cout<<"coconuts shaken loose from eight palms in twenty minutes: "<<calmRow.CoconutDropCount()
                <<" in the trade wind, "<<stormRow.CoconutDropCount()<<" in a storm's\n";
        }
        // A nut from a palm leaning over the shallows strikes the water, bobs at the surface,
        // drifts toward the shore and washes up on the sand within a minute, no thud counted.
        {
            island::FallingFruitField shallows(terrain);
            const float palmX=terrain.ShoreRadius(0.0f)+2.5f;
            const island::PalmPlacement leaning{palmX,std::max(terrain.Height(palmX,0.0f),0.0f),0.0f,0.0f,1.0f,-2.5f,0.0f,1U,island::PalmCondition::Living};
            shallows.ShakeCoconut(leaning,3U);
            bool floated=false,beachedOnSand=false;
            for(int step=0;step<1200&&!beachedOnSand;++step) {
                shallows.Update(0.05f,{});
                for(const auto& f:shallows.Fruit()) if(f.active&&f.variant==island::FallingFruitField::CoconutVariant) {
                    if(f.afloat&&std::abs(f.y)<0.08f&&terrain.Height(f.x,f.z)<0.0f) floated=true;
                    if(!f.afloat&&f.restSeconds>=0.0f&&shallows.BeachedCount()>=1U&&terrain.Height(f.x,f.z)>=0.0f) beachedOnSand=true;
                }
            }
            Check(floated&&beachedOnSand&&shallows.LastCoconutLanding().count==0U,"a nut that strikes the shallows floats, drifts ashore and washes up on the sand");
        }
        Check(highest>monkey.selectedTree.y+3.0f,"the fruit starts up in the crown");
        Check(landed,"the fruit lands on the ground under the tree and lies there");
        // The fronts cross the open sea as well as the land (the ocean's cat's paws read the
        // same field): strength over the lagoon and far offshore peaks within three spacings
        // (the across-wind envelope drifts slowly, so one spacing can pass a point weakly).
        float seaPeak=0,offshorePeak=0;
        for(float t=0;t<3.0f*island::WindGust::FrontSpacing/island::WindGust::FrontSpeed;t+=0.25f) {
            seaPeak=std::max(seaPeak,island::WindGust::Strength(0.0f,-230.0f,t));
            offshorePeak=std::max(offshorePeak,island::WindGust::Strength(400.0f,120.0f,t));
        }
        Check(seaPeak>0.8f&&offshorePeak>0.8f,"gust fronts cross the lagoon and the open sea within three spacings");
        // And the beaches, where the terrain material lifts sand streamers in the band: the
        // field peaks on the dry sand a few metres inland on the east and west shores.
        float eastBeach=0,westBeach=0;
        const float eastShore=terrain.ShoreRadius(0.0f)-3.0f,westShore=terrain.ShoreRadius(3.14159f)-3.0f;
        for(float t=0;t<3.0f*island::WindGust::FrontSpacing/island::WindGust::FrontSpeed;t+=0.25f) {
            eastBeach=std::max(eastBeach,island::WindGust::Strength(eastShore,0.0f,t));
            westBeach=std::max(westBeach,island::WindGust::Strength(-westShore,0.0f,t));
        }
        Check(eastBeach>0.8f&&westBeach>0.8f,"gust fronts cross both beaches within three spacings");
    {
        // Leaves drift down the stream: every leaf floats on the channel's water inside its
        // banks, the current hurries them over the cascade and slackens in the pool, and over
        // ten minutes leaves reach the sea and are replaced near the source.
        const island::IslandTerrain terrain;
        island::StreamDriftField drift(terrain);
        const auto& course=drift.Course();
        const auto afloat=[&](const island::DriftingLeaf& leaf){
            if(!leaf.active||!std::isfinite(leaf.x)||!std::isfinite(leaf.y)||!std::isfinite(leaf.z)) return false;
            if(leaf.atSea||leaf.beached) return true;   // on the lagoon or washed up, checked separately
            float best=1e9f; const island::StreamStation* nearest=nullptr;
            for(std::size_t i=0;i<course.Count();++i) { const float d=std::hypot(course.Station(i).x-leaf.x,course.Station(i).z-leaf.z); if(d<best) { best=d; nearest=&course.Station(i); } }
            return nearest&&best<=nearest->halfWidth*1.2f+0.3f&&std::abs(leaf.y-nearest->water-island::StreamDriftField::SurfaceLift)<0.45f;
        };
        bool allAfloat=true;
        for(const auto& leaf:drift.Leaves()) allAfloat=allAfloat&&afloat(leaf);
        Check(allAfloat,"every drifting leaf starts afloat inside the stream banks");
        const float lipDistance=course.Station(course.CascadeIndex()).distance;
        Check(drift.SpeedAt(lipDistance-20.0f)==island::StreamDriftField::ReachSpeed
              &&drift.SpeedAt(lipDistance+0.2f)>drift.SpeedAt(lipDistance-20.0f)*3.0f
              &&drift.SpeedAt(course.PoolStation().distance)<drift.SpeedAt(lipDistance-20.0f),
              "the current hurries over the cascade and slackens in the pool");
        for(int step=0;step<12000;++step) {
            drift.Update(0.05f);
            if(step%600==0) for(const auto& leaf:drift.Leaves()) allAfloat=allAfloat&&afloat(leaf);
        }
        Check(allAfloat&&drift.PassCount()>=1U,"leaves stay afloat in the channel and some reach the sea within ten minutes");
        // Out of the mouth the leaves drift on across the lagoon downwind (toward -x) for a
        // while, on the surface, before they are lost and replaced near the source.
        {
            bool seaLeaf=false,downwind=true,onSurface=true;
            for(const auto& leaf:drift.Leaves()) if(leaf.active&&leaf.atSea) {
                seaLeaf=true;
                if(leaf.x>course.Station(course.Count()-1).x+0.5f) downwind=false;
                if(std::abs(leaf.y)>0.06f) onSurface=false;
            }
            Check(drift.SeaCount()>=1U&&seaLeaf&&downwind&&onSurface,"leaves out of the mouth drift on the lagoon downwind at the surface");
        }
        {
            // A leaf on the lagoon drifting onto rising sand washes up and lies on the strand
            // a while: with the drift run long enough for the sea leaves to meet the shore,
            // beached leaves lie at sand height out of the water, never afloat and beached at once.
            island::StreamDriftField washed(terrain);
            bool lying=true;
            for(int step=0;step<24000&&washed.BeachedCount()==0U;++step) washed.Update(0.05f);
            for(const auto& leaf:washed.Leaves()) if(leaf.active&&leaf.beached) {
                if(leaf.atSea||terrain.Height(leaf.x,leaf.z)<0.0f||std::abs(leaf.y-terrain.Height(leaf.x,leaf.z)-0.02f)>0.001f) lying=false;
            }
            Check(washed.BeachedCount()>=1U&&lying,"a leaf drifting onto rising sand washes up and lies on the strand at sand height");
        }
        {
            // A point on the water is located on the course; one fifty metres off is not. A
            // leaf that landed there joins the drift in the place of the leaf furthest along, in its own colour
            // and goes on down the stream.
            island::StreamDriftField adopter(terrain);
            const auto& course=adopter.Course();
            const auto& station=course.Station(40);
            float courseDistance=0,lateral=0;
            Check(course.Locate(station.x,station.z,courseDistance,lateral)&&std::abs(courseDistance-station.distance)<0.05f&&std::abs(lateral)<0.05f,
                  "a point on a stream station is located at its distance in the middle of the water");
            Check(!course.Locate(station.x+50.0f,station.z,courseDistance,lateral),"a point fifty metres from the stream is not on it");
            for(int step=0;step<40;++step) adopter.Update(0.05f);
            Check(adopter.Adopt(station.distance,0.3f,3U)&&adopter.AdoptCount()==1U,"a leaf that landed on the water joins the drift");
            std::size_t active=0;for(const auto& leaf:adopter.Leaves()) active+=leaf.active?1U:0U;
            Check(active==island::StreamDriftField::Capacity,"the adopted leaf takes the place of the leaf furthest along, the drift staying full");
            std::size_t adopted=adopter.Leaves().size();
            for(std::size_t i=0;i<adopter.Leaves().size();++i) if(adopter.Leaves()[i].active&&adopter.Leaves()[i].variant==3U) adopted=i;
            Check(adopted<adopter.Leaves().size()&&std::abs(adopter.Leaves()[adopted].distance-station.distance)<0.05f,"the adopted leaf starts where it landed, in its own colour");
            const float before=adopter.Leaves()[adopted].distance;
            for(int step=0;step<60;++step) adopter.Update(0.05f);
            Check(adopter.Leaves()[adopted].active&&adopter.Leaves()[adopted].distance>before+0.3f,"the adopted leaf goes on down the stream with the current");
        }
    }
    {
        // Small fish in the plunge pool: all of them hold under the surface inside the pool's
        // reach, dart now and then, and scatter toward the deepest station when the walker
        // comes to the bank, returning to their stations once they are left alone.
        const island::IslandTerrain terrain;
        island::StreamFishField pool(terrain);
        const auto& course=pool.Course();
        const auto inPool=[&](const island::StreamFish& f){
            if(!f.active||!std::isfinite(f.x)||!std::isfinite(f.y)||!std::isfinite(f.z)) return false;
            float best=1e9f; const island::StreamStation* nearest=nullptr;
            for(std::size_t i=0;i<course.Count();++i) { const float d=std::hypot(course.Station(i).x-f.x,course.Station(i).z-f.z); if(d<best) { best=d; nearest=&course.Station(i); } }
            // Under the surface at the fish's own spot: the water level steps between the
            // pool's stations, so the nearest station's level is not the surface there.
            float sx=0,sy=0,sz=0,sh=0;
            if(!pool.Place(f.distance,f.lateral,0.0f,sx,sy,sz,sh)) return false;
            return nearest&&best<=nearest->halfWidth+0.2f&&f.y<=sy-0.05f&&f.y>=sy-0.5f
                &&f.distance>=pool.PoolStart()-0.01f&&f.distance<=pool.PoolEnd()+0.01f;
        };
        bool allInPool=true;
        for(const auto& f:pool.Fish()) allInPool=allInPool&&inPool(f);
        Check(allInPool,"every pool fish starts under the surface inside the plunge pool");
        for(int step=0;step<1200;++step) { pool.Update(0.05f,1000.0f,1000.0f); for(const auto& f:pool.Fish()) allInPool=allInPool&&inPool(f); }
        Check(allInPool&&pool.DartCount()>=3U,"the fish dart now and then and stay in the pool");
        float deepX=0,deepY=0,deepZ=0,deepHeading=0;
        Check(pool.Place(pool.DeepDistance(),0.0f,0.1f,deepX,deepY,deepZ,deepHeading),"the pool's deep station resolves");
        {
            // A splash in the pool with no walker about scatters the fish to the deep as the
            // walker does, and they drift back once it has passed.
            island::StreamFishField splashed(terrain);
            for(int step=0;step<200;++step) splashed.Update(0.05f,1000.0f,1000.0f);
            const auto before=splashed.ScatterCount();
            splashed.NoteSplash(deepX,deepZ);
            splashed.Update(0.05f,1000.0f,1000.0f);
            Check(splashed.ScatterCount()==before+1U&&splashed.SplashScatterCount()==1U,"a splash in the pool scatters the fish");
            bool hiding=false;
            for(const auto& f:splashed.Fish()) if(f.active&&f.hiding) hiding=true;
            Check(hiding,"the splashed fish make for the deep");
            for(int step=0;step<200;++step) splashed.Update(0.05f,1000.0f,1000.0f);
            bool allHiding=true;
            for(const auto& f:splashed.Fish()) if(f.active&&!f.hiding) allHiding=false;
            Check(!allHiding,"with the splash passed the fish come back out");
            island::StreamFishField farSplash(terrain);
            farSplash.NoteSplash(deepX+60.0f,deepZ);
            farSplash.Update(0.05f,1000.0f,1000.0f);
            Check(farSplash.SplashScatterCount()==0U,"a splash sixty metres off scatters no pool fish");
        }
        float spreadBefore=0; for(const auto& f:pool.Fish()) spreadBefore+=std::abs(f.distance-pool.DeepDistance());
        for(int step=0;step<120;++step) pool.Update(0.05f,deepX+1.5f,deepZ);
        float spreadAfter=0; bool allHiding=true; for(const auto& f:pool.Fish()) { spreadAfter+=std::abs(f.distance-pool.DeepDistance()); allHiding=allHiding&&f.hiding; }
        Check(pool.ScatterCount()==1U&&allHiding&&spreadAfter<spreadBefore*0.5f,"the walker at the bank sends every fish to the deep within six seconds");
        for(int step=0;step<600;++step) pool.Update(0.05f,1000.0f,1000.0f);
        float spreadBack=0; for(const auto& f:pool.Fish()) { spreadBack+=std::abs(f.distance-pool.DeepDistance()); allInPool=allInPool&&inPool(f); }
        Check(allInPool&&spreadBack>spreadAfter*2.0f,"left alone, the fish spread back to their stations");
        // Leaves drifting through the pool draw the fish up: with the drift field feeding the
        // pool, a fish rises to a passing leaf within five minutes and the dart ends on the
        // leaf's line.
        island::StreamDriftField leaves(terrain);
        for(int step=0;step<6000&&pool.RiseCount()==0U;++step) {
            leaves.Update(0.05f);
            pool.SetLeaves(leaves.Leaves());
            pool.Update(0.05f,1000.0f,1000.0f);
        }
        bool inPoolStill=true;
        for(const auto& f:pool.Fish()) inPoolStill=inPoolStill&&inPool(f);
        Check(pool.RiseCount()>=1U&&inPoolStill,"a fish rises to a leaf drifting over the pool within five minutes");
    }
    {
        // Turtle hatchlings: a clutch breaks out of a nest on the dry east beach, every one of
        // them scrambles seaward across the sand, and within three minutes some reach the surf
        // and swim off; the sand between the nest and the sea is where they all are.
        const island::IslandTerrain terrain;
        island::HatchlingField hatchlings(terrain);
        const float nestX=terrain.ShoreRadius(0.0f)-6.0f;
        const std::vector<island::HatchlingNest> nests{{nestX,0.0f}};
        hatchlings.SetNests(nests);
        {
            // With the clock set, a clutch waits for the dark and breaks out once it falls.
            island::HatchlingField nightly(terrain);
            nightly.SetNests(nests);
            nightly.SetNightness(0.0f);
            for(int step=0;step<200*10;++step) nightly.Update(0.05f);
            const auto byDay=nightly.ClutchCount();
            nightly.SetNightness(1.0f);
            for(int step=0;step<40;++step) nightly.Update(0.05f);
            Check(byDay==0U&&nightly.ClutchCount()==1U,"a clutch waits for the dark and breaks out once it falls");
        }
        for(int step=0;step<200;++step) hatchlings.Update(0.05f);
        Check(hatchlings.ClutchCount()==1U&&hatchlings.ActiveCount()==island::HatchlingField::ClutchSize,"a clutch breaks out of the nest soon after the start");
        float nearest=1e9f,farthest=0;
        for(const auto& h:hatchlings.Hatchlings()) if(h.active) { const float r=std::hypot(h.x,h.z); nearest=std::min(nearest,r); farthest=std::max(farthest,r); }
        bool allSeaward=true;
        for(int step=0;step<3600;++step) {
            hatchlings.Update(0.05f);
            for(const auto& h:hatchlings.Hatchlings()) if(h.active&&!h.swimming) {
                const float r=std::hypot(h.x,h.z);
                if(r<nestX-1.0f||terrain.Height(h.x,h.z)<=0.0f||std::abs(h.y-terrain.Height(h.x,h.z)-0.02f)>0.001f) allSeaward=false;
            }
        }
        Check(allSeaward,"the hatchlings keep to the sand between the nest and the sea");
        Check(hatchlings.ReachedSeaCount()>=3U,"some hatchlings reach the surf within three minutes");
        // A flying bird's shadow falls on the ground opposite the fixed sun, further the
        // higher the bird; a bird on the ground and a bird over the sea cast none.
        {
            // Twenty metres inside the east shore: the higher bird's shadow lands ten metres
            // seaward of it and must still be on the sand.
            const float groundX=nestX-14.0f,groundZ=0.0f;
            const float ground=terrain.Height(groundX,groundZ);
            const auto low=island::BirdShadowProjection::Project(terrain,groundX,ground+6.0f,groundZ);
            const auto high=island::BirdShadowProjection::Project(terrain,groundX,ground+18.0f,groundZ);
            const float slopeX=island::environment::SunDirectionX/island::environment::SunDirectionY;
            const float slopeZ=island::environment::SunDirectionZ/island::environment::SunDirectionY;
            Check(low.valid&&high.valid&&low.height>5.0f&&high.height>16.0f,"birds six and eighteen metres up cast shadows on the beach");
            Check((low.x-groundX)*(-slopeX)>0.0f&&(low.z-groundZ)*(-slopeZ)>0.0f
                  &&std::hypot(high.x-groundX,high.z-groundZ)>std::hypot(low.x-groundX,low.z-groundZ)*2.0f,
                  "the shadow falls away from the sun, further for the higher bird");
            const auto perched=island::BirdShadowProjection::Project(terrain,groundX,ground+0.1f,groundZ);
            const auto overSea=island::BirdShadowProjection::Project(terrain,terrain.ShoreRadius(0.0f)+60.0f,8.0f,0.0f);
            Check(!perched.valid&&!overSea.valid,"a bird on the ground or over the sea casts no ground shadow");
        }
        // The cumulus shadow field: about a quarter of the island under shade with soft
        // edges, and the whole pattern drifting with the trade wind rather than flickering.
        {
            int shaded=0,soft=0,samples=0;
            for(float x=-300.0f;x<=300.0f;x+=10.0f) for(float z=-300.0f;z<=300.0f;z+=10.0f) {
                const float cover=island::CloudShadow::Cover(x,z,100.0f);
                ++samples; if(cover>0.5f) ++shaded; if(cover>0.02f&&cover<0.98f) ++soft;
            }
            const float coverage=static_cast<float>(shaded)/static_cast<float>(samples);
            Check(coverage>0.08f&&coverage<0.5f&&soft>samples/40,"the cumulus shade covers a bounded fraction of the island with soft edges");
            float drift=0;
            for(float x=-200.0f;x<=200.0f;x+=25.0f) for(float z=-200.0f;z<=200.0f;z+=25.0f)
                drift=std::max(drift,std::abs(island::CloudShadow::Cover(x,z,100.0f)
                    -island::CloudShadow::Cover(x+island::CloudShadow::DriftX*30.0f,z+island::CloudShadow::DriftZ*30.0f,70.0f)));
            Check(drift<0.05f,"the cumulus shade drifts with the trade wind as one pattern");
        }
        // The cascade's mist: puffs are released at the foot of the drop, rise above the
        // pool, swell and fade within their life, and never stray far from the foot.
        {
            island::CascadeMistField mist(terrain);
            Check(mist.Present()&&mist.DropHeight()>0.3f,"the cascade has a drop for the mist to rise from");
            bool rose=false,strayed=false;std::size_t most=0;
            for(int step=0;step<200;++step) {
                mist.Update(0.05f,static_cast<float>(step)*0.05f);
                std::size_t active=0;
                for(const auto& puff:mist.Puffs()) if(puff.active) {
                    ++active;
                    if(puff.y>mist.FootY()+0.45f&&puff.density>0.5f) rose=true;
                    if(std::hypot(puff.x-mist.FootX(),puff.z-mist.FootZ())>6.0f||puff.y<mist.FootY()||puff.y>mist.FootY()+4.0f) strayed=true;
                }
                most=std::max(most,active);
            }
            Check(rose&&!strayed&&most>=6U,"the spray rises above the pool in a bounded cloud of puffs");
        }
        // The sun shafts belong under the canopy and need the sun: full under a dense crown
        // in the open sun, none in the open, almost none under a cloud.
        Check(island::CanopyShafts::Strength(0.2f,0.0f)==1.0f&&island::CanopyShafts::Strength(0.9f,0.0f)==0.0f
              &&island::CanopyShafts::Strength(0.2f,1.0f)<0.2f&&island::CanopyShafts::Strength(0.575f,0.0f)>0.45f
              &&island::CanopyShafts::Strength(0.575f,0.0f)<0.55f,
              "canopy sun shafts show under a crown in the sun and not in the open or under a cloud");
        // The walker's trail: ten metres along the sand lays a print every stride, feet
        // alternating either side of the line; off the sand none are laid; old prints fade.
        {
            island::FootprintTrail trail;
            float wx=0,wz=0;
            for(int step=0;step<200;++step) { wx+=1.0f*0.05f; trail.Update(0.05f,wx,wz,1.0f,0.0f,true); }
            Check(trail.PrintCount()>=14U&&trail.PrintCount()<=18U,"a ten-metre walk on sand lays a print every stride");
            bool alternating=true,onLine=true;
            int lefts=0;
            for(const auto& print:trail.Prints()) if(print.active) {
                if(std::abs(print.z)>island::FootprintTrail::FootAcross+0.01f||print.x<0.0f||print.x>10.5f) onLine=false;
                if(print.left) ++lefts;
                if((print.left&&print.z>0.0f)||(!print.left&&print.z<0.0f)) alternating=false;
            }
            Check(onLine&&alternating&&lefts>=6,"the prints lie along the line of travel, left and right feet either side");
            const std::uint32_t before=trail.PrintCount();
            for(int step=0;step<200;++step) { wx+=0.05f; trail.Update(0.05f,wx,wz,1.0f,0.0f,false); }
            Check(trail.PrintCount()==before,"no prints are laid off the sand");
            for(int step=0;step<2500;++step) trail.Update(0.1f,wx,wz,0.0f,0.0f,true);
            Check(trail.ActiveCount()==0U,"the sand takes the prints back within the trail's life");
        }
        // In a downpour the rain beats the prints out within a minute; in the dry they last.
        {
            island::FootprintTrail dry,rained;
            rained.SetRain(1.0f);
            float wx=0;
            for(int step=0;step<60;++step) { wx+=0.05f; dry.Update(0.05f,wx,0,1.0f,0.0f,true); rained.Update(0.05f,wx,0,1.0f,0.0f,true); }
            for(int step=0;step<450;++step) { dry.Update(0.1f,wx,0,0.0f,0.0f,true); rained.Update(0.1f,wx,0,0.0f,0.0f,true); }
            Check(dry.ActiveCount()>=4U&&rained.ActiveCount()==0U,"a downpour beats the prints out within a minute");
        }
        // A hoofed trail: cloven prints at the animal's own stride, laid only on the ground
        // the caller says is printable.
        {
            island::FootprintTrail hooves;
            hooves.SetKind(1U);
            hooves.SetStride(0.9f);
            float hx=0;
            for(int step=0;step<200;++step) { hx+=0.05f; hooves.Update(0.05f,hx,0.0f,1.0f,0.0f,true); }
            Check(hooves.PrintCount()>=10U&&hooves.PrintCount()<=12U,"a ten-metre walk lays a cloven print every 0.9 m stride");
            bool cloven=true;
            for(const auto& print:hooves.Prints()) if(print.active&&print.kind!=1U) cloven=false;
            Check(cloven&&hooves.Kind()==1U,"the hoofed trail's prints are cloven");
            island::FootprintTrail bare;
            bare.SetStride(0.0f);
            for(int step=0;step<200;++step) { hx+=0.05f; bare.Update(0.05f,hx,0.0f,1.0f,0.0f,true); }
            Check(bare.PrintCount()>=14U&&bare.PrintCount()<=18U&&bare.Prints()[0].kind==0U,"an invalid stride leaves the walker's own");
            // Prints on the wet sand are taken back by the swash within a minute; those laid
            // on dry sand in the same walk keep for four.
            island::FootprintTrail shore;
            float sx=0;
            for(int step=0;step<40;++step) { sx+=0.05f; shore.Update(0.05f,sx,0.0f,1.0f,0.0f,true,true); }
            for(int step=0;step<40;++step) { sx+=0.05f; shore.Update(0.05f,sx,0.0f,1.0f,0.0f,true,false); }
            std::size_t wetPrints=0,dryPrints=0;
            for(const auto& print:shore.Prints()) if(print.active) { if(print.life<100.0f) ++wetPrints; else ++dryPrints; }
            Check(wetPrints>=2U&&dryPrints>=2U,"a walk from the wet strip onto dry sand lays prints of both lives");
            for(int step=0;step<700;++step) shore.Update(0.1f,sx,0.0f,0.0f,0.0f,true,false);
            std::size_t wetLeft=0,dryLeft=0;
            for(const auto& print:shore.Prints()) if(print.active) { if(print.life<100.0f) ++wetLeft; else ++dryLeft; }
            Check(wetLeft==0U&&dryLeft==dryPrints,"after seventy seconds the wet sand's prints are gone and the dry sand's remain");
        }
        // Blown sand: the windward beaches' exposed dry sand streams puffs downwind while a
        // gust front passes and none between fronts; the puffs drift westward with the wind
        // and are gone within two seconds.
        {
            island::BlownSandField sand(terrain);
            Check(sand.Sources().size()>=20U,"the windward beaches give the blown sand its sources");
            {
                // Regression (2026-10-20 crash): over a beach in a gust the field holds more
                // puffs than the dust renderer's old 18-puff buffer, so the renderer drawing it
                // must be sized for BlownSandField::Capacity.
                island::BlownSandField busy(terrain);
                const auto at=busy.Sources()[0];
                std::size_t peak=0U;
                for(int step=0;step<1800;++step) { busy.Update(0.05f,step*0.05f,at.x,at.z); peak=std::max(peak,busy.ActiveCount()); }
                std::cout<<"blown sand peak active puffs "<<peak<<" of "<<island::BlownSandField::Capacity<<"\n";
                Check(peak<=island::BlownSandField::Capacity,"the blown sand field stays within its capacity");
            }
            bool allDry=true;
            for(const auto& source:sand.Sources()) {
                const auto sample=terrain.Sample(source.x,source.z);
                if(sample.surface!=island::TerrainSurface::DrySand||island::wind::Exposure(source.x,source.z,sample.signedShoreDistance)<island::BlownSandField::MinimumExposure) allDry=false;
            }
            Check(allDry,"every source lies on exposed dry sand");
            const auto source=sand.Sources()[0];
            float gustTime=-1.0f,calmTime=-1.0f;
            for(float t=0.0f;t<90.0f&&(gustTime<0.0f||calmTime<0.0f);t+=0.1f) {
                const float g=island::WindGust::Strength(source.x,source.z,t);
                if(gustTime<0.0f&&g>0.7f) gustTime=t;
                if(calmTime<0.0f&&g<0.05f) calmTime=t;
            }
            Check(gustTime>=0.0f&&calmTime>=0.0f,"a gust front and a calm pass the source within ninety seconds");
            for(int step=0;step<20;++step) sand.Update(0.05f,calmTime,source.x,source.z);
            Check(sand.ActiveCount()==0U&&sand.ReleaseCount()==0U,"between fronts no sand blows");
            for(int step=0;step<20;++step) sand.Update(0.05f,gustTime,source.x,source.z);
            Check(sand.ReleaseCount()>=1U&&sand.ActiveCount()>=1U,"a front's passing streams sand off the source");
            const float startX=sand.Puffs()[0].x;
            Check(sand.Puffs()[0].active,"the first puff is live for the drift check");
            for(int step=0;step<10;++step) sand.Update(0.05f,gustTime,source.x,source.z);
            Check(sand.Puffs()[0].active&&sand.Puffs()[0].x<startX-0.3f,"the sand drifts downwind, westward");
            for(int step=0;step<60;++step) sand.Update(0.05f,calmTime,source.x,source.z);
            Check(sand.ActiveCount()==0U,"the sand is gone within two seconds of the front passing");
            island::BlownSandField far(terrain);
            for(int step=0;step<20;++step) far.Update(0.05f,gustTime,source.x+500.0f,source.z);
            Check(far.ReleaseCount()==0U||far.ActiveCount()>0U,"sources far from the camera stay quiet unless another lies in reach");
        }
        // The wetland mist: ten faint puffs over the seasonal wetland, a hand's breadth to a
        // knee's height over the ground, drifting westward with the wind and wrapping back
        // in at the windward edge, never leaving the wetland, breathing in density.
        {
            island::WetlandMistField mist(terrain);
            const auto inside=[&](const island::WetlandMistField& field) {
                bool ok=true;
                for(const auto& puff:field.Puffs()) {
                    if(!puff.active||island::WetlandMistField::Extent(puff.x,puff.z)>1.0001f) ok=false;
                    const float hover=puff.y-terrain.Height(puff.x,puff.z);
                    if(hover<0.05f||hover>0.7f) ok=false;
                    if(puff.density<0.1f||puff.density>1.0f||puff.size<1.5f) ok=false;
                }
                return ok;
            };
            Check(mist.Puffs().size()==island::WetlandMistField::Capacity&&inside(mist),"the wetland mist starts as faint low puffs inside the wetland");
            const float startX=mist.Puffs()[0].x;
            for(int step=0;step<200;++step) mist.Update(0.05f,static_cast<float>(step)*0.05f);
            Check(mist.Puffs()[0].x<startX-0.5f||mist.WrapCount()>=1U,"the mist drifts westward with the trade wind");
            bool breathed=false;
            float low=1.0f,high=0.0f;
            for(int step=0;step<2400;++step) {
                mist.Update(0.05f,10.0f+static_cast<float>(step)*0.05f);
                low=std::min(low,mist.Puffs()[0].density);high=std::max(high,mist.Puffs()[0].density);
                if(!inside(mist)) break;
                if(high-low>0.08f) breathed=true;
            }
            Check(inside(mist)&&mist.WrapCount()>=1U&&breathed,"over two minutes the mist wraps back in at the windward edge, stays inside the wetland and breathes");
        }
        // Hoof dust: an animal running on dry ground raises a puff behind its hooves every
        // few strides, the puffs rising, drifting downwind and gone within the second; a
        // walking animal, or one on wet ground, raises none.
        {
            island::HoofDustField dust(terrain);
            std::vector<island::HoofDustRunner> runners{{10.0f,0.0f,10.0f,0.0f,2.8f,1.0f,true}};
            for(int step=0;step<20;++step) { runners[0].x+=2.8f*0.05f; dust.Update(0.05f,0.0f,runners); }
            Check(dust.ReleaseCount()>=4U&&dust.ReleaseCount()<=8U&&dust.ActiveCount()>=3U,"a trotting boar on dry ground raises a puff every few strides");
            bool behind=true,low=true;
            for(const auto& puff:dust.Puffs()) if(puff.active) {
                if(puff.x>runners[0].x-0.2f) behind=false;
                if(puff.y-terrain.Height(puff.x,puff.z)>0.6f) low=false;
            }
            Check(behind&&low,"the dust rises low behind the hooves");
            for(int step=0;step<30;++step) dust.Update(0.05f,0.0f,{});
            Check(dust.ActiveCount()==0U,"the dust is gone within the second once the animal stops");
            island::HoofDustField walking(terrain),wet(terrain);
            std::vector<island::HoofDustRunner> slow{{10.0f,0.0f,10.0f,0.0f,0.8f,1.0f,true}};
            std::vector<island::HoofDustRunner> mud{{10.0f,0.0f,10.0f,0.0f,2.8f,1.0f,false}};
            for(int step=0;step<20;++step) { walking.Update(0.05f,0.0f,slow); wet.Update(0.05f,0.0f,mud); }
            Check(walking.ReleaseCount()==0U&&wet.ReleaseCount()==0U,"a walking animal, or one on wet ground, raises no dust");
        }
        // Rooting dust: a boar with its snout among the roots throws up puffs at the snout
        // that rise, swell and are gone within the second; a walking boar throws none.
        {
            island::RootingDustField dust(terrain);
            island::BoarState rooter{};
            rooter.x=40.0f;rooter.z=-16.0f;rooter.y=terrain.Height(40.0f,-16.0f);rooter.headingRadians=0.3f;
            rooter.behavior=island::BoarBehavior::Forage;rooter.rootingPose=1.0f;rooter.scale=1.0f;
            rooter.simulationTier=island::WildlifeSimulationTier::Full;
            std::vector<island::BoarState> boars{rooter};
            bool atSnout=true,rose=false;std::size_t most=0;
            for(int step=0;step<120;++step) {
                dust.Update(0.05f,0.0f,boars);
                most=std::max(most,dust.ActiveCount());
                for(const auto& puff:dust.Puffs()) if(puff.active) {
                    const float snoutX=rooter.x+std::cos(rooter.headingRadians)*island::RootingDustField::SnoutAhead;
                    const float snoutZ=rooter.z+std::sin(rooter.headingRadians)*island::RootingDustField::SnoutAhead;
                    if(std::hypot(puff.x-snoutX,puff.z-snoutZ)>0.6f) atSnout=false;
                    if(puff.y>terrain.Height(puff.x,puff.z)+island::RootingDustField::SnoutHeight+0.08f&&puff.density>0.5f) rose=true;
                }
            }
            Check(dust.ReleaseCount()>=10U&&most>=2U&&atSnout&&rose,"a rooting boar throws up puffs of soil at its snout that rise and are soon gone");
            boars[0].behavior=island::BoarBehavior::Wander;boars[0].rootingPose=0.0f;
            const std::uint32_t before=dust.ReleaseCount();
            for(int step=0;step<60;++step) dust.Update(0.05f,0.0f,boars);
            Check(dust.ReleaseCount()==before&&dust.ActiveCount()==0U,"a walking boar throws no dust and the old puffs clear");
        }
        // The hermit crabs' drag lines: a foraging crab crossing two metres of sand leaves a
        // chain of short segments along its path; a paused crab leaves none; lines fade.
        {
            island::ShellDragTrail drags;
            island::HermitCrabState crab{};
            crab.behavior=island::HermitCrabBehavior::Forage;crab.speed=0.05f;crab.simulationTier=island::WildlifeSimulationTier::Full;
            crab.x=10.0f;crab.z=5.0f;
            std::vector<island::HermitCrabState> crabs{crab};
            for(int step=0;step<400;++step) { crabs[0].x+=0.005f; drags.Update(0.05f,crabs); }
            bool onPath=true;
            for(const auto& segment:drags.Segments()) if(segment.active) {
                if(std::abs(segment.z0-5.0f)>0.001f||std::abs(segment.z1-5.0f)>0.001f||segment.x0<9.99f||segment.x1>12.01f||segment.x1<=segment.x0) onPath=false;
            }
            Check(drags.LaidCount()>=22U&&drags.LaidCount()<=26U&&onPath,"a foraging hermit crab lays a chain of drag segments along its path");
            crabs[0].behavior=island::HermitCrabBehavior::Pause;crabs[0].speed=0.0f;
            const std::uint32_t before=drags.LaidCount();
            for(int step=0;step<100;++step) drags.Update(0.05f,crabs);
            Check(drags.LaidCount()==before,"a paused crab drags no line");
            for(int step=0;step<1000;++step) drags.Update(0.1f,crabs);
            Check(drags.ActiveCount()==0U,"the sand levels the drag lines within their life");
            // and the rain levels them sooner: after half a minute in a downpour none are left,
            // where in the dry they still show
            island::ShellDragTrail dryDrags,wetDrags;
            wetDrags.SetRain(1.0f);
            std::vector<island::HermitCrabState> walking{crab};
            for(int step=0;step<200;++step) { walking[0].x+=0.005f; dryDrags.Update(0.05f,walking); wetDrags.Update(0.05f,walking); }
            walking[0].behavior=island::HermitCrabBehavior::Pause;walking[0].speed=0.0f;
            for(int step=0;step<300;++step) { dryDrags.Update(0.1f,walking); wetDrags.Update(0.1f,walking); }
            Check(dryDrags.ActiveCount()>0U&&wetDrags.ActiveCount()==0U,"a downpour levels the drag lines sooner");
        }
        // Fish breaking the surface: with the observer over a school (full simulation) a
        // breach is noted within a minute, on the water within two metres of a school's
        // centre, and the ring ages out within its seconds.
        {
            island::FishSchools breachers(terrain);
            const auto first=breachers.Schools()[0];
            bool nearSchool=false;std::uint32_t seen=0;
            for(int step=0;step<1200;++step) {
                breachers.Update(0.05f,first.centreX,first.centreY+3.0f,first.centreZ);
                const auto& breach=breachers.LastBreach();
                if(breach.valid&&breach.count!=seen) {
                    seen=breach.count;
                    for(const auto& school:breachers.Schools())
                        if(std::hypot(school.centreX-breach.x,school.centreZ-breach.z)<2.0f&&terrain.Height(breach.x,breach.z)<0.0f) nearSchool=true;
                }
            }
            Check(seen>=1U&&nearSchool,"a fish breaks the surface over a nearby school within a minute");
            for(int step=0;step<80;++step) breachers.Update(0.05f,1000.0f,0.0f,1000.0f);
            Check(!breachers.LastBreach().valid||breachers.LastBreach().ageSeconds<=island::FishSchools::BreachSeconds,"a breach ring ages out");
        }
        // Surf spray: an offshore rock near the observer bursts spray each time the breaker
        // phase crests at it (about every twenty seconds), the puffs rising off its seaward
        // face and gone within the second and a half; a rock on land throws none.
        {
            island::SurfSprayField spray(terrain);
            const float rockX=terrain.ShoreRadius(0.0f)+4.0f;
            island::RockPlacement sea{};sea.x=rockX;sea.z=0.0f;sea.y=terrain.Height(rockX,0.0f);sea.scaleX=1.2f;sea.scaleY=0.9f;sea.scaleZ=1.0f;sea.offshore=true;
            island::RockPlacement land=sea;land.x=rockX-12.0f;land.offshore=false;
            const std::vector<island::RockPlacement> rocks{sea,land};
            spray.SetRocks(rocks);
            Check(spray.RockCount()==1U,"only offshore rocks throw spray");
            bool nearRock=true,rose=false;
            for(int step=0;step<1200;++step) {
                spray.Update(0.05f,static_cast<float>(step)*0.05f,rockX-6.0f,0.0f);
                for(const auto& puff:spray.Puffs()) if(puff.active) {
                    if(std::hypot(puff.x-rockX,puff.z)>3.0f) nearRock=false;
                    if(puff.y>sea.y+sea.scaleY*0.6f+0.3f&&puff.density>0.5f) rose=true;
                }
            }
            Check(spray.BurstCount()>=2U&&spray.BurstCount()<=4U&&nearRock&&rose,"breakers burst spray up the offshore rock about every twenty seconds");
            const std::uint32_t before=spray.BurstCount();
            for(int step=0;step<1200;++step) spray.Update(0.05f,60.0f+static_cast<float>(step)*0.05f,1000.0f,1000.0f);
            Check(spray.BurstCount()==before,"a rock far from the observer throws no spray");
        }
        // The dolphins' blows: a dolphin rising through the surface blows once from the
        // blowhole, not again within two seconds, and one that stays below blows nothing.
        {
            island::DolphinBlowField blows;
            island::DolphinState rising{};rising.x=50.0f;rising.z=-200.0f;rising.y=-1.0f;rising.headingRadians=0.0f;rising.scale=1.0f;
            rising.behavior=island::DolphinBehavior::Surface;rising.simulationTier=island::WildlifeSimulationTier::Full;
            island::DolphinState deep=rising;deep.y=-2.0f;
            std::vector<island::DolphinState> pod{rising,deep};
            bool atBlowhole=true;
            for(int step=0;step<40;++step) {
                pod[0].y=-1.0f+static_cast<float>(step)*0.05f;   // breaks the surface at step 17
                blows.Update(0.05f,0.0f,pod);
                for(const auto& puff:blows.Puffs()) if(puff.active&&std::hypot(puff.x-(pod[0].x+island::DolphinBlowField::BlowholeAhead),puff.z-pod[0].z)>0.6f) atBlowhole=false;
            }
            Check(blows.BlowCount()==1U&&atBlowhole,"a dolphin breaking the surface blows once from the blowhole");
            pod[0].y=-1.0f;blows.Update(0.05f,0.0f,pod);pod[0].y=0.2f;blows.Update(0.05f,0.0f,pod);
            Check(blows.BlowCount()==1U,"a second surfacing within two seconds blows nothing more");
            for(int step=0;step<60;++step) blows.Update(0.05f,0.0f,pod);
            Check(blows.ActiveCount()==0U,"a blow is gone within the second");
        }
        // Coconut crabs take the coconuts: a nut shaken down at a crab's home draws its
        // next forage; it walks there, works the nut with raised claws and the nut is gone.
        {
            island::CoconutCrabGroup crabs(terrain);
            const auto home=crabs.Homes()[0];
            island::FallingFruitField nuts(terrain);
            island::PalmPlacement palm{home.x,terrain.Height(home.x,home.z),home.z,0.0f,1.0f,30.0f,0.0f,1U,island::PalmCondition::Living};
            nuts.ShakeCoconut(palm,1U);
            for(int step=0;step<200;++step) nuts.Update(0.05f,{});
            bool lying=false;
            for(const auto& f:nuts.Fruit()) if(f.active&&f.variant==island::FallingFruitField::CoconutVariant&&f.restSeconds>=0.0f&&!f.rolling) lying=true;
            Check(lying,"a coconut lies at the crab's home");
            bool worked=false;
            for(int step=0;step<12000&&crabs.WorkCount()==0U;++step) {
                crabs.SetLyingCoconuts(nuts.Fruit());
                crabs.Update(0.05f,home.x+40.0f,terrain.Height(home.x+40.0f,home.z)+1.5f,home.z);
                for(const std::size_t index:crabs.EatenCoconuts()) nuts.Consume(index);
                for(const auto& crab:crabs.Crabs()) if(crab.working&&crab.clawPose>0.5f) worked=true;
                nuts.Update(0.05f,{});
            }
            Check(crabs.WorkCount()>=1U&&worked&&nuts.ConsumedCount()>=1U,"a coconut crab walks to the nut, works it with raised claws and finishes it within ten minutes");
        }
        // Ant trails to fallen fruit: a fruit lying two metres past a ground trail's food end
        // bends the trail to it, an ant reaches the fruit within a minute, and with the fruit
        // gone the trail returns to its line.
        {
            const island::JungleLayout antJungle(terrain);
            island::AntTrails ants(terrain,antJungle);
            std::size_t groundTrail=island::AntTrails::TrailCount;
            float endX=0,endZ=0;
            for(std::size_t t=0;t<island::AntTrails::TrailCount&&groundTrail==island::AntTrails::TrailCount;++t)
                if(ants.FoodEnd(t,endX,endZ)) groundTrail=t;
            Check(groundTrail<island::AntTrails::TrailCount,"a ground ant trail has a food end");
            const float lineX=endX,lineZ=endZ;
            island::FallingFruit fruit{};fruit.x=endX+2.0f;fruit.z=endZ;fruit.y=terrain.Height(fruit.x,fruit.z);fruit.restSeconds=5.0f;fruit.variant=2U;fruit.active=true;
            std::vector<island::FallingFruit> lying{fruit};
            ants.SetLyingFruit(lying);
            Check(ants.FoodTrailCount()==1U&&ants.FoodEnd(groundTrail,endX,endZ)&&std::hypot(endX-fruit.x,endZ-fruit.z)<0.01f,"the trail's food end moves to the fruit");
            bool reached=false;
            for(int step=0;step<1200&&!reached;++step) {
                ants.Update(0.05f,fruit.x,fruit.y+1.0f,fruit.z);
                for(const auto& ant:ants.Ants()) if(ant.trailIndex==groundTrail&&std::hypot(ant.x-fruit.x,ant.z-fruit.z)<0.4f) reached=true;
            }
            Check(reached,"an ant reaches the fruit within a minute");
            ants.SetLyingFruit({});
            Check(ants.FoodTrailCount()==0U&&ants.FoodEnd(groundTrail,endX,endZ)&&std::hypot(endX-lineX,endZ-lineZ)<0.01f,"with the fruit gone the trail returns to its line");
            // The antennae sweep: watched from six metres, within half a minute a pausing ant
            // of the first trail sweeps its antennae at full with the sweep advancing, and a
            // walking ant's antennae lie at their set.
            island::AntTrails sweepers(terrain,antJungle);
            const auto& firstTrail=sweepers.Trails()[0];
            float sweepPeak=0.0f,swept=0.0f;
            for(int step=0;step<600;++step) {
                sweepers.Update(0.05f,firstTrail.x+6.0f,firstTrail.y+1.0f,firstTrail.z);
                for(const auto& ant:sweepers.Ants()) {
                    if(ant.simulationTier!=island::WildlifeSimulationTier::Full||ant.behavior!=island::AntBehavior::Pause) continue;
                    sweepPeak=std::max(sweepPeak,ant.antennaSweep);
                    if(ant.antennaSweep>0.9f) swept=std::max(swept,ant.sweepPhase);
                }
            }
            Check(sweepers.SweepCount()>=1U&&sweepPeak>0.9f&&swept>1.0f,"a pausing ant does not sweep its antennae");
            // Meetings: through another minute ants of the first trail meet head to head,
            // and at a meeting both are paused within reach of each other.
            bool metPaused=false;
            for(int step=0;step<1200&&!metPaused;++step) {
                sweepers.Update(0.05f,firstTrail.x+6.0f,firstTrail.y+1.0f,firstTrail.z);
                const auto ants=sweepers.Ants();
                for(std::size_t a=0;a<island::AntTrails::PerTrail&&!metPaused;++a)
                    for(std::size_t b=a+1;b<island::AntTrails::PerTrail;++b)
                        if(ants[a].behavior==island::AntBehavior::Pause&&ants[b].behavior==island::AntBehavior::Pause
                           &&std::hypot(ants[a].x-ants[b].x,ants[a].z-ants[b].z)<island::AntTrails::MeetReach*2.0f) { metPaused=true; break; }
            }
            Check(sweepers.MeetCount()>=1U&&metPaused,"ants meeting head to head on the trail do not pause together to touch antennae");
        }
        // The plunge pool has its own dragonflies: the last three homes sit on the pool.
        {
            island::DragonflySwarm poolFlies(terrain);
            const island::StreamCourse poolCourse(terrain);
            const auto& pool=poolCourse.PoolStation();
            bool onPool=true;
            for(std::size_t i=island::DragonflySwarm::Count-3;i<island::DragonflySwarm::Count;++i) {
                const auto& home=poolFlies.Homes()[i];
                if(std::hypot(home.x-pool.x,home.z-pool.z)>0.01f||home.radius!=island::DragonflySwarm::PoolHabitatRadius) onPool=false;
            }
            Check(onPool,"three dragonflies have the plunge pool for their habitat");
        }
        // Two frogs keep the plunge pool's banks, one either side of the pool station.
        {
            island::FrogPopulation poolFrogs(terrain);
            const island::StreamCourse poolCourse(terrain);
            const auto& pool=poolCourse.PoolStation();
            const auto& a=poolFrogs.Homes()[island::FrogPopulation::Count-2];
            const auto& b=poolFrogs.Homes()[island::FrogPopulation::Count-1];
            Check(std::abs(std::hypot(a.x-pool.x,a.z-pool.z)-2.4f)<0.01f&&std::abs(std::hypot(b.x-pool.x,b.z-pool.z)-2.4f)<0.01f
                  &&std::hypot(a.x-b.x,a.z-b.z)>4.7f,"two frogs have the plunge pool's banks for their homes");
        }
        // The reef herons wade the shallows: each stands in the wading band, works its stretch
        // of shore, freezes and strikes at fish; a walker close by flushes it off along the
        // shore to land in the shallows again; at night they stand hunched at rest.
        {
            island::ReefHeronGroup herons(terrain);
            bool inBand=true;
            for(const auto& heron:herons.Herons()) {
                const float depth=-terrain.Height(heron.x,heron.z);
                if(depth<island::ReefHeronGroup::MinimumDepth-0.02f||depth>island::ReefHeronGroup::MaximumDepth+0.02f) inBand=false;
            }
            Check(inBand,"each reef heron starts in the wading band of the lagoon's edge");
            float startX=herons.Herons()[1].x,startZ=herons.Herons()[1].z;
            float wandered=0.0f;
            for(int step=0;step<1200;++step) {
                herons.Update(0.05f,0.0f,1000.0f,0.0f);
                for(const auto& heron:herons.Herons()) {
                    if(heron.behavior==island::ReefHeronBehavior::Fly) continue;
                    const float depth=-terrain.Height(heron.x,heron.z);
                    if(depth<island::ReefHeronGroup::MinimumDepth-0.03f||depth>island::ReefHeronGroup::MaximumDepth+0.03f) inBand=false;
                }
                wandered=std::max(wandered,std::hypot(herons.Herons()[1].x-startX,herons.Herons()[1].z-startZ));
            }
            Check(inBand&&wandered>2.0f&&herons.StrikeCount()>0U,
                  "the herons stalk along the shore in the wading band and strike at fish");
            {
                // the strike's fleck and ring: noted where the bill went in, in the water, and
                // gone within its time
                const auto& strike=herons.LastStrike();
                Check(strike.count==herons.StrikeCount()&&terrain.Height(strike.x,strike.z)<0.0f,
                      "a heron's strike is noted in the water in front of it");
                island::ReefHeronGroup quiet(terrain);
                bool expired=true;
                for(int step=0;step<2000&&quiet.StrikeCount()==0U;++step) quiet.Update(0.05f,0.0f,1000.0f,0.0f);
                for(int step=0;step<40;++step) quiet.Update(0.05f,0.0f,1000.0f,0.0f);
                if(quiet.LastStrike().valid&&quiet.LastStrike().ageSeconds>island::ReefHeronGroup::StrikeSeconds) expired=false;
                Check(quiet.StrikeCount()>0U&&expired,"a strike's splash lasts only its time");
            }
            const auto before=herons.Herons()[2];
            const std::uint32_t flushes=herons.FlushCount();
            for(int step=0;step<10;++step) herons.Update(0.05f,before.x+3.0f,before.y+1.6f,before.z);
            Check(herons.FlushCount()>flushes&&herons.Herons()[2].behavior==island::ReefHeronBehavior::Fly
                      &&herons.LastFlush().count==herons.FlushCount()
                      &&std::hypot(herons.LastFlush().x-before.x,herons.LastFlush().z-before.z)<1.0f,
                  "a walker three metres off flushes the heron, noted where it took off");
            bool landed=false;
            for(int step=0;step<600&&!landed;++step) {
                herons.Update(0.05f,before.x+3.0f,before.y+1.6f,before.z);
                const auto& heron=herons.Herons()[2];
                landed=heron.behavior!=island::ReefHeronBehavior::Fly
                    &&std::hypot(heron.x-before.x,heron.z-before.z)>20.0f
                    &&-terrain.Height(heron.x,heron.z)>=island::ReefHeronGroup::MinimumDepth-0.03f;
            }
            Check(landed,"the flushed heron lands in the shallows further along the shore");
            {
                // something else coming near flushes a heron too (the resident on the shore,
                // the quad...), each from its own distance
                island::ReefHeronGroup shy(terrain);
                const auto target=shy.Herons()[0];
                const island::ReefHeronGroup::Disturber far{target.x+9.0f,target.z,6.0f};
                shy.SetDisturbers(std::span(&far,1));
                for(int step=0;step<10;++step) shy.Update(0.05f,0.0f,1000.0f,0.0f);
                const bool stayed=shy.FlushCount()==0U;
                const island::ReefHeronGroup::Disturber close{target.x+5.0f,target.z,6.0f};
                shy.SetDisturbers(std::span(&close,1));
                for(int step=0;step<10;++step) shy.Update(0.05f,0.0f,1000.0f,0.0f);
                Check(stayed&&shy.FlushCount()>0U&&shy.Herons()[0].behavior==island::ReefHeronBehavior::Fly,
                      "a heron stays for a disturber beyond its distance and flushes from one within it");
            }
            herons.SetNightness(1.0f);
            for(int step=0;step<200;++step) herons.Update(0.05f,0.0f,1000.0f,0.0f);
            bool resting=true;
            for(const auto& heron:herons.Herons())
                if(heron.behavior!=island::ReefHeronBehavior::Rest||heron.rest<0.9f) resting=false;
            Check(resting,"at night the reef herons stand hunched at rest");
            {
                // with roosts a few metres up near each, the herons fly up at dusk and stand
                // hunched there, one to a roost, and at dawn fly back down to the shallows
                island::ReefHeronGroup roosting(terrain);
                std::vector<island::ReefHeronGroup::Roost> roosts;
                for(const auto& heron:roosting.Herons()) {
                    roosts.push_back({heron.x*0.97f,4.5f,heron.z*0.97f});
                    roosts.push_back({heron.x*0.96f,5.0f,heron.z*0.96f});
                }
                roosting.SetRoosts(roosts);
                roosting.SetNightness(1.0f);
                for(int step=0;step<600;++step) roosting.Update(0.05f,0.0f,1000.0f,0.0f);
                bool up=roosting.RoostedCount()==island::ReefHeronGroup::Count;
                for(const auto& heron:roosting.Herons())
                    if(heron.y<4.0f||heron.behavior!=island::ReefHeronBehavior::Rest) up=false;
                for(std::size_t a=0;a<island::ReefHeronGroup::Count;++a)
                    for(std::size_t b=a+1;b<island::ReefHeronGroup::Count;++b)
                        if(std::hypot(roosting.Herons()[a].x-roosting.Herons()[b].x,
                                      roosting.Herons()[a].z-roosting.Herons()[b].z)<0.1f) up=false;
                roosting.SetNightness(0.0f);
                for(int step=0;step<900;++step) roosting.Update(0.05f,0.0f,1000.0f,0.0f);
                bool down=roosting.RoostedCount()==0U;
                for(const auto& heron:roosting.Herons()) {
                    if(heron.behavior==island::ReefHeronBehavior::Fly) { down=false; continue; }
                    const float depth=-terrain.Height(heron.x,heron.z);
                    if(depth<island::ReefHeronGroup::MinimumDepth-0.03f) down=false;
                }
                Check(up&&down,"the herons roost up off the shore at night, one to a perch, and come back down to wade at dawn");
            }
        }
        // A settled stingray bursts away from the swimmer: with the observer two metres from
        // the settled ray it glides off at once, the burst is noted where it lay, and a ring of
        // sand puffs rises there and is gone within two seconds.
        {
            island::StingrayGroup rays(terrain);
            std::size_t settled=island::StingrayGroup::Count;
            for(std::size_t i=0;i<rays.Stingrays().size();++i) if(rays.Stingrays()[i].behavior==island::StingrayBehavior::Settle) { settled=i; break; }
            Check(settled<island::StingrayGroup::Count,"a stingray starts settled on the sand");
            const auto lay=rays.Stingrays()[settled];
            for(int step=0;step<4;++step) rays.Update(0.05f,lay.x+2.0f,lay.y,lay.z);
            Check(rays.Stingrays()[settled].behavior==island::StingrayBehavior::Glide&&rays.LastBurst().count==1U
                  &&std::hypot(rays.LastBurst().x-lay.x,rays.LastBurst().z-lay.z)<0.5f,"the settled ray bursts into a glide when the swimmer comes within four metres");
            island::StingraySandField sand;
            sand.Update(0.05f,rays.LastBurst());
            bool ringed=true;
            for(const auto& puff:sand.Puffs()) if(puff.active&&std::hypot(puff.x-lay.x,puff.z-lay.z)>1.0f) ringed=false;
            Check(sand.ActiveCount()==island::StingraySandField::PuffsPerBurst&&ringed,"a ring of sand puffs rises where the ray lay");
            for(int step=0;step<50;++step) sand.Update(0.05f,rays.LastBurst());
            Check(sand.ActiveCount()==0U,"the sand settles again within two seconds");
        }
        // The shark passing a settled ray within nine metres, with no swimmer about, bursts
        // it from the sand as the swimmer does; the shark twenty metres off leaves it lying.
        {
            island::StingrayGroup rays(terrain);
            std::size_t settled=island::StingrayGroup::Count;
            for(std::size_t i=0;i<rays.Stingrays().size();++i) if(rays.Stingrays()[i].behavior==island::StingrayBehavior::Settle) { settled=i; break; }
            Check(settled<island::StingrayGroup::Count,"a stingray starts settled for the shark check");
            const auto lay=rays.Stingrays()[settled];
            rays.SetThreat(lay.x+20.0f,lay.z,true);
            for(int step=0;step<4;++step) rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z);
            Check(rays.Stingrays()[settled].behavior==island::StingrayBehavior::Settle&&rays.ThreatBurstCount()==0U,"the shark twenty metres off leaves the settled ray lying");
            rays.SetThreat(lay.x+6.0f,lay.z,true);
            for(int step=0;step<4;++step) rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z);
            Check(rays.Stingrays()[settled].behavior==island::StingrayBehavior::Glide&&rays.ThreatBurstCount()==1U&&rays.LastBurst().count==1U,
                  "the shark six metres off bursts the settled ray from the sand");
            float away=0.0f;
            for(int step=0;step<40;++step) { rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z); away=std::hypot(rays.Stingrays()[settled].x-(lay.x+6.0f),rays.Stingrays()[settled].z-lay.z); }
            Check(away>6.5f,"the burst ray glides away from the shark");
        }
        // A settled ray buries itself: two seconds into its settle it flicks sand over its
        // back (a ring of puffs where it lies) and the disc takes the sand over a few
        // seconds; the swimmer coming close bursts it out and the sand sheds at once.
        {
            island::StingrayGroup rays(terrain);
            std::size_t settled=island::StingrayGroup::Count;
            for(std::size_t i=0;i<rays.Stingrays().size();++i) if(rays.Stingrays()[i].behavior==island::StingrayBehavior::Settle) { settled=i; break; }
            Check(settled<island::StingrayGroup::Count,"a stingray starts settled for the bury check");
            const auto lay=rays.Stingrays()[settled];
            for(int step=0;step<30;++step) rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z);
            Check(rays.BuryCount()==0U&&rays.Stingrays()[settled].buryAmount==0.0f,"a ray settled a second and a half has not flicked yet");
            for(int step=0;step<20;++step) rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z);
            Check(rays.BuryCount()==1U&&rays.LastFlick().count==1U&&rays.BuriedCount()==1U
                  &&std::hypot(rays.LastFlick().x-lay.x,rays.LastFlick().z-lay.z)<0.5f,"two and a half seconds in the settled ray has flicked sand over its back where it lies");
            island::StingraySandField flickSand;
            flickSand.Update(0.05f,rays.LastFlick());
            Check(flickSand.ActiveCount()==island::StingraySandField::PuffsPerBurst,"the flick raises a ring of sand puffs");
            for(int step=0;step<60;++step) rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z);
            Check(rays.Stingrays()[settled].buryAmount>0.9f&&rays.Stingrays()[settled].behavior==island::StingrayBehavior::Settle,"a few seconds on the ray lies fully buried");
            // The margin ripple: the settled ray's edges ripple at full with the wave's phase
            // advancing; a gliding ray's edges lie still.
            float phaseA=rays.Stingrays()[settled].ripplePhase;
            rays.Update(0.05f,lay.x+30.0f,lay.y,lay.z);
            bool gliderStill=true;
            for(const auto& ray:rays.Stingrays()) if(ray.behavior==island::StingrayBehavior::Glide&&ray.speed>0.3f&&ray.marginRipple>0.3f) gliderStill=false;
            Check(rays.Stingrays()[settled].marginRipple>0.9f&&rays.Stingrays()[settled].ripplePhase!=phaseA&&rays.RipplingCount()>=1U&&gliderStill,
                  "a settled ray does not ripple its wing margins, or a gliding one does");
            for(int step=0;step<12;++step) rays.Update(0.05f,lay.x+2.0f,lay.y,lay.z);
            Check(rays.Stingrays()[settled].behavior==island::StingrayBehavior::Glide&&rays.Stingrays()[settled].buryAmount<0.3f&&rays.BuriedCount()==0U,
                  "the swimmer at two metres bursts the buried ray out and the sand sheds within half a second");
        }
        // The grass walkers: of nine candidates the six nearest the camera within reach are
        // taken in distance order, the far and the strengthless left out; a standing body
        // presses lightly and a trotting one fully.
        {
            std::vector<island::GrassWalker> candidates;
            for(int i=0;i<9;++i) candidates.push_back({static_cast<float>(i)*4.0f,0.0f,0.5f,1.0f});
            candidates[3].strength=0.0f;          // a body that is not there
            candidates.push_back({100.0f,0.0f,0.5f,1.0f});   // beyond reach
            std::array<island::GrassWalker,island::GrassWalkers::Slots> chosen{};
            const std::size_t taken=island::GrassWalkers::Select(candidates,0.0f,0.0f,chosen);
            bool ordered=true;
            for(std::size_t i=1;i<taken;++i) if(chosen[i].x<chosen[i-1].x) ordered=false;
            Check(taken==6U&&ordered&&chosen[0].x==0.0f&&chosen[5].x==24.0f,"the six nearest bodies in reach press the grass, nearest first");
            Check(island::GrassWalkers::Strength(0.0f)>0.3f&&island::GrassWalkers::Strength(0.0f)<0.4f&&island::GrassWalkers::Strength(1.0f)==1.0f,
                  "a standing body presses the grass lightly and a moving one fully");
        }
        // Butterflies scatter from a passing animal: a body one metre from a butterfly, with no
        // walker about, sends it fluttering away from the body.
        {
            island::ButterflyMeadow scared(terrain);
            const auto fly=scared.Butterflies()[0];
            const std::vector<island::GrassWalker> bodies{{fly.x+1.0f,fly.z,0.5f,1.0f}};
            scared.SetWalkers(bodies);
            for(int step=0;step<3;++step) scared.Update(0.05f,fly.x,fly.y+1.0f,fly.z+40.0f);
            const auto& after=scared.Butterflies()[0];
            const float away=std::remainder(after.headingRadians-std::atan2(0.0f,-1.0f),6.2831853f);
            Check(scared.ScatterCount()>=1U&&after.behavior==island::ButterflyBehavior::Flutter&&std::abs(away)<1.2f,
                  "a butterfly scatters away from a passing animal");
        }
        // A dove foraging on the ground flushes from a passing animal: the seeded ground dove,
        // with a body two metres off and no walker about, departs at once.
        {
            island::BirdFlock doves(terrain);
            // The ground dove is seeded when the perches are installed.
            const std::vector<island::BirdPerchSite> doveSites{{20.0f,terrain.Height(20.0f,20.0f)+2.0f,20.0f,0.3f,island::BirdPerchKind::Wreck}};
            doves.SetPerchSites(doveSites);
            const auto dove=doves.Birds()[14U];
            Check(dove.behavior==island::BirdBehavior::GroundForage,"a dove starts foraging on the ground");
            const std::vector<island::GrassWalker> bodies{{dove.x+2.0f,dove.z,0.6f,1.0f}};
            doves.SetWalkers(bodies);
            doves.Update(0.05f,std::numeric_limits<float>::quiet_NaN(),0.0f,0.0f);
            Check(doves.GroundFlushCount()==1U&&doves.Birds()[14U].behavior==island::BirdBehavior::GroundDepart,
                  "a dove on the ground flushes from a passing animal");
        }
        // The small fauna scatter from a passing animal: a body a metre from a rodent, a
        // resting gecko and a skink, with no walker about, sends each off away from the body.
        {
            island::RodentPopulation rodents(terrain);
            const auto rodent=rodents.Rodents()[0];
            const std::vector<island::GrassWalker> nearRodent{{rodent.x+1.0f,rodent.z,0.6f,1.0f}};
            rodents.SetWalkers(nearRodent);
            // The observer stands twelve metres off: near enough for full simulation, too far to frighten.
            for(int step=0;step<60;++step) rodents.Update(0.05f,rodent.x+12.0f,rodent.y+1.0f,rodent.z);
            Check(rodents.WalkerScatterCount()>=1U,"a rodent scurries from a passing animal");
            // A snake's head near a rodent: it bolts away from the snake (toward its home)
            // and goes to ground for six seconds or more; the walker twelve metres off plays
            // no part.
            island::RodentPopulation hunted(terrain);
            const auto prey=hunted.Rodents()[0];
            const auto den=hunted.Homes()[0];
            float ax=prey.x-den.x,az=prey.z-den.z;
            const float along=std::hypot(ax,az);
            if(along>0.05f) { ax/=along; az/=along; } else { ax=1.0f; az=0.0f; }
            std::vector<island::SnakeState> snakes(1);
            for(auto& point:snakes[0].body) point={prey.x+ax*2.5f,prey.y,prey.z+az*2.5f};
            hunted.SetSnakes(snakes);
            for(int step=0;step<10;++step) hunted.Update(0.05f,prey.x+12.0f,prey.y+1.0f,prey.z);
            Check(hunted.SnakeScatterCount()>=1U&&hunted.Rodents()[0].behavior==island::RodentBehavior::Scurry,
                  "a snake's head two and a half metres off sends the rodent bolting");
            const auto bolted=hunted.Rodents()[0];
            Check(std::hypot(bolted.x-snakes[0].body.front().x,bolted.z-snakes[0].body.front().z)>2.6f,"it bolts away from the snake");
            for(int step=0;step<50;++step) hunted.Update(0.05f,prey.x+12.0f,prey.y+1.0f,prey.z);
            Check(hunted.Rodents()[0].behavior==island::RodentBehavior::Hidden,"three seconds on the rodent has gone to ground");
            for(int step=0;step<50;++step) hunted.Update(0.05f,prey.x+12.0f,prey.y+1.0f,prey.z);
            Check(hunted.Rodents()[0].behavior==island::RodentBehavior::Hidden,"five and a half seconds on it is still hidden from the snake");
            island::GeckoPopulation geckos(terrain);
            std::size_t resting=geckos.Geckos().size();
            for(std::size_t i=0;i<geckos.Geckos().size();++i) if(geckos.Geckos()[i].behavior==island::GeckoBehavior::Rest) { resting=i; break; }
            Check(resting<geckos.Geckos().size(),"a gecko starts at rest");
            const auto gecko=geckos.Geckos()[resting];
            const std::vector<island::GrassWalker> nearGecko{{gecko.x+1.0f,gecko.z,0.6f,1.0f}};
            geckos.SetWalkers(nearGecko);
            for(int step=0;step<60;++step) geckos.Update(0.05f,gecko.x+12.0f,gecko.y+1.0f,gecko.z);
            Check(geckos.WalkerScatterCount()>=1U,"a resting gecko scurries from a passing animal");
            island::SkinkPopulation skinks(terrain);
            const auto skink=skinks.Skinks()[0];
            const std::vector<island::GrassWalker> nearSkink{{skink.x+1.0f,skink.z,0.6f,1.0f}};
            skinks.SetWalkers(nearSkink);
            for(int step=0;step<60;++step) skinks.Update(0.05f,skink.x+12.0f,skink.y+1.0f,skink.z);
            Check(skinks.WalkerScatterCount()>=1U,"a skink flees a passing animal");
        }
        // The boars' wallow sites: at least one, each on drainage ground at the bank, distinct.
        {
            island::BoarHerd herd(terrain);
            const auto sites=herd.WallowSites();
            bool onBank=true,distinct=true;
            for(std::size_t i=0;i<sites.size();++i) {
                if(!herd.IsValidGround(sites[i].x,sites[i].z,true)) onBank=false;
                for(std::size_t j=0;j<i;++j) if(std::hypot(sites[i].x-sites[j].x,sites[i].z-sites[j].z)<1.5f) distinct=false;
            }
            Check(!sites.empty()&&sites.size()<=island::BoarHerd::BoarCount&&onBank&&distinct,"the boars' wallow sites are distinct bank points");
        }
        // Fallen twigs: dozens under the crowns, every one on forest floor off the path and
        // the stream, within three metres of a canopy trunk, deterministic.
        {
            const island::JungleLayout twigJungle(terrain);
            island::ForestTwigLayout twigs(terrain,twigJungle),repeat(terrain,twigJungle);
            bool floor=true,nearTrunk=true,same=twigs.Twigs().size()==repeat.Twigs().size();
            for(std::size_t i=0;i<twigs.Twigs().size();++i) {
                const auto& twig=twigs.Twigs()[i];
                if(!island::ForestTwigLayout::IsForestFloor(terrain,twig.x,twig.z)) floor=false;
                float best=1e9f;
                for(const auto& tree:twigJungle.Canopy()) best=std::min(best,std::hypot(tree.x-twig.x,tree.z-twig.z));
                if(best>3.05f) nearTrunk=false;
                if(same&&(twig.x!=repeat.Twigs()[i].x||twig.z!=repeat.Twigs()[i].z)) same=false;
            }
            Check(twigs.Twigs().size()>=60U&&floor&&nearTrunk&&same,"fallen twigs lie on the forest floor near the trunks, deterministically");
        }
        // An octopus in the open startled by the swimmer inks: the swimmer a metre from the
        // foraging octopus sends it to its den and notes an inking where it was; a cloud of
        // four puffs forms there and thins away within three seconds.
        {
            island::OctopusGroup octopuses(terrain);
            const auto open=octopuses.Octopuses()[0];
            Check(open.behavior!=island::OctopusBehavior::Den,"the first octopus starts in the open");
            for(int step=0;step<4;++step) octopuses.Update(0.05f,open.x+1.0f,open.y,open.z);
            Check(octopuses.LastInk().count==1U&&std::hypot(octopuses.LastInk().x-open.x,octopuses.LastInk().z-open.z)<0.5f
                  &&octopuses.Octopuses()[0].behavior==island::OctopusBehavior::Den,"the startled octopus jets for its den and inks once");
            // The jet itself: the startled octopus has its mantle drawn in within a fifth of a
            // second, and eases off within a second and a half.
            Check(octopuses.JetCount()>=1U&&octopuses.Octopuses()[0].jet>0.9f&&octopuses.JettingCount()>=1U,"the startled octopus does not jet");
            for(int step=0;step<30;++step) octopuses.Update(0.05f,open.x+1.0f,open.y,open.z);
            Check(octopuses.Octopuses()[0].jet<0.1f&&octopuses.JettingCount()==0U,"the jet does not ease off");
            // The probe: a foraging octopus left alone reaches an arm out within a quarter
            // minute, the probe at full, draws it back, and the next probe takes another arm.
            island::OctopusGroup probers(terrain);
            Check(probers.Octopuses()[0].behavior==island::OctopusBehavior::Forage,"the first octopus starts foraging for the probe check");
            float probePeak=0.0f; bool drawnBack=false; std::uint8_t firstArm=255U,secondArm=255U;
            const auto forager=probers.Octopuses()[0];
            for(int step=0;step<1200&&secondArm==255U;++step) {
                probers.Update(0.05f,forager.x+20.0f,forager.y,forager.z);
                const auto& o=probers.Octopuses()[0];
                probePeak=std::max(probePeak,o.probe);
                if(o.probe>0.9f) { if(firstArm==255U) firstArm=o.probeArm; else if(o.probeArm!=firstArm) secondArm=o.probeArm; }
                if(probePeak>0.9f&&o.probe<0.05f) drawnBack=true;
            }
            Check(probers.ProbeCount()>=2U&&probePeak>0.9f&&drawnBack&&firstArm!=255U&&secondArm!=255U,"a foraging octopus does not probe with one arm after another");
            island::OctopusInkField ink;
            ink.Update(0.05f,octopuses.LastInk());
            bool clouded=true;
            for(const auto& puff:ink.Puffs()) if(puff.active&&std::hypot(puff.x-open.x,puff.z-open.z)>0.6f) clouded=false;
            Check(ink.ActiveCount()==island::OctopusInkField::PuffsPerCloud&&clouded,"a cloud of ink forms where the octopus was");
            for(int step=0;step<70;++step) ink.Update(0.05f,octopuses.LastInk());
            Check(ink.ActiveCount()==0U,"the ink thins away within three seconds");
        }
        // Camouflage: an octopus lying still in its den takes the sand's tones within a few
        // seconds; startled into the open it flashes back to its own colours at once.
        {
            island::OctopusGroup hidden(terrain);
            const auto& denned=hidden.Octopuses()[2];
            Check(denned.behavior==island::OctopusBehavior::Den&&denned.camouflageAmount==0.0f,"the third octopus starts in its den in its own colours");
            const float farX=denned.x+60.0f;
            for(int step=0;step<140;++step) hidden.Update(0.05f,farX,denned.y,denned.z);
            Check(denned.camouflageAmount>.9f&&hidden.CamouflagedCount()>=1U,"an octopus lying still in its den is matched to the sand within seven seconds");
            hidden.Update(0.05f,denned.x+1.0f,denned.y,denned.z);
            for(int step=0;step<12;++step) hidden.Update(0.05f,denned.x+1.0f,denned.y,denned.z);
            Check(denned.camouflageAmount<.05f,"a startled octopus flashes back to its own colours within a second");
        }
        // A crocodile on the bank coming alert at the walker hisses once, at its jaws, and gapes
        // while it does; the hiss is not given again while the walker stays, and the walker
        // far off gives none.
        {
            island::CrocodileGroup wary(terrain);
            std::size_t basking=island::CrocodileGroup::CrocodileCount;
            for(std::size_t i=0;i<wary.Crocodiles().size();++i) if(wary.Crocodiles()[i].behavior==island::CrocodileBehavior::Bask&&wary.Crocodiles()[i].submerged<.2f) { basking=i; break; }
            Check(basking<island::CrocodileGroup::CrocodileCount,"a crocodile starts basking on the bank for the hiss check");
            const auto lie=wary.Crocodiles()[basking];
            for(int step=0;step<20;++step) wary.Update(0.05f,lie.x+40.0f,lie.y+1.6f,lie.z);
            Check(wary.LastHiss().count==0U,"the walker forty metres off draws no hiss");
            for(int step=0;step<20;++step) wary.Update(0.05f,lie.x+20.0f,lie.y+1.6f,lie.z);
            const std::uint32_t noted=wary.LastHiss().count;
            bool atJaws=false;
            for(const auto& croc:wary.Crocodiles()) if(std::hypot(wary.LastHiss().x-croc.x,wary.LastHiss().z-croc.z)<2.0f) atJaws=true;
            Check(noted>=1U&&noted<=island::CrocodileGroup::CrocodileCount&&wary.LastHiss().valid&&atJaws,
                  "a crocodile coming alert at the walker twenty metres off hisses, at its jaws");
            Check(wary.Crocodiles()[basking].jawOpen>.2f,"the hissing crocodile gapes");
            for(int step=0;step<100;++step) wary.Update(0.05f,lie.x+20.0f,lie.y+1.6f,lie.z);
            Check(wary.LastHiss().count==noted,"the hiss is not given again while the walker stays");
            // A basking crocodile gapes: a few seconds into its lie-up the jaws open to the
            // gape and hold there, then close again within the bask; the swimming one never
            // gapes.
            island::CrocodileGroup gaper(terrain);
            std::size_t lying=island::CrocodileGroup::CrocodileCount;
            for(std::size_t i=0;i<gaper.Crocodiles().size();++i) if(gaper.Crocodiles()[i].behavior==island::CrocodileBehavior::Bask&&gaper.Crocodiles()[i].submerged<.2f) { lying=i; break; }
            Check(lying<island::CrocodileGroup::CrocodileCount,"a crocodile starts basking on the bank for the gape check");
            const auto bank=gaper.Crocodiles()[lying];
            float widest=0.0f; int opened=-1;
            for(int step=0;step<240;++step) {
                gaper.Update(0.05f,bank.x+60.0f,bank.y+1.6f,bank.z);
                const float jaw=gaper.Crocodiles()[lying].jawOpen;
                if(jaw>widest) widest=jaw;
                if(opened<0&&jaw>.4f) opened=step;
            }
            Check(gaper.GapeCount()>=1U&&opened>=60&&opened<220,
                  "a basking crocodile gapes a few seconds into its lie-up, the jaws reaching the gape within eleven seconds");
            Check(widest<.6f,"the gape is held short of the hiss's gape");
            bool swimmerGaped=false;
            for(int step=0;step<600;++step) {
                gaper.Update(0.05f,bank.x+60.0f,bank.y+1.6f,bank.z);
                for(const auto& croc:gaper.Crocodiles()) if(croc.behavior==island::CrocodileBehavior::Swim&&croc.jawOpen>.3f) swimmerGaped=true;
            }
            Check(!swimmerGaped,"a swimming crocodile never gapes");
            Check(gaper.Crocodiles()[lying].jawOpen<.3f||gaper.GapingCount()>=1U,
                  "forty seconds on, the first gape has closed unless a new lie-up has begun one");
            // The blink: through those forty-two seconds the basking crocodile has blinked at
            // least three times; a blink shuts the eyes fully and opens them within a second.
            island::CrocodileGroup blinker(terrain);
            float shut=0.0f; int shutFrames=0,longestShut=0; bool reopened=false;
            for(int step=0;step<400;++step) {
                blinker.Update(0.05f,bank.x+60.0f,bank.y+1.6f,bank.z);
                const float blink=blinker.Crocodiles()[lying].blink;
                shut=std::max(shut,blink);
                shutFrames=blink>.5f?shutFrames+1:0; longestShut=std::max(longestShut,shutFrames);
                if(shut>.9f&&blink<.05f) reopened=true;
            }
            Check(blinker.BlinkCount()>=3U&&shut>.9f&&longestShut<=20&&reopened,"a crocodile blinks every several seconds, the eyes shut for a moment and open again");
        }
        {
            // The crocodile's hiss PCM: deterministic, two variants, 1.2 s, silent at both
            // ends, low (few zero crossings) and loud in its middle.
            const auto hissA=island::GenerateCrocodileHiss(0),hissB=island::GenerateCrocodileHiss(1);
            const std::size_t frames=static_cast<std::size_t>(22050*1.2f);
            int crossings=0;int peak=0;
            for(std::size_t i=8820;i<17640;++i) { if((hissA.samples[i]>=0)!=(hissA.samples[i-1]>=0)) ++crossings; peak=std::max(peak,std::abs(static_cast<int>(hissA.samples[i]))); }
            Check(hissA.sampleRate==22050&&hissA.samples.size()==frames&&hissB.samples.size()==frames
                &&hissA.samples!=hissB.samples&&hissA.samples==island::GenerateCrocodileHiss(0).samples
                &&hissA.samples.front()==0&&hissA.samples.back()==0&&hissB.samples.front()==0&&hissB.samples.back()==0,
                "crocodile hiss PCM is deterministic, two variants, 1.2 s and silent at endpoints");
            Check(peak>5000&&crossings<3200,"the crocodile's hiss is loud in its middle and low in pitch");
        }
        {
            // The bird alarm chatter PCM: deterministic, two variants, 1.0 s, silent at both
            // ends, high (many zero crossings) and in distinct chirps (quiet gaps between).
            const auto chatterA=island::GenerateBirdChatter(0),chatterB=island::GenerateBirdChatter(1);
            const std::size_t frames=static_cast<std::size_t>(22050*1.0f);
            int crossings=0,quietRuns=0;bool quiet=false;
            for(std::size_t i=2205;i<19845;++i) {
                if((chatterA.samples[i]>=0)!=(chatterA.samples[i-1]>=0)) ++crossings;
                const bool silent=std::abs(static_cast<int>(chatterA.samples[i]))<40;
                if(silent&&!quiet) ++quietRuns;
                quiet=silent;
            }
            Check(chatterA.sampleRate==22050&&chatterA.samples.size()==frames&&chatterB.samples.size()==frames
                &&chatterA.samples!=chatterB.samples&&chatterA.samples==island::GenerateBirdChatter(0).samples
                &&chatterA.samples.front()==0&&chatterA.samples.back()==0&&chatterB.samples.front()==0&&chatterB.samples.back()==0,
                "bird chatter PCM is deterministic, two variants, 1 s and silent at endpoints");
            Check(crossings>1500&&quietRuns>=5,"the bird chatter is high and comes in distinct chirps");
        }
        {
            // The hoofbeats PCM: deterministic, two variants, 1.4 s, silent at both ends, low
            // (few zero crossings) and in distinct beats (quiet gaps between).
            {
                // The claw knock: an original 0.35 s double click, deterministic, bounded, with
                // its two hits a tenth of a second apart and silence between and after.
                const auto knock=island::GenerateClawKnock();
                Check(knock.sampleRate==22050&&knock.samples.size()==static_cast<std::size_t>(22050*0.35f)&&knock.samples==island::GenerateClawKnock().samples,
                      "the claw knock is not deterministic at its length");
                float peak=0.0f,secondHit=0.0f,between=0.0f,tail=0.0f;
                for(std::size_t i=0;i<knock.samples.size();++i) {
                    const float t=static_cast<float>(i)/static_cast<float>(knock.sampleRate);
                    const float a=static_cast<float>(std::abs(static_cast<int>(knock.samples[i])))/32767.0f;
                    peak=std::max(peak,a);
                    if(t>0.13f&&t<0.16f) secondHit=std::max(secondHit,a);
                    if(t>0.09f&&t<0.125f) between=std::max(between,a);
                    if(t>0.33f) tail=std::max(tail,a);
                }
                Check(peak>0.3f&&peak<=1.0f&&secondHit>0.2f&&between<peak*0.3f&&tail<0.05f,"the claw knock is not two short clicks with quiet between and after");
            }
            const auto boar=island::GenerateHoofbeats(0),deer=island::GenerateHoofbeats(1);
            const std::size_t frames=static_cast<std::size_t>(22050*1.4f);
            int crossings=0,quietRuns=0;bool quiet=false;
            for(std::size_t i=2205;i<24255;++i) {
                if((boar.samples[i]>=0)!=(boar.samples[i-1]>=0)) ++crossings;
                const bool silent=std::abs(static_cast<int>(boar.samples[i]))<40;
                if(silent&&!quiet) ++quietRuns;
                quiet=silent;
            }
            Check(boar.sampleRate==22050&&boar.samples.size()==frames&&deer.samples.size()==frames
                &&boar.samples!=deer.samples&&boar.samples==island::GenerateHoofbeats(0).samples
                &&boar.samples.front()==0&&boar.samples.back()==0&&deer.samples.front()==0&&deer.samples.back()==0,
                "hoofbeats PCM is deterministic, two variants, 1.4 s and silent at endpoints");
            Check(crossings<6000&&quietRuns>=4,"the hoofbeats are low and come in distinct beats");
        }
        // A crocodile on the bank slides into the water when the walker comes near: the entry
        // is noted once, at a point at the water's edge, within a minute.
        {
            island::CrocodileGroup crocs(terrain);
            std::size_t basking=island::CrocodileGroup::CrocodileCount;
            for(std::size_t i=0;i<crocs.Crocodiles().size();++i) if(crocs.Crocodiles()[i].submerged<0.01f) { basking=i; break; }
            Check(basking<island::CrocodileGroup::CrocodileCount,"a crocodile starts on the bank");
            const auto start=crocs.Crocodiles()[basking];
            for(int step=0;step<1200&&crocs.LastEntry().count==0U;++step) crocs.Update(0.05f,start.x+3.0f,start.y+1.6f,start.z);
            Check(crocs.LastEntry().count==1U&&terrain.Height(crocs.LastEntry().x,crocs.LastEntry().z)<0.3f,
                  "the crocodile slides into the water with one entry noted at the water's edge");
        }
        // The wake: a crocodile swimming at the surface, watched from close by, lays foam at
        // its snout a few times a second, every puff on the water within two metres of a
        // swimmer, gone within two and a half seconds; a crocodile on the bank lays none.
        {
            island::CrocodileGroup swimmers(terrain);
            std::size_t swimmer=island::CrocodileGroup::CrocodileCount;
            for(std::size_t i=0;i<swimmers.Crocodiles().size();++i) if(swimmers.Crocodiles()[i].submerged>0.99f) { swimmer=i; break; }
            Check(swimmer<island::CrocodileGroup::CrocodileCount,"a crocodile starts swimming");
            island::CrocodileWakeField wake;
            bool near=true;
            for(int step=0;step<60;++step) {
                const auto subject=swimmers.Crocodiles()[swimmer];
                swimmers.Update(0.05f,subject.x+6.0f,subject.y+1.6f,subject.z);
                wake.Update(0.05f,swimmers.Crocodiles());
                for(const auto& puff:wake.Puffs()) {
                    if(!puff.active) continue;
                    bool close=false;
                    for(const auto& croc:swimmers.Crocodiles())
                        if(croc.submerged>0.6f&&std::hypot(croc.x-puff.x,croc.z-puff.z)<2.0f*croc.scale+2.0f) close=true;
                    if(!close||std::abs(puff.y)>0.1f) near=false;
                }
            }
            Check(wake.ReleaseCount()>=5U&&wake.ActiveCount()>0U&&near,"a swimming crocodile lays a wake of foam on the water behind its snout");
            // The legs: the swimmer has them folded back, a crocodile on the bank splayed.
            bool bankSplayed=true;
            for(const auto& croc:swimmers.Crocodiles()) if(croc.submerged<0.2f&&croc.legFold>0.1f) bankSplayed=false;
            Check(swimmers.Crocodiles()[swimmer].legFold>0.9f&&swimmers.FoldedLegCount()>=1U&&bankSplayed,
                  "a swimming crocodile does not fold its legs back, or one on the bank does");
            island::CrocodileState bank; bank.simulationTier=island::WildlifeSimulationTier::Full; bank.speed=1.0f;
            island::CrocodileWakeField still;
            for(int step=0;step<60;++step) still.Update(0.05f,std::span<const island::CrocodileState>(&bank,1));
            Check(still.ReleaseCount()==0U,"a crocodile on the bank lays no wake");
            for(int step=0;step<60;++step) still.Update(0.05f,swimmers.Crocodiles().subspan(0,0));
            for(int step=0;step<60;++step) wake.Update(0.05f,swimmers.Crocodiles().subspan(0,0));
            Check(wake.ActiveCount()==0U,"the wake is gone within two and a half seconds");
        }
        // The deer keep clear of the campfire: a clearing home reaches the fire, yet over ten
        // minutes no deer, grazing, walking or bolting from the walker, comes within seven
        // metres of it.
        {
            island::DeerHerd herd(terrain,{});
            bool homeReachesFire=false;
            for(const auto& home:herd.Homes()) if(std::hypot(home.x-island::CampfireSite::X,home.z-island::CampfireSite::Z)<home.radius+5.0f) homeReachesFire=true;
            Check(homeReachesFire,"a deer home reaches the campfire");
            float nearest=1e9f;
            for(int step=0;step<12000;++step) {
                const auto& lead=herd.Deer()[0];
                // The walker drives the herd about: standing near the fire every other minute.
                const bool drive=(step/1200)%2==0;
                herd.Update(0.05f,drive?island::CampfireSite::X+4.0f:1000.0f,drive?lead.y+1.6f:0.0f,drive?island::CampfireSite::Z:1000.0f);
                for(const auto& deer:herd.Deer()) nearest=std::min(nearest,std::hypot(deer.x-island::CampfireSite::X,deer.z-island::CampfireSite::Z));
            }
            Check(nearest>island::DeerHerd::FireClearance-0.01f,"no deer comes within seven metres of the campfire");
        }
        // The bear eats the fallen fruit: fruit shaken down three metres from its home draws
        // its next forage; it walks there, stands over it and the fruit is gone within ten
        // simulated minutes.
        {
            island::BearMotion bear(terrain,{});
            const auto den=bear.State();
            island::FallingFruitField larder(terrain);
            island::MonkeyState picker{};
            picker.behavior=island::MonkeyBehavior::InspectTree;picker.pickingFruit=true;picker.simulationTier=island::WildlifeSimulationTier::Full;
            picker.selectedTree={den.homeX+3.0f,terrain.Height(den.homeX+3.0f,den.homeZ),den.homeZ,0.4f,1.0f,2U};
            std::vector<island::MonkeyState> pickers{picker};
            for(int step=0;step<400;++step) larder.Update(0.05f,pickers);
            for(int step=0;step<12000&&bear.EatCount()==0U;++step) {
                bear.SetLyingFruit(larder.Fruit());
                bear.Update(0.05f,{NAN,NAN,NAN});
                for(const std::size_t index:bear.EatenFruit()) larder.Consume(index);
                larder.Update(0.05f,pickers);
            }
            Check(bear.EatCount()>=1U&&larder.ConsumedCount()>=1U,"the bear walks to a lying fruit and eats it within ten minutes");
        }
        // A sea turtle's breath: with the observer over the turtles (full simulation) a turtle
        // surfacing for air is noted within three minutes, and one small puff forms at its
        // nose and is gone within the second.
        {
            island::SeaTurtleGroup turtles(terrain);
            const auto first=turtles.Turtles()[0];
            for(int step=0;step<3600&&turtles.LastBreath().count==0U;++step)
                turtles.Update(0.05f,first.x,2.0f,first.z);
            Check(turtles.LastBreath().count>=1U,"a sea turtle lifts its head through the surface for a breath within three minutes");
            {
                // The breath's blink: the breathing turtle has its eyes squeezed shut within a
                // fifth of a second of the breath and open again within two seconds.
                std::size_t breather=0; float nearest=1e9f;
                for(std::size_t i=0;i<turtles.Turtles().size();++i) {
                    const float d=std::hypot(turtles.Turtles()[i].x-turtles.LastBreath().x,turtles.Turtles()[i].z-turtles.LastBreath().z);
                    if(d<nearest) { nearest=d; breather=i; }
                }
                float shut=0.0f;
                for(int step=0;step<4;++step) { turtles.Update(0.05f,first.x,2.0f,first.z); shut=std::max(shut,turtles.Turtles()[breather].blink); }
                Check(turtles.BlinkCount()>=1U&&shut>0.9f,"a turtle does not squeeze its eyes shut as it breaks the surface");
                for(int step=0;step<40;++step) turtles.Update(0.05f,first.x,2.0f,first.z);
                Check(turtles.Turtles()[breather].blink<0.1f,"the breath's blink does not open again");
            }
            island::TurtleBreathField breaths;
            breaths.Update(0.05f,0.0f,turtles.LastBreath());
            const auto& breath=turtles.LastBreath();
            bool atNose=true;
            for(const auto& puff:breaths.Puffs()) if(puff.active&&std::hypot(puff.x-breath.x,puff.z-breath.z)>1.0f) atNose=false;
            Check(breaths.ActiveCount()==1U&&atNose,"one puff of spray stands at the turtle's nose");
            for(int step=0;step<30;++step) breaths.Update(0.05f,0.0f,turtles.LastBreath());
            Check(breaths.ActiveCount()==0U,"the breath is gone within the second");
        }
        // The boars fear the bear: with the bear four metres from a boar and the walker far off
        // (near enough for full simulation), the boar trots away within two seconds.
        {
            island::BoarHerd herd(terrain);
            const auto sow=herd.Boars()[0];
            herd.SetBear(sow.x+4.0f,sow.z,true);
            for(int step=0;step<40;++step) herd.Update(0.05f,sow.x+40.0f,sow.y+1.5f,sow.z);
            Check(herd.BearRetreatCount()>=1U&&herd.Boars()[0].behavior==island::BoarBehavior::Retreat,"a boar trots away from the bear");
        }
        // A boar's trot off is counted once, with the boar: the walker at four metres sends the
        // first boar off and the count names it; a deer bolting from the walker likewise.
        {
            island::BoarHerd herd(terrain);
            const auto sow=herd.Boars()[0];
            for(int step=0;step<10;++step) herd.Update(0.05f,sow.x+4.0f,sow.y+1.5f,sow.z);
            Check(herd.RetreatCount()==1U&&herd.LastRetreatIndex()==0U&&herd.Boars()[0].behavior==island::BoarBehavior::Retreat,
                  "the walker at four metres sends the first boar trotting off, counted once");
            island::DeerHerd deer(terrain,{});
            const auto doe=deer.Deer()[1];
            for(int step=0;step<10;++step) deer.Update(0.05f,doe.x+4.0f,doe.y+1.6f,doe.z);
            Check(deer.BoltCount()>=1U&&deer.Deer()[deer.LastBoltIndex()].behavior==island::DeerBehavior::Retreat,
                  "the walker at four metres sends a deer bolting, counted with the deer");
        }
        // A resting boar lies down: a second and a half into an undisturbed rest the boar
        // settles onto folded legs (and the rest is lengthened for it); the walker at ten
        // metres has it on its feet within half a second.
        {
            island::BoarHerd herd(terrain);
            const auto sow=herd.Boars()[1];
            bool lay=false,atRest=false;
            for(int step=0;step<2400&&!lay;++step) {
                herd.Update(0.05f,sow.x+40.0f,sow.y+1.5f,sow.z);
                if(herd.Boars()[1].lieAmount>0.9f) {lay=true;atRest=herd.Boars()[1].behavior==island::BoarBehavior::Rest;}
            }
            Check(lay&&atRest&&herd.LieCount()>=1U&&herd.LyingCount()>=1U,"a resting boar lies down within two minutes, at rest");
            const auto lying=herd.Boars()[1];
            bool up=false;
            for(int step=0;step<12&&!up;++step) {
                herd.Update(0.05f,lying.x+10.0f,lying.y+1.5f,lying.z);
                up=herd.Boars()[1].lieAmount<0.2f&&!herd.Lying(1);
            }
            Check(up,"the walker at ten metres has the lying boar on its feet within half a second");
        }
        // The hut's roof ridge is a perch: it stands on the ridge cap above the hut, near the
        // veranda end, well clear of the ground, and the flock accepts it as a site.
        {
            const island::JungleLayout hutJungle(terrain);
            const std::vector<island::CircularObstacle> none;
            const auto hut=island::HutLayout::FindInWorld(terrain,hutJungle,none);
            Check(hut.has_value(),"the hut is placed for its ridge perch");
            {
                // The resident's yard (backlog item 12): the drying rack, coconut pile, net
                // frame and basket stand at their four hut-local sites in the clearing, each
                // on the rendered ground, distinct, and clear of the hut's walls.
                const island::RenderedTerrainSurface yardSurface(terrain);
                bool yardOk=true;
                for(std::size_t site=0;site<hut->yard.size();++site) {
                    const auto& prop=hut->yard[site];
                    const float c=std::cos(hut->yawRadians),s=std::sin(hut->yawRadians);
                    const float u=island::HutLayout::YardSites[site][0],v=island::HutLayout::YardSites[site][1];
                    const float ex=hut->x+c*u+s*v,ez=hut->z-s*u+c*v;
                    const float fromHut=std::hypot(prop.x-hut->x,prop.z-hut->z);
                    if(!std::isfinite(prop.groundY)||std::hypot(prop.x-ex,prop.z-ez)>0.001f
                       ||std::abs(prop.groundY-yardSurface.Sample(prop.x,prop.z).height)>0.001f
                       ||fromHut<island::HutLayout::Width*0.5f||fromHut>island::HutLayout::SiteClearance) yardOk=false;
                    for(std::size_t other=0;other<site;++other)
                        if(std::hypot(prop.x-hut->yard[other].x,prop.z-hut->yard[other].z)<1.0f) yardOk=false;
                }
                Check(yardOk,"the resident's four yard props stand at their sites in the hut's clearing, on the ground and apart");
            }
            const auto perch=island::HutLayout::RidgePerch(*hut);
            Check(std::hypot(perch.x-hut->x,perch.z-hut->z)<island::HutLayout::Depth*0.5f+0.01f
                  &&std::abs(perch.y-(hut->floorY+island::HutLayout::RidgeHeight+0.04f))<0.001f
                  &&perch.y>terrain.Height(perch.x,perch.z)+2.5f,"the ridge perch sits on the roof ridge well above the ground");
            island::BirdFlock roofBirds(terrain);
            const std::vector<island::BirdPerchSite> sites{perch};
            roofBirds.SetPerchSites(sites);
            Check(roofBirds.PerchSites().size()==1U&&roofBirds.Birds()[0].behavior==island::BirdBehavior::Perched,"the flock seeds a bird onto the ridge perch");
        }
    }
        // The ground under that tree slopes, so a fruit bounces off its strike point and rolls
        // on a little before it rests; resting fruit sit on the ground and keep still.
        Check(field.LastRollDistance()>0.08f&&field.LastRollDistance()<4.0f,"a fruit rolls a bounded distance from where it struck before it rests");
        bool stillAtRest=true;
        for(const auto& f:field.Fruit()) if(f.active&&f.restSeconds>=0.0f&&!f.rolling&&(f.velocityX!=0.0f||f.velocityZ!=0.0f)) stillAtRest=false;
        Check(stillAtRest,"resting fruit keeps still");
        std::size_t lying=0;
        for(std::size_t i=0;i<field.Fruit().size();++i) if(field.Fruit()[i].active&&field.Fruit()[i].restSeconds>=0.0f) { field.Consume(i); ++lying; break; }
        Check(lying==1U&&field.ConsumedCount()==1U&&field.ActiveCount()+1U>=lying,"a lying fruit can be eaten and is counted once");
        // Monkeys eat the fruit lying under the crowns: fruit shaken down beside a troop
        // member's home draws its next wander, and arriving at it removes the fruit.
        island::MonkeyTroop eaters(terrain);
        const island::MonkeyState& first=eaters.Monkeys().front();
        island::FallingFruitField larder(terrain);
        island::MonkeyState picker=monkey;
        picker.selectedTree={first.homeX+3.0f,terrain.Height(first.homeX+3.0f,first.homeZ),first.homeZ,0.4f,1.0f,2U};
        std::vector<island::MonkeyState> pickers{picker};
        for(int step=0;step<600;++step) larder.Update(0.05f,pickers);
        std::size_t lyingNearHome=0;
        for(const auto& f:larder.Fruit()) if(f.active&&f.restSeconds>=0.0f) ++lyingNearHome;
        Check(lyingNearHome>=3U,"fruit lies on the ground beside the monkey's home");
        std::uint32_t eaten=0;
        for(int step=0;step<36000&&eaten==0U;++step) {
            const island::MonkeyState& watched=eaters.Monkeys().front();
            eaters.SetLyingFruit(larder.Fruit());
            eaters.Update(1.0f/30.0f,watched.homeX+16.0f,watched.y+1.5f,watched.homeZ);
            for(const std::size_t index:eaters.EatenFruit()) larder.Consume(index);
            larder.Update(1.0f/30.0f,{});
            eaten=eaters.EatCount();
        }
        Check(eaten>=1U&&larder.ConsumedCount()>=1U,"a monkey walks to a lying fruit within twenty minutes and eats it");
        // Boars forage for the fallen fruit too, coconuts included: fruit shaken down beside a
        // boar's home is its next rooting site, and nosing it eats it.
        island::BoarHerd rooters(terrain);
        const island::BoarState& sow=rooters.Boars().front();
        island::FallingFruitField mast(terrain);
        island::MonkeyState shaker=monkey;
        shaker.selectedTree={sow.homeX+2.5f,terrain.Height(sow.homeX+2.5f,sow.homeZ),sow.homeZ,0.4f,1.0f,2U};
        std::vector<island::MonkeyState> shakers{shaker};
        for(int step=0;step<600;++step) mast.Update(0.05f,shakers);
        std::size_t lyingNearSow=0;
        for(const auto& f:mast.Fruit()) if(f.active&&f.restSeconds>=0.0f) ++lyingNearSow;
        Check(lyingNearSow>=3U,"fruit lies on the ground beside the boar's home");
        std::uint32_t rooted=0;
        for(int step=0;step<36000&&rooted==0U;++step) {
            const island::BoarState& watched=rooters.Boars().front();
            rooters.SetLyingFruit(mast.Fruit());
            rooters.Update(1.0f/30.0f,watched.homeX+40.0f,watched.y+1.5f,watched.homeZ);
            for(const std::size_t index:rooters.EatenFruit()) mast.Consume(index);
            mast.Update(1.0f/30.0f,{});
            rooted=rooters.EatCount();
        }
        Check(rooted>=1U&&mast.ConsumedCount()>=1U,"a boar roots its way to a lying fruit within twenty minutes and eats it");
        // Butterflies come down to fruit that has lain fermenting: fruit shaken down eight
        // metres from the first habitat draws a butterfly to hover low over it.
        island::ButterflyMeadow drinkers(terrain);
        const auto habitat=drinkers.Homes()[0];
        island::FallingFruitField windfall(terrain);
        island::MonkeyState dropper=monkey;
        dropper.selectedTree={habitat.x+8.0f,terrain.Height(habitat.x+8.0f,habitat.z),habitat.z,0.4f,1.0f,2U};
        std::vector<island::MonkeyState> droppers{dropper};
        for(int step=0;step<800;++step) windfall.Update(0.05f,droppers);
        std::size_t fermenting=0;
        for(const auto& f:windfall.Fruit()) if(f.active&&f.restSeconds>=island::ButterflyMeadow::FermentSeconds) ++fermenting;
        Check(fermenting>=1U,"fruit has lain fermenting beside the butterfly habitat");
        // The fruit a visit was decided on may have rotted away by the time of the hover,
        // so the check is the hover itself: low over the target, at the fruit's height.
        bool fedOnFruit=false;
        for(int step=0;step<7200&&!fedOnFruit;++step) {
            drinkers.SetLyingFruit(windfall.Fruit());
            drinkers.Update(0.05f,std::numeric_limits<float>::quiet_NaN(),0.0f,0.0f);
            windfall.Update(0.05f,droppers);
            for(std::size_t index=0;index<6U;++index) {
                const auto& fly=drinkers.Butterflies()[index];
                float x=0.0f,z=0.0f,height=0.0f;
                if(fly.behavior!=island::ButterflyBehavior::NectarHover||!drinkers.NectarTarget(index,x,z,height)) continue;
                if(height<=island::ButterflyMeadow::FruitHoverHeight+0.01f&&std::hypot(fly.x-x,fly.z-z)<1.5f) fedOnFruit=true;
            }
        }
        Check(drinkers.FruitVisitCount()>=1U&&fedOnFruit,"a butterfly hovers low over the fermenting fruit within six minutes");
    }

    {
        // The lookout tower beside the hut clearing: a site exists, the platform stands three
        // metres up, the ramp is walkable, and the support acts only on the deck and ramp.
        const island::IslandTerrain terrain;
        island::HutPlacement hut{};
        hut.x=island::HutLayout::PreferredClearingX; hut.z=island::HutLayout::PreferredClearingZ;
        hut.yawRadians=std::atan2(hut.x,hut.z);
        const auto lookout=island::LookoutLayout::FindInWorld(terrain,hut,{});
        Check(lookout.has_value(),"a lookout tower site exists beside the hut clearing");
        if(lookout) {
            const float ground=terrain.Height(lookout->x,lookout->z);
            Check(lookout->SupportHeight(lookout->x,lookout->z,ground)>=ground+3.0f,
                  "the lookout platform stands three metres above the ground");
            const float midU=island::LookoutPlacement::Half+island::LookoutPlacement::RampLength*0.5f;
            const float midX=lookout->x+lookout->dirX*midU,midZ=lookout->z+lookout->dirZ*midU;
            const float midGround=terrain.Height(midX,midZ);
            const float midSupport=lookout->SupportHeight(midX,midZ,midGround);
            Check(lookout->OnRamp(midX,midZ)&&midSupport>midGround&&midSupport<lookout->platformY,
                  "the ramp carries the walker between the ground and the platform");
            Check(std::atan2(lookout->platformY-lookout->rampFootY,island::LookoutPlacement::RampLength)
                  <island::PlayerMotion::MaximumWalkSlopeRadians,"the lookout ramp is walkable");
            const float offX=lookout->x+lookout->AcrossX()*3.0f,offZ=lookout->z+lookout->AcrossZ()*3.0f;
            Check(!lookout->OnDeck(offX,offZ)&&lookout->SupportHeight(offX,offZ,2.0f)==2.0f,
                  "the lookout support leaves the ground beside it alone");
            Check(lookout->RailPerch().y>lookout->platformY+1.0f,"the lookout rail offers a perch above the platform");
        }
    }

    {
        // The rope bridge: anchored on both banks above the water, a walkway that sags but
        // clears the stream, and a support that only acts over the planks.
        const island::IslandTerrain terrain;
        const island::StreamCourse course(terrain);
        const auto bridge=island::BridgeLayout::FindInWorld(terrain,course);
        Check(bridge.has_value(),"a rope bridge site exists on the stream's lower reach");
        if(bridge) {
            const auto& station=course.Station(bridge->station);
            Check(bridge->station>course.CascadeIndex()+island::StreamCourse::PoolLength,
                  "rope bridge crosses downstream of the plunge pool");
            Check(bridge->ay>station.water+0.3f&&bridge->by>station.water+0.3f,
                  "rope bridge anchors stand on both banks above the water");
            const float midX=(bridge->ax+bridge->bx)*0.5f,midZ=(bridge->az+bridge->bz)*0.5f;
            const float mid=bridge->DeckHeight(bridge->span*0.5f);
            Check(mid<std::min(bridge->ay,bridge->by)&&mid>station.water+0.2f,
                  "rope bridge walkway sags below its anchors but clears the stream");
            Check(bridge->OnDeck(midX,midZ)&&bridge->SupportHeight(midX,midZ,station.bed)==mid,
                  "rope bridge supports the walker above the carved bed mid-span");
            const float offX=midX+bridge->AcrossX()*1.6f,offZ=midZ+bridge->AcrossZ()*1.6f;
            Check(!bridge->OnDeck(offX,offZ)&&bridge->SupportHeight(offX,offZ,station.bed)==station.bed,
                  "rope bridge support leaves the stream beside the planks alone");
        }
    }

    {
        // The jetty below the hut: rooted on sand, ending over water, a deck the walker can
        // stand on above the shallows, and a canoe on dry sand beside the root.
        const island::IslandTerrain terrain;
        island::HutPlacement hut{};
        hut.x=island::HutLayout::PreferredClearingX; hut.z=island::HutLayout::PreferredClearingZ;
        const auto jetty=island::JettyLayout::FindInWorld(terrain,hut);
        Check(jetty.has_value(),"a jetty site exists on the shore below the hut clearing");
        if(jetty) {
            const float rootHeight=terrain.Height(jetty->rootX,jetty->rootZ);
            const float endX=jetty->rootX+jetty->dirX*island::JettyPlacement::Length;
            const float endZ=jetty->rootZ+jetty->dirZ*island::JettyPlacement::Length;
            Check(rootHeight>=island::JettyLayout::MinimumRootHeight&&terrain.Height(endX,endZ)<0.0f,
                  "jetty roots on the sand and ends over water");
            const float midX=jetty->rootX+jetty->dirX*9.0f,midZ=jetty->rootZ+jetty->dirZ*9.0f;
            const float midTerrain=terrain.Height(midX,midZ);
            Check(jetty->OnDeck(midX,midZ)&&jetty->SupportHeight(midX,midZ,midTerrain)==jetty->deckY
                  &&jetty->deckY>=island::JettyLayout::DeckClearance,
                  "jetty deck supports the walker at its level height over the shallows");
            const float offX=midX+jetty->AcrossX()*2.5f,offZ=midZ+jetty->AcrossZ()*2.5f;
            Check(!jetty->OnDeck(offX,offZ)&&jetty->SupportHeight(offX,offZ,-0.7f)==-0.7f,
                  "jetty support leaves the water beside the deck alone");
            Check(jetty->canoeY>=island::JettyLayout::MinimumCanoeHeight,"the dugout canoe rests on dry sand");
            const auto perch=jetty->MooringPerch();
            Check(perch.y>jetty->deckY+0.5f,"the mooring post offers a perch above the deck");
            const island::RenderedTerrainSurface rendered(terrain);
            const auto prints=jetty->FootprintLines(terrain,rendered);
            bool onBeach=true,leadingAway=true;
            float previousAway=0.0f;
            for(std::size_t i=0;i<prints.size();++i) {
                const auto& print=prints[i];
                const float away=std::hypot(print.x-jetty->rootX,print.z-jetty->rootZ);
                const float inland=terrain.SignedShoreDistance(print.x,print.z);
                onBeach=onBeach&&print.variant==2U&&print.y>0.02f&&inland>0.4f&&inland<4.0f;
                if(i%2U==0U) { leadingAway=leadingAway&&away>previousAway; previousAway=away; }
            }
            Check(onBeach&&leadingAway,"the resident's footprints follow the beach away from the jetty foot");
        }
    }

    {
        // Sea from within: the pod leaps within a few minutes, the shark fins at the surface
        // during its window, and a school parts around a swimmer placed at its centre.
        const island::IslandTerrain terrain;
        island::DolphinPod pod(terrain);
        float crest = -10.0f;
        for (int step = 0; step < 6000; ++step) {
            pod.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
            for (const auto& dolphin : pod.Dolphins()) crest = std::max(crest, dolphin.y);
        }
        Check(pod.LeapCount() >= 1U && crest > 0.9f && pod.LastLeap().valid && std::isfinite(pod.LastLeap().x),
              "dolphins leap clear of the water within five minutes");
        // Every finished leap came down in a splash, noted once where the dolphin re-entered,
        // within a leap's travel of where it left the water; the ring ages out in under
        // three seconds.
        Check(pod.SplashCount() >= 1U && pod.SplashCount() + 1U >= pod.LeapCount(),
              "each finished leap is noted as a splash");
        {
            island::DolphinPod again(terrain);
            std::uint32_t seen = 0U, leapSplashes = 0U;
            bool placed = true, aged = true;
            float leapX = 0.0f, leapZ = 0.0f;
            for (int step = 0; step < 6000 && leapSplashes < 2U; ++step) {
                again.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
                if (again.LastLeap().valid) { leapX = again.LastLeap().x; leapZ = again.LastLeap().z; }
                if (again.SplashCount() != seen) {
                    seen = again.SplashCount();
                    const auto& splash = again.LastSplash();
                    if (splash.tailSlap) continue;   // a tail slap's splash is not a leap's
                    ++leapSplashes;
                    if (!splash.valid || std::hypot(splash.x - leapX, splash.z - leapZ) > 12.0f) placed = false;
                    for (int tick = 0; tick < 60; ++tick) again.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
                    if (again.LastSplash().valid && again.LastSplash().count == seen) aged = false;
                    seen = again.SplashCount();
                }
            }
            Check(leapSplashes >= 1U && placed && aged, "a leap's splash lies where the dolphin came down and its ring is gone within three seconds");
        }
        island::FishSchools schools(terrain);
        schools.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        const auto centre = schools.Schools()[0];
        auto nearest = [&](float x, float y, float z) {
            float best = 1e9f;
            for (std::size_t i = 0; i < island::FishSchools::FishPerSchool; ++i) {
                const auto& fish = schools.Fish()[i];
                best = std::min(best, std::hypot(fish.y - y, std::hypot(fish.x - x, fish.z - z)));
            }
            return best;
        };
        const float before = nearest(centre.centreX, centre.centreY, centre.centreZ);
        for (int step = 0; step < 80; ++step)
            schools.Update(0.05f, centre.centreX, centre.centreY, centre.centreZ);
        const float after = nearest(centre.centreX, centre.centreY, centre.centreZ);
        Check(after > before + 0.4f && after > 1.0f,
              "a fish school parts around a swimmer at its centre");
    }

    {
        // The campfire is tended by the resident pausing at the pit: within the tend
        // distance and pausing, not walking past, and not pausing elsewhere.
        Check(island::CampfireSite::Tending(island::CampfireSite::X + 2.0f, island::CampfireSite::Z + 1.0f, true),
              "the resident pausing two metres from the pit tends the fire");
        Check(!island::CampfireSite::Tending(island::CampfireSite::X + 2.0f, island::CampfireSite::Z + 1.0f, false),
              "the resident walking past the pit does not tend the fire");
        Check(!island::CampfireSite::Tending(island::CampfireSite::X + 12.0f, island::CampfireSite::Z, true),
              "a pause twelve metres from the pit does not tend the fire");
        Check(!island::CampfireSite::Tending(std::numeric_limits<float>::quiet_NaN(), island::CampfireSite::Z, true),
              "a non-finite resident position tends nothing");
        // The resident's authored route pauses at the campfire each lap, within the tend distance.
        const island::IslandTerrain terrain;
        island::ResidentMotion tender(terrain, {});
        bool tended = false;
        for (int step = 0; step < 24000 && !tended; ++step) {
            tender.Update(0.05f, {1000.0f, 0.0f, 1000.0f});
            const auto& state = tender.State();
            tended = island::CampfireSite::Tending(state.x, state.z, state.mode == island::ResidentMode::Pausing);
        }
        Check(tended, "the resident's route brings a pause within tend distance of the campfire within twenty minutes");
        {
            // The crouch: asked for at the pause, the resident squats over a second, the
            // pelvis a third of a metre lower on the same planted feet with neither leg
            // reach-limited and both hands forward; asked off, they stand within a second.
            const auto standing = tender.State();
            tender.SetCrouch(1.0f);
            float deepest = 0.0f;
            for (int step = 0; step < 30 && tender.State().mode == island::ResidentMode::Pausing; ++step) {
                tender.Update(0.05f, {1000.0f, 0.0f, 1000.0f});
                deepest = std::max(deepest, tender.CrouchAmount());
            }
            Check(deepest > 0.7f && standing.pose.pelvisY > 0.0f, "at the pause the resident crouches most of the way within a second and a half");
            island::ResidentMotion held(terrain, {});
            const auto upright = held.State();
            held.PreviewCrouch();
            const auto& crouched = held.State();
            Check(upright.pose.pelvisY - crouched.pose.pelvisY > 0.3f && upright.pose.pelvisY - crouched.pose.pelvisY < 0.4f,
                  "the crouch drops the pelvis about a third of a metre");
            bool planted = true, reachable = true;
            for (std::size_t leg = 0; leg < 2; ++leg) {
                if (std::hypot(crouched.pose.legs[leg].ankle.x - upright.pose.legs[leg].ankle.x,
                               crouched.pose.legs[leg].ankle.z - upright.pose.legs[leg].ankle.z) > 0.01f) planted = false;
                if (crouched.pose.legs[leg].reachLimited) reachable = false;
            }
            const float forwardX = std::sin(crouched.pose.heading), forwardZ = std::cos(crouched.pose.heading);
            const auto& wrist = crouched.pose.wrists[0];
            const float ahead = (wrist.x - crouched.x) * forwardX + (wrist.z - crouched.z) * forwardZ;
            Check(planted && reachable && ahead > 0.2f, "the crouch keeps the feet planted, both legs reachable and the hands forward");
            held.SetCrouch(0.0f);
            bool stood = false;
            for (int step = 0; step < 24 && !stood; ++step) { held.Update(0.05f, {1000.0f, 0.0f, 1000.0f}); stood = held.CrouchAmount() < 0.1f; }
            Check(stood, "asked off, the resident stands within a second");
        }
        {
            // Two landmarks with facings, as the game gives the jetty foot and the lookout's
            // ramp foot: the route takes both, and at a landmark pause the resident turns to
            // its facing.
            const island::JungleLayout jungle(terrain);
            const std::vector<island::CircularObstacle> none;
            const auto hut = island::HutLayout::FindInWorld(terrain, jungle, none);
            const auto jetty = hut ? island::JettyLayout::FindInWorld(terrain, *hut) : std::nullopt;
            const auto lookout = hut ? island::LookoutLayout::FindInWorld(terrain, *hut, none) : std::nullopt;
            Check(jetty.has_value() && lookout.has_value(), "the jetty and the lookout are placed for the resident's calls");
            const island::JettyPlacement deck = jetty.value_or(island::JettyPlacement{});
            const island::LookoutPlacement tower = lookout.value_or(island::LookoutPlacement{});
            const float footAlong = island::LookoutPlacement::RampLength + 2.0f;
            const std::vector<island::ResidentPoint> landmarks{
                {deck.rootX - deck.dirX * 3.0f, 0.0f, deck.rootZ - deck.dirZ * 3.0f},
                {tower.x + tower.dirX * footAlong, 0.0f, tower.z + tower.dirZ * footAlong}};
            const std::vector<island::ResidentPoint> facings{
                {deck.dirX, 0.0f, deck.dirZ}, {-tower.dirX, 0.0f, -tower.dirZ}};
            island::ResidentMotion caller(terrain, {}, landmarks, facings);
            Check(caller.Route().LandmarkCount() == 2U, "the route takes both the jetty foot and the lookout's ramp foot");
            bool atLookout = false, facedUp = false;
            for (int step = 0; step < 36000 && !(atLookout && facedUp); ++step) {
                caller.Update(0.05f, {1000.0f, 0.0f, 1000.0f});
                const auto& state = caller.State();
                if (state.mode == island::ResidentMode::Pausing && caller.LookingOut()
                    && std::hypot(state.x - landmarks[1].x, state.z - landmarks[1].z) < 2.5f) {
                    atLookout = true;
                    const float wantHeading = std::atan2(-tower.dirX, -tower.dirZ);
                    if (std::abs(std::remainder(state.heading - wantHeading, 6.2831853f)) < 0.35f) facedUp = true;
                }
            }
            Check(atLookout && facedUp, "the resident pauses at the lookout's ramp foot within half an hour and looks up the ramp");
            // Fishing: with the jetty foot set as the fishing spot the rod comes out at that
            // pause (the right hand forward) and stays in at the lookout's.
            island::ResidentMotion angler(terrain, {}, landmarks, facings);
            angler.SetFishingSpot(landmarks[0].x, landmarks[0].z);
            bool fished = false, rodAtLookout = false, handForward = false;
            for (int step = 0; step < 36000 && !fished; ++step) {
                angler.Update(0.05f, {1000.0f, 0.0f, 1000.0f});
                const auto& state = angler.State();
                const bool atJetty = std::hypot(state.x - landmarks[0].x, state.z - landmarks[0].z) < island::ResidentMotion::FishingReach;
                const bool atTower = std::hypot(state.x - landmarks[1].x, state.z - landmarks[1].z) < 2.5f;
                if (atTower && state.fishing > 0.1f) rodAtLookout = true;
                if (atJetty && angler.Fishing() && state.fishing > 0.95f) {
                    fished = true;
                    const float forwardX = std::sin(state.pose.heading), forwardZ = std::cos(state.pose.heading);
                    const auto& wrist = state.pose.wrists[1];
                    handForward = (wrist.x - state.x) * forwardX + (wrist.z - state.z) * forwardZ > 0.25f;
                }
            }
            Check(fished && handForward && !rodAtLookout, "the resident fishes at the jetty foot, the right hand forward, and not at the lookout");
            // Through the rest of that pause the float rides the swell and the rod is lifted once.
            float lowestBob = 1.0f, highestBob = -1.0f, lift = 0.0f;
            for (int step = 0; step < 400 && angler.Fishing(); ++step) {
                angler.Update(0.05f, {1000.0f, 0.0f, 1000.0f});
                lowestBob = std::min(lowestBob, angler.State().floatBob);
                highestBob = std::max(highestBob, angler.State().floatBob);
                lift = std::max(lift, angler.State().rodLift);
            }
            Check(lowestBob < -0.02f && highestBob > 0.02f && lift > 0.9f, "the float rides the swell and the rod is lifted to check the line");
            Check(angler.State().rodLift == 0.0f || angler.Fishing(), "the rod lift ends with the fishing");
            island::ResidentMotion held(terrain, {});
            held.PreviewFishing();
            Check(held.State().fishing == 1.0f && held.Fishing(), "the fishing view's preview holds the rod out");
        }
    }

    {
        // The deer's tail: flagged up within half a second of a bolt, down again once the
        // walker is gone, and flicked half up every few seconds while grazing or resting.
        const island::IslandTerrain terrain;
        island::DeerHerd herd(terrain, {});
        const auto start = herd.Deer()[2];
        bool flagged = false;
        for (int step = 0; step < 30 && !flagged; ++step) {
            herd.Update(0.05f, start.x + 3.0f, start.y + 1.6f, start.z);
            const auto& deer = herd.Deer()[2];
            flagged = deer.behavior == island::DeerBehavior::Retreat && deer.tailRaise > 0.9f;
        }
        Check(flagged, "a bolting deer flags its tail up within a second and a half");
        bool lowered = false, flicked = false, settled = false;
        for (int step = 0; step < 600 && !(lowered && flicked && settled); ++step) {
            herd.Update(0.05f, 1000.0f, 0.0f, 1000.0f);
            const auto& deer = herd.Deer()[2];
            if (deer.behavior != island::DeerBehavior::Retreat && deer.tailRaise < 0.3f) lowered = true;
            const bool still = deer.behavior == island::DeerBehavior::Graze || deer.behavior == island::DeerBehavior::Rest;
            if (lowered && still && deer.tailRaise > 0.4f && deer.tailRaise < 0.7f) flicked = true;
            if (flicked && still && deer.tailRaise < 0.1f) settled = true;
        }
        Check(lowered, "with the walker gone the tail comes down");
        Check(flicked && settled, "a grazing or resting deer flicks its tail half up and lets it hang again");
        // The ears: watched from sixty metres (fully simulated, not alerted), over the next
        // fifteen seconds the deer twitches an ear at least twice, each twitch rising to
        // full and settling, the ears taking turns.
        const std::uint32_t twitchesBefore = herd.EarTwitchCount();
        float earPeak = 0.0f; bool earSettled = false, leftEar = false, rightEar = false;
        for (int step = 0; step < 300; ++step) {
            const auto watched = herd.Deer()[2];
            herd.Update(0.05f, watched.x + 60.0f, watched.y + 1.6f, watched.z);
            const auto& deer = herd.Deer()[2];
            earPeak = std::max(earPeak, deer.earTwitch);
            if (deer.earTwitch > 0.9f) { if (deer.earSide < 0.0f) leftEar = true; else rightEar = true; }
            if (earPeak > 0.9f && deer.earTwitch < 0.05f) earSettled = true;
        }
        Check(herd.EarTwitchCount() >= twitchesBefore + 2U && earPeak > 0.9f && earSettled && leftEar && rightEar,
              "a deer twitches one ear back and out every few seconds, the ears taking turns");
        // The stamp: the walker standing twelve metres off has the deer alert, and within
        // eight seconds it stamps a forefoot, the stamp to full and down again within a
        // second; with the walker gone the deer grazes on without stamping.
        {
            island::DeerHerd stampers(terrain, {});
            const auto watched = stampers.Deer()[2];
            float stampPeak = 0.0f; int stampFrames = 0, longestStamp = 0; bool stampDown = false, alerted = false;
            for (int step = 0; step < 160; ++step) {
                stampers.Update(0.05f, watched.x + 12.0f, watched.y + 1.6f, watched.z);
                const auto& deer = stampers.Deer()[2];
                if (deer.behavior == island::DeerBehavior::Rest) alerted = true;
                stampPeak = std::max(stampPeak, deer.stamp);
                stampFrames = deer.stamp > 0.5f ? stampFrames + 1 : 0; longestStamp = std::max(longestStamp, stampFrames);
                if (stampPeak > 0.9f && deer.stamp < 0.05f) stampDown = true;
            }
            Check(alerted && stampers.StampCount() >= 1U && stampPeak > 0.9f && longestStamp <= 20 && stampDown,
                  "a deer standing alert at the walker stamps a forefoot and sets it down again");
            float grazingStamp = 0.0f;
            for (int step = 0; step < 200; ++step) {
                stampers.Update(0.05f, 1000.0f, 0.0f, 1000.0f);
                if (step > 20) grazingStamp = std::max(grazingStamp, stampers.Deer()[2].stamp);
            }
            Check(grazingStamp < 0.05f, "a deer with the walker gone keeps stamping");
            // The cud: with the walker gone, within two minutes a resting deer chews, the
            // muzzle swung past eight tenths to either side, and a walking deer's jaw is still.
            bool chewLeft = false, chewRight = false, walkingChew = false;
            for (int step = 0; step < 2400 && !(chewLeft && chewRight); ++step) {
                stampers.Update(0.05f, 1000.0f, 0.0f, 1000.0f);
                for (const auto& deer : stampers.Deer()) {
                    if (deer.behavior == island::DeerBehavior::Rest && deer.chew > 0.8f) chewRight = true;
                    if (deer.behavior == island::DeerBehavior::Rest && deer.chew < -0.8f) chewLeft = true;
                    if (deer.behavior == island::DeerBehavior::Walk && deer.speed > 0.5f && std::abs(deer.chew) > 0.3f) walkingChew = true;
                }
            }
            Check(chewLeft && chewRight && !walkingChew, "a resting deer does not chew its cud, or chews on the move");
            // The head toss: through those minutes a deer coming up from a graze tosses its
            // head to full within half a second of the graze's end and settles it within a
            // second; a grazing deer's head is not tossed.
            {
                std::uint32_t tossesBefore = stampers.TossCount();
                float tossPeak = 0.0f, grazingToss = 0.0f; bool tossSettled = false;
                // Watched from sixty metres (fully simulated, not alerted): a dormant deer's
                // updates come seconds apart and would step over the brief toss.
                for (int step = 0; step < 2400; ++step) {
                    const auto watched = stampers.Deer()[2];
                    stampers.Update(0.05f, watched.x + 60.0f, watched.y + 1.6f, watched.z);
                    for (const auto& deer : stampers.Deer()) {
                        if (deer.simulationTier != island::WildlifeSimulationTier::Full) continue;
                        tossPeak = std::max(tossPeak, deer.headToss);
                        if (deer.grazeAmount > 0.9f) grazingToss = std::max(grazingToss, deer.headToss);
                        if (tossPeak > 0.9f && deer.headToss < 0.05f) tossSettled = true;
                    }
                }
                Check(stampers.TossCount() >= tossesBefore + 1U && tossPeak > 0.9f && tossSettled && grazingToss < 0.5f,
                      "a deer coming up from a graze does not toss its head and settle it");
            }
        }
    }

    {
        // In a downpour a deer in the open walks to the crown's cover and beds down under it.
        const island::IslandTerrain terrain;
        island::DeerHerd herd(terrain, {});
        const island::DeerHome home = herd.Homes()[2];
        const float slopeX = island::environment::SunDirectionX / island::environment::SunDirectionY;
        const float slopeZ = island::environment::SunDirectionZ / island::environment::SunDirectionY;
        const float groundY = terrain.Height(home.x, home.z);
        const island::CanopyOccluder crown{home.x + 5.0f + slopeX * 6.0f, groundY + 6.0f, home.z + slopeZ * 6.0f,
                                           3.5f, 2.5f, 3.5f, 0.0f, 3.0f};
        const island::CanopyLightField light(terrain, std::span<const island::CanopyOccluder>(&crown, 1), 512);
        herd.SetLight(&light);
        herd.SetRain(0.9f);
        bool sheltered = false;
        for (int step = 0; step < 24000 && !sheltered; ++step) {
            herd.Update(0.05f, home.x + 60.0f, groundY + 1.6f, home.z);
            const auto& deer = herd.Deer()[2];
            sheltered = herd.RainShelterCount() >= 1U && deer.behavior == island::DeerBehavior::Rest
                && light.Sample(deer.x, deer.z) < island::DeerHerd::ShadedLevel;
        }
        Check(sheltered, "in a downpour a deer in the open walks to a crown's cover and rests under it");
    }
    {
        // A deer rests in the shade: with one dense crown shading a spot five metres from the
        // third deer's home, a rest that comes on sunlit ground becomes a walk to that
        // shade and the rest is taken there; with no light field the deer rests where it is.
        const island::IslandTerrain terrain;
        island::DeerHerd herd(terrain, {}), plain(terrain, {});
        const island::DeerHome home = herd.Homes()[2];
        const float slopeX = island::environment::SunDirectionX / island::environment::SunDirectionY;
        const float slopeZ = island::environment::SunDirectionZ / island::environment::SunDirectionY;
        const float groundY = terrain.Height(home.x, home.z);
        // The crown sits so that its shadow lands five metres east of the home centre.
        const island::CanopyOccluder crown{home.x + 5.0f + slopeX * 6.0f, groundY + 6.0f, home.z + slopeZ * 6.0f,
                                           3.5f, 2.5f, 3.5f, 0.0f, 3.0f};
        const island::CanopyLightField light(terrain, std::span<const island::CanopyOccluder>(&crown, 1), 512);
        Check(light.Sample(home.x + 5.0f, home.z) < island::DeerHerd::ShadedLevel
                  && light.Sample(home.x - 8.0f, home.z) > island::DeerHerd::SunlitLevel,
              "the test crown shades the spot east of the home and leaves the west sunlit");
        herd.SetLight(&light);
        bool sought = false, restedInShade = false;
        for (int step = 0; step < 36000 && !restedInShade; ++step) {
            herd.Update(0.05f, home.x + 60.0f, groundY + 1.6f, home.z);
            plain.Update(0.05f, home.x + 60.0f, groundY + 1.6f, home.z);
            if (herd.SeekingShade(2)) sought = true;
            const auto& deer = herd.Deer()[2];
            if (herd.ShadeRestCount() >= 1U && deer.behavior == island::DeerBehavior::Rest
                && light.Sample(deer.x, deer.z) < island::DeerHerd::ShadedLevel)
                restedInShade = true;
        }
        Check(sought, "a rest on sunlit ground sends the deer walking to the shade");
        Check(restedInShade, "the deer takes its rest in the crown's shade within half an hour");
        Check(plain.ShadeRestCount() == 0U && !plain.SeekingShade(2), "with no light field the deer rests where it stands");
        // The shade rest beds the deer down within two seconds; a walker arriving at three
        // metres has it up and bolting within half a second.
        bool bedded = false;
        for (int step = 0; step < 60 && !bedded; ++step) {
            herd.Update(0.05f, home.x + 60.0f, groundY + 1.6f, home.z);
            bedded = herd.Bedded(2) && herd.Deer()[2].bedAmount > 0.9f && herd.Deer()[2].behavior == island::DeerBehavior::Rest;
        }
        Check(bedded, "a deer resting in the shade beds down within three seconds");
        Check(plain.Deer()[2].bedAmount == 0.0f, "a deer resting on its feet never beds down");
        const auto lying = herd.Deer()[2];
        bool risen = false;
        for (int step = 0; step < 12 && !risen; ++step) {
            herd.Update(0.05f, lying.x + 3.0f, lying.y + 1.6f, lying.z);
            risen = herd.Deer()[2].behavior == island::DeerBehavior::Retreat && herd.Deer()[2].bedAmount < 0.1f && !herd.Bedded(2);
        }
        Check(risen, "a bedded deer is up and bolting within half a second of a walker arriving");
    }

    {
        // Deer herd: the antlered sentinel stands alert before its clearing partner, and the
        // partner's wider alert range opens only after the reaction delay.
        const island::IslandTerrain terrain;
        island::DeerHerd herd(terrain, {});
        const auto sentinel = herd.Deer()[0];
        const auto partner = herd.Deer()[1];
        // Approach from the far side of the sentinel, 21 m out: inside its range, outside the
        // partner's ordinary 18 m range.
        const float dx = sentinel.x - partner.x, dz = sentinel.z - partner.z;
        const float apart = std::max(std::hypot(dx, dz), 0.01f);
        const float observerX = sentinel.x + dx / apart * 21.0f;
        const float observerZ = sentinel.z + dz / apart * 21.0f;
        herd.Update(0.05f, observerX, sentinel.y + 1.6f, observerZ);
        Check(herd.SentinelAlert() && herd.SentinelAlertSeconds() < island::DeerHerd::HerdReactionSeconds,
              "deer sentinel notices the observer at once");
        Check(herd.Deer()[0].behavior == island::DeerBehavior::Rest,
              "deer sentinel stands alert before its partner");
        Check(herd.Deer()[1].behavior != island::DeerBehavior::Retreat,
              "deer partner does not bolt at a distant observer");
        for (int step = 0; step < 40; ++step) herd.Update(0.05f, observerX, sentinel.y + 1.6f, observerZ);
        Check(herd.SentinelAlertSeconds() >= island::DeerHerd::HerdReactionSeconds,
              "deer herd reaction delay elapses while the sentinel keeps watching");
        herd.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        Check(!herd.SentinelAlert(), "deer sentinel relaxes once the observer is gone");
        // A coconut's thud with no observer about: the deer 12 m from it stands facing the
        // fall and turns toward it; after the startle fades it is free to graze again.
        // The observer stands 60 m off: near enough for full simulation, too far to alarm.
        for (int step = 0; step < 200; ++step) herd.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        const auto grazer = herd.Deer()[2];
        const float thudX = grazer.x + 12.0f, thudZ = grazer.z;
        herd.NoteDisturbance(thudX, thudZ);
        bool stoodFacing = false;
        for (int step = 0; step < 110; ++step) {
            herd.Update(0.05f, grazer.x, grazer.y + 1.6f, grazer.z + 60.0f);
            const auto& deer = herd.Deer()[2];
            const float toward = std::remainder(std::atan2(thudZ - deer.z, thudX - deer.x) - deer.headingRadians, 6.2831853f);
            if (deer.behavior == island::DeerBehavior::Rest && std::abs(toward) < 0.35f) stoodFacing = true;
        }
        Check(herd.Startled() && stoodFacing, "a thud twelve metres off stops the deer and turns it toward the fall");
        for (int step = 0; step < 80; ++step) herd.Update(0.05f, std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
        Check(!herd.Startled(), "the deer's startle fades after six seconds");
    }

    {
        // The stream cascade: one lip, one concentrated drop, a wider deeper plunge pool, and
        // a water profile that still never rises downstream.
        const island::IslandTerrain terrain;
        const island::StreamCourse course(terrain);
        const std::size_t lip=course.CascadeIndex();
        Check(lip>=13&&lip+island::StreamCourse::PoolLength+6<course.Count(),
              "stream cascade sits inside the authored course");
        Check(course.CascadeDrop()>=0.12f,"stream cascade drops at least twelve centimetres in one metre");
        Check(course.Station(lip).water==course.Station(lip-1).water
              ||course.Station(lip).water==course.Station(lip).ground-island::StreamCourse::BankFreeboard,
              "stream cascade lip holds level or sits at the bank freeboard");
        bool monotone=true,incised=true;
        for(std::size_t i=1;i<course.Count();++i) {
            if(course.Station(i).water>course.Station(i-1).water+1e-5f) monotone=false;
            if(course.Station(i).ground-course.Station(i).bed>island::StreamCourse::PoolIncisionLimit+1e-4f) incised=false;
        }
        Check(monotone,"stream water never rises downstream across the cascade");
        Check(incised,"stream pool incision stays within its limit");
        const auto& pool=course.PoolStation();
        const auto& upstream=course.Station(lip-2);
        Check(pool.water-pool.bed>upstream.water-upstream.bed+0.2f,"plunge pool is deeper than the reach above the lip");
        Check(pool.halfWidth>island::StreamRoute::HalfWidth(pool.x)*1.3f,"plunge pool is wider than the authored channel");
        const island::StreamCascadePoint cascade{course.CascadeX(),course.CascadeZ(),true};
        const auto near=island::StreamSoundField::Sample(course.CascadeX()+3.0f,course.CascadeZ(),cascade);
        const auto far=island::StreamSoundField::Sample(course.CascadeX()+45.0f,course.CascadeZ()+30.0f,cascade);
        const auto farQuiet=island::StreamSoundField::Sample(course.CascadeX()+45.0f,course.CascadeZ()+30.0f);
        Check(near.turbulence>0.9f&&far.presence>=farQuiet.presence,
              "stream cascade is turbulent up close and audible further than the quiet reaches");
    }

    {
        // The wreck scour bar: a bounded replacement that meets the coarse terrain at its
        // edge, breaks the surface only on the lee side and never digs below the seabed.
        const island::IslandTerrain terrain;
        const island::RenderedTerrainSurface rendered(terrain);
        const island::ShipwreckLayout wreck(terrain);
        const island::WreckScourPatch scour(wreck,rendered);
        const auto& ship=wreck.Ship();
        Check(scour.Contains(ship.x,ship.z)&&!scour.Vertices().empty()&&scour.Indices().size()%3==0,
              "wreck scour patch covers the hull with a triangle mesh");
        bool boundaryFlat=true,neverLower=true,capped=true,seawardSubmerged=true;
        float highest=-10.0f;
        for(const auto& v:scour.Vertices()) {
            const float base=rendered.Sample(v.x,v.z).height;
            if(scour.IsBoundary(v)&&v.y!=base) boundaryFlat=false;
            if(v.y<base) neverLower=false;
            if(v.y>std::max(base,island::WreckScourPatch::LeeCap)+1e-4f) capped=false;
            highest=std::max(highest,v.y);
            const float dx=v.x-ship.x,dz=v.z-ship.z;
            const float outward=dx*std::cos(island::ShipwreckLayout::AuthoredCoastAngle)
                +dz*std::sin(island::ShipwreckLayout::AuthoredCoastAngle);
            if(outward>3.0f&&v.y>base&&v.y>0.0f) seawardSubmerged=false;
        }
        Check(boundaryFlat,"wreck scour boundary vertices keep the original terrain height");
        Check(neverLower,"wreck scour never lowers the seabed");
        Check(capped,"wreck scour bar never rises above the lee cap");
        Check(highest>0.05f,"wreck scour bar breaks the surface beside the hull");
        Check(seawardSubmerged,"wreck scour stays submerged on the seaward side");
        const float keel=ship.y-1.35f;
        Check(scour.SampleHeight(ship.x,ship.z)>keel,"scour sand rises above the buried keel");
    }

    {
        // The sun shadows' far fade (loop pass 206): receivers keep a continuous share of the
        // map's shadow while the viewer flies across many 8 m re-centres of the map, where the
        // square edge fade it replaces jumped; and nothing near the viewer is faded at all.
        namespace fade = island::sun_shadow_fade;
        const float sx = island::environment::SunDirectionX, sy = island::environment::SunDirectionY,
                    sz = island::environment::SunDirectionZ;
        const float heights[] = {0.0f, 12.0f, 26.0f, 40.0f};   // the island rises to 40 m
        struct Receiver { float x, h, z; };
        std::vector<Receiver> receivers;
        for (float x = -96.0f; x <= 96.0f; x += 6.0f)
            for (float z = -96.0f; z <= 96.0f; z += 6.0f)
                for (const float h : heights) receivers.push_back({x, h, z});
        const auto oldSquare = [&](const Receiver& r, const float vx, const float vz) {
            const auto o = fade::Offset(r.x, r.h, r.z, fade::AnchorOf(vx), fade::AnchorOf(vz), sx, sy, sz);
            const float edge = (fade::MapRadius - std::max(std::abs(o.across), std::abs(o.up))) / (2.0f * fade::MapRadius);
            return edge <= 0.0f ? 0.0f : fade::SmoothStep(0.0f, 0.06f, edge);
        };
        std::vector<float> previous(receivers.size()), previousOld(receivers.size());
        float largestStep = 0.0f, largestOldStep = 0.0f;
        bool nearFull = true;
        int anchorChanges = 0;
        float lastAnchorX = 0, lastAnchorZ = 0;
        for (int step = 0; step <= 2000; ++step) {
            // a 40 m by 26 m flight line, 2 cm per step
            const float t = static_cast<float>(step) / 2000.0f;
            const float vx = -20.0f + 40.0f * t, vz = -13.0f + 26.0f * t;
            if (step > 0 && (fade::AnchorOf(vx) != lastAnchorX || fade::AnchorOf(vz) != lastAnchorZ)) ++anchorChanges;
            lastAnchorX = fade::AnchorOf(vx);
            lastAnchorZ = fade::AnchorOf(vz);
            for (std::size_t i = 0; i < receivers.size(); ++i) {
                const auto& r = receivers[i];
                const float strength = fade::Strength(r.x, r.h, r.z, vx, vz, sx, sy, sz);
                const float old = oldSquare(r, vx, vz);
                if (step > 0) {
                    largestStep = std::max(largestStep, std::abs(strength - previous[i]));
                    largestOldStep = std::max(largestOldStep, std::abs(old - previousOld[i]));
                }
                previous[i] = strength;
                previousOld[i] = old;
                if (std::hypot(r.x - vx, r.z - vz) <= 40.0f && strength != 1.0f) nearFull = false;
            }
        }
        Check(anchorChanges >= 6, "the sun shadow fade sweep crosses several of the map's re-centres");
        Check(largestStep <= 0.01f,
              "the sun shadows' far fade changes by at most 1 % per 2 cm of flight across the map's re-centres");
        Check(largestOldStep > 0.5f, "the square edge fade it replaces jumped at a re-centre (the check discriminates)");
        Check(nearFull, "the sun shadows keep full strength within 40 m of the viewer at every height of the island");
        std::cout << "sun shadow far fade: largest step " << largestStep << " per 2 cm across " << anchorChanges
                  << " re-centres (square edge fade: " << largestOldStep << ")\n";
    }

    {
        // The walker's sun shadow is a figure laid along the light: some 1.2 m long under the
        // afternoon sun, over six at a sun of fifteen degrees, none from a sun on the horizon.
        namespace ws = island::walker_shadow;
        const ws::Pose standing{0.0f, 1.0f, 0.0f, 0.0f};
        const ws::Shape afternoon = ws::Build(standing, -0.45f, 0.82f, -0.34f);
        const auto at = [](const ws::Shape& shape, const float along, const float across) {
            return ws::SignedDistance(shape, shape.alongX * along - shape.alongZ * across,
                                      shape.alongZ * along + shape.alongX * across);
        };
        Check(afternoon.cast && std::abs(afternoon.length - 1.2f) < 0.05f
                  && at(afternoon, 0.93f * afternoon.length, 0.0f) < 0.0f
                  && at(afternoon, 0.5f * afternoon.length, 0.0f) < 0.0f
                  && at(afternoon, 1.25f * afternoon.length, 0.0f) > 0.0f
                  && at(afternoon, -0.5f * afternoon.length, 0.0f) > 0.0f
                  && afternoon.alongX > 0.0f && afternoon.alongZ > 0.0f,
              "the afternoon sun lays the walker's figure some 1.2 m long away from itself");
        const float low = std::sin(15.0f * std::numbers::pi_v<float> / 180.0f);
        const ws::Shape evening = ws::Build(standing, std::sqrt(1.0f - low * low), low, 0.0f);
        Check(evening.cast && evening.length > 6.0f && evening.length < 7.0f
                  && at(evening, 0.93f * evening.length, 0.0f) < 0.0f && evening.alongX < -0.99f,
              "a fifteen-degree sun lays it over six metres long");
        Check(!ws::Build(standing, 1.0f, 0.05f, 0.0f).cast && !ws::Build(standing, 0.0f, -1.0f, 0.0f).cast,
              "a sun on or below the horizon lays no figure");
        // Side-lit, the striding legs part in the shadow and close again as the walker stands.
        const ws::Shape stride = ws::Build({0.0f, 1.0f, std::numbers::pi_v<float> * 0.5f, 1.0f}, 1.0f, 1.0f, 0.0f);
        const ws::Shape still = ws::Build({0.0f, 1.0f, std::numbers::pi_v<float> * 0.5f, 0.0f}, 1.0f, 1.0f, 0.0f);
        // The crabs' figures: feet on the sand, low bodies, the body's shadow inside the figure.
        {
            island::CrabState beach;
            beach.scale = 0.7f;
            island::HermitCrabState hermit;
            hermit.scale = 0.45f;
            island::CoconutCrabState coconut;
            const std::array<std::pair<std::array<ws::Limb, ws::CapsuleCount>, float>, 3> crabs{{
                {ws::GhostCrabLimbs(beach), 0.08f * 0.7f}, {ws::HermitCrabLimbs(hermit), 0.11f * 0.45f},
                {ws::CoconutCrabLimbs(coconut), 0.245f}}};
            bool grounded = true;
            for (const auto& [limbs, bodyHeight] : crabs) {
                float lowest = 1e9f, highest = -1e9f;
                for (const auto& limb : limbs) {
                    if (!(limb.radius > 0.0f)) continue;
                    lowest = std::min({lowest, limb.ay, limb.by});
                    highest = std::max({highest, limb.ay + limb.radius, limb.by + limb.radius});
                }
                const auto figure = ws::Lay(limbs, 0.0f, 0.0f, 0.0f, -0.45f, 0.82f, -0.34f, 0.4f);
                grounded = grounded && std::abs(lowest) < 1e-4f && highest < 0.5f
                    && ws::SignedDistance(figure, 0.45f / 0.82f * bodyHeight, 0.34f / 0.82f * bodyHeight) < 0.0f;
            }
            Check(grounded, "the crabs' figures stand on the sand, low, with the body's shadow inside");
        }
        {
            // A ground dove's figure stands under its body (kept 0.20 of its scale over the
            // feet), and a heron's on its two long legs with the head above the body.
            island::BirdState dove;
            dove.scale = 0.85f;
            dove.y = ws::GroundBirdBodyHeight * dove.scale;
            const auto doveLimbs = ws::GroundBirdLimbs(dove);
            island::ReefHeronState heron;
            const auto heronLimbs = ws::HeronLimbs(heron);
            const auto extent = [](const std::array<ws::Limb, ws::CapsuleCount>& limbs) {
                float lowest = 1e9f, highest = -1e9f;
                for (const auto& limb : limbs) {
                    if (!(limb.radius > 0.0f)) continue;
                    lowest = std::min({lowest, limb.ay, limb.by});
                    highest = std::max({highest, limb.ay + limb.radius, limb.by + limb.radius});
                }
                return std::array<float, 2>{lowest, highest};
            };
            const auto doveExtent = extent(doveLimbs), heronExtent = extent(heronLimbs);
            const auto heronFigure = ws::Lay(heronLimbs, 0.0f, 0.0f, 0.0f, -0.45f, 0.82f, -0.34f, 0.75f);
            const float bodyHeight = heronLimbs[0].ay;
            Check(std::abs(doveExtent[0]) < 1e-4f && doveExtent[1] > 0.3f && doveExtent[1] < 0.5f
                      && std::abs(heronExtent[0]) < 1e-4f && heronExtent[1] > 0.5f && heronExtent[1] < 0.8f
                      && heronLimbs[3].ay > bodyHeight
                      && ws::SignedDistance(heronFigure, heronLimbs[0].ax + 0.45f / 0.82f * bodyHeight,
                                            heronLimbs[0].az + 0.34f / 0.82f * bodyHeight) < 0.0f,
                  "a ground dove's and a heron's figures stand on their feet");
        }
        {
            // The small ground animals' figures: a skink's, a frog's and a rat's feet on the
            // ground and their bodies low, the trunk's shadow inside each figure.
            island::SkinkState skink;
            skink.scale = 0.4f;
            island::FrogState frog;
            frog.scale = 0.33f;
            island::RodentState rat;
            rat.species = island::RodentSpecies::Rat;
            const std::array<std::pair<std::array<ws::Limb, ws::CapsuleCount>, float>, 3> animals{{
                {ws::SkinkLimbs(skink), 0.06f * 0.4f}, {ws::FrogLimbs(frog), 0.10f * 0.33f}, {ws::RodentLimbs(rat), 0.095f}}};
            bool low = true;
            for (const auto& [limbs, bodyHeight] : animals) {
                float lowest = 1e9f, highest = -1e9f;
                for (const auto& limb : limbs) {
                    if (!(limb.radius > 0.0f)) continue;
                    lowest = std::min({lowest, limb.ay, limb.by});
                    highest = std::max({highest, limb.ay + limb.radius, limb.by + limb.radius});
                }
                const auto figure = ws::Lay(limbs, 0.0f, 0.0f, 0.0f, -0.45f, 0.82f, -0.34f, 0.2f);
                low = low && std::abs(lowest) < 1e-4f && highest < 0.2f
                    && ws::SignedDistance(figure, 0.45f / 0.82f * bodyHeight, 0.34f / 0.82f * bodyHeight) < 0.0f;
            }
            Check(low, "the skink's, the frog's and the rat's figures stand on the ground, low");
        }
        Check(at(stride, 0.25f, 0.0f) > 0.08f && at(still, 0.25f, 0.0f) < 0.05f
                  && at(stride, 1.62f, 0.0f) < 0.0f && at(still, 1.62f, 0.0f) < 0.0f,
              "side-lit, the striding legs part in the walker's shadow and the head stays in it");
    }

    {
        // Footprints in mud: the stream's banks always, any bare soil once the rain has soaked
        // it, and never grass, rock or the open ground far from the stream in the dry.
        namespace fg = island::footprint_ground;
        const float x = island::StreamRoute::SourceX - 40.0f, z = island::StreamRoute::CentreZ(x);
        const float bank = z + island::StreamRoute::HalfWidth(x) + 0.8f, away = z + 25.0f;
        Check(fg::Muddy(island::FootstepSurface::Soil, x, bank, 0.0f) && !fg::Muddy(island::FootstepSurface::Soil, x, away, 0.0f)
                  && fg::Muddy(island::FootstepSurface::Soil, x, away, 0.6f) && !fg::Muddy(island::FootstepSurface::Grass, x, bank, 0.6f)
                  && !fg::Muddy(island::FootstepSurface::Rock, x, away, 0.9f)
                  && !fg::Muddy(island::FootstepSurface::Soil, island::StreamRoute::SourceX + 30.0f, z, 0.0f),
              "footprints press into the stream's muddy banks and into rain-soaked soil, not grass or rock");
    }

    {
        // The frogs call most on a wet night: one frog by the listener, five minutes each.
        island::FrogState frog;
        frog.x = 3.0f; frog.y = 0.0f; frog.z = 0.0f;
        frog.simulationTier = island::WildlifeSimulationTier::Full;
        const std::array<island::FrogState, 1> frogs{frog};
        const auto calls = [&](const float nightness, const float wetness) {
            island::FrogCallScheduler scheduler;
            scheduler.SetChorus(nightness, wetness);
            int count = 0;
            for (int step = 0; step < 3000; ++step)
                count += scheduler.Update(0.1f, 0.0f, 1.6f, 0.0f, frogs, true).trigger ? 1 : 0;
            return count;
        };
        const int dryDay = calls(0.0f, 0.0f), wetNight = calls(1.0f, 1.0f);
        std::cout << "frog calls in five minutes: dry day " << dryDay << ", wet night " << wetNight << '\n';
        Check(dryDay >= 15 && dryDay <= 45 && wetNight >= 3 * dryDay,
              "the frogs call several times as often on a wet night as by a dry day");
    }

    {
        // Morning mist in the stream's valley: thick at first light after a dewy night, none at
        // noon or after a dry clear night, burnt off by half past nine; the banks lie over the
        // stream, a knee's to a head's height up.
        namespace vm = island;
        Check(vm::ValleyMistField::Strength(6.2f, 0.45f, 0.0f) > 0.8f && vm::ValleyMistField::Strength(12.0f, 0.45f, 0.9f) == 0.0f
                  && vm::ValleyMistField::Strength(9.6f, 0.5f, 1.0f) == 0.0f && vm::ValleyMistField::Strength(6.2f, 0.0f, 0.0f) == 0.0f
                  && vm::ValleyMistField::Strength(6.2f, 0.1f, 0.9f) > vm::ValleyMistField::Strength(6.2f, 0.1f, 0.0f),
              "the valley mist lies thick at first light after a dewy or rainy night and burns off by half past nine");
        island::ValleyMistField mist(terrain);
        for (int step = 0; step < 50; ++step) mist.Update(0.1f, 6.2f, 0.45f, 0.0f);
        bool overStream = true, dense = true;
        for (const auto& puff : mist.Puffs()) {
            const float lift = puff.y - terrain.Height(puff.x, puff.z);
            overStream = overStream && std::abs(puff.z - island::StreamRoute::CentreZ(puff.x)) < 4.0f
                && lift > 0.2f && lift < 2.5f;
            dense = dense && puff.active && puff.density > 0.15f;
        }
        Check(overStream && dense, "the morning mist banks lie over the stream, a knee's to a head's height up");
    }
    {
        // Steam off the sunlit wet ground after rain: only with the ground wet, the rain
        // stopped and the sun out and up; the wisps start on the open ground round the walker
        // (none where the ground probe refuses), rise, thin away within their life, and the
        // first update already shows a full set.
        using island::SunSteamField;
        Check(SunSteamField::Strength(0.85f, 0.0f, 1.0f, 50.0f) > 0.95f && SunSteamField::Strength(0.1f, 0.0f, 1.0f, 50.0f) == 0.0f
                  && SunSteamField::Strength(0.85f, 0.5f, 1.0f, 50.0f) == 0.0f && SunSteamField::Strength(0.85f, 0.0f, 0.0f, 50.0f) == 0.0f
                  && SunSteamField::Strength(0.85f, 0.0f, 1.0f, 5.0f) == 0.0f,
              "the wet ground steams only with the rain stopped and the sun out and up");
        const SunSteamField::GroundProbe open = [](float, float, float& y) { y = 3.0f; return true; };
        const SunSteamField::GroundProbe shaded = [](float, float, float&) { return false; };
        SunSteamField steam, refused;
        steam.Update(0.1f, 10.0f, -20.0f, 1.0f, -1.0f, 0.0f, open);
        refused.Update(0.1f, 10.0f, -20.0f, 1.0f, -1.0f, 0.0f, shaded);
        bool round = true;
        for (const auto& puff : steam.Puffs()) {
            if (!puff.active) continue;
            const float distance = std::hypot(puff.x - 10.0f, puff.z + 20.0f);
            round = round && distance > SunSteamField::NearClear - 0.5f && distance < SunSteamField::Reach + 2.0f
                && puff.y > 3.0f && puff.y < 5.0f && puff.density >= 0.0f && puff.density < 0.5f;
        }
        const std::size_t warm = steam.ActiveCount();
        for (int step = 0; step < 80; ++step) steam.Update(0.1f, 10.0f, -20.0f, 0.0f, -1.0f, 0.0f, open);
        Check(warm >= SunSteamField::Capacity * 3 / 4 && round && refused.ActiveCount() == 0 && steam.ActiveCount() == 0,
              "steam wisps rise from the open ground round the walker, none where refused, all gone a life after the sun goes");
    }

    if (failures != 0) {
        std::cerr << failures << " core test assertion(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "IslandCoreTests: all checks passed\n";
    return EXIT_SUCCESS;
}
