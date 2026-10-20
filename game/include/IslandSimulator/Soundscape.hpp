#pragma once

#include <memory>
#include <limits>
#include <span>
#include <string>

#include "IslandSimulator/BirdFlock.hpp"
#include "IslandSimulator/MonkeyTroop.hpp"
#include "IslandSimulator/SoundscapeModel.hpp"
#include "IslandSimulator/StreamSoundField.hpp"

namespace Microsoft::Xna::Framework::Audio
{
    class SoundEffect;
    class SoundEffectInstance;
}

namespace island
{
    /** @brief Renderer-independent camera state consumed by positional audio. */
    struct SoundscapeListenerState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float forwardX = 0.0f;
        float forwardY = 0.0f;
        float forwardZ = -1.0f;
        float velocityX = 0.0f;
        float velocityZ = 0.0f;
    };

    /** @brief Read-only runtime state consumed by opt-in audio diagnostics. */
    struct SoundscapeDebugState
    {
        WildlifeAudioSource nearestBird;
        WildlifeAudioSource nearestMonkey;
        WildlifeCallRequest activeWildlifeCall;
        int playingSourceCount = 0;
        int sourceBudget = 6;
        bool campfirePlaying = false;
        float campfireVolume=0;
        bool bunkerHumPlaying=false;
        float bunkerHumVolume=0;
        UnderwaterAudioMix underwaterAudio;
    };

    /** @brief Owns the procedural ambient layers and their CNA playback instances. */
    class Soundscape final
    {
    public:
        static constexpr int PersistentSourceCount = 4;
        static constexpr int MaximumPlayingSourceCount = 6;

        Soundscape();
        ~Soundscape();

        Soundscape(const Soundscape&) = delete;
        Soundscape& operator=(const Soundscape&) = delete;
        Soundscape(Soundscape&&) = delete;
        Soundscape& operator=(Soundscape&&) = delete;

        /** @brief Builds PCM and begins the looping layers, or records a graceful device failure. */
        bool Start(float signedShoreDistance, bool swimming, float waterDepth,
                   float horizontalSpeed, float oceanCurrentStrength,
                   float coastalWindExposure=.5f,
                   float playerX=0.0f, float playerZ=0.0f,
                   float playerY=std::numeric_limits<float>::quiet_NaN()) noexcept;

        /** @brief Advances location-driven crossfades for active playback. */
        void Update(float signedShoreDistance, bool swimming, float waterDepth,
                    float horizontalSpeed, float oceanCurrentStrength, float deltaSeconds,
                    float coastalWindExposure,
                    const SoundscapeListenerState& listener,
                    std::span<const BirdState> birds,
                    std::span<const MonkeyState> monkeys,
                    std::span<const BoarState> boars,
                    std::span<const GoatState> goats,
                    std::span<const SnakeState> snakes,
                    const BearState* bear=nullptr,
                    float submergedDepth=0.0f,
                    std::span<const FrogState> frogs={},
                    std::span<const CicadaState> cicadas={},
                    std::span<const FlyState> flies={}) noexcept;

        /** @brief Refreshes diagnostic emitter candidates even when audio playback is disabled. */
        void RefreshDebugSources(float listenerX, float listenerY, float listenerZ,
                                 std::span<const BirdState> birds,
                                 std::span<const MonkeyState> monkeys) noexcept;

        /** @brief Names the ground under the player's feet for the next footsteps (None = shore rule). */
        void SetFootstepGround(FootstepSurface ground) noexcept { footstepGround_ = ground; }
        /** @brief A footfall of the resident, where it landed and on what: played through the
         *  footstep voice when the player's own step does not need it, heard within about
         *  twenty-four metres. */
        void NoteResidentStep(float x, float y, float z, FootstepSurface surface) noexcept;
        [[nodiscard]] int PlayedResidentStepCount() const noexcept { return playedResidentStepCount_; }
        /** @brief The clock the gust fronts run on (the same one the grass and palms use). */
        void SetWindClock(float seconds) noexcept { windSeconds_ = seconds; }
        [[nodiscard]] float GustStrength() const noexcept { return gustStrength_; }

        /** @brief Supplies the immutable grounded-wreck emitter position. */
        void SetShipwreckSource(float x, float y, float z) noexcept;
        /** @brief The stream cascade lip; its white water carries further than the quiet reaches. */
        void SetStreamCascade(float x, float z) noexcept;
        /** @brief A seabird has just struck the water here; played once when the slot is free. */
        void NoteBirdSplash(float x, float y, float z) noexcept;
        /** @brief A coconut has just hit the ground here; played once when the slot is free. */
        void NoteCoconutThud(float x, float y, float z) noexcept;
        /** @brief A fish breaking the surface: the splash clip, small, quick and high, heard
         *  within about twenty-five metres. */
        void NoteFishBreach(float x, float y, float z) noexcept;
        /** @brief A monkey breaking into flight from the walker: its alarm call, the troop's
         *  second phrase pitched up and loud, heard within about forty metres. */
        void NoteMonkeyAlarm(float x, float y, float z) noexcept;
        /** @brief The deer sentinel coming alert: its snort, placed where it stands, heard
         *  within about fifty metres. */
        void NoteDeerSnort(float x, float y, float z) noexcept;
        /** @brief The resident whistling a phrase on the walk, where they are; heard within
         *  about thirty metres. */
        void NoteResidentWhistle(float x, float y, float z) noexcept;
        /** @brief A bird flushing from a perch or the ground, where it was: its wing clap,
         *  heard within about twenty metres. */
        void NoteWingClap(float x, float y, float z) noexcept;
        [[nodiscard]] int PlayedWingClapCount() const noexcept { return playedWingClapCount_; }
        [[nodiscard]] int PlayedResidentWhistleCount() const noexcept { return playedResidentWhistleCount_; }
        /** @brief The goat sentinel standing alert at the walker: its alarm bleat goes next. */
        void NoteGoatAlarm(std::size_t animalIndex) noexcept;
        /** @brief A boar the bear sent off: its alarm grunt goes next. */
        void NoteBoarAlarm(std::size_t animalIndex) noexcept;

        /** @brief Supplies the sampled portal elevation for underground acoustic zoning. */
        void SetBunkerSource(float entranceGroundY) noexcept;

        /** @brief Toggles user-requested silence without destroying the active loops. */
        void ToggleMuted() noexcept;

        /** @brief Applies a persisted mute preference without relying on a synthetic toggle. */
        void SetMuted(bool muted) noexcept;

        [[nodiscard]] bool IsAvailable() const noexcept { return available_; }
        [[nodiscard]] bool IsMuted() const noexcept { return muted_; }
        [[nodiscard]] bool WasStartAttempted() const noexcept { return startAttempted_; }
        [[nodiscard]] const SoundscapeMix& Mix() const noexcept { return model_.Mix(); }
        [[nodiscard]] const WaterMovementMix& WaterMix() const noexcept
        {
            return waterMovementModel_.Mix();
        }
        [[nodiscard]] const std::string& Error() const noexcept { return error_; }
        [[nodiscard]] int PlayedWaveAccentCount() const noexcept
        {
            return playedWaveAccentCount_;
        }
        [[nodiscard]] int PlayedShipwreckCreakCount() const noexcept
        {
            return playedShipwreckCreakCount_;
        }
        [[nodiscard]] int PlayedWaterTransitionCount() const noexcept
        {
            return playedWaterTransitionCount_;
        }
        [[nodiscard]] int PlayedFootstepCount() const noexcept
        {
            return playedFootstepCount_;
        }
        [[nodiscard]] int PlayedBirdCallCount() const noexcept
        {
            return playedBirdCallCount_;
        }
        [[nodiscard]] int PlayedMonkeyCallCount() const noexcept
        {
            return playedMonkeyCallCount_;
        }
        [[nodiscard]] int PlayedBoarCallCount() const noexcept { return playedBoarCallCount_; }
        [[nodiscard]] int PlayedGoatCallCount() const noexcept { return playedGoatCallCount_; }
        [[nodiscard]] int PlayedSnakeHissCount() const noexcept { return playedSnakeHissCount_; }
        [[nodiscard]] int PlayedBearCallCount() const noexcept{return playedBearCallCount_;}
        [[nodiscard]] int PlayedFrogCallCount() const noexcept{return playedFrogCallCount_;}
        [[nodiscard]] int PlayedDeerSnortCount() const noexcept{return playedDeerSnortCount_;}
        [[nodiscard]] int PlayedCicadaCallCount() const noexcept{return playedCicadaCallCount_;}
        [[nodiscard]] int PlayedFlyBuzzCount() const noexcept{return playedFlyBuzzCount_;}
        [[nodiscard]] bool IsWildlifeCallActive() const noexcept
        {
            return wildlifeSecondsRemaining_ > 0.0f;
        }
        [[nodiscard]] SoundscapeDebugState DebugState() const noexcept;

    private:
        void ApplyVolumes() noexcept;
        void DisableAfterRuntimeFailure(const std::string& message) noexcept;

        SoundscapeModel model_;
        WaterMovementModel waterMovementModel_;
        UnderwaterAudioMix underwaterAudio_;
        WaveAccentScheduler waveScheduler_;
        ShipwreckCreakScheduler shipwreckCreakScheduler_;
        WaterTransitionScheduler waterTransitionScheduler_;
        FootstepScheduler footstepScheduler_;
        WildlifeAudioSource residentStep_;
        FootstepSurface residentStepSurface_ = FootstepSurface::None;
        std::uint8_t residentStepVariant_ = 0U;
        int playedResidentStepCount_ = 0;
        FootstepSurface footstepGround_ = FootstepSurface::None;
        float windSeconds_ = 0.0f;
        float gustStrength_ = 0.0f;
        WildlifeCallScheduler wildlifeScheduler_;
        BoarCallScheduler boarScheduler_;
        GoatCallScheduler goatScheduler_;
        SnakeHissScheduler snakeHissScheduler_;
        BearCallScheduler bearCallScheduler_;
        FrogCallScheduler frogCallScheduler_;
        CicadaCallScheduler cicadaCallScheduler_;
        FlyBuzzScheduler flyBuzzScheduler_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>, 6> boarEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>, 6> goatEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>, 2> snakeHissEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>,6> bearEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>,2> frogEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>,2> cicadaEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>,2> flyBuzzEffects_;
        BoarCallRequest activeBoarCall_;
        GoatCallRequest activeGoatCall_;
        SnakeHissRequest activeSnakeHiss_;
        BearCallRequest activeBearCall_;
        FrogCallRequest activeFrogCall_;
        CicadaCallRequest activeCicadaCall_;
        FlyBuzzRequest activeFlyBuzz_;
        float boarVoiceGain_ = 1.0f;
        float goatVoiceGain_ = 1.0f;
        float snakeVoiceGain_ = 1.0f;
        float bearVoiceGain_=1.0f;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> surfEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> surfInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> breezeEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> breezeInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> jungleEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> jungleInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> streamEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> streamInstance_;
        StreamSoundSample streamSound_;
        StreamCascadePoint streamCascade_;
        bool streamPlaybackActive_ = false;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> campfireEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> campfireInstance_;
        bool campfirePlaybackActive_ = false;
        float campfirePresence_=0,campfireVolume_=0;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> bunkerHumEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> bunkerHumInstance_;
        bool bunkerHumPlaybackActive_=false;
        float bunkerPresence_=0,bunkerHumVolume_=0,bunkerEntranceGroundY_=0;
        bool hasBunkerSource_=false;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> waveEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> waveAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> waveInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> shipwreckCreakEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            shipwreckCreakAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> waterMovementEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>
            waterMovementInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            waterTransitionEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            waterTransitionAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>
            waterTransitionInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> sandFootstepEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            sandFootstepAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> soilFootstepEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            soilFootstepAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> grassFootstepEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> grassFootstepAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> rockFootstepEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> rockFootstepAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> woodFootstepEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> woodFootstepAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> wadeFootstepEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> wadeFootstepAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>
            footstepInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> birdCallEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            birdCallAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> parrotCallEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> doveCallEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> doveCallAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> birdSplashEffect_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>, 2> deerSnortEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>, 2> residentWhistleEffects_;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>, 2> wingClapEffects_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> coconutThudEffect_;
        WildlifeAudioSource pendingSplash_;
        float pendingSplashSeconds_ = 0.0f;
        WildlifeAudioSource pendingPlip_;
        float pendingPlipSeconds_ = 0.0f;
        WildlifeAudioSource pendingAlarm_;
        float pendingAlarmSeconds_ = 0.0f;
        WildlifeAudioSource pendingSnort_;
        float pendingSnortSeconds_ = 0.0f;
        std::uint32_t snortCount_ = 0U;
        WildlifeAudioSource pendingWhistle_;
        float pendingWhistleSeconds_ = 0.0f;
        std::uint32_t whistleCount_ = 0U;
        WildlifeAudioSource pendingClap_;
        float pendingClapSeconds_ = 0.0f;
        std::uint32_t clapCount_ = 0U;
        int playedWingClapCount_ = 0;
        int playedResidentWhistleCount_ = 0;
        WildlifeAudioSource pendingThud_;
        float pendingThudSeconds_ = 0.0f;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            parrotCallAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> monkeyCallEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>
            monkeyCallAlternateEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>
            wildlifeInstance_;
        WildlifeAudioSource nearestBirdSource_;
        WildlifeAudioSource nearestMonkeySource_;
        WildlifeCallRequest activeWildlifeCall_;
        std::string error_;
        bool available_ = false;
        bool muted_ = false;
        bool startAttempted_ = false;
        bool waveAccentsEnabled_ = false;
        bool waveFailureReported_ = false;
        bool waterTransitionsEnabled_ = false;
        bool waterTransitionFailureReported_ = false;
        bool footstepsEnabled_ = false;
        bool footstepFailureReported_ = false;
        bool wildlifeCallsEnabled_ = false;
        bool wildlifeFailureReported_ = false;
        bool underwaterAudioReported_ = false;
        bool sharedLoopRecoveryReported_ = false;
        float waveSecondsRemaining_ = 0.0f;
        float waterTransitionSecondsRemaining_ = 0.0f;
        float footstepSecondsRemaining_ = 0.0f;
        float wildlifeSecondsRemaining_ = 0.0f;
        int playedWaveAccentCount_ = 0;
        int playedShipwreckCreakCount_ = 0;
        int playedWaterTransitionCount_ = 0;
        int playedFootstepCount_ = 0;
        int playedBirdCallCount_ = 0;
        int playedMonkeyCallCount_ = 0;
        int playedBoarCallCount_ = 0;
        int playedGoatCallCount_ = 0;
        int playedSnakeHissCount_ = 0;
        int playedBearCallCount_=0;
        int playedFrogCallCount_=0;
        int playedDeerSnortCount_=0;
        int playedCicadaCallCount_=0;
        int playedFlyBuzzCount_=0;
        float shipwreckX_ = 0.0f;
        float shipwreckY_ = 0.0f;
        float shipwreckZ_ = 0.0f;
        bool hasShipwreckSource_ = false;
    };
}
