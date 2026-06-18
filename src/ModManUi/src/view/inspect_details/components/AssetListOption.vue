<script setup lang="ts">
import type { AssetDto } from "@/native/AssetBinds.ts";
import { localizeAssetType } from "@/i18n/i18n.ts";

defineProps<{
  asset: AssetDto;
}>();
</script>

<template>
  <div class="asset-option">
    <span class="asset-type">{{ localizeAssetType(asset.type) }}</span>
    <span class="asset-name">{{ asset.name }}</span>
  </div>
</template>

<style scoped lang="scss">
@use "@style/colors";

.asset-option {
  font-size: 0.95rem;
  max-width: 100%;
  overflow-x: clip;
  white-space: nowrap;
  padding: 0 0.2em;
}

.asset-type {
  display: inline-block;
  color: colors.$color-text-muted;
  min-width: 8em;
}

.asset-name {
  margin-left: 0.5em;
}
</style>
