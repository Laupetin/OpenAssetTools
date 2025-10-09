#include "DialogHandler.h"

namespace ui
{
    DialogWithCallback::DialogWithCallback() = default;

    void DialogWithCallback::SetCallback(callback_t callback)
    {
        m_callback = std::move(callback);
    }

    FileDialogFilter::FileDialogFilter(std::string name, std::string filter)
        : m_name(std::move(name)),
          m_filter(std::move(filter))
    {
    }

    FileDialog::FileDialog() = default;

    void FileDialog::AddFilter(std::string name, std::string filter)
    {
        m_filters.emplace_back(std::move(name), std::move(filter));
    }
} // namespace ui
