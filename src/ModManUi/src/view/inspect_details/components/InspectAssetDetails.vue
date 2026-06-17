<script setup lang="ts">
import type { AssetDto } from "@/native/AssetBinds.ts";
import Tag from "primevue/tag";
import { computed } from "vue";
import { localizeAssetType } from "@/i18n/i18n.ts";

const props = defineProps<{
  selectedAsset?: AssetDto;
}>();

const assetTypeName = computed(() =>
  props.selectedAsset ? localizeAssetType(props.selectedAsset.type) : "",
);
</script>

<template>
  <div v-if="selectedAsset">
    <h2 class="asset-header">
      <span>{{ selectedAsset.name }}</span>
      <Tag class="asset-type" :value="assetTypeName" />
    </h2>
    <div>No details available</div>
  </div>
  <div v-else>
    <div>No asset selected</div>
  </div>
</template>

<style scoped lang="scss">
.asset-header {
  display: flex;
  align-items: center;
  margin: 0 0 0.5em 0;
}

.asset-type {
  margin-left: 0.5em;
}
</style>
