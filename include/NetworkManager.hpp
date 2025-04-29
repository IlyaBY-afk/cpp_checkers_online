#pragma once

#include <SFML/Network.hpp>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>

// Enum to track network status
enum class NetworkStatus {
    Disconnected,
    Hosting,
    Connecting,
    Connected
};

// Move data structure to be sent over network
struct NetworkMove {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    // Server functions
    bool hostGame(unsigned short port = 50001);
    void stopHosting();
    
    // Client functions
    bool connectToGame(const std::string& ip, unsigned short port = 50001);
    void disconnect();
    
    // Send and receive moves
    bool sendMove(int fromRow, int fromCol, int toRow, int toCol);
    bool hasReceivedMove();
    NetworkMove getReceivedMove();
    
    // Status management
    NetworkStatus getStatus() const;
    std::string getStatusText() const;
    std::string getLocalIpAddress() const;
    
private:
    // Network components
    sf::TcpListener m_listener;
    sf::TcpSocket m_socket;
    std::thread m_listenThread;
    std::thread m_receiveThread;
    
    // Status tracking
    NetworkStatus m_status;
    std::string m_statusMessage;
    std::atomic<bool> m_running;
    
    // Thread-safe move queue
    std::queue<NetworkMove> m_receivedMoves;
    std::mutex m_movesMutex;
    
    // Private methods
    void listenForConnections();
    void receiveData();
    void updateStatus(NetworkStatus status, const std::string& message = "");
}; 