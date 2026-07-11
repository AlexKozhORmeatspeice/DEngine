#pragma once
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace DEngine
{
    class FileWatcher
    {
    public:
        using FileChangeCallback = std::function<void(const std::filesystem::path&)>;
        
        FileWatcher() = default;
        ~FileWatcher() = default;
        
        void AddWatchPath(const std::filesystem::path& path, FileChangeCallback callback);
        void RemoveWatchPath(const std::filesystem::path& path);
        void Update(); // Call this each frame to check for changes
        
    private:
        struct WatchInfo
        {
            std::filesystem::file_time_type LastWriteTime;
            FileChangeCallback Callback;
        };
        
        std::unordered_map<std::filesystem::path, WatchInfo> m_WatchedFiles;
        std::filesystem::path m_BasePath;
    };
}
