#include "../include/NetworkManager.hpp"

NetworkManager::NetworkManager() 
    : m_status(NetworkStatus::Disconnected), m_running(false) {
    // Set socket to non-blocking mode
    m_socket.setBlocking(false);
}

NetworkManager::~NetworkManager() {
    // Ensure threads are stopped
    disconnect();
    stopHosting();
}

bool NetworkManager::hostGame(unsigned short port) {
    // Make sure we're not already connected
    if (m_status != NetworkStatus::Disconnected) {
        return false;
    }
    
    // Set up the listener
    if (m_listener.listen(port) != sf::Socket::Status::Done) {
        updateStatus(NetworkStatus::Disconnected, "Failed to start listener");
        return false;
    }
    
    // Start hosting
    updateStatus(NetworkStatus::Hosting, "Waiting for opponent to connect...");
    m_running = true;
    
    // Start listening thread
    m_listenThread = std::thread(&NetworkManager::listenForConnections, this);
    
    return true;
}

void NetworkManager::stopHosting() {
    m_running = false;
    m_listener.close();
    
    // Join listener thread if it's running
    if (m_listenThread.joinable()) {
        m_listenThread.join();
    }
    
    // Disconnect if connected
    disconnect();
}

bool NetworkManager::connectToGame(const std::string& ip, unsigned short port) {
    // Make sure we're not already connected
    if (m_status != NetworkStatus::Disconnected) {
        return false;
    }
    
    updateStatus(NetworkStatus::Connecting, "Connecting to host...");
    
    // Try to connect
    auto ipAddressOpt = sf::IpAddress::resolve(ip);
    if (!ipAddressOpt) {
        updateStatus(NetworkStatus::Disconnected, "Failed to resolve host IP");
        return false;
    }
    
    sf::Socket::Status status = m_socket.connect(*ipAddressOpt, port, sf::seconds(5));
    if (status != sf::Socket::Status::Done) {
        updateStatus(NetworkStatus::Disconnected, "Failed to connect to host");
        return false;
    }
    
    // Connected successfully
    updateStatus(NetworkStatus::Connected, "Connected to host");
    m_running = true;
    
    // Start receive thread
    m_receiveThread = std::thread(&NetworkManager::receiveData, this);
    
    return true;
}

void NetworkManager::disconnect() {
    m_running = false;
    m_socket.disconnect();
    
    // Join receive thread if it's running
    if (m_receiveThread.joinable()) {
        m_receiveThread.join();
    }
    
    updateStatus(NetworkStatus::Disconnected);
    
    // Clear any remaining moves
    std::lock_guard<std::mutex> lock(m_movesMutex);
    while (!m_receivedMoves.empty()) {
        m_receivedMoves.pop();
    }
}

bool NetworkManager::sendMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (m_status != NetworkStatus::Connected) {
        return false;
    }
    
    // Package the move data
    sf::Packet packet;
    packet << fromRow << fromCol << toRow << toCol;
    
    // Send the data
    if (m_socket.send(packet) != sf::Socket::Status::Done) {
        updateStatus(NetworkStatus::Disconnected, "Failed to send move");
        return false;
    }
    
    return true;
}

bool NetworkManager::hasReceivedMove() {
    std::lock_guard<std::mutex> lock(m_movesMutex);
    return !m_receivedMoves.empty();
}

NetworkMove NetworkManager::getReceivedMove() {
    std::lock_guard<std::mutex> lock(m_movesMutex);
    if (m_receivedMoves.empty()) {
        // Return invalid move if queue is empty
        return {-1, -1, -1, -1};
    }
    
    NetworkMove move = m_receivedMoves.front();
    m_receivedMoves.pop();
    return move;
}

NetworkStatus NetworkManager::getStatus() const {
    return m_status;
}

std::string NetworkManager::getStatusText() const {
    return m_statusMessage;
}

std::string NetworkManager::getLocalIpAddress() const {
    // Get the local IP address
    auto localIpOpt = sf::IpAddress::getLocalAddress();
    if (localIpOpt) {
        return localIpOpt->toString();
    }
    return "Unknown";
}

// Private methods
void NetworkManager::listenForConnections() {
    // Set listener to non-blocking mode
    m_listener.setBlocking(false);
    
    while (m_running && m_status == NetworkStatus::Hosting) {
        // Check for new connections
        if (m_listener.accept(m_socket) == sf::Socket::Status::Done) {
            // Client connected!
            updateStatus(NetworkStatus::Connected, "Opponent connected!");
            
            // Stop listening for new connections
            m_listener.close();
            
            // Start receive thread
            m_receiveThread = std::thread(&NetworkManager::receiveData, this);
            
            break;
        }
        
        // Sleep to avoid maxing out CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void NetworkManager::receiveData() {
    // Set socket to non-blocking mode
    m_socket.setBlocking(false);
    
    while (m_running && m_status == NetworkStatus::Connected) {
        // Receive packets
        sf::Packet packet;
        sf::Socket::Status status = m_socket.receive(packet);
        
        if (status == sf::Socket::Status::Done) {
            // Process received packet
            int fromRow, fromCol, toRow, toCol;
            if (packet >> fromRow >> fromCol >> toRow >> toCol) {
                // Add to move queue
                std::lock_guard<std::mutex> lock(m_movesMutex);
                m_receivedMoves.push({fromRow, fromCol, toRow, toCol});
            }
        } 
        else if (status == sf::Socket::Status::Disconnected) {
            // Connection lost
            updateStatus(NetworkStatus::Disconnected, "Opponent disconnected");
            break;
        }
        
        // Sleep to avoid maxing out CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void NetworkManager::updateStatus(NetworkStatus status, const std::string& message) {
    m_status = status;
    m_statusMessage = message;
} 