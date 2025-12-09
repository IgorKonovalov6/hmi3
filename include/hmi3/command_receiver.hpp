#ifndef HMI3_COMMAND_RECEIVER_HPP
#define HMI3_COMMAND_RECEIVER_HPP

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>
#include <thread>

namespace hmi3 {

struct ProjectLoadCommand {
    std::string projectName;
    std::string projectData;
    std::string checksum;
    uint32_t version;
    bool forceLoad;
    
    ProjectLoadCommand() : version(1), forceLoad(false) {}
};

class AbstractCommandReceiver {
public:
    virtual ~AbstractCommandReceiver() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual std::vector<ProjectLoadCommand> getCommands() = 0;

    void setCommandCallback(std::function<void(const ProjectLoadCommand&)> callback) {
        m_commandCallback = std::move(callback);
    }

    std::function<void(const ProjectLoadCommand&)> getCommandCallback() const {
        return m_commandCallback;
    }

protected:
    std::function<void(const ProjectLoadCommand&)> m_commandCallback;
};

class NetworkCommandReceiver : public AbstractCommandReceiver {
public:
    explicit NetworkCommandReceiver(unsigned short port = 8080);
    ~NetworkCommandReceiver();

    bool start() override;
    void stop() override;
    bool isRunning() const override;
    std::vector<ProjectLoadCommand> getCommands() override;
    void setReceiveTimeout(int timeout) { m_receiveTimeout = timeout; }

private:
    void receiveLoop();

    unsigned short m_port;
    int m_receiveTimeout;
    bool m_running;
    std::vector<ProjectLoadCommand> m_commands;
    mutable std::mutex m_commandsMutex;
    std::thread m_receiveThread;
};

} // namespace hmi3

#endif // HMI3_COMMAND_RECEIVER_HPP