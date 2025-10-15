#include "AssetDumpingContext.h"

#include <filesystem>

AssetDumpingContext::AssetDumpingContext(const Zone& zone,
                                         const std::string& basePath,
                                         IOutputPath& outputPath,
                                         ISearchPath& objSearchPath,
                                         std::optional<std::unique_ptr<ProgressCallback>> progressCallback)
    : m_zone(zone),
      m_base_path(basePath),
      m_output_path(outputPath),
      m_obj_search_path(objSearchPath),
      m_current_progress(0uz),
      m_total_progress(0uz)
{
    if (progressCallback)
        m_progress_callback = *std::move(progressCallback);
}

std::unique_ptr<std::ostream> AssetDumpingContext::OpenAssetFile(const std::string& fileName) const
{
    return m_output_path.Open(fileName);
}

void AssetDumpingContext::IncrementProgress()
{
    if (m_progress_callback)
    {
        m_current_progress++;
        m_progress_callback->OnProgress(m_current_progress, m_total_progress);
    }
}

void AssetDumpingContext::SetTotalProgress(const size_t value)
{
    m_total_progress = value;
}

bool AssetDumpingContext::ShouldTrackProgress() const
{
    return static_cast<bool>(m_progress_callback);
}
