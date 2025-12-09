#ifndef HMI3_COMPONENT_HPP
#define HMI3_COMPONENT_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace hmi3 {

class Component : public sf::Drawable {
public:
    virtual ~Component() = default;
    virtual void update(float dt) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
    
    void setPosition(const sf::Vector2f& position) { m_position = position; }
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    const sf::Vector2f& getPosition() const { return m_position; }
    const std::string& getId() const { return m_id; }

protected:
    explicit Component(std::string id) : m_id(std::move(id)) {}
    
    sf::Vector2f m_position;
    bool m_visible = true;
    std::string m_id;
};

} // namespace hmi3

#endif // HMI3_COMPONENT_HPP