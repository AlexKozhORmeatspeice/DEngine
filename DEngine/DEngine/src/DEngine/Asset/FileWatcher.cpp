#include "dpch.h"
#include "FileWatcher.h"
#include "DEngine/Core.h"

namespace DEngine
{
    void FileWatcher::AddWatchPath(const std::filesystem::path& path, FileChangeCallback callback)
    {
        if (!std::filesystem::exists(path))
        {
            D_CORE_WARN("FileWatcher: File {0} doesn't exist", path.string());
            return;
        }
        
        auto lastWrite = std::filesystem::last_write_time(path);
        m_WatchedFiles[path] = { lastWrite, callback };
    }
    
    void FileWatcher::RemoveWatchPath(const std::filesystem::path& path)
    {
        m_WatchedFiles.erase(path);
    }
    
    void FileWatcher::Update()
    {
        for (auto& [path, info] : m_WatchedFiles)
        {
            if (!std::filesystem::exists(path))
                continue;
                
            auto currentWrite = std::filesystem::last_write_time(path);
            if (currentWrite != info.LastWriteTime)
            {
                info.LastWriteTime = currentWrite;
                if (info.Callback)
                    info.Callback(path);
            }
        }
    }
}