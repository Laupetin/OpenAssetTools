<script setup lang="ts"></script>

<template>
  <div class="preview">
    <span>No preview available</span>
  </div>
</template>

<style lang="scss" scoped>
@use "@style/colors";
@use "@style/variables";

.preview {
  height: 100%;
  width: 100%;
  display: flex;
  justify-content: center;
  align-items: center;

  background-color: colors.$COLOR_CONTENT_HOVER_BACKGROUND;
  border-radius: variables.$BORDER_RADIUS_MD;
}
</style>
