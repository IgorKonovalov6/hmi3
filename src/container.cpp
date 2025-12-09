#include "hmi3/container.hpp"
#include <algorithm>

namespace hmi3 {

Container::Container(std::string id) 
    : Component(std::move(id))
    , m_size(800.0f, 600.0f)
    , m_backgroundColor(sf::Color::Transparent) {
}

void Container::update(float dt) {
    for (auto& component : m_components) {
        if (component->isVisible()) {
            component->update(dt);
        }
    }
}

void Container::handleEvent(const sf::Event& event) {
    for (auto it = m_components.rbegin(); it != m_components.rend(); ++it) {
        if ((*it)->isVisible()) {
            (*it)->handleEvent(event);
        }
    }
}

void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_backgroundColor != sf::Color::Transparent) {
        sf::RectangleShape background(m_size);
        background.setFillColor(m_backgroundColor);
        target.draw(background, states);
    }

    for (const auto& component : m_components) {
        if (component->isVisible()) {
            // Компоненты должны сами наследоваться
            target.draw(*component, states);
        }
    }
}

void Container::addComponent(std::shared_ptr<Component> component) {
    if (!component) return;
    
    auto it = m_componentMap.find(component->getId());
    if (it == m_componentMap.end()) {
        m_components.push_back(component);
        m_componentMap[component->getId()] = component;
    }
}

bool Container::removeComponent(const std::string& id) {
    auto it = m_componentMap.find(id);
    if (it != m_componentMap.end()) {
        auto component = it->second;
        m_components.erase(
            std::remove(m_components.begin(), m_components.end(), component),
            m_components.end()
        );
        m_componentMap.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<Component> Container::getComponent(const std::string& id) const {
    auto it = m_componentMap.find(id);
    return it != m_componentMap.end() ? it->second : nullptr;
}

void Container::clear() {
    m_components.clear();
    m_componentMap.clear();
}

} // namespace hmi3