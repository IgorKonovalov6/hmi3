#include "hmi3/command_receiver.hpp"
#include <SFML/Network.hpp>
#include <thread>
#include <iostream>
#include <cmath>

namespace hmi3 {

NetworkCommandReceiver::NetworkCommandReceiver(unsigned short port) 
    : m_port(port)
    , m_receiveTimeout(5000)
    , m_running(false) {
}

NetworkCommandReceiver::~NetworkCommandReceiver() {
    stop();
}

bool NetworkCommandReceiver::start() {
    if (m_running) return true;

    m_running = true;
    m_receiveThread = std::thread(&NetworkCommandReceiver::receiveLoop, this);
    return true;
}

void NetworkCommandReceiver::stop() {
    m_running = false;
    if (m_receiveThread.joinable()) {
        m_receiveThread.join();
    }
}

bool NetworkCommandReceiver::isRunning() const {
    return m_running;
}

std::vector<ProjectLoadCommand> NetworkCommandReceiver::getCommands() {
    std::lock_guard<std::mutex> lock(m_commandsMutex);
    auto commands = std::move(m_commands);
    m_commands.clear();
    return commands;
}

void NetworkCommandReceiver::receiveLoop() {
    sf::TcpListener listener;
    
    if (listener.listen(m_port) != sf::Socket::Status::Done) {
        std::cerr << "Failed to bind to port " << m_port << std::endl;
        m_running = false;
        return;
    }

    std::cout << "Command receiver listening on port " << m_port << std::endl;

    while (m_running) {
        sf::TcpSocket socket;
        if (listener.accept(socket) == sf::Socket::Status::Done) {
            auto remoteAddr = socket.getRemoteAddress();
            if (remoteAddr.has_value()) {
                std::cout << "Client connected from " << remoteAddr.value().toString() << std::endl;
            } else {
                std::cout << "Client connected from unknown address" << std::endl;
            }

            char buffer[1024];
            std::size_t received;
            std::string data;

            // Получаем данные
            while (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Status::Done) {
                data.append(buffer, received);
            }

            if (!data.empty()) {
                ProjectLoadCommand command;
                command.projectName = "ReceivedProject";
                command.projectData = std::move(data);
                command.version = 1;
                command.forceLoad = false;

                {
                    std::lock_guard<std::mutex> lock(m_commandsMutex);
                    m_commands.push_back(command);
                }

                if (m_commandCallback) {
                    m_commandCallback(command);
                }

                std::cout << "Received project load command, data size: " 
                          << command.projectData.size() << " bytes" << std::endl;
            }

            socket.disconnect();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    listener.close();
}

} // namespace hmi3