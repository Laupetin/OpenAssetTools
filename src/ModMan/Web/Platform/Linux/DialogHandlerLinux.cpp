#include "DialogHandlerLinux.h"

#ifdef __linux__

#include <gtk/gtk.h>

using namespace PLATFORM_NAMESPACE_LINUX;

namespace
{
    bool InitGtk()
    {
#if GTK_MAJOR_VERSION >= 4
        return gtk_init_check();
#else
        return gtk_init_check(nullptr, nullptr);
#endif
    }

    void OpenFileDialog()
    {
#ifdef GDK_AVAILABLE_IN_4_10
        auto* dialog = gtk_file_dialog_new();

        gtk_file_dialog_open(dialog, nullptr, nullptr, [](GObject      *source,
               GAsyncResult *result,
               gpointer      user_data
            ) -> void {

               }, nullptr
            );

        g_object_unref(dialog);
#endif
    }

    bool SetFilters(void* pFileOpen, const std::vector<FileDialogFilter>& filters)
    {
        if (filters.empty())
            return true;

        return false;
    }

    std::optional<std::string> ShowFileDialog()
    {
        std::optional<std::string> result = std::nullopt;

        return result;
    }
} // namespace

namespace PLATFORM_NAMESPACE_LINUX
{
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

    OpenFileDialog::OpenFileDialog() = default;

    std::optional<std::string> OpenFileDialog::Open() const
    {
        if (!InitGtk())
            return std::nullopt;

        return std::nullopt;
    }

    SaveFileDialog::SaveFileDialog() = default;

    std::optional<std::string> SaveFileDialog::Open() const
    {
        return std::nullopt;
    }

    FolderSelectDialog::FolderSelectDialog() = default;

    std::optional<std::string> FolderSelectDialog::Open() const
    {
        return std::nullopt;
    }
} // namespace PLATFORM_NAMESPACE_LINUX
#endif
