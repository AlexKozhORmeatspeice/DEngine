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

        const auto lastWrite = std::filesystem::last_write_time(path);
        const auto existing = m_Index.find(path);
        if (existing != m_Index.end())
        {
            WatchInfo& info = m_WatchedFiles[existing->second];
            info.LastWriteTime = lastWrite;
            info.Callback = std::move(callback);
            return;
        }

        m_Index.emplace(path, static_cast<uint32_t>(m_WatchedFiles.size()));
        m_WatchedFiles.push_back({ path, lastWrite, std::move(callback) });
    }

    void FileWatcher::RemoveWatchPath(const std::filesystem::path& path)
    {
        const auto existing = m_Index.find(path);
        if (existing == m_Index.end())
            return;

        const uint32_t index = existing->second;
        const uint32_t last = static_cast<uint32_t>(m_WatchedFiles.size() - 1);
        if (index != last)
        {
            m_WatchedFiles[index] = std::move(m_WatchedFiles[last]);
            m_Index[m_WatchedFiles[index].Path] = index;
        }

        m_WatchedFiles.pop_back();
        m_Index.erase(existing);
        if (m_NextCheck >= m_WatchedFiles.size())
            m_NextCheck = 0;
    }

    void FileWatcher::SetLastWriteTime(const std::filesystem::path& path, std::filesystem::file_time_type time)
    {
        const auto existing = m_Index.find(path);
        if (existing == m_Index.end())
            return;

        m_WatchedFiles[existing->second].LastWriteTime = time;
    }

    void FileWatcher::Update()
    {
        if (m_WatchedFiles.empty())
            return;

        const uint32_t checks = std::min(MaxFilesPerUpdate, static_cast<uint32_t>(m_WatchedFiles.size()));
        for (uint32_t n = 0; n < checks; ++n)
        {
            if (m_WatchedFiles.empty())
                return;
            if (m_NextCheck >= m_WatchedFiles.size())
                m_NextCheck = 0;

            const uint32_t index = m_NextCheck++;
            const std::filesystem::path path = m_WatchedFiles[index].Path;
            const auto lastWrite = m_WatchedFiles[index].LastWriteTime;
            FileChangeCallback callback = m_WatchedFiles[index].Callback;

            if (!std::filesystem::exists(path))
                continue;

            std::filesystem::file_time_type currentWrite;
            try
            {
                currentWrite = std::filesystem::last_write_time(path);
            }
            catch (const std::filesystem::filesystem_error&)
            {
                continue;
            }

            if (currentWrite <= lastWrite)
                continue;

            const auto stillWatched = m_Index.find(path);
            if (stillWatched != m_Index.end())
                m_WatchedFiles[stillWatched->second].LastWriteTime = currentWrite;

            if (callback)
                callback(path);
        }
    }
}
