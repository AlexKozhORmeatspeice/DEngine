#pragma once
#include <cstdint>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

namespace DEngine
{
    class FileWatcher
    {
    public:
        using FileChangeCallback = std::function<void(const std::filesystem::path&)>;

        static constexpr uint32_t MaxFilesPerUpdate = 20;

        FileWatcher() = default;
        ~FileWatcher() = default;

        void AddWatchPath(const std::filesystem::path& path, FileChangeCallback callback);
        void RemoveWatchPath(const std::filesystem::path& path);
        void SetLastWriteTime(const std::filesystem::path& path, std::filesystem::file_time_type time);
        void Update();

    private:
        struct WatchInfo
        {
            std::filesystem::path Path;
            std::filesystem::file_time_type LastWriteTime;
            FileChangeCallback Callback;
        };

        std::vector<WatchInfo> m_WatchedFiles;
        std::unordered_map<std::filesystem::path, uint32_t> m_Index;
        uint32_t m_NextCheck = 0;
        std::filesystem::path m_BasePath;
    };
}
