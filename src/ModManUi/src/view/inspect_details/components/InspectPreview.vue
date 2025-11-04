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

  background-color: colors.$color-content-hover-background;
  border-radius: variables.$border-radius-md;
}
</style>
