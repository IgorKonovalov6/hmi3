#ifndef HMI3_CONTAINER_HPP
#define HMI3_CONTAINER_HPP

#include "components/component.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace hmi3 {

class Container : public Component {
public:
    explicit Container(std::string id = "container");
    virtual ~Container() = default;

    void update(float dt) override;
    void handleEvent(const sf::Event& event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;
    
    void addComponent(std::shared_ptr<Component> component);
    bool removeComponent(const std::string& id);
    std::shared_ptr<Component> getComponent(const std::string& id) const;
    void setSize(const sf::Vector2f& size) { m_size = size; }
    void setBackgroundColor(const sf::Color& color) { m_backgroundColor = color; }
    void clear();
    size_t getComponentCount() const { return m_components.size(); }

private:
    sf::Vector2f m_size;
    sf::Color m_backgroundColor;
    std::vector<std::shared_ptr<Component>> m_components;
    std::unordered_map<std::string, std::shared_ptr<Component>> m_componentMap;
};

} // namespace hmi3

#endif // HMI3_CONTAINER_HPP