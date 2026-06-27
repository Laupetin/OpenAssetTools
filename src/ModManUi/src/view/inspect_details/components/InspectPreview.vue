<script setup lang="ts">
import XModelPreview from "@/components/assets/xmodel/XModelPreview.vue";
import type { AssetDto } from "@/native/AssetBinds.ts";
import ImagePreview from "@/components/assets/image/ImagePreview.vue";

defineProps<{
  asset?: AssetDto;
  zoneName: string;
}>();
</script>

<template>
  <div class="preview">
    <ImagePreview v-if="asset?.type === 'image'" :asset :zone-name />
    <XModelPreview v-else-if="asset?.type === 'xmodel'" :asset :zone-name />
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
