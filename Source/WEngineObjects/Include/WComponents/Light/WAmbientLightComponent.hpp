#pragma once

#include "WEngineObjects/WComponent.hpp"

#include <glm/glm.hpp>

#include "WAmbientLightComponent.WEngine.hpp"

namespace wcm::light {
    
    class WENGINEOBJECTS_API WAmbientLightComponent : public WComponent {

        WOBJECT_BODY

    public:

        bool Active() const noexcept {
            return active_;
        }

        void Active(const bool & in_active) noexcept {
            active_ = in_active;
        }

        float Intensity() const noexcept {
            return intensity_;
        }

        void Intensity(const float & in_intensity) noexcept {
            intensity_ = in_intensity;
        }

        glm::vec4 Color() const noexcept {
            return color_;
        }

        void Color(const glm::vec4 & in_color) noexcept {
            color_ = in_color;
        }

        /*

        WPROPERTY(glm::vec4, color, {0.5, 0.5, 0.5, 1.0}, ...)
        // glm::vec4 color;

        property<glm::vec4> { T value; } color;

        struct TagColor{};
        static PropertyMeta<TagColor> _color_meta {};

        inline glm::vec4 Get_color() { return Properties.color; }

        inline glm::vec4 Set_color(const ptype & in_val) { Properties.color=in_val; }

        inline void * Get_property(std::string_view name);

        inline void * Set_property(std::string_view name, void * in_value);

        void IterProperties(prop_visitor); // wobject, prop_name, prop_type (strig_view), prop_value;

        inline std::string_view PropertyType(std::string_view);

        properties name

        properties type
        */
        
    private:

        bool active_{true};
        float intensity_{1.f};
        glm::vec4 color_{0.5,0.5,0.5,1.f};
        
    };

}
