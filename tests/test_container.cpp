#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "hmi3/components/component.hpp"
#include "hmi3/container.hpp"

// Mock компонент для тестов
class TestComponent : public hmi3::Component {
public:
    TestComponent(std::string id) : Component(std::move(id)), updateCount(0) {}
    
    void update(float dt) override { updateCount++; }
    void handleEvent(const sf::Event& event) override { 
        eventHandled = true;
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {}
    
    int updateCount = 0;
    bool eventHandled = false;
};

class ContainerTest : public ::testing::Test {
protected:
    void SetUp() override {
        container = std::make_unique<hmi3::Container>();
    }

    std::unique_ptr<hmi3::Container> container;
};

TEST_F(ContainerTest, AddComponent) {
    auto component = std::make_shared<TestComponent>("test_component");
    container->addComponent(component);
    
    EXPECT_EQ(container->getComponentCount(), 1);
    EXPECT_EQ(container->getComponent("test_component"), component);
}

TEST_F(ContainerTest, RemoveComponent) {
    auto component = std::make_shared<TestComponent>("test_component");
    container->addComponent(component);
    
    EXPECT_TRUE(container->removeComponent("test_component"));
    EXPECT_EQ(container->getComponentCount(), 0);
    EXPECT_FALSE(container->getComponent("test_component"));
}

TEST_F(ContainerTest, UpdateComponents) {
    auto component1 = std::make_shared<TestComponent>("comp1");
    auto component2 = std::make_shared<TestComponent>("comp2");
    
    container->addComponent(component1);
    container->addComponent(component2);
    
    container->update(0.1f);
    
    EXPECT_EQ(component1->updateCount, 1);
    EXPECT_EQ(component2->updateCount, 1);
}

TEST_F(ContainerTest, ClearComponents) {
    container->addComponent(std::make_shared<TestComponent>("comp1"));
    container->addComponent(std::make_shared<TestComponent>("comp2"));
    
    EXPECT_EQ(container->getComponentCount(), 2);
    
    container->clear();
    
    EXPECT_EQ(container->getComponentCount(), 0);
}