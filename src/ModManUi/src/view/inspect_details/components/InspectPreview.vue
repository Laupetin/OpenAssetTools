<script setup lang="ts">
import XModelPreview from "@/components/assets/xmodel/XModelPreview.vue";
import type { AssetDto } from "@/native/AssetBinds.ts";

defineProps<{
  asset?: AssetDto;
}>();
</script>

<template>
  <div class="preview">
    <XModelPreview v-if="asset?.type === 'xmodel'" :asset />
    <span v-else>No preview available</span>
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
