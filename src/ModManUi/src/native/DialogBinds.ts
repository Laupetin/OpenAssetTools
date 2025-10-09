export interface FileDialogFilterDto {
  name: string;
  filter: string;
}

export interface OpenFileDialogDto {
  filters?: FileDialogFilterDto[];
}

export interface SaveFileDialogDto {
  filters?: FileDialogFilterDto[];
}

export interface DialogBinds {
  openFileDialog(options?: OpenFileDialogDto): string | null;
  saveFileDialog(options?: SaveFileDialogDto): string | null;
  folderSelectDialog(): string | null;
}
