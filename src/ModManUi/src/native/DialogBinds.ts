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
  openFileDialog(options?: OpenFileDialogDto): Promise<string | null>;
  saveFileDialog(options?: SaveFileDialogDto): Promise<string | null>;
  folderSelectDialog(): Promise<string | null>;
}
