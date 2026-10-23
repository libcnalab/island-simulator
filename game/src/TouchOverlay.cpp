#include "IslandSimulator/TouchOverlay.hpp"

#include "IslandSimulator/GeneratedTextureBudget.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int TextureSize = GeneratedTextureBudget::TouchCircleSize;

        [[nodiscard]] Color PremultipliedColor(const int red, const int green,
                                               const int blue, const int alpha) noexcept
        {
            // SpriteBatch's default AlphaBlend state expects premultiplied source RGB. Keeping
            // transparent texels white would add an opaque rectangular quad on GLES as well as
            // producing bright fringes around the antialiased circles on every renderer.
            const auto premultiply = [alpha](const int channel) {
                return static_cast<int>(std::lround(
                    static_cast<float>(channel * alpha) / 255.0f));
            };
            return Color(premultiply(red), premultiply(green), premultiply(blue), alpha);
        }

        [[nodiscard]] std::unique_ptr<Texture2D> CreateCircleTexture(
            GraphicsDevice& device, const bool ring)
        {
            std::vector<Color> pixels(static_cast<std::size_t>(TextureSize * TextureSize),
                                      Color(0, 0, 0, 0));
            constexpr float centre = static_cast<float>(TextureSize - 1) * 0.5f;
            constexpr float outerRadius = centre - 1.0f;
            constexpr float innerRadius = outerRadius - 5.0f;
            for (int y = 0; y < TextureSize; ++y) {
                for (int x = 0; x < TextureSize; ++x) {
                    const float distance = std::hypot(static_cast<float>(x) - centre,
                                                      static_cast<float>(y) - centre);
                    const float outerCoverage = std::clamp(outerRadius + 1.0f - distance,
                                                           0.0f, 1.0f);
                    float coverage = outerCoverage;
                    if (ring) {
                        const float innerCoverage = std::clamp(distance - innerRadius,
                                                               0.0f, 1.0f);
                        coverage *= innerCoverage;
                    }
                    const int alpha = static_cast<int>(std::lround(coverage * 255.0f));
                    pixels[static_cast<std::size_t>(y * TextureSize + x)] =
                        PremultipliedColor(255, 255, 255, alpha);
                }
            }

            auto texture = std::make_unique<Texture2D>(device, TextureSize, TextureSize);
            texture->SetData(pixels.data(), static_cast<int>(pixels.size()));
            return texture;
        }

        [[nodiscard]] Rectangle CentredRectangle(const float x, const float y,
                                                  const float radius)
        {
            const int diameter = std::max(1, static_cast<int>(std::lround(radius * 2.0f)));
            return Rectangle(static_cast<int>(std::lround(x - radius)),
                             static_cast<int>(std::lround(y - radius)), diameter, diameter);
        }
    }

    TouchOverlay::TouchOverlay(GraphicsDevice& device)
        : spriteBatch_(std::make_unique<SpriteBatch>(device)),
          ringTexture_(CreateCircleTexture(device, true)),
          discTexture_(CreateCircleTexture(device, false))
    {
    }

    TouchOverlay::~TouchOverlay() = default;

    void TouchOverlay::Draw(const TouchControlState& state, const float inputWidth,
                            const float inputHeight, const int viewportWidth,
                            const int viewportHeight, const bool showIdleHints,
                            const TouchActionDisplay& actions)
    {
        if (viewportWidth <= 0 || viewportHeight <= 0
            || (!showIdleHints && !state.movementActive && !state.lookActive)) {
            return;
        }

        const float safeInputWidth = std::max(1.0f, inputWidth);
        const float safeInputHeight = std::max(1.0f, inputHeight);
        const float scaleX = static_cast<float>(viewportWidth) / safeInputWidth;
        const float scaleY = static_cast<float>(viewportHeight) / safeInputHeight;
        const float radius = static_cast<float>(viewportHeight)
            * TouchControls::MovementRadiusFraction;
        const float knobRadius = radius * 0.34f;
        const float edgeInset = static_cast<float>(viewportHeight) * 0.04f;
        const float idleY = static_cast<float>(viewportHeight) - radius - edgeInset;
        const float idleLeftX = radius * 1.35f;
        const float idleRightX = static_cast<float>(viewportWidth) - radius * 1.35f;
        const TouchSensitivitySelectorLayout inputSelector =
            TouchControls::SensitivitySelectorLayout(safeInputWidth, safeInputHeight);
        const float selectorX = inputSelector.x * scaleX;
        const float selectorY = inputSelector.y * scaleY;
        const float selectorRadius = inputSelector.radius * scaleY * 0.72f;

        float movementCentreX = idleLeftX;
        float movementCentreY = idleY;
        float movementKnobX = movementCentreX;
        float movementKnobY = movementCentreY;
        if (state.movementActive) {
            const float rawCentreX = state.movementAnchorX * scaleX;
            const float rawCentreY = state.movementAnchorY * scaleY;
            movementCentreX = std::clamp(rawCentreX, radius + edgeInset,
                                         static_cast<float>(viewportWidth) - radius - edgeInset);
            movementCentreY = std::clamp(rawCentreY, radius + edgeInset,
                                         static_cast<float>(viewportHeight) - radius - edgeInset);
            float deltaX = state.movementTouchX * scaleX - rawCentreX;
            float deltaY = state.movementTouchY * scaleY - rawCentreY;
            const float distance = std::hypot(deltaX, deltaY);
            if (distance > radius) {
                deltaX *= radius / distance;
                deltaY *= radius / distance;
            }
            movementKnobX = movementCentreX + deltaX;
            movementKnobY = movementCentreY + deltaY;
        }

        float lookX = idleRightX;
        float lookY = idleY;
        if (state.lookActive) {
            const float lookRadius = radius * 0.82f;
            lookX = std::clamp(state.lookTouchX * scaleX, lookRadius + edgeInset,
                               static_cast<float>(viewportWidth) - lookRadius - edgeInset);
            lookY = std::clamp(state.lookTouchY * scaleY, lookRadius + edgeInset,
                               static_cast<float>(viewportHeight) - lookRadius - edgeInset);
        }

        const Color movementRingColor = state.runRequested
            ? PremultipliedColor(255, 208, 112, 168)
            : state.movementActive
                ? PremultipliedColor(99, 235, 190, 150)
                : PremultipliedColor(235, 250, 244, 62);
        const Color movementKnobColor = state.runRequested
            ? PremultipliedColor(255, 244, 205, 188)
            : state.movementActive
                ? PremultipliedColor(225, 255, 243, 170)
                : PremultipliedColor(235, 250, 244, 54);

        spriteBatch_->Begin();
        spriteBatch_->Draw(*ringTexture_, CentredRectangle(movementCentreX, movementCentreY, radius),
                           movementRingColor);
        spriteBatch_->Draw(*discTexture_, CentredRectangle(movementKnobX, movementKnobY, knobRadius),
                           movementKnobColor);
        spriteBatch_->Draw(*ringTexture_, CentredRectangle(lookX, lookY, radius * 0.82f),
                           state.lookActive
                               ? PremultipliedColor(110, 220, 255, 135)
                               : PremultipliedColor(235, 250, 255, 50));
        spriteBatch_->Draw(*discTexture_, CentredRectangle(lookX, lookY, knobRadius * 0.72f),
                           state.lookActive
                               ? PremultipliedColor(225, 250, 255, 150)
                               : PremultipliedColor(235, 250, 255, 40));
        spriteBatch_->Draw(*ringTexture_,
                           CentredRectangle(selectorX, selectorY, selectorRadius),
                           state.sensitivitySelectorActive || state.lookSensitivityChanged
                               ? PremultipliedColor(255, 220, 130, 180)
                               : PremultipliedColor(235, 250, 255, 62));
        constexpr std::array<TouchLookSensitivityLevel, 3> levels = {
            TouchLookSensitivityLevel::Low,
            TouchLookSensitivityLevel::Standard,
            TouchLookSensitivityLevel::High,
        };
        const float dotRadius = selectorRadius * 0.13f;
        for (std::size_t index = 0; index < levels.size(); ++index) {
            const float offset = (static_cast<float>(index) - 1.0f) * selectorRadius * 0.48f;
            const bool selected = state.lookSensitivity == levels[index];
            spriteBatch_->Draw(*discTexture_,
                               CentredRectangle(selectorX + offset, selectorY, dotRadius),
                               selected
                                   ? PremultipliedColor(225, 250, 255, 205)
                                   : PremultipliedColor(235, 250, 255, 58));
        }

        // The action buttons under the selector, each a ring round a small glyph drawn from the
        // same two circles: the quad as two wheels under its rider's head, the torch as a lamp.
        const auto button = [&](const TouchAction action, float& x, float& y, float& r) {
            const TouchSensitivitySelectorLayout layout =
                TouchControls::ActionButtonLayout(action, safeInputWidth, safeInputHeight);
            x = layout.x * scaleX;
            y = layout.y * scaleY;
            r = layout.radius * scaleY * 0.72f;
        };
        const Color glyphIdle = PremultipliedColor(235, 250, 255, 120);
        const Color glyphLit = PremultipliedColor(255, 236, 170, 225);
        float x = 0.0f, y = 0.0f, r = 0.0f;
        if (actions.rideShown) {
            button(TouchAction::Ride, x, y, r);
            spriteBatch_->Draw(*ringTexture_, CentredRectangle(x, y, r),
                               actions.riding ? PremultipliedColor(255, 208, 112, 180)
                                              : PremultipliedColor(235, 250, 255, 62));
            const Color glyph = actions.riding ? glyphLit : glyphIdle;
            spriteBatch_->Draw(*ringTexture_, CentredRectangle(x - r * 0.36f, y + r * 0.22f, r * 0.27f), glyph);
            spriteBatch_->Draw(*ringTexture_, CentredRectangle(x + r * 0.36f, y + r * 0.22f, r * 0.27f), glyph);
            spriteBatch_->Draw(*discTexture_, CentredRectangle(x, y - r * 0.12f, r * 0.13f), glyph);
            spriteBatch_->Draw(*discTexture_, CentredRectangle(x - r * 0.04f, y - r * 0.40f, r * 0.12f), glyph);
        }
        if (actions.torchShown) {
            button(TouchAction::Torch, x, y, r);
            spriteBatch_->Draw(*ringTexture_, CentredRectangle(x, y, r),
                               actions.torchOn ? PremultipliedColor(255, 208, 112, 180)
                                               : PremultipliedColor(235, 250, 255, 62));
            if (actions.torchOn) {
                spriteBatch_->Draw(*discTexture_, CentredRectangle(x, y, r * 0.62f),
                                   PremultipliedColor(255, 230, 150, 70));
            }
            spriteBatch_->Draw(*discTexture_, CentredRectangle(x, y, r * 0.30f),
                               actions.torchOn ? glyphLit : glyphIdle);
        }
        if (actions.journalShown) {
            // the journal as a short list: three rows of a bullet and its line
            button(TouchAction::Journal, x, y, r);
            spriteBatch_->Draw(*ringTexture_, CentredRectangle(x, y, r),
                               actions.journalOpen ? PremultipliedColor(255, 208, 112, 180)
                                                   : PremultipliedColor(235, 250, 255, 62));
            const Color glyph = actions.journalOpen ? glyphLit : glyphIdle;
            for (const float row : {-0.32f, 0.0f, 0.32f}) {
                spriteBatch_->Draw(*discTexture_, CentredRectangle(x - r * 0.30f, y + r * row, r * 0.08f), glyph);
                for (const float along : {-0.08f, 0.06f, 0.20f, 0.34f})
                    spriteBatch_->Draw(*discTexture_, CentredRectangle(x + r * along, y + r * row, r * 0.06f), glyph);
            }
        }
        spriteBatch_->End();
    }
}
