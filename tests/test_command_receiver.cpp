#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include <vector>
#include <string>
#include "hmi3/command_receiver.hpp"

// Mock command receiver для тестов
class MockCommandReceiver : public hmi3::AbstractCommandReceiver {
public:
    bool start() override {
        m_running = true;
        return true;
    }
    
    void stop() override {
        m_running = false;
    }
    
    bool isRunning() const override {
        return m_running;
    }
    
    std::vector<hmi3::ProjectLoadCommand> getCommands() override {
        std::lock_guard<std::mutex> lock(m_commandsMutex);
        std::vector<hmi3::ProjectLoadCommand> commands;
        commands.swap(m_commands);
        return commands;
    }
    
    void simulateCommand(const hmi3::ProjectLoadCommand& command) {
        std::lock_guard<std::mutex> lock(m_commandsMutex);
        m_commands.push_back(command);
        
        if (m_commandCallback) {
            m_commandCallback(command);
        }
    }

private:
    bool m_running = false;
    std::vector<hmi3::ProjectLoadCommand> m_commands;
    mutable std::mutex m_commandsMutex;
};

class CommandReceiverTest : public ::testing::Test {
protected:
    void SetUp() override {
        receiver = std::make_unique<MockCommandReceiver>();
    }

    void TearDown() override {
        if (receiver) {
            receiver->stop();
        }
    }

    std::unique_ptr<MockCommandReceiver> receiver;
};

TEST_F(CommandReceiverTest, StartStop) {
    EXPECT_FALSE(receiver->isRunning());
    
    EXPECT_TRUE(receiver->start());
    EXPECT_TRUE(receiver->isRunning());
    
    receiver->stop();
    EXPECT_FALSE(receiver->isRunning());
}

TEST_F(CommandReceiverTest, SimulateCommand) {
    receiver->start();
    
    hmi3::ProjectLoadCommand cmd;
    cmd.projectName = "TestProject";
    cmd.projectData = "TestData";
    cmd.version = 1;
    
    bool callbackCalled = false;
    receiver->setCommandCallback([&](const hmi3::ProjectLoadCommand& receivedCmd) {
        callbackCalled = true;
        EXPECT_EQ(receivedCmd.projectName, "TestProject");
    });
    
    receiver->simulateCommand(cmd);
    
    EXPECT_TRUE(callbackCalled);
    
    auto commands = receiver->getCommands();
    EXPECT_EQ(commands.size(), 1);
    EXPECT_EQ(commands[0].projectName, "TestProject");
    
    receiver->stop();
}