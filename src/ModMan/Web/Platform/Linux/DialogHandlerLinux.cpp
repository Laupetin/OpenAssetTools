#include "Web/Platform/DialogHandler.h"
#include "Web/Platform/Platform.h"

#ifdef PLATFORM_LINUX

#include <cassert>
#include <cstdint>
#include <gtk/gtk.h>

namespace
{
    enum class DialogOperation : std::uint8_t
    {
        OPEN_FILE,
        SAVE_FILE,
        SELECT_FOLDER
    };

    class DialogData
    {
    public:
        DialogData(DialogOperation operation, ui::DialogWithCallback::callback_t callback)
            : m_operation(operation),
              m_callback(std::move(callback))
        {
        }

        DialogOperation m_operation;
        ui::DialogWithCallback::callback_t m_callback;
    };

    bool InitGtk()
    {
#if GTK_MAJOR_VERSION >= 4
        return gtk_init_check();
#else
        return gtk_init_check(nullptr, nullptr);
#endif
    }

    void SetFilters(GtkFileDialog* pDialog, const std::vector<ui::FileDialogFilter>& filters)
    {
        if (filters.empty())
            return;

        auto* listStore = g_list_store_new(GTK_TYPE_FILE_FILTER);

        for (auto& filter : filters)
        {
            auto* fileFilter = gtk_file_filter_new();
            gtk_file_filter_set_name(fileFilter, filter.m_name.c_str());
            gtk_file_filter_add_pattern(fileFilter, filter.m_filter.c_str());
            g_list_store_append(listStore, fileFilter);
            g_object_unref(fileFilter);
        }

        auto* wildcardFilter = gtk_file_filter_new();
        gtk_file_filter_set_name(wildcardFilter, "All files");
        gtk_file_filter_add_pattern(wildcardFilter, "*.*");
        g_list_store_append(listStore, wildcardFilter);
        g_object_unref(wildcardFilter);

        gtk_file_dialog_set_filters(pDialog, G_LIST_MODEL(listStore));
        g_object_unref(listStore);
    }

    void OnDialogResult(GObject* source, GAsyncResult* asyncResult, gpointer userData)
    {
        auto* dialogData = reinterpret_cast<DialogData*>(userData);

        GError* error = nullptr;
        GFile* file = nullptr;

        if (dialogData->m_operation == DialogOperation::OPEN_FILE)
        {
            file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), asyncResult, &error);
        }
        else if (dialogData->m_operation == DialogOperation::SAVE_FILE)
        {
            file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), asyncResult, &error);
        }
        else if (dialogData->m_operation == DialogOperation::SELECT_FOLDER)
        {
            file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), asyncResult, &error);
        }
        else
        {
            assert(false);
        }

        std::optional<std::string> result;
        ui::DialogCallbackResultType resultType;
        if (error)
        {
            if (error->code == GTK_DIALOG_ERROR_DISMISSED)
                resultType = ui::DialogCallbackResultType::CANCELLED;
            else
                resultType = ui::DialogCallbackResultType::FAILED;

            g_error_free(error);
        }
        else
        {
            resultType = ui::DialogCallbackResultType::SUCCESSFUL;
            result = std::string(g_file_get_path(file));
        }

        if (file)
        {
            g_object_unref(file);
        }

        dialogData->m_callback(resultType, result);

        delete dialogData;
    }

    std::optional<std::string> ShowFileDialog()
    {
        std::optional<std::string> result = std::nullopt;

        return result;
    }
} // namespace

namespace ui
{
#if GTK_MAJOR_VERSION >= 4
    OpenFileDialog::OpenFileDialog() = default;

    void OpenFileDialog::OpenAsync()
    {
        if (!InitGtk())
            return m_callback(DialogCallbackResultType::FAILED, std::nullopt);

        auto* dialog = gtk_file_dialog_new();

        SetFilters(dialog, m_filters);

        // Move data out of the dialog object since it may be destroyed
        auto* dialogData = new DialogData(DialogOperation::OPEN_FILE, std::move(m_callback));
        gtk_file_dialog_open(dialog, nullptr, nullptr, OnDialogResult, dialogData);

        g_object_unref(dialog);
    }

    SaveFileDialog::SaveFileDialog() = default;

    void SaveFileDialog::OpenAsync()
    {
        if (!InitGtk())
            return m_callback(DialogCallbackResultType::FAILED, std::nullopt);

        auto* dialog = gtk_file_dialog_new();

        SetFilters(dialog, m_filters);

        // Move data out of the dialog object since it may be destroyed
        auto* dialogData = new DialogData(DialogOperation::SAVE_FILE, std::move(m_callback));
        gtk_file_dialog_save(dialog, nullptr, nullptr, OnDialogResult, dialogData);

        g_object_unref(dialog);
    }

    FolderSelectDialog::FolderSelectDialog() = default;

    void FolderSelectDialog::OpenAsync()
    {
        if (!InitGtk())
            return m_callback(DialogCallbackResultType::FAILED, std::nullopt);

        auto* dialog = gtk_file_dialog_new();

        // Move data out of the dialog object since it may be destroyed
        auto* dialogData = new DialogData(DialogOperation::SELECT_FOLDER, std::move(m_callback));
        gtk_file_dialog_select_folder(dialog, nullptr, nullptr, OnDialogResult, dialogData);

        g_object_unref(dialog);
    }
#endif
} // namespace ui
#endif
