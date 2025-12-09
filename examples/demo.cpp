#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <functional>
#include <cmath>
#include "hmi3/container.hpp"
#include "hmi3/command_receiver.hpp"

// Простой компонент для демонстрации работы контейнера
class DemoComponent : public hmi3::Component {
public:
    DemoComponent(const std::string& id, const sf::Vector2f& size, sf::Color color) 
        : hmi3::Component(id), m_shape(size), m_originalColor(color) {
        m_shape.setFillColor(color);
        m_shape.setOutlineColor(sf::Color::White);
        m_shape.setOutlineThickness(2.0f);
    }
    
    void update(float dt) override {
        // Упрощенная анимация - только пульсация прозрачности
        static float time = 0;
        time += dt;
        
        if (!m_clicked) {
            // Простая пульсация прозрачности
            uint8_t alpha = 150 + static_cast<uint8_t>(100 * std::sin(time));
            m_shape.setFillColor(sf::Color(m_originalColor.r, m_originalColor.g, m_originalColor.b, alpha));
        }
    }
    
    void handleEvent(const sf::Event& event) override {
        if (!isVisible()) return;

        // используем getIf для проверки типа события
        if (auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x), 
                                     static_cast<float>(mousePressed->position.y));
                
                sf::FloatRect bounds(getPosition(), m_shape.getSize());
                if (bounds.contains(mousePos)) {
                    // меняем цвет при клике
                    m_shape.setFillColor(sf::Color::Yellow);
                    m_clicked = true;
                    std::cout << "Component" << getId() << "clicked!" << std::endl;
                    
                    // Вызываем callback
                    if (m_callback) {
                        m_callback();
                    }
                }
            }
        }
        else if (auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
            m_clicked = false;
            m_shape.setFillColor(m_originalColor);
        }
    }
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (!isVisible()) return;
        
        states.transform.translate(getPosition());
        target.draw(m_shape, states);
    }
    
    void setCallback(std::function<void()> callback) {
        m_callback = std::move(callback);
    }

private:
    sf::RectangleShape m_shape;
    sf::Color m_originalColor;
    bool m_clicked = false;
    std::function<void()> m_callback;
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "HMI3 - Container & Command System Demo");
    
    // Демонстрация контейнера
    
    // Создаем контейнер с видимым фоном
    auto container = std::make_shared<hmi3::Container>("main_container");
    container->setSize(sf::Vector2f(800, 600));
    container->setBackgroundColor(sf::Color(40, 40, 80));
    
    // Добавляем компоненты в контейнер
    auto redComp = std::make_shared<DemoComponent>("red_component", sf::Vector2f(100, 50), sf::Color::Red);
    redComp->setPosition({50, 50});
    
    auto greenComp = std::make_shared<DemoComponent>("green_component", sf::Vector2f(100, 50), sf::Color::Green);
    greenComp->setPosition({200, 50});
    
    auto blueComp = std::make_shared<DemoComponent>("blue_component", sf::Vector2f(100, 50), sf::Color::Blue);
    blueComp->setPosition({350, 50});
    
    container->addComponent(redComp);
    container->addComponent(greenComp);
    container->addComponent(blueComp);
    
    std::cout << "Container Demo Started!" << std::endl;
    std::cout << "Container has " << container->getComponentCount() << " components" << std::endl;
    std::cout << "Click on colored components to see interaction" << std::endl;
    
    // Демонстрация системы команд
    
    hmi3::NetworkCommandReceiver receiver(8080);
    
    // Callback для визуального отображения команд
    receiver.setCommandCallback([container](const hmi3::ProjectLoadCommand& cmd) {
        std::cout << "📨 Received command: " << cmd.projectName << std::endl;
        std::cout << "   Data size: " << cmd.projectData.size() << " bytes" << std::endl;
        
        // Визуальная реакция на команду - меняем цвет контейнера
        static int colorIndex = 0;
        sf::Color colors[] = {
            sf::Color(40, 40, 80),   // исходный
            sf::Color(80, 40, 40),   // красноватый
            sf::Color(40, 80, 40),   // зеленоватый
            sf::Color(40, 40, 120)   // синеватый
        };
        container->setBackgroundColor(colors[colorIndex % 4]);
        colorIndex++;
        
        std::cout << "Container background color changed!" << std::endl;
    });
    
    if (receiver.start()) {
        std::cout << "Command receiver started on port 8080" << std::endl;
        std::cout << "Send test command: echo 'Hello HMI3!' | nc localhost 8080" << std::endl;
        std::cout << "Or click the 'Test Command' button below" << std::endl;
    }
    
    // Тестовая кнопка для симуляции команд
    auto testButton = std::make_shared<DemoComponent>("test_button", sf::Vector2f(150, 40), sf::Color::Magenta);
    testButton->setPosition({50, 150});
    testButton->setCallback([&receiver]() {
        std::cout << "Simulating command via test button..." << std::endl;
        
        // Создаем тестовую команду
        hmi3::ProjectLoadCommand cmd;
        cmd.projectName = "TestProject";
        cmd.projectData = "Simulated command data from test button";
        cmd.version = 1;
        cmd.forceLoad = true;
        
        // Используем callback напрямую для демонстрации
        if (auto callback = receiver.getCommandCallback()) {
            callback(cmd);
        }
    });
    container->addComponent(testButton);
    
    // Главный цикл
    
    sf::Clock clock;
    sf::Clock removeTimer;
    bool componentRemoved = false;
    
    while (window.isOpen()) {
        // Обработка событий
        for (auto event = window.pollEvent(); event.has_value(); event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // Контейнер обрабатывает события для всех дочерних компонентов
            container->handleEvent(*event);
        }
        
        float dt = clock.restart().asSeconds();
        
        // Демонстрация динамического управления контейнером
        if (!componentRemoved && removeTimer.getElapsedTime().asSeconds() > 8.0f) {
            if (container->removeComponent("green_component")) {
                std::cout << "Component 'green_component' automatically removed after 8 seconds!" << std::endl;
                std::cout << "Container now has " << container->getComponentCount() << " components" << std::endl;
                componentRemoved = true;
            }
        }
        
        // Обновление контейнера и всех компонентов
        container->update(dt);
        
        // Отрисовка
        window.clear(sf::Color(20, 20, 20));
        window.draw(*container);
        window.display();
    }
    
    // Остановка системы команд
    receiver.stop();
    std::cout << "Demo finished!" << std::endl;
    
    return 0;
}